#include "RendererD3D9StableHeader.h"
#include "RendPipe_LightPasses.h"
#include "Profiler/gkGPUTimer.h"
#include "RenderRes/gkShader.h"
#include "RenderRes/gkMesh.h"
#include "gkPostProcessManager.h"



void RendPipe_LightPasses::Prepare(gkRenderSequence* renderSeq)
{
	
}

void RendPipe_LightPasses::Execute(gkRenderSequence* renderSeq)
{
	gkShader* pShader = reinterpret_cast<gkShader*>(gkShaderManager::ms_DeferredLighting.getPointer());
	if (!pShader)
	{
		return;
	}


	gkRendererD3D9::FX_PushRenderTarget(0, gkTextureManager::ms_SceneDifAcc, 0, 0, true);
	//_clearBuffer( true, 0x7f7f7f7f );
	//_clearBuffer(false, 0x0);
	gkRendererD3D9::FX_PushRenderTarget(1, gkTextureManager::ms_SceneSpecAcc);
	//_clear2Buffer(false, 0x0);
	PROFILE_LABEL_PUSH("DEFERRED_LIGHTING");
	gkRendererD3D9::ms_GPUTimers[_T("Deferred Lighting")].start();

	// DO NOT WRITE Z ! SO WE CAN RENDER AUX GEOM AFTER THIS!
	//  [8/21/2011 Kaiming-Desktop]
	gkRendererD3D9::SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	gkRendererD3D9::SetRenderState(D3DRS_ZENABLE, FALSE);
	gkRendererD3D9::SetRenderState(D3DRS_STENCILENABLE, FALSE);
	gkRendererD3D9::SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	PROFILE_LABEL_PUSH("AMBIENT & SUN");
	gkRendererD3D9::ms_GPUTimers[_T("Ambient Pass")].start();

	//////////////////////////////////////////////////////////////////////////
	// 1. first, Ambient & Sun pass

	gkTextureManager::ms_SceneNormal->Apply(0, 0);
	gkTextureManager::ms_SceneDepth->Apply(1, 0);
	gkTextureManager::ms_SSAOTarget->Apply(3, 0);
	gkTextureManager::ms_ShadowMask->Apply(4, 0);

	//Vec3 lightdirInWorld = getShaderContent().getLightDirViewSpace();

	pShader->FX_SetValue("g_mViewI", &(gkRendererD3D9::getShaderContent().getInverseViewMatrix()), sizeof(Matrix44));

	const STimeOfDayKey& tod = gEnv->p3DEngine->getTimeOfDay()->getCurrentTODKey();
	pShader->FX_SetColor4("g_Ambient", tod.clSkyLight);
	pShader->FX_SetColor4("g_cAmbGround", tod.clGroundAmb);

	Vec4 ambHeightParam;
	ambHeightParam.x = tod.fGroundAmbMin;
	ambHeightParam.y = tod.fGroundAmbMax;
	ambHeightParam.z = 1.0f / tod.fGroundAmbMax;
	pShader->FX_SetFloat4("g_vAmbHeightParams", ambHeightParam);

	if (g_pRendererCVars->r_ShadingMode == 0)
	{
		pShader->FX_SetTechnique("RenderLightPass");
	}
	else
	{
		pShader->FX_SetTechnique("RenderLightPassDS");
		gkTextureManager::ms_ShadowMask->Apply(2, 0);

		// set light Params from TOD
		const STimeOfDayKey& tod = gkRendererD3D9::getShaderContent().getCurrTodKey();
		ColorF lightdif = tod.clSunLight;
		ColorF lightspec = tod.clSunLight;
		lightdif *= (tod.fSunIntensity / 10.0f);
		lightspec *= tod.fSunSpecIntensity;

		pShader->FX_SetValue("g_LightPos", &(gkRendererD3D9::getShaderContent().getLightDir()), sizeof(Vec3));
		pShader->FX_SetFloat4("g_camPos", gkRendererD3D9::getShaderContent().getCamPos());
		pShader->FX_SetValue("g_LightDiffuse", &lightdif, sizeof(ColorF));
		pShader->FX_SetValue("g_LightSpecular", &lightspec, sizeof(ColorF));
	}


	UINT cPasses;
	pShader->FX_Begin(&cPasses, 0);
	for (UINT p = 0; p < cPasses; ++p)
	{
		pShader->FX_BeginPass(p);




		pShader->FX_Commit();
		gkPostProcessManager::DrawFullScreenQuad(gkTextureManager::ms_SceneDifAcc);

		pShader->FX_EndPass();
	}
	pShader->FX_End();

	gkRendererD3D9::ms_GPUTimers[_T("Ambient Pass")].stop();
	PROFILE_LABEL_POP("AMBIENT & SUN");
	//////////////////////////////////////////////////////////////////////////
	// 2. next, PointLight pass


#if 1


	gkRendererD3D9::SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	gkRendererD3D9::SetRenderState(D3DRS_STENCILENABLE, TRUE);

	// MAY WE SHOULD CLEAR STENCIL BUFFER
	gkRendererD3D9::SetRenderState(D3DRS_ZENABLE, TRUE);
	gkRendererD3D9::ms_GPUTimers[_T("Lights")].start();


	// PASS THE FAR_CLIP_DELTA
	Vec4 ddx = gkRendererD3D9::getShaderContent().getCamFarVerts(0) - gkRendererD3D9::getShaderContent().getCamFarVerts(2);
	Vec4 ddy = gkRendererD3D9::getShaderContent().getCamFarVerts(3) - gkRendererD3D9::getShaderContent().getCamFarVerts(2);

	pShader->FX_SetFloat4("PS_lefttop", gkRendererD3D9::getShaderContent().getCamFarVerts(2));
	pShader->FX_SetFloat4("PS_ddx", ddx);
	pShader->FX_SetFloat4("PS_ddy", ddy);
	pShader->FX_SetFloat4("g_camPos", gkRendererD3D9::getShaderContent().getCamPos());

	for (gkRenderLightList::const_iterator it = renderSeq->getRenderLightList().begin(); it != renderSeq->getRenderLightList().end(); ++it)
	{

		PROFILE_LABEL_PUSH("LIGHT");

		getRenderer()->getDevice()->Clear(0, NULL, D3DCLEAR_STENCIL, 0, 0, 0L);

		// STENCIL PASS
		pShader->FX_SetTechnique("RenderLightStencilPass");

		// SET STATE
		if ((it->m_vPos - gkRendererD3D9::getShaderContent().getCamPos().GetXYZ()).GetLength() < it->m_fRadius)
		{
			gkRendererD3D9::SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
			gkRendererD3D9::SetRenderState(D3DRS_ZFUNC, D3DCMP_GREATER);
		}
		else
		{
			gkRendererD3D9::SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
			gkRendererD3D9::SetRenderState(D3DRS_ZFUNC, D3DCMP_LESS);
		}

		gkRendererD3D9::SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		gkRendererD3D9::SetRenderState(D3DRS_COLORWRITEENABLE, 0);

		gkRendererD3D9::SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);
		gkRendererD3D9::SetRenderState(D3DRS_STENCILWRITEMASK, 0xffffffff);
		gkRendererD3D9::SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_INCRSAT);
		gkRendererD3D9::SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_INCRSAT);

		pShader->FX_Begin(&cPasses, 0);
		for (UINT p = 0; p < cPasses; ++p)
		{
			pShader->FX_BeginPass(p);
			// build matrix
			Matrix34 mat;
			mat.Set(Vec3((*it).m_fRadius, (*it).m_fRadius, (*it).m_fRadius),
				Quat::CreateIdentity(), (*it).m_vPos);
			Matrix44 matForRender(mat);
			matForRender.Transpose();
			matForRender = matForRender * gkRendererD3D9::getShaderContent().getViewMatrix();
			pShader->FX_SetValue("g_mWorldView", &matForRender, sizeof(Matrix44));
			matForRender = matForRender * gkRendererD3D9::getShaderContent().getProjectionMatrix();
			pShader->FX_SetValue("g_mWorldViewProj", &matForRender, sizeof(Matrix44));
			pShader->FX_SetValue("g_mProj", &(gkRendererD3D9::getShaderContent().getProjectionMatrix()), sizeof(Matrix44));

			pShader->FX_Commit();

			// light mesh
			gkMeshManager::ms_generalSphere->touch();
			gkRenderOperation rop;
			gkMeshManager::ms_generalSphere->getRenderOperation(rop);

			gkRendererD3D9::_render(rop);

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
			pShader->FX_SetTechnique("RenderLightGeneralGlobalShadowPass");
		}
		else
		{

			pShader->FX_SetTechnique("RenderLightGeneralPass");
		}


		// SET STATE
		gkRendererD3D9::SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
		gkRendererD3D9::SetRenderState(D3DRS_ZFUNC, D3DCMP_GREATER);

		gkRendererD3D9::SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
		gkRendererD3D9::SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

		gkRendererD3D9::SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		gkRendererD3D9::SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_ALPHA);

		gkRendererD3D9::SetRenderState(D3DRS_STENCILFUNC, D3DCMP_EQUAL);
		gkRendererD3D9::SetRenderState(D3DRS_STENCILMASK, 0xf);
		gkRendererD3D9::SetRenderState(D3DRS_STENCILREF, 0x1);
		gkRendererD3D9::SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);
		gkRendererD3D9::SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
		gkRendererD3D9::SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);

		pShader->FX_Begin(&cPasses, 0);
		for (UINT p = 0; p < cPasses; ++p)
		{
			pShader->FX_BeginPass(p);
			// build matrix
			Matrix34 mat;
			mat.Set(Vec3((*it).m_fRadius, (*it).m_fRadius, (*it).m_fRadius),
				Quat::CreateIdentity(), (*it).m_vPos);
			Matrix44 matForRender(mat);
			matForRender.Transpose();
			matForRender = matForRender * gkRendererD3D9::getShaderContent().getViewMatrix();
			pShader->FX_SetValue("g_mWorldView", &matForRender, sizeof(Matrix44));
			matForRender = matForRender * gkRendererD3D9::getShaderContent().getProjectionMatrix();
			pShader->FX_SetValue("g_mWorldViewProj", &matForRender, sizeof(Matrix44));

			matForRender = gkRendererD3D9::getShaderContent().getViewMatrix() * gkRendererD3D9::getShaderContent().getProjectionMatrix();

			pShader->FX_SetValue("g_mProj", &matForRender, sizeof(Matrix44));

			ColorF lightParam0 = (*it).m_vDiffuse;
			lightParam0.a = (*it).m_fRadius;
			ColorF lightSpec = (*it).m_vSpecular;
			pShader->FX_SetColor4("g_LightDiffuse", lightParam0);
			pShader->FX_SetColor4("g_LightSpecular", lightSpec);
			pShader->FX_SetFloat3("g_LightPos", (*it).m_vPos);

			static gkTexturePtr texRandomStep;
			texRandomStep = gEnv->pSystem->getTextureMngPtr()->loadSync(_T("Engine/Assets/Textures/procedure/randomstep.dds"), _T("internal"));
			texRandomStep->Apply(2, 0);

			gkTextureManager::ms_ShadowMask->Apply(3, 0);

			pShader->FX_Commit();

			// light mesh
			gkMeshManager::ms_generalSphere->touch();
			gkRenderOperation rop;
			gkMeshManager::ms_generalSphere->getRenderOperation(rop);

			gkRendererD3D9::_render(rop);

			pShader->FX_EndPass();
		}
		pShader->FX_End();


		PROFILE_LABEL_POP("LIGHT");
	}


	getRenderer()->getDevice()->Clear(0, NULL, D3DCLEAR_STENCIL, 0, 0, 0);

	gkRendererD3D9::ms_GPUTimers[_T("Lights")].stop();

#endif

	gkRendererD3D9::ms_GPUTimers[_T("Deferred Lighting")].stop();
	PROFILE_LABEL_POP("DEFERRED_LIGHTING");

	//////////////////////////////////////////////////////////////////////////
	// back to SRT rendering, set RT1 to null
	gkRendererD3D9::FX_PopRenderTarget(0);
	gkRendererD3D9::FX_PopRenderTarget(1);

	gkRendererD3D9::SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	gkRendererD3D9::SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	gkRendererD3D9::SetRenderState(D3DRS_ZENABLE, FALSE);

	gkRendererD3D9::FX_ClearAllSampler();
}

void RendPipe_LightPasses::End(gkRenderSequence* renderSeq)
{
	// wether do it or not, here we close stencil
	gkRendererD3D9::RS_SetRenderState(D3DRS_STENCILENABLE, FALSE);
}
