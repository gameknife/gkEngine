#include "RendererD3D9StableHeader.h"
#include "RendPipe_SSRL.h"
#include "Profiler/gkGPUTimer.h"
#include "RenderRes/gkShader.h"
#include "RenderRes/gkMesh.h"
#include "gkPostProcessManager.h"



void RendPipe_SSRL::Prepare(gkRenderSequence* renderSeq)
{
	
}

void RendPipe_SSRL::Execute(gkRenderSequence* renderSeq)
{
	gkRendererD3D9::RS_SetRenderState(D3DRS_STENCILENABLE, TRUE);
	gkRendererD3D9::RS_SetRenderState(D3DRS_STENCILMASK, 1 << 7);
	gkRendererD3D9::RS_SetRenderState(D3DRS_STENCILREF, 0xffffffff);

	// always success
	gkRendererD3D9::RS_SetRenderState(D3DRS_STENCILFUNC, D3DCMP_EQUAL);
	gkRendererD3D9::RS_SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);
	gkRendererD3D9::RS_SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
	gkRendererD3D9::RS_SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);

	gkRendererD3D9::FX_PushRenderTarget(0, gkTextureManager::ms_ReflMap0);

	gkRendererD3D9::_clearBuffer(false, D3DCOLOR_ARGB(0, 0, 0, 0));

	static bool first = true;

	gkShaderPtr pShader = gkShaderManager::ms_SSRL;
	GKHANDLE hTech = pShader->FX_GetTechniqueByName("SSRL");
	pShader->FX_SetTechnique(hTech);

	gkTextureManager::ms_SceneNormal->Apply(0, 0);
	gkTextureManager::ms_SceneDepth->Apply(1, 0);
	gkTextureManager::ms_HDRTarget0->Apply(2, 0);

	// PASS THE FAR_CLIP_DELTA
	Vec4 ddx = gkRendererD3D9::getShaderContent().getCamFarVerts(0) - gkRendererD3D9::getShaderContent().getCamFarVerts(2);
	Vec4 ddy = gkRendererD3D9::getShaderContent().getCamFarVerts(3) - gkRendererD3D9::getShaderContent().getCamFarVerts(2);

	pShader->FX_SetFloat4("PS_lefttop", gkRendererD3D9::getShaderContent().getCamFarVerts(2));
	pShader->FX_SetFloat4("PS_ddx", ddx);
	pShader->FX_SetFloat4("PS_ddy", ddy);
	pShader->FX_SetFloat4("g_camPos", gkRendererD3D9::getShaderContent().getCamPos());

	Matrix44 matForRender = gkRendererD3D9::getShaderContent().getViewMatrix() * gkRendererD3D9::getShaderContent().getProjectionMatrix();

	pShader->FX_SetValue("g_mProjection", &(matForRender), sizeof(Matrix44));
	pShader->FX_Commit();

	UINT cPasses;
	pShader->FX_Begin(&cPasses, 0);
	for (UINT p = 0; p < cPasses; ++p)
	{
		pShader->FX_BeginPass(p);

		gkPostProcessManager::DrawFullScreenQuad(gkTextureManager::ms_ReflMap0);

		pShader->FX_EndPass();
	}
	pShader->FX_End();

	gkRendererD3D9::FX_PopRenderTarget(0);

	//////////////////////////////////////////////////////////////////////////
	// change the autogenmipmap method [4/1/2015 gameKnife]
	//gkTextureManager::ms_ReflMap0->AutoGenMipmap();

	// to gaussin blur, but blur the mipmap chain

	gkRendererD3D9::SetRenderState(D3DRS_STENCILENABLE, FALSE);

	gkRendererD3D9::FX_TexBlurGaussian(gkTextureManager::ms_ReflMap0, 1.0, 1.0, 5.0, gkTextureManager::ms_ReflMap0Tmp, 2, true);
}

void RendPipe_SSRL::End(gkRenderSequence* renderSeq)
{
	
}
