#pragma once

#include "IRenderer.h"

class gkVkDeviceContext;
class gkVkNullAuxRenderer;
class gkVkRenderSequence;
class gkVkTextureManager;
class gkVkMeshManager;
class gkVkMaterialManager;
class gkVkShaderManager;

class gkRendererVulkan : public IRenderer
{
public:
	gkRendererVulkan();
	virtual ~gkRendererVulkan();

	virtual IResourceManager* getResourceManager(BYTE type);
	virtual HWND Init(ISystemInitInfo& sii);
	virtual void Destroy();

	virtual IParticleProxy* createGPUParticleProxy();
	virtual void destroyGPUParticelProxy(IParticleProxy* proxy);

	virtual IRenderSequence* RT_SwapRenderSequence();
	virtual IRenderSequence* RT_GetRenderSequence();
	virtual void RT_CleanRenderSequence();
	virtual bool RT_StartRender();
	virtual bool RT_EndRender();
	virtual void RT_SkipOneFrame(int framecount = 1);

	virtual void FX_ColorGradingTo(gkTexturePtr& pCch, float fAmount);
	virtual void RC_SetSunDir(const Vec3& lightdir);

	virtual uint32 GetScreenHeight(bool forceOrigin = false);
	virtual uint32 GetScreenWidth(bool forceOrigin = false);
	virtual HWND GetWindowHwnd();
	virtual Vec2i GetWindowOffset();
	virtual void SetCurrContent(HWND hWnd, uint32 posx, uint32 posy,
		uint32 width, uint32 height, bool fullscreen = false);
	virtual IAuxRenderer* getAuxRenderer();
	virtual ERendererAPI GetRendererAPI();

	virtual Ray GetRayFromScreen(uint32 nX, uint32 nY);
	virtual Vec3 ProjectScreenPos(const Vec3& worldpos);
	virtual Vec3 ScreenPosToViewportPos(Vec3 screenPos);
	virtual bool FullScreenMode();

private:
	gkVkDeviceContext* m_context;
	gkVkNullAuxRenderer* m_auxRenderer;
	gkVkRenderSequence* m_updatingSequence;
	gkVkRenderSequence* m_renderingSequence;
	gkVkTextureManager* m_textureManager;
	gkVkMeshManager* m_meshManager;
	gkVkMaterialManager* m_materialManager;
	gkVkShaderManager* m_shaderManager;
	int m_skipFrames;
	bool m_fullscreen;
	Vec3 m_sunDirection;
};
