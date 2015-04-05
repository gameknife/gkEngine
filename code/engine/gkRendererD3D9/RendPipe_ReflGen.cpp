#include "RendererD3D9StableHeader.h"
#include "RendPipe_ReflGen.h"
#include "Profiler/gkGPUTimer.h"
#include "gkRenderSequence.h"
#include "RenderRes/gkTextureManager.h"

void gkRendPipe_ReflGen::Prepare( gkRenderSequence* renderSeq )
{
	gkRendererD3D9::FX_PushRenderTarget(0, gkTextureManager::ms_ReflMap0Tmp, 0, 0, true);
	gkRendererD3D9::_clearBuffer(true, 0xff469fe9);
}

void gkRendPipe_ReflGen::Execute( gkRenderSequence* renderSeq )
{
	PROFILE_LABEL_PUSH("GEN_REFLECTMAP");
	gkRendererD3D9::ms_GPUTimers[_T("ReflectGen")].start();


	gkRendererD3D9::RP_ProcessRenderLayer(RENDER_LAYER_OPAQUE, eSIT_ReflGenPass, false);
	gkRendererD3D9::RP_ProcessRenderLayer(RENDER_LAYER_OPAQUE, eSIT_ReflGenPass, true);

	gkRendererD3D9::RP_ProcessRenderLayer(RENDER_LAYER_TERRIAN, eSIT_ReflGenPass, false);
	gkRendererD3D9::RP_ProcessRenderLayer(RENDER_LAYER_SKIES_EARLY, eSIT_ReflGenPass, false);

	gkRendererD3D9::ms_GPUTimers[_T("ReflectGen")].stop();
	PROFILE_LABEL_POP("GEN_REFLECTMAP");
}

void gkRendPipe_ReflGen::End( gkRenderSequence* renderSeq )
{
	gkRendererD3D9::FX_PopRenderTarget(0);
}
