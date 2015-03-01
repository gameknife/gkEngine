#include "RendererD3D9StableHeader.h"
#include "RendPipe_ShadingPassDeferredLighting.h"


RendPipe_ShadingPassDeferredLighting::RendPipe_ShadingPassDeferredLighting(void)
{
}


RendPipe_ShadingPassDeferredLighting::~RendPipe_ShadingPassDeferredLighting(void)
{
}

void RendPipe_ShadingPassDeferredLighting::Prepare( gkRenderSequence* renderSeq )
{
	gkRendererD3D9::FX_PushRenderTarget(0, gkTextureManager::ms_HDRTarget0);

	gkRendererD3D9::RS_SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	gkRendererD3D9::RS_SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	gkRendererD3D9::RS_SetRenderState(D3DRS_ZENABLE, TRUE);






	gkRendererD3D9::getDevice()->Clear(0, NULL, D3DCLEAR_STENCIL, 0, 0, 0L);
	gkRendererD3D9::RS_SetRenderState(D3DRS_STENCILENABLE, TRUE);
	gkRendererD3D9::RS_SetRenderState(D3DRS_STENCILWRITEMASK, 1 << 7);
	gkRendererD3D9::RS_SetRenderState(D3DRS_STENCILREF, 0xffffffff);

	// always success
	gkRendererD3D9::RS_SetRenderState( D3DRS_STENCILFUNC,     D3DCMP_ALWAYS );
	gkRendererD3D9::RS_SetRenderState( D3DRS_STENCILPASS, D3DSTENCILOP_KEEP );
	gkRendererD3D9::RS_SetRenderState( D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP );
	gkRendererD3D9::RS_SetRenderState( D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP );

	float fDepthbia = -0.00001f;
	//gEnv->pRenderer->RS_SetRenderState(D3DRS_DEPTHBIAS, *(DWORD*)&fDepthbia );    

	gkRendererD3D9::_clearBuffer(false, 0xff7f7f7f);

	gkRendererD3D9::FX_ClearAllSampler();
}

void RendPipe_ShadingPassDeferredLighting::Execute( gkRenderSequence* renderSeq )
{
	// use ZFUNC_EQUAL to render opaque using Early-Z
	gkRendererD3D9::RS_SetRenderState(D3DRS_ZFUNC, D3DCMP_EQUAL);
	gkRendererD3D9::RS_SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	// Process Terrian Layer
	gkRendererD3D9::RP_ProcessRenderLayer(RENDER_LAYER_TERRIAN, eSIT_General, false);

	// Process Opaque Layer
	gkRendererD3D9::RP_ProcessRenderLayer(RENDER_LAYER_OPAQUE, eSIT_General, false);
	gkRendererD3D9::RP_ProcessRenderLayer(RENDER_LAYER_OPAQUE, eSIT_General, true);

	// restore to LessEqual, for sky and transp
	gkRendererD3D9::RS_SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);

	gkRendererD3D9::RP_ProcessRenderLayer(RENDER_LAYER_SKIES_EARLY, eSIT_General, false);

	

	// water, etc
	gkRendererD3D9::RS_SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	gkRendererD3D9::FX_StrechRect( gkTextureManager::ms_HDRTarget0, gkTextureManager::ms_ReflMap0 );

	gkRendererD3D9::RP_ProcessRenderLayer(RENDER_LAYER_WATER, eSIT_General, false);

	gkRendererD3D9::RS_SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	gkRendererD3D9::RS_SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	gkRendererD3D9::RS_SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	// transp
	gkRendererD3D9::RP_ProcessRenderLayer(RENDER_LAYER_TRANSPARENT, eSIT_General, false);
	gkRendererD3D9::RP_ProcessRenderLayer(RENDER_LAYER_TRANSPARENT, eSIT_General, true);
}

void RendPipe_ShadingPassDeferredLighting::End( gkRenderSequence* renderSeq )
{
	gkRendererD3D9::FX_PopRenderTarget(0);

	gkRendererD3D9::SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
}
