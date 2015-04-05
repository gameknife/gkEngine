#include "RendererD3D9StableHeader.h"
#include "RendPipe_DeferredFog.h"
#include "Profiler/gkGPUTimer.h"
#include "RenderRes/gkShader.h"
#include "RenderRes/gkMesh.h"
#include "gkPostProcessManager.h"

void RendPipe_DeferredFog::Prepare(gkRenderSequence* renderSeq)
{
	
}

void RendPipe_DeferredFog::Execute(gkRenderSequence* renderSeq)
{
	PROFILE_LABEL_PUSH("FOG_PROCESS");
	gkRendererD3D9::ms_GPUTimers[_T("Transparent")].start();

	gkRendererD3D9::FX_PushRenderTarget(0, gkTextureManager::ms_HDRTarget0);


	gkShaderPtr pShader = gkShaderManager::ms_HDRProcess;
	GKHANDLE hTech = pShader->FX_GetTechniqueByName("FogPass");
	pShader->FX_SetTechnique(hTech);



	//pShader->FX_SetValue( "g_mViewI", &(getShaderContent().getInverseViewMatrix()), sizeof(Matrix44) );
	//float amount = gEnv->p3DEngine->getSnowAmount();
	//pShader->FX_SetValue( "g_fSnowAmount", &(amount), sizeof(float));
	//pShader->FX_Commit();

	gkTextureManager::ms_SceneDepth->Apply(0, 0);

	UINT cPasses;
	pShader->FX_Begin(&cPasses, 0);
	for (UINT p = 0; p < cPasses; ++p)
	{
		pShader->FX_BeginPass(p);


		// 			Matrix44 matViewImulCampos =;
		// 			matViewImulCampos.SetTranslation(Vec3(0,0,0));
		pShader->FX_SetValue("g_mViewI", &(gkRendererD3D9::getShaderContent().getInverseViewMatrix()), sizeof(Matrix44));

		// set fog params [12/7/2011 Kaiming]
		const STimeOfDayKey& tod = gkRendererD3D9::getShaderContent().getCurrTodKey();

		ColorF fogColor = tod.clFogColor * tod.clFogColor.a;
		if (!g_pRendererCVars->r_HDRRendering)
		{
			//fogColor.srgb2rgb();
		}

		pShader->FX_SetValue("vfColGradBase", &fogColor, sizeof(ColorF));
		Vec4 fogParam(0.005, tod.clFogDensity, tod.clFogDensity, tod.clFogStart);
		pShader->FX_SetValue("vfParams", &fogParam, sizeof(Vec4));

		pShader->FX_Commit();

		gkPostProcessManager::DrawFullScreenQuad(gkTextureManager::ms_HDRTarget0);

		pShader->FX_EndPass();
	}
	pShader->FX_End();

	gkRendererD3D9::FX_PopRenderTarget(0);

	gkRendererD3D9::ms_GPUTimers[_T("Transparent")].stop();
	PROFILE_LABEL_POP("FOG_PROCESS");
}

void RendPipe_DeferredFog::End(gkRenderSequence* renderSeq)
{

}
