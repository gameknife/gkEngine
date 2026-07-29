#include "gkVkDeviceContext.h"
#include "ISystem.h"
#include "IMesh.h"

#include <cstdio>
#include <cstring>

SSystemGlobalEnvironment* gEnv = NULL;

int main(int argc, char** argv)
{
	const bool interactive = argc > 1 && strcmp(argv[1], "--interactive") == 0;
	ISystemInitInfo initInfo;
	initInfo.fWidth = interactive ? 1280 : 640;
	initInfo.fHeight = interactive ? 720 : 360;

	gkVkDeviceContext context;
	if (!context.initialize(initInfo))
	{
		fprintf(stderr, "gkVulkanSmoke: initialization failed\n");
		return 1;
	}

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
		Sleep(interactive ? 1 : 2);
	}

	context.shutdown();
	printf("gkVulkanSmoke: %s completed\n", interactive ? "interactive run" : "passed");
	return 0;
}
