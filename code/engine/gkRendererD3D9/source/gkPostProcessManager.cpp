#include "RendererD3D9StableHeader.h"
#include "gkPostProcessManager.h"
#include "RenderRes\gkTexture.h"
#include "RenderRes\gkTextureManager.h"
#include "ICamera.h"
//#include "gkConsoleVariablesManager.h"

#include "gkFilePath.h"
#include "gk_Camera.h"
#include "gkRendererD3D9.h"
#include "ITimeOfDay.h"


#include "gkMemoryLeakDetecter.h"
#include "..\Profiler\gkGPUTimer.h"

// Vertex declaration for post-processing
const D3DVERTEXELEMENT9 PPVERT::Decl[4] =
{
	{ 0, 0,  D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITIONT, 0 },
	{ 0, 16, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,  0 },
	{ 0, 32, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,  1 },
	D3DDECL_END()
};

IDirect3DVertexDeclaration9*    gkPostProcessManager::m_pVertDeclPP = NULL;

HRESULT gkPostProcessManager::OnCreateDevice(IDirect3DDevice9* pd3dDevice)
{
	HRESULT hr;
	if( FAILED( hr = pd3dDevice->CreateVertexDeclaration( PPVERT::Decl, &m_pVertDeclPP ) ) )
	{
		return hr;
	}

	return S_OK;
}

//-----------------------------------------------------------------------
void gkPostProcessManager::OnDestroyDevice()
{
	SAFE_RELEASE( m_pVertDeclPP );
}

void gkPostProcessManager::DrawFullScreenQuadCubeSpec(int nTexWidth, int nTexHeight, uint8 level, uint8 index)
{
	float texWidth = (float)(nTexWidth >> level);
	float texHeight = (float)(nTexHeight >> level);

	// Render a screen-sized quad
	PPVERT quad[4];

	quad[0].post = Vec4(-0.5f, -0.5f, 1.0f, 1.0f);
	quad[1].post = Vec4(-0.5f, texHeight - 0.5f, 1.0f, 1.0f);
	quad[2].post = Vec4(texWidth - 0.5f, -0.5, 1.0f, 1.0f);
	quad[3].post = Vec4(texWidth - 0.5f,texHeight - 0.5f, 1.0f, 1.0f);

	// screenTC
	// cubemap space

	// 				y  x
	// 				| /
	// 			z___|/

	// just hack out, no reason

	switch (index)
	{
	case 0:
		quad[0].texcoord0 = Vec4(Vec3(1, 1, 1).GetNormalized(), 0.0f);
		quad[1].texcoord0 = Vec4(Vec3(1, -1, 1).GetNormalized(), 0.0f);
		quad[2].texcoord0 = Vec4(Vec3(1, 1, -1).GetNormalized(), 0.0f);
		quad[3].texcoord0 = Vec4(Vec3(1, -1, -1).GetNormalized(), 0.0f);
		break;
	case 1:
		quad[0].texcoord0 = Vec4(Vec3(-1, 1, -1).GetNormalized(), 0.0f);
		quad[1].texcoord0 = Vec4(Vec3(-1, -1, -1).GetNormalized(), 0.0f);
		quad[2].texcoord0 = Vec4(Vec3(-1, 1, 1).GetNormalized(), 0.0f);
		quad[3].texcoord0 = Vec4(Vec3(-1, -1, 1).GetNormalized(), 0.0f);
		break;
	case 2:
		quad[0].texcoord0 = Vec4(Vec3(1, 1, -1).GetNormalized(), 0.0f);
		quad[1].texcoord0 = Vec4(Vec3(1, 1, 1).GetNormalized(), 0.0f);
		quad[2].texcoord0 = Vec4(Vec3(-1, 1, -1).GetNormalized(), 0.0f);
		quad[3].texcoord0 = Vec4(Vec3(-1, 1, 1).GetNormalized(), 0.0f);
		break;
	case 3:
		quad[0].texcoord0 = Vec4(Vec3(-1, -1, 1).GetNormalized(), 0.0f);
		quad[1].texcoord0 = Vec4(Vec3(-1, -1, -1).GetNormalized(), 0.0f);
		quad[2].texcoord0 = Vec4(Vec3(1, -1, 1).GetNormalized(), 0.0f);
		quad[3].texcoord0 = Vec4(Vec3(1, -1, -1).GetNormalized(), 0.0f);
		break;
	case 4:
		quad[0].texcoord0 = Vec4(Vec3(-1, 1, 1).GetNormalized(), 0.0f);
		quad[1].texcoord0 = Vec4(Vec3(-1, -1, 1).GetNormalized(), 0.0f);
		quad[2].texcoord0 = Vec4(Vec3(1, 1, 1).GetNormalized(), 0.0f);
		quad[3].texcoord0 = Vec4(Vec3(1, -1, 1).GetNormalized(), 0.0f);
		break;
	case 5:
		quad[0].texcoord0 = Vec4(Vec3(1, 1, -1).GetNormalized(), 0.0f);
		quad[1].texcoord0 = Vec4(Vec3(1, -1, -1).GetNormalized(), 0.0f);
		quad[2].texcoord0 = Vec4(Vec3(-1, 1, -1).GetNormalized(), 0.0f);
		quad[3].texcoord0 = Vec4(Vec3(-1, -1, -1).GetNormalized(), 0.0f);
		break;
	}


	// screen FarPlane Verts
	quad[0].texcoord1 = getRenderer()->getShaderContent().getCamFarVerts(2);
	quad[1].texcoord1 = getRenderer()->getShaderContent().getCamFarVerts(3);
	quad[2].texcoord1 = getRenderer()->getShaderContent().getCamFarVerts(0);
	quad[3].texcoord1 = getRenderer()->getShaderContent().getCamFarVerts(1);


	getRenderer()->getDevice()->SetVertexDeclaration(m_pVertDeclPP);
	getRenderer()->getDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, quad, sizeof(PPVERT));
}

void gkPostProcessManager::DrawFullScreenQuad(int nTexWidth, int nTexHeight, Vec4& region, Vec2& repeat, uint8 level)
{
	float texWidth = (float)(nTexWidth >> level);
	float texHeight = (float)(nTexHeight >> level);

	// Render a screen-sized quad
	PPVERT quad[4];
	
	quad[0].post = Vec4(  -0.5f + region.x * texWidth,              -0.5f + region.y * texHeight,		1.0f, 1.0f );
	quad[1].post = Vec4(  -0.5f + region.x * texWidth,				region.w * texHeight - 0.5f,		1.0f, 1.0f );	
	quad[2].post = Vec4(   region.z * texWidth - 0.5f,				-0.5 + region.y * texHeight,		1.0f, 1.0f );	
	quad[3].post = Vec4(  region.z * texWidth - 0.5f,				region.w * texHeight - 0.5f,		1.0f, 1.0f );

	// screenTC
	quad[0].texcoord0 = Vec4(0.0f, 0.0f, 0.0f, 0.0f);
	quad[1].texcoord0 = Vec4(0.0f, 1.0f, 0.0f, repeat.y);
	quad[2].texcoord0 = Vec4(1.0f, 0.0f, repeat.x, 0.0f);
	quad[3].texcoord0 = Vec4(1.0f, 1.0f, repeat.x, repeat.y);

	// screen FarPlane Verts
	quad[0].texcoord1 = getRenderer()->getShaderContent().getCamFarVerts(2);
	quad[1].texcoord1 = getRenderer()->getShaderContent().getCamFarVerts(3);
	quad[2].texcoord1 = getRenderer()->getShaderContent().getCamFarVerts(0);
	quad[3].texcoord1 = getRenderer()->getShaderContent().getCamFarVerts(1);


	getRenderer()->getDevice()->SetVertexDeclaration( m_pVertDeclPP );
	getRenderer()->getDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, quad, sizeof(PPVERT) );
}

void gkPostProcessManager::DrawFullScreenQuad(gkTexturePtr targetTex, Vec4& region, Vec2& repeat, uint8 level)
{
	gkTexture* tgt = reinterpret_cast<gkTexture*>(targetTex.getPointer());

	DrawFullScreenQuad(targetTex->getWidth(), targetTex->getHeight(), region, repeat, level);
}

void gkPostProcessManager::DrawScreenQuad( Vec4& region )
{
	DrawFullScreenQuad( getRenderer()->GetScreenWidth(), getRenderer()->GetScreenHeight(), region);
}



