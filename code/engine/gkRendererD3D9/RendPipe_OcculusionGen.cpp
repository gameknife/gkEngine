#include "RendererD3D9StableHeader.h"
#include "RendPipe_OcculusionGen.h"
#include "Profiler/gkGPUTimer.h"
#include "gkRenderSequence.h"
#include "RenderRes/gkTextureManager.h"
#include "gkPostProcessManager.h"

void gkRendPipe_OcculusionGen::Prepare(gkRenderSequence* renderSeq)
{
	
}

void gkRendPipe_OcculusionGen::Execute(gkRenderSequence* renderSeq)
{


	//////////////////////////////////////////////////////////////////////////
	// 4. SSAO

	PROFILE_LABEL_PUSH("SSAO");
	gkRendererD3D9::ms_GPUTimers[_T("SSAO")].start();

	STimeOfDayKey& tod = gkRendererD3D9::getShaderContent().getCurrTodKey();
	//////////////////////////////////////////////////////////////////////////
	// SSAO


	if (g_pRendererCVars->r_ssaodownscale < 0.01f)
	{
		gkTextureManager::ms_SSAOTargetTmp->changeAttr(_T("size"), _T("full"));
		gkTextureManager::ms_SSAOTarget->changeAttr(_T("size"), _T("full"));
	}
	else if (g_pRendererCVars->r_ssaodownscale < 1.01f)
	{
		gkTextureManager::ms_SSAOTargetTmp->changeAttr(_T("size"), _T("half"));
		gkTextureManager::ms_SSAOTarget->changeAttr(_T("size"), _T("half"));
	}
	else
	{
		gkTextureManager::ms_SSAOTargetTmp->changeAttr(_T("size"), _T("quad"));
		gkTextureManager::ms_SSAOTarget->changeAttr(_T("size"), _T("quad"));
	}

	gkRendererD3D9::FX_PushRenderTarget(0, gkTextureManager::ms_SSAOTargetTmp);
	gkRendererD3D9::_clearBuffer(false, 0xFFFFFFFF);

	getRenderer()->SetRenderState(D3DRS_ZENABLE, FALSE);
	getRenderer()->SetRenderState(D3DRS_STENCILENABLE, FALSE);

	if (g_pRendererCVars->r_SSAO)
	{
		// selete tech
		gkShaderPtr pShader = gkShaderManager::ms_SSAO;
		GKHANDLE hTech = pShader->FX_GetTechniqueByName("SSAO");
		pShader->FX_SetTechnique(hTech);

		// params set
		gkTextureManager::ms_SceneNormal->Apply(0, 0);
		gkTextureManager::ms_SceneDepth->Apply(1, 0);
		gkTextureManager::ms_RotSamplerAO->Apply(2, 0);

		// PASS THE FAR_CLIP_DELTA
		Vec4 ddx = gkRendererD3D9::getShaderContent().getCamFarVerts(0) - gkRendererD3D9::getShaderContent().getCamFarVerts(2);
		Vec4 ddy = gkRendererD3D9::getShaderContent().getCamFarVerts(3) - gkRendererD3D9::getShaderContent().getCamFarVerts(2);

		pShader->FX_SetFloat4("PS_ddx", ddx);
		pShader->FX_SetFloat4("PS_ddy", ddy);
		pShader->FX_SetFloat4("PS_ScreenSize", Vec4(gkTextureManager::ms_SSAOTargetTmp->getWidth(), gkTextureManager::ms_SSAOTargetTmp->getHeight(), 0, 0));
		pShader->FX_SetFloat4("AOSetting", Vec4(g_pRendererCVars->r_SSAOScale, g_pRendererCVars->r_SSAOBias, g_pRendererCVars->r_SSAOAmount * tod.fSSAOIntensity, g_pRendererCVars->r_SSAORadius));
		pShader->FX_SetMatrix("matView", gkRendererD3D9::getShaderContent().getViewMatrix());
		pShader->FX_SetFloat4("g_camPos", gkRendererD3D9::getShaderContent().getCamPos());
		pShader->FX_Commit();

		UINT cPasses;
		pShader->FX_Begin(&cPasses, 0);
		for (UINT p = 0; p < cPasses; ++p)
		{
			pShader->FX_BeginPass(p);

			gkPostProcessManager::DrawFullScreenQuad(gkTextureManager::ms_SSAOTargetTmp, Vec4(0, 0, 1, 1), Vec2(getRenderer()->GetScreenWidth() / 4.f, getRenderer()->GetScreenHeight() / 4.f));

			pShader->FX_EndPass();
		}
		pShader->FX_End();
	}

	gkRendererD3D9::FX_PopRenderTarget(0);


	gkRendererD3D9::FX_PushRenderTarget(0, gkTextureManager::ms_SSAOTarget);
	if (g_pRendererCVars->r_SSAO)
	{
		// selete tech
		gkShaderPtr pShader = gkShaderManager::ms_SSAO;
		GKHANDLE hTech = pShader->FX_GetTechniqueByName("SSAO_Blur");
		pShader->FX_SetTechnique(hTech);

		// params set
		gkTextureManager::ms_SSAOTargetTmp->Apply(0, 0);
		gkTextureManager::ms_SceneDepth->Apply(1, 0);

		// PASS THE FAR_CLIP_DELTA
		Vec4 ddx = gkRendererD3D9::getShaderContent().getCamFarVerts(2) - gkRendererD3D9::getShaderContent().getCamFarVerts(0);
		Vec4 ddy = gkRendererD3D9::getShaderContent().getCamFarVerts(1) - gkRendererD3D9::getShaderContent().getCamFarVerts(0);

		pShader->FX_SetFloat4("blurKernel1", Vec4(-0.5f / (float)gkTextureManager::ms_SSAOTargetTmp->getWidth(), -0.5f / (float)gkTextureManager::ms_SSAOTargetTmp->getHeight(), 1.5f / (float)gkTextureManager::ms_SSAOTargetTmp->getWidth(), -0.5f / (float)gkTextureManager::ms_SSAOTargetTmp->getHeight()));
		pShader->FX_SetFloat4("blurKernel2", Vec4(-0.5f / (float)gkTextureManager::ms_SSAOTargetTmp->getWidth(), 1.5f / (float)gkTextureManager::ms_SSAOTargetTmp->getHeight(), 1.5f / (float)gkTextureManager::ms_SSAOTargetTmp->getWidth(), 1.5f / (float)gkTextureManager::ms_SSAOTargetTmp->getHeight()));
		pShader->FX_Commit();

		UINT cPasses;
		pShader->FX_Begin(&cPasses, 0);
		for (UINT p = 0; p < cPasses; ++p)
		{
			pShader->FX_BeginPass(p);

			gkPostProcessManager::DrawFullScreenQuad(gkTextureManager::ms_SSAOTarget, Vec4(0, 0, 1, 1), Vec2(getRenderer()->GetScreenWidth() / 4.f, getRenderer()->GetScreenHeight() / 4.f));

			pShader->FX_EndPass();
		}
		pShader->FX_End();
	}
	else
	{
		gkRendererD3D9::_clearBuffer(false, 0xFFFFFFFF);
	}
	gkRendererD3D9::FX_PopRenderTarget(0);

	gkRendererD3D9::ms_GPUTimers[_T("SSAO")].stop();
	PROFILE_LABEL_POP("SSAO");



	//////////////////////////////////////////////////////////////////////////
	// 6. ShadowMaskGen

	PROFILE_LABEL_PUSH("SHADOWMASK_GEN");
	gkRendererD3D9::ms_GPUTimers[_T("ShadowMaskGen")].start();

	//STimeOfDayKey& tod = gkRendererD3D9::getShaderContent().getCurrTodKey();

	float todTime = tod.fKeyTime;
	if (todTime >= 6.0f && todTime <= 18.0f)
	{
		float fDepth = 0.0f;

		if (todTime >= 6.0f && todTime < 8.0f)
		{
			fDepth = powf(1.0f - (todTime - 6.0f) * 0.5f, 2.2f);
		}
		else if (todTime > 16.0f && todTime <= 18.0f)
		{
			fDepth = powf(1.0f - (18.0f - todTime) * 0.5f, 2.2f);
		}


		// check the size
		if (g_pRendererCVars->r_shadowmaskdownscale < 0.01f)
		{
			gkTextureManager::ms_ShadowMask->changeAttr(_T("size"), _T("full"));
		}
		else if (g_pRendererCVars->r_shadowmaskdownscale < 1.01f)
		{
			gkTextureManager::ms_ShadowMask->changeAttr(_T("size"), _T("half"));
		}
		else
		{
			gkTextureManager::ms_ShadowMask->changeAttr(_T("size"), _T("quad"));
		}

		gkRendererD3D9::FX_PushRenderTarget(0, gkTextureManager::ms_ShadowMask, 0, 0, true);

		gkRendererD3D9::_clearBuffer(false, 0xFFFFFFFF);
		gkRendererD3D9::getDevice()->Clear(0, NULL, D3DCLEAR_STENCIL, 0, 0, 0L);

		if (g_pRendererCVars->r_Shadow)
		{
			Matrix44 mViewToLightProj;
			Matrix44 mViewCascade[3];

			mViewToLightProj.SetIdentity();

			mViewToLightProj.Multiply(mViewToLightProj, gkRendererD3D9::getShaderContent().getViewMatrix_ShadowCascade(0));
			mViewToLightProj.Multiply(mViewToLightProj, gkRendererD3D9::getShaderContent().getProjectionMatrix_ShadowCascade(0));

			mViewCascade[0] = mViewToLightProj;

			mViewToLightProj.SetIdentity();
			mViewToLightProj.Multiply(mViewToLightProj, gkRendererD3D9::getShaderContent().getViewMatrix_ShadowCascade(1));
			mViewToLightProj.Multiply(mViewToLightProj, gkRendererD3D9::getShaderContent().getProjectionMatrix_ShadowCascade(1));

			mViewCascade[1] = mViewToLightProj;

			mViewToLightProj.SetIdentity();
			mViewToLightProj.Multiply(mViewToLightProj, gkRendererD3D9::getShaderContent().getViewMatrix_ShadowCascade(2));
			mViewToLightProj.Multiply(mViewToLightProj, gkRendererD3D9::getShaderContent().getProjectionMatrix_ShadowCascade(2));

			mViewCascade[2] = mViewToLightProj;

			// add a stencil write step, write the shadow cascade into 3 stencil mask [11/21/2011 Kaiming]

			gkRendererD3D9::RS_SetRenderState(D3DRS_ZENABLE, FALSE);
			gkRendererD3D9::RS_SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

			// open stencil test for writing
			gkRendererD3D9::RS_SetRenderState(D3DRS_STENCILENABLE, TRUE);

			// 0x0 mask the shadow cascade
			gkRendererD3D9::RS_SetRenderState(D3DRS_STENCILWRITEMASK, 0xFFFFFFFF);
			gkRendererD3D9::RS_SetRenderState(D3DRS_STENCILREF, 0x0);

			// 模板掩码设为0
			gkRendererD3D9::RS_SetRenderState(D3DRS_STENCILMASK, 0x0);

			// always success
			gkRendererD3D9::RS_SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);
			// if success or failed, both increase
			gkRendererD3D9::RS_SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_INCRSAT);
			gkRendererD3D9::RS_SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_INCRSAT);
			gkRendererD3D9::RS_SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);

			// first render cascade 3
			gkShaderPtr pShader = gkShaderManager::ms_ShadowStencilGen;
			GKHANDLE hTech = pShader->FX_GetTechniqueByName("ShadowStencilGen");
			pShader->FX_SetTechnique(hTech);
			// params set
			gkTextureManager::ms_SceneDepth->Apply(0, 0);
			gkTextureManager::ms_SceneNormal->Apply(1, 0);

			pShader->FX_SetValue("g_mViewToLightProj0", &(mViewCascade[2]), sizeof(Matrix44));
			pShader->FX_SetFloat3("g_mLightDir", gkRendererD3D9::getShaderContent().getShadowCamDir(0));

			UINT cPasses;
			pShader->FX_Begin(&cPasses, 0);
			for (UINT p = 0; p < cPasses; ++p)
			{
				pShader->FX_BeginPass(p);

				gkPostProcessManager::DrawFullScreenQuad(gkTextureManager::ms_ShadowMask);

				pShader->FX_EndPass();
			}
			pShader->FX_End();

			pShader->FX_SetValue("g_mViewToLightProj0", &(mViewCascade[1]), sizeof(Matrix44));
			pShader->FX_SetFloat3("g_mLightDir", gkRendererD3D9::getShaderContent().getShadowCamDir(0));
			pShader->FX_Begin(&cPasses, 0);
			for (UINT p = 0; p < cPasses; ++p)
			{
				pShader->FX_BeginPass(p);

				gkPostProcessManager::DrawFullScreenQuad(gkTextureManager::ms_ShadowMask);

				pShader->FX_EndPass();
			}
			pShader->FX_End();

			pShader->FX_SetValue("g_mViewToLightProj0", &(mViewCascade[0]), sizeof(Matrix44));
			pShader->FX_SetFloat3("g_mLightDir", gkRendererD3D9::getShaderContent().getShadowCamDir(0));
			pShader->FX_Begin(&cPasses, 0);
			for (UINT p = 0; p < cPasses; ++p)
			{
				pShader->FX_BeginPass(p);

				gkPostProcessManager::DrawFullScreenQuad(gkTextureManager::ms_ShadowMask);

				pShader->FX_EndPass();
			}
			pShader->FX_End();


			// shadow mask gen
			gkRendererD3D9::RS_SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
			gkRendererD3D9::RS_SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
			gkRendererD3D9::RS_SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);
			gkRendererD3D9::RS_SetRenderState(D3DRS_STENCILFUNC, D3DCMP_EQUAL);


			gkRendererD3D9::RS_SetRenderState(D3DRS_STENCILMASK, 0xFFFFFFFF);
			// selete tech
			pShader = gkShaderManager::ms_ShadowMaskGen;
			hTech = pShader->FX_GetTechniqueByName("SimpleShadowMaskGen");
			pShader->FX_SetTechnique(hTech);

			// params set
			gkTextureManager::ms_SceneDepth->Apply(0, 0);
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
			gkRendererD3D9::RS_SetRenderState(D3DRS_STENCILREF, 1);
			pShader->FX_SetValue("g_mViewToLightProj0", &(mViewCascade[2]), sizeof(Matrix44));
			pShader->FX_Commit();

			pShader->FX_Begin(&cPasses, 0);
			for (UINT p = 0; p < cPasses; ++p)
			{
				pShader->FX_BeginPass(p);

				gkPostProcessManager::DrawFullScreenQuad(gkTextureManager::ms_ShadowMask);

				pShader->FX_EndPass();
			}
			pShader->FX_End();


			// change to high perfomance shadow method
			hTech = pShader->FX_GetTechniqueByName("ShadowMaskGen");
			pShader->FX_SetTechnique(hTech);

			// DRAW cascade1 
			gkTextureManager::ms_ShadowCascade1->Apply(1, 0);
			gkRendererD3D9::RS_SetRenderState(D3DRS_STENCILREF, 2);
			pShader->FX_SetValue("g_mViewToLightProj0", &(mViewCascade[1]), sizeof(Matrix44));
			gsmparam0 = Vec4(g_pRendererCVars->r_GSMShadowConstbia * 3.0f, g_pRendererCVars->r_GSMShadowPenumbraStart * 0.5f, g_pRendererCVars->r_GSMShadowPenumbraLengh * 0.5f, 1.0f);
			pShader->FX_SetFloat4("gsmShadowParam0", gsmparam0);
			pShader->FX_Commit();


			pShader->FX_Begin(&cPasses, 0);
			for (UINT p = 0; p < cPasses; ++p)
			{
				pShader->FX_BeginPass(p);

				gkPostProcessManager::DrawFullScreenQuad(gkTextureManager::ms_ShadowMask);

				pShader->FX_EndPass();
			}
			pShader->FX_End();



			// DRAW cascade0
			gkTextureManager::ms_ShadowCascade0->Apply(1, 0);
			gkRendererD3D9::RS_SetRenderState(D3DRS_STENCILREF, 3);
			pShader->FX_SetValue("g_mViewToLightProj0", &(mViewCascade[0]), sizeof(Matrix44));
			gsmparam0 = Vec4(g_pRendererCVars->r_GSMShadowConstbia, g_pRendererCVars->r_GSMShadowPenumbraStart, g_pRendererCVars->r_GSMShadowPenumbraLengh, 1.0f);
			pShader->FX_SetFloat4("gsmShadowParam0", gsmparam0);
			pShader->FX_Commit();



			pShader->FX_Begin(&cPasses, 0);
			for (UINT p = 0; p < cPasses; ++p)
			{
				pShader->FX_BeginPass(p);

				gkPostProcessManager::DrawFullScreenQuad(gkTextureManager::ms_ShadowMask);

				pShader->FX_EndPass();
			}
			pShader->FX_End();




		}
		gkRendererD3D9::FX_PopRenderTarget(0);




	}
	else
	{
		gkRendererD3D9::FX_PushRenderTarget(0, gkTextureManager::ms_ShadowMask);

		gkRendererD3D9::_clearBuffer(false, 0x00000000);

		gkRendererD3D9::FX_PopRenderTarget(0);

	}


	// wether do it or not, here we close stencil
	gkRendererD3D9::RS_SetRenderState(D3DRS_STENCILENABLE, FALSE);



	// BLUR TRY
	//FX_TexBlurGaussian(gkTextureManager::ms_ShadowMask, 1, 1.0f, 1.0f, gkTextureManager::ms_ShadowMaskBlur, 1);


	gkRendererD3D9::ms_GPUTimers[_T("ShadowMaskGen")].stop();
	PROFILE_LABEL_POP("SHADOWMASK_GEN");
}

void gkRendPipe_OcculusionGen::End(gkRenderSequence* renderSeq)
{
	
}
