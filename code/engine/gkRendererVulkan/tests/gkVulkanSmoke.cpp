#include "gkVkDeviceContext.h"
#include "ISystem.h"
#include "IInputManager.h"
#include "IMesh.h"

#include <cstdio>
#include <cstring>

#if defined(OS_APPLE)
#include "gkVkMacPlatform.h"
#include <unistd.h>
#endif

SSystemGlobalEnvironment* gEnv = NULL;

#if defined(OS_APPLE)
class MacInputCapture : public IInputManager
{
public:
	MacInputCapture()
		: keyPressed(false), keyReleased(false), mouseMoved(false),
		  mousePressed(false), mouseReleased(false), exclusive(NULL),
		  modifiers(eMM_None), modifiedKeyPressed(false) {}

	virtual HRESULT Init(HWND) { return NULL; }
	virtual void Destroy() {}
	virtual void Update(float) {}
	virtual void setHWND(HWND) {}
	virtual void addInputEventListener(IInputEventListener*) {}
	virtual void removeEventListener(IInputEventListener*) {}
	virtual void setExclusiveListener(IInputEventListener* listener)
	{
		exclusive = listener;
	}
	virtual IInputEventListener* getExclusiveListener() { return exclusive; }
	virtual const char* GetKeyName(const SInputEvent&, bool) { return ""; }
	virtual int GetModifiers() const { return modifiers; }
	virtual void SetModifiers(int value) { modifiers = value; }
	virtual void PostInputEvent(const SInputEvent& event, bool)
	{
		if (event.keyId == eKI_W && event.state == eIS_Pressed)
		{
			keyPressed = true;
			modifiedKeyPressed = (event.modifiers & eMM_LShift) != 0;
		}
		if (event.keyId == eKI_W && event.state == eIS_Released)
			keyReleased = true;
		if ((event.keyId == eKI_MouseX || event.keyId == eKI_MouseY) &&
			event.state == eIS_Changed && event.value != 0.0f)
			mouseMoved = true;
		if (event.keyId == eKI_Mouse1 && event.state == eIS_Pressed)
			mousePressed = true;
		if (event.keyId == eKI_Mouse1 && event.state == eIS_Released)
			mouseReleased = true;
	}

	bool complete() const
	{
		return keyPressed && keyReleased && mouseMoved &&
			mousePressed && mouseReleased && modifiedKeyPressed;
	}

	bool keyPressed;
	bool keyReleased;
	bool mouseMoved;
	bool mousePressed;
	bool mouseReleased;
	IInputEventListener* exclusive;
	int modifiers;
	bool modifiedKeyPressed;
};
#endif

int main(int argc, char** argv)
{
	const bool interactive = argc > 1 && strcmp(argv[1], "--interactive") == 0;
	ISystemInitInfo initInfo;
	initInfo.fWidth = interactive ? 1280 : 640;
	initInfo.fHeight = interactive ? 720 : 360;

#if defined(OS_APPLE)
	MacInputCapture inputCapture;
	SSystemGlobalEnvironment environment = {};
	environment.pInputManager = &inputCapture;
	gEnv = &environment;
#endif

	gkVkDeviceContext context;
	if (!context.initialize(initInfo))
	{
		fprintf(stderr, "gkVulkanSmoke: initialization failed\n");
		return 1;
	}

#if defined(OS_APPLE)
	gkVkMacWindow* macWindow =
		reinterpret_cast<gkVkMacWindow*>(context.window());
	uint32 drawableWidth = 0;
	uint32 drawableHeight = 0;
	gkVkMacDrawableSize(macWindow, &drawableWidth, &drawableHeight);
	const float backingScale = gkVkMacBackingScale(macWindow);
	if (context.width() != drawableWidth || context.height() != drawableHeight ||
		drawableWidth != static_cast<uint32>(initInfo.fWidth) ||
		drawableHeight != static_cast<uint32>(initInfo.fHeight))
	{
		fprintf(stderr,
			"gkVulkanSmoke: macOS logical drawable mismatch "
			"(scale %.2f, drawable %ux%u, swapchain %ux%u)\n",
			backingScale, drawableWidth, drawableHeight,
			context.width(), context.height());
		return 4;
	}
#endif

	gkVertexBuffer vertexBuffer(sizeof(GKVL_P3T2U4), 3, eVI_P3T2U4, eBF_Reserved);
	GKVL_P3T2U4 vertices[3] = {};
	vertices[0].position = Vec3(-0.65f, -0.55f, 0.25f);
	vertices[0].texcoord = Vec2(0.0f, 0.0f);
	vertices[1].position = Vec3(0.65f, -0.55f, 0.25f);
	vertices[1].texcoord = Vec2(1.0f, 0.0f);
	vertices[2].position = Vec3(0.0f, 0.65f, 0.25f);
	vertices[2].texcoord = Vec2(0.5f, 1.0f);
	memcpy(vertexBuffer.data, vertices, sizeof(vertices));
	gkIndexBuffer indexBuffer(3, true);
	indexBuffer.push_back(0);
	indexBuffer.push_back(1);
	indexBuffer.push_back(2);
	gkRenderOperation operation;
	operation.vertexData = &vertexBuffer;
	operation.vertexCount = 3;
	operation.vertexStart = 0;
	operation.vertexSize = sizeof(GKVL_P3T2U4);
	operation.operationType = gkRenderOperation::OT_TRIANGLE_LIST;
	operation.useIndexes = true;
	operation.indexData = &indexBuffer;
	operation.indexCount = 3;
	operation.indexStart = 0;
	Matrix44 identity(IDENTITY);

	bool running = true;
	for (int frame = 0; running && (interactive || frame < 90); ++frame)
	{
#if defined(OS_WIN32)
		MSG message;
		while (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
		{
			if (message.message == WM_QUIT)
				running = false;
			if (message.message == WM_KEYDOWN && message.wParam == VK_ESCAPE)
				PostMessage(context.window(), WM_CLOSE, 0, 0);
			TranslateMessage(&message);
			DispatchMessage(&message);
		}

		if (!interactive && frame == 20)
			SetWindowPos(context.window(), NULL, 0, 0, 800, 500,
				SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
		if (!interactive && frame == 45)
		{
			ShowWindow(context.window(), SW_MINIMIZE);
			for (int i = 0; i < 4; ++i)
			{
				PeekMessage(&message, NULL, 0, 0, PM_REMOVE);
				DispatchMessage(&message);
			}
			ShowWindow(context.window(), SW_RESTORE);
		}
		if (!interactive && frame == 60)
			context.setFullscreen(true);
		if (!interactive && frame == 75)
			context.setFullscreen(false);
#elif defined(OS_APPLE)
		if (!interactive && frame == 5)
		{
			gkVkMacPostTestKey(56, true);
			gkVkMacPostTestKey(13, true);
			gkVkMacPostTestKey(13, false);
			gkVkMacPostTestKey(56, false);
			gkVkMacPostTestMouseMove(100.0f, 100.0f);
			gkVkMacPostTestMouseMove(140.0f, 125.0f);
			gkVkMacPostTestMouseButton(0, true, 140.0f, 125.0f);
			gkVkMacPostTestMouseButton(0, false, 140.0f, 125.0f);
		}
		if (!interactive && frame == 20)
			context.resize(800, 500);
		if (!interactive && frame == 60)
			context.setFullscreen(true);
		if (!interactive && frame == 75)
			context.setFullscreen(false);
#endif

		if (context.beginFrame())
		{
			context.prepareTexture(NULL);
			if (!context.drawRenderOperation(operation, identity,
				ColorF(0.2f, 0.75f, 1.0f, 1.0f), NULL, Vec2(1.0f, 1.0f)) ||
				!context.endFrame())
			{
				fprintf(stderr, "gkVulkanSmoke: indexed scene frame failed\n");
				return 2;
			}
		}
#if defined(OS_WIN32)
		Sleep(interactive ? 1 : 2);
#else
		usleep((interactive ? 1 : 2) * 1000);
#endif
	}

	context.shutdown();
#if defined(OS_APPLE)
	if (!interactive && !inputCapture.complete())
	{
		fprintf(stderr, "gkVulkanSmoke: macOS input bridge failed\n");
		return 3;
	}
	gEnv = NULL;
#endif
	printf("gkVulkanSmoke: %s completed\n", interactive ? "interactive run" : "passed");
	return 0;
}
