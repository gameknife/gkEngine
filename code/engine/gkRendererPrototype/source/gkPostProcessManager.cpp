#include "RendererD3D9StableHeader.h"
#include "gkPostProcessManager.h"
#include "gkRenderTarget.h"
#include "RenderRes\gkTexture.h"
#include "RenderRes\gkTextureManager.h"
#include "ICamera.h"
//#include "gkConsoleVariablesManager.h"

#include "gkFilePath.h"
#include "gk_Camera.h"
#include "gkRendererD3D9.h"
#include "ITimeOfDay.h"


#include "gkMemoryLeakDetecter.h"
#include "..\Profiler\gkGPUTimer.h"






LPCWSTR gkPostProcessManager::m_aszFxFile[] =
{
	//////////////////////////////////////////////////////////////////////////
	// strectch operation
	L"PP_CurToSrc.fx",			//0
	L"PP_SrcToCur.fx",			//1
	L"PP_CopyToCache0.fx",
	L"PP_CopyToCache1.fx",
	L"PP_CopyToCache2.fx",
	L"PP_CopyFromCache0.fx",	//5
	L"PP_CopyFromCache1.fx",
	L"PP_CopyFromCache2.fx",
	L"PP_Cache0ToSrc.fx",
	L"PP_Cache1ToSrc.fx",
	L"PP_Cache2ToSrc.fx",		//10
	L"PP_CurToSrc.fx",			//
	L"PP_CurToSrc.fx",			//
	L"PP_CurToSrc.fx",			//
	L"PP_CurToSrc.fx",			//14

	//////////////////////////////////////////////////////////////////////////
	// internal operation
	L"PP_SSAO.fx",				//15
	L"PP_ShadowMap.fx",
	L"PP_FakeHDR.fx",

	L"PP_FakeHDR.fx",
	// REFACTOR FIX
	//L"PP_DeferredDecals.fx",

	L"PP_ColorMultiply.fx",		//19
	L"PP_ColorMultiply4.fx",
	L"PP_ColorScreen.fx",
	L"PP_ColorCombine.fx",
	L"PP_ColorCombine4.fx",			
	L"PP_DofCombine.fx",		//24

	L"PP_NormalMap.fx",			//25
	L"PP_PositionMap.fx",		//26
	L"PP_ShadowMapToScreen.fx",	//27

	L"PP_HDRFinalPass.fx",			// 28
	L"PP_ObjSpecEffect.fx",			//29

	//////////////////////////////////////////////////////////////////////////
	// other operation
	L"PP_ColorMonochrome.fx",				//30
	L"PP_ColorInverse.fx",				//27
	L"PP_ColorGBlurH.fx",				//28
	L"PP_ColorGBlurV.fx",				//29
	L"PP_ColorBloomH.fx",				//30
	L"PP_ColorBloomV.fx",					//31
	L"PP_ColorBrightPass.fx",					//32
	L"PP_ColorToneMap.fx",						//33
	L"PP_ColorEdgeDetect.fx",			//34
	L"PP_ColorDownFilter4.fx",					//35
	L"PP_ColorUpFilter4.fx",					//36
	L"PP_NormalEdgeDetect.fx",			//37
	L"PP_ColorSSAOBlur.fx",			//37
	L"PP_VtxLayerBlend.fx",			//37
};
//int gkPostProcessManager::m_ppCount= 19;

// Vertex declaration for post-processing
const D3DVERTEXELEMENT9 PPVERT::Decl[4] =
{
	{ 0, 0,  D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITIONT, 0 },
	{ 0, 16, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,  0 },
	{ 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,  1 },
	D3DDECL_END()
};


// HDR Find [5/28/2011 Kaiming-Desktop]
HRESULT FindBestHDRFormat( D3DFORMAT* pBestFormat )
{
	D3DCAPS9 Caps;
	gEnv->pRenderer->getDevice()->GetDeviceCaps( &Caps );

	IDirect3D9* pD3D;
	gEnv->pRenderer->getDevice()->GetDirect3D( &pD3D );
	D3DDISPLAYMODE DisplayMode;
	gEnv->pRenderer->getDevice()->GetDisplayMode( 0, &DisplayMode );

	// Retrieve important information about the current configuration

// 	if( FAILED( pD3D->CheckDeviceFormat( Caps.AdapterOrdinal, Caps.DeviceType,
// 		DisplayMode.Format,
// 		D3DUSAGE_QUERY_FILTER | D3DUSAGE_RENDERTARGET,
// 		D3DRTYPE_TEXTURE, D3DFMT_A32B32G32R32F ) ) )
// 	{
// 		// We don't support 128 bit render targets with filtering. Check the next format.
// 		OutputDebugString(
// 			L"Enumeration::FindBestHDRFormat() - Current device does *not* support single-precision floating point textures with filtering.\n" );
// 
// 		if( FAILED( pD3D->CheckDeviceFormat( Caps.AdapterOrdinal, Caps.DeviceType,
// 			DisplayMode.Format,
// 			D3DUSAGE_QUERY_FILTER | D3DUSAGE_RENDERTARGET,
// 			D3DRTYPE_TEXTURE, D3DFMT_A16B16G16R16F ) ) )
// 		{
// 			// We don't support 64 bit render targets with filtering. Check the next format.
// 			OutputDebugString(
// 				L"Enumeration::FindBestHDRFormat() - Current device does *not* support half-precision floating point textures with filtering.\n" );
// 
// 			if( FAILED( pD3D->CheckDeviceFormat( Caps.AdapterOrdinal, Caps.DeviceType,
// 				DisplayMode.Format, D3DUSAGE_RENDERTARGET,
// 				D3DRTYPE_TEXTURE, D3DFMT_A32B32G32R32F ) ) )
// 			{
// 				// We don't support 128 bit render targets. Check the next format.
// 				OutputDebugString(
// 					L"Enumeration::FindBestHDRFormat() - Current device does *not* support single-precision floating point textures.\n" );

				if( FAILED( pD3D->CheckDeviceFormat( Caps.AdapterOrdinal, Caps.DeviceType,
					DisplayMode.Format, D3DUSAGE_RENDERTARGET,
					D3DRTYPE_TEXTURE, D3DFMT_A16B16G16R16F ) ) )
				{
					// We don't support 64 bit render targets. This device is not compatable.
					OutputDebugString(
						L"Enumeration::FindBestHDRFormat() - Current device does *not* support half-precision floating point textures.\n" );
					OutputDebugString(
						L"Enumeration::FindBestHDRFormat() - THE CURRENT HARDWARE DOES NOT SUPPORT HDR RENDERING!\n" );
					return E_FAIL;
				}
				else
				{
					// We have support for 64 bit render targets without filtering
					OutputDebugString(
						L"Enumeration::FindBestHDRFormat() - Best available format is 'half-precision without filtering'.\n" );
					if( NULL != pBestFormat ) *pBestFormat = D3DFMT_A16B16G16R16F;
				}
// 			}
// 			else
// 			{
// 				// We have support for 128 bit render targets without filtering
// 				OutputDebugString(
// 					L"Enumeration::FindBestHDRFormat() - Best available format is 'single-precision without filtering'.\n" );
// 				if( NULL != pBestFormat ) *pBestFormat = D3DFMT_A32B32G32R32F;
// 			}
// 
// 		}
// 		else
// 		{
// 			// We have support for 64 bit render targets with filtering
// 			OutputDebugString(
// 				L"Enumeration::FindBestHDRFormat() - Best available format is 'half-precision with filtering'.\n" );
// 			if( NULL != pBestFormat ) *pBestFormat = D3DFMT_A16B16G16R16F;
// 		}
// 	}
// 	else
// 	{
// 		// We have support for 128 bit render targets with filtering
// 		OutputDebugString(
// 			L"Enumeration::FindBestHDRFormat() - Best available format is 'single-precision with filtering'.\n" );
// 		if( NULL != pBestFormat ) *pBestFormat = D3DFMT_A32B32G32R32F;
// 	}

	return S_OK;

}

HRESULT gkPostProcess::Init( LPDIRECT3DDEVICE9 pDev, DWORD dwShaderFlags, LPCWSTR wszName )
{
	HRESULT hr;
	WCHAR wszPath[MAX_PATH];

	if( FAILED( hr = gkFindFileRelativeGame( wszPath, MAX_PATH, wszName ) ) )
		return hr;
	hr = D3DXCreateEffectFromFile( pDev,
		wszPath,
		NULL,
		NULL,
		dwShaderFlags,
		NULL,
		&m_pEffect,
		NULL );
	if( FAILED( hr ) )
		return hr;

	// 
	// Get the PostProcess technique handle
	m_hTPostProcess = m_pEffect->GetTechniqueByName( "PostProcess" );

	// 取得贴图和场景句柄
	// Obtain the handles to all texture objects in the effect
	m_hTexScene[0] = m_pEffect->GetParameterByName( NULL, "g_txSceneColor" );
	m_hTexScene[1] = m_pEffect->GetParameterByName( NULL, "g_txSceneNormal" );
	m_hTexScene[2] = m_pEffect->GetParameterByName( NULL, "g_txScenePosition" );
	m_hTexScene[3] = m_pEffect->GetParameterByName( NULL, "g_txSceneVelocity" );
	m_hTexSource[0] = m_pEffect->GetParameterByName( NULL, "g_txSrcColor" );
	m_hTexSource[1] = m_pEffect->GetParameterByName( NULL, "g_txSrcNormal" );
	m_hTexSource[2] = m_pEffect->GetParameterByName( NULL, "g_txSrcPosition" );
	m_hTexSource[3] = m_pEffect->GetParameterByName( NULL, "g_txSrcVelocity" );

	// Find out what render targets the technique writes to.
	// 寻找该技术输出的渲染目标
	D3DXTECHNIQUE_DESC techdesc;
	if( FAILED( m_pEffect->GetTechniqueDesc( m_hTPostProcess, &techdesc ) ) )
		return D3DERR_INVALIDCALL;

	for( DWORD i = 0; i < techdesc.Passes; ++i )
	{
		D3DXPASS_DESC passdesc;
		// 取得第一个pass的描述
		if( SUCCEEDED( m_pEffect->GetPassDesc( m_pEffect->GetPass( m_hTPostProcess, i ), &passdesc ) ) )
		{
			D3DXSEMANTIC aSem[MAXD3DDECLLENGTH];
			UINT uCount;
			// 得到PIXEL SHADER的输出语义，写入ASEM, 以及个数
			if( SUCCEEDED( D3DXGetShaderOutputSemantics( passdesc.pPixelShaderFunction, aSem, &uCount ) ) )
			{
				// Semantics received. Now examine the content and
				// find out which render target this technique
				// writes to.
				while( uCount-- )
				{
					// 如果此输出语义是COLOR，并且使用的COLOR0这种INDEX小于RT个数，则使用此渲染输出通道
					if( D3DDECLUSAGE_COLOR == aSem[uCount].Usage &&
						RT_COUNT > aSem[uCount].UsageIndex )
						m_bWrite[uCount] = true;
				}
			}
		}
	}

	// 取得RT的通道（根据注释ANNOTATION来取得）
	// 此RT默认为0(COLOR通道), 可以设置为1,2（normal,position）
	// Obtain the render target channel
	D3DXHANDLE hAnno;
	hAnno = m_pEffect->GetAnnotationByName( m_hTPostProcess, "nRenderTarget" );
	if( hAnno )
		m_pEffect->GetInt( hAnno, &m_nRenderTarget );


	// 如果有可以设置的参数，取得参数的句柄
	// Obtain the handles to the changeable parameters, if any.
	for( int i = 0; i < NUM_PARAMS; ++i )
	{
		char szName[32];

		sprintf_s( szName, 32, "Parameter%d", i );
		hAnno = m_pEffect->GetAnnotationByName( m_hTPostProcess, szName );
		LPCSTR szParamName;
		if( hAnno &&
			SUCCEEDED( m_pEffect->GetString( hAnno, &szParamName ) ) )
		{
			m_ahParam[i] = m_pEffect->GetParameterByName( NULL, szParamName );
			MultiByteToWideChar( CP_ACP, 0, szParamName, -1, m_awszParamName[i], MAX_PATH );
		}

		// Get the parameter description
		sprintf_s( szName, 32, "Parameter%dDesc", i );
		hAnno = m_pEffect->GetAnnotationByName( m_hTPostProcess, szName );
		if( hAnno &&
			SUCCEEDED( m_pEffect->GetString( hAnno, &szParamName ) ) )
		{
			MultiByteToWideChar( CP_ACP, 0, szParamName, -1, m_awszParamDesc[i], MAX_PATH );
		}

		// Get the parameter size
		sprintf_s( szName, 32, "Parameter%dSize", i );
		hAnno = m_pEffect->GetAnnotationByName( m_hTPostProcess, szName );
		if( hAnno )
			m_pEffect->GetInt( hAnno, &m_anParamSize[i] );

		// Get the parameter default
		sprintf_s( szName, 32, "Parameter%dDef", i );
		hAnno = m_pEffect->GetAnnotationByName( m_hTPostProcess, szName );
		if( hAnno )
			m_pEffect->GetVector( hAnno, &m_avParamDef[i] );
	}

	return S_OK;
}

HRESULT gkPostProcess::OnResetDevice( DWORD dwWidth, DWORD dwHeight )
{
	assert( m_pEffect );
	m_pEffect->OnResetDevice();

	// 在RESET时，（通常是丢失设备：窗口大小改变，窗口拖动等）
	// 将存在的PIXEL滤镜核心转换为TEXEL滤镜核心
	// If one or more kernel exists, convert kernel from
	// pixel space to texel space.

	// 使用ANNOTATION："ConvertPixelsToTexels"来标明核心
	// First check for kernels.  Kernels are identified by
	// having a string annotation of name "ConvertPixelsToTexels"
	D3DXHANDLE hParamToConvert;
	D3DXHANDLE hAnnotation;
	UINT uParamIndex = 0;
	// If a top-level parameter has the "ConvertPixelsToTexels" annotation,
	// do the conversion.
	// 如果能继续取得参数，就继续
	while( NULL != ( hParamToConvert = m_pEffect->GetParameter( NULL, uParamIndex++ ) ) )
	{
		//如果能找到此ANNOTATION
		if( NULL != ( hAnnotation = m_pEffect->GetAnnotationByName( hParamToConvert, "ConvertPixelsToTexels" ) ) )
		{
			LPCSTR szSource;
			// 取得ANNOTATION指向的字符串
			m_pEffect->GetString( hAnnotation, &szSource );
			D3DXHANDLE hConvertSource = m_pEffect->GetParameterByName( NULL, szSource );

			if( hConvertSource )
			{
				// Kernel source exists. Proceed.
				// Retrieve the kernel size
				D3DXPARAMETER_DESC desc;
				m_pEffect->GetParameterDesc( hConvertSource, &desc );
				// Each element has 2 floats
				// 取得每个元素的XY
				DWORD cKernel = desc.Bytes / ( 2 * sizeof( float ) );
				D3DXVECTOR4* pvKernel = new D3DXVECTOR4[cKernel];
				if( !pvKernel )
					return E_OUTOFMEMORY;
				// 取得KERNEL的指针和KERNEL的个数，使用循环来转换
				m_pEffect->GetVectorArray( hConvertSource, pvKernel, cKernel );
				// Convert
				for( DWORD i = 0; i < cKernel; ++i )
				{
					//PIXEL除以当前的的屏幕宽高即可得到TEXEL
					pvKernel[i].x = pvKernel[i].x / dwWidth;
					pvKernel[i].y = pvKernel[i].y / dwHeight;
				}
				// Copy back
				// 存入转换后的数组
				m_pEffect->SetVectorArray( hParamToConvert, pvKernel, cKernel );

				delete[] pvKernel;
			}
		}
	}

	return S_OK;
}
//-----------------------------------------------------------------------
void gkPostProcess::Cleanup()
{
	SAFE_RELEASE( m_pEffect );
}
HRESULT gkPostProcessManager::OnCreateDevice(IDirect3DDevice9* pd3dDevice)
{
	HRESULT hr;
	m_pDevice = pd3dDevice;
	// Query multiple RT setting and set the num of passes required
	D3DCAPS9 Caps;
	pd3dDevice->GetDeviceCaps( &Caps );
	// 查看硬件支持同时存在的RT个数，高端卡>=4
	if( Caps.NumSimultaneousRTs > 2 )
	{
		// One pass of 3 RTs
		m_nPasses = 1;
		m_nRtUsed = 3;
	}
	else if( Caps.NumSimultaneousRTs > 1 )
	{
		// Two passes of 2 RTs. The 2nd pass uses only one.
		m_nPasses = 2;
		m_nRtUsed = 2;
	}
	else
	{
		// Three passes of single RT.
		m_nPasses = 3;
		m_nRtUsed = 1;
	}

	FindBestHDRFormat(&m_TexFormat);

	DWORD dwShaderFlags = D3DXFX_NOT_CLONEABLE;

	// #if defined( DEBUG ) || defined( _DEBUG )
	// 	dwShaderFlags |= D3DXSHADER_DEBUG;
	// #endif

#ifdef DEBUG_VS
	dwShaderFlags |= D3DXSHADER_FORCE_VS_SOFTWARE_NOOPT;
#endif
#ifdef DEBUG_PS
	dwShaderFlags |= D3DXSHADER_FORCE_PS_SOFTWARE_NOOPT;
#endif

	// Read the D3DX effect file
	WCHAR str[MAX_PATH];
	V_RETURN( gkFindFileRelativeGame( str, MAX_PATH, L"ksDefaultPost.fx" ) );

	// If this fails, there should be debug output as to 
	// they the .fx file failed to compile
	V_RETURN( D3DXCreateEffectFromFile( pd3dDevice, str, NULL, NULL, dwShaderFlags,
		NULL, &m_pEffect, NULL ) );

	// 初始化后期处理模块，根据写好的PP效果个数决定
	// Initialize the PostProcess objects
	for( int i = 0; i < m_ppCount; ++i )
	{
		hr = m_aPostProcess[i].Init( pd3dDevice, dwShaderFlags, m_aszFxFile[i] );
		if( FAILED( hr ) )
			return hr;
	}

	// Create vertex declaration for post-process
	// 为后期特效的的顶点声明
	if( FAILED( hr = pd3dDevice->CreateVertexDeclaration( PPVERT::Decl, &m_pVertDeclPP ) ) )
	{
		return hr;
	}

	//m_pDefaultDecal = gkTextureManager::getSingleton().load(L"defaultDecal.png",L"");

	// initialize the scenesave [10/22/2011 Kaiming]

	return S_OK;
}

HRESULT gkPostProcessManager::OnResetDevice(IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc)
{
	HRESULT hr;

	if( m_pEffect )
		V_RETURN( m_pEffect->OnResetDevice() );

	// add screen property [10/17/2011 Kaiming]
	// set a static shared vector [10/13/2011 Kaiming]
	D3DXVECTOR4 screensizer = D3DXVECTOR4(1.0f / (float)pBackBufferSurfaceDesc->Width, 1.0f / (float)pBackBufferSurfaceDesc->Height, 0, 0);
	m_pEffect->SetVector("g_fScreenSize", &screensizer);

	for( int p = 0; p < m_ppCount; ++p )
		V_RETURN( m_aPostProcess[p].OnResetDevice( pBackBufferSurfaceDesc->Width, pBackBufferSurfaceDesc->Height ) );

	D3DXCreateSphere(pd3dDevice, 1, 10, 10, &m_pLightSphere, NULL);

	// Create scene save texture
	// 创建存储场景的贴图
	for( int i = 0; i < RT_COUNT; ++i )
	{
// 		V_RETURN( pd3dDevice->CreateTexture( pBackBufferSurfaceDesc->Width,
// 			pBackBufferSurfaceDesc->Height,
// 			1,
// 			D3DUSAGE_RENDERTARGET,
// 			m_TexFormat,
// 			D3DPOOL_DEFAULT,
// 			&(m_pSceneSave[i]->getTexture()),
// 			NULL ) );

		// Create the textures for this render target chains
		// 为RT链创建贴图
		IDirect3DTexture9* pRT[2];
		ZeroMemory( pRT, sizeof( pRT ) );
		for( int t = 0; t < 2; ++t )
		{
			V_RETURN( pd3dDevice->CreateTexture( pBackBufferSurfaceDesc->Width,
				pBackBufferSurfaceDesc->Height,
				1,
				D3DUSAGE_RENDERTARGET,
				m_TexFormat,
				D3DPOOL_DEFAULT,
				&pRT[t],
				NULL ) );
		}
		// 使用建立的贴图初始化RT链
		m_RTChain[i].Init( pRT );
		SAFE_RELEASE( pRT[0] );
		SAFE_RELEASE( pRT[1] );
	}

	// 创建CACHE层
	for( int i = 0; i < CACHE_COUNT; ++i )
	{
		V_RETURN( pd3dDevice->CreateTexture( pBackBufferSurfaceDesc->Width,
			pBackBufferSurfaceDesc->Height,
			1,
			D3DUSAGE_RENDERTARGET,
			m_TexFormat,
			D3DPOOL_DEFAULT,
			&m_pCache[i],
			NULL ) );	
	}

	// 为RT创建SURFACE
	// Initialize the render target table based on how many simultaneous RTs
	// the card can support.
	IDirect3DSurface9* pSurf;

		gkTextureManager::ms_SceneTarget0->getTexture()->GetSurfaceLevel( 0, &pSurf );
		m_aRtTable[0].pRT[0] = pSurf;
		gkTextureManager::ms_SceneNormal->getTexture()->GetSurfaceLevel( 0, &pSurf );
		m_aRtTable[0].pRT[1] = pSurf;
		gkTextureManager::ms_SceneDepth->getTexture()->GetSurfaceLevel( 0, &pSurf );
		m_aRtTable[0].pRT[2] = pSurf;
		// Passes 1 and 2 are not used
		m_aRtTable[1].pRT[0] = NULL;
		m_aRtTable[1].pRT[1] = NULL;
		m_aRtTable[1].pRT[2] = NULL;
		m_aRtTable[2].pRT[0] = NULL;
		m_aRtTable[2].pRT[1] = NULL;
		m_aRtTable[2].pRT[2] = NULL;

	// if MSAA enabled
	if(pBackBufferSurfaceDesc->MultiSampleType != D3DMULTISAMPLE_NONE)
	{
		// create a swap RT for MSAA
		V_RETURN( pd3dDevice->CreateRenderTarget( pBackBufferSurfaceDesc->Width,
			pBackBufferSurfaceDesc->Height, D3DFMT_A8R8G8B8, 
			pBackBufferSurfaceDesc->MultiSampleType,
			pBackBufferSurfaceDesc->MultiSampleQuality,
			FALSE,
			&m_pSwapSurf0,
			NULL
			) );		

		// turn on the flag
		m_bIsMultiSample = true;
	}
	else
		// turn off the flag
		m_bIsMultiSample = false;

	return S_OK;
}

//--------------------------------------------------------------------------------------
// Name: PerformSinglePostProcess()
// Desc: Perform post-process by setting the previous render target as a
//       source texture and rendering a quad with the post-process technique
//       set.
//       This method changes render target without saving any. The caller
//       should ensure that the default render target is saved before calling
//       this.
//       When this method is invoked, m_dwNextTarget is the index of the
//       rendertarget of this post-process.  1 - m_dwNextTarget is the index
//       of the source of this post-process.
HRESULT gkPostProcessManager::PerformSinglePostProcess( IDirect3DDevice9* pd3dDevice,
											 gkPostProcess& PP,
											 CPProcInstance& Inst,
											 IDirect3DVertexBuffer9* pVB,
											 PPVERT* aQuad,
											 float& fExtentX,
											 float& fExtentY,
											 bool clearPrev)
{
	HRESULT hr;



	D3DXHANDLE hTech = PP.m_pEffect->GetTechniqueByName("PostProcess");

	//
	// The post-process effect may require that a copy of the
	// originally rendered scene be available for use, so
	// we initialize them here.
	//


	PP.m_pEffect->SetTexture( PP.m_hTexScene[0], gkTextureManager::ms_SceneTarget0->getTexture() );
	PP.m_pEffect->SetTexture( PP.m_hTexScene[1], gkTextureManager::ms_SceneNormal->getTexture() );
	PP.m_pEffect->SetTexture( PP.m_hTexScene[2], gkTextureManager::ms_SceneDepth->getTexture() );
		
	// vtxBlendLayer的判断
	D3DXHANDLE hVtxOverLayTex = PP.m_pEffect->GetParameterByName( NULL, "g_txVtxBlendLayer" );
	if (hVtxOverLayTex)
	{
		gkTexturePtr pTexture = gEnv->pSystem->getTextureMngPtr()->getByName(L"vtxOverlay");
		if (!pTexture.isNull())
		{
			PP.m_pEffect->SetTexture(hVtxOverLayTex, pTexture->getTexture());
		}
	}
	// 判断下是不是ssao为其设置randomtex
	D3DXHANDLE hSSAORandomNormal = PP.m_pEffect->GetParameterByName( NULL, "g_txRandomNormal" );
	if (hSSAORandomNormal)
	{
		PP.m_pEffect->SetTexture(hSSAORandomNormal, m_pRandomTexture);
	}

	// 判断下是不是shadowmap为其设置shadowmaptex
	D3DXHANDLE hShadowMap = PP.m_pEffect->GetParameterByName( NULL, "g_txShadow" );
	if (hShadowMap)
	{
		gkTexturePtr pTexture = gEnv->pSystem->getTextureMngPtr()->getByName(L"DefaultShadowMap");
		if (!pTexture.isNull())
		{
			PP.m_pEffect->SetTexture(hShadowMap, pTexture->getTexture());
			PP.m_pEffect->SetFloat("g_fShadowDepth", m_fShadowDepth);
			PP.m_pEffect->SetFloat("g_fShadowSlopedEpsilon", gEnv->pCVManager->r_GSMShadowConstbia);
		}
	}
// REFACTOR FIX [8/10/2011 Kaiming-Desktop]

	// 判断下是不是decalsPP为其设置临时Decalstex
// 	D3DXHANDLE hDecalsMap = PP.m_pEffect->GetParameterByName( NULL, "g_txDecals0" );
// 	if (hDecalsMap)
// 	{
// 		D3DXMATRIX mViewToLightProj;
// 		// the inverse of view matrix * view matrix of light * light projection matrix
// 		mViewToLightProj = getSceneMngPtr()->getCamera(L"MainCamera")->getViewMatrix();
// 		D3DXMatrixInverse( &mViewToLightProj, NULL, &mViewToLightProj );
// 		PP.m_pEffect->SetMatrix("g_matShadowProjInWorldView", &mViewToLightProj);
// 
// 		//pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
// 	}

	// 判断下是不是shadowlayer为其设置矩阵g_mViewToLightProj
	// 判断下是不是shadowmap为其设置shadowmaptex
	D3DXHANDLE hViewtoLightProj = PP.m_pEffect->GetParameterByName( NULL, "g_mViewToLightProj" );

	// 判断下是否需要做halfRender
	// 为其设置quat面片长度
	bool bUseHalfRender = false;
	D3DXHANDLE hSSAOdivide = PP.m_pEffect->GetAnnotationByName( hTech, "bCanDivide" );
	if (hSSAOdivide)
	{
		bUseHalfRender = true;
	}

	// 判断一下是不是一个CACHE操作标志
	int bCopyToCache = -1;
	int bCopyFromCache = -1;
	int bCacheToSrc = -1;
	int bCurToSrc = -1;
	int bSrcToCur = -1;
	D3DXHANDLE hCopyToCache;
	hCopyToCache = PP.m_pEffect->GetAnnotationByName( hTech, "copytocache" );
	if (hCopyToCache)
	{
		PP.m_pEffect->GetInt( hCopyToCache, &bCopyToCache );
	}
	else
	{
		hCopyToCache = PP.m_pEffect->GetAnnotationByName( hTech, "copyfromcache" );
		if (hCopyToCache)
		{
			PP.m_pEffect->GetInt( hCopyToCache, &bCopyFromCache );
		}
		else
		{
			hCopyToCache = PP.m_pEffect->GetAnnotationByName( hTech, "cachetosrc" );
			if (hCopyToCache)
			{
				PP.m_pEffect->GetInt( hCopyToCache, &bCacheToSrc );
			}
			else
			{
				hCopyToCache = PP.m_pEffect->GetAnnotationByName( hTech, "curtosrc" );
				if (hCopyToCache)
				{
					bCurToSrc = 1;
				}
				else
				{
					hCopyToCache = PP.m_pEffect->GetAnnotationByName( hTech, "srctocur" );
					if (hCopyToCache)
					{
						bSrcToCur = 1;
					}
				}
			}
		}
	}

	// 如果得到了操作标志就执行拷贝操作并返回，不进行flip操作了
	if(bCopyToCache != -1)
	{
		IDirect3DSurface9* pCacheSurf;
		IDirect3DSurface9* pTargetSrc;
		hr = m_pCache[bCopyToCache]->GetSurfaceLevel( 0, &pCacheSurf );
		if( FAILED( hr ) )
			return hr;
		hr = m_RTChain[PP.m_nRenderTarget].GetPrevTarget()->GetSurfaceLevel( 0, &pTargetSrc );
		if( FAILED( hr ) )
			return hr;

		D3DSURFACE_DESC srcDesc;
		D3DSURFACE_DESC dstDesc;
		pCacheSurf->GetDesc(&dstDesc);
		pTargetSrc->GetDesc(&srcDesc);
		RECT srcRect;
		RECT dstRect;
		srcRect.left = 0;
		srcRect.right = srcDesc.Width * Inst.m_avParam[0].x;
		srcRect.top = 0;
		srcRect.bottom = srcDesc.Height * Inst.m_avParam[0].x;

		dstRect.left = 0;
		dstRect.right = dstDesc.Width * Inst.m_avParam[1].x;
		dstRect.top = 0;
		dstRect.bottom = dstDesc.Height * Inst.m_avParam[1].x;


		pd3dDevice->StretchRect(pTargetSrc, &srcRect, pCacheSurf, &dstRect, D3DTEXF_POINT);

		SAFE_RELEASE(pCacheSurf);
		SAFE_RELEASE(pTargetSrc);
		return S_OK;
	}
	else if(bCopyFromCache != -1)
	{
		IDirect3DSurface9* pCacheSurf;
		IDirect3DSurface9* pTargetSrc;
		hr = m_pCache[bCopyFromCache]->GetSurfaceLevel( 0, &pCacheSurf );
		if( FAILED( hr ) )
			return hr;
		hr = m_RTChain[PP.m_nRenderTarget].GetNextSource()->GetSurfaceLevel( 0, &pTargetSrc );
		if( FAILED( hr ) )
			return hr;

		D3DSURFACE_DESC srcDesc;
		D3DSURFACE_DESC dstDesc;
		pCacheSurf->GetDesc(&srcDesc);
		pTargetSrc->GetDesc(&dstDesc);
		RECT srcRect;
		RECT dstRect;
		srcRect.left = 0;
		srcRect.right = srcDesc.Width * Inst.m_avParam[0].x;
		srcRect.top = 0;
		srcRect.bottom = srcDesc.Height * Inst.m_avParam[0].x;

		dstRect.left = 0;
		dstRect.right = dstDesc.Width * Inst.m_avParam[1].x;
		dstRect.top = 0;
		dstRect.bottom = dstDesc.Height * Inst.m_avParam[1].x;


		pd3dDevice->StretchRect(pCacheSurf, &srcRect, pTargetSrc, &dstRect, D3DTEXF_POINT);

		//pd3dDevice->StretchRect(pCacheSurf , NULL, pTargetSrc, NULL, D3DTEXF_LINEAR);
		SAFE_RELEASE(pCacheSurf);
		SAFE_RELEASE(pTargetSrc);
		return S_OK;
	}
	else if(bCacheToSrc != -1)
	{
		IDirect3DSurface9* pCacheSurf;
		IDirect3DSurface9* pTargetSrc;
		hr = m_pCache[bCacheToSrc]->GetSurfaceLevel( 0, &pCacheSurf );
		if( FAILED( hr ) )
			return hr;
		hr = gkTextureManager::ms_SceneTarget0->getTexture()->GetSurfaceLevel( 0, &pTargetSrc );
		if( FAILED( hr ) )
			return hr;

		D3DSURFACE_DESC srcDesc;
		D3DSURFACE_DESC dstDesc;
		pCacheSurf->GetDesc(&srcDesc);
		pTargetSrc->GetDesc(&dstDesc);
		RECT srcRect;
		RECT dstRect;
		srcRect.left = 0;
		srcRect.right = srcDesc.Width * Inst.m_avParam[0].x;
		srcRect.top = 0;
		srcRect.bottom = srcDesc.Height * Inst.m_avParam[0].x;

		dstRect.left = 0;
		dstRect.right = dstDesc.Width * Inst.m_avParam[1].x;
		dstRect.top = 0;
		dstRect.bottom = dstDesc.Height * Inst.m_avParam[1].x;


		pd3dDevice->StretchRect(pCacheSurf, &srcRect, pTargetSrc, &dstRect, D3DTEXF_POINT);

		SAFE_RELEASE(pCacheSurf);
		SAFE_RELEASE(pTargetSrc);
		return S_OK;
	}
	else if(bCurToSrc != -1)
	{
		IDirect3DSurface9* pCurSurf;
		IDirect3DSurface9* pTargetSrc;
		hr = m_RTChain[PP.m_nRenderTarget].GetPrevTarget()->GetSurfaceLevel( 0, &pCurSurf );
		if( FAILED( hr ) )
			return hr;
		hr = gkTextureManager::ms_SceneTarget0->getTexture()->GetSurfaceLevel( 0, &pTargetSrc );
		if( FAILED( hr ) )
			return hr;

		D3DSURFACE_DESC srcDesc;
		D3DSURFACE_DESC dstDesc;
		pCurSurf->GetDesc(&srcDesc);
		pTargetSrc->GetDesc(&dstDesc);
		RECT srcRect;
		RECT dstRect;
		srcRect.left = 0;
		srcRect.right = srcDesc.Width * Inst.m_avParam[0].x;
		srcRect.top = 0;
		srcRect.bottom = srcDesc.Height * Inst.m_avParam[0].x;

		dstRect.left = 0;
		dstRect.right = dstDesc.Width * Inst.m_avParam[1].x;
		dstRect.top = 0;
		dstRect.bottom = dstDesc.Height * Inst.m_avParam[1].x;


		pd3dDevice->StretchRect(pCurSurf, &srcRect, pTargetSrc, &dstRect, D3DTEXF_POINT);
		SAFE_RELEASE(pCurSurf);
		SAFE_RELEASE(pTargetSrc);
		return S_OK;
	}
	else if(bSrcToCur != -1)
	{
		IDirect3DSurface9* pSrcSurf;
		IDirect3DSurface9* pTargetSrc;
		hr = gkTextureManager::ms_SceneTarget0->getTexture()->GetSurfaceLevel( 0, &pSrcSurf );
		if( FAILED( hr ) )
			return hr;
		hr = m_RTChain[PP.m_nRenderTarget].GetNextSource()->GetSurfaceLevel( 0, &pTargetSrc );
		if( FAILED( hr ) )
			return hr;

		D3DSURFACE_DESC srcDesc;
		D3DSURFACE_DESC dstDesc;
		pSrcSurf->GetDesc(&srcDesc);
		pTargetSrc->GetDesc(&dstDesc);
		RECT srcRect;
		RECT dstRect;
		srcRect.left = 0;
		srcRect.right = srcDesc.Width * Inst.m_avParam[0].x;
		srcRect.top = 0;
		srcRect.bottom = srcDesc.Height * Inst.m_avParam[0].x;

		dstRect.left = 0;
		dstRect.right = dstDesc.Width * Inst.m_avParam[1].x;
		dstRect.top = 0;
		dstRect.bottom = dstDesc.Height * Inst.m_avParam[1].x;

		pd3dDevice->StretchRect(pSrcSurf, &srcRect, pTargetSrc, &dstRect, D3DTEXF_POINT);
		SAFE_RELEASE(pSrcSurf);
		SAFE_RELEASE(pTargetSrc);
		return S_OK;
	}

	//
	// If there are any parameters, initialize them here.
	//

	for( int i = 0; i < NUM_PARAMS; ++i )
		if( PP.m_ahParam[i] )
			PP.m_pEffect->SetVector( PP.m_ahParam[i], &Inst.m_avParam[i] );


	// Render the quad
	if( SUCCEEDED( pd3dDevice->BeginScene() ) )
	{
		PP.m_pEffect->SetTechnique(hTech);
		// Set the vertex declaration
		V( pd3dDevice->SetVertexDeclaration( m_pVertDeclPP ) );

		// Draw the quad
		UINT cPasses, p;
		PP.m_pEffect->Begin( &cPasses, 0 );
		for( p = 0; p < cPasses; ++p )
		{
			static bool bUpdateVB = false;  // Inidicates whether the vertex buffer
			// needs update for this pass.

			//
			// If the extents has been modified, the texture coordinates
			// in the quad need to be updated.
			//

 			if (bUseHalfRender)
 			{
				float uv = 1.0f / Inst.m_avParam[0].x;
				aQuad[2].tu = uv;
				aQuad[3].tu = uv;
				aQuad[1].tv = uv;
				aQuad[3].tv = uv;

				aQuad[2].tu2 = uv;
				aQuad[3].tu2 = uv;
				aQuad[1].tv2 = uv;
				aQuad[3].tv2 = uv;
				bUpdateVB = true;
 			}

			if( aQuad[2].tu != fExtentX )
			{
				aQuad[2].tu = aQuad[3].tu = fExtentX;
				bUpdateVB = true;
			}
			if( aQuad[1].tv != fExtentY )
			{
				aQuad[1].tv = aQuad[3].tv = fExtentY;
				bUpdateVB = true;
			}

			//
			// Check if the pass has annotation for extent info.  Update
			// fScaleX and fScaleY if it does.  Otherwise, default to 1.0f.
			//

			float fScaleX = 1.0f, fScaleY = 1.0f;
			D3DXHANDLE hPass = PP.m_pEffect->GetPass( PP.m_hTPostProcess, p );
			D3DXHANDLE hExtentScaleX = PP.m_pEffect->GetAnnotationByName( hPass, "fScaleX" );
			if( hExtentScaleX )
				PP.m_pEffect->GetFloat( hExtentScaleX, &fScaleX );
			D3DXHANDLE hExtentScaleY = PP.m_pEffect->GetAnnotationByName( hPass, "fScaleY" );
			if( hExtentScaleY )
				PP.m_pEffect->GetFloat( hExtentScaleY, &fScaleY );




			//
			// Now modify the quad according to the scaling values specified for
			// this pass
			//
			if( fScaleX != 1.0f )
			{
				aQuad[2].x = ( aQuad[2].x + 0.5f ) * fScaleX - 0.5f;
				aQuad[3].x = ( aQuad[3].x + 0.5f ) * fScaleX - 0.5f;
				bUpdateVB = true;
			}
			if( fScaleY != 1.0f )
			{
				aQuad[1].y = ( aQuad[1].y + 0.5f ) * fScaleY - 0.5f;
				aQuad[3].y = ( aQuad[3].y + 0.5f ) * fScaleY - 0.5f;
				bUpdateVB = true;
			}

			if ( bUseHalfRender )
			{
				aQuad[2].x = ( aQuad[2].x + 0.5f ) * Inst.m_avParam[0].x - 0.5f;
				aQuad[3].x = ( aQuad[3].x + 0.5f ) * Inst.m_avParam[0].x - 0.5f;
				aQuad[1].y = ( aQuad[1].y + 0.5f ) * Inst.m_avParam[0].x - 0.5f;
				aQuad[3].y = ( aQuad[3].y + 0.5f ) * Inst.m_avParam[0].x - 0.5f;
				bUpdateVB = true;
			}

			if( bUpdateVB )
			{
				LPVOID pVBData;
				// Scaling requires updating the vertex buffer.
				if( SUCCEEDED( pVB->Lock( 0, 0, &pVBData, D3DLOCK_DISCARD ) ) )
				{
					CopyMemory( pVBData, aQuad, 4 * sizeof( PPVERT ) );
					pVB->Unlock();
				}
				bUpdateVB = false;
			}
			fExtentX *= fScaleX;
			fExtentY *= fScaleY;


			if(!clearPrev)
			{
				m_RTChain[0].Flip();
				m_RTChain[1].Flip();
				m_RTChain[2].Flip();
			}
			// Set up the textures and the render target
			//
			for( int i = 0; i < RT_COUNT; ++i )
			{
				// If this is the very first post-process rendering,
				// obtain the source textures from the scene.
				// Otherwise, initialize the post-process source texture to
				// the previous render target.
				//
				if( m_RTChain[i].m_bFirstRender )
				{
					if ( i == 0)
						PP.m_pEffect->SetTexture( PP.m_hTexSource[i],gkTextureManager::ms_SceneTarget0->getTexture() );
					else if ( i == 1)
						PP.m_pEffect->SetTexture( PP.m_hTexSource[i],gkTextureManager::ms_SceneNormal->getTexture() );
					else if ( i == 2)
						PP.m_pEffect->SetTexture( PP.m_hTexSource[i],gkTextureManager::ms_SceneDepth->getTexture() );
				}
					
				else
					PP.m_pEffect->SetTexture( PP.m_hTexSource[i], m_RTChain[i].GetNextSource() );
			}

 			if (m_aPostProcess[PPOP_SRCDIRECT_DOF].m_pEffect == PP.m_pEffect)
 			{

 				// we got DOF here
 				PP.m_pEffect->SetTexture( PP.m_hTexSource[1], gkTextureManager::ms_SceneTargetBlur->getTexture() );

				D3DXVECTOR4 plane = D3DXVECTOR4(0.0f, 0.004f, 0.002f, 0.004f);
				plane.z *= 8 * (gEnv->p3DEngine->getSnowAmount() + 0.1f);
				PP.m_pEffect->SetVector("plane", &plane);
 			}

			//
			// Set up the new render target
			//
			IDirect3DTexture9* pTarget = m_RTChain[PP.m_nRenderTarget].GetNextTarget();
			IDirect3DSurface9* pTexSurf;
			hr = pTarget->GetSurfaceLevel( 0, &pTexSurf );
			if( FAILED( hr ) )
				return hr;

			// 
			// MSAA middle transition
			// 
			if( m_bIsMultiSample )
			{
				pd3dDevice->SetRenderTarget(0, m_pSwapSurf0);
			}
			else
			{
				pd3dDevice->SetRenderTarget( 0, pTexSurf );
			}
			// We have output to this render target. Flag it.
			m_RTChain[PP.m_nRenderTarget].m_bFirstRender = false;

			//
			// Clear the render target
			//

			// some effect can use alphablend feature, so may not clear here [10/13/2011 Kaiming]
			if (clearPrev)
			{
				pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET,
					0x00000000, 1.0f, 0L );
			}

			//
			// Render
			//
			PP.m_pEffect->BeginPass( p );
			
// 			if (hDecalsMap)
// 			{
// 				int unitNum = getSceneMngPtr()->getDecalManager().getDecalUnitNum();
// 
// 				D3DXHANDLE hTex[4];
// 				hTex[0] = PP.m_pEffect->GetParameterByName( NULL, "g_txDecals0" );
// 				hTex[1] = PP.m_pEffect->GetParameterByName( NULL, "g_txDecals1" );
// 				hTex[2] = PP.m_pEffect->GetParameterByName( NULL, "g_txDecals2" );
// 				hTex[3] = PP.m_pEffect->GetParameterByName( NULL, "g_txDecals3" );
// 
// 				for (int i=0; i < unitNum; ++i)
// 				{
// 					gkTexturePtr pTexture = gkTextureManager::getSingletonPtr()->getByName(getSceneMngPtr()->getDecalManager().getDecalTextureName(i));
// 					PP.m_pEffect->SetTexture(hTex[i], pTexture->getTexture());
// 				}
// 
// 				int nDecalNum = getSceneMngPtr()->getDecalManager().getDeferredDecalNum();
// 				PP.m_pEffect->SetInt("g_nDecalNum", nDecalNum);
// 
// 				int* pDivider = getSceneMngPtr()->getDecalManager().getDeferredDecalTextureDivide();
// 				PP.m_pEffect->SetIntArray("g_nTxDivide", pDivider, 3);
// 
// 				pd3dDevice->SetPixelShaderConstantF(0, &( getSceneMngPtr()->getDecalManager().getDeferredDecalDataCache()->x), nDecalNum*2);
// 
// 				PP.m_pEffect->CommitChanges();
// 			}

			gkRenderOperation rop;
			rop.operationType = gkRenderOperation::OT_TRIANGLE_STRIP;
			rop.vertexData = pVB;
			rop.vertexDecl = m_pVertDeclPP;
			rop.vertexStart = 0;
			rop.vertexCount = 6;
			rop.vertexSize = sizeof( PPVERT );

			gEnv->pRenderer->_render(rop);
// 			pd3dDevice->SetStreamSource( 0, pVB, 0, sizeof( PPVERT ) );
// 			pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );

			PP.m_pEffect->EndPass();

			// if MSAA enabled, StretchRect
			if (m_bIsMultiSample)
				pd3dDevice->StretchRect(m_pSwapSurf0, NULL, pTexSurf, NULL, D3DTEXF_POINT);

			SAFE_RELEASE( pTexSurf );

			// Update next rendertarget index
			m_RTChain[PP.m_nRenderTarget].Flip();

			if (bUseHalfRender)
			{
				fExtentX = 1;
				fExtentY = 1;
 				aQuad[2].x = ( aQuad[2].x + 0.5f ) / Inst.m_avParam[0].x - 0.5f;
 				aQuad[3].x = ( aQuad[3].x + 0.5f ) / Inst.m_avParam[0].x - 0.5f;
 				aQuad[1].y = ( aQuad[1].y + 0.5f ) / Inst.m_avParam[0].x - 0.5f;
 				aQuad[3].y = ( aQuad[3].y + 0.5f ) / Inst.m_avParam[0].x - 0.5f;
				aQuad[2].tu = aQuad[3].tu = 1.0f;
				aQuad[1].tv = aQuad[3].tv = 1.0f;
				aQuad[2].tu2 = aQuad[3].tu2 = 1.0f;
				aQuad[1].tv2 = aQuad[3].tv2 = 1.0f;
				bUpdateVB = true;
			}
		}
		PP.m_pEffect->End();

		// End scene
		pd3dDevice->EndScene();
	}

	return S_OK;
}


//--------------------------------------------------------------------------------------
// PerformPostProcess()
// Perform all active post-processes in order.
HRESULT gkPostProcessManager::PostPipeline( IDirect3DDevice9* pd3dDevice )
{
	HRESULT hr;

	//
	// Extents are used to control how much of the rendertarget is rendered
	// during PostProcess. For example, with the extent of 0.5 and 0.5, only
	// the upper left quarter of the rendertarget will be rendered during
	// PostProcess.
	//
	float fExtentX = 1.0f, fExtentY = 1.0f;
	const D3DSURFACE_DESC* pd3dsdBackBuffer = gEnv->pRenderer->getBackBufferDesc();

	//
	// Set up our quad
	//
	PPVERT Quad[4] =
	{
		{ -0.5f,                        -0.5f,                         1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f },
		{ -0.5,                         pd3dsdBackBuffer->Height - 0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f },
		{ pd3dsdBackBuffer->Width - 0.5f, -0.5,                          1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f },
		{ pd3dsdBackBuffer->Width - 0.5f, pd3dsdBackBuffer->Height - 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f }
	};



	//
	// Create a vertex buffer out of the quad
	//
	IDirect3DVertexBuffer9* pVB;
	hr = pd3dDevice->CreateVertexBuffer( sizeof( PPVERT ) * 4,
		D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
		0,
		D3DPOOL_DEFAULT,
		&pVB,
		NULL );
	if( FAILED( hr ) )
		return hr;

	// Fill in the vertex buffer
	LPVOID pVBData;
	if( SUCCEEDED( pVB->Lock( 0, 0, &pVBData, D3DLOCK_DISCARD ) ) )
	{
		CopyMemory( pVBData, Quad, sizeof( Quad ) );
		pVB->Unlock();
	}

	// Clear first-time render flags
	for( int i = 0; i < RT_COUNT; ++i )
		m_RTChain[i].m_bFirstRender = true;

	// post sequence:
	// decals

	// low to quat
	//float param[2] = {0.5f, 1.0f};
	//gkRoot::getSingletonPtr()->getPostProcessMngPtr()->InsertPostEffect(PPOP_STRETCH_CURTOCACHE0, 2, param);

	gEnv->pRenderer->getDevice()->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE); 

// 	if (m_bIsDeferredDecal && getSceneMngPtr()->getDecalManager().getDecalUnitNum() > 0)
// 	{
// 		CPProcInstance pTempInst;
// 		pTempInst.m_avParam[0].x = 1.0f;
// 		pTempInst.m_avParam[1].x = 1.0f;
//  		PerformSinglePostProcess( pd3dDevice,
//  			m_aPostProcess[PPOP_INTERNAL_DEFERREDDECALS],		// decal
//  			pTempInst,
//  			pVB,
//  			Quad,
//  			fExtentX,
//  			fExtentY );
// 
// 
// 
// 		PerformSinglePostProcess( pd3dDevice,
// 			m_aPostProcess[PPOP_STRETCH_CURTOSRC],		// decal
// 			pTempInst,
// 			pVB,
// 			Quad,
// 			fExtentX,
// 			fExtentY );
//  	}
#if 0
	// add a HDR change pass [6/2/2011 Kaiming-Desktop]
	//if (gEnv->pCVManager->sw_HDRMode != 0)
	if(0)
	{
		CPProcInstance pTempInst;
		pTempInst.m_avParam[0].x = 1.0f;
		pTempInst.m_avParam[1].x = 1.0f;

		PerformSinglePostProcess( pd3dDevice,
			m_aPostProcess[PPOP_BRIGHTPASS],		// down4x
			pTempInst,
			pVB,
			Quad,
			fExtentX,
			fExtentY );

		PerformSinglePostProcess( pd3dDevice,
			m_aPostProcess[PPOP_DOWN4X],		// down4x
			pTempInst,
			pVB,
			Quad,
			fExtentX,
			fExtentY );


		PerformSinglePostProcess( pd3dDevice,
			m_aPostProcess[PPOP_BLOOMH],		// down4x
			pTempInst,
			pVB,
			Quad,
			fExtentX,
			fExtentY );

		PerformSinglePostProcess( pd3dDevice,
			m_aPostProcess[PPOP_BLOOMV],		// down4x
			pTempInst,
			pVB,
			Quad,
			fExtentX,
			fExtentY );


		PerformSinglePostProcess( pd3dDevice,
			m_aPostProcess[PPOP_UP4X],		// down4x
			pTempInst,
			pVB,
			Quad,
			fExtentX,
			fExtentY );

		if (gEnv->pCVManager->sw_HDRMode == 2)
		{
			return S_OK;
		}

		PerformSinglePostProcess( pd3dDevice,
			m_aPostProcess[PPOP_HDR_FINALPASS],		// down4x
			pTempInst,
			pVB,
			Quad,
			fExtentX,
			fExtentY );

		//return S_OK;

		PerformSinglePostProcess( pd3dDevice,
			m_aPostProcess[PPOP_STRETCH_CURTOSRC],		// to src
			pTempInst,
			pVB,
			Quad,
			fExtentX,
			fExtentY );
	}

	

	//if (m_ySSAOmode != PPSSAO_NONE)
	if (0)
	{
		CPProcInstance pTempInst;
		switch(m_ySSAOmode)
		{
		case PPSSAO_QUAT:
			pTempInst.m_avParam[0].x = 0.25f;
			pTempInst.m_avParam[1].x = 0.25f;
			break;
		case PPSSAO_HALF:
			pTempInst.m_avParam[0].x = 0.5f;
			pTempInst.m_avParam[1].x = 0.5f;
			break;
		case PPSSAO_SHOW:
		case PPSSAO_FULL:
			pTempInst.m_avParam[0].x = 1.0f;
			pTempInst.m_avParam[1].x = 1.0f;
			break;

		}
		PerformSinglePostProcess( pd3dDevice,
			m_aPostProcess[PPOP_INTERNAL_SSAO],		// ssao
			pTempInst,
			pVB,
			Quad,
			fExtentX,
			fExtentY );

		PerformSinglePostProcess( pd3dDevice,
			m_aPostProcess[PPOP_SSAOBLUR],		// ssao
			pTempInst,
			pVB,
			Quad,
			fExtentX,
			fExtentY );

		if (m_ySSAOmode == PPSSAO_SHOW)
		{
			pVB->Release();
			return S_OK;
		}

 		pTempInst.m_avParam[1].x = 1.0f;
 		PerformSinglePostProcess( pd3dDevice,
 			m_aPostProcess[PPOP_STRETCH_CURTOCACHE0],		// to 2x
 			pTempInst,
 			pVB,
 			Quad,
 			fExtentX,
 			fExtentY );
 
 		pTempInst.m_avParam[0].x = 1.0f;
 		pTempInst.m_avParam[1].x = 1.0f;
 		PerformSinglePostProcess( pd3dDevice,
 			m_aPostProcess[PPOP_STRETCH_CURFROMCACHE0],		// to cur
 			pTempInst,
 			pVB,
 			Quad,
 			fExtentX,
 			fExtentY );

		PerformSinglePostProcess( pd3dDevice,
			m_aPostProcess[PPOP_SRCDIRECT_MULTIPLY],		// multiply with src
			pTempInst,
			pVB,
			Quad,
			fExtentX,
			fExtentY );

		PerformSinglePostProcess( pd3dDevice,
			m_aPostProcess[PPOP_STRETCH_CURTOSRC],		// to src
			pTempInst,
			pVB,
			Quad,
			fExtentX,
			fExtentY );
	}


	if (m_bIsShadowMap && m_bIsShadowMapSoften)
	{
		CPProcInstance pTempInst;
		PerformSinglePostProcess( pd3dDevice,
			m_aPostProcess[PPOP_INTERNAL_SHADOWMAP],		// ssao
			pTempInst,
			pVB,
			Quad,
			fExtentX,
			fExtentY );

		pTempInst.m_avParam[0].x = 1.0f;
		pTempInst.m_avParam[1].x = 1.0f;
		PerformSinglePostProcess( pd3dDevice,
			m_aPostProcess[PPOP_SRCDIRECT_MULTIPLY],		// to 2x
			pTempInst,
			pVB,
			Quad,
			fExtentX,
			fExtentY );

		PerformSinglePostProcess( pd3dDevice,
			m_aPostProcess[PPOP_STRETCH_CURTOSRC],		// to 2x
			pTempInst,
			pVB,
			Quad,
			fExtentX,
			fExtentY );
	}
#endif

	// generate scene blured here [10/11/2011 Kaiming]
	//SceneBlurDownScalePass(pd3dDevice);

// 	{
// 		CPProcInstance pTempInst;
// 		pTempInst.m_avParam[0].x = 1.0f;
// 		pTempInst.m_avParam[1].x = 1.0f;
// 		PerformSinglePostProcess( pd3dDevice,
// 			m_aPostProcess[PPOP_EDGEDETECTED],		// to 2x
// 			pTempInst,
// 			pVB,
// 			Quad,
// 			fExtentX,
// 			fExtentY );
// 	}

	// DO DOF AND EDGEAA
//  	{
//  		CPProcInstance pTempInst;
//  		pTempInst.m_avParam[0].x = 1.0f;
//  		pTempInst.m_avParam[1].x = 1.0f;
//  		PerformSinglePostProcess( pd3dDevice,
//  			m_aPostProcess[PPOP_SRCDIRECT_DOF],		// to 2x
//  			pTempInst,
//  			pVB,
//  			Quad,
//  			fExtentX,
//  			fExtentY );
//  	}

	// DO OUT GLOW SILHOUTTE
// 	{
// 		CPProcInstance pTempInst;
// 		pTempInst.m_avParam[0].x = 1.0f;
// 		pTempInst.m_avParam[1].x = 1.0f;
// 		PerformSinglePostProcess( pd3dDevice,
// 			m_aPostProcess[PPOP_OBJECTSPEC_EFFECT],		// to 2x
// 			pTempInst,
// 			pVB,
// 			Quad,
// 			fExtentX,
// 			fExtentY );
// 	}


//#endif
	// Perform post processing
	// The last (blank) item has special purpose so do not process it.
	for( int nEffIndex = 0; nEffIndex != m_vectorPPInstance.size(); ++nEffIndex )
	{
		CPProcInstance* pInstance = m_vectorPPInstance[nEffIndex];
		PerformSinglePostProcess( pd3dDevice,
			m_aPostProcess[pInstance->m_nFxIndex],
			*pInstance,
			pVB,
			Quad,
			fExtentX,
			fExtentY );
	}

	// finally vtx overlay process
// 
//  	if (getVtxLayerMngPtr()->getEnable())
//  	{
//  		CPProcInstance pTempInst;
//  		PerformSinglePostProcess( pd3dDevice,
//  			m_aPostProcess[PPOP_VTXOVERLAYBLEND],		// ssao
//  			pTempInst,
//  			pVB,
//  			Quad,
//  			fExtentX,
//  			fExtentY );
//  	}
	// Release the vertex buffer
	pVB->Release();

	return S_OK;
}

HRESULT gkPostProcessManager::SceneBlurDownScalePass( IDirect3DDevice9* pd3dDevice )
{
	HRESULT hr;

	// get the prev RT, set the light pass
	IDirect3DSurface9* pSceneBlurredSurf = NULL;
	IDirect3DSurface9* pRTBackup = NULL;

	m_pSceneBlurred->GetSurfaceLevel(0, &pSceneBlurredSurf);

	pd3dDevice->GetRenderTarget(0, &pRTBackup);
	pd3dDevice->SetRenderTarget( 0, pSceneBlurredSurf );

	// DO NOT WRITE Z ! SO WE CAN RENDER AUX GEOM AFTER THIS!
	//  [8/21/2011 Kaiming-Desktop]
	gEnv->pRenderer->getDevice()->SetRenderState(D3DRS_ZWRITEENABLE, FALSE); 

	const D3DSURFACE_DESC* pd3dsdBackBuffer = gEnv->pRenderer->getBackBufferDesc();

	if( SUCCEEDED( pd3dDevice->BeginScene() ) )
	{
		// Render a screen-sized quad
		PPVERT quad[4] =
		{
			{ -0.5f,                          -0.5f,                           0.5f, 1.0f, 0.0f, 0.0f, 0.0f,
			0.0f },
			{ pd3dsdBackBuffer->Width - 0.5f, -0.5f,                           0.5f, 1.0f, 2.0f, 0.0f, 0.0f,
			0.0f },
			{ -0.5f,                          pd3dsdBackBuffer->Height - 0.5f, 0.5f, 1.0f, 0.0f, 2.0f, 0.0f,
			0.0f },
			{ pd3dsdBackBuffer->Width - 0.5f, pd3dsdBackBuffer->Height - 0.5f, 0.5f, 1.0f, 2.0f, 2.0f, 0.0f, 0.0f }
		};

		//
		// Create a vertex buffer out of the quad
		//
		IDirect3DVertexBuffer9* pVB;
		hr = pd3dDevice->CreateVertexBuffer( sizeof( PPVERT ) * 4,
			D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
			0,
			D3DPOOL_DEFAULT,
			&pVB,
			NULL );
		if( FAILED( hr ) )
			return hr;

		// Fill in the vertex buffer
		LPVOID pVBData;
		if( SUCCEEDED( pVB->Lock( 0, 0, &pVBData, D3DLOCK_DISCARD ) ) )
		{
			CopyMemory( pVBData, quad, sizeof( quad ) );
			pVB->Unlock();
		}

		V( pd3dDevice->SetVertexDeclaration( m_pVertDeclPP ) );
		V( m_pEffect->SetTexture( "g_txSceneColor", gkTextureManager::ms_SceneTarget0->getTexture() ));
	// 	V( m_pEffect->SetTexture( "g_txSceneNormal", gkTextureManager::ms_SceneTarget1 ) );
	// 	V( m_pEffect->SetTexture( "g_txScenePosition", gkTextureManager::ms_SceneTarget2 ) );


		// 4 samples
	// 	{0.494679, 	0.340907},
	// 	{0.340907, 	-0.494679},
	// 	{-0.494679, 	-0.340907},
	// 	{-0.340907, 	0.494679},

		float screenWidth = m_pRenderer->GetScreenWidth() * 0.25f;
		float screenHeight = m_pRenderer->GetScreenHeight() * 0.25f;

		D3DXVECTOR4  texelKernel0 = D3DXVECTOR4(0.494679f / screenWidth, 0.340907f / screenHeight,
									 0.340907f / screenWidth, -0.494679f / screenHeight);
		D3DXVECTOR4  texelKernel1 = D3DXVECTOR4(-0.494679f / screenWidth, -0.340907f / screenHeight,
									 -0.340907f / screenWidth, 0.494679f / screenHeight);

		m_pEffect->SetVector("TexelKernel0", &texelKernel0 );
		m_pEffect->SetVector("TexelKernel1", &texelKernel1 );
		m_pEffect->SetTechnique( "RenderBlurredPass" );

		UINT cPasses;
		V( m_pEffect->Begin( &cPasses, 0 ) );
		for( UINT p = 0; p < cPasses; ++p )
		{
			V( m_pEffect->BeginPass( p ) );

			gkRenderOperation rop;
			rop.operationType = gkRenderOperation::OT_TRIANGLE_STRIP;
			rop.vertexData = pVB;
			rop.vertexDecl = m_pVertDeclPP;
			rop.vertexStart = 0;
			rop.vertexCount = 6;
			rop.vertexSize = sizeof( PPVERT );

			gEnv->pRenderer->_render(rop);

			// 			pd3dDevice->SetStreamSource( 0, pVB, 0, sizeof( PPVERT ) );
			// 			pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
			V( m_pEffect->EndPass() );
		}
		V( m_pEffect->End() );

		// Release the vertex buffer
		pVB->Release();
		// End scene
		pd3dDevice->EndScene();
	}

	//////////////////////////////////////////////////////////////////////////
	// restore the prev RT

	pd3dDevice->SetRenderTarget( 0, pRTBackup );

	SAFE_RELEASE( pSceneBlurredSurf );
	SAFE_RELEASE( pRTBackup );

	// DO NOT WRITE Z! [8/21/2011 Kaiming-Desktop]
	gEnv->pRenderer->getDevice()->SetRenderState(D3DRS_ZWRITEENABLE, TRUE); 


	return S_OK;
}

HRESULT gkPostProcessManager::LightPass( IDirect3DDevice9* pd3dDevice, const gkRenderLightList& LightList )
{
	HRESULT hr;
	
	PROFILE_LABEL_PUSH( "DEFERRED_LIGHTING" );
	gkRendererD3D9::ms_GPUTimers[L"Deferred Lighting"].start();

	// DO NOT WRITE Z ! SO WE CAN RENDER AUX GEOM AFTER THIS!
	//  [8/21/2011 Kaiming-Desktop]
	gEnv->pRenderer->getDevice()->SetRenderState(D3DRS_ZWRITEENABLE, FALSE); 


	PROFILE_LABEL_PUSH( "AMBIENT & SUN" );
	gkRendererD3D9::ms_GPUTimers[L"Ambient Pass"].start();
	
	//////////////////////////////////////////////////////////////////////////
	// 1. first, Ambient & Sun pass

	const D3DSURFACE_DESC* pd3dsdBackBuffer = gEnv->pRenderer->getBackBufferDesc();
		// Render a screen-sized quad
		PPVERT quad[4] =
		{
			{ -0.5f,                          -0.5f,                           0.5f, 1.0f, 0.0f, 0.0f, 0.0f,
			0.0f },
			{ pd3dsdBackBuffer->Width - 0.5f, -0.5f,                           0.5f, 1.0f, 1.0f, 0.0f, 0.0f,
			0.0f },
			{ -0.5f,                          pd3dsdBackBuffer->Height - 0.5f, 0.5f, 1.0f, 0.0f, 1.0f, 0.0f,
			0.0f },
			{ pd3dsdBackBuffer->Width - 0.5f, pd3dsdBackBuffer->Height - 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f }
		};

		//
		// Create a vertex buffer out of the quad
		//
		IDirect3DVertexBuffer9* pVB;
		hr = pd3dDevice->CreateVertexBuffer( sizeof( PPVERT ) * 4,
			D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
			0,
			D3DPOOL_DEFAULT,
			&pVB,
			NULL );
		if( FAILED( hr ) )
			return hr;

		// Fill in the vertex buffer
		LPVOID pVBData;
		if( SUCCEEDED( pVB->Lock( 0, 0, &pVBData, D3DLOCK_DISCARD ) ) )
		{
			CopyMemory( pVBData, quad, sizeof( quad ) );
			pVB->Unlock();
		}

		V( pd3dDevice->SetVertexDeclaration( m_pVertDeclPP ) );
		V( m_pEffect->SetTexture( "g_txSceneColor", gkTextureManager::ms_SceneTarget0->getTexture() ) );
		V( m_pEffect->SetTexture( "g_txSceneNormal", gkTextureManager::ms_SceneNormal->getTexture() ) );
		V( m_pEffect->SetTexture( "g_txScenePosition", gkTextureManager::ms_SceneDepth->getTexture() ) );
		gkTextureManager::ms_SSAOTarget->Apply(4,0);
		gkTextureManager::ms_ShadowMask->Apply(5,0);

		Vec3 lightdirInWorld = m_pRenderer->getShaderContent().getLightDirViewSpace();
		

		Vec4 sunParam0(lightdirInWorld, m_fSunIntensive);
		//sunParam0.w = m_fSunIntensive;
		V( m_pEffect->SetValue( "g_LightDir", &sunParam0, sizeof(Vec4) ) );
		V( m_pEffect->SetValue( "g_mViewI", &(m_pRenderer->getShaderContent().getInverseViewMatrix()), sizeof(Matrix44) ));

		const STimeOfDayKey& tod = gEnv->p3DEngine->getTimeOfDay()->getCurrentTODKey();
		V( m_pEffect->SetValue( "g_Ambient", &(tod.clSkyLight), sizeof(ColorF) ) );
		V( m_pEffect->SetValue( "g_cAmbGround", &(tod.clGroundAmb), sizeof(ColorF) ) );
		Vec4 ambHeightParam;
		ambHeightParam.x = tod.fGroundAmbMin;
		ambHeightParam.y = tod.fGroundAmbMax;
		ambHeightParam.z = 1.0f / tod.fGroundAmbMax;
		V( m_pEffect->SetValue( "g_vAmbHeightParams", &ambHeightParam, sizeof(Vec4) ) );
		//"g_Ambient"
		//"g_cAmbGround"
		m_pEffect->CommitChanges();

		m_pEffect->SetTechnique( "RenderLightPass" );

		UINT cPasses;
		V( m_pEffect->Begin( &cPasses, 0 ) );
		for( UINT p = 0; p < cPasses; ++p )
		{
			V( m_pEffect->BeginPass( p ) );

			gkRenderOperation rop;
			rop.operationType = gkRenderOperation::OT_TRIANGLE_STRIP;
			rop.vertexData = pVB;
			rop.vertexDecl = m_pVertDeclPP;
			rop.vertexStart = 0;
			rop.vertexCount = 6;
			rop.vertexSize = sizeof( PPVERT );

			gEnv->pRenderer->_render(rop);

			// 			pd3dDevice->SetStreamSource( 0, pVB, 0, sizeof( PPVERT ) );
			// 			pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
			V( m_pEffect->EndPass() );
		}
		V( m_pEffect->End() );

		// Release the vertex buffer
		pVB->Release();

	gkRendererD3D9::ms_GPUTimers[L"Ambient Pass"].stop();
	PROFILE_LABEL_POP( "AMBIENT & SUN" );
	//////////////////////////////////////////////////////////////////////////
	// 2. next, PointLight pass


	gkRendererD3D9::ms_GPUTimers[L"Lights"].start();
	m_pEffect->SetTechnique( "RenderSingleLightPass" );
	
	V( m_pEffect->Begin( &cPasses, 0 ) );
	for( UINT p = 0; p < cPasses; ++p )
	{
		V( m_pEffect->BeginPass( p ) );

		for(gkRenderLightList::const_iterator it = LightList.begin(); it != LightList.end(); ++it)
		{

			PROFILE_LABEL_PUSH( "LIGHT" );
			// build matrix
			Matrix34 mat;
 			mat.Set(Vec3((*it).m_fRadius, (*it).m_fRadius, (*it).m_fRadius),
 				Quat::CreateIdentity(), (*it).m_vPos);

// 			mat.Set(Vec3(1,1,1),
// 				Quat::CreateIdentity(), (*it).m_vPos);
			Matrix44 matForRender(mat);
			matForRender.Transpose();

			matForRender = matForRender * m_pRenderer->getShaderContent().getViewMatrix();
			V( m_pEffect->SetValue("g_mWorldView", &matForRender, sizeof(Matrix44) ));
			matForRender = matForRender * m_pRenderer->getShaderContent().getProjectionMatrix();
			V( m_pEffect->SetValue("g_mWorldViewProj", &matForRender, sizeof(Matrix44) ));
			V( m_pEffect->SetValue("g_mProj", &(m_pRenderer->getShaderContent().getProjectionMatrix()), sizeof(Matrix44) ));

			Vec3 lightdirInWorld = m_pRenderer->getShaderContent().getLightDirViewSpace();

			ColorF lightParam0 = (*it).m_vDiffuse;
			lightParam0.a = (*it).m_fRadius;
			V( m_pEffect->SetValue( "g_LightDiffuse", &lightParam0, sizeof(ColorF) ) );

			if (it->m_bFakeShadow)
			{
				V( m_pEffect->SetBool( "bFakeShadow", true ) );
			}
			else
			{
				V( m_pEffect->SetBool( "bFakeShadow", false ) );
			}
			

			gkTexturePtr texRandomStep = gEnv->pSystem->getTextureMngPtr()->load(L"randomstep.dds", L"internal");
			texRandomStep->Apply(3, 0);

			m_pEffect->CommitChanges();

			// build render operation
			gkRenderOperation rop;
			rop.operationType = gkRenderOperation::OT_D3DXMESH;
			rop.mesh = m_pLightSphere;

			gEnv->pRenderer->_render(rop);

			PROFILE_LABEL_POP( "LIGHT" );
		}


		V( m_pEffect->EndPass() );
	}
	V( m_pEffect->End() );

	gkRendererD3D9::ms_GPUTimers[L"Lights"].stop();


	gkRendererD3D9::ms_GPUTimers[L"Deferred Lighting"].stop();
	PROFILE_LABEL_POP( "DEFERRED_LIGHTING" );

	PROFILE_LABEL_PUSH( "FORWARD_SHADING" );
	PROFILE_LABEL_PUSH( "OPAQUE" );

	// DO NOT WRITE Z! [8/21/2011 Kaiming-Desktop]
	gEnv->pRenderer->getDevice()->SetRenderState(D3DRS_ZWRITEENABLE, TRUE); 
	gkRendererD3D9::ms_GPUTimers[L"Opaque"].start();
	return S_OK;
}

HRESULT gkPostProcessManager::PostRender( IDirect3DDevice9* pd3dDevice )
{
	// 如果关闭了pp，直接放弃渲染
	if( !m_bEnablePostProc )
		return S_OK;

	HRESULT hr;
	//
	// Swap the chains
	//
	// 为COLOR NORMAL POSITION交换RT链
	int i;
	for( i = 0; i < RT_COUNT; ++i )
		m_RTChain[i].Flip();

	// Reset all render targets used besides RT 0
	// 重置除了RT0之外的所有RT（释放）
	for( i = 1; i < m_nRtUsed; ++i )
		pd3dDevice->SetRenderTarget( i, NULL );


	

	//
	// Perform post-processes
	//
	// 后期处理效果开始
	//bool bPerformPostProcess = (m_vectorPPInstance.size() > 0) ;
	//if( bPerformPostProcess )
		PostPipeline( pd3dDevice );

	// Restore old render target 0 (back buffer)
	m_pRenderer->FX_RestoreBackBuffer();
	//
	// Get the final result image onto the backbuffer
	//

	// DO NOT WRITE Z ! SO WE CAN RENDER AUX GEOM AFTER THIS!
	//  [8/21/2011 Kaiming-Desktop]
	gEnv->pRenderer->getDevice()->SetRenderState(D3DRS_ZWRITEENABLE, FALSE); 

	const D3DSURFACE_DESC* pd3dsdBackBuffer = gEnv->pRenderer->getBackBufferDesc();
	if( SUCCEEDED( pd3dDevice->BeginScene() ) )
	{
		// Render a screen-sized quad
		PPVERT quad[4] =
		{
			{ -0.5f,                          -0.5f,                           0.5f, 1.0f, 0.0f, 0.0f, 0.0f,
			0.0f },
			{ pd3dsdBackBuffer->Width - 0.5f, -0.5f,                           0.5f, 1.0f, 1.0f, 0.0f, 0.0f,
			0.0f },
			{ -0.5f,                          pd3dsdBackBuffer->Height - 0.5f, 0.5f, 1.0f, 0.0f, 1.0f, 0.0f,
			0.0f },
			{ pd3dsdBackBuffer->Width - 0.5f, pd3dsdBackBuffer->Height - 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f }
		};

		//
		// Create a vertex buffer out of the quad
		//
		IDirect3DVertexBuffer9* pVB;
		hr = pd3dDevice->CreateVertexBuffer( sizeof( PPVERT ) * 4,
			D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
			0,
			D3DPOOL_DEFAULT,
			&pVB,
			NULL );
		if( FAILED( hr ) )
			return hr;

		// Fill in the vertex buffer
		LPVOID pVBData;
		if( SUCCEEDED( pVB->Lock( 0, 0, &pVBData, D3DLOCK_DISCARD ) ) )
		{
			CopyMemory( pVBData, quad, sizeof( quad ) );
			pVB->Unlock();
		}


		IDirect3DTexture9* pPrevTarget=NULL;
		//if(bPerformPostProcess)
			pPrevTarget = m_RTChain[0].GetPrevTarget();
		//else 
		//	pPrevTarget = gkTextureManager::ms_SceneTarget0;

		V( pd3dDevice->SetVertexDeclaration( m_pVertDeclPP ) );
		V( m_pEffect->SetTexture( "g_txSceneColor", gkTextureManager::ms_SceneTarget0->getTexture() ) );

// 		gkTexturePtr texVignetting = gEnv->pSystem->getTextureMngPtr()->load(L"vignetting.dds", L"internal");
// 		V( m_pEffect->SetTexture( "g_txVignetting", texVignetting->getTexture() ) );

		// set the adjust value
		const STimeOfDayKey& tod = gEnv->p3DEngine->getTimeOfDay()->getCurrentTODKey();
// 		float4 ColorGradingParams0 = float4(10,0.95f,220,0);
// 		float4 ColorGradingParams1 = float4(255,0.8f,0,0);

		Vec4 ColorGradingParams0 = Vec4(tod.fLevelInputMin, tod.fLevel,tod.fLeveInputMax,0);
		Vec4 ColorGradingParams1 = Vec4(255,tod.fSaturate,0,0);

		m_pEffect->SetValue( "ColorGradingParams0", &ColorGradingParams0, sizeof(Vec4));
		m_pEffect->SetValue( "ColorGradingParams1", &ColorGradingParams1, sizeof(Vec4));

		m_pEffect->SetTechnique( "RenderNoLight" );

		UINT cPasses;
		V( m_pEffect->Begin( &cPasses, 0 ) );
		for( UINT p = 0; p < cPasses; ++p )
		{
			V( m_pEffect->BeginPass( p ) );

			gkRenderOperation rop;
			rop.operationType = gkRenderOperation::OT_TRIANGLE_STRIP;
			rop.vertexData = pVB;
			rop.vertexDecl = m_pVertDeclPP;
			rop.vertexStart = 0;
			rop.vertexCount = 6;
			rop.vertexSize = sizeof( PPVERT );

			gEnv->pRenderer->_render(rop);

// 			pd3dDevice->SetStreamSource( 0, pVB, 0, sizeof( PPVERT ) );
// 			pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
			V( m_pEffect->EndPass() );
		}
		V( m_pEffect->End() );

		// Release the vertex buffer
		pVB->Release();
		V( pd3dDevice->EndScene());


	}


	// DO NOT WRITE Z! [8/21/2011 Kaiming-Desktop]
	gEnv->pRenderer->getDevice()->SetRenderState(D3DRS_ZWRITEENABLE, TRUE); 


	return S_OK;
}
//-----------------------------------------------------------------------
void gkPostProcessManager::InsertPostEffect( int iIndex, int argc, float argv[] )
{
	CPProcInstance* pNewInst = new CPProcInstance;
	pNewInst->m_nFxIndex = iIndex;
	for( int i=0; i<NUM_PARAMS; i++ )
	{
		if(i < argc)
		{
			pNewInst->m_avParam[i].x = argv[i];
		}
		else
		{
			pNewInst->m_avParam[i] = m_aPostProcess[iIndex].m_avParamDef[i];
		}
	}
	m_vectorPPInstance.push_back(pNewInst);

	SetEnable(true);
}
//-----------------------------------------------------------------------
void gkPostProcessManager::RemoveAllPostEffect()
{
	for( int nEffIndex = 0; nEffIndex != m_vectorPPInstance.size(); ++nEffIndex )
	{
		SAFE_DELETE(m_vectorPPInstance[nEffIndex]);
	}
	m_vectorPPInstance.clear();

	SetEnable(false);
}
//-----------------------------------------------------------------------
void gkPostProcessManager::OnLostDevice()
{
	if( m_pEffect )
		m_pEffect->OnLostDevice();

	for( int p = 0; p < m_ppCount; ++p )
		m_aPostProcess[p].OnLostDevice();

	SAFE_RELEASE( m_pLightSphere );

	// Release the scene save and render target textures
	for( int i = 0; i < RT_COUNT; ++i )
	{
		//SAFE_RELEASE( m_pSceneSave[i] );
		m_RTChain[i].Cleanup();
	}

	for (int i = 0; i < CACHE_COUNT; ++i)
	{
		SAFE_RELEASE( m_pCache[i] );
	}

// 	for (int i = 0; i < HDRCACHE_COUNT; ++i)
// 	{
// 		SAFE_RELEASE( m_pLuminaceCache[i] );
// 	}
// 
// 	for (int i = 0; i < HDREYEADAPTCACHE_COUNT; ++i)
// 	{
// 		SAFE_RELEASE( m_pEyeAdaptCache[i] );
// 	}

//	SAFE_RELEASE( m_pSceneBlurred );
// 	SAFE_RELEASE( m_pLightDiffuse );
// 	SAFE_RELEASE( m_pLightSpec );

//	SAFE_RELEASE( m_pRandomTexture );
		
	// Release the RT table's references
	for( int p = 0; p < RT_COUNT; ++p )
		for( int rt = 0; rt < RT_COUNT; ++rt )
			SAFE_RELEASE( m_aRtTable[p].pRT[rt] );

	SAFE_RELEASE( m_pSwapSurf0 );
	SAFE_RELEASE( m_pSwapSurf1 );
	SAFE_RELEASE( m_pSwapSurf2 );
}
//-----------------------------------------------------------------------
void gkPostProcessManager::OnDestroyDevice()
{
	SAFE_RELEASE( m_pEffect );
	SAFE_RELEASE( m_pVertDeclPP );

	for( int p = 0; p < m_ppCount; ++p )
		m_aPostProcess[p].Cleanup();

// 	for (int i = 0; i < CACHE_COUNT; ++i)
// 	{
// 		SAFE_RELEASE( m_pCache[i] );
// 	}

//	SAFE_RELEASE( m_pRandomTexture );

	// Release the RT table's references
	for( int p = 0; p < RT_COUNT; ++p )
		for( int rt = 0; rt < RT_COUNT; ++rt )
			SAFE_RELEASE( m_aRtTable[p].pRT[rt] );

	SAFE_RELEASE( m_pSwapSurf0 );
	SAFE_RELEASE( m_pSwapSurf1 );
	SAFE_RELEASE( m_pSwapSurf2 );

	for(UINT i=0;i<m_vectorPPInstance.size();i++)
	{
		SAFE_DELETE( m_vectorPPInstance[i] );
	}
}
//-----------------------------------------------------------------------
gkPostProcessManager::gkPostProcessManager( gkRendererD3D9* creator )
{
	m_pRenderer = creator;

	m_pVertDeclPP = NULL;									// Vertex decl for post-processing
	m_bEnablePostProc = false;								// Whether or not to enable post-processing

	m_nPasses = 0;											// Number of passes required to render scene
	m_nRtUsed = 0;											// Number of simultaneous RT used to render scene

	m_pSwapSurf0 = NULL;
	m_pSwapSurf1 = NULL;
	m_pSwapSurf2 = NULL;

	m_ppOldRT=NULL;

	m_ySSAOmode = PPSSAO_HALF;
	m_bIsDeferredDecal = 1;
	m_bIsShadowMap = 1;
	m_bIsShadowMapSoften = 1;

	m_fShadowDepth = 0.8f;

	memset(m_pCache, 0, sizeof(m_pCache));

	memset(m_aRtTable, 0, sizeof(m_aRtTable));
}
//-----------------------------------------------------------------------
gkPostProcessManager::~gkPostProcessManager( void )
{

}
//-----------------------------------------------------------------------
void gkPostProcessManager::PrepareRenderTarget( gkRenderTarget* pRT )
{
	// 如果关闭了pp，直接放弃截获
	if( !m_bEnablePostProc )
		return;


	m_ppOldRT = pRT->_getRenderSurface();

// 	if(m_bIsMultiSample)
// 		pRT->_setRenderSurface(&m_pSwapSurf0, &m_pSwapSurf1, &m_pSwapSurf2);
// 	else
// 		pRT->_setRenderSurface(&(m_aRtTable[0].pRT[1]), &(m_aRtTable[0].pRT[2])/*, &(m_aRtTable[0].pRT[2])*/); // only for zPrepass, so 2 RT is enough [10/9/2011 Kaiming]

	m_pRenderer->FX_PushRenderTarget(0, gkTextureManager::ms_SceneNormal);
	m_pRenderer->FX_PushRenderTarget(1, gkTextureManager::ms_SceneDepth);
}
//-----------------------------------------------------------------------
void gkPostProcessManager::RestoreRenderTarget( gkRenderTarget* pRT )
{
	// 如果关闭了pp，直接放弃截获
	if( !m_bEnablePostProc )
		return;

	// restore
	//pRT->_setRenderSurface(m_ppOldRT, NULL, NULL);

	m_pRenderer->FX_PopRenderTarget(0);
	m_pRenderer->FX_PopRenderTarget(1);
}

void gkPostProcessManager::DrawFullScreenQuad(int nTexWidth, int nTexHeight, Vec4& region )
{
		float texWidth = (float)nTexWidth;
		float texHeight = (float)nTexHeight;

		// Render a screen-sized quad
		PPVERT quad[4] =	
		{
			{ -0.5f + region.x * texWidth,              -0.5f + region.y * texHeight,		1.0f, 1.0f, 0.0f,	0.0f, 0.0f, 0.0f },
			{ -0.5f + region.x * texWidth,				region.w * texHeight - 0.5f,		1.0f, 1.0f, 0.0f,	1.0f, 0.0f, 1.0f },
			{ region.z * texWidth - 0.5f,				-0.5 + region.y * texHeight,		1.0f, 1.0f, 1.0f,	0.0f, 1.0f, 0.0f },
			{ region.z * texWidth - 0.5f,				region.w * texHeight - 0.5f,		1.0f, 1.0f, 1.0f,	1.0f, 1.0f, 1.0f }
		};

		//
		// Create a vertex buffer out of the quad
		//
		IDirect3DVertexBuffer9* pVB;
		m_pDevice->CreateVertexBuffer( sizeof( PPVERT ) * 4,
			D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
			0,
			D3DPOOL_DEFAULT,
			&pVB,
			NULL );

		// Fill in the vertex buffer
		LPVOID pVBData;
		if( SUCCEEDED( pVB->Lock( 0, 0, &pVBData, D3DLOCK_DISCARD ) ) )
		{
			CopyMemory( pVBData, quad, sizeof( quad ) );
			pVB->Unlock();
		}

		//m_pDevice->SetVertexDeclaration( m_pVertDeclPP );

			gkRenderOperation rop;
			rop.operationType = gkRenderOperation::OT_TRIANGLE_STRIP;
			rop.vertexData = pVB;
			rop.vertexDecl = m_pVertDeclPP;
			rop.vertexStart = 0;
			rop.vertexCount = 6;
			rop.vertexSize = sizeof( PPVERT );

			gEnv->pRenderer->_render(rop);

		// Release the vertex buffer
		pVB->Release();

}

void gkPostProcessManager::DrawFullScreenQuad( gkTexturePtr targetTex )
{
	DrawFullScreenQuad(targetTex->getWidth(), targetTex->getHeight());
}

void gkPostProcessManager::DrawScreenQuad( Vec4& region )
{
	DrawFullScreenQuad( m_pRenderer->GetScreenWidth(), m_pRenderer->GetScreenHeight(), region);
}

//-----------------------------------------------------------------------
void gkRenderTargetChain::Cleanup()
{
	SAFE_RELEASE( m_pRenderTarget[0] );
	SAFE_RELEASE( m_pRenderTarget[1] );
}



