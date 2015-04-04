#include "RendererD3D9StableHeader.h"
#include "RendPipe_ShadingPassDeferredShading.h"
#include "Profiler/gkGPUTimer.h"


void RendPipe_ShadingPassDeferredShading::Prepare( gkRenderSequence* renderSeq )
{
	gkRendererD3D9::FX_PushRenderTarget(0, gkTextureManager::ms_HDRTarget0);
	gkRendererD3D9::FX_ClearAllSampler();
	gkRendererD3D9::RS_SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
}

void RendPipe_ShadingPassDeferredShading::Execute( gkRenderSequence* renderSeq )
{
	PROFILE_LABEL_PUSH("SHADING OPAQUE");
	gkRendererD3D9::ms_GPUTimers[_T("Opaque")].start();

	gkShaderPtr pShader = gkShaderManager::ms_DeferredShading;

	GKHANDLE hTech = pShader->FX_GetTechniqueByName("GeneralDeferredShading");
	pShader->FX_SetTechnique( hTech );


	gkTextureManager::ms_SceneAlbeto->Apply(0, 0);
	gkTextureManager::ms_SceneDifAcc->Apply(1, 0);
	gkTextureManager::ms_SceneSpecAcc->Apply(2, 0);

	{
		UINT cPasses;
		pShader->FX_Begin( &cPasses, 0 );
		for( UINT p = 0; p < cPasses; ++p )
		{
			pShader->FX_BeginPass( p );

			getRenderer()->FX_DrawFullScreenQuad(gkTextureManager::ms_HDRTarget0);

			pShader->FX_EndPass();
		}
		pShader->FX_End();
	}



	// restore to LessEqual, for sky and transp
	
	gkRendererD3D9::RS_SetRenderState(D3DRS_ZENABLE, TRUE);
	gkRendererD3D9::RS_SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);

	gkRendererD3D9::RP_ProcessRenderLayer(RENDER_LAYER_SKIES_EARLY, eSIT_General, false);

	gkRendererD3D9::RS_SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	gkRendererD3D9::ms_GPUTimers[_T("Opaque")].stop();
	PROFILE_LABEL_POP("SHADING OPAQUE");
}

void RendPipe_ShadingPassDeferredShading::End( gkRenderSequence* renderSeq )
{
	gkRendererD3D9::FX_PopRenderTarget(0);

	gkRendererD3D9::SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
}
