//////////////////////////////////////////////////////////////////////////
/*
Copyright (c) 2011-2015 Kaiming Yi
	
	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:
	
	The above copyright notice and this permission notice shall be included in
	all copies or substantial portions of the Software.
	
	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
	THE SOFTWARE.
	
*/
//////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////
//
// Name:   	gkD3D9RenderSystem.h
// Desc:	抽象出的渲染系统，以方便配合各个部件的工作
// 描述:	
// 
// Author:  Kaiming-Desktop	 
// Date:	9/16/2010 	
// 
//////////////////////////////////////////////////////////////////////////
#pragma once

#include "RendererD3D9Prerequisites.h"
#include "IRenderer.h"
#include "gkShaderParamDataSource.h"
#include "ITexture.h"
#include "IShader.h"
#include "RenderThread.h"
#include "gkStateManager.h"
#include "gkRenderOperation.h"

struct DXUTDeviceSettings;
struct ID3DXFont;
struct ID3DXSprite;
class	CDXUTTextHelper;
class  gkAuxRenderer;
struct gkRenderableList;
class CCamera;
class IShader;
class gkGpuTimer;
struct gkRendererCVars;
struct IRenderSequence;
class gkRenderSequence;
struct gkShaderGroup;
class gkShader;
struct ID3DXEffectStateManager;

enum ERenderPipeType
{
	RP_ShadowMapGen = 0,
	RP_ReflMapGen,
	RP_ZpassDeferredLighting,
	RP_ZpassDeferredShading,
	RP_ShadowMaskGen,
	RP_AmbientLight,
	RP_DeferredLight,
	RP_ShadingPassDeferredLighting,
	RP_ShadingPassDeferredShading,
	RP_ShadingPassForwardShading,
	RP_DeferredFog,
	RP_HDR,
	RP_DOF,
	RP_MSAA,

	RP_Count,
};

enum ERenderStereoType
{
	eRS_Mono,
	eRS_LeftEye,
	eRS_RightEye,
};

struct IRendPipeBase;
typedef std::vector<IRendPipeBase*> RendPipelines;

struct gkRenderContent
{
	HWND m_hwnd;
	POINT m_offset;
	POINT m_size;
	bool m_fullscreen;
};

struct gkDynTex
{
	gkTexturePtr m_pTex;
	bool m_needDS;
	uint8 m_level;
	uint8 m_index;
	
	gkDynTex(gkTexturePtr pTex, uint8 level, uint8 index, bool needDS) { m_pTex = pTex; m_needDS = needDS; m_level = level; m_index = index; }
	~gkDynTex() {}
};

struct gkRenderTargetSize
{
	uint32 width;
	uint32 height;

	bool operator == ( const gkRenderTargetSize& other )
	{
		if ( other.width != width || other.height != height)
		{
			return false;
		}

		return true;
	}
};
inline bool operator < ( const gkRenderTargetSize& lhs, const gkRenderTargetSize& rhs)
{
	if ( lhs.width < rhs.width)
	{
		return true;
	}

	return false;
}


typedef std::map<gkRenderTargetSize, LPDIRECT3DSURFACE9> gkDSCaches;
typedef std::vector<gkDynTex> gkRenderTargetStack;
typedef std::map<gkStdString, gkGpuTimer> gkGpuTimerMap; 

class gkRendererD3D9 : public IRenderer
{
private:
	static IDirect3DDevice9*	m_pd3d9Device;			// D3D9设备指针 [9/19/2010 Kaiming-Laptop]
	static LPD3DXEFFECTPOOL		m_pd3dEffectPool;		// D3D9 shader pool
	static IDirect3DSwapChain9* m_pSwapChain1;
	bool m_bIsCreated;
	bool m_bFrameRendering;
	static LPDIRECT3DSURFACE9	m_pBackBufferSurface;
	static LPDIRECT3DSURFACE9	m_pOriginDepthSurface;
	static gkDSCaches			m_dsCaches;
	static gkDSCaches			m_nullRTs;

	IDirect3DQuery9*			m_pEvent;					// perfHud use it

	bool								m_bDeviceLost;

	bool								m_rtSizeChange;

	// resource management
	static IResourceManager*			m_pTextureManager;
	static IResourceManager*			m_pMeshManager;
	static IResourceManager*			m_pMaterialManager;
	static IResourceManager*			m_pShaderManager;
	
	// aux renderer
	static gkAuxRenderer*				m_pAuxRenderer;

	// shader 
	static gkShaderParamDataSource		m_pShaderParamDataSource;

	HWND								m_hWnd;

	bool								m_IsEditor;
	DWORD								m_dwWindowStyle;
	HINSTANCE							m_hInst;

	uint8								m_uCurrContent;
	std::vector<gkRenderContent>		m_lstRenderContent;

	bool								m_bIsSizeChange;
	LONG								m_lNewWidth;
	LONG								m_lNewHeight;
	bool								m_bFullScreen;

	static class gkStateManager* m_pStateManager;
	// create RT stack for the 4 MRT
	static gkRenderTargetStack					m_RTStack[4];

	CRenderThread						m_tdRenderThread;
	bool								m_bRenderThread;

	int									m_bSkipThisFrame;

	IFtFont*							m_pDefaultFont;

	ERenderStereoType					m_currStereoType;

	int									m_override_width;
	int									m_override_height;
	bool								m_use_override;

public:
	static uint32						ms_FrameCount;
	static gkGpuTimerMap				ms_GPUTimers;

	gkRendererCVars*					m_pRendererCVar;

public:
	gkRendererD3D9(void);
	virtual ~gkRendererD3D9(void);

	static IDirect3DDevice9*	getDevice() {return m_pd3d9Device;}
	virtual ID3DXEffectPool**	getEffectPoolPointer() {return &m_pd3dEffectPool;}

	virtual IResourceManager*	getResourceManager(BYTE type);

	virtual HWND Init(ISystemInitInfo& sii);
	virtual void Destroy();

	// DXUT bind function
	static HRESULT	CALLBACK	OnD3D9CreateDevice(LPDIRECT3DDEVICE9 pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext);
	static bool		CALLBACK	ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, void* pUserContext );
	static HRESULT	CALLBACK	OnD3D9ResetDevice(LPDIRECT3DDEVICE9 pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext);
	static void		CALLBACK	OnD3D9LostDevice(void* pUserContext);
	static void		CALLBACK	OnD3D9DestroyDevice(void* pUserContext );

	static void					OnD3D9ResetExDevice(LPDIRECT3DDEVICE9 pd3dDevice);

	static D3DPOOL GetManagedTexturePoolType();

	static ID3DXEffectStateManager* GetStateManager() {return m_pStateManager;}
	void RenderText();

	static gkShaderParamDataSource& getShaderContent() {return m_pShaderParamDataSource;}

	virtual bool CheckDevice();
	virtual void _startFrame( ERenderStereoType type = eRS_Mono );
	virtual void _endFrame( ERenderStereoType type = eRS_Mono );

	virtual void _beginScene();
	virtual void _endScene();

	static void _clearBuffer(bool clearDepth, DWORD color = 0x00000000);
	static void _clear2Buffer(bool clearDepth, DWORD color = 0x00000000);
	static void _render(const gkRenderOperation& op, bool isShadowPass = false);

	// GPU粒子更新ROUTINE接口 [4/12/2013 YiKaiming]
	virtual IParticleProxy* createGPUParticleProxy();
	virtual void destroyGPUParticelProxy(IParticleProxy* proxy);
	// RT的驱动入口
	virtual bool RT_StartRender();
	virtual bool RT_EndRender();
	virtual IRenderSequence* RT_SwapRenderSequence();
	virtual IRenderSequence* RT_GetRenderSequence();
	virtual void RT_CleanRenderSequence();
	virtual void RT_SkipOneFrame(int frameCount);

	bool RP_RenderScene(ERenderStereoType type = eRS_Mono);

	void debugDynTexs();

	static const D3DSURFACE_DESC* getBackBufferDesc();

	UINT GetScreenHeight(bool forceOrigin = false);
	UINT GetScreenWidth(bool forceOrigin = false);

	virtual HWND GetWindowHwnd();
	virtual Vec2i GetWindowOffset();

	bool CheckDeviceLost();
	virtual bool FullScreenMode();
	
	static void RS_EnableAlphaTest(bool enable, DWORD ref = 0x00000060);
	static void RS_SetRenderState(D3DRENDERSTATETYPE rs, DWORD value);
	bool SetWindow(int width, int height, bool fullscreen = false, HWND hWnd = 0);

	virtual void SetCurrContent(HWND hWnd, uint32 posx, uint32 posy, uint32 width, uint32 height, bool fullscreen=false);
	virtual void SetWindowContent(HWND hWnd, RECT windowRect);

	IAuxRenderer* getAuxRenderer();



	virtual Vec3 ScreenPosToViewportPos(Vec3 screenPos);



	virtual void RP_StretchRefraction();
	
	virtual void RC_SetTodKey(const STimeOfDayKey& key);
	virtual void RC_SetMainCamera(const CCamera* cam);
	virtual void RC_SetShadowCascadeCamera(const CCamera* cam, uint8 index);
	virtual void RC_SetSunDir(const Vec3& lightdir);

	//////////////////////////////////////////////////////////////////////////
	// RENDER PIPELINE FUNCTIONS
	// RP_FUNC()


	//////////////////////////////////////////////////////////////////////////
	// Process Single Renderable

	static void RP_ProcessSingleRenderable( gkRenderable* pRenderable, IShader* pShader, int8 ShadowCascade = -1);

	//////////////////////////////////////////////////////////////////////////
	// General RenderLayer Render Process
	// for every layer call
	static void RP_ProcessRenderLayer( uint8 layerID, EShaderInternalTechnique tech, bool skinned, int shadowCascade = -1 );

	// for every shader group call
	static void RP_ProcessShaderGroup( const gkShaderGroup* pShaderGroup, EShaderInternalTechnique tech, bool skinned = false, int shadowCascade = -1);
	// for every material group call
	static void RP_ProcessRenderList(  const gkRenderableList* obj, IShader* pShader, bool skinned = false, int shadowCascade = -1 );

	// ShadowMap Generation
	static void RP_ProcessShadowObjects( const gkRenderableList* objs, uint8 cascade, BYTE sortType = 0 );
	// ReflectionMap Generation
	void RP_GenReflectExcute( const gkRenderableList* objs, IShader* pShader, BYTE sortType /*= 0*/);

	// Zpass Generation
	static void RP_ZprepassPrepare();
	static void RP_ProcessZpassObjects( const gkRenderableList* objs, IShader* pShader, EShaderInternalTechnique tech);
	static void RP_ZprepassEnd();

	void RP_DeferredLightExcute(const gkRenderLightList& LightList);

	void RP_DeferredSnow();
	void RP_FogProcess();
	void RP_HDRProcess();
	void RP_DepthOfField();
	void RP_FinalOutput();
	void RP_FXSpecil();
	void RP_SetHDRParams(gkShaderPtr pShader);

	void RP_SSRL();

	// wrapped RT process
	static void FX_StrechRect(gkTexturePtr src, gkTexturePtr dest, uint8 src_level = 0, uint8 dest_level = 0, bool FilterIfNeed = false, uint8 cubeindex = 0);

	static void FX_PushHwDepthTarget(gkTexturePtr src, bool bClearTarget = false);
	static void FX_PopHwDepthTarget();

	static void FX_PushRenderTarget(uint8 channel, gkTexturePtr src,  uint8 level = 0, uint8 index = 0, bool bNeedDS = false, bool bClearTarget = false);
	static void FX_PopRenderTarget(uint8 channel);
	static void FX_SetRenderTarget(uint8 channel, gkTexturePtr src, uint8 level, uint8 index, bool bNeedDS, bool bClearTarget);
	static void FX_RestoreRenderTarget(uint8 channel);
	static void FX_RestoreBackBuffer();
	static void FX_StrechToBackBuffer(gkTexturePtr tex, ERenderStereoType type);


	// Utils
	static void FX_TexBlurGaussian(gkTexturePtr tgt, int nAmount, float fScale, float fDistribution, gkTexturePtr tmp, int iterate = 1, bool blur_mipmapchain = false);
	
	static void FX_BlurCubeMap(gkTexturePtr tgt, int nAmount, float fScale, float fDistribution, gkTexturePtr tmp, int iterate = 1);
	
	static void GaussionBlurWithMipLevel(gkTexturePtr tmp, float fDistribution, float fScale, int iterate, uint8 source_blur_mipmapchain, gkShaderPtr pShader, gkTexturePtr tgt, uint8 target_blur_mipmapchain);

	static void GaussionBlurV(float fDistribution, float fScale, float s1, float t1, gkTexturePtr tgt, uint8 target_blur_mipmapchain, gkShaderPtr pShader, uint8 source_blur_mipmapchain, gkTexturePtr tmp);

	static void GaussionBlurH(float fDistribution, float fScale, float s1, float t1, gkTexturePtr tmp, uint8 source_blur_mipmapchain, gkShaderPtr pShader, gkTexturePtr tgt);


	static void FX_TexBlurDirectional(gkTexturePtr pTex, const Vec2 &vDir, int nIterationsMul, gkTexturePtr pTmp);
	static void FX_ClearAllSampler();

	// texture debug
	void FX_DrawDynTextures(gkTexturePtr tex, Vec4& region);
	void FX_DrawFloatTextures(gkTexturePtr tex, Vec4& region, float scale = 1.0f, float base = 0.f);
	static void FX_DrawScreenImage(gkTexturePtr tex, Vec4& region);
	static void FX_DrawFullScreenQuad(gkTexturePtr& tex);

	virtual void FX_ColorGradingTo( gkTexturePtr& pCch, float fAmount );

	virtual ERendererAPI GetRendererAPI() {return ERdAPI_D3D9;}

	virtual Ray GetRayFromScreen( uint32 nX, uint32 nY );
	virtual Vec3 ProjectScreenPos( const Vec3& worldpos );

	//////////////////////////////////////////////////////////////////////////
	// Gpu Timer Functions
	static void buildGpuTimers();
	static void initGpuTimers(IDirect3DDevice9* pDevice);
	static void destoryGpuTimers();
	static void updateGpuTimers();

	static void SetRenderState(uint32 key, uint32 value);

	virtual IResourceManager* getTextureMngPtr() {return m_pTextureManager;}
	virtual IResourceManager* getMaterialMngPtr() {return m_pMaterialManager;}
	virtual IResourceManager* getMeshMngPtr() {return m_pMeshManager;}
	virtual IResourceManager* getShaderMngPtr() {return m_pShaderManager;}

	virtual void SetOverrideSize(int width, int height, bool set);
	virtual void GetOverrideSize(int& width, int& height) { width = m_override_width; height = m_override_height;}

	virtual void SetPixelReSize(float scale);
	virtual float GetPixelReSize();

	static gkRenderSequence*	m_pUpdatingRenderSequence;
	static gkRenderSequence*	m_pRenderingRenderSequence;

	std::vector<IParticleProxy*> m_particleProxy;

	RendPipelines m_pipelines;

	RECT m_windowRectInClient;

	Vec2i m_adapterFullscreenSize;

public:
	static struct IDirect3DVertexDeclaration9* m_generalDeclPt4F2;
	static struct IDirect3DVertexDeclaration9* m_generalDeclPt4F4;
	static struct IDirect3DVertexDeclaration9* m_generalDeclP4F4;
	static struct IDirect3DVertexDeclaration9* m_generalDeclP4F4F4;
	static struct IDirect3DVertexDeclaration9* m_generalDeclP4F4F4F4U4;
	static struct IDirect3DVertexDeclaration9* m_generalDeclT2T2;
	static struct IDirect3DVertexDeclaration9* m_generalDeclP3T2U4;
	static struct IDirect3DVertexDeclaration9* m_generalDeclPt2T2T2T2T2;

	class gkColorGradingController* m_pColorGradingController;	
	static D3DSURFACE_DESC m_bb_desc;

	static IDirect3DSurface9* m_cache_surf_cubemap;
	static IDirect3DSurface9* m_cache_ds_cubemap;
};

gkRendererD3D9* getRenderer();
