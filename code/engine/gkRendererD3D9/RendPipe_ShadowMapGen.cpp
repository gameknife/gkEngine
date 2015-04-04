#include "RendererD3D9StableHeader.h"
#include "RendPipe_ShadowMapGen.h"
#include "Profiler/gkGPUTimer.h"
#include "gkRenderSequence.h"
#include "RenderRes/gkTextureManager.h"

void gkRendPipe_ShadowMapGen::Prepare( gkRenderSequence* renderSeq )
{
	gkRendererD3D9::RS_SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE);
	gkRendererD3D9::RS_SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE);
	gkRendererD3D9::RS_SetRenderState( D3DRS_ALPHATESTENABLE, FALSE);
	gkRendererD3D9::RS_SetRenderState( D3DRS_ZENABLE, TRUE );
	gkRendererD3D9::RS_SetRenderState( D3DRS_ZWRITEENABLE, TRUE);
	gkRendererD3D9::RS_SetRenderState( D3DRS_COLORWRITEENABLE, 0x0 );
	float slopebia = g_pRendererCVars->r_shadowSlopeBias;
	gkRendererD3D9::RS_SetRenderState( D3DRS_SLOPESCALEDEPTHBIAS, F2DW( slopebia ));
	gkRendererD3D9::RS_SetRenderState( D3DRS_DEPTHBIAS, F2DW( 0 ));
}

void gkRendPipe_ShadowMapGen::Execute( gkRenderSequence* renderSeq )
{
	PROFILE_LABEL_PUSH("GEN_SHADOWMAP");
	gkRendererD3D9::ms_GPUTimers[_T("ShadowMapGen")].start();

	if (renderSeq->getCamera(eRFMode_ShadowCas0))
	{
		if( gkTextureManager::ms_ShadowCascade0->getWidth() != g_pRendererCVars->r_shadowmapsize )
		{
			TCHAR buffer[50];
			_stprintf(buffer, _T("%d"), (int)(g_pRendererCVars->r_shadowmapsize));
			gkTextureManager::ms_ShadowCascade0->changeAttr( _T("size"), buffer);
			gkTextureManager::ms_ShadowCascade1->changeAttr( _T("size"), buffer);
			gkTextureManager::ms_ShadowCascade2->changeAttr( _T("size"), buffer);

			gkTextureManager::ms_ShadowCascade_color->changeAttr( _T("size"), buffer);
		}

		// clear the shadow rt
		gkRendererD3D9::FX_ClearAllSampler();

		// set cascade 0

		gkRendererD3D9::FX_PushRenderTarget( 0, gkTextureManager::ms_ShadowCascade_color );
		gkRendererD3D9::FX_PushHwDepthTarget( gkTextureManager::ms_ShadowCascade0 );
		gkRendererD3D9::_clearBuffer(true, 0xffffffff);

		gkRendererD3D9::RP_ProcessShadowObjects(renderSeq->getShadowCascade0Objects(), 0, 0);
		gkRendererD3D9::RP_ProcessShadowObjects(renderSeq->getShadowCascade0ObjectsSkinned(), 0, 1);

		gkRendererD3D9::RP_ProcessRenderLayer(RENDER_LAYER_CUSTOM_SHADOW_CASCADE0, eSIT_ShadowPass, false, 0);
		gkRendererD3D9::RP_ProcessRenderLayer(RENDER_LAYER_CUSTOM_SHADOW_CASCADE0, eSIT_ShadowPass, true, 0);


		gkRendererD3D9::FX_PopHwDepthTarget();

		if( gEnv->pProfiler->getFrameCount() % GSM_SHADOWCASCADE1_DELAY == 0)
		{
			gkRendererD3D9::FX_PushHwDepthTarget( gkTextureManager::ms_ShadowCascade1 );
			gkRendererD3D9::_clearBuffer(true, 0xffffffff);

			gkRendererD3D9::RP_ProcessShadowObjects(renderSeq->getShadowCascade1Objects(), 1, 0);
			gkRendererD3D9::RP_ProcessShadowObjects(renderSeq->getShadowCascade1ObjectsSkinned(), 1, 1);

			gkRendererD3D9::RP_ProcessRenderLayer(RENDER_LAYER_CUSTOM_SHADOW_CASCADE1, eSIT_ShadowPass, false, 1);
			gkRendererD3D9::RP_ProcessRenderLayer(RENDER_LAYER_CUSTOM_SHADOW_CASCADE1, eSIT_ShadowPass, true, 1);

			gkRendererD3D9::FX_PopHwDepthTarget();
		}

		if (gEnv->pProfiler->getFrameCount() % GSM_SHADOWCASCADE2_DELAY == 0)
		{
			gkRendererD3D9::FX_PushHwDepthTarget( gkTextureManager::ms_ShadowCascade2 );
			gkRendererD3D9::_clearBuffer(true, 0xffffffff);

			gkRendererD3D9::RP_ProcessShadowObjects(renderSeq->getShadowCascade2Objects(), 2, 0);

			gkRendererD3D9::RP_ProcessRenderLayer(RENDER_LAYER_CUSTOM_SHADOW_CASCADE2, eSIT_ShadowPass, false, 2);
			gkRendererD3D9::RP_ProcessRenderLayer(RENDER_LAYER_CUSTOM_SHADOW_CASCADE2, eSIT_ShadowPass, true, 2);

			gkRendererD3D9::FX_PopHwDepthTarget();
		}

		gkRendererD3D9::FX_PopRenderTarget(0);
	}	
	
	gkRendererD3D9::ms_GPUTimers[_T("ShadowMapGen")].stop();
	PROFILE_LABEL_POP("GEN_SHADOWMAP");
}

void gkRendPipe_ShadowMapGen::End( gkRenderSequence* renderSeq )
{
	gkRendererD3D9::RS_SetRenderState( D3DRS_COLORWRITEENABLE, 15 );
	gkRendererD3D9::RS_SetRenderState( D3DRS_SLOPESCALEDEPTHBIAS, F2DW( 0 ));
}
