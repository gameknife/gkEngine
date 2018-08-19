#include "RendererD3D9StableHeader.h"
#include "gkRendererD3D9.h"
#include "DXUT.h"

#include "ITimer.h"

#include "ISystem.h"

// resource manager
#include "IResourceManager.h"
#include "RenderRes\gkTextureManager.h"
#include "RenderRes\gkMaterialManager.h"
#include "RenderRes\gkMeshManager.h"
#include "RenderRes\gkShaderManager.h"

#include "RenderRes/gkShader.h"

// pp
#include "gkPostProcessManager.h"

#include "gkAuxRenderer.h"

#include "ICamera.h"


#include "IShader.h"
#include "gkRenderable.h"
#include "gk_Camera.h"
#include "ITimeOfDay.h"

#include "..\Profiler\gkGPUTimer.h"
#include "gkRenderSequence.h"
#include "gkRenderLayer.h"

#include "gkCommonStructs.h"


#include "RenderSys\gkColorGradingController.h"
#include "IMesh.h"

#include "gkStateManager.h"
#include "gkGPUInfo.h"

#include "gkGpuParticleProxy.h"
#include "RendPipe_ShadowMapGen.h"

#include "gkMemoryLeakDetecter.h"
#include "RendPipe_ZpassDeferredLighting.h"
#include "RendPipe_ShadingPassDeferredLighting.h"
#include "RendPipe_ZpassDeferredShading.h"
#include "RendPipe_ShadingPassDeferredShading.h"

#include "RenderCamera.h"
#include "IStereoDevice.h"
#include "IInputManager.h"
#include "RendPipe_ReflGen.h"
#include "RendPipe_OcculusionGen.h"
#include "RendPipe_SSRL.h"
#include "RendPipe_DeferredFog.h"
#include "RendPipe_LightPasses.h"
#include "gkLightProbeSystem.h"

#define USE_DXUT 0
#define USE_SWAPCHAIN 1;

LRESULT CALLBACK gkStudioFSWndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
	{
	case WM_PAINT:
		{
			getRenderer()->getDevice()->Present( NULL, NULL, NULL, NULL);
			break;
		}
	case WM_KEYDOWN:
		{
			switch( wParam )
			{
			case VK_F8:
				{

					static bool fullscreen = true;
					static RECT rcCurrentClient;
					static RECT rcWindowRect;
					if(getRenderer()->FullScreenMode())
					{
						getRenderer()->SetCurrContent( NULL, 
							rcCurrentClient.left, 
							rcCurrentClient.top, 
							rcCurrentClient.right - rcCurrentClient.left, 
							rcCurrentClient.bottom - rcCurrentClient.top );

						getRenderer()->SetWindowContent(hWnd, rcWindowRect);

						::ShowWindow(hWnd, SW_HIDE);
					}
					else
					{
						GetClientRect( hWnd, &rcCurrentClient );
						GetWindowRect( hWnd, &rcWindowRect );

						int width = getRenderer()->m_adapterFullscreenSize.x;
						int height = getRenderer()->m_adapterFullscreenSize.y;
						getRenderer()->SetCurrContent( hWnd, 0,0,width,height,true );
					}
					return 0;
				}
			}
			break;
		}
	}

	return DefWindowProc( hWnd, uMsg, wParam, lParam );
}

LRESULT CALLBACK gkENGINEStaticWndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
	{
	case WM_PAINT:
		{
			getRenderer()->getDevice()->Present( NULL, NULL, NULL, NULL);
			break;
		}

	case WM_SIZE:
		if( SIZE_MINIMIZED == wParam )
		{

		}
		else
		{
			RECT rcCurrentClient;
			GetClientRect( hWnd, &rcCurrentClient );
			RECT rcWindowRect;
			GetWindowRect( hWnd, &rcWindowRect);
			if( rcCurrentClient.top == 0 && rcCurrentClient.bottom == 0 )
			{
				// Rapidly clicking the task bar to minimize and restore a window
				// can cause a WM_SIZE message with SIZE_RESTORED when 
				// the window has actually become minimized due to rapid change
				// so just ignore this message
			}
			else if( SIZE_MAXIMIZED == wParam )
			{

			}
			else if( SIZE_RESTORED == wParam )
			{

			}

			getRenderer()->SetCurrContent( hWnd, rcCurrentClient.left, rcCurrentClient.top, rcCurrentClient.right - rcCurrentClient.left, rcCurrentClient.bottom - rcCurrentClient.top, getRenderer()->FullScreenMode() );
			getRenderer()->SetWindowContent(hWnd, rcWindowRect);
		}
		break;

	case WM_GETMINMAXINFO:
		break;

	case WM_ENTERSIZEMOVE:
		break;

	case WM_EXITSIZEMOVE:
		break;

	case WM_MOUSEMOVE:
		break;

	case WM_SETCURSOR:
		break;

	case WM_ACTIVATEAPP:

		break;

	case WM_ENTERMENULOOP:
		break;

	case WM_EXITMENULOOP:
		break;

	case WM_MENUCHAR:
		return MAKELRESULT( 0, MNC_CLOSE );
		break;

	case WM_NCHITTEST:
		break;

	case WM_POWERBROADCAST:
		switch( wParam )
		{
#ifndef PBT_APMQUERYSUSPEND
#define PBT_APMQUERYSUSPEND 0x0000
#endif
		case PBT_APMQUERYSUSPEND:
			return true;

#ifndef PBT_APMRESUMESUSPEND
#define PBT_APMRESUMESUSPEND 0x0007
#endif
		case PBT_APMRESUMESUSPEND:
			return true;
		}
		break;

	case WM_CLOSE:
		{
			HMENU hMenu;
			hMenu = GetMenu( hWnd );
			if( hMenu != NULL )
				DestroyMenu( hMenu );
			DestroyWindow( hWnd );
			UnregisterClass( _T("Direct3DWindowClass"), NULL );
			return 0;
		}
	case WM_SYSKEYDOWN:
		{
			switch( wParam )
			{
			case VK_RETURN:
				{

					{
						// Toggle full screen upon alt-enter 
						DWORD dwMask = ( 1 << 29 );
						if( ( lParam & dwMask ) != 0 ) // Alt is down also
						{
							static bool fullscreen = true;
							static RECT rcCurrentClient;
							static RECT rcWindowRect;
							if(getRenderer()->FullScreenMode())
							{
								getRenderer()->SetCurrContent( hWnd, 
									rcCurrentClient.left, 
									rcCurrentClient.top, 
									rcCurrentClient.right - rcCurrentClient.left, 
									rcCurrentClient.bottom - rcCurrentClient.top );

								getRenderer()->SetWindowContent(hWnd, rcWindowRect);
							}
							else
							{
								GetClientRect( hWnd, &rcCurrentClient );
								GetWindowRect( hWnd, &rcWindowRect );

								int width = getRenderer()->m_adapterFullscreenSize.x;
								int height = getRenderer()->m_adapterFullscreenSize.y;
								getRenderer()->SetCurrContent( hWnd, 0,0,width,height,true );
							}



							//::MoveWindow( hWnd, 0,0,width, height, TRUE );

							//fullscreen = !fullscreen;
							return 0;
						}
					}
				}
			}
			break;
		}
	case WM_DESTROY:
		PostQuitMessage( 0 );
		break;
	}

	// Don't allow the F10 key to act as a shortcut to the menu bar
	// by not passing these messages to the DefWindowProc only when
	// there's no menu present
	// 	if( !GetDXUTState().GetCallDefWindowProc() || GetDXUTState().GetMenu() == NULL &&
	// 		( uMsg == WM_SYSKEYDOWN || uMsg == WM_SYSKEYUP ) && wParam == VK_F10 )
	// 		return 0;
	// 	else
	return DefWindowProc( hWnd, uMsg, wParam, lParam );
}


// Vertex declaration
D3DVERTEXELEMENT9 gkVIDeclT2T2[] =
{
	{ 0,  0, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD, 0},
	{ 0,  8, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD, 1},
	D3DDECL_END()
};



D3DVERTEXELEMENT9 gkVIDeclP4F4[] =
{
	{ 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_POSITION, 0},
	{ 0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD, 0},
	D3DDECL_END()
};

D3DVERTEXELEMENT9 gkVIDeclPt4F4[] =
{
	{ 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_POSITION, 0},
	{ 0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD, 0},
	D3DDECL_END()
};

D3DVERTEXELEMENT9 gkVIDeclPt4F2[] =
{
	{ 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_POSITION, 0},
	{ 0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD, 0},
	{ 0, 20, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD, 1},
	D3DDECL_END()
};

D3DVERTEXELEMENT9 gkVIDeclPt2T2T2T2T2[] =
{
	{ 0,  0, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_POSITION, 0},
	{ 0,  8, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD, 0},
	{ 0,  16, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD, 1},
	{ 0,  24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD, 2},
	{ 0,  32, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD, 3},
	D3DDECL_END()
};

D3DVERTEXELEMENT9 gkVertexDeclP4F4F4[] =
{
	{ 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_POSITION, 0},				// POS
	{ 0, 12, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD, 0},				// TEX0|TEX1
	{ 0, 28, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD, 1},				// TANGENT|FLIP
	{ 0, 44, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD, 2},				// BINORMAL
	D3DDECL_END()
};

D3DVERTEXELEMENT9 gkVIDeclP4F4F4U4[] =
{
	{ 0,  0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_POSITION, 0},
	{ 0, 16, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD, 0},
	{ 0, 32, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD, 1},
	{ 0, 48, D3DDECLTYPE_UBYTE4, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD, 2},
	D3DDECL_END()
};

D3DVERTEXELEMENT9 gkVertexDeclP4F4F4F4U4[] =
{
	{ 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_POSITION, 0},
	{ 0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD, 0},
	{ 0, 20, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD, 1},
	{ 0, 36, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD, 2},
	{ 0, 48, D3DDECLTYPE_UBYTE4N, D3DDECLMETHOD_DEFAULT,	 D3DDECLUSAGE_BLENDWEIGHT, 0},
	{ 0, 52, D3DDECLTYPE_UBYTE4, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_BLENDINDICES, 0},
	D3DDECL_END()
};

D3DVERTEXELEMENT9 gkVertexDeclP3T2U4[] =
{
	{ 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_POSITION, 0},
	{ 0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD, 0},
	{ 0, 20, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD, 1},
	D3DDECL_END()
};


struct AXISVERTEX										//轴顶点类型
{
	D3DXVECTOR3 position;							// 顶点位置
	D3DXVECTOR2 texcoord;  								// 漫反射颜色
};

gkRendererD3D9* getRenderer()
{
	return static_cast<gkRendererD3D9*>(gEnv->pRenderer);
}

D3DSURFACE_DESC gkRendererD3D9::m_bb_desc;
IDirect3DDevice9*	gkRendererD3D9::m_pd3d9Device = NULL;
LPD3DXEFFECTPOOL	gkRendererD3D9::m_pd3dEffectPool = NULL;	
IDirect3DSwapChain9*	gkRendererD3D9::m_pSwapChain1 = NULL;

IResourceManager*	gkRendererD3D9::m_pTextureManager = NULL;
IResourceManager*	gkRendererD3D9::m_pMeshManager = NULL;
IResourceManager*	gkRendererD3D9::m_pMaterialManager = NULL;
IResourceManager*	gkRendererD3D9::m_pShaderManager = NULL;

gkAuxRenderer*		gkRendererD3D9::m_pAuxRenderer = 0;
LPDIRECT3DSURFACE9	gkRendererD3D9::m_pBackBufferSurface = NULL;
LPDIRECT3DSURFACE9  gkRendererD3D9::m_pOriginDepthSurface = NULL;
LPDIRECT3DSURFACE9	gkRendererD3D9::m_pBackBufferSystemSurface = NULL;
uint32*				gkRendererD3D9::m_pBackBufferSystemData = NULL;

gkDSCaches			gkRendererD3D9::m_dsCaches;
gkDSCaches			gkRendererD3D9::m_nullRTs;

uint32				gkRendererD3D9::ms_FrameCount = 0;

gkShaderParamDataSource	gkRendererD3D9::m_pShaderParamDataSource;

gkGpuTimerMap		gkRendererD3D9::ms_GPUTimers;

gkRenderTargetStack					gkRendererD3D9::m_RTStack[4];

IDirect3DVertexDeclaration9* gkRendererD3D9::m_generalDeclPt2T2T2T2T2 = NULL;
IDirect3DVertexDeclaration9* gkRendererD3D9::m_generalDeclPt4F2 = NULL;
IDirect3DVertexDeclaration9* gkRendererD3D9::m_generalDeclPt4F4 = NULL;
IDirect3DVertexDeclaration9* gkRendererD3D9::m_generalDeclP4F4 = NULL;
IDirect3DVertexDeclaration9* gkRendererD3D9::m_generalDeclP4F4F4 = NULL;
IDirect3DVertexDeclaration9* gkRendererD3D9::m_generalDeclP4F4F4F4U4 = NULL;
IDirect3DVertexDeclaration9* gkRendererD3D9::m_generalDeclT2T2 = NULL;
IDirect3DVertexDeclaration9* gkRendererD3D9::m_generalDeclP3T2U4 = NULL;

gkStateManager* gkRendererD3D9::m_pStateManager = NULL;

IDirect3DSurface9* gkRendererD3D9::m_cache_surf_cubemap = NULL;
IDirect3DSurface9* gkRendererD3D9::m_cache_ds_cubemap = NULL;


gkRenderSequence*	gkRendererD3D9::m_pUpdatingRenderSequence;
gkRenderSequence*	gkRendererD3D9::m_pRenderingRenderSequence;
gkLightProbeSystem* gkRendererD3D9::m_pLightProbeSystem;

// 轴自定义FVF, which describes our custom vertex structure
#define D3DFVF_AXISVERTEX (D3DFVF_XYZ|D3DFVF_TEX1)

gkRendererD3D9::gkRendererD3D9( void ):m_bIsCreated(false)
	,m_hWnd(0)
	,m_IsEditor(0)
	,m_dwWindowStyle(-1)
	,m_bIsSizeChange(false)
	,m_lNewWidth(1280)
	,m_lNewHeight(720)
	,m_bRenderThread(false)
	,m_bSkipThisFrame(0)
	,m_currStereoType(eRS_Mono)
	,m_use_override(false)
	,m_override_height(0)
	,m_override_width(0)
	,m_bFullScreen(false)
{
	m_pAuxRenderer = new gkAuxRenderer;
	m_lstRenderContent.clear();
}

gkRendererD3D9::~gkRendererD3D9( void )
{
	SAFE_DELETE( m_pAuxRenderer );
}

bool gkRendererD3D9::CheckDeviceLost()
{
	HRESULT hReturn;

	GK_ASSERT(m_pd3d9Device);

	// Test the cooperative level to see if it's okay to render
	if (FAILED(hReturn = m_pd3d9Device->TestCooperativeLevel()))
	{
		// If the device was lost, do not render until we get it back
		if (D3DERR_DEVICELOST == hReturn)
		{
			//RestoreGamma();
			m_bDeviceLost = true;
			return true;
		}
		// Check if the device needs to be reset.
		if (D3DERR_DEVICENOTRESET == hReturn)
		{
			m_bDeviceLost = true;
			if(FAILED(hReturn = DXUTReset3DEnvironment9()))
				return true;
			m_bDeviceLost = false;
		}
	}
	m_bDeviceLost = false;
	return false;
}

//-----------------------------------------------------------------------
void gkRendererD3D9::_beginScene()
{
	if( SUCCEEDED( m_pd3d9Device->BeginScene()))
		m_bFrameRendering = true;

	// SET BACKBUFFER
	if (g_pRendererCVars->r_PostMsaa)
	{
		if(g_pRendererCVars->r_PostMsaaMode != 2)
		{
			if ( gEnv->pProfiler->getFrameCount() % 2 )
			{
				if( m_currStereoType == eRS_RightEye)
				{
					gkTextureManager::ms_BackBuffer = gkTextureManager::ms_BackBufferA_RightEye;
				}
				else
				{
					gkTextureManager::ms_BackBuffer = gkTextureManager::ms_BackBufferA_LeftEye;
				}
			}
			else
			{
				if( m_currStereoType == eRS_RightEye)
				{
					gkTextureManager::ms_BackBuffer = gkTextureManager::ms_BackBufferB_RightEye;
				}
				else
				{
					gkTextureManager::ms_BackBuffer = gkTextureManager::ms_BackBufferB_LeftEye;
				}
			}
		}
		else
		{
			gkTextureManager::ms_BackBuffer = gkTextureManager::ms_BackBuffer_Mono;
		}

	}
	else
	{
		gkTextureManager::ms_BackBuffer = gkTextureManager::ms_BackBufferA_LeftEye;
	}

	FX_PushRenderTarget( 0, gkTextureManager::ms_BackBuffer, 0, 0, true );
}
//-----------------------------------------------------------------------
void gkRendererD3D9::_endScene()
{
	FX_PopRenderTarget(0);

	if (m_bFrameRendering)
	{
		m_pd3d9Device->EndScene();
	}
}

bool gkRendererD3D9::CheckDevice()
{
	if (m_bIsSizeChange)
	{
		if(USE_DXUT)
		{
			DXUTChangeRenderSize(m_lNewWidth, m_lNewHeight);
		}
		else
		{
			bool FakeFullScreen = true;

			if(FakeFullScreen && m_bFullScreen)
			{
				m_lNewWidth = getRenderer()->m_adapterFullscreenSize.x;
				m_lNewHeight = getRenderer()->m_adapterFullscreenSize.y;
			}

			if(m_pBackBufferSystemData != NULL)
			{
				free(m_pBackBufferSystemData);
			}

			m_pBackBufferSystemData = (uint32*)malloc(m_lNewWidth * m_lNewHeight * sizeof(uint32));

			// D3D9EX, Directly Change the BackBuffer
			D3DPRESENT_PARAMETERS pp;
			memset( &pp, 0, sizeof(pp));

			pp.BackBufferWidth = m_lNewWidth;
			pp.BackBufferHeight = m_lNewHeight;
			pp.BackBufferCount = 0;
			pp.BackBufferFormat = D3DFMT_A8R8G8B8;

			pp.AutoDepthStencilFormat = D3DFMT_D24S8;
			pp.EnableAutoDepthStencil = TRUE;

			pp.hDeviceWindow = m_hWnd;

			pp.Windowed = !m_bFullScreen;

			if(m_bFullScreen && FakeFullScreen)
			{
				pp.Windowed = TRUE;
			}

			pp.SwapEffect = D3DSWAPEFFECT_DISCARD;

			pp.FullScreen_RefreshRateInHz = (m_bFullScreen && !FakeFullScreen) ? 60 : 0;

			pp.MultiSampleType = D3DMULTISAMPLE_NONE;
			pp.MultiSampleQuality = 0;

			pp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

			pp.Flags |= D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;

			D3DDISPLAYMODEEX* modeptr = NULL;
			if ((m_bFullScreen && !FakeFullScreen))
			{
				D3DDISPLAYMODEEX mode;
				modeptr = &mode;
				mode.Width = m_lNewWidth;
				mode.Height = m_lNewHeight;
				mode.RefreshRate = 60;
				mode.Size = sizeof(D3DDISPLAYMODEEX);
				mode.Format = D3DFMT_A8R8G8B8;
				mode.ScanLineOrdering = D3DSCANLINEORDERING_PROGRESSIVE;
			}

			((IDirect3DDevice9Ex*)m_pd3d9Device)->ResetEx(&pp, modeptr);

#if USE_SWAPCHAIN
			m_pSwapChain1->Release();
			((IDirect3DDevice9Ex*)m_pd3d9Device)->CreateAdditionalSwapChain( &pp, &m_pSwapChain1);
#endif

			gEnv->pInputManager->setHWND( gEnv->pSystem->getHWnd() );  

			if(!m_IsEditor)
			{
				if(!m_bFullScreen)
				{
					::MoveWindow( m_lstRenderContent[m_uCurrContent].m_hwnd, m_windowRectInClient.left,
						m_windowRectInClient.top,
						m_windowRectInClient.right - m_windowRectInClient.left, 
						m_windowRectInClient.bottom - m_windowRectInClient.top , TRUE );
				}
			}
			else
			{
				if(m_bFullScreen)
				{
					gEnv->pInputManager->setHWND( m_hWnd );  
					::ShowWindow( m_hWnd, SW_SHOW );
					::MoveWindow( m_hWnd, 0,0,m_lstRenderContent[m_uCurrContent].m_size.x,m_lstRenderContent[m_uCurrContent].m_size.y, TRUE );
				}
			}

			HWND target = m_lstRenderContent[m_uCurrContent].m_hwnd;

			// window style
			if (!m_IsEditor)
			{
				long style;
				style=GetWindowLong(target,GWL_STYLE);
				style |= WS_CAPTION;
				style |= WS_THICKFRAME;
				SetWindowLong(target,GWL_STYLE,style );
			}

			if(m_bFullScreen && FakeFullScreen)
			{
				if(m_IsEditor)
				{
					target = m_hWnd;
				}

				// window style
				long style;
				style=GetWindowLong(target,GWL_STYLE);

				style &= ~WS_CAPTION;
				style &= ~WS_THICKFRAME;

				SetWindowLong(target,GWL_STYLE,style );
				::MoveWindow( target, 0,0,m_lNewWidth,m_lNewHeight, TRUE );
				::SetForegroundWindow( target );
				::SetFocus( target);
			}

			m_pd3d9Device->GetRenderTarget(0, &m_pBackBufferSurface);	
#if USE_SWAPCHAIN
			m_pSwapChain1->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &m_pBackBufferSurface );
#endif
			m_pd3d9Device->GetDepthStencilSurface( &m_pOriginDepthSurface );

			m_rtSizeChange = true;
		}

		Sleep(50);
		m_bIsSizeChange = false;
	}

	CheckDeviceLost();

	if( m_rtSizeChange )
	{
		gEnv->p3DEngine->getMainCamera()->setAspect( GetScreenWidth() / (float)GetScreenHeight() );

		OnD3D9ResetExDevice(m_pd3d9Device);
		m_rtSizeChange = false;
	}

	if (m_bDeviceLost)
	{
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------
void gkRendererD3D9::_startFrame(ERenderStereoType stereoType)
{
	m_currStereoType = stereoType;

	// 根据stereo type来reset RT
	if( m_currStereoType == eRS_Mono || m_currStereoType == eRS_LeftEye )
	{
		_clearBuffer(true, 0xff0000ff);
	}

	CCamera camL = *(m_pShaderParamDataSource.m_pCam);
	CCamera camR = *(m_pShaderParamDataSource.m_pCam);

	Vec3 leftEyeOffset, rightEyeOffset;
	gEnv->p3DEngine->getMainCamera()->getStereoOffset( leftEyeOffset, rightEyeOffset );

	camL.SetPosition( camL.GetPosition() + leftEyeOffset );
	camR.SetPosition( camR.GetPosition() + rightEyeOffset );

	switch( m_currStereoType )
	{
	case eRS_LeftEye:

		m_pShaderParamDataSource.setRenderMainCamera( &camL );
		//m_pShaderParamDataSource.setOverrideProjectionMatrix( leftProj, true );
		break;
	case eRS_RightEye:
		m_pShaderParamDataSource.setRenderMainCamera( &camR );
		//m_pShaderParamDataSource.setOverrideProjectionMatrix( rightProj, true );
		break;
	}

	SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
}

static inline uint32 uint32RGB_2_uint32ABGR(const uint32 rgb)
{
	uint32 ret = ((rgb & 0xff) << 16) |
		((rgb & 0xff00) << 0) |
		((rgb & 0xff0000) >> 16) |
		((0xff) << 24);

	return ret;
}

//-----------------------------------------------------------------------
void gkRendererD3D9::_endFrame(ERenderStereoType stereoType)
{
	m_currStereoType = stereoType;

	if (m_bDeviceLost)
	{
		return;
	}
	HRESULT hReturn;

	if( SUCCEEDED( m_pd3d9Device->BeginScene() ) )
	{

		FX_PushRenderTarget(0, gkTextureManager::ms_BackBuffer);

		if (m_pAuxRenderer && g_pRendererCVars->r_aux)
		{
			// flush AuxObject

			m_pAuxRenderer->_FlushAllHelper(m_pd3d9Device, (stereoType==eRS_Mono || stereoType==eRS_RightEye));
		}

		FX_PopRenderTarget(0);

		FX_PushRenderTarget(0, gkTextureManager::ms_BackBuffersStereoOut );

		// MSAA PROCESS here
		//if (g_pRendererCVars->r_PostMsaa)
		{
			// 这个我们之后再来处理
			SetRenderState(D3DRS_ZENABLE, FALSE);
			SetRenderState( D3DRS_LIGHTING, FALSE );
			SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
			SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
			SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
			SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

			RP_FinalOutput();
		}
// 		else
// 		{
// 			FX_StrechRect( gkTextureManager::ms_hdr )
// 			//FX_StrechToBackBuffer( gkTextureManager::ms_BackBuffer, stereoType );
// 		}

		gEnv->pProfiler->setStartCommit();

		// finally output debug info
		if (g_pRendererCVars->r_DebugDynTex)
		{
			debugDynTexs();
		}

		if (m_pAuxRenderer && g_pRendererCVars->r_aux)
		{
			// flush AuxObject
			m_pAuxRenderer->_FlushAllHelperDeferred(m_pd3d9Device,(stereoType==eRS_Mono || stereoType==eRS_RightEye));
		}

		if ( g_pRendererCVars->r_disinfo)
		{
			// Console
			if (gEnv->pCVManager->isConsoleEnable())
			{
				Vec4 region = Vec4(0.0f,0.0f,1.0f,0.5f);
				gkTexturePtr texTesting = gEnv->pSystem->getTextureMngPtr()->load(_T("engine/assets/textures/default/consolebg.dds"), _T("internal"));
				if (!texTesting.isNull())
				{
					FX_DrawScreenImage(texTesting, region);
				}
				//gEnv->pCVManager->render();
			}
		}

		RS_SetRenderState( D3DRS_ZENABLE, FALSE );
		RS_SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
		RS_SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

		RS_SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		RS_SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

		RP_ProcessRenderLayer(RENDER_LAYER_HUDUI, eSIT_General, false);

		RS_SetRenderState( D3DRS_ZENABLE, FALSE );
		RS_SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
		RS_SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);



		// 全部绘制结束，back to L/R
		FX_PopRenderTarget( 0 );

		FX_StrechToBackBuffer( gkTextureManager::ms_BackBuffersStereoOut, stereoType );

		// RENDER A STEREO DEVICE
		if( stereoType == eRS_RightEye)
		{
			_clearBuffer( 0, 0);

			RS_SetRenderState( D3DRS_ZENABLE, FALSE );
			RS_SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
			RS_SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

			RS_SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
			RS_SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

			RP_ProcessRenderLayer(RENDER_LAYER_STEREO_DEVICE, eSIT_General, false);

			RS_SetRenderState( D3DRS_ZENABLE, FALSE );
			RS_SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
			RS_SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		}

		if (m_pStateManager)
		{
			m_pStateManager->EndFrameStats();
		}


		m_pd3d9Device->EndScene();
	}

	FX_ClearAllSampler();

	if( stereoType == eRS_Mono || stereoType == eRS_RightEye )
	{
		if(!m_IsEditor)
		{
			// GameMode
			if(USE_DXUT)
			{
				if (FAILED((hReturn = (m_pd3d9Device)->Present(NULL, NULL, NULL, NULL))))
				{
					if( D3DERR_DEVICELOST == hReturn )
					{
						m_bDeviceLost = true;
					}
					else if( D3DERR_DRIVERINTERNALERROR == hReturn )
					{
						m_bDeviceLost = true;
					}
				}
			}
			else
			{
#if USE_SWAPCHAIN
				m_pSwapChain1->Present(NULL,NULL,NULL,NULL,NULL);
#else
				if (FAILED((hReturn = (m_pd3d9Device)->Present(NULL, NULL, NULL, NULL))))
				{
					if( D3DERR_DEVICELOST == hReturn )
					{
						m_bDeviceLost = true;
					}
					else if( D3DERR_DRIVERINTERNALERROR == hReturn )
					{
						m_bDeviceLost = true;
					}
				}
#endif



				HRESULT res = S_OK;
				IDirect3DSurface9* bb;
				m_pSwapChain1->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &bb);

				// get buffer from backbuffer
				if (m_pBackBufferSystemSurface == NULL)
				{
					m_pd3d9Device->CreateOffscreenPlainSurface(m_lNewWidth, m_lNewHeight,
						D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM,
						&m_pBackBufferSystemSurface, NULL);
				}


				res = m_pd3d9Device->GetRenderTargetData(bb, m_pBackBufferSystemSurface);
				if (res != S_OK)
				{
					// FATAL ERROR
					return;
				}

				D3DLOCKED_RECT lockinfo;
				memset(&lockinfo, 0, sizeof(lockinfo));

				res = m_pBackBufferSystemSurface->LockRect(&lockinfo, NULL, D3DLOCK_DISCARD);
				if (res != S_OK)
				{
					// FATAL ERROR
					return;
				}

				memcpy(m_pBackBufferSystemData, lockinfo.pBits, m_lNewWidth * m_lNewHeight * sizeof(uint32));
				// swizzle
				for(int i=0; i < m_lNewWidth * m_lNewHeight; ++i)
				{
					m_pBackBufferSystemData[i] = uint32RGB_2_uint32ABGR(m_pBackBufferSystemData[i]);
				}

				m_pBackBufferSystemSurface->UnlockRect();
				m_pBackBufferSystemSurface->Release();
				m_pBackBufferSystemSurface = NULL;


			}

		}
		else
		{
			if (!m_lstRenderContent.empty())
			{
				RECT rc;
				rc.left = m_lstRenderContent[m_uCurrContent].m_offset.x;
				rc.right = rc.left + m_lstRenderContent[m_uCurrContent].m_size.x;
				rc.top = m_lstRenderContent[m_uCurrContent].m_offset.y;
				rc.bottom = rc.top + m_lstRenderContent[m_uCurrContent].m_size.y;

				HWND target = m_lstRenderContent[m_uCurrContent].m_hwnd;
				RECT* target_rc = &rc;
				// EditorMode
				if ( FullScreenMode() )
				{
					target = NULL;
					target_rc = NULL;
				}

				if (FAILED((hReturn = m_pd3d9Device->Present(NULL, target_rc, target, NULL))))
				{
					if( D3DERR_DEVICELOST == hReturn )
					{
						m_bDeviceLost = true;
					}
					else if( D3DERR_DRIVERINTERNALERROR == hReturn )
					{
						m_bDeviceLost = true;
					}
				}
			}	
		}
	}


	gEnv->pProfiler->setEndCommit();
	updateGpuTimers();

	ms_FrameCount++;
	ms_FrameCount %= 100000;
}
//-----------------------------------------------------------------------
void gkRendererD3D9::_clearBuffer(bool clearDepth, DWORD color)
{
	if (clearDepth)
	{
		m_pd3d9Device->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL , color, 1.0f, 0L );
		//SetRenderState(D3DRS_STENCILENABLE, FALSE);
	}
	else
		m_pd3d9Device->Clear( 0, NULL, D3DCLEAR_TARGET , color, 1.0f, 0 );
}

//////////////////////////////////////////////////////////////////////////
void gkRendererD3D9::_clear2Buffer( bool clearDepth, DWORD color /*= 0x00000000*/ )
{
	if (clearDepth)
	{
		m_pd3d9Device->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL , color, 1.0f, 0L );
		//SetRenderState(D3DRS_STENCILENABLE, FALSE);
	}
	else
		m_pd3d9Device->Clear( 0, NULL, D3DCLEAR_TARGET , color, 1.0f, 0 );
}


void gkRendererD3D9::_render(const gkRenderOperation& op, bool isShadowPass)
{
	switch (op.operationType)
	{
	case gkRenderOperation::OT_TRIANGLE_LIST:
		{
			if (!op.vertexData)
				return;
			if (op.useIndexes && !op.indexData)
				return;

			// 这里的size？是vertex的？ [4/8/2011 Kaiming-Desktop]
			switch(op.vertexData->vertexType)
			{
			case eVI_P4F4F4:
				m_pd3d9Device->SetVertexDeclaration(m_generalDeclP4F4F4);
				break;
			case eVI_P4F4F4F4U4:
				m_pd3d9Device->SetVertexDeclaration(m_generalDeclP4F4F4F4U4);
				break;
			case eVI_T2T2:
				m_pd3d9Device->SetVertexDeclaration(m_generalDeclT2T2);
				break;
			case eVI_P3T2U4:
				m_pd3d9Device->SetVertexDeclaration(m_generalDeclP3T2U4);
				break;
			case eVI_PT2T2T2T2T2:
				m_pd3d9Device->SetVertexDeclaration(m_generalDeclPt2T2T2T2T2);
				break;
			default:
				return;
			}

			m_pd3d9Device->SetStreamSource(0, (IDirect3DVertexBuffer9*)(op.vertexData->userData), op.vertexStart, op.vertexSize);


			if (op.useIndexes)
			{
				m_pd3d9Device->SetIndices((IDirect3DIndexBuffer9*)(op.indexData->userData));
				m_pd3d9Device->DrawIndexedPrimitive(
					D3DPT_TRIANGLELIST, 
					op.vertexStart, 
					0, // Min vertex index - assume we can go right down to 0 
					op.vertexCount, 
					op.indexStart, 
					op.indexCount / 3
					);

				gEnv->pProfiler->increaseElementCount(ePe_Triangle_Total, op.indexCount / 3);
				if (isShadowPass)
				{
					gEnv->pProfiler->increaseElementCount(ePe_Triangle_Shadow, op.indexCount / 3);
				}
			}
			else
			{
				m_pd3d9Device->DrawPrimitive(D3DPT_TRIANGLELIST, op.vertexStart, op.vertexCount / 3);
				gEnv->pProfiler->increaseElementCount(ePe_Triangle_Total, op.vertexCount / 3);
				if (isShadowPass)
				{
					gEnv->pProfiler->increaseElementCount(ePe_Triangle_Shadow, op.vertexCount / 3);
				}
			}
			gEnv->pProfiler->increaseElementCount(ePe_Batch_Total);
			if (isShadowPass)
				gEnv->pProfiler->increaseElementCount(ePe_Batch_Shadow);
		}
		break;

	case gkRenderOperation::OT_TRIANGLE_STRIP:
		{
			if (!op.vertexData)
				return;
			if (op.useIndexes && !op.indexData)
				return;

			// 这里的size？是vertex的？ [4/8/2011 Kaiming-Desktop]
			m_pd3d9Device->SetVertexDeclaration(op.vertexDecl);
			m_pd3d9Device->SetStreamSource(0, (IDirect3DVertexBuffer9*)(op.vertexData->userData), 0, op.vertexSize);


			if (op.useIndexes)
			{
				m_pd3d9Device->SetIndices((IDirect3DIndexBuffer9*)(op.indexData->userData));
				m_pd3d9Device->DrawIndexedPrimitive(
					D3DPT_TRIANGLESTRIP, 
					op.vertexStart, 
					0, // Min vertex index - assume we can go right down to 0 
					op.vertexCount, 
					op.indexStart, 
					op.indexCount
					);
			}
			else
			{
				m_pd3d9Device->DrawPrimitive(D3DPT_TRIANGLESTRIP, op.vertexStart, op.vertexCount / 3);
			}
			gEnv->pProfiler->increaseElementCount(ePe_Batch_Total);
			if (isShadowPass)
				gEnv->pProfiler->increaseElementCount(ePe_Batch_Shadow);
		}
		break;
	}	
}
//-----------------------------------------------------------------------
UINT gkRendererD3D9::GetScreenHeight(bool forceOrigin)
{
	if(m_use_override && !forceOrigin)
	{
		return m_override_height;
	}
	else
	{
		return getBackBufferDesc()->Height;
	}
}
//-----------------------------------------------------------------------
UINT gkRendererD3D9::GetScreenWidth(bool forceOrigin)
{
	if(m_use_override && !forceOrigin)
	{
		return m_override_width;
	}
	else
	{
		return getBackBufferDesc()->Width;
	}
}


//--------------------------------------------------------------------------------------
// Returns true if a particular depth-stencil format can be created and used with
// an adapter format and backbuffer format combination.
BOOL IsDepthFormatOk( D3DFORMAT DepthFormat,
	D3DFORMAT AdapterFormat,
	D3DFORMAT BackBufferFormat )
{
	// Verify that the depth format exists
	HRESULT hr = DXUTGetD3D9Object()->CheckDeviceFormat( D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		AdapterFormat,
		D3DUSAGE_DEPTHSTENCIL,
		D3DRTYPE_SURFACE,
		DepthFormat );
	if( FAILED( hr ) ) return FALSE;

	// Verify that the backbuffer format is valid
	hr = DXUTGetD3D9Object()->CheckDeviceFormat( D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		AdapterFormat,
		D3DUSAGE_RENDERTARGET,
		D3DRTYPE_SURFACE,
		BackBufferFormat );
	if( FAILED( hr ) ) return FALSE;

	// Verify that the depth format is compatible
	hr = DXUTGetD3D9Object()->CheckDepthStencilMatch( D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		AdapterFormat,
		BackBufferFormat,
		DepthFormat );

	return SUCCEEDED( hr );
}

//--------------------------------------------------------------------------------------
// Before a device is created, modify the device settings as needed
//--------------------------------------------------------------------------------------
bool CALLBACK gkRendererD3D9::ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, void* pUserContext )
{
	if( pDeviceSettings->ver == DXUT_D3D9_DEVICE )
	{
		// Turn vsync off
		pDeviceSettings->d3d9.pp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

		IDirect3D9* pD3D = DXUTGetD3D9Object();
		D3DCAPS9 Caps;
		pD3D->GetDeviceCaps( pDeviceSettings->d3d9.AdapterOrdinal, pDeviceSettings->d3d9.DeviceType, &Caps );

		pDeviceSettings->d3d9.BehaviorFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING;
		// If device doesn't support HW T&L or doesn't support 1.1 vertex shaders in HW 
		// then switch to SWVP.
		if( ( Caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT ) == 0 ||
			Caps.VertexShaderVersion < D3DVS_VERSION( 1, 1 ) )
		{
			pDeviceSettings->d3d9.BehaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
		}
		//pDeviceSettings->d3d9.BehaviorFlags |= D3DCREATE_FPU_PRESERVE;
		pDeviceSettings->d3d9.BehaviorFlags |= D3DCREATE_MULTITHREADED;
		//pDeviceSettings->d3d9.BehaviorFlags |= D3DCREATE_PUREDEVICE;
		//pDeviceSettings->d3d9.BehaviorFlags |= D3DCREATE_DISABLE_DRIVER_MANAGEMENT;

		// Debugging vertex shaders requires either REF or software vertex processing 
		// and debugging pixel shaders requires REF.  
#ifdef DEBUG_VS
		if( pDeviceSettings->d3d9.DeviceType != D3DDEVTYPE_REF )
		{
			pDeviceSettings->d3d9.BehaviorFlags &= ~D3DCREATE_HARDWARE_VERTEXPROCESSING;
			pDeviceSettings->d3d9.BehaviorFlags &= ~D3DCREATE_PUREDEVICE;
			pDeviceSettings->d3d9.BehaviorFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
		}
#endif
#ifdef DEBUG_PS
		pDeviceSettings->d3d9.DeviceType = D3DDEVTYPE_REF;
#endif
	}

	// This sample requires a stencil buffer.
	if( IsDepthFormatOk( D3DFMT_D24S8,
		pDeviceSettings->d3d9.AdapterFormat,
		pDeviceSettings->d3d9.pp.BackBufferFormat ) )
		pDeviceSettings->d3d9.pp.AutoDepthStencilFormat = D3DFMT_D24S8;
	else if( IsDepthFormatOk( D3DFMT_D24X4S4,
		pDeviceSettings->d3d9.AdapterFormat,
		pDeviceSettings->d3d9.pp.BackBufferFormat ) )
		pDeviceSettings->d3d9.pp.AutoDepthStencilFormat = D3DFMT_D24X4S4;
	else if( IsDepthFormatOk( D3DFMT_D24FS8,
		pDeviceSettings->d3d9.AdapterFormat,
		pDeviceSettings->d3d9.pp.BackBufferFormat ) )
		pDeviceSettings->d3d9.pp.AutoDepthStencilFormat = D3DFMT_D24FS8;
	else if( IsDepthFormatOk( D3DFMT_D15S1,
		pDeviceSettings->d3d9.AdapterFormat,
		pDeviceSettings->d3d9.pp.BackBufferFormat ) )
		pDeviceSettings->d3d9.pp.AutoDepthStencilFormat = D3DFMT_D15S1;

	// For the first device created if its a REF device, optionally display a warning dialog box
	static bool s_bFirstTime = true;
	if( s_bFirstTime )
	{
		s_bFirstTime = false;
		// 		if( ( DXUT_D3D9_DEVICE == pDeviceSettings->ver && pDeviceSettings->d3d9.DeviceType == D3DDEVTYPE_REF ) ||
		// 			( DXUT_D3D10_DEVICE == pDeviceSettings->ver &&
		// 			pDeviceSettings->d3d10.DriverType == D3D10_DRIVER_TYPE_REFERENCE ) )
		//DXUTDisplaySwitchingToREFWarning( pDeviceSettings->ver );
	}
	return true;
}

//class IDirect3D9EX;

typedef IDirect3D9* ( WINAPI* FNDIRECT3DCREATE9 )( UINT );
typedef HRESULT ( WINAPI* FNDIRECT3DCREATE9EX )(UINT SDKVersion,IDirect3D9Ex **ppD3D);


HWND gkRendererD3D9::Init(ISystemInitInfo& sii)
{

	m_hInst = sii.hInstance;
	if (m_hInst != 0)
	{
		m_IsEditor = true;
	}

	m_pRendererCVar = new gkRendererCVars;
	g_pRendererCVars = m_pRendererCVar;

	// init pipelines
	m_pipelines.assign( RP_Count, NULL );
	m_pipelines[RP_ShadowMapGen] = new gkRendPipe_ShadowMapGen();
	m_pipelines[RP_ZpassDeferredLighting] = new RendPipe_ZpassDeferredLighting();
	m_pipelines[RP_ShadingPassDeferredLighting] = new RendPipe_ShadingPassDeferredLighting();
	m_pipelines[RP_ZpassDeferredShading] = new RendPipe_ZpassDeferredShading();
	m_pipelines[RP_ShadingPassDeferredShading] = new RendPipe_ShadingPassDeferredShading();
	m_pipelines[RP_ReflMapGen] = new gkRendPipe_ReflGen();
	m_pipelines[RP_OcculusionGen] = new gkRendPipe_OcculusionGen();
	m_pipelines[RP_DeferredLight] = new RendPipe_LightPasses();
	m_pipelines[RP_SSRL] = new RendPipe_SSRL();
	m_pipelines[RP_DeferredFog] = new RendPipe_DeferredFog();


	// init Resource Managers
	m_pTextureManager =			new gkTextureManager();
	m_pShaderManager =			new gkShaderManager();
	m_pMaterialManager =		new gkMaterialManager();
	m_pMeshManager =			new gkMeshManager();

	m_pColorGradingController = new gkColorGradingController();

	buildGpuTimers();

	m_pDefaultFont = NULL;

	SetWindow(m_lNewWidth, m_lNewHeight, 0, 0);

	m_pLightProbeSystem = new gkLightProbeSystem();
	m_pLightProbeSystem->Init();

	if(USE_DXUT)
	{
		// Initial thing should before here [8/10/2011 Kaiming-Desktop]
		DXUTSetCallbackD3D9DeviceCreated( OnD3D9CreateDevice );
		DXUTSetCallbackD3D9DeviceDestroyed( OnD3D9DestroyDevice );
		DXUTSetCallbackDeviceChanging( ModifyDeviceSettings );
		DXUTSetCallbackD3D9DeviceReset( OnD3D9ResetDevice );
		DXUTSetCallbackD3D9DeviceLost( OnD3D9LostDevice );


		DXUTInit(true, true);

		DXUTSetWindow(m_hWnd, m_hWnd, m_hWnd, false);

		// set a default render size [7/24/2011 Kaiming-Desktop]
		DXUTCreateDevice(true, sii.fWidth, sii.fHeight);
		m_pd3d9Device = DXUTGetD3D9Device();
	}
	else
	{


		// This may fail if Direct3D 9 isn't installed
		HMODULE d3d9dll = LoadLibrary( _T("d3d9.dll") );
		if( d3d9dll != NULL )
		{
			FNDIRECT3DCREATE9 s_DynamicDirect3DCreate9 = ( FNDIRECT3DCREATE9 )GetProcAddress( d3d9dll, "Direct3DCreate9" );
			FNDIRECT3DCREATE9EX s_DynamicDirect3DCreate9Ex = ( FNDIRECT3DCREATE9EX )GetProcAddress( d3d9dll, "Direct3DCreate9Ex" );

			//IDirect3D9* pD3D = s_DynamicDirect3DCreate9( D3D_SDK_VERSION );
			IDirect3D9Ex* pD3DEx;
			s_DynamicDirect3DCreate9Ex( D3D_SDK_VERSION, &pD3DEx );

			int adapter_oculus = D3DADAPTER_DEFAULT;

			m_adapterFullscreenSize = Vec2i(1280,720);

			for (unsigned Adapter=0; Adapter < pD3DEx->GetAdapterCount(); Adapter++)
			{
				MONITORINFOEX Monitor;
				Monitor.cbSize = sizeof(Monitor);

				if (::GetMonitorInfo(pD3DEx->GetAdapterMonitor(Adapter), &Monitor) && Monitor.szDevice[0])
				{
					DISPLAY_DEVICE DispDev;
					memset(&DispDev, 0, sizeof(DispDev));
					DispDev.cb = sizeof(DispDev);
					if (::EnumDisplayDevices(Monitor.szDevice, 0, &DispDev, 0))
					{
						if (Adapter == 0)
						{
							m_adapterFullscreenSize = Vec2i(Monitor.rcMonitor.right - Monitor.rcMonitor.left, Monitor.rcMonitor.bottom - Monitor.rcMonitor.top);
						}

						if (_tcsstr(DispDev.DeviceID, _T("OVR")) )
						{
							adapter_oculus = Adapter;
							m_adapterFullscreenSize = Vec2i(Monitor.rcMonitor.right - Monitor.rcMonitor.left, Monitor.rcMonitor.bottom - Monitor.rcMonitor.top);
						}
					}
				}
			}



			D3DPRESENT_PARAMETERS pp;
			memset( &pp, 0, sizeof(pp));

			pp.BackBufferWidth = sii.fWidth;
			pp.BackBufferHeight = sii.fHeight;
			pp.BackBufferCount = 0;
			pp.BackBufferFormat = D3DFMT_A8R8G8B8;

			pp.AutoDepthStencilFormat = D3DFMT_D24S8;
			pp.EnableAutoDepthStencil = TRUE;

			pp.hDeviceWindow = m_hWnd;
			pp.Windowed = TRUE;

			pp.SwapEffect = D3DSWAPEFFECT_DISCARD;

			pp.MultiSampleType = D3DMULTISAMPLE_NONE;
			pp.MultiSampleQuality = 0;

			pp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

			pp.Flags |= D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;


			// 			if( FAILED( pD3D->CreateDevice( 0, D3DDEVTYPE_HAL, m_hWnd, D3DCREATE_MULTITHREADED | D3DCREATE_HARDWARE_VERTEXPROCESSING, &pp ,&m_pd3d9Device )))
			// 			{
			// 				MessageBox( NULL, _T("Create Device Failed."), _T("Fatal Error"), MB_OK );
			// 				return NULL;
			// 			}

			if( FAILED( pD3DEx->CreateDevice( adapter_oculus, D3DDEVTYPE_HAL, m_hWnd, D3DCREATE_MULTITHREADED | D3DCREATE_HARDWARE_VERTEXPROCESSING, &pp ,&m_pd3d9Device )))
			{
				MessageBox( NULL, _T("Create Device Failed."), _T("Fatal Error"), MB_OK );
				return NULL;
			}

			// 			IDirect3DDevice9Ex* m_pd3d9DeviceEx;
			// 			pD3DEx->CreateDeviceEx(0, D3DDEVTYPE_HAL, m_hWnd, D3DCREATE_MULTITHREADED | D3DCREATE_HARDWARE_VERTEXPROCESSING, &pp, ,&m_pd3d9DeviceEx);

			D3DSURFACE_DESC desc;

			IDirect3DSurface9* bb;
			m_pd3d9Device->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &bb);

			bb->GetDesc( &desc );

#if USE_SWAPCHAIN
			((IDirect3DDevice9Ex*)m_pd3d9Device)->CreateAdditionalSwapChain( &pp, &m_pSwapChain1 );
#endif
			OnD3D9CreateDevice( m_pd3d9Device, &desc, NULL );
			OnD3D9ResetDevice( m_pd3d9Device, &desc, NULL );
		}


	}

	GK_ASSERT( m_pd3d9Device );

	//InitGpuInfo();

	// post create [10/20/2011 Kaiming]
	return m_hWnd;
}
//-----------------------------------------------------------------------
void gkRendererD3D9::Destroy()
{
	// flush
	//RT_EndRender();

	// terminal
	m_tdRenderThread.Terminate();

	m_pLightProbeSystem->Destroy();
	delete m_pLightProbeSystem;


	SAFE_DELETE( m_pColorGradingController );


	DXUTShutdown();

	SAFE_DELETE( m_pTextureManager );
	SAFE_DELETE( m_pMaterialManager );
	SAFE_DELETE( m_pMeshManager );
	SAFE_DELETE( m_pShaderManager );
	SAFE_DELETE( m_pRendererCVar ); 


	for (uint32 i=0; i < m_pipelines.size(); ++i)
	{
		if (m_pipelines[i])
		{
			delete m_pipelines[i];
		}
	}
}


void gkRendererD3D9::OnD3D9ResetExDevice(LPDIRECT3DDEVICE9 pd3dDevice)
{
	static_cast<gkTextureManager*>(m_pTextureManager)->resizeall();
}

//-----------------------------------------------------------------------
HRESULT CALLBACK gkRendererD3D9::OnD3D9ResetDevice( LPDIRECT3DDEVICE9 pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
	m_pd3d9Device = pd3dDevice;

	// that is backbuffer
	pd3dDevice->GetRenderTarget(0, &m_pBackBufferSurface);

#if USE_SWAPCHAIN
	m_pSwapChain1->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &m_pBackBufferSurface );
#endif

	pd3dDevice->GetDepthStencilSurface( &m_pOriginDepthSurface );

	gkDSCaches::iterator it = m_dsCaches.begin();
	for ( ; it != m_dsCaches.end(); ++it )
	{
		D3DSURFACE_DESC pOldDSDesc;
		m_pOriginDepthSurface->GetDesc(&pOldDSDesc);

		gkRenderTargetSize size = it->first;

		pd3dDevice->CreateDepthStencilSurface(size.width,
			size.height,
			D3DFMT_D24S8,
			D3DMULTISAMPLE_NONE,
			0,
			TRUE,
			&(it->second),
			NULL );
	}

	if( m_pStateManager )
		m_pStateManager->DirtyCachedValues();

	static_cast<gkShaderManager*>(m_pShaderManager)->resetAll();
	static_cast<gkTextureManager*>(m_pTextureManager)->resetAll();
	static_cast<gkMeshManager*>(m_pMeshManager)->resetAll();

	pd3dDevice->CreateVertexDeclaration( gkVIDeclPt4F2, &m_generalDeclPt4F2 );
	pd3dDevice->CreateVertexDeclaration( gkVIDeclPt4F4, &m_generalDeclPt4F4);
	pd3dDevice->CreateVertexDeclaration( gkVIDeclP4F4, &m_generalDeclP4F4);
	pd3dDevice->CreateVertexDeclaration( gkVertexDeclP4F4F4, &m_generalDeclP4F4F4);
	pd3dDevice->CreateVertexDeclaration( gkVertexDeclP4F4F4F4U4, &m_generalDeclP4F4F4F4U4);
	pd3dDevice->CreateVertexDeclaration( gkVertexDeclP3T2U4, &m_generalDeclP3T2U4);
	pd3dDevice->CreateVertexDeclaration( gkVIDeclPt2T2T2T2T2, &m_generalDeclPt2T2T2T2T2);


	pd3dDevice->CreateVertexDeclaration( gkVIDeclT2T2 , &m_generalDeclT2T2 );

	pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);

	initGpuTimers(pd3dDevice);

	ICamera* cam = gEnv->p3DEngine->getMainCamera();
	if (cam)
	{
		float aspect = (float)(pBackBufferSurfaceDesc->Width) / (float)(pBackBufferSurfaceDesc->Height);
		cam->setAspect(aspect);
	}

	gEnv->pRenderer->RT_SkipOneFrame();
	return S_OK;
}
//-----------------------------------------------------------------------
void CALLBACK gkRendererD3D9::OnD3D9LostDevice( void* pUserContext )
{
	SAFE_RELEASE( m_generalDeclPt4F2 );
	SAFE_RELEASE( m_generalDeclPt4F4 );
	SAFE_RELEASE( m_generalDeclP4F4 );
	SAFE_RELEASE( m_generalDeclP4F4F4 );
	SAFE_RELEASE( m_generalDeclP4F4F4F4U4 );

	SAFE_RELEASE( m_generalDeclT2T2 );
	SAFE_RELEASE( m_generalDeclP3T2U4 );

	SAFE_RELEASE( m_pBackBufferSurface );
	SAFE_RELEASE( m_pOriginDepthSurface );

	SAFE_RELEASE( m_generalDeclPt2T2T2T2T2 );

	gkDSCaches::iterator it = m_dsCaches.begin();
	for ( ; it != m_dsCaches.end(); ++it )
	{
		SAFE_RELEASE( it->second );
	}

	static_cast<gkShaderManager*>(m_pShaderManager)->lostAll();

	static_cast<gkTextureManager*>(m_pTextureManager)->lostAll();

	static_cast<gkMeshManager*>(m_pMeshManager)->lostAll();

	destoryGpuTimers();

	getRenderer()->FX_ClearAllSampler();

	m_pd3d9Device->EvictManagedResources();
}
//-----------------------------------------------------------------------
HRESULT CALLBACK gkRendererD3D9::OnD3D9CreateDevice( LPDIRECT3DDEVICE9 pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
	m_pUpdatingRenderSequence = new gkRenderSequence();
	m_pRenderingRenderSequence = new gkRenderSequence();

	m_pd3d9Device = pd3dDevice;
	// basic staffs [8/1/2011 Kaiming-Desktop]
	D3DXCreateEffectPool( &m_pd3dEffectPool );

	gkPostProcessManager::OnCreateDevice(pd3dDevice);

	m_pStateManager = gkStateManager::Create(pd3dDevice);

	m_pTextureManager->reloadAll();
	m_pMaterialManager->reloadAll();
	m_pMeshManager->reloadAll();
	m_pShaderManager->reloadAll();

	return S_OK;
}
//-----------------------------------------------------------------------
void    CALLBACK gkRendererD3D9::OnD3D9DestroyDevice( void* pUserContext )
{

	SAFE_RELEASE( m_pd3dEffectPool );

	m_pMaterialManager->unloadAll();
	m_pMeshManager->unloadAll();
	m_pShaderManager->unloadAll();
	m_pTextureManager->unloadAll();

	gkPostProcessManager::OnDestroyDevice();

	SAFE_RELEASE( m_pStateManager );

	SAFE_DELETE( m_pUpdatingRenderSequence );
	SAFE_DELETE( m_pRenderingRenderSequence );
}
//-----------------------------------------------------------------------
IResourceManager* gkRendererD3D9::getResourceManager( BYTE type )
{
	switch (type)
	{
	case GK_RESOURCE_MANAGER_TYPE_TEXTURE:
		return m_pTextureManager;
	case GK_RESOURCE_MANAGER_TYPE_MATERIAL:
		return m_pMaterialManager;
	case GK_RESOURCE_MANAGER_TYPE_MESH:
		return m_pMeshManager;
	case GK_RESOURCE_MANAGER_TYPE_SHADER:
		return m_pShaderManager;
	}

	GK_ASSERT(0);
	return NULL;
}
//-----------------------------------------------------------------------
const D3DSURFACE_DESC* gkRendererD3D9::getBackBufferDesc()
{
	if(USE_DXUT)
	{
		return DXUTGetD3D9BackBufferSurfaceDesc();
	}
	else
	{
		IDirect3DSurface9* bb;
		m_pd3d9Device->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &bb);

		bb->GetDesc( &m_bb_desc );

		return &m_bb_desc;
	}

}
//-----------------------------------------------------------------------
void gkRendererD3D9::RS_EnableAlphaTest( bool enable, DWORD ref )
{
	if(!m_pd3d9Device) return;
	if (enable)
	{
		SetRenderState(D3DRS_ALPHAREF, ref);
		SetRenderState(D3DRS_ALPHATESTENABLE, TRUE); 
		SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
	}
	else
	{
		SetRenderState(D3DRS_ALPHATESTENABLE, FALSE); 
	}
}

bool gkRendererD3D9::SetWindow(int width, int height, bool fullscreen, HWND hWnd)
{
#ifdef WIN32
	if (m_IsEditor)
	{

		HWND temp = GetDesktopWindow();
		RECT trect;
		GetWindowRect(temp, &trect);
		//m_singleMonitorWidth =trect.right-trect.left;
		//m_singleMonitorHeight=trect.bottom-trect.top;  

		RECT rc;
		HDC hdc = GetDC(NULL);
		GetClipBox(hdc, &rc);
		ReleaseDC(NULL, hdc);
		//m_deskwidth = rc.right  - rc.left;
		//m_deskheight = rc.bottom - rc.top;

		DWORD style, exstyle;
		int x, y, wdt, hgt;

		if (width < 640)
			width = 640;
		if (height < 480)
			height = 480;

		m_dwWindowStyle = WS_OVERLAPPEDWINDOW | WS_VISIBLE;

		// Do not allow the user to resize the window
		m_dwWindowStyle &= ~WS_MAXIMIZEBOX;
		m_dwWindowStyle &= ~WS_THICKFRAME;

		if (fullscreen)
		{
			exstyle = WS_EX_TOPMOST;
			style = WS_POPUP | WS_VISIBLE;
			x = (GetSystemMetrics(SM_CXFULLSCREEN)-width)/2;
			y = (GetSystemMetrics(SM_CYFULLSCREEN)-height)/2;
			wdt = width;
			hgt = height;
		}
		else
		{
			exstyle = WS_EX_APPWINDOW;
			style = m_dwWindowStyle;
			x = (GetSystemMetrics(SM_CXFULLSCREEN)-width)/2;
			y = (GetSystemMetrics(SM_CYFULLSCREEN)-height)/2;
			wdt = GetSystemMetrics(SM_CXFIXEDFRAME)*2 + width;
			hgt = GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYFIXEDFRAME)*2 + height;
		}


		m_dwWindowStyle = WS_OVERLAPPED;
		style = m_dwWindowStyle;
		exstyle = 0;
		x = y = 0;
		wdt = 100;
		hgt = 100;

		WNDCLASS wc;
		memset(&wc, 0, sizeof(WNDCLASS));
		wc.style         = CS_DBLCLKS;
		wc.lpfnWndProc   = gkStudioFSWndProc;
		wc.hInstance     = m_hInst;
		//    wc.hbrBackground =(HBRUSH)GetStockObject(BLACK_BRUSH);
		wc.lpszMenuName  = 0;
		wc.lpszClassName = _T("D3DDeviceWindowClassForSandbox");
		if (!RegisterClass(&wc))
		{
			OutputDebugString( _T("Cannot Register Window Class.") );
			return false;
		}
		m_hWnd = CreateWindowEx(exstyle, wc.lpszClassName, _T("gkENGINE"), WS_OVERLAPPEDWINDOW, x, y, wdt, hgt, NULL,NULL, m_hInst, NULL);
		ShowWindow( m_hWnd, SW_HIDE );
	}
	else
	{
		if (!hWnd)
		{
			HINSTANCE hInstance = NULL;
			if( hInstance == NULL )
				hInstance = ( HINSTANCE )GetModuleHandle( NULL );

			HICON hIcon = NULL;

			// Register the windows class
			WNDCLASS wndClass;
			wndClass.style = CS_DBLCLKS;
			if(USE_DXUT)
			{
				wndClass.lpfnWndProc = DXUTStaticWndProc;
			}
			else
			{
				wndClass.lpfnWndProc = gkENGINEStaticWndProc;
			}

			wndClass.cbClsExtra = 0;
			wndClass.cbWndExtra = 0;
			wndClass.hInstance = hInstance;
			wndClass.hIcon = hIcon;
			wndClass.hCursor = LoadCursor( NULL, IDC_ARROW );
			wndClass.hbrBackground = ( HBRUSH )GetStockObject( BLACK_BRUSH );
			wndClass.lpszMenuName = NULL;
			wndClass.lpszClassName = _T("Direct3DWindowClass");

			if (!RegisterClass(&wndClass))
			{
				DWORD dwError = GetLastError();
				if( dwError != ERROR_CLASS_ALREADY_EXISTS )
					OutputDebugString( _T("Cannot Register Window Class.") );
				return false;
			}

			// Create the render window
			m_hWnd = CreateWindow( _T("Direct3DWindowClass"), _T("gkENGINE"), WS_OVERLAPPEDWINDOW,
				20, 20, width, height, 0,
				NULL, hInstance, 0 );
			//m_hWnd = CreateWindowEx(exstyle, _T("gkENGINE"), _T("gkENGINE"),style,x,y,wdt,hgt, NULL, NULL, hInstance, NULL);

			RECT realRect;
			GetClientRect(m_hWnd, &realRect);

			int realwidth = realRect.right - realRect.left;
			int realheight = realRect.bottom - realRect.top;
			width = width * 2 - realwidth;
			height = height * 2 - realheight;

			MoveWindow(m_hWnd, GetSystemMetrics(SM_CXSCREEN) / 2 - width / 2, GetSystemMetrics(SM_CYSCREEN) / 2 - height / 2, width, height, FALSE);

		}
		else
			m_hWnd = (HWND)hWnd;


		//ShowWindow(m_hWnd, SW_SHOWNORMAL);
		ShowWindow(m_hWnd, SW_HIDE);
		SetFocus(m_hWnd);
		SetForegroundWindow(m_hWnd);
	}

#endif //WIN32
	return true;
}
//-----------------------------------------------------------------------
void gkRendererD3D9::SetCurrContent( HWND hWnd, uint32 posx, uint32 posy, uint32 width, uint32 height, bool fullscreen )
{
	bool FakeFullScreen = true;
	if (fullscreen && FakeFullScreen)
	{
		posx = 0;
		posy = 0;
		width = m_adapterFullscreenSize.x;
		height = m_adapterFullscreenSize.y;
	}

	if( hWnd == NULL)
	{
		hWnd = m_lstRenderContent[m_uCurrContent].m_hwnd;
		posx = m_lstRenderContent[m_uCurrContent].m_offset.x;
		posy = m_lstRenderContent[m_uCurrContent].m_offset.y;
		width = m_lstRenderContent[m_uCurrContent].m_size.x;
		height = m_lstRenderContent[m_uCurrContent].m_size.y;
		//fullscreen = m_lstRenderContent[m_uCurrContent].m_fullscreen;
	}


	for (uint8 i =0; i < m_lstRenderContent.size(); ++i)
	{
		if (m_lstRenderContent[i].m_hwnd == hWnd)
		{
			m_uCurrContent = i;

			if (m_lstRenderContent[i].m_size.x == width
				&& m_lstRenderContent[i].m_size.y == height
				&& m_lstRenderContent[i].m_offset.x == posx
				&& m_lstRenderContent[i].m_offset.y == posy
				&& m_lstRenderContent[i].m_fullscreen == fullscreen)
			{
				m_bIsSizeChange = true;
				m_lNewWidth = width;
				m_lNewHeight = height;
				m_bFullScreen = fullscreen;
				m_bIsSizeChange = true;
				return;
			}

			m_lstRenderContent[i].m_offset.x = posx;
			m_lstRenderContent[i].m_offset.y = posy;
			m_lstRenderContent[i].m_size.x = width;
			m_lstRenderContent[i].m_size.y = height;
			m_lstRenderContent[i].m_fullscreen = fullscreen;

			m_bIsSizeChange = true;
			m_lNewWidth = width;
			m_lNewHeight = height;
			m_bFullScreen = fullscreen;
			return;
		}
	}

	// go here, not found, so create one
	gkRenderContent renderContent;
	renderContent.m_hwnd = hWnd;
	renderContent.m_offset.x = posx;
	renderContent.m_offset.y = posy;
	renderContent.m_size.x = width;
	renderContent.m_size.y = height;
	renderContent.m_fullscreen = fullscreen;

	m_lstRenderContent.push_back(renderContent);
	m_uCurrContent = m_lstRenderContent.size() - 1;

	m_bIsSizeChange = true;
	m_lNewWidth = width;
	m_lNewHeight = height;
	m_bFullScreen = fullscreen;
}
//-----------------------------------------------------------------------
IAuxRenderer* gkRendererD3D9::getAuxRenderer()
{
	return m_pAuxRenderer;
}


//-----------------------------------------------------------------------
void gkRendererD3D9::RC_SetMainCamera( const CCamera* cam )
{
	m_pShaderParamDataSource.setMainCamera(cam);
}

void gkRendererD3D9::RC_SetShadowCascadeCamera( const CCamera* cam, uint8 index )
{
	m_pShaderParamDataSource.setShadowCascadeCamera(cam, index);
}

//-----------------------------------------------------------------------
void gkRendererD3D9::RC_SetSunDir( const Vec3& lightdir )
{
	m_pShaderParamDataSource.setLightDir(lightdir);
}

void gkRendererD3D9::RS_SetRenderState( D3DRENDERSTATETYPE rs, DWORD value )
{
	SetRenderState((D3DRENDERSTATETYPE)rs, value);
}

void gkRendererD3D9::debugDynTexs()
{
	Vec4 region;
	if (g_pRendererCVars->r_DebugDynTexType == 3)
	{
		region = Vec4(0,0,0.2f,0.2f);
		//FX_DrawDynTextures(gkTextureManager::ms_SceneTarget0, region);

		region = Vec4(0.2f,0,0.4f,0.2f);
		FX_DrawDynTextures(gkTextureManager::ms_SceneNormal, region);

		region = Vec4(0.4f,0,0.6f,0.2f);
		FX_DrawFloatTextures(gkTextureManager::ms_SceneDepth, region);

		region = Vec4(0.6f,0,0.8f,0.2f);
		FX_DrawDynTextures(gkTextureManager::ms_SceneTargetBlur, region);

		region = Vec4(0.0f,0.2,0.4f,0.6f);
		FX_DrawDynTextures(gkTextureManager::ms_SceneDifAcc, region);

		region = Vec4(0.4f,0.2f,0.8f,0.6f);
		FX_DrawDynTextures(gkTextureManager::ms_SceneSpecAcc, region);

		region = Vec4(0.2f, 0.6f, 0.6f,1.0f);
		FX_DrawDynTextures(gkTextureManager::ms_ReflMap0, region);

		region = Vec4(0.6f, 0.6f, 1.0f, 1.0f);
		FX_DrawDynTextures(gkTextureManager::ms_SceneAlbeto, region);
	}
	else if(g_pRendererCVars->r_DebugDynTexType == 2)
	{
		region = Vec4(0,0,0.2f,0.2f);
		FX_DrawDynTextures(gkTextureManager::ms_SceneTarget0, region);

		region = Vec4(0.2f,0,0.4f,0.2f);
		//FX_DrawDynTextures(gkTextureManager::ms_HDRTargetScaledHalf, region);

		region = Vec4(0.4f,0,0.6f,0.2f);
		FX_DrawDynTextures(gkTextureManager::ms_HDRTargetScaledQuad, region);

		region = Vec4(0.6f,0,0.8f,0.2f);
		FX_DrawFloatTextures(gkTextureManager::ms_HDRToneMap64, region);

		region = Vec4(0.8f,0,1.0f,0.2f);
		FX_DrawFloatTextures(gkTextureManager::ms_HDRToneMap16, region);

		region = Vec4(0.0f,0.2f,0.2f,0.4f);
		FX_DrawFloatTextures(gkTextureManager::ms_HDRToneMap01, region);

		region = Vec4(0.2f,0.2f,0.4f,0.4f);
		FX_DrawDynTextures(gkTextureManager::ms_HDRBrightPassQuad, region);

		region = Vec4(0.4f,0.2f,0.6f,0.4f);
		FX_DrawDynTextures(gkTextureManager::ms_HDRBrightPassQuadQuad, region);

		region = Vec4(0.6f,0.2f,0.8f,0.4f);
		FX_DrawDynTextures(gkTextureManager::ms_HDRBrightPassQuadQuadQuad, region);

		region = Vec4(0.8f,0.2f,1.0f,0.4f);
		FX_DrawDynTextures(gkTextureManager::ms_HDRBloom0Quad, region);

		region = Vec4(0.0f,0.4f,0.2f,0.6f);
		FX_DrawDynTextures(gkTextureManager::ms_HDRTarget0, region);	
		// 
		region = Vec4(0.2f,0.4f,0.3f,0.5f);
		FX_DrawFloatTextures(gkTextureManager::ms_HDREyeAdaption[0], region);	
		region = Vec4(0.3f,0.4f,0.4f,0.5f);
		FX_DrawFloatTextures(gkTextureManager::ms_HDREyeAdaption[1], region);	
		region = Vec4(0.4f,0.4f,0.5f,0.5f);
		FX_DrawFloatTextures(gkTextureManager::ms_HDREyeAdaption[2], region);	
		region = Vec4(0.5f,0.4f,0.6f,0.5f);
		FX_DrawFloatTextures(gkTextureManager::ms_HDREyeAdaption[3], region);	
		region = Vec4(0.6f,0.4f,0.7f,0.5f);
		FX_DrawFloatTextures(gkTextureManager::ms_HDREyeAdaption[4], region);	
		region = Vec4(0.7f,0.4f,0.8f,0.5f);
		FX_DrawFloatTextures(gkTextureManager::ms_HDREyeAdaption[5], region);	
		region = Vec4(0.8f,0.4f,0.9f,0.5f);
		FX_DrawFloatTextures(gkTextureManager::ms_HDREyeAdaption[6], region);	
		region = Vec4(0.9f,0.4f,1.0f,0.5f);
		FX_DrawFloatTextures(gkTextureManager::ms_HDREyeAdaption[7], region);	
	}
	else if(g_pRendererCVars->r_DebugDynTexType == 4)
	{
		region = Vec4(0,0,1.0f,1.0f);
		FX_DrawDynTextures(gkTextureManager::ms_ReflMap0Tmp, region);	
	}
	else if(g_pRendererCVars->r_DebugDynTexType == 5)
	{
		region = Vec4(0,0,1.0f,1.0f);
		FX_DrawFloatTextures(gkTextureManager::ms_ShadowMask, region);		
	}
	else if(g_pRendererCVars->r_DebugDynTexType == 1)
	{
		region = Vec4(0,0,1.0f,1.0f);

		// 		gkTexturePtr tex = getTextureMngPtr()->getByName(_T("fontmaptest"));
		// 		FX_DrawFloatTextures(tex, region);	
		FX_DrawFloatTextures(gkTextureManager::ms_SceneDepth, region);	
	}
	else if(g_pRendererCVars->r_DebugDynTexType == 6)
	{
		region = Vec4(0,0,1.0f,1.0f);
		FX_DrawDynTextures(gkTextureManager::ms_SceneDifAcc, region);	
	}
	else if(g_pRendererCVars->r_DebugDynTexType == 7)
	{
		region = Vec4(0,0,0.5f,0.5f);
		FX_DrawFloatTextures(gkTextureManager::ms_ShadowMask, region);	

		region = Vec4(0.5,0,1.0f,0.5f);
		FX_DrawFloatTextures(gkTextureManager::ms_SSAOTarget, region);	

		region = Vec4(0,0.5,0.33f,1.0f);
		FX_DrawFloatTextures(gkTextureManager::ms_ShadowCascade0, region, -1, 1);
		region = Vec4(0.33,0.5,0.66f,1.0f);
		FX_DrawFloatTextures(gkTextureManager::ms_ShadowCascade1, region, -1, 1);
		region = Vec4(0.66,0.5,1.0f,1.0f);
		FX_DrawFloatTextures(gkTextureManager::ms_ShadowCascade2, region, -1, 1);

		// 		region = Vec4(0.5,0.5,1.0f,1.0f);
		// 		FX_DrawDynTextures(gkTextureManager::ms_SceneDifAcc, region);
	}
	else
	{

	}
}

void gkRendererD3D9::buildGpuTimers()
{
	ms_GPUTimers.insert( gkGpuTimerMap::value_type( _T("GPUTime"), gkGpuTimer(_T("GPUTime"))) );
	ms_GPUTimers.insert( gkGpuTimerMap::value_type( _T("ReflectGen"), gkGpuTimer(_T("ReflectGen"))) );
	ms_GPUTimers.insert( gkGpuTimerMap::value_type( _T("ShadowMapGen"), gkGpuTimer(_T("ShadowMapGen"))) );
	ms_GPUTimers.insert( gkGpuTimerMap::value_type( _T("Zpass"), gkGpuTimer(_T("Zpass"))) );
	ms_GPUTimers.insert( gkGpuTimerMap::value_type( _T("SSAO"), gkGpuTimer(_T("SSAO"))) );
	ms_GPUTimers.insert( gkGpuTimerMap::value_type( _T("ShadowMaskGen"), gkGpuTimer(_T("ShadowMaskGen"))) );
	ms_GPUTimers.insert( gkGpuTimerMap::value_type( _T("Deferred Lighting"), gkGpuTimer(_T("Deferred Lighting"))) );
	ms_GPUTimers.insert( gkGpuTimerMap::value_type( _T("Ambient Pass"), gkGpuTimer(_T("Ambient Pass"))) );
	ms_GPUTimers.insert( gkGpuTimerMap::value_type( _T("Lights"), gkGpuTimer(_T("Lights"))) );
	ms_GPUTimers.insert( gkGpuTimerMap::value_type( _T("Opaque"), gkGpuTimer(_T("Opaque"))) );
	ms_GPUTimers.insert( gkGpuTimerMap::value_type( _T("Transparent"), gkGpuTimer(_T("Transparent"))) );
	ms_GPUTimers.insert( gkGpuTimerMap::value_type( _T("HDR"), gkGpuTimer(_T("HDR"))) );

	ms_GPUTimers.insert( gkGpuTimerMap::value_type( _T("PostProcess"), gkGpuTimer(_T("PostProcess"))) );
}

void gkRendererD3D9::initGpuTimers(IDirect3DDevice9* pDevice)
{
	gkGpuTimerMap::iterator it =  ms_GPUTimers.begin();
	for (; it != ms_GPUTimers.end(); ++it)
	{
		it->second.init(pDevice);
	}
}

void gkRendererD3D9::destoryGpuTimers()
{
	gkGpuTimerMap::iterator it =  ms_GPUTimers.begin();
	for (; it != ms_GPUTimers.end(); ++it)
	{
		it->second.destroy();
	}
}

void gkRendererD3D9::updateGpuTimers()
{
	if(g_pRendererCVars->r_ProfileGpu == 1)
	{
		gkGpuTimerMap::iterator it =  ms_GPUTimers.begin();
		for (; it != ms_GPUTimers.end(); ++it)
		{
			it->second.update();

			//if( gEnv->pProfiler->getFrameCount() % 30 == 0 )
			{
				gEnv->pProfiler->setGpuTimerElement( it->first.c_str(), it->second.getAvgTime() );
			}
		}
	}
}

bool gkRendererD3D9::RT_StartRender()
{
	if(CheckDevice() && m_bSkipThisFrame == 0)
	{
		if (g_pRendererCVars->r_MTRendering)
		{
			m_bRenderThread = true;
		}

		m_pAuxRenderer->_FlushAllText(m_pd3d9Device);

		if (m_bRenderThread)
		{
			m_tdRenderThread.Start();
			m_tdRenderThread.SetReadyToRender();
		}
		else
		{
			gEnv->pProfiler->setStartRendering();

			if( g_pRendererCVars->r_stereo && gEnv->pStereoDevice && gEnv->pStereoDevice->Avaliable() )
			{
				getRenderer()->_startFrame(eRS_LeftEye);
				getRenderer()->RP_RenderScene(eRS_LeftEye);
				getRenderer()->_endFrame(eRS_LeftEye);

				getRenderer()->_startFrame(eRS_RightEye);
				getRenderer()->RP_RenderScene(eRS_RightEye);
				getRenderer()->_endFrame(eRS_RightEye);
			}
			else
			{
				getRenderer()->_startFrame(eRS_Mono);
				getRenderer()->RP_RenderScene(eRS_Mono);
				getRenderer()->_endFrame(eRS_Mono);
			}

			gEnv->pProfiler->setEndRendering();
		}
		return true;
	}
	else
	{
		RT_CleanRenderSequence();
	}
	return false;
}

bool gkRendererD3D9::RT_EndRender()
{
	if (m_bSkipThisFrame == 0)
	{
		if (m_bRenderThread)
		{
			gEnv->pProfiler->setElementStart(ePe_ThreadSync_Cost);
			m_tdRenderThread.WaitForRenderFinish();
			gEnv->pProfiler->setElementEnd(ePe_ThreadSync_Cost);
		}
		else
		{

		}
	}
	else
	{
		gkLogWarning( _T("skip one frame rendering." ) );
	}

	m_bRenderThread = false;
	m_bSkipThisFrame--;
	if (m_bSkipThisFrame < 0)
	{
		m_bSkipThisFrame = 0;
	}
	return true;
}


void gkRendererD3D9::RC_SetTodKey(const STimeOfDayKey& key)
{
	m_pShaderParamDataSource.setCurrTodKey(key);
}
//////////////////////////////////////////////////////////////////////////
IRenderSequence* gkRendererD3D9::RT_SwapRenderSequence()
{
	// renderSeq clean and prepare
	if (m_pUpdatingRenderSequence && m_pUpdatingRenderSequence->m_bIsReady && m_pRenderingRenderSequence)
	{
		*m_pRenderingRenderSequence = *m_pUpdatingRenderSequence;

		RC_SetMainCamera(m_pRenderingRenderSequence->getCamera(eRFMode_General)->getCCam());

		if( g_pRendererCVars->r_Shadow )
		{
			if (m_pRenderingRenderSequence->getCamera(eRFMode_ShadowCas0))
			{
				RC_SetShadowCascadeCamera(m_pRenderingRenderSequence->getCamera(eRFMode_ShadowCas0)->getCCam(), 0);
			}


			if (gEnv->pProfiler->getFrameCount() % GSM_SHADOWCASCADE1_DELAY == 0 && m_pRenderingRenderSequence->getCamera(eRFMode_ShadowCas1))
				RC_SetShadowCascadeCamera(m_pRenderingRenderSequence->getCamera(eRFMode_ShadowCas1)->getCCam(), 1);

			if (gEnv->pProfiler->getFrameCount() % GSM_SHADOWCASCADE2_DELAY == 0 && m_pRenderingRenderSequence->getCamera(eRFMode_ShadowCas2))
				RC_SetShadowCascadeCamera(m_pRenderingRenderSequence->getCamera(eRFMode_ShadowCas2)->getCCam(), 2);
		}
	}
	m_pAuxRenderer->_swapBufferForRendering();


	m_pUpdatingRenderSequence->clear();
	m_pRenderingRenderSequence->PrepareRenderSequence();

	m_pShaderParamDataSource.setCurrTodKey( gEnv->p3DEngine->getTimeOfDay()->getCurrentTODKey() );

	return m_pUpdatingRenderSequence;
}

IRenderSequence* gkRendererD3D9::RT_GetRenderSequence()
{
	return m_pUpdatingRenderSequence;
}

//////////////////////////////////////////////////////////////////////////
void gkRendererD3D9::FX_ColorGradingTo( gkTexturePtr& pCch, float fAmount )
{
	if( m_pColorGradingController )
	{
		m_pColorGradingController->setTargetColorChart(pCch);
		m_pColorGradingController->setTargetBlendAmount(fAmount);
	}
}
//////////////////////////////////////////////////////////////////////////
void gkRendererD3D9::RT_CleanRenderSequence()
{
	m_pUpdatingRenderSequence->clear();
	m_pRenderingRenderSequence->clear();

	m_pAuxRenderer->_cleanBuffer();
}
//////////////////////////////////////////////////////////////////////////
void gkRendererD3D9::RT_SkipOneFrame(int framecount)
{
	m_bSkipThisFrame = framecount;
	RT_CleanRenderSequence();
}

Ray gkRendererD3D9::GetRayFromScreen( uint32 nX, uint32 nY )
{
	Ray ret;

	ICamera* cam = gEnv->p3DEngine->getMainCamera();
	m_pShaderParamDataSource.setMainCamera( cam->getCCam() );
	m_pShaderParamDataSource.m_pCurrentCamera->GetPixelRay( nX < 0 ? 0 : nX, nY < 0 ? 0 : nY, GetScreenWidth(), GetScreenHeight(), &(ret.origin), &(ret.direction) );
	//	ret.direction.Normalize();

	return ret;
}

Vec3 gkRendererD3D9::ProjectScreenPos( const Vec3& worldpos )
{
	Vec3 ret;

	Matrix44 matViewProj = m_pShaderParamDataSource.getMainCamViewProjMatrixForRay();
	//matViewProj.Transpose();
	Vec4 pParams= Vec4(worldpos, 1.0f);   
	// no need to waste gpu to compute sun screen pos
	Vec4 pScreenPos = matViewProj * pParams;
	pScreenPos.x = ( ( pScreenPos.x + pScreenPos.w) * 0.5f ) /pScreenPos.w;   
	pScreenPos.y = ( (-pScreenPos.y + pScreenPos.w) * 0.5f ) /pScreenPos.w;


	UINT width = gEnv->pRenderer->GetScreenWidth();
	UINT height = gEnv->pRenderer->GetScreenHeight();

	ret.x = pScreenPos.x * width;
	ret.y = pScreenPos.y * height;
	ret.z = pScreenPos.z;

	return ret;
}

void gkRendererD3D9::SetRenderState( uint32 key, uint32 value )
{
	if (m_pStateManager)
	{
		m_pStateManager->SetRenderState( (D3DRENDERSTATETYPE)key, value );
	}
	else
	{
		m_pd3d9Device->SetRenderState( (D3DRENDERSTATETYPE)key, value );
	}
}

HWND gkRendererD3D9::GetWindowHwnd()
{
	if (gEnv->pSystem->IsEditor() && m_lstRenderContent.size() > m_uCurrContent && !FullScreenMode())
	{
		return m_lstRenderContent[m_uCurrContent].m_hwnd;
	}
	else
	{
		return m_hWnd;
	}

}

Vec2i gkRendererD3D9::GetWindowOffset()
{
	if (gEnv->pSystem->IsEditor() && m_lstRenderContent.size() > m_uCurrContent)
	{
		return Vec2i( m_lstRenderContent[m_uCurrContent].m_offset.x, m_lstRenderContent[m_uCurrContent].m_offset.y);
	}
	else
	{
		return Vec2i(0,0);
	}	
}

IParticleProxy* gkRendererD3D9::createGPUParticleProxy()
{
	// HACK FIX ME [4/12/2013 YiKaiming]
	IParticleProxy* newProxy = new gkGpuParticleProxy();

	m_particleProxy.push_back(newProxy);

	return newProxy;
}

void gkRendererD3D9::destroyGPUParticelProxy( IParticleProxy* proxy )
{
	if (proxy)
	{
		std::vector< IParticleProxy* >::iterator it = m_particleProxy.begin();
		for( ; it != m_particleProxy.end(); ++it )
		{
			if ( *it == proxy)
			{
				delete proxy;
				m_particleProxy.erase( it );
				break;
			}
		}		
	}
}

D3DPOOL gkRendererD3D9::GetManagedTexturePoolType()
{
	if(USE_DXUT)
	{
		return D3DPOOL_MANAGED;
	}
	else
	{
		return D3DPOOL_DEFAULT;
	}
}

bool gkRendererD3D9::FullScreenMode()
{
	return m_bFullScreen;
}

void gkRendererD3D9::SetWindowContent(HWND hWnd, RECT windowRect)
{
	m_windowRectInClient = windowRect;
}

void gkRendererD3D9::SetOverrideSize(int width, int height, bool set)
{
	m_override_width = width;
	m_override_height = height;
	m_use_override = set;

	m_rtSizeChange = true;
}

Vec3 gkRendererD3D9::ScreenPosToViewportPos(Vec3 screenPos)
{
	Vec3 output = screenPos;
	output.x = ( screenPos.x / (float)GetScreenWidth() - 0.5f ) * 2.0f;
	output.y = - ( screenPos.y / (float)GetScreenHeight()- 0.5f ) * 2.0f;

	return output;
}

void gkRendererD3D9::SetPixelReSize(float scale)
{
	g_pRendererCVars->r_pixelscale = scale;
	m_rtSizeChange = true;
}

float gkRendererD3D9::GetPixelReSize()
{
	return g_pRendererCVars->r_pixelscale;
	return 1.0f;
}
void gkRendererD3D9::SavePositionCubeMap(Vec3 position, const TCHAR* texturename)
{
	m_pLightProbeSystem->SavePositionCubeMap(position, texturename);
}