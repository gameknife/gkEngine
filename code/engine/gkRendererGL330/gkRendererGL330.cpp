#include "StableHeader.h"
#include "gkRendererGL330.h"
#include "IResourceManager.h"
#include "RenderRes/gkTextureManager.h"
#include "RenderRes/gkMaterialManager.h"
#include "RenderRes/gkMeshManager.h"
#include "RenderRes/gkShaderManager.h"
#include "RenderRes/gkMeshGLES2.h"
#include "gkAuxRenderer.h"
#include "gk_Camera.h"
#include "RenderCamera.h"
#include "gkRenderSequence.h"
#include "ICamera.h"
#include "RenderRes/gkShader.h"
#include "RenderRes/gkTextureGLES2.h"
#include "RendererCVars.h"

#ifdef OS_WIN32
#include "Platform/Win32/gkRenderContextWin32.h"
#else
#include "Platform/OSX/gkRenderContextOSX.h"
#endif
IResourceManager*	gkRendererGL330::m_pTextureManager = NULL;
IResourceManager*	gkRendererGL330::m_pMeshManager = NULL;
IResourceManager*	gkRendererGL330::m_pMaterialManager = NULL;
IResourceManager*	gkRendererGL330::m_pShaderManager = NULL;
gkAuxRendererGLES2*		gkRendererGL330::m_pAuxRenderer = 0;
gkShaderParamDataSource	gkRendererGL330::m_ShaderParamDataSource;

gkRendererGL330* getRenderer()
{
	return static_cast<gkRendererGL330*>(gEnv->pRenderer);
}

gkRendererGL330::gkRendererGL330(void)
{
	m_pAuxRenderer = new gkAuxRendererGLES2;

	m_pUpdatingRenderSequence = new gkRenderSequence();
	m_pRenderingRenderSequence = new gkRenderSequence();

	m_bSkipThisFrame = 0;
}

gkRendererGL330::~gkRendererGL330(void)
{
	SAFE_DELETE( m_pAuxRenderer );
	SAFE_DELETE(m_pUpdatingRenderSequence)
	SAFE_DELETE(m_pRenderingRenderSequence)
}

IResourceManager* gkRendererGL330::getResourceManager( BYTE type )
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

HWND gkRendererGL330::Init( struct ISystemInitInfo& sii )
{
 	m_deviceContext = new gkDeviceRenderContext();
 	m_hWnd = m_deviceContext->initDevice( sii );

	g_pRendererCVars = new gkRendererCVars;
	
	m_screenWidth = sii.fWidth;
	m_screenHeight = sii.fHeight;

	gl::init();

	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &m_backbufferFBO);

	glGenFramebuffers(1, &m_rtFBO);
	glGenFramebuffers(1, &m_drtFBO);

	m_pTextureManager		= new gkTextureManager();
	m_pShaderManager		= new gkShaderManager();
	m_pMaterialManager		= new gkMaterialManager();
	m_pMeshManager		= new gkMeshManager();

	// Sets the clear color
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glClearDepth(1.0f);
	glViewport(0,0,sii.fWidth,sii.fHeight);

	//glfwSwapInterval(0);

	m_pShaderManager->reloadAll();

	m_pAuxRenderer->init();

	return m_hWnd;
}

void gkRendererGL330::Destroy()
{
	m_pAuxRenderer->destroy();
	
	m_deviceContext->destroyDevice();

	delete m_deviceContext;

	delete g_pRendererCVars;
}

IParticleProxy* gkRendererGL330::createGPUParticleProxy()
{
	return NULL;
}

void gkRendererGL330::destroyGPUParticelProxy( IParticleProxy* )
{
	
}

IRenderSequence* gkRendererGL330::RT_SwapRenderSequence()
{
	// renderSeq clean and prepare
	if (m_pUpdatingRenderSequence && m_pUpdatingRenderSequence->m_bIsReady && m_pRenderingRenderSequence)
	{
		*m_pRenderingRenderSequence = *m_pUpdatingRenderSequence;

		RC_SetMainCamera(m_pRenderingRenderSequence->getCamera(eRFMode_General)->getCCam());

		// 		if( gEnv->pCVManager->r_Shadow )
		// 		{
		if (m_pRenderingRenderSequence->getCamera(eRFMode_ShadowCas0))
		{
			RC_SetShadowCascadeCamera(m_pRenderingRenderSequence->getCamera(eRFMode_ShadowCas0)->getCCam(), 0);
		}
		if (gEnv->pProfiler->getFrameCount() % GSM_SHADOWCASCADE1_DELAY == 0 && m_pRenderingRenderSequence->getCamera(eRFMode_ShadowCas1))
			RC_SetShadowCascadeCamera(m_pRenderingRenderSequence->getCamera(eRFMode_ShadowCas1)->getCCam(), 1);

		if (gEnv->pProfiler->getFrameCount() % GSM_SHADOWCASCADE2_DELAY == 0 && m_pRenderingRenderSequence->getCamera(eRFMode_ShadowCas2))
			RC_SetShadowCascadeCamera(m_pRenderingRenderSequence->getCamera(eRFMode_ShadowCas2)->getCCam(), 2);
		// 		}
	}
	m_pAuxRenderer->_swapBufferForRendering();


	m_pUpdatingRenderSequence->clear();
	m_pRenderingRenderSequence->PrepareRenderSequence();

	m_ShaderParamDataSource.setCurrTodKey( gEnv->p3DEngine->getTimeOfDay()->getCurrentTODKey() );

	return m_pUpdatingRenderSequence;
}

IRenderSequence* gkRendererGL330::RT_GetRenderSequence()
{
	return m_pUpdatingRenderSequence;
}

void gkRendererGL330::RT_CleanRenderSequence()
{
	m_pUpdatingRenderSequence->clear();
	m_pRenderingRenderSequence->clear();

	m_pAuxRenderer->_cleanBuffer();
}

void gkRendererGL330::RP_ProcessRenderLayer( uint8 layerID )
{
	gkRenderLayer* pLayer = m_pRenderingRenderSequence->getRenderLayer(layerID);
	// ª˘±æ‰÷»æ—≠ª∑£¨—≠ª∑‰÷»æshader◊”≤„
	gkRenderLayer::ShaderGroupMapIterator shaderGroupIter = pLayer->_getShaderGroupIterator();

	while (shaderGroupIter.hasMoreElements())
	{
		IShader* pShaderOfThisLayer = shaderGroupIter.peekNextKey();
		const gkShaderGroup& pRenderablelistOfShader = shaderGroupIter.getNext();

		RP_ProcessShaderGroup(&pRenderablelistOfShader, 0);

	}
}


void gkRendererGL330::RP_ProcessShaderGroup( const gkShaderGroup* pShaderGroup, BYTE sortType )
{
	gkShaderGLES2* pShader = reinterpret_cast<gkShaderGLES2*>(pShaderGroup->m_pShader);

	if(!pShader)
	{
		return;
	}
	pShader->FX_Begin( 0, 0 );

    if ( pShader->getDefaultRenderLayer() == RENDER_LAYER_SKIES_EARLY )
    {
		const STimeOfDayKey& tod = gEnv->p3DEngine->getTimeOfDay()->getCurrentTODKey();
		pShader->FX_SetFloat4( "g_SkyBottom", (tod.clZenithBottom * 0.85f).toVec4() );
		pShader->FX_SetFloat4( "g_SkyTop", (tod.clZenithTop * 0.85f).toVec4() );
		pShader->FX_SetFloat( "g_ZenithHeight", tod.fZenithShift);
	}

	// global cam param
	Matrix44 viewiMat = gkRendererGL330::getShaderContent().getInverseViewMatrix();
	Vec4 screensize( GetScreenWidth(), GetScreenHeight(), 1.0f / (float)GetScreenWidth(), 1.0f / (float)GetScreenHeight() );
	pShader->FX_SetFloat4( "g_screenSize", screensize );
	pShader->FX_SetMatrix("g_matViewI", viewiMat);
	pShader->FX_SetFloat3("g_camDir", gkRendererGL330::getShaderContent().getCamDir());
	pShader->FX_SetFloat3("g_camPos", gkRendererGL330::getShaderContent().getCamPos());

	// global light param
	const STimeOfDayKey& tod = getShaderContent().getCurrTodKey();
	ColorF lightdif = tod.clSunLight;
	ColorF lightspec = tod.clSunLight;
	ColorF skylight = tod.clGroundAmb;
	lightdif *= (tod.fSunIntensity / 10.0f);
	lightspec *= tod.fSunSpecIntensity;
	pShader->FX_SetFloat3( "g_LightDir", m_ShaderParamDataSource.getLightDir() );
	pShader->FX_SetValue( "g_SkyLight", &skylight, sizeof(ColorF) );
	pShader->FX_SetValue( "g_LightDiffuse", &lightdif, sizeof(ColorF) );
	pShader->FX_SetValue( "g_LightSpecular", &lightspec, sizeof(ColorF) );

	// global shadow map info
	Matrix44 mViewToLightProj;
	Matrix44 mViewCascade[3];

	mViewToLightProj.SetIdentity();
	mViewToLightProj.Multiply(mViewToLightProj,  getShaderContent().getViewMatrix_ShadowCascade(1));
	mViewToLightProj.Multiply(mViewToLightProj,  getShaderContent().getProjectionMatrix_ShadowCascade(1));

	mViewCascade[1] = mViewToLightProj;

	pShader->FX_SetMatrix( "g_mViewToLightProj", mViewCascade[1] );



	// per material
	gkShaderGroup::gkMaterialGroupRenderableMap::const_iterator it = pShaderGroup->m_opaqueMaterialMap.begin();
	for (; it != pShaderGroup->m_opaqueMaterialMap.end(); ++it)
	{
		// Set param per Material
		IMaterial* pMtl = it->first;
		pMtl->ApplyParameterBlock();

		static gkTexturePtr texRot;
		texRot = gEnv->pSystem->getTextureMngPtr()->load(_T("Engine/Assets/Textures/procedure/rotrandomcm.dds"), _T("internal"));
		texRot->Apply(3, 0);
		gkTextureManager::ms_ShadowCascade1->Apply(6, 0);

		pShader->FX_Commit();

		RP_ProcessRenderList(&(it->second), pShader );
	}

	pShader->FX_End();
}

void gkRendererGL330::RP_ProcessRenderList( const gkRenderableList* obj, gkShaderGLES2* pShader, int shadowcascade )
{
	// ‰÷»ælist÷–µƒ√ø“ª∏ˆrenderable
	std::list<gkRenderable*>::const_iterator iter, iterend;
	iterend = obj->m_vecRenderable.end();
	for (iter = obj->m_vecRenderable.begin(); iter != iterend; ++iter)
	{
		m_ShaderParamDataSource.setCurrentRenderable((*iter));

		// bind MVP MATRIX
		//int location = glGetUniformLocation(m_uiProgramObject, "matMVP");
		Matrix44 mvpMat = gkRendererGL330::getShaderContent().getWorldViewProjMatrix();
		Matrix44 worldMat = gkRendererGL330::getShaderContent().getWorldMatrix();
		switch( shadowcascade )
		{
		case 0:
			mvpMat = gkRendererGL330::getShaderContent().getWorldViewProjMatrix_ShadowCascade(0);
			worldMat = gkRendererGL330::getShaderContent().getWorldViewMatrix_ShadowCascade(0);
			break;
		case 1:
			mvpMat = gkRendererGL330::getShaderContent().getWorldViewProjMatrix_ShadowCascade(1);
			worldMat = gkRendererGL330::getShaderContent().getWorldViewMatrix_ShadowCascade(1);
			break;
		case 2:
			mvpMat = gkRendererGL330::getShaderContent().getWorldViewProjMatrix_ShadowCascade(2);
			worldMat = gkRendererGL330::getShaderContent().getWorldViewMatrix_ShadowCascade(2);
			break;

		}

		pShader->FX_SetMatrix("g_matWVP", mvpMat);
		pShader->FX_SetMatrix("g_matWorld", worldMat);
		pShader->FX_SetMatrix("g_matWorldI", gkRendererGL330::getShaderContent().getInverseWorldMatrix());

		if (shadowcascade + 1)
		{
			(*iter)->getMaterial()->getTexture(eMS_Diffuse)->Apply(0,0);
		}

		// ◊Ó∫Û£¨»°≥ˆvb∫Õib
		gkRenderOperation op; 
		(*iter)->getRenderOperation(op);

		(*iter)->RP_Prepare();

		// code for drawprimitives
		_render(op, false);
	}
}

bool gkRendererGL330::RT_StartRender()
{
	glClearColor(0.5,0.5,0.5,1.0);
    glClear(GL_COLOR_BUFFER_BIT);
	//return true;
	FX_PushRenderTarget( 0, NULL, gkTextureManager::ms_ShadowCascade1 );
	glClear(GL_DEPTH_BUFFER_BIT);
	if( g_pRendererCVars->r_Shadow )
	{
		gkShaderGLES2* shader = (gkShaderGLES2*)(gkShaderManager::ms_ShadowMapGen.getPointer());
		shader->FX_Begin(0,0);

		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		RP_ProcessRenderList( m_pRenderingRenderSequence->getShadowCascade1Objects(), shader, 1 );
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		shader->FX_End();
	}
	FX_PopRenderTarget(0);

// 	if (g_pRendererCVars->r_HDRRendering)
// 	{ 
//		gkTextureManager::ms_HDRTarget0->changeAttr(_T("format"), _T("A16B16G16R16F"));
// 	}
// 	else
// 	{ 
// 		gkTextureManager::ms_HDRTarget0->changeAttr(_T("format"), _T("A8R8G8B8"));
// 	}
	FX_PushRenderTarget( 0, gkTextureManager::ms_HDRTarget0, gkTextureManager::ms_SceneDepth );
    glDisable(GL_STENCIL_TEST);

    //glClearColor(1.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

	RP_ProcessRenderLayer( RENDER_LAYER_SKIES_EARLY );
	RP_ProcessRenderLayer( RENDER_LAYER_OPAQUE );
	RP_ProcessRenderLayer( RENDER_LAYER_TERRIAN );

	FX_PopRenderTarget(0);
    
    if(1)
    {
        FX_PushRenderTarget(0, gkTextureManager::ms_SceneDepth_Linear, NULL);
        
        gkShaderPtr pShader = gkShaderManager::ms_postcvtlineardepth;
        pShader->FX_Begin( 0, 1);
        
        pShader->FX_SetFloat4("ViewportParam", getShaderContent().getViewParam() );
        
        gkTextureManager::ms_SceneDepth->Apply(0, 0);
        FX_DrawScreenQuad();
        
        pShader->FX_End();
        
        FX_PopRenderTarget(0);
    }
    
    FX_PushRenderTarget( 0, gkTextureManager::ms_HDRTarget0, NULL );
    
    if(g_pRendererCVars->r_SSAO)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        gkShaderPtr pShader = gkShaderManager::ms_postssao;
        pShader->FX_Begin( 0, 1);
        
        pShader->FX_SetFloat4("ViewportParam", getShaderContent().getViewParam() );
        pShader->FX_SetFloat3("campos", getShaderContent().getCamPos() );
        
        Vec3 ps_ddx = getShaderContent().getCamFarVerts(1) - getShaderContent().getCamFarVerts(0);
        Vec3 ps_ddy = getShaderContent().getCamFarVerts(2) - getShaderContent().getCamFarVerts(0);
        
        pShader->FX_SetFloat3("PS_ddx", ps_ddx);
        pShader->FX_SetFloat3("PS_ddy", ps_ddy);
        
        gkTextureManager::ms_SceneDepth_Linear->Apply(0, 0);
        gkTextureManager::ms_RotSamplerAO->Apply(1, 2);
        
        
        FX_DrawScreenQuad();
        
        pShader->FX_End();
        
        glDisable(GL_BLEND);
        
        
    }
    
	if(1)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        gkShaderPtr pShader = gkShaderManager::ms_postforprocess;
        pShader->FX_Begin( 0, 1);

		STimeOfDayKey& tod = getShaderContent().getCurrTodKey();

		
        Vec4 fogParam(0.005, tod.clFogDensity, tod.clFogDensity, tod.clFogStart);
		ColorF fogColor = tod.clFogColor * tod.clFogColor.a;
		pShader->FX_SetFloat4("vfParams", fogParam );
		pShader->FX_SetFloat4("fogColor", fogColor.toVec4());

        pShader->FX_SetFloat4("ViewportParam", getShaderContent().getViewParam() );
        pShader->FX_SetFloat3("campos", getShaderContent().getCamPos() );
        
        gkTextureManager::ms_SceneDepth_Linear->Apply(0, 0);
        
        FX_DrawScreenQuad();
        
        pShader->FX_End();
        
        glDisable(GL_BLEND);
    }

    FX_PopRenderTarget(0);
    
	//////////////////////////////////////////////////////////////////////////
	if (g_pRendererCVars->r_HDRRendering)
	{

		//////////////////////////////////////////////////////////////////////////
		// scale down
		{
			FX_PushRenderTarget( 0, gkTextureManager::ms_HDRTargetScaledHalf, NULL );

			gkShaderPtr pShader = gkShaderManager::ms_postcopy;
			pShader->FX_Begin( 0, 0 );

			gkTextureManager::ms_HDRTarget0->Apply(0,1);
			FX_DrawScreenQuad();

			pShader->FX_End();

			FX_PopRenderTarget(0);
		}

        FX_TexBlurGaussian( gkTextureManager::ms_HDRTargetScaledHalf, 1.0, 1.0, 3.0, gkTextureManager::ms_HDRTargetScaledHalfBlur, 1 );

		//////////////////////////////////////////////////////////////////////////
		// bright pass
        {
			FX_PushRenderTarget( 0, gkTextureManager::ms_HDRBrightPassQuad, NULL );
            
			gkShaderPtr pShader = gkShaderManager::ms_postbrightpass;
			pShader->FX_Begin( 0, 0 );
            
			gkTextureManager::ms_HDRTargetScaledHalf->Apply(0,1);
			FX_DrawScreenQuad();
            
			pShader->FX_End();
            
			FX_PopRenderTarget(0);
            
			//////////////////////////////////////////////////////////////////////////
			// blur the bright pass
			FX_TexBlurGaussian( gkTextureManager::ms_HDRBrightPassQuad, 1.0, 1.0, 3.0, gkTextureManager::ms_HDRBrightPassQuadTmp, 1 );
		}
        
		{
			FX_PushRenderTarget( 0, gkTextureManager::ms_HDRBrightPassQuadQuad, NULL );

			gkShaderPtr pShader = gkShaderManager::ms_postcopy;
			pShader->FX_Begin( 0, 0 );

			gkTextureManager::ms_HDRBrightPassQuad->Apply(0,1);
			FX_DrawScreenQuad();

			pShader->FX_End();

			FX_PopRenderTarget(0);

			//////////////////////////////////////////////////////////////////////////
			// blur the bright pass
			FX_TexBlurGaussian( gkTextureManager::ms_HDRBrightPassQuadQuad, 1.0, 1.0, 3.0, gkTextureManager::ms_HDRBrightPassQuadQuadTmp, 1 );
		}

		//FX_TexBlurGaussian( gkTextureManager::ms_HDRTargetScaledHalf, 1.0, 1.0, 3.0, gkTextureManager::ms_HDRTargetScaledHalfBlur, 1 );
		//////////////////////////////////////////////////////////////////////////
		// downscale blur once
		{
		FX_PushRenderTarget( 0, gkTextureManager::ms_HDRBrightPassQuadQuadQuad, NULL );
		
		gkShaderPtr pShader = gkShaderManager::ms_postcopy;
		pShader->FX_Begin( 0, 0 );
		//
		gkTextureManager::ms_HDRBrightPassQuadQuad->Apply(0,1);
		FX_DrawScreenQuad();
		//
		pShader->FX_End();
		//
		FX_PopRenderTarget(0);
		FX_TexBlurGaussian( gkTextureManager::ms_HDRBrightPassQuadQuadQuad, 1.0, 1.0, 3.0, gkTextureManager::ms_HDRBrightPassQuadQuadQuadTmp, 1 );
		}


		if (g_pRendererCVars->r_dof)
		{
			gkShaderPtr pShader = gkShaderManager::ms_postbloomdof;
			pShader->FX_Begin( 0, 1 );

			// hack me down
			const STimeOfDayKey& key = gEnv->p3DEngine->getTimeOfDay()->getCurrentTODKey();
			pShader->FX_SetFloat4( "clBloom", key.clBloomColor.toVec4() );
			gkTextureManager::ms_HDRTarget0->Apply(0,1);
			gkTextureManager::ms_HDRBrightPassQuad->Apply(1,1);
			gkTextureManager::ms_HDRBrightPassQuadQuad->Apply(2,1);
			gkTextureManager::ms_HDRBrightPassQuadQuadQuad->Apply(3,1);

			gkTextureManager::ms_HDRTargetScaledHalf->Apply(5,1);
			gkTextureManager::ms_SceneDepth_Linear->Apply(6,0);


			static gkTexturePtr texVignetting;
			texVignetting = gEnv->pSystem->getTextureMngPtr()->loadSync(_T("Engine/Assets/Textures/procedure/vignetting.dds"), _T("internal"));
			if (!texVignetting.isNull())
			{
				texVignetting->Apply(4, 1);
			}

			FX_DrawScreenQuad();

			pShader->FX_End();
		}
		else
		{
			gkShaderPtr pShader = gkShaderManager::ms_postbloom;
			pShader->FX_Begin( 0, 1 );

			// hack me down
			const STimeOfDayKey& key = gEnv->p3DEngine->getTimeOfDay()->getCurrentTODKey();
			pShader->FX_SetFloat4( "clBloom", key.clBloomColor.toVec4() );
			gkTextureManager::ms_HDRTarget0->Apply(0,1);
			gkTextureManager::ms_HDRBrightPassQuad->Apply(1,1);
			gkTextureManager::ms_HDRBrightPassQuadQuad->Apply(2,1);
			gkTextureManager::ms_HDRBrightPassQuadQuadQuad->Apply(3,1);

			static gkTexturePtr texVignetting;
			texVignetting = gEnv->pSystem->getTextureMngPtr()->loadSync(_T("Engine/Assets/Textures/procedure/vignetting.dds"), _T("internal"));
			if (!texVignetting.isNull())
			{
				texVignetting->Apply(4, 1);
			}

			FX_DrawScreenQuad();

			pShader->FX_End();
		}

		//////////////////////////////////////////////////////////////////////////
		// bloom out
// 		{
// 			gkShaderPtr pShader = gkShaderManager::ms_postbloom;
// 			pShader->FX_Begin( 0, 1 );
// 
// 			// hack me down
// 			const STimeOfDayKey& key = gEnv->p3DEngine->getTimeOfDay()->getCurrentTODKey();
// 			pShader->FX_SetFloat4( "clBloom", key.clBloomColor.toVec4() );
// 			gkTextureManager::ms_HDRTarget0->Apply(0,1);
//             gkTextureManager::ms_HDRBrightPassQuad->Apply(1,1);
//             gkTextureManager::ms_HDRBrightPassQuadQuad->Apply(2,1);
// 			gkTextureManager::ms_HDRBrightPassQuadQuadQuad->Apply(3,1);
// //			gkTextureManager::ms_SceneDepth->Apply(2,0);
// 
//             
// 			static gkTexturePtr texVignetting;
// 			texVignetting = gEnv->pSystem->getTextureMngPtr()->loadSync(_T("Engine/Assets/Textures/procedure/vignetting.dds"), _T("internal"));
// 			if (!texVignetting.isNull())
// 			{
// 				texVignetting->Apply(4, 1);
// 			}
//             
//             gkTextureManager::ms_HDRTargetScaledHalf->Apply(5,1);
//             gkTextureManager::ms_SceneDepth_Linear->Apply(6,1);
// 
// 			FX_DrawScreenQuad();
// 
// 			pShader->FX_End();
// 		}

	}
	else
	{
		gkShaderPtr pShader = gkShaderManager::ms_PostCommon;
		pShader->FX_Begin( 0, 1 );

		gkTextureManager::ms_HDRTarget0->Apply(0,1);

		FX_DrawScreenQuad();

		pShader->FX_End();
	}

	

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	m_pAuxRenderer->_FlushAllHelper();
	RP_ProcessRenderLayer( RENDER_LAYER_HUDUI );

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

    m_deviceContext->swapBuffer();
    
	return true;
}

bool gkRendererGL330::RT_EndRender()
{
	//glfwSwapBuffers();
	
    // kaimingyi 2015.03.26
    // here do not need mt rendering, put it in StartRender, to poll event at a right place.
    
    //m_deviceContext->swapBuffer();
	return true;
}

void gkRendererGL330::RT_SkipOneFrame( int framecount /*= 1 */ )
{
	m_bSkipThisFrame = framecount;
	RT_CleanRenderSequence();
}

void gkRendererGL330::FX_ColorGradingTo( gkTexturePtr& pCch, float fAmount )
{
	
}

void gkRendererGL330::RC_SetSunDir( const Vec3& lightdir )
{
	m_ShaderParamDataSource.setLightDir(lightdir);
}

uint32 gkRendererGL330::GetScreenHeight(bool forceOrigin)
{
	return m_screenHeight;
}

uint32 gkRendererGL330::GetScreenWidth(bool forceOrigin)
{
	return m_screenWidth;
}

HWND gkRendererGL330::GetWindowHwnd()
{
	return m_hWnd;
}

Vec2i gkRendererGL330::GetWindowOffset()
{
	return Vec2i(0,0);
}

void gkRendererGL330::SetCurrContent( HWND hWnd, uint32 posx, uint32 posy, uint32 width, uint32 height, bool fullscreen )
{
	
}

IAuxRenderer* gkRendererGL330::getAuxRenderer()
{
	return m_pAuxRenderer;
}

Ray gkRendererGL330::GetRayFromScreen( uint32 nX, uint32 nY )
{
	Ray ret;

	if (m_ShaderParamDataSource.m_pCurrentCamera)
	{
		m_ShaderParamDataSource.m_pCurrentCamera->GetPixelRay( nX, nY, GetScreenWidth(), GetScreenHeight(), &(ret.origin), &(ret.direction) );
	}
	return ret;
}

Vec3 gkRendererGL330::ProjectScreenPos( const Vec3& worldpos )
{
	Vec3 ret;

	if (m_ShaderParamDataSource.m_pCam)
	{
		m_ShaderParamDataSource.m_pCam->Project( worldpos, ret );
	}

	//ret.y *= 2;

	return ret;
}

void gkRendererGL330::RC_SetMainCamera( const CCamera* cam )
{
	m_ShaderParamDataSource.setMainCamera(cam);
}

void gkRendererGL330::RC_SetShadowCascadeCamera( const CCamera* cam, uint8 index )
{
	m_ShaderParamDataSource.setShadowCascadeCamera(cam, index);
}

void gkRendererGL330::_render( const gkRenderOperation& op, bool isShadowPass /*= false*/ )
{
	switch (op.operationType)
	{
	case gkRenderOperation::OT_SKIP:
		return;
	case gkRenderOperation::OT_TRIANGLE_LIST:
		{
			if (!op.vertexData)
				return;
			if (op.useIndexes && !op.indexData)
				return;

			// bind vertex buffer

			{

				GLuint vaoName = 0;

				gkVAOmapping::iterator it = m_vaoMap.find( op.vertexData );
				if (it != m_vaoMap.end())
				{
					vaoName = it->second;
				}
				else
				{
					glGenVertexArrays(1, &vaoName);
					glBindVertexArray( vaoName );

					// use vbo and bind attribute by hand
					glBindBuffer( GL_ARRAY_BUFFER, (GLint)(op.vertexData->userData) );
					switch( op.vertexData->vertexType )
					{
					case eVI_P4F4F4:
						{
							// Pass the vertex data
							glEnableVertexAttribArray(GK_SHADER_VERTEX_ARRAY);
							glVertexAttribPointer(GK_SHADER_VERTEX_ARRAY, 3, GL_FLOAT, GL_FALSE, op.vertexSize, 0);

							glEnableVertexAttribArray(GK_SHADER_TEXCOORD_ARRAY);
							glVertexAttribPointer(GK_SHADER_TEXCOORD_ARRAY, 2, GL_FLOAT, GL_FALSE, op.vertexSize, (void*)(3 * sizeof(float)));

							// Pass the texture coordinates data
							glEnableVertexAttribArray(GK_SHADER_TANGENT_ARRAY);
							glVertexAttribPointer(GK_SHADER_TANGENT_ARRAY, 4, GL_FLOAT, GL_FALSE, op.vertexSize,  (void*)(5 * sizeof(float)));

							glEnableVertexAttribArray(GK_SHADER_BINORMAL_ARRAY);
							glVertexAttribPointer(GK_SHADER_BINORMAL_ARRAY, 3, GL_FLOAT, GL_FALSE, op.vertexSize,  (void*)(9 * sizeof(float)));
						}
						break;
					case eVI_P3T2U4:
						{
							// Pass the vertex data
							glEnableVertexAttribArray(GK_SHADER_VERTEX_ARRAY);
							glVertexAttribPointer(GK_SHADER_VERTEX_ARRAY, 3, GL_FLOAT, GL_FALSE, op.vertexSize, 0);

							glEnableVertexAttribArray(GK_SHADER_TEXCOORD_ARRAY);
							glVertexAttribPointer(GK_SHADER_TEXCOORD_ARRAY, 2, GL_FLOAT, GL_FALSE, op.vertexSize, (void*)(3 * sizeof(float)));

							// Pass the texture coordinates data
							glEnableVertexAttribArray(GK_SHADER_TANGENT_ARRAY);
							glVertexAttribPointer(GK_SHADER_TANGENT_ARRAY, 4, GL_UNSIGNED_BYTE, GL_TRUE, op.vertexSize,  (void*)(5 * sizeof(float)));

							//glDisableVertexAttribArray(GK_SHADER_BINORMAL_ARRAY);
						}
						break;
					}

					glBindVertexArray( 0 );

					m_vaoMap.insert(gkVAOmapping::value_type( op.vertexData, vaoName ));


				}

				glBindVertexArray( vaoName );

			}





			if (op.useIndexes)
			{
				if( (op.indexData->userData ) )
				{
					glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, (GLuint)(op.indexData->userData) );
					glDrawElements(GL_TRIANGLES, op.indexCount,	GL_UNSIGNED_INT, (void*)(op.indexStart * sizeof(int32)) );//(uint32*)(op.indexData->data) + op.indexStart );
					gEnv->pProfiler->increaseElementCount(ePe_Batch_Total),
						gEnv->pProfiler->increaseElementCount(ePe_Triangle_Total, op.indexCount / 3);
				}

			}
			else
			{
				glDrawArrays( GL_TRIANGLES, 0, op.vertexCount );
				gEnv->pProfiler->increaseElementCount(ePe_Batch_Total);
				gEnv->pProfiler->increaseElementCount(ePe_Triangle_Total, op.indexCount / 3);
			}
			// 			ms_CurrFrameDP++;
			// 			if (isShadowPass)
			// 				ms_CurrFrameShadowDP++;

				glBindVertexArray( 0 );
				glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

		}
		break;
	}
}

void gkRendererGL330::FX_PushRenderTarget( uint8 channel, gkTexturePtr src, gkTexturePtr depth, bool bClearTarget /*= false*/ )
{
    gkTextureGLES2* colortex = (gkTextureGLES2*)(src.getPointer());
    gkTextureGLES2* depthtex = (gkTextureGLES2*)(depth.getPointer());
    
    if (!colortex && !depthtex) {
        // all NULL, cannot set as RT
        return;
    }
    
    if (colortex)
    {
        src->touch();
    }
    if(depthtex)
    {
        depth->touch();
    }
    
    
    if (colortex)
    {
        glViewport(0, 0, colortex->getWidth(), colortex->getHeight());
    }
    else if(depthtex)
    {
        glViewport(0, 0, depthtex->getWidth(), depthtex->getHeight());
    }
    
    gkFBO fbo( src, depth);
    gkFBO* pFbo = m_fboMap.find(fbo);
    if(pFbo)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, pFbo->m_hwFBO);
        
        //        if (!src.isNull())
        //        {
        //            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colortex->getGLHandle(), 0 );
        //        }
        //        else
        //        {
        //            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0 );
        //        }
        //
        //        if (!depth.isNull())
        //        {
        //            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthtex->getGLHandle(), 0 );
        //        }
        //        else
        //        {
        //            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 0, 0 );
        //        }
    }
    else
    {
        GLuint fbo_handle;
        glGenFramebuffers(1, &fbo_handle);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo_handle);
        
        if (!src.isNull())
        {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colortex->getGLHandle(), 0 );
        }
        else
        {
            //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0 );
        }
        
        if (!depth.isNull())
        {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthtex->getGLHandle(), 0 );
        }
        else
        {
            //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 0, 0 );
        }
        
        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            //PVRShellSet(prefExitMessage, "ERROR: Frame buffer not set up correctly\n");
            //return false;
            gkLogError(_T("bind fbo failed"));
        }
        
        glBindFramebuffer(GL_FRAMEBUFFER, m_backbufferFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo_handle);
        
        fbo.m_hwFBO = fbo_handle;
        m_fboMap.push_back(fbo);
        pFbo = m_fboMap.find(fbo);
    }
    
    m_fboStack.push_back(pFbo);
}

void gkRendererGL330::FX_PopRenderTarget( uint8 channel )
{
    if (m_fboStack.empty())
    {
    	glViewport(0,0,GetScreenWidth(), GetScreenHeight());
        glBindFramebuffer(GL_FRAMEBUFFER, m_backbufferFBO );
        return;
    }
    
    
    m_fboStack.pop_back();
    
    if (m_fboStack.empty())
    {
    	glViewport(0,0,GetScreenWidth(), GetScreenHeight());
        glBindFramebuffer(GL_FRAMEBUFFER, m_backbufferFBO );
    }
    else
    {
        gkFBO* fbo = m_fboStack.back();
        if( fbo->m_colorRT )
        {
            glViewport(0,0, fbo->m_colorRT->getWidth(), fbo->m_colorRT->getHeight() );
        }
        else if( fbo->m_depthRT )
        {
            glViewport(0,0, fbo->m_depthRT->getWidth(), fbo->m_depthRT->getHeight() );
        }
        
        if( !fbo->m_depthRT )
        {
            //const GLenum attachments[] = {GL_STENCIL_ATTACHMENT, GL_DEPTH_ATTACHMENT};
            //glDiscardFramebufferEXT(GL_FRAMEBUFFER, 1, attachments);
        }
        
        
        glBindFramebuffer(GL_FRAMEBUFFER, fbo->m_hwFBO);
    }
}

void gkRendererGL330::FX_DrawScreenQuad()
{
	FX_DrawScreenQuad(Vec4(0,0,1,1));
}

void gkRendererGL330::FX_DrawScreenQuad( Vec4 region )
{
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	//glDisable(GL_BLEND);
    glDisable(GL_ALPHA_TEST);

	region *= 2.0f;
	region.x -= 1.0f;
	region.y -= 1.0f;

	static GLuint tmpVAO = 0;
	static GLuint tmpVBO = 0;
    
    struct STmpVert
    {
        Vec2 pos;
        Vec2 tex;
        Vec3 farclip;
    };
    
	if (tmpVBO == 0) {

		glGenBuffers(1, &tmpVBO);
		glBindBuffer(GL_ARRAY_BUFFER, tmpVBO);

		glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(STmpVert), NULL, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glGenVertexArrays(1, &tmpVAO);
		glBindVertexArray( tmpVAO );

		glBindBuffer(GL_ARRAY_BUFFER, tmpVBO);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(STmpVert), 0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(STmpVert), (void*)(2 * sizeof(float)) );
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(STmpVert), (void*)(4 * sizeof(float)) );
		glEnableVertexAttribArray(2);
        
		glBindVertexArray( 0 );
	}
    
    // update quad
    glBindBuffer(GL_ARRAY_BUFFER, tmpVBO);
    void* data = glMapBufferRange(GL_ARRAY_BUFFER, 0, 4 * sizeof(STmpVert), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_UNSYNCHRONIZED_BIT);
    
    STmpVert verts[4];
    verts[0].pos = Vec2(region.x, region.y);
    verts[1].pos = Vec2(region.x + region.z, region.y);
    verts[2].pos = Vec2(region.x, region.y + region.w);
    verts[3].pos = Vec2(region.x + region.z, region.y + region.w);
    
    verts[0].tex = Vec2(0,0);
    verts[1].tex = Vec2(1,0);
    verts[2].tex = Vec2(0,1);
    verts[3].tex = Vec2(1,1);
    
    verts[0].farclip = getShaderContent().getCamFarVerts(0);
    verts[1].farclip = getShaderContent().getCamFarVerts(1);
    verts[2].farclip = getShaderContent().getCamFarVerts(2);
    verts[3].farclip = getShaderContent().getCamFarVerts(3);
    
    memcpy( data, verts, 4 * sizeof(STmpVert));
    
    glUnmapBuffer(GL_ARRAY_BUFFER);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray( tmpVAO );

	// Draw the quad
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	// Unbind the VBO
	glBindVertexArray( 0 );

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_BLEND);
}

static float GaussianDistribution1D(float x, float rho)
{
	float g = 1.0f / ( rho * sqrtf(2.0f * 3.14159265358979323f)); 
	g *= expf( -(x * x)/(2.0f * rho * rho) );
	return g;
}

void gkRendererGL330::FX_TexBlurGaussian( gkTexturePtr tgt, int nAmount, float fScale, float fDistribution, gkTexturePtr tmp, int iterate /*= 1*/ )
{
	// blur first
	float s1 = 1.f / (float)tmp->getWidth();
	float t1 = 1.f / (float)tmp->getHeight();
	Vec4 vWhite( 1.0f, 1.0f, 1.0f, 1.0f );

	// Horizontal/Vertical pass params
	const int nSamples = 16;
	int nHalfSamples = (nSamples>>1);

	Vec4 pHParams[32], pVParams[32], pWeightsPS[32];
	float pWeights[32], fWeightSum = 0;

	memset( pWeights,0,sizeof(pWeights) );

	int s;
	for(s = 0; s<nSamples; ++s)
	{
		if(fDistribution != 0.0f)
			pWeights[s] = GaussianDistribution1D(s - nHalfSamples, fDistribution);      
		else
			pWeights[s] = 0.0f;
		fWeightSum += pWeights[s];
	}

	// normalize weights
	for(s = 0; s < nSamples; ++s)
	{
		pWeights[s] /= fWeightSum;  
	}

	// set bilinear offsets
	for(s = 0; s < nHalfSamples; ++s)
	{
		float off_a = pWeights[s*2];
		float off_b = ( (s*2+1) <= nSamples-1 )? pWeights[s*2+1] : 0;   
		float a_plus_b = (off_a + off_b);
		if (a_plus_b == 0)
			a_plus_b = 1.0f;
		float offset = off_b / a_plus_b;

		pWeights[s] = off_a + off_b;
		pWeights[s] *= fScale ;
		pWeightsPS[s] = vWhite * pWeights[s];

		float fCurrOffset = (float) s*2 + offset - nHalfSamples;
		pHParams[s] = Vec4(s1 * fCurrOffset , 0, 0, 0);  
		pVParams[s] = Vec4(0, t1 * fCurrOffset , 0, 0);       
	}


	gkShaderPtr pShader = gkShaderManager::ms_postgaussian;

	pShader->FX_Begin(0,0);

	for (uint8 i=0; i < iterate; ++i)
	{
		//////////////////////////////////////////////////////////////////////////
		// Horizon first
		// tgt -> tmp
		{
			FX_PushRenderTarget(0, tmp, NULL);

			pShader->FX_SetValue("PI_psOffsets", pHParams, sizeof(Vec4) * nHalfSamples);
			pShader->FX_SetValue("psWeights", pWeightsPS, sizeof(Vec4) * nHalfSamples);
			pShader->FX_Commit();
			tgt->Apply(0, 1);

			FX_DrawScreenQuad();

			FX_PopRenderTarget(0);
		}

		//////////////////////////////////////////////////////////////////////////
		// Vertical second
		// tmp -> tgt
		{
			FX_PushRenderTarget(0, tgt, NULL);

			pShader->FX_SetValue("PI_psOffsets", pVParams, sizeof(Vec4) * nHalfSamples);
			pShader->FX_SetValue("psWeights", pWeightsPS, sizeof(Vec4) * nHalfSamples);
			pShader->FX_Commit();
			tmp->Apply(0, 1);

			FX_DrawScreenQuad();

			FX_PopRenderTarget(0);
		}
	}

	pShader->FX_End();
}

Vec3 gkRendererGL330::ScreenPosToViewportPos(Vec3 screenPos)
{
    Vec3 output = screenPos;
    output.x = ( screenPos.x / (float)GetScreenWidth() - 0.5f ) * 2.0f;
    output.y = - ( screenPos.y / (float)GetScreenHeight()- 0.5f ) * 2.0f;
    
    return output;
}
