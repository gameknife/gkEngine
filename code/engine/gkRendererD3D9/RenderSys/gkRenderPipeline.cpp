#include "RendererD3D9StableHeader.h"
#include "gkRendererD3D9.h"

#include "RenderRes\gkTextureManager.h"
#include "RenderRes\gkMaterialManager.h"
#include "RenderRes\gkMeshManager.h"
#include "RenderRes\gkShaderManager.h"

#include "gkRenderSequence.h"
#include "gkRenderLayer.h"
#include "..\Profiler\gkGPUTimer.h"
#include "gkPostProcessManager.h"
#include "RenderRes\gkShader.h"
#include "ITimer.h"
#include "Rendersys\gkColorGradingController.h"
#include "IMesh.h"
#include "IParticle.h"
#include "RendPipelineBase.h"
#include "gk_Camera.h"
#include "ICamera.h"

enum EShadingMode
{
	eSM_DeferredLighing,
	eSM_DeferredShading,
	eSM_ForwardShading,
};

bool gkRendererD3D9::RP_RenderScene(ERenderStereoType stereoType)
{
	//gEnv->p3DEngine->_RT_ProcessRenderSequence();

	//////////////////////////////////////////////////////////////////////////
	// Main D3D9 Renderer Pipeline

	_beginScene();
	FX_ClearAllSampler();

	// SET GLOBAL PARAM
	// set a static shared vector [10/13/2011 Kaiming]
	//gkShaderManager::ms_GeneralZpass->FX_SetFloat4("g_fScreenSize", getShaderContent().getCamParam());

	gkShaderManager::ms_DefaultRender->FX_SetFloat4("g_fScreenSize", getShaderContent().getCamParam());

	if (g_pRendererCVars->r_HDRRendering)
	{
		gkShaderManager::ms_DefaultRender->FX_SetFloat("g_HDRScalar", 10.0);
		gkShaderManager::ms_DefaultRender->FX_SetFloat("g_SRGBRevert", 1.0);  
		gkTextureManager::ms_HDRTarget0->changeAttr(_T("format"), _T("A16B16G16R16F"));
	}
	else
	{
		gkShaderManager::ms_DefaultRender->FX_SetFloat("g_HDRScalar", 5.0);
		gkShaderManager::ms_DefaultRender->FX_SetFloat("g_SRGBRevert", 1.0f / 2.2f);  
		gkTextureManager::ms_HDRTarget0->changeAttr(_T("format"), _T("A8R8G8B8"));
	}
	
	ms_GPUTimers[_T("GPUTime")].start();

	//////////////////////////////////////////////////////////////////////////
	// gpu particle
	for (uint32 i=0; i < m_particleProxy.size(); ++i)
	{
		m_particleProxy[i]->Update();
	}


	//////////////////////////////////////////////////////////////////////////
	// color grading compute
 	if (m_pColorGradingController)
 	{
 		m_pColorGradingController->_RT_RenderingMergedColorChart();
 	}


	//////////////////////////////////////////////////////////////////////////
	// test cubemap gen
	for (int i=0; i < 6; ++i)
	{
		FX_PushCubeRenderTarget( 0, i, gkTextureManager::ms_TestCubeRT );

		gkRendererD3D9::_clearBuffer( true, 0x00000000 );

		gkRendererD3D9::RS_SetRenderState( D3DRS_ZENABLE, TRUE );
		gkRendererD3D9::RS_SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
		gkRendererD3D9::RS_SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE);
		gkRendererD3D9::RS_SetRenderState( D3DRS_CULLMODE, D3DCULL_CW);
		gkRendererD3D9::RS_SetRenderState( D3DRS_ALPHATESTENABLE, FALSE);


		CCamera cam;
		cam.SetFrustum(512,512,DEG2RAD(90.0f), 0.1f, 1000.0f, 1.0f);
		


// 
		switch( i )
		{
		case 0:
			cam.SetAngles( Ang3(0,0,DEG2RAD(90) * 3) );
			break;
		case 1:
			cam.SetAngles( Ang3(0,0,DEG2RAD(90) * 1) );
			break;
		case 2:
			cam.SetAngles( Ang3(DEG2RAD(90),0, DEG2RAD(0)) );
			break;
		case 3:
			cam.SetAngles( Ang3(-DEG2RAD(90), 0, DEG2RAD(0)) );
			break;
		case 4:
			cam.SetAngles( Ang3(0,0,DEG2RAD(90) * 0) );
			break;
		case 5:
			cam.SetAngles( Ang3(0,0,DEG2RAD(90) * 2) );
			break;
		}

// 		switch( i )
// 		{
// 		case 0:
// 			cam.SetAngles( Ang3(0,DEG2RAD(90),DEG2RAD(90) * 0) );
// 			break;
// 		case 1:
// 			cam.SetAngles( Ang3(0,DEG2RAD(-90),DEG2RAD(90) * 2) );
// 			break;
// 		case 2:
// 			cam.SetAngles( Ang3(0,DEG2RAD(180),DEG2RAD(90) * 3) );
// 			break;
// 		case 3:
// 			cam.SetAngles( Ang3(0,DEG2RAD(0),DEG2RAD(90) * 1) );
// 			break;
// 		case 4:
// 			cam.SetAngles( Ang3(DEG2RAD(90), 0, DEG2RAD(90)) );
// 			break;
// 		case 5:
// 			cam.SetAngles( Ang3(-DEG2RAD(90), 0, DEG2RAD(-90)) );
// 			break;
// 		}

// 		switch( i )
// 		{
// 		case 0:
// 			cam.SetAngles( Ang3(0,DEG2RAD(90),DEG2RAD(90) * 3) );
// 			break;
// 		case 1:
// 			cam.SetAngles( Ang3(0,DEG2RAD(-90),DEG2RAD(90) * 1) );
// 			break;
// 		case 2:
// 			cam.SetAngles( Ang3(0,DEG2RAD(180),DEG2RAD(90) * 0) );
// 			break;
// 		case 3:
// 			cam.SetAngles( Ang3(0,DEG2RAD(0),DEG2RAD(90) * 2) );
// 			break;
// 		case 4:
// 			cam.SetAngles( Ang3(DEG2RAD(90), 0, DEG2RAD(180)) );
// 			break;
// 		case 5:
// 			cam.SetAngles( Ang3(-DEG2RAD(90), 0, DEG2RAD(180)) );
// 			break;
// 		}



		cam.SetPosition( Vec3(0,0,1.2) );

		m_pShaderParamDataSource.setMainCamera(&cam);

		gkRendererD3D9::RP_ProcessRenderLayer(RENDER_LAYER_TERRIAN, eSIT_FastCubeGen, false);
		gkRendererD3D9::RP_ProcessRenderLayer(RENDER_LAYER_OPAQUE, eSIT_FastCubeGen, false);
		gkRendererD3D9::RP_ProcessRenderLayer(RENDER_LAYER_OPAQUE, eSIT_FastCubeGen, true);
		//gkRendererD3D9::RP_ProcessRenderLayer(RENDER_LAYER_SKIES_EARLY, eSIT_General, false);

// 
// 		switch( i )
// 		{
// 		case 0:
// 			//gkRendererD3D9::_clearBuffer( true, 0x00000000 );
// 			break;
// 		case 1:
// 			//gkRendererD3D9::_clearBuffer( true, 0x00000000 );
// 			break;
// 		case 2:
// 			//gkRendererD3D9::_clearBuffer( true, 0x00000000 );
// 			break;
// 		case 3:
// 			//gkRendererD3D9::_clearBuffer( true, 0x00000000 );
// 			break;
// 		case 4:
// 			//gkRendererD3D9::_clearBuffer( true, 0xFF0000FF );
// 			break;
// 		case 5:
// 			gkRendererD3D9::_clearBuffer( true, 0xFFFF0000 );
// 			break;
// 		}



		FX_PopCubeRenderTarget( 0 );
	}



	//gkTextureManager::ms_TestCubeRT->AutoGenMipmap();

	m_pShaderParamDataSource.setMainCamera(gEnv->p3DEngine->getMainCamera()->getCCam());
	
	//////////////////////////////////////////////////////////////////////////
	// 1. ShadowMap Gen

	PROFILE_LABEL_PUSH( "GEN_SHADOWMAP" );
	gkRendererD3D9::ms_GPUTimers[_T("ShadowMapGen")].start();

	if( g_pRendererCVars->r_Shadow )
	{

		m_pipelines[RP_ShadowMapGen]->Prepare(m_pRenderingRenderSequence);
		m_pipelines[RP_ShadowMapGen]->Execute(m_pRenderingRenderSequence);
		m_pipelines[RP_ShadowMapGen]->End(m_pRenderingRenderSequence);

	}

	gkRendererD3D9::ms_GPUTimers[_T("ShadowMapGen")].stop();
	PROFILE_LABEL_POP ( "GEN_SHADOWMAP" );






	//////////////////////////////////////////////////////////////////////////
	// 2. ReflectMap Gen

	PROFILE_LABEL_PUSH( "GEN_REFLECTMAP" );
	ms_GPUTimers[_T("ReflectGen")].start();

	//RP_GenReflectExcute(m_pRenderingRenderSequence->getZprepassObjects(), gkShaderManager::ms_ReflGen.getPointer() ,0);


	m_pipelines[RP_ReflMapGen]->Prepare(m_pRenderingRenderSequence);
	m_pipelines[RP_ReflMapGen]->Execute(m_pRenderingRenderSequence);
	m_pipelines[RP_ReflMapGen]->End(m_pRenderingRenderSequence);

	ms_GPUTimers[_T("ReflectGen")].stop();
	PROFILE_LABEL_POP( "GEN_REFLECTMAP" );






	//////////////////////////////////////////////////////////////////////////
	// 3. Zpass

	PROFILE_LABEL_PUSH( "Z_PREPASS" );
	ms_GPUTimers[_T("Zpass")].start();

	switch ( g_pRendererCVars->r_ShadingMode )
	{
	case eSM_DeferredLighing:

		m_pipelines[RP_ZpassDeferredLighting]->Prepare(m_pRenderingRenderSequence);
		m_pipelines[RP_ZpassDeferredLighting]->Execute(m_pRenderingRenderSequence);
		m_pipelines[RP_ZpassDeferredLighting]->End(m_pRenderingRenderSequence);
		break;

	case eSM_DeferredShading:

		m_pipelines[RP_ZpassDeferredShading]->Prepare(m_pRenderingRenderSequence);
		m_pipelines[RP_ZpassDeferredShading]->Execute(m_pRenderingRenderSequence);
		m_pipelines[RP_ZpassDeferredShading]->End(m_pRenderingRenderSequence);
		break;
	}

	ms_GPUTimers[_T("Zpass")].stop();
	PROFILE_LABEL_POP( "Z_PREPASS" );







	//////////////////////////////////////////////////////////////////////////
	// 4. SSAO

	PROFILE_LABEL_PUSH( "SSAO" );
	ms_GPUTimers[_T("SSAO")].start();

	RP_SSAO();

	ms_GPUTimers[_T("SSAO")].stop();
	PROFILE_LABEL_POP( "SSAO" );



	//////////////////////////////////////////////////////////////////////////
	// 6. ShadowMaskGen

	PROFILE_LABEL_PUSH( "SHADOWMASK_GEN" );
	ms_GPUTimers[_T("ShadowMaskGen")].start();

	RP_ShadowMaskGen();

	ms_GPUTimers[_T("ShadowMaskGen")].stop();
	PROFILE_LABEL_POP( "SHADOWMASK_GEN" );


	//////////////////////////////////////////////////////////////////////////
	// 5. DeferredLighting
	RP_DeferredLightExcute(m_pRenderingRenderSequence->getRenderLightList());

	// wether do it or not, here we close stencil
	RS_SetRenderState(D3DRS_STENCILENABLE, FALSE);

// 	RS_SetRenderState( D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP );
// 	RS_SetRenderState( D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP );
// 	RS_SetRenderState( D3DRS_STENCILPASS, D3DSTENCILOP_KEEP );
// 	RS_SetRenderState( D3DRS_STENCILFUNC, D3DCMP_EQUAL );
// 
// 
// 	RS_SetRenderState( D3DRS_STENCILMASK, 0xFFFFFFFF );

	//////////////////////////////////////////////////////////////////////////
	// 7. ForwardShading: Opaque

	PROFILE_LABEL_PUSH( "FORWARDSHADING" );
 	ms_GPUTimers[_T("Opaque")].start();


	switch ( g_pRendererCVars->r_ShadingMode )
	{
	case eSM_DeferredLighing:

		m_pipelines[RP_ShadingPassDeferredLighting]->Prepare(m_pRenderingRenderSequence);
		m_pipelines[RP_ShadingPassDeferredLighting]->Execute(m_pRenderingRenderSequence);
		m_pipelines[RP_ShadingPassDeferredLighting]->End(m_pRenderingRenderSequence);

		break;

	case eSM_DeferredShading:

		m_pipelines[RP_ShadingPassDeferredShading]->Prepare(m_pRenderingRenderSequence);
		m_pipelines[RP_ShadingPassDeferredShading]->Execute(m_pRenderingRenderSequence);
		m_pipelines[RP_ShadingPassDeferredShading]->End(m_pRenderingRenderSequence);

		break;

	}

	FX_ClearAllSampler();

	// test SSRL here [5/11/2013 Kaiming]
	if (g_pRendererCVars->r_ssrl)
	{
		RP_SSRL();
	}


	RS_SetRenderState(D3DRS_STENCILENABLE, FALSE);

 	ms_GPUTimers[_T("Opaque")].stop();
// 	PROFILE_LABEL_POP( "OPAQUE" );

	//////////////////////////////////////////////////////////////////////////
	// 8. StretchRect: Opaque 2 Refraction
// 	RP_StretchRefraction();
// 
// 
// 	//////////////////////////////////////////////////////////////////////////
// 	// 9. Water
// 
// 
// 	PROFILE_LABEL_PUSH( "TRANSPARENT" );
// 	RP_ProcessRenderLayer(RENDER_LAYER_WATER, eSIT_General);
// 
// 
// 	//////////////////////////////////////////////////////////////////////////
// 	// 10. StretchRect: Combine Water result to Refraction
// 	RP_StretchRefraction();


	//////////////////////////////////////////////////////////////////////////
	// 11. ForwardShading: Transparent
	ms_GPUTimers[_T("Transparent")].start();

	//ms_GPUTimers[_T("Transparent")].stop();

	//PROFILE_LABEL_POP( "TRANSPARENT" );


	//RP_GeneralEnd();
	//PROFILE_LABEL_POP( "FORWARDSHADING" );

	//////////////////////////////////////////////////////////////////////////
	// 12. Deferred Snow

	// PostPass: HDR -> ShadowLayer -> SSAO -> PostFXs
	PROFILE_LABEL_PUSH( "POSTPROCESS" );

	// defferred snow here [11/22/2011 Kaiming]

	if (gEnv->p3DEngine->getSnowAmount() > 0.01f)
	{
		PROFILE_LABEL_PUSH( "DEFERRED_SNOW" );
		RP_DeferredSnow();
		PROFILE_LABEL_POP( "DEFERRED_SNOW" );
	}


	//////////////////////////////////////////////////////////////////////////
	// 13. Fog



	PROFILE_LABEL_PUSH( "FOG_PROCESS" );
	RP_FogProcess();
	ms_GPUTimers[_T("Transparent")].stop();
	PROFILE_LABEL_POP( "FOG_PROCESS" );

	//////////////////////////////////////////////////////////////////////////
	// 14. HDR Process



	PROFILE_LABEL_PUSH( "HDRPROCESS" );
	ms_GPUTimers[_T("HDR")].start();

	RS_SetRenderState( D3DRS_ZENABLE, FALSE );
	RS_SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	RS_SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	FX_ClearAllSampler();
	if (  g_pRendererCVars->r_HDRRendering )
	{
		RP_HDRProcess();
	}
	else
	{
		FX_StrechRect( gkTextureManager::ms_HDRTarget0, gkTextureManager::ms_BackBuffer, false );
	}

	if (g_pRendererCVars->r_dof)
	{
		RP_DepthOfField();
	}
	
// 	if (g_pRendererCVars->r_ssrl)
// 	{
// 		RP_SSRL();
// 	}


	FX_PushRenderTarget(0, gkTextureManager::ms_BackBuffer);

	RS_SetRenderState( D3DRS_ZENABLE, TRUE );
	RS_SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	RS_SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	RP_ProcessRenderLayer(RENDER_LAYER_AFTERDOF, eSIT_General, false);
	RP_ProcessRenderLayer(RENDER_LAYER_AFTERDOF, eSIT_General, true);

	RS_SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	RS_SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	//RP_ProcessRenderLayer(RENDER_LAYER_HUDUI, eSIT_General);


	RS_SetRenderState( D3DRS_ZENABLE, FALSE );
	RS_SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	RS_SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	FX_PopRenderTarget(0);

	// clear all
	FX_ClearAllSampler();

	ms_GPUTimers[_T("HDR")].stop();
	PROFILE_LABEL_POP( "HDRPROCESS" );

	//////////////////////////////////////////////////////////////////////////
	// 16. PostProcess

	// PPmanager后渲染
	// Only for some optional postprocess [11/25/2011 Kaiming]
	PROFILE_LABEL_PUSH( "POSTEFFECTS" );
	//gkPostProcessManager::PostRender(m_pd3d9Device);
	PROFILE_LABEL_POP( "POSTEFFECTS" );

	//////////////////////////////////////////////////////////////////////////
	// Finish Render Pipeline
	PROFILE_LABEL_POP( "POSTPROCESS" );

	ms_GPUTimers[_T("GPUTime")].stop();
	_endScene();

	return true;
}





void gkRendererD3D9::RP_DeferredLightExcute(const gkRenderLightList& LightList)
{
	gkShader* pShader = reinterpret_cast<gkShader*>(gkShaderManager::ms_DeferredLighting.getPointer());
	if (!pShader)
	{
		return;
	}


	FX_PushRenderTarget(0, gkTextureManager::ms_SceneDifAcc, true);
	//_clearBuffer( true, 0x7f7f7f7f );
	//_clearBuffer(false, 0x0);
	FX_PushRenderTarget(1, gkTextureManager::ms_SceneSpecAcc);
	//_clear2Buffer(false, 0x0);
	PROFILE_LABEL_PUSH( "DEFERRED_LIGHTING" );
	gkRendererD3D9::ms_GPUTimers[_T("Deferred Lighting")].start();

	// DO NOT WRITE Z ! SO WE CAN RENDER AUX GEOM AFTER THIS!
	//  [8/21/2011 Kaiming-Desktop]
	getRenderer()->SetRenderState(D3DRS_ZWRITEENABLE, FALSE); 
	getRenderer()->SetRenderState(D3DRS_ZENABLE, FALSE);
	SetRenderState(D3DRS_STENCILENABLE, FALSE);
	SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	PROFILE_LABEL_PUSH( "AMBIENT & SUN" );
	gkRendererD3D9::ms_GPUTimers[_T("Ambient Pass")].start();

	//////////////////////////////////////////////////////////////////////////
	// 1. first, Ambient & Sun pass

	gkTextureManager::ms_SceneNormal->Apply(0,0);
	gkTextureManager::ms_SceneDepth->Apply(1,0);
	gkTextureManager::ms_SSAOTarget->Apply(3,0);
	gkTextureManager::ms_ShadowMask->Apply(4,0);

	//Vec3 lightdirInWorld = getShaderContent().getLightDirViewSpace();

	pShader->FX_SetValue( "g_mViewI", &(getShaderContent().getInverseViewMatrix()), sizeof(Matrix44) );

	const STimeOfDayKey& tod = gEnv->p3DEngine->getTimeOfDay()->getCurrentTODKey();
	pShader->FX_SetColor4( "g_Ambient", tod.clSkyLight );
	pShader->FX_SetColor4( "g_cAmbGround", tod.clGroundAmb );

	Vec4 ambHeightParam;
	ambHeightParam.x = tod.fGroundAmbMin;
	ambHeightParam.y = tod.fGroundAmbMax;
	ambHeightParam.z = 1.0f / tod.fGroundAmbMax;
	pShader->FX_SetFloat4( "g_vAmbHeightParams", ambHeightParam );
	
	if (g_pRendererCVars->r_ShadingMode == 0)
	{
		pShader->FX_SetTechnique( "RenderLightPass" );
	}
	else
	{
		pShader->FX_SetTechnique( "RenderLightPassDS" );
		gkTextureManager::ms_ShadowMask->Apply(2,0);

		// set light Params from TOD
		const STimeOfDayKey& tod = getShaderContent().getCurrTodKey();
		ColorF lightdif = tod.clSunLight;
		ColorF lightspec = tod.clSunLight;
		lightdif *= (tod.fSunIntensity / 10.0f);
		lightspec *= tod.fSunSpecIntensity;

		pShader->FX_SetValue( "g_LightPos",&(m_pShaderParamDataSource.getLightDir()), sizeof(Vec3) );
		pShader->FX_SetFloat4("g_camPos", getShaderContent().getCamPos());
		pShader->FX_SetValue( "g_LightDiffuse", &lightdif, sizeof(ColorF) );
		pShader->FX_SetValue( "g_LightSpecular", &lightspec, sizeof(ColorF) );
	}
	
	
	UINT cPasses;
	pShader->FX_Begin( &cPasses, 0 );
	for( UINT p = 0; p < cPasses; ++p )
	{
		pShader->FX_BeginPass( p );




		pShader->FX_Commit();
		gkPostProcessManager::DrawFullScreenQuad(gkTextureManager::ms_SceneDifAcc);

		pShader->FX_EndPass();
	}
	pShader->FX_End();

	gkRendererD3D9::ms_GPUTimers[_T("Ambient Pass")].stop();
	PROFILE_LABEL_POP( "AMBIENT & SUN" );
	//////////////////////////////////////////////////////////////////////////
	// 2. next, PointLight pass


#if 1


	SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	SetRenderState(D3DRS_STENCILENABLE, TRUE);

	// MAY WE SHOULD CLEAR STENCIL BUFFER
	SetRenderState(D3DRS_ZENABLE, TRUE);
	gkRendererD3D9::ms_GPUTimers[_T("Lights")].start();
	

	// PASS THE FAR_CLIP_DELTA
	Vec4 ddx = getShaderContent().getCamFarVerts(0) - getShaderContent().getCamFarVerts(2);
	Vec4 ddy = getShaderContent().getCamFarVerts(3) - getShaderContent().getCamFarVerts(2);

	pShader->FX_SetFloat4("PS_lefttop", getShaderContent().getCamFarVerts(2));
	pShader->FX_SetFloat4("PS_ddx", ddx);
	pShader->FX_SetFloat4("PS_ddy", ddy);
	pShader->FX_SetFloat4("g_camPos", getShaderContent().getCamPos());

	for(gkRenderLightList::const_iterator it = LightList.begin(); it != LightList.end(); ++it)
	{

		PROFILE_LABEL_PUSH( "LIGHT" );

		getDevice()->Clear(0, NULL, D3DCLEAR_STENCIL, 0, 0, 0L);

		// STENCIL PASS
		pShader->FX_SetTechnique( "RenderLightStencilPass" );

		// SET STATE
		if ((it->m_vPos - getShaderContent().getCamPos().GetXYZ()).GetLength() < it->m_fRadius   )
		{
			SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
			SetRenderState(D3DRS_ZFUNC, D3DCMP_GREATER);
		}
		else
		{
			SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
			SetRenderState(D3DRS_ZFUNC, D3DCMP_LESS);
		}

		SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		SetRenderState(D3DRS_COLORWRITEENABLE, 0);

		SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);
		SetRenderState(D3DRS_STENCILWRITEMASK, 0xffffffff);
		SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_INCRSAT);
		SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_INCRSAT);

		pShader->FX_Begin( &cPasses, 0 );
		for( UINT p = 0; p < cPasses; ++p )
		{
			pShader->FX_BeginPass( p );
				// build matrix
				Matrix34 mat;
				mat.Set(Vec3((*it).m_fRadius, (*it).m_fRadius, (*it).m_fRadius),
					Quat::CreateIdentity(), (*it).m_vPos);
				Matrix44 matForRender(mat);
				matForRender.Transpose();
				matForRender = matForRender * getShaderContent().getViewMatrix();
				pShader->FX_SetValue("g_mWorldView", &matForRender, sizeof(Matrix44) );
				matForRender = matForRender * getShaderContent().getProjectionMatrix();
				pShader->FX_SetValue("g_mWorldViewProj", &matForRender, sizeof(Matrix44) );
				pShader->FX_SetValue("g_mProj", &(getShaderContent().getProjectionMatrix()), sizeof(Matrix44) );
			
				pShader->FX_Commit();

				// light mesh
				gkMeshManager::ms_generalSphere->touch();
				gkRenderOperation rop;
				gkMeshManager::ms_generalSphere->getRenderOperation( rop );

				_render(rop);

			pShader->FX_EndPass();
		}
		pShader->FX_End();

		// GENERAL PASS / FAKE SHADOW PASS
// 		if (it->m_bFakeShadow)
// 		{
// 			pShader->FX_SetTechnique( "RenderLightFakeShadowPass" );
// 		}
// 		else

		if (it->m_bGlobalShadow)
		{
			pShader->FX_SetTechnique( "RenderLightGeneralGlobalShadowPass" );
		}
		else
		{

			pShader->FX_SetTechnique( "RenderLightGeneralPass" );
		}
		

		// SET STATE
		SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
		SetRenderState(D3DRS_ZFUNC, D3DCMP_GREATER);

		SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
		SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

		SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED|D3DCOLORWRITEENABLE_BLUE|D3DCOLORWRITEENABLE_GREEN|D3DCOLORWRITEENABLE_ALPHA);

		SetRenderState(D3DRS_STENCILFUNC, D3DCMP_EQUAL);
		SetRenderState(D3DRS_STENCILMASK, 0xf);
		SetRenderState(D3DRS_STENCILREF, 0x1);
		SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);
		SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
		SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);

		pShader->FX_Begin( &cPasses, 0 );
		for( UINT p = 0; p < cPasses; ++p )
		{
			pShader->FX_BeginPass( p );
			// build matrix
			Matrix34 mat;
			mat.Set(Vec3((*it).m_fRadius, (*it).m_fRadius, (*it).m_fRadius),
				Quat::CreateIdentity(), (*it).m_vPos);
			Matrix44 matForRender(mat);
			matForRender.Transpose();
			matForRender = matForRender * getShaderContent().getViewMatrix();
			pShader->FX_SetValue("g_mWorldView", &matForRender, sizeof(Matrix44) );
			matForRender = matForRender * getShaderContent().getProjectionMatrix();
			pShader->FX_SetValue("g_mWorldViewProj", &matForRender, sizeof(Matrix44) );

			matForRender = getShaderContent().getViewMatrix() * getShaderContent().getProjectionMatrix();

			pShader->FX_SetValue("g_mProj", &matForRender, sizeof(Matrix44) );

			ColorF lightParam0 = (*it).m_vDiffuse;
			lightParam0.a = (*it).m_fRadius;
			ColorF lightSpec = (*it).m_vSpecular;
			pShader->FX_SetColor4( "g_LightDiffuse", lightParam0 );
			pShader->FX_SetColor4( "g_LightSpecular", lightSpec );
			pShader->FX_SetFloat3( "g_LightPos", (*it).m_vPos );

			static gkTexturePtr texRandomStep;
			texRandomStep = gEnv->pSystem->getTextureMngPtr()->loadSync(_T("Engine/Assets/Textures/procedure/randomstep.dds"), _T("internal"));
			texRandomStep->Apply(2, 0);

			gkTextureManager::ms_ShadowMask->Apply(3,0);

			pShader->FX_Commit();

			// light mesh
			gkMeshManager::ms_generalSphere->touch();
			gkRenderOperation rop;
			gkMeshManager::ms_generalSphere->getRenderOperation( rop );

			_render(rop);

			pShader->FX_EndPass();
		}
		pShader->FX_End();


		PROFILE_LABEL_POP( "LIGHT" );
	}


	getDevice()->Clear(0, NULL, D3DCLEAR_STENCIL, 0, 0, 0);

	gkRendererD3D9::ms_GPUTimers[_T("Lights")].stop();

#endif

	gkRendererD3D9::ms_GPUTimers[_T("Deferred Lighting")].stop();
	PROFILE_LABEL_POP( "DEFERRED_LIGHTING" );

	//////////////////////////////////////////////////////////////////////////
	// back to SRT rendering, set RT1 to null
	FX_PopRenderTarget(0);
	FX_PopRenderTarget(1);

	SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	SetRenderState(D3DRS_ZENABLE, FALSE);

	FX_ClearAllSampler();
}
//////////////////////////////////////////////////////////////////////////
void gkRendererD3D9::RP_GenReflectExcute( const gkRenderableList* objs, IShader* pShader, BYTE sortType /*= 0*/ )
{
	if(!pShader)
		return;



	// set the RT
	FX_PushRenderTarget(0, gkTextureManager::ms_ReflMap0Tmp, true);
	_clearBuffer(true, 0xff469fe9);


	//fix: 这里直接找一个可用技术，对于单技术shader好用，以后实现复杂功能
	GKHANDLE hTech = NULL;

	hTech = pShader->FX_GetTechniqueByName("BasicPass");

	pShader->FX_SetTechnique( hTech );

	float fShadowsSlopeScaleBias = 0.8f;
	UINT cPasses;
	pShader->FX_Begin( &cPasses, 0 );
	for( UINT p = 0; p < cPasses; ++p )
	{
		pShader->FX_BeginPass( p );

		// 全局参数设置，以后可考虑放置与_renderScene
		pShader->FX_SetValue( "g_mView", &(m_pShaderParamDataSource.getViewMatrix()), sizeof(Matrix44) );
		pShader->FX_SetValue( "g_mProjection", &(m_pShaderParamDataSource.getProjectionMatrix()), sizeof(Matrix44)  );
		pShader->FX_SetValue( "g_mViewI", &(m_pShaderParamDataSource.getInverseViewMatrix()), sizeof(Matrix44) );
		pShader->FX_Commit();

		// 渲染list中的每一个renderable
		std::list<gkRenderable*>::const_iterator iter, iterend;
		iterend = objs->m_vecRenderable.end();
		for (iter = objs->m_vecRenderable.begin(); iter != iterend; ++iter)
		{
			// 首先设置dataSource的currentRenderable
			m_pShaderParamDataSource.setCurrentRenderable((*iter));

			// 然后，设置world矩阵相关
			pShader->FX_SetValue( "g_mWorld", &(m_pShaderParamDataSource.getWorldMatrix()) , sizeof(Matrix44) );
			pShader->FX_SetValue( "g_mWorldI", &(m_pShaderParamDataSource.getInverseWorldMatrix()), sizeof(Matrix44) );
			pShader->FX_SetValue( "g_mWorldViewProj", &(m_pShaderParamDataSource.getWorldViewProjMatrix()), sizeof(Matrix44) );
			pShader->FX_SetValue( "g_mWorldView", &(m_pShaderParamDataSource.getWorldViewMatrix()), sizeof(Matrix44) );

			// 设置texture， 关乎阴影的alphatest
			IMaterial* pMatPtr = (*iter)->getMaterial();
			if(pMatPtr)
			{
				if (pMatPtr->getName() == _T("skysphere"))
				{
					continue;
				}

				if (pMatPtr->isDoubleSide())
					RS_SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
				// set normal and alpha
				pMatPtr->getTexture(eMS_Diffuse)->Apply(0,0);
			}
			pShader->FX_Commit();

			// 最后，取出vb和ib
			gkRenderOperation op; 
			(*iter)->getRenderOperation(op);

			// code for drawprimitives
			_render(op);

			// 一个物体渲染完成
			if(pMatPtr)
				if (pMatPtr->isDoubleSide())
					RS_SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

		}

		pShader->FX_EndPass();
	}
	pShader->FX_End();

	FX_PopRenderTarget(0);
}
//////////////////////////////////////////////////////////////////////////
void gkRendererD3D9::RP_StretchRefraction()
{

	IDirect3DSurface9* pSourceSurf;
	IDirect3DSurface9* pTargetSurf;

	m_pd3d9Device->GetRenderTarget(0, &pSourceSurf);
	gkTexture* hwTex = reinterpret_cast<gkTexture*>(gkTextureManager::ms_SceneTargetTmp0.getPointer());


	hwTex->get2DTexture()->GetSurfaceLevel(0, &pTargetSurf);

	m_pd3d9Device->StretchRect(pSourceSurf, NULL, pTargetSurf, NULL, D3DTEXF_POINT );


	SAFE_RELEASE(pSourceSurf);
	SAFE_RELEASE(pTargetSurf);
}
//////////////////////////////////////////////////////////////////////////
void gkRendererD3D9::RP_SSAO()
{
	STimeOfDayKey& tod = getShaderContent().getCurrTodKey();
	//////////////////////////////////////////////////////////////////////////
	// SSAO

	
	if ( g_pRendererCVars->r_ssaodownscale < 0.01f)
	{
		gkTextureManager::ms_SSAOTargetTmp->changeAttr(_T("size"), _T("full"));
		gkTextureManager::ms_SSAOTarget->changeAttr(_T("size"), _T("full"));
	}
	else if ( g_pRendererCVars->r_ssaodownscale < 1.01f)
	{
		gkTextureManager::ms_SSAOTargetTmp->changeAttr(_T("size"), _T("half"));
		gkTextureManager::ms_SSAOTarget->changeAttr(_T("size"), _T("half"));
	}
	else
	{
		gkTextureManager::ms_SSAOTargetTmp->changeAttr(_T("size"), _T("quad"));
		gkTextureManager::ms_SSAOTarget->changeAttr(_T("size"), _T("quad"));
	}

	FX_PushRenderTarget(0, gkTextureManager::ms_SSAOTargetTmp);
	_clearBuffer(false, 0xFFFFFFFF);

	getRenderer()->SetRenderState( D3DRS_ZENABLE, FALSE );
	getRenderer()->SetRenderState( D3DRS_STENCILENABLE, FALSE );

	if (g_pRendererCVars->r_SSAO)
	{
		// selete tech
		gkShaderPtr pShader = gkShaderManager::ms_SSAO;
		GKHANDLE hTech = pShader->FX_GetTechniqueByName("SSAO");
		pShader->FX_SetTechnique( hTech );

		// params set
		gkTextureManager::ms_SceneNormal->Apply(0,0);
		gkTextureManager::ms_SceneDepth->Apply(1,0);
		gkTextureManager::ms_RotSamplerAO->Apply(2,0);
		
		// PASS THE FAR_CLIP_DELTA
		Vec4 ddx = getShaderContent().getCamFarVerts(0) - getShaderContent().getCamFarVerts(2);
		Vec4 ddy = getShaderContent().getCamFarVerts(3) - getShaderContent().getCamFarVerts(2);

		pShader->FX_SetFloat4("PS_ddx", ddx);
		pShader->FX_SetFloat4("PS_ddy", ddy);
		pShader->FX_SetFloat4("PS_ScreenSize", Vec4(gkTextureManager::ms_SSAOTargetTmp->getWidth(), gkTextureManager::ms_SSAOTargetTmp->getHeight(), 0, 0));
		pShader->FX_SetFloat4("AOSetting", Vec4(g_pRendererCVars->r_SSAOScale, g_pRendererCVars->r_SSAOBias, g_pRendererCVars->r_SSAOAmount * tod.fSSAOIntensity, g_pRendererCVars->r_SSAORadius));
		pShader->FX_SetMatrix("matView", getShaderContent().getViewMatrix());
		pShader->FX_SetFloat4("g_camPos", getShaderContent().getCamPos());
		pShader->FX_Commit();

		UINT cPasses;
		pShader->FX_Begin( &cPasses, 0 );
		for( UINT p = 0; p < cPasses; ++p )
		{
			pShader->FX_BeginPass( p );

			gkPostProcessManager::DrawFullScreenQuad(gkTextureManager::ms_SSAOTargetTmp, Vec4(0,0,1,1),  Vec2(GetScreenWidth() / 4.f, GetScreenHeight() / 4.f));

			pShader->FX_EndPass();
		}
		pShader->FX_End();
	}

	FX_PopRenderTarget(0);


	FX_PushRenderTarget(0, gkTextureManager::ms_SSAOTarget);
	if (g_pRendererCVars->r_SSAO)
	{
		// selete tech
		gkShaderPtr pShader = gkShaderManager::ms_SSAO;
		GKHANDLE hTech = pShader->FX_GetTechniqueByName("SSAO_Blur");
		pShader->FX_SetTechnique( hTech );

		// params set
		gkTextureManager::ms_SSAOTargetTmp->Apply(0,0);
		gkTextureManager::ms_SceneDepth->Apply(1,0);

		// PASS THE FAR_CLIP_DELTA
		Vec4 ddx = getShaderContent().getCamFarVerts(2) - getShaderContent().getCamFarVerts(0);
		Vec4 ddy = getShaderContent().getCamFarVerts(1) - getShaderContent().getCamFarVerts(0);

		pShader->FX_SetFloat4("blurKernel1", Vec4(-0.5f / (float)gkTextureManager::ms_SSAOTargetTmp->getWidth(), -0.5f / (float)gkTextureManager::ms_SSAOTargetTmp->getHeight(), 1.5f / (float)gkTextureManager::ms_SSAOTargetTmp->getWidth(), -0.5f / (float)gkTextureManager::ms_SSAOTargetTmp->getHeight()));
		pShader->FX_SetFloat4("blurKernel2", Vec4(-0.5f / (float)gkTextureManager::ms_SSAOTargetTmp->getWidth(), 1.5f / (float)gkTextureManager::ms_SSAOTargetTmp->getHeight(), 1.5f / (float)gkTextureManager::ms_SSAOTargetTmp->getWidth(), 1.5f / (float)gkTextureManager::ms_SSAOTargetTmp->getHeight()));
		pShader->FX_Commit();

		UINT cPasses;
		pShader->FX_Begin( &cPasses, 0 );
		for( UINT p = 0; p < cPasses; ++p )
		{
			pShader->FX_BeginPass( p );

			gkPostProcessManager::DrawFullScreenQuad(gkTextureManager::ms_SSAOTarget, Vec4(0,0,1,1),  Vec2(GetScreenWidth() / 4.f, GetScreenHeight() / 4.f));

			pShader->FX_EndPass();
		}
		pShader->FX_End();
	}
	else
	{
		_clearBuffer(false, 0xFFFFFFFF);
	}
	FX_PopRenderTarget(0);


}
//////////////////////////////////////////////////////////////////////////
void gkRendererD3D9::RP_ShadowMaskGen()
{
	STimeOfDayKey& tod = getShaderContent().getCurrTodKey();

	float todTime = tod.fKeyTime;
	if(todTime >= 6.0f && todTime <= 18.0f)
	{
		float fDepth = 0.0f;

		if (todTime >= 6.0f && todTime < 8.0f)
		{
			fDepth = powf( 1.0f - (todTime - 6.0f) * 0.5f, 2.2f);
		}
		else if (todTime > 16.0f && todTime <= 18.0f)
		{
			fDepth =  powf(1.0f - (18.0f - todTime) * 0.5f, 2.2f);
		}


		// check the size
		if ( g_pRendererCVars->r_shadowmaskdownscale < 0.01f)
		{
			gkTextureManager::ms_ShadowMask->changeAttr(_T("size"), _T("full"));
		}
		else if ( g_pRendererCVars->r_shadowmaskdownscale < 1.01f)
		{
			gkTextureManager::ms_ShadowMask->changeAttr(_T("size"), _T("half"));
		}
		else
		{
			gkTextureManager::ms_ShadowMask->changeAttr(_T("size"), _T("quad"));
		}

		FX_PushRenderTarget(0, gkTextureManager::ms_ShadowMask, true);

		_clearBuffer(false, 0xFFFFFFFF);
		getDevice()->Clear(0, NULL, D3DCLEAR_STENCIL, 0, 0, 0L);

		if (g_pRendererCVars->r_Shadow)
		{
			Matrix44 mViewToLightProj;
			Matrix44 mViewCascade[3];

			mViewToLightProj.SetIdentity();

			mViewToLightProj.Multiply(mViewToLightProj,  getShaderContent().getViewMatrix_ShadowCascade(0));
			mViewToLightProj.Multiply(mViewToLightProj,  getShaderContent().getProjectionMatrix_ShadowCascade(0));

			mViewCascade[0] = mViewToLightProj;

			mViewToLightProj.SetIdentity();
			mViewToLightProj.Multiply(mViewToLightProj,  getShaderContent().getViewMatrix_ShadowCascade(1));
			mViewToLightProj.Multiply(mViewToLightProj,  getShaderContent().getProjectionMatrix_ShadowCascade(1));

			mViewCascade[1] = mViewToLightProj;

			mViewToLightProj.SetIdentity();
			mViewToLightProj.Multiply(mViewToLightProj,  getShaderContent().getViewMatrix_ShadowCascade(2));
			mViewToLightProj.Multiply(mViewToLightProj,  getShaderContent().getProjectionMatrix_ShadowCascade(2));

			mViewCascade[2] = mViewToLightProj;

			// add a stencil write step, write the shadow cascade into 3 stencil mask [11/21/2011 Kaiming]

			RS_SetRenderState(D3DRS_ZENABLE, FALSE);
			RS_SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

			// open stencil test for writing
			RS_SetRenderState(D3DRS_STENCILENABLE, TRUE);

			// 0x0 mask the shadow cascade
			RS_SetRenderState(D3DRS_STENCILWRITEMASK, 0xFFFFFFFF);
			RS_SetRenderState(D3DRS_STENCILREF, 0x0);

			// 模板掩码设为0
			RS_SetRenderState( D3DRS_STENCILMASK,     0x0 );

			// always success
			RS_SetRenderState( D3DRS_STENCILFUNC,     D3DCMP_ALWAYS );
			// if success or failed, both increase
			RS_SetRenderState( D3DRS_STENCILPASS, D3DSTENCILOP_INCRSAT );
			RS_SetRenderState( D3DRS_STENCILFAIL, D3DSTENCILOP_INCRSAT );
			RS_SetRenderState( D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP );

			// first render cascade 3
			gkShaderPtr pShader = gkShaderManager::ms_ShadowStencilGen;
			GKHANDLE hTech = pShader->FX_GetTechniqueByName("ShadowStencilGen");
			pShader->FX_SetTechnique( hTech );
			// params set
			gkTextureManager::ms_SceneDepth->Apply(0,0);
			gkTextureManager::ms_SceneNormal->Apply(1,0);

			pShader->FX_SetValue("g_mViewToLightProj0", &(mViewCascade[2]), sizeof(Matrix44));
			pShader->FX_SetFloat3("g_mLightDir", getShaderContent().getShadowCamDir(0) );

			UINT cPasses;
			pShader->FX_Begin( &cPasses, 0 );
			for( UINT p = 0; p < cPasses; ++p )
			{
				pShader->FX_BeginPass( p );

				gkPostProcessManager::DrawFullScreenQuad(gkTextureManager::ms_ShadowMask);

				pShader->FX_EndPass();
			}
			pShader->FX_End();

			pShader->FX_SetValue("g_mViewToLightProj0", &(mViewCascade[1]), sizeof(Matrix44));
			pShader->FX_SetFloat3("g_mLightDir", getShaderContent().getShadowCamDir(0) );
			pShader->FX_Begin( &cPasses, 0 );
			for( UINT p = 0; p < cPasses; ++p )
			{
				pShader->FX_BeginPass( p );

				gkPostProcessManager::DrawFullScreenQuad(gkTextureManager::ms_ShadowMask);

				pShader->FX_EndPass();
			}
			pShader->FX_End();

			pShader->FX_SetValue("g_mViewToLightProj0", &(mViewCascade[0]), sizeof(Matrix44));
			pShader->FX_SetFloat3("g_mLightDir", getShaderContent().getShadowCamDir(0) );
			pShader->FX_Begin( &cPasses, 0 );
			for( UINT p = 0; p < cPasses; ++p )
			{
				pShader->FX_BeginPass( p );

				gkPostProcessManager::DrawFullScreenQuad(gkTextureManager::ms_ShadowMask);

				pShader->FX_EndPass();
			}
			pShader->FX_End();


			// shadow mask gen
			RS_SetRenderState( D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP );
			RS_SetRenderState( D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP );
			RS_SetRenderState( D3DRS_STENCILPASS, D3DSTENCILOP_KEEP );
			RS_SetRenderState( D3DRS_STENCILFUNC, D3DCMP_EQUAL );


			RS_SetRenderState( D3DRS_STENCILMASK, 0xFFFFFFFF );
			// selete tech
			pShader = gkShaderManager::ms_ShadowMaskGen;
			hTech = pShader->FX_GetTechniqueByName("SimpleShadowMaskGen");
			pShader->FX_SetTechnique( hTech );

			// params set
			gkTextureManager::ms_SceneDepth->Apply(0,0);
			//gkTexturePtr pShadowMap = gEnv->pSystem->getTextureMngPtr()->getByName(_T("DefaultShadowMap"));


			//pShadowMap->Apply(1,0);

			static gkTexturePtr texRot;
			texRot = gEnv->pSystem->getTextureMngPtr()->loadSync(_T("Engine/Assets/Textures/procedure/rotrandomcm.dds"), _T("internal"));
			texRot->Apply(2, 0);


			pShader->FX_SetValue("g_fShadowDepth", &fDepth, sizeof(float));
			Vec4 gsmparam0(g_pRendererCVars->r_GSMShadowConstbia * 3.0f, g_pRendererCVars->r_GSMShadowPenumbraStart * 0.5f, g_pRendererCVars->r_GSMShadowPenumbraLengh * 0.5f, 1.0f);
			pShader->FX_SetFloat4("gsmShadowParam0", gsmparam0);


			// DRAW cascade2 first
			gkTextureManager::ms_ShadowCascade2->Apply(1, 0);
			RS_SetRenderState( D3DRS_STENCILREF,   1 );
			pShader->FX_SetValue("g_mViewToLightProj0", &(mViewCascade[2]), sizeof(Matrix44));
			pShader->FX_Commit();

			pShader->FX_Begin( &cPasses, 0 );
			for( UINT p = 0; p < cPasses; ++p )
			{
				pShader->FX_BeginPass( p );

				gkPostProcessManager::DrawFullScreenQuad(gkTextureManager::ms_ShadowMask);

				pShader->FX_EndPass();
			}
			pShader->FX_End();


			// change to high perfomance shadow method
			hTech = pShader->FX_GetTechniqueByName("ShadowMaskGen");
			pShader->FX_SetTechnique( hTech );

			// DRAW cascade1 
			gkTextureManager::ms_ShadowCascade1->Apply(1, 0);
			RS_SetRenderState( D3DRS_STENCILREF,   2 );
			pShader->FX_SetValue("g_mViewToLightProj0", &(mViewCascade[1]), sizeof(Matrix44));
			gsmparam0 = Vec4(g_pRendererCVars->r_GSMShadowConstbia * 3.0f, g_pRendererCVars->r_GSMShadowPenumbraStart * 0.5f, g_pRendererCVars->r_GSMShadowPenumbraLengh * 0.5f, 1.0f);
			pShader->FX_SetFloat4("gsmShadowParam0", gsmparam0);
			pShader->FX_Commit();


			pShader->FX_Begin( &cPasses, 0 );
			for( UINT p = 0; p < cPasses; ++p )
			{
				pShader->FX_BeginPass( p );

				gkPostProcessManager::DrawFullScreenQuad(gkTextureManager::ms_ShadowMask);

				pShader->FX_EndPass();
			}
			pShader->FX_End();



			// DRAW cascade0
			gkTextureManager::ms_ShadowCascade0->Apply(1, 0);
			RS_SetRenderState( D3DRS_STENCILREF,   3 );
			pShader->FX_SetValue("g_mViewToLightProj0", &(mViewCascade[0]), sizeof(Matrix44));
			gsmparam0 = Vec4(g_pRendererCVars->r_GSMShadowConstbia, g_pRendererCVars->r_GSMShadowPenumbraStart, g_pRendererCVars->r_GSMShadowPenumbraLengh, 1.0f);
			pShader->FX_SetFloat4("gsmShadowParam0", gsmparam0);
			pShader->FX_Commit();



			pShader->FX_Begin( &cPasses, 0 );
			for( UINT p = 0; p < cPasses; ++p )
			{
				pShader->FX_BeginPass( p );

				gkPostProcessManager::DrawFullScreenQuad(gkTextureManager::ms_ShadowMask);

				pShader->FX_EndPass();
			}
			pShader->FX_End();




		}
		FX_PopRenderTarget(0);




	}
	else
	{
		FX_PushRenderTarget(0, gkTextureManager::ms_ShadowMask);

		_clearBuffer(false, 0x00000000);

		FX_PopRenderTarget(0);

	}


	// wether do it or not, here we close stencil
	RS_SetRenderState(D3DRS_STENCILENABLE, FALSE);



	// BLUR TRY
	//FX_TexBlurGaussian(gkTextureManager::ms_ShadowMask, 1, 1.0f, 1.0f, gkTextureManager::ms_ShadowMaskBlur, 1);


}


void gkRendererD3D9::RP_DeferredSnow()
{
	FX_PushRenderTarget(0, gkTextureManager::ms_HDRTarget0);


	gkShaderPtr pShader = gkShaderManager::ms_DeferredSnow;
	GKHANDLE hTech = pShader->FX_GetTechniqueByName("DeferredSnow");
	pShader->FX_SetTechnique( hTech );

	pShader->FX_SetValue( "g_mViewI", &(getShaderContent().getInverseViewMatrix()), sizeof(Matrix44) );
	float amount = gEnv->p3DEngine->getSnowAmount();
	pShader->FX_SetValue( "g_fSnowAmount", &(amount), sizeof(float));
	pShader->FX_Commit();

	gkTextureManager::ms_SceneNormal->Apply(0, 0);
	gkTextureManager::ms_ShadowMask->Apply(1, 0);
	gkTextureManager::ms_SceneDepth->Apply(2, 0);

	static gkTexturePtr texVignetting;
	texVignetting = gEnv->pSystem->getTextureMngPtr()->loadSync(_T("Engine/Assets/Textures/snow/screenfrost_alpha.dds"), _T("internal"));
	texVignetting->Apply(3, 0);
	static gkTexturePtr texSnowMask;
	texSnowMask = gEnv->pSystem->getTextureMngPtr()->loadSync(_T("Engine/Assets/Textures/snow/snowmask_big.dds"), _T("internal"));
	texSnowMask->Apply(4, 0);

	UINT cPasses;
	pShader->FX_Begin( &cPasses, 0 );
	for( UINT p = 0; p < cPasses; ++p )
	{
		pShader->FX_BeginPass( p );

		gkPostProcessManager::DrawFullScreenQuad(gkTextureManager::ms_HDRTarget0);

		pShader->FX_EndPass();
	}
	pShader->FX_End();

	FX_PopRenderTarget(0);


}


void gkRendererD3D9::RP_FogProcess()
{
	FX_PushRenderTarget(0, gkTextureManager::ms_HDRTarget0);


	gkShaderPtr pShader = gkShaderManager::ms_HDRProcess;
	GKHANDLE hTech = pShader->FX_GetTechniqueByName("FogPass");
	pShader->FX_SetTechnique( hTech );



	//pShader->FX_SetValue( "g_mViewI", &(getShaderContent().getInverseViewMatrix()), sizeof(Matrix44) );
	//float amount = gEnv->p3DEngine->getSnowAmount();
	//pShader->FX_SetValue( "g_fSnowAmount", &(amount), sizeof(float));
	//pShader->FX_Commit();

	gkTextureManager::ms_SceneDepth->Apply(0, 0);

	UINT cPasses;
	pShader->FX_Begin( &cPasses, 0 );
	for( UINT p = 0; p < cPasses; ++p )
	{
		pShader->FX_BeginPass( p );


		// 			Matrix44 matViewImulCampos =;
		// 			matViewImulCampos.SetTranslation(Vec3(0,0,0));
		pShader->FX_SetValue( "g_mViewI", &(getShaderContent().getInverseViewMatrix()), sizeof(Matrix44) );

		// set fog params [12/7/2011 Kaiming]
		const STimeOfDayKey& tod = getShaderContent().getCurrTodKey();

		ColorF fogColor = tod.clFogColor * tod.clFogColor.a;
		if ( !g_pRendererCVars->r_HDRRendering )
		{
			//fogColor.srgb2rgb();
		}

		pShader->FX_SetValue( "vfColGradBase", &fogColor, sizeof(ColorF) );
		Vec4 fogParam(0.005, tod.clFogDensity, tod.clFogDensity, tod.clFogStart);
		pShader->FX_SetValue( "vfParams", &fogParam, sizeof(Vec4) );

		pShader->FX_Commit();

		gkPostProcessManager::DrawFullScreenQuad(gkTextureManager::ms_HDRTarget0);

		pShader->FX_EndPass();
	}
	pShader->FX_End();

	FX_PopRenderTarget(0);
}

void gkRendererD3D9::RP_SSRL()
{
 	gkRendererD3D9::RS_SetRenderState(D3DRS_STENCILENABLE, TRUE);
	gkRendererD3D9::RS_SetRenderState(D3DRS_STENCILMASK, 1 << 7);
	gkRendererD3D9::RS_SetRenderState(D3DRS_STENCILREF, 0xffffffff);

	// always success
	gkRendererD3D9::RS_SetRenderState( D3DRS_STENCILFUNC,     D3DCMP_EQUAL );
	gkRendererD3D9::RS_SetRenderState( D3DRS_STENCILPASS, D3DSTENCILOP_KEEP );
	gkRendererD3D9::RS_SetRenderState( D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP );
	gkRendererD3D9::RS_SetRenderState( D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP );

	FX_PushRenderTarget(0, gkTextureManager::ms_ReflMap0);

	_clearBuffer(false, D3DCOLOR_ARGB( 0, 0, 0, 0 ) );

	static bool first = true;

	gkShaderPtr pShader = gkShaderManager::ms_SSRL;
	GKHANDLE hTech = pShader->FX_GetTechniqueByName("SSRL");
	pShader->FX_SetTechnique( hTech );

	gkTextureManager::ms_SceneNormal->Apply(0,0);
	gkTextureManager::ms_SceneDepth->Apply(1,0);
	gkTextureManager::ms_HDRTarget0->Apply(2,0);

	// PASS THE FAR_CLIP_DELTA
	Vec4 ddx = getShaderContent().getCamFarVerts(0) - getShaderContent().getCamFarVerts(2);
	Vec4 ddy = getShaderContent().getCamFarVerts(3) - getShaderContent().getCamFarVerts(2);

	pShader->FX_SetFloat4("PS_lefttop", getShaderContent().getCamFarVerts(2));
	pShader->FX_SetFloat4("PS_ddx", ddx);
	pShader->FX_SetFloat4("PS_ddy", ddy);
	pShader->FX_SetFloat4("g_camPos", getShaderContent().getCamPos());

// 	pShader->FX_SetValue("g_mWorldView", &matForRender, sizeof(Matrix44) );
// 	matForRender = matForRender * getShaderContent().getProjectionMatrix();
// 	pShader->FX_SetValue("g_mWorldViewProj", &matForRender, sizeof(Matrix44) );

	Matrix44 matForRender = getShaderContent().getViewMatrix() * getShaderContent().getProjectionMatrix();

	pShader->FX_SetValue("g_mProjection", &(matForRender), sizeof(Matrix44) );
	pShader->FX_Commit();

	UINT cPasses;
	pShader->FX_Begin( &cPasses, 0 );
	for( UINT p = 0; p < cPasses; ++p )
	{
		pShader->FX_BeginPass( p );

		gkPostProcessManager::DrawFullScreenQuad(gkTextureManager::ms_ReflMap0);

		pShader->FX_EndPass();
	}
	pShader->FX_End();

	FX_PopRenderTarget( 0 );

	gkTextureManager::ms_ReflMap0->AutoGenMipmap();

	//FX_TexBlurGaussian( gkTextureManager::ms_ReflMap0, 1.0, 1.0, 1.0, gkTextureManager::ms_ReflMap0Tmp);
}

void gkRendererD3D9::RP_FinalOutput()
{

	//////////////////////////////////////////////////////////////////////////
	// if pixel size not 1.0
	double rtResize = 1.0;
	
	rtResize = g_pRendererCVars->r_pixelscale;

	if (rtResize < 0.99999 || rtResize > 1.00001 )
	{
		FX_PushRenderTarget(0, gkTextureManager::ms_SceneTarget0);
	}

	if(m_currStereoType == eRS_RightEye)
	{
		gkTextureManager::ms_BackBufferA_RightEye->Apply(0,0);
		gkTextureManager::ms_BackBufferB_RightEye->Apply(1,0);
	}
	else
	{
		gkTextureManager::ms_BackBufferA_LeftEye->Apply(0,0);
		gkTextureManager::ms_BackBufferB_LeftEye->Apply(1,0);
	}

	if (g_pRendererCVars->r_PostMsaa)
	{
		if( g_pRendererCVars->r_PostMsaaMode != 2 )
		{
			static bool first = true;
			gkShaderPtr pShader = gkShaderManager::ms_post_msaa;

			if(g_pRendererCVars->r_PostMsaaMode == 1)
			{
				pShader = gkShaderManager::ms_post_msaa_smaa1tx;
			}

			GKHANDLE hTech = pShader->FX_GetTechniqueByName("PostMSAA");
			UINT cPasses;

			pShader->FX_SetTechnique( hTech );

			pShader->FX_Begin( &cPasses, 0 );
			for( UINT p = 0; p < cPasses; ++p )
			{
				pShader->FX_BeginPass( p );

				float fCurrFrameTime = gEnv->pTimer->GetFrameTime();

				// scale down shutter speed a b it. default shutter is 0.02, so final result is 0.004 (old default value)
				float fExposureTime = 0.2f * 0.02f;//CRenderer::CV_r_MotionBlurShutterSpeed;
				float fAlpha = fabsf(fCurrFrameTime) < 1E-6f ? 0.0f : fExposureTime /fCurrFrameTime;

				int frameT0 = (gEnv->pProfiler->getFrameCount() % 2) ? 1 : -1;
				int frameT1 = (gEnv->pProfiler->getFrameCount() % 2) ? -1 : 1;
				Vec4 pParam = Vec4( 0.25f * 1.0f / (float)GetScreenWidth() * frameT0, 0.25f * 1.0f / (float)GetScreenHeight() * frameT1, 0.25f * 1.0f / (float)GetScreenWidth() * frameT1, 0.25f * 1.0f / (float)GetScreenHeight() * frameT0);
				//m_ProjMatrix.m20 -= vScreenOffset.x;
				//m_ProjMatrix.m21 -= vScreenOffset.y;
				Vec4 pPS_ScreenSize((float)GetScreenWidth(),(float)GetScreenHeight(),1/(float)GetScreenWidth(),1/(float)GetScreenHeight());
				pShader->FX_SetValue("PS_ScreenSize", &pPS_ScreenSize, sizeof(Vec4));
				pShader->FX_SetValue("PostMsaaParams", &pParam, sizeof(Vec4));

				pShader->FX_Commit();

				gkPostProcessManager::DrawFullScreenQuad(gkTextureManager::ms_SceneTarget0);

				pShader->FX_EndPass();
			}
			pShader->FX_End();
		}
		else
		{
			
			Vec2 pixelsize = Vec2( 1.0f / (float)GetScreenWidth(), 1.0f / (float)GetScreenHeight() );
			gkShaderPtr pShader = gkShaderManager::ms_post_msaa;

			{
				FX_PushRenderTarget(0, gkTextureManager::ms_SMAA_Edge, true);
				getDevice()->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_STENCIL, 0, 0, 0L);

				GKHANDLE hTech = pShader->FX_GetTechniqueByName("ColorEdgeDetection");
				UINT cPasses;
				pShader->FX_SetTechnique( hTech );

				pShader->FX_Begin( &cPasses, 0 );
				for( UINT p = 0; p < cPasses; ++p )
				{
					pShader->FX_BeginPass( p );

					pShader->FX_SetFloat2( "SMAA_PIXEL_SIZE", pixelsize);

					pShader->FX_Commit();

					//gkTextureManager::ms_SceneDepth->Apply(0,0);

					gkTextureManager::ms_BackBuffer_Mono->Apply(0, 1);

					gkPostProcessManager::DrawFullScreenQuad(gkTextureManager::ms_SceneTarget0);

					pShader->FX_EndPass();
				}
				pShader->FX_End();

				FX_PopRenderTarget(0);
			}


			{

				FX_PushRenderTarget(0, gkTextureManager::ms_SMAA_Blend, true);
				getDevice()->Clear(0, NULL, D3DCLEAR_TARGET, 0, 0, 0L);

				GKHANDLE hTech = pShader->FX_GetTechniqueByName("BlendWeightCalculation");
				UINT cPasses;
				pShader->FX_SetTechnique( hTech );

				pShader->FX_Begin( &cPasses, 0 );
				for( UINT p = 0; p < cPasses; ++p )
				{
					pShader->FX_BeginPass( p );

					int indices[][4] = {
						{ 1, 1, 1, 0 }, // S0
						{ 2, 2, 2, 0 }  // S1
					};

					// T1X
					//pShader->FX_SetValue( "subsampleIndices", &(indices[ gEnv->pProfiler->getFrameCount() % 2 ]), sizeof(int) * 4 );


					//pShader->FX_SetValue( "subsampleIndices", &(indices[ gEnv->pProfiler->getFrameCount() % 2 ]), sizeof(int) * 4 );
					pShader->FX_SetFloat2( "SMAA_PIXEL_SIZE", pixelsize);

					pShader->FX_Commit();



					static gkTexturePtr texArea;
						texArea = gEnv->pSystem->getTextureMngPtr()->loadSync( _T("Engine/Assets/Textures/procedure/AreaTexDX9.dds"), _T("internal") );
					static gkTexturePtr texSearch;
						texSearch = gEnv->pSystem->getTextureMngPtr()->loadSync( _T("Engine/Assets/Textures/procedure/SearchTex.dds"), _T("internal") );

					gkTextureManager::ms_SMAA_Edge->Apply(0,0);
					texArea->Apply(1,0);
					texSearch->Apply(2,0);

					gkPostProcessManager::DrawFullScreenQuad(gkTextureManager::ms_SceneTarget0);

					pShader->FX_EndPass();
				}
				pShader->FX_End();

				FX_PopRenderTarget(0);
			}		

			{
// 				if( gEnv->pProfiler->getFrameCount() % 2 == 0 )
// 				{
// 					FX_PushRenderTarget(0, gkTextureManager::ms_BackBufferA_LeftEye);
// 				}
// 				else
// 				{
// 					FX_PushRenderTarget(0, gkTextureManager::ms_BackBufferB_LeftEye);
// 				}
				

				GKHANDLE hTech = pShader->FX_GetTechniqueByName("NeighborhoodBlending");
				UINT cPasses;
				pShader->FX_SetTechnique( hTech );

				pShader->FX_Begin( &cPasses, 0 );
				for( UINT p = 0; p < cPasses; ++p )
				{
					pShader->FX_BeginPass( p );
					pShader->FX_SetFloat2( "SMAA_PIXEL_SIZE", pixelsize);
					pShader->FX_Commit();

					gkTextureManager::ms_BackBuffer_Mono->Apply(0,0);
					gkTextureManager::ms_SMAA_Blend->Apply(1,0);

					gkPostProcessManager::DrawFullScreenQuad(gkTextureManager::ms_SceneTarget0);

					pShader->FX_EndPass();
				}
				pShader->FX_End();

	//			FX_PopRenderTarget(0);
			}		

			// direct blend
			{
// 				GKHANDLE hTech = pShader->FX_GetTechniqueByName("TemporalResolve");
// 				UINT cPasses;
// 				pShader->FX_SetTechnique( hTech );
// 
// 				pShader->FX_Begin( &cPasses, 0 );
// 				for( UINT p = 0; p < cPasses; ++p )
// 				{
// 					pShader->FX_BeginPass( p );
// 					pShader->FX_SetFloat2( "SMAA_PIXEL_SIZE", pixelsize);
// 
// 					int frameT0 = (gEnv->pProfiler->getFrameCount() % 2) ? 1 : -1;
// 					int frameT1 = (gEnv->pProfiler->getFrameCount() % 2) ? -1 : 1;
// 					Vec4 pParam = Vec4( 0.25f * 1.0f / (float)GetScreenWidth() * frameT0, 0.25f * 1.0f / (float)GetScreenHeight() * frameT1, 0.25f * 1.0f / (float)GetScreenWidth() * frameT1, 0.25f * 1.0f / (float)GetScreenHeight() * frameT0);
// 					//m_ProjMatrix.m20 -= vScreenOffset.x;
// 					//m_ProjMatrix.m21 -= vScreenOffset.y;
// 					//Vec4 pPS_ScreenSize((float)GetScreenWidth(),(float)GetScreenHeight(),1/(float)GetScreenWidth(),1/(float)GetScreenHeight());
// 					//pShader->FX_SetValue("PS_ScreenSize", &pPS_ScreenSize, sizeof(Vec4));
// 					pShader->FX_SetValue("PostMsaaParams", &pParam, sizeof(Vec4));
// 
// 
// 					pShader->FX_Commit();
// 
// 					gkTextureManager::ms_BackBufferA_LeftEye->Apply(0,0);
// 					gkTextureManager::ms_BackBufferB_LeftEye->Apply(1,0);
// 
// 					gkPostProcessManager::DrawFullScreenQuad(gkTextureManager::ms_SceneTarget0);
// 
// 					pShader->FX_EndPass();
// 				}
// 				pShader->FX_End();

			}
			
		}
	}
	else
	{
		if (rtResize < 0.99999 || rtResize > 1.00001 )
		{
			FX_StrechRect(gkTextureManager::ms_BackBufferA_LeftEye, gkTextureManager::ms_SceneTarget0);
		}
		else
		{
			FX_StrechRect(gkTextureManager::ms_BackBufferA_LeftEye, gkTextureManager::ms_BackBuffersStereoOut);
		}
	}

	if ( (g_pRendererCVars->r_sharpenpass != 0 && rtResize < 0.99999))
	{
		FX_StrechRect( gkTextureManager::ms_SceneTarget0, gkTextureManager::ms_OutGlowMask );
		FX_StrechRect( gkTextureManager::ms_SceneTarget0, gkTextureManager::ms_SceneNormal );
		FX_TexBlurGaussian( gkTextureManager::ms_OutGlowMask, 1.0, 1.0, 0.1, gkTextureManager::ms_OutGlowMaskHalf );

		// sharpen
		gkShaderPtr pShader = gkShaderManager::ms_PostCommon;
		UINT cPasses;

		GKHANDLE hTech = pShader->FX_GetTechniqueByName("Sharpen");
		pShader->FX_SetTechnique( hTech );

		gkTextureManager::ms_OutGlowMask->Apply(0,0);
		gkTextureManager::ms_SceneNormal->Apply(1,0);

		// cPasses;
		pShader->FX_Begin( &cPasses, 0 );
		for( UINT p = 0; p < cPasses; ++p )
		{
			pShader->FX_BeginPass( p );

			gkPostProcessManager::DrawFullScreenQuad(gkTextureManager::ms_SceneTarget0);

			pShader->FX_EndPass();
		}
		pShader->FX_End();
	}


	if (rtResize < 0.99999 || rtResize > 1.00001 )
	{
		FX_PopRenderTarget( 0 );
		//FX_StrechToBackBuffer( gkTextureManager::ms_SceneTarget0, m_currStereoType );

		FX_StrechRect( gkTextureManager::ms_SceneTarget0, gkTextureManager::ms_BackBuffersStereoOut );
	}

}


inline HRESULT GetSampleOffsets_DownScale4x4( DWORD dwWidth, DWORD dwHeight, Vec4 avSampleOffsets[])
{
	if(NULL == avSampleOffsets)
		return E_INVALIDARG;

	float tU = 1.0f / dwWidth;
	float tV = 1.0f / dwHeight;

	// Sample from the 16 surrounding points. Since the center point will be in
	// the exact center of 16 texels, a 0.5f offset is needed to specify a texel
	// center.
	int index=0;
	for(int y=0; y<4; y++)
	{
		for(int x=0; x<4; x++)
		{
			avSampleOffsets[index].x = (x - 1.5f) * tU;
			avSampleOffsets[index].y = (y - 1.5f) * tV;
			avSampleOffsets[index].z = 0;
			avSampleOffsets[index].w = 1;

			index++;
		}
	}

	return S_OK;
}

void gkRendererD3D9::RP_HDRProcess()
{
	// prepare shader
	gkShaderPtr pShader = gkShaderManager::ms_HDRProcess;
	GKHANDLE hTech;


	//FX_StrechRect(gkTextureManager::ms_HDRTarget0, gkTextureManager::ms_HDRTargetScaledHalf);
	FX_StrechRect(gkTextureManager::ms_HDRTarget0, gkTextureManager::ms_HDRTargetScaledQuad);

	//////////////////////////////////////////////////////////////////////////
	// now, we prepare the HDR DOF texture here

	// copy it to blur tex
//	FX_StrechRect(gkTextureManager::ms_HDRTargetScaledHalf, gkTextureManager::ms_SceneTargetBlur);

	// set the dof params
	//
	// dof param
	// set fog params [12/7/2011 Kaiming]
	const STimeOfDayKey& tod = getShaderContent().getCurrTodKey();


	// start process

// 	FX_PushRenderTarget(0, gkTextureManager::ms_SceneTargetBlur);
// 
// 	hTech = pShader->FX_GetTechniqueByName("CopyDepthToAlphaNoMask");
// 	pShader->FX_SetTechnique( hTech );
// 
// 
// 
// 
// 	//Vec4 fogParam( 1.0f/ tod.fDofDistance, 300.0f, 0.8f, 1.0f);
// 	//pShader->FX_SetValue( "HDRDofParams", &fogParam, sizeof(Vec4) );
// 
// 	pShader->FX_Commit();
// 
// 	gkTextureManager::ms_SceneDepth->Apply(0, 0);
// 	gkTextureManager::ms_HDRTarget0->Apply(1, 0);
// 
// 	UINT cPasses;
// 	pShader->FX_Begin( &cPasses, 0 );
// 	for( UINT p = 0; p < cPasses; ++p )
// 	{
// 		pShader->FX_BeginPass( p );
// 
// 		gkPostProcessManager::DrawFullScreenQuad(gkTextureManager::ms_SceneTargetBlur);
// 
// 		pShader->FX_EndPass();
// 	}
// 	pShader->FX_End();
// 
// 	FX_PopRenderTarget(0);
// 
// 	// finally blur it
// 	FX_TexBlurGaussian(gkTextureManager::ms_SceneTargetBlur, 1, 1.0f, 5.0f, gkTextureManager::ms_SceneTargetBlurTmp, 4 );

	//////////////////////////////////////////////////////////////////////////
	// optimise, every four time do once

	// initialized
	static gkTexturePtr pTexCurLum = gkTextureManager::ms_HDREyeAdaption[0];
	if (ms_FrameCount % 4 == 0)
	{
		//////////////////////////////////////////////////////////////////////////
		// tonemap initialize
		FX_PushRenderTarget(0, gkTextureManager::ms_HDRToneMap64);


		hTech = pShader->FX_GetTechniqueByName("HDRSampleLumInitial");
		pShader->FX_SetTechnique( hTech );

		// commit the offset kernel
		float s1 = 1.0f / (float) gkTextureManager::ms_HDRTargetScaledQuad->getWidth();
		float t1 = 1.0f / (float) gkTextureManager::ms_HDRTargetScaledQuad->getHeight();       

		// Use rotated grid
		Vec4 vSampleLumOffsets0 = Vec4(s1*0.95f, t1*0.25f, -s1*0.25f, t1*0.96f); 
		Vec4 vSampleLumOffsets1 = Vec4(-s1*0.96f, -t1*0.25f, s1*0.25f, -t1*0.96f);  

		pShader->FX_SetValue("SampleLumOffsets0", &vSampleLumOffsets0, sizeof(Vec4));
		pShader->FX_SetValue("SampleLumOffsets1", &vSampleLumOffsets1, sizeof(Vec4));

		RP_SetHDRParams(pShader);
		pShader->FX_Commit();

		gkTextureManager::ms_HDRTargetScaledQuad->Apply(0, 0);

		UINT cPasses;
		pShader->FX_Begin( &cPasses, 0 );
		for( UINT p = 0; p < cPasses; ++p )
		{
			pShader->FX_BeginPass( p );

			gkPostProcessManager::DrawFullScreenQuad(gkTextureManager::ms_HDRToneMap64);

			pShader->FX_EndPass();
		}
		pShader->FX_End();

		FX_PopRenderTarget(0);


		//////////////////////////////////////////////////////////////////////////
		// tonemap iterate

		// kernel struct
		Vec4 avSampleOffsets[16];

		hTech = pShader->FX_GetTechniqueByName("HDRSampleLumIterative");
		pShader->FX_SetTechnique( hTech );

		{
			// push RT
			FX_PushRenderTarget(0, gkTextureManager::ms_HDRToneMap16);

			// commit the offset kernel and tex
			GetSampleOffsets_DownScale4x4(gkTextureManager::ms_HDRToneMap64->getWidth(), gkTextureManager::ms_HDRToneMap64->getHeight(), avSampleOffsets);
			gkTextureManager::ms_HDRToneMap64->Apply(0, 0);
			pShader->FX_SetValue("SampleOffsets", avSampleOffsets, sizeof(Vec4) * 16);
			RP_SetHDRParams(pShader);
			pShader->FX_Commit();

			UINT cPasses;
			pShader->FX_Begin( &cPasses, 0 );
			for( UINT p = 0; p < cPasses; ++p )
			{
				pShader->FX_BeginPass( p );
				gkPostProcessManager::DrawFullScreenQuad(gkTextureManager::ms_HDRToneMap16);
				pShader->FX_EndPass();
			}
			pShader->FX_End();

			FX_PopRenderTarget(0);
		}

		{
			// push RT
			FX_PushRenderTarget(0, gkTextureManager::ms_HDRToneMap04);

			// commit the offset kernel and tex
			GetSampleOffsets_DownScale4x4(gkTextureManager::ms_HDRToneMap16->getWidth(), gkTextureManager::ms_HDRToneMap16->getHeight(), avSampleOffsets);
			gkTextureManager::ms_HDRToneMap16->Apply(0, 0);
			pShader->FX_SetValue("SampleOffsets", avSampleOffsets, sizeof(Vec4) * 16);
			RP_SetHDRParams(pShader);
			pShader->FX_Commit();

			UINT cPasses;
			pShader->FX_Begin( &cPasses, 0 );
			for( UINT p = 0; p < cPasses; ++p )
			{
				pShader->FX_BeginPass( p );
				gkPostProcessManager::DrawFullScreenQuad(gkTextureManager::ms_HDRToneMap04);
				pShader->FX_EndPass();
			}
			pShader->FX_End();

			FX_PopRenderTarget(0);
		}

		{
			// push RT
			FX_PushRenderTarget(0, gkTextureManager::ms_HDRToneMap01);

			// commit the offset kernel and tex
			GetSampleOffsets_DownScale4x4(gkTextureManager::ms_HDRToneMap04->getWidth(), gkTextureManager::ms_HDRToneMap04->getHeight(), avSampleOffsets);
			gkTextureManager::ms_HDRToneMap04->Apply(0, 0);
			pShader->FX_SetValue("SampleOffsets", avSampleOffsets, sizeof(Vec4) * 16);
			RP_SetHDRParams(pShader);
			pShader->FX_Commit();

			UINT cPasses;
			pShader->FX_Begin( &cPasses, 0 );
			for( UINT p = 0; p < cPasses; ++p )
			{
				pShader->FX_BeginPass( p );
				gkPostProcessManager::DrawFullScreenQuad(gkTextureManager::ms_HDRToneMap01);
				pShader->FX_EndPass();
			}
			pShader->FX_End();

			FX_PopRenderTarget(0);
		}


		//////////////////////////////////////////////////////////////////////////
		// Luminace measure end, eye adatption
		// WIP: skip this now


		hTech = pShader->FX_GetTechniqueByName("HDRCalculateAdaptedLum");
		pShader->FX_SetTechnique( hTech );

		// Swap current & last luminance
		static int nCurLumIndex = 0;
		int nCurLum = nCurLumIndex++;
		nCurLum &= 7;
		gkTexturePtr pTexPrev = gkTextureManager::ms_HDREyeAdaption[(nCurLum-1)&7];
		gkTexturePtr pTexCur = gkTextureManager::ms_HDREyeAdaption[nCurLum];
		pTexCurLum = pTexCur;
		//assert(pTexCur);
		{
			// push RT
			FX_PushRenderTarget(0, pTexCur);

			Vec4 v;
			v[0] = gEnv->pTimer->GetFrameTime();
			v[1] = 1 - powf( 0.98f, 2.0f * 33.0f * v[0]); // eye adapted speed
			v[2] = 1 - powf( 0.98f, 4.0f * 33.0f * v[0]);
			v[3] = 0;

			pShader->FX_SetValue("ElapsedTime", &v, sizeof(Vec4));
			pTexPrev->Apply(0, 0);
			gkTextureManager::ms_HDRToneMap01->Apply(1, 0);
			RP_SetHDRParams(pShader);
			pShader->FX_Commit();

			UINT cPasses;
			pShader->FX_Begin( &cPasses, 0 );
			for( UINT p = 0; p < cPasses; ++p )
			{
				pShader->FX_BeginPass( p );
				gkPostProcessManager::DrawFullScreenQuad(pTexCur);
				pShader->FX_EndPass();
			}
			pShader->FX_End();

			FX_PopRenderTarget(0);
		}
	}



	//////////////////////////////////////////////////////////////////////////
	// BrightPass 


	hTech = pShader->FX_GetTechniqueByName("HDRBrightPassFilter");
	pShader->FX_SetTechnique( hTech );
	{
		// push RT
		FX_PushRenderTarget(0, gkTextureManager::ms_HDRBrightPassQuad);

		// commit the half size hdrtarget as tex0, lum as tex1
		RP_SetHDRParams(pShader);
		gkTextureManager::ms_HDRTargetScaledQuad->Apply(0, 0);
		pTexCurLum->Apply(1, 0);

		pShader->FX_Commit();

		UINT cPasses;
		pShader->FX_Begin( &cPasses, 0 );
		for( UINT p = 0; p < cPasses; ++p )
		{
			pShader->FX_BeginPass( p );
			gkPostProcessManager::DrawFullScreenQuad(gkTextureManager::ms_HDRBrightPassQuad);
			pShader->FX_EndPass();
		}
		pShader->FX_End();

		FX_PopRenderTarget(0);
	}

	FX_TexBlurGaussian(gkTextureManager::ms_HDRBrightPassQuad, 1, 1.0f, 3.0f, gkTextureManager::ms_HDRBrightPassQuadTmp, 1);
	// downscale
	FX_StrechRect( gkTextureManager::ms_HDRBrightPassQuad, gkTextureManager::ms_HDRBrightPassQuadQuad);
	FX_TexBlurGaussian(gkTextureManager::ms_HDRBrightPassQuadQuad, 1, 1.0f, 3.0f, gkTextureManager::ms_HDRBrightPassQuadQuadTmp, 1);

	FX_StrechRect( gkTextureManager::ms_HDRBrightPassQuadQuad, gkTextureManager::ms_HDRBrightPassQuadQuadQuad );
	FX_TexBlurGaussian(gkTextureManager::ms_HDRBrightPassQuadQuadQuad, 1, 1.0f, 3.0f, gkTextureManager::ms_HDRBrightPassQuadQuadQuadTmp, 1);


	// now we get the 3 bloom based texture
	// finally GEN the bloom map
	// we should bind streak after


	// make streak


	hTech = pShader->FX_GetTechniqueByName("HDRStreakInitial");
	pShader->FX_SetTechnique( hTech );
	{
		// push RT
		FX_PushRenderTarget(0, gkTextureManager::ms_HDRStreak);

		// commit the half size hdrtarget as tex0, lum as tex1

		gkTextureManager::ms_HDRBrightPassQuad->Apply(0, 0);
		gkTextureManager::ms_HDRBrightPassQuadQuad->Apply(1, 0);
		gkTextureManager::ms_HDRBrightPassQuadQuadQuad->Apply(2, 0);
		RP_SetHDRParams(pShader);
		pShader->FX_Commit();
		UINT cPasses;
		pShader->FX_Begin( &cPasses, 0 );
		for( UINT p = 0; p < cPasses; ++p )
		{
			pShader->FX_BeginPass( p );
			gkPostProcessManager::DrawFullScreenQuad(gkTextureManager::ms_HDRStreak);
			pShader->FX_EndPass();
		}
		pShader->FX_End();

		FX_PopRenderTarget(0);
	}

	FX_TexBlurDirectional(gkTextureManager::ms_HDRStreak, Vec2(16.0f, 0.0f), 2, gkTextureManager::ms_HDRBrightPassQuadQuadTmp);


	// Bloommerge
	hTech = pShader->FX_GetTechniqueByName("HDRBloomGen");
	pShader->FX_SetTechnique( hTech );
	{
		// push RT

		// here, we push BloomCache swaped everyday [10/25/2011 Kaiming]
		if (ms_FrameCount % 2 == 0)
		{
			FX_PushRenderTarget(0, gkTextureManager::ms_HDRBloom0Quad);
			gkTextureManager::ms_HDRBloom1Quad->Apply(4, 0);
		}
		else
		{
			FX_PushRenderTarget(0, gkTextureManager::ms_HDRBloom1Quad);
			gkTextureManager::ms_HDRBloom0Quad->Apply(4, 0);
		}


		// commit the half size hdrtarget as tex0, lum as tex1

		gkTextureManager::ms_HDRBrightPassQuad->Apply(0, 0);
		gkTextureManager::ms_HDRBrightPassQuadQuad->Apply(1, 0);
		gkTextureManager::ms_HDRBrightPassQuadQuadQuad->Apply(2, 0);
		gkTextureManager::ms_HDRStreak->Apply(3, 0);
		RP_SetHDRParams(pShader);
		pShader->FX_Commit();

		UINT cPasses;
		pShader->FX_Begin( &cPasses, 0 );
		for( UINT p = 0; p < cPasses; ++p )
		{
			pShader->FX_BeginPass( p );
			gkPostProcessManager::DrawFullScreenQuad(gkTextureManager::ms_HDRBloom0Quad);		// just for size, ignore this
			pShader->FX_EndPass();
		}
		pShader->FX_End();

		FX_PopRenderTarget(0);
	}


	// Sun Shaft Gen
	// Common Params

	// calc params and set
	Vec3 pSunPos = getShaderContent().getLightDir() * -100000.0f;

	Vec4 pParams= Vec4(pSunPos, 1.0f);   
	// no need to waste gpu to compute sun screen pos
	Vec4 pSunPosScreen = getShaderContent().getMainCamViewProjMatrix() * pParams;
	pSunPosScreen.x = ( ( pSunPosScreen.x + pSunPosScreen.w) * 0.5f ) /pSunPosScreen.w;   
	pSunPosScreen.y = ( (-pSunPosScreen.y + pSunPosScreen.w) * 0.5f ) /pSunPosScreen.w;

	Vec3 camFront = getShaderContent().getInverseViewMatrix().GetRow(0).GetNormalized();
	// if not face sun, not so light
	pSunPosScreen.w = getShaderContent().getLightDir().dot(getShaderContent().getCamDir());
	// if sun under ground, not light
	pSunPosScreen.w *= clamp(getShaderContent().getLightDir().z * 5.0f, 0.0f, 1.f);


	STimeOfDayKey& todkey = getShaderContent().getCurrTodKey();
	// big radius, project until end of screen
	Vec4 pShaftParams;
	pShaftParams.x = 0.1f;// * todkey.fSunShaftLength;    
	pShaftParams.y = clamp_tpl<float>(todkey.fSunShaftAtten, 0.0f, 10.0f);  

	// Bloommerge
	hTech = pShader->FX_GetTechniqueByName("SunShaftsGen");
	pShader->FX_SetTechnique( hTech );
	{
		// push RT

		FX_PushRenderTarget(0, gkTextureManager::ms_BackBufferQuad);
		_clearBuffer(false, 0x00000000);

		// commit the position map
		gkTextureManager::ms_SceneDepth->Apply(0, 0);



		pShader->FX_SetValue("PI_sunShaftsParams", &pShaftParams, sizeof(Vec4));
		pShader->FX_SetValue("SunShafts_SunPos", &pSunPosScreen, sizeof(Vec4));
		RP_SetHDRParams(pShader);
		pShader->FX_Commit();

		UINT cPasses;
		pShader->FX_Begin( &cPasses, 0 );
		for( UINT p = 0; p < cPasses; ++p )
		{
			pShader->FX_BeginPass( p );
			gkPostProcessManager::DrawFullScreenQuad(gkTextureManager::ms_BackBufferQuad);		// just for size, ignore this
			pShader->FX_EndPass();
		}
		pShader->FX_End();

		FX_PopRenderTarget(0);
	}

	// SunShaft Iterate
	hTech = pShader->FX_GetTechniqueByName("SunShaftsIterate");
	pShader->FX_SetTechnique( hTech );
	{
		// push RT
		for (uint32 i=0; i < 1; ++i)
		{
			FX_PushRenderTarget(0, gkTextureManager::ms_BackBufferQuadTmp);

			// commit the position map
			gkTextureManager::ms_BackBufferQuad->Apply(0, 0);


			pShaftParams.x = 0.025f;// * todkey.fSunShaftVisibility;
			pShader->FX_SetValue("PI_sunShaftsParams", &pShaftParams, sizeof(Vec4));
			pShader->FX_SetValue("SunShafts_SunPos", &pSunPosScreen, sizeof(Vec4));

			pShader->FX_Commit();

			UINT cPasses;
			pShader->FX_Begin( &cPasses, 0 );
			for( UINT p = 0; p < cPasses; ++p )
			{
				pShader->FX_BeginPass( p );
				gkPostProcessManager::DrawFullScreenQuad(gkTextureManager::ms_BackBufferQuadTmp);		// just for size, ignore this
				pShader->FX_EndPass();
			}
			pShader->FX_End();

			FX_PopRenderTarget(0);


			FX_PushRenderTarget(0, gkTextureManager::ms_BackBufferQuad);

			// commit the position map
			gkTextureManager::ms_BackBufferQuadTmp->Apply(0, 0);


			pShaftParams.x = 0.01f;// * todkey.fSunShaftVisibility;   
			pShader->FX_SetValue("PI_sunShaftsParams", &pShaftParams, sizeof(Vec4));
			pShader->FX_SetValue("SunShafts_SunPos", &pSunPosScreen, sizeof(Vec4));

			pShader->FX_Commit();

			cPasses;
			pShader->FX_Begin( &cPasses, 0 );
			for( UINT p = 0; p < cPasses; ++p )
			{
				pShader->FX_BeginPass( p );
				gkPostProcessManager::DrawFullScreenQuad(gkTextureManager::ms_BackBufferQuad);		// just for size, ignore this
				pShader->FX_EndPass();
			}
			pShader->FX_End();

			FX_PopRenderTarget(0);
		}
	}




	// finally, the final bind, bind, bind!
	hTech = pShader->FX_GetTechniqueByName("HDRFinalScene");
	pShader->FX_SetTechnique( hTech );
	{
		// push RT
		FX_PushRenderTarget(0, gkTextureManager::ms_BackBuffer);

		// commit the half size hdrtarget as tex0, lum as tex1

		gkTextureManager::ms_HDRTarget0->Apply(0, 0);
		gkTextureManager::ms_HDRToneMap01->Apply(1, 0);
		if (ms_FrameCount % 2 == 0)
			gkTextureManager::ms_HDRBloom0Quad->Apply(2, 0);
		else
			gkTextureManager::ms_HDRBloom1Quad->Apply(2, 0);

		static gkTexturePtr texVignetting;
		texVignetting = gEnv->pSystem->getTextureMngPtr()->loadSync(_T("Engine/Assets/Textures/procedure/vignetting.dds"), _T("internal"));
		if (!texVignetting.isNull())
		{
			texVignetting->Apply(3, 0);
		}
		
		// apply sunshaft
		gkTextureManager::ms_BackBufferQuad->Apply(4,0);

		// apply DOF 
 		//gkTextureManager::ms_SceneDepth->Apply(5,0);
 		//gkTextureManager::ms_SceneTargetBlur->Apply(6,0);

		
		static gkTexturePtr texNoise;
		texNoise = gEnv->pSystem->getTextureMngPtr()->loadSync(_T("Engine/Assets/Textures/procedure/jumpnoiseHF_x27y19.dds"), _T("internal"));
		if (!texNoise.isNull())
		{
			texNoise->Apply(7, 0);
		}
		

		gkTextureManager::ms_MergedColorChart->Apply(8,0);

		RP_SetHDRParams(pShader);

		// dof param
		// set fog params [12/7/2011 Kaiming]
		const STimeOfDayKey& tod = getShaderContent().getCurrTodKey();

// 		Vec4 fogParam( 1.0f/ tod.fDofDistance, 300.0f, 0.8f, 1.0f);
// 		pShader->FX_SetValue( "HDRDofParams", &fogParam, sizeof(Vec4) );


		pShader->FX_Commit();
		UINT cPasses;
		pShader->FX_Begin( &cPasses, 0 );
		for( UINT p = 0; p < cPasses; ++p )
		{
			pShader->FX_BeginPass( p );
			gkPostProcessManager::DrawFullScreenQuad(gkTextureManager::ms_BackBuffer);
			pShader->FX_EndPass();
		}
		pShader->FX_End();

		FX_PopRenderTarget(0);
	}

	// HDR process down here;
}

void gkRendererD3D9::RP_DepthOfField()
{
	const STimeOfDayKey& tod = getShaderContent().getCurrTodKey();

	// prepare shader
	gkShaderPtr pShader = gkShaderManager::ms_HDRProcess;
	GKHANDLE hTech;

	FX_StrechRect(gkTextureManager::ms_BackBuffer, gkTextureManager::ms_SceneTargetBlur);

	// finally blur it
	FX_TexBlurGaussian(gkTextureManager::ms_SceneTargetBlur, 1, 1.0f, 5.0f * tod.fDofBlurIntensity, gkTextureManager::ms_SceneTargetBlurTmp, 2 );
	
	// picture was store at ms_backbuffer
	FX_PushRenderTarget(0, gkTextureManager::ms_BackBuffer);

	SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	hTech = pShader->FX_GetTechniqueByName("BlendDOF");
	pShader->FX_SetTechnique( hTech ); 
	
	
	Vec4 fogParam( tod.fDofFocusPlane - tod.fDofFocusDistance, tod.fDofFocusPlane + tod.fDofFocusDistance, tod.fDofDistance, max(tod.fDofDistanceNear, 1.0f) );
	pShader->FX_SetValue( "HDRDofParams", &fogParam, sizeof(Vec4) );
	 
	pShader->FX_Commit();
	 
	gkTextureManager::ms_SceneDepth->Apply(0, 0);
	gkTextureManager::ms_SceneTargetBlur->Apply(1, 0);
	 
	UINT cPasses;
	pShader->FX_Begin( &cPasses, 0 );
	for( UINT p = 0; p < cPasses; ++p )
	{
	 	pShader->FX_BeginPass( p );
	 
	 	gkPostProcessManager::DrawFullScreenQuad(gkTextureManager::ms_BackBuffer);
	 
	 	pShader->FX_EndPass();
	}
	pShader->FX_End();
	 
	FX_PopRenderTarget(0);
	 
	SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);


	// blend with backbuffer

}


void gkRendererD3D9::RP_ZprepassPrepare()
{

}

void gkRendererD3D9::RP_ZprepassEnd()
{



}

void gkRendererD3D9::RP_SetHDRParams( gkShaderPtr pShader )
{
	const STimeOfDayKey& tod = getShaderContent().getCurrTodKey();

	// hdr level
	Vec4 HDRParams1 = Vec4(0.0f, 4.0f, 10.0f, 1.0f);
	// SceneLum, SceneScale, HDRConstract, 
	Vec4 HDRParams2 = Vec4(1.0f, 1.0f, tod.fHDRConstract, 1.0f);

	ColorF HDRParams5 = tod.clBloomColor * tod.fBloomIntensity;
	HDRParams5.a = g_pRendererCVars->r_HDRGrain;
	ColorF HDRParams7 = tod.clStreakColor * tod.fStreakIntensity;


	pShader->FX_SetValue("HDRParams1", &HDRParams1, sizeof(Vec4));
	pShader->FX_SetValue("HDRParams2", &HDRParams2, sizeof(Vec4));

	pShader->FX_SetValue("HDRParams5", &HDRParams5, sizeof(ColorF));
	pShader->FX_SetValue("HDRParams7", &HDRParams7, sizeof(ColorF));

	static uint32 dwNoiseOffsetX=0;
	static uint32 dwNoiseOffsetY=0;

	dwNoiseOffsetX = (dwNoiseOffsetX+27)&0x3f;
	dwNoiseOffsetY = (dwNoiseOffsetY+19)&0x3f;

	Vec4 pFrameRand(dwNoiseOffsetX/64.0f, dwNoiseOffsetX/64.0f, 0,0 );
	pShader->FX_SetValue("FrameRand", &pFrameRand, sizeof(Vec4));	

	Vec4 pPS_ScreenSize((float)GetScreenWidth(),(float)GetScreenHeight(),1/(float)GetScreenWidth(),1/(float)GetScreenHeight());
	pShader->FX_SetValue("PS_ScreenSize", &pPS_ScreenSize, sizeof(Vec4));	

}

void gkRendererD3D9::RP_FXSpecil()
{

	FX_PushRenderTarget(0, gkTextureManager::ms_OutGlowMask);
	//_clearBuffer(false, 0x00000000);

	m_pd3d9Device->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_STENCIL , 0x00000000, 1.0f, 0L );

	gkShaderPtr pShader = gkShaderManager::ms_SpecilEffects;
	GKHANDLE hTech = pShader->FX_GetTechniqueByName("OutGlowMaskGen");
	pShader->FX_SetTechnique( hTech );


	// stencil clear [11/24/2011 Kaiming]
	// open stencil test for writing
	RS_SetRenderState(D3DRS_STENCILENABLE, TRUE);

	// 0x0 mask the shadow cascade
	RS_SetRenderState(D3DRS_STENCILWRITEMASK, 0xffffffff);
	RS_SetRenderState(D3DRS_STENCILREF, 0x0);

	// 模板掩码设为0
	RS_SetRenderState( D3DRS_STENCILMASK,     0x0 );

	// always success
	RS_SetRenderState( D3DRS_STENCILFUNC,     D3DCMP_ALWAYS );
	// if success or failed, both increase
	RS_SetRenderState( D3DRS_STENCILPASS, D3DSTENCILOP_INCRSAT );
	RS_SetRenderState( D3DRS_STENCILZFAIL, D3DSTENCILOP_INCRSAT );

	gkTextureManager::ms_SceneTarget0->Apply(0, 0);

	static gkTexturePtr texVignetting;
	texVignetting = gEnv->pSystem->getTextureMngPtr()->loadSync(_T("outglowcolor.png"), _T("internal"));
	texVignetting->Apply(1, 0);

	UINT cPasses;
	pShader->FX_Begin( &cPasses, 0 );
	for( UINT p = 0; p < cPasses; ++p )
	{
		pShader->FX_BeginPass( p );

		gkPostProcessManager::DrawFullScreenQuad(gkTextureManager::ms_OutGlowMask);

		pShader->FX_EndPass();
	}
	pShader->FX_End();

	FX_PopRenderTarget(0);

	FX_StrechRect(gkTextureManager::ms_OutGlowMask, gkTextureManager::ms_OutGlowMaskHalf);

	FX_TexBlurGaussian(gkTextureManager::ms_OutGlowMaskHalf, 1, 1.0f, 3.0f, gkTextureManager::ms_OutGlowMaskHalfSwap);


	FX_PushRenderTarget(0, gkTextureManager::ms_SceneTarget0);

	hTech = pShader->FX_GetTechniqueByName("OutGlowBlend");
	pShader->FX_SetTechnique( hTech );

	RS_SetRenderState( D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP );
	RS_SetRenderState( D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP );
	RS_SetRenderState( D3DRS_STENCILPASS, D3DSTENCILOP_KEEP );
	RS_SetRenderState( D3DRS_STENCILFUNC, D3DCMP_EQUAL );


	RS_SetRenderState( D3DRS_STENCILMASK, 0xffffffff );
	RS_SetRenderState( D3DRS_STENCILREF, 0x0 );

	gkTextureManager::ms_OutGlowMaskHalf->Apply(0, 0);

	pShader->FX_Begin( &cPasses, 0 );
	for( UINT p = 0; p < cPasses; ++p )
	{
		pShader->FX_BeginPass( p );

		gkPostProcessManager::DrawFullScreenQuad(gkTextureManager::ms_SceneTarget0);

		pShader->FX_EndPass();
	}
	pShader->FX_End();

	RS_SetRenderState(D3DRS_STENCILENABLE, FALSE);

	FX_PopRenderTarget(0);
}
