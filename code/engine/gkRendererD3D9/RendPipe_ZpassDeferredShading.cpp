#include "RendererD3D9StableHeader.h"
#include "RendPipe_ZpassDeferredShading.h"

void RendPipe_ZpassDeferredShading::Prepare( gkRenderSequence* renderSeq )
{
	// prepare z prepass MRT
	gkRendererD3D9::FX_PushRenderTarget(0, gkTextureManager::ms_SceneDepth,0,0, true);
	// Clear Normal Backbuffer & HW Depth Buffer
	gkRendererD3D9::_clearBuffer(true, 0x00FFFFFF);

	gkRendererD3D9::FX_PushRenderTarget(1, gkTextureManager::ms_SceneNormal);
	gkRendererD3D9::FX_PushRenderTarget(2, gkTextureManager::ms_SceneAlbeto);

	// WRITE STENCIL
	// open stencil test for writing
// 	gkRendererD3D9::RS_SetRenderState(D3DRS_STENCILENABLE, TRUE);
// 
// 	// 0x0 mask the shadow cascade
// 	gkRendererD3D9::RS_SetRenderState(D3DRS_STENCILWRITEMASK, 0xF0);
// 	gkRendererD3D9::RS_SetRenderState(D3DRS_STENCILREF, 0xF0);
// 
// 	// 模板掩码设为0
// 	gkRendererD3D9::RS_SetRenderState( D3DRS_STENCILMASK,     0x0 );
// 
// 	// always success
// 	gkRendererD3D9::RS_SetRenderState( D3DRS_STENCILFUNC,     D3DCMP_ALWAYS );
// 	// if success or failed, both increase
// 	gkRendererD3D9::RS_SetRenderState( D3DRS_STENCILPASS, D3DSTENCILOP_REPLACE );
// 	gkRendererD3D9::RS_SetRenderState( D3DRS_STENCILFAIL, D3DSTENCILOP_REPLACE );
// 	gkRendererD3D9::RS_SetRenderState( D3DRS_STENCILZFAIL, D3DSTENCILOP_REPLACE );

	gkRendererD3D9::RS_SetRenderState( D3DRS_CULLMODE, D3DCULL_CW);
	gkRendererD3D9::RS_SetRenderState( D3DRS_ZENABLE, TRUE );
	gkRendererD3D9::RS_SetRenderState( D3DRS_ZWRITEENABLE, TRUE);

	gkRendererD3D9::RS_SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
}

void RendPipe_ZpassDeferredShading::Execute( gkRenderSequence* renderSeq )
{
	gkRendererD3D9::FX_ClearAllSampler();
	// General Zpass Technique
	//gkRendererD3D9::RP_ProcessZpassObjects(renderSeq->getZprepassObjects(), gkShaderManager::ms_GeneralZpass.getPointer() , eSIT_Zpass_DS);
	//gkRendererD3D9::RP_ProcessZpassObjects(renderSeq->getZprepassObjectsSkinned(), gkShaderManager::ms_GeneralZpassSkinned.getPointer() , eSIT_Zpass_DS);

	// Custom Zpass Shader
	// Process Terrian Layer
	gkRendererD3D9::RP_ProcessRenderLayer(RENDER_LAYER_TERRIAN, eSIT_Zpass_DS, false);

	// Process Opaque Layer
	gkRendererD3D9::RP_ProcessRenderLayer(RENDER_LAYER_OPAQUE, eSIT_Zpass_DS, false);
	gkRendererD3D9::RP_ProcessRenderLayer(RENDER_LAYER_OPAQUE, eSIT_Zpass_DS, true);
}

void RendPipe_ZpassDeferredShading::End( gkRenderSequence* renderSeq )
{
	gkRendererD3D9::FX_PopRenderTarget(0);
	gkRendererD3D9::FX_PopRenderTarget(1);
	gkRendererD3D9::FX_PopRenderTarget(2);

// 	gkRendererD3D9::RS_SetRenderState( D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP );
// 	gkRendererD3D9::RS_SetRenderState( D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP );
// 	gkRendererD3D9::RS_SetRenderState( D3DRS_STENCILPASS, D3DSTENCILOP_KEEP );
// 
// 	gkRendererD3D9::RS_SetRenderState( D3DRS_STENCILFUNC, D3DCMP_EQUAL );
// 
// 
// 	gkRendererD3D9::RS_SetRenderState( D3DRS_STENCILMASK, 0xF0 );
// 	gkRendererD3D9::RS_SetRenderState( D3DRS_STENCILREF, 0XF0 );
}
