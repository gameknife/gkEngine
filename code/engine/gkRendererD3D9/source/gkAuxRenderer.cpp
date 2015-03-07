#include "RendererD3D9StableHeader.h"
#include "gkAuxRenderer.h"
#include "ICamera.h"
#include "RenderRes\gkMesh.h"
#include "gkRendererD3D9.h"
#include "RenderRes\gkShaderManager.h"
#include "IGameObjectLayer.h"
#include "gkRenderable.h"
#include "RenderRes\gkTextureManager.h"


//REFACTOR FIX

template<> gkAuxRenderer* Singleton<gkAuxRenderer>::ms_Singleton = 0;

gkAuxRenderer* gkAuxRenderer::getSingletonPtr(void)
{
	return ms_Singleton;
}

gkAuxRenderer& gkAuxRenderer::getSingleton( void )
{
	assert( ms_Singleton );  
	return ( *ms_Singleton );  
}

gkAuxRenderer::gkAuxRenderer( void )
{
	m_vecLineVertexBuffer.clear();
	m_vecSolidVertexBuffer.clear();
	m_vecOverHudLineVertexBuffer.clear();
	m_vecOverHudSolidVertexBuffer.clear();
	SetDrawColor(ColorF(1, 1, 1, 1));
}

gkAuxRenderer::~gkAuxRenderer( void )
{
	
}
//-----------------------------------------------------------------------
void gkAuxRenderer::SetDrawColor( ColorF& color )
{
	m_curColor = (DWORD)color.pack_argb8888();
}
//-----------------------------------------------------------------------
void gkAuxRenderer::AuxRender3DLine( const Vec3& from, const Vec3& to, ColorF& color, bool ignoreZ)
{
	SetDrawColor(color);
	GK_HELPER_LINEVERTEX vertexFrom(from, m_curColor);
	GK_HELPER_LINEVERTEX vertexTo(to, m_curColor);
	if (ignoreZ)
	{
		m_vecOverHudLineVertexBuffer.push_back(vertexFrom);
		m_vecOverHudLineVertexBuffer.push_back(vertexTo);
	}
	else
	{
		m_vecLineVertexBuffer.push_back(vertexFrom);
		m_vecLineVertexBuffer.push_back(vertexTo);
	}

}

void gkAuxRenderer::AuxRenderSkeleton( const Vec3& from, const Vec3& to, ColorF& color /*= ColorF(1.0,1.0,1.0,1.0)*/, float radius /*= 0.05f*/, bool ignoreZ /*= false */ )
{
	Vec3 dir = to - from;
	Vec3 dirInPlane = Vec3::CreateProjection(Vec3(0,0,1), dir.GetNormalized());
	if (dirInPlane.IsEquivalent(Vec3(0,0,0)))
	{
		dirInPlane = Vec3::CreateProjection(Vec3(1,0,0), dir.GetNormalized());
	}

	float len = dir.GetLength();
	dirInPlane.Normalize();
	dirInPlane *= radius * len;

	Vec3 dirInPlane1 = dirInPlane.GetRotated(dir.GetNormalized(), DEG2RAD(120.0f));
	Vec3 dirInPlane2 = dirInPlane.GetRotated(dir.GetNormalized(), DEG2RAD(-120.0f));

	Vec3 jointPt = from + dir.GetNormalized() * len * 0.8f;

	AuxRender3DLine(from, jointPt + dirInPlane, color, true);
	AuxRender3DLine(from, jointPt + dirInPlane1, color, true);
	AuxRender3DLine(from, jointPt + dirInPlane2, color, true);

	AuxRender3DLine(to, jointPt + dirInPlane, ColorF(1,0,0,1), true);
	AuxRender3DLine(to, jointPt + dirInPlane1, ColorF(1,0,0,1), true);
	AuxRender3DLine(to, jointPt + dirInPlane2, ColorF(1,0,0,1), true);

	AuxRender3DLine(jointPt + dirInPlane, jointPt + dirInPlane2, ColorF(1,0,0,1), true);
	AuxRender3DLine(jointPt + dirInPlane1, jointPt + dirInPlane2, ColorF(1,0,0,1), true);
	AuxRender3DLine(jointPt + dirInPlane, jointPt + dirInPlane1, ColorF(1,0,0,1), true);



}


//-----------------------------------------------------------------------
void gkAuxRenderer::AuxRender3DGird( const Vec3& center, int row, float gap, ColorF& color, bool ignoreZ )
{
	if (row % 2 != 0)
	{
		row++;
	}
	int half = row / 2;
	for( int i=0; i < row - 1; i++ )
	{
		Vec3 from = center - Vec3( (half - i - 1) * gap, (half - row + 1) * gap, 0);
		Vec3 to = center - Vec3( (half - i - 1) * gap, (half - 1) * gap, 0);

		Vec3 from1 = center - Vec3( (half - row + 1) * gap, (half - i - 1) * gap, 0);
		Vec3 to1 = center - Vec3( (half - 1) * gap, (half - i - 1) * gap, 0);

		AuxRender3DLine(from, to, color, ignoreZ );
		AuxRender3DLine(from1, to1, color, ignoreZ );
	}
}

//-----------------------------------------------------------------------
void gkAuxRenderer::AuxRender3DCircle( const Vec3& center, const Vec3& normal, float radius, uint32 side, ColorF& color /*= ColorF(1.0,1.0,1.0,1.0)*/, bool ignoreZ /*= false*/ )
{
	//
	//   1. create a circle and 0,0,0, normal = z
// 	Vec3* circlePoints = new Vec3[side];
// 	memset( circlePoints, 0 sizeof(circlePoints) );
// 
// 	for (int i=0; i < side; ++i)
// 	{
// 		circlePoints[i].x = 
// 	}
}

void gkAuxRenderer::AuxRender3DBoxFrameRotated( const Vec3& center, const Quat& rot, float length, ColorF& color /*= ColorF(1.0,1.0,1.0,1.0)*/, bool ignoreZ /*= false*/ )
{
	AuxRender3DBoxFrameRotated(center , rot, Vec3(length, length, length), color, ignoreZ);
}

void gkAuxRenderer::AuxRender3DBoxFrameRotated( const Vec3& center, const Quat& rot, const Vec3& size, ColorF& color /*= ColorF(1.0,1.0,1.0,1.0)*/, bool ignoreZ /*= false*/ )
{
	// TODO
	//SetDrawColor(color);

	//       4_____6
	// 	    /    /|
	//    0/____/2|
	//     |    | |7
	//    1|____|/
	// 	        3
	// 	

	Vec3 p0 = Vec3(-size.x, size.y, -size.z) * 0.5f;
	Vec3 p1 = Vec3(-size.x, -size.y, -size.z) * 0.5f;
	Vec3 p2 = Vec3(size.x, size.y, -size.z) * 0.5f;
	Vec3 p3 = Vec3(size.x, -size.y, -size.z) * 0.5f;
	Vec3 p4 = Vec3(-size.x, size.y, size.z) * 0.5f;
	Vec3 p5 = Vec3(-size.x, -size.y, size.z) * 0.5f;
	Vec3 p6 = Vec3(size.x, size.y, size.z) * 0.5f;
	Vec3 p7 = Vec3(size.x, -size.y, size.z) * 0.5f;

	p0 = rot * p0 + center;
	p1 = rot * p1 + center;
	p2 = rot * p2 + center;
	p3 = rot * p3 + center;
	p4 = rot * p4 + center;
	p5 = rot * p5 + center;
	p6 = rot * p6 + center;
	p7 = rot * p7 + center;

	AuxRender3DLine(p0, p1, color, ignoreZ );
	AuxRender3DLine(p0, p2, color, ignoreZ );
	AuxRender3DLine(p0, p4, color, ignoreZ );

	AuxRender3DLine(p3, p1, color, ignoreZ );
	AuxRender3DLine(p3, p2, color, ignoreZ );
	AuxRender3DLine(p3, p7, color, ignoreZ );

	AuxRender3DLine(p6, p4, color, ignoreZ );
	AuxRender3DLine(p6, p2, color, ignoreZ );
	AuxRender3DLine(p6, p7, color, ignoreZ );

	AuxRender3DLine(p5, p4, color, ignoreZ );
	AuxRender3DLine(p5, p1, color, ignoreZ );
	AuxRender3DLine(p5, p7, color, ignoreZ );
}

//-----------------------------------------------------------------------
void gkAuxRenderer::AuxRender3DBoxFrame( const Vec3& center, float length ,ColorF& color, bool ignoreZ )
{
	AuxRender3DBoxFrame(center, Vec3(length, length, length), color, ignoreZ);
}
//-----------------------------------------------------------------------
void gkAuxRenderer::AuxRender3DBoxFrame( const Vec3& center, Vec3& size, ColorF& color /*= ColorF(1.0,1.0,1.0,1.0)*/, bool ignoreZ )
{
	// TODO
	//SetDrawColor(color);

	//       4_____6
	// 	    /    /|
	//    0/____/2|
	//     |    | |7
	//    1|____|/
	// 	        3
	// 	

	Vec3 p0 = center + Vec3(-size.x, size.y, -size.z) * 0.5f;
	Vec3 p1 = center + Vec3(-size.x, -size.y, -size.z) * 0.5f;
	Vec3 p2 = center + Vec3(size.x, size.y, -size.z) * 0.5f;
	Vec3 p3 = center + Vec3(size.x, -size.y, -size.z) * 0.5f;
	Vec3 p4 = center + Vec3(-size.x, size.y, size.z) * 0.5f;
	Vec3 p5 = center + Vec3(-size.x, -size.y, size.z) * 0.5f;
	Vec3 p6 = center + Vec3(size.x, size.y, size.z) * 0.5f;
	Vec3 p7= center + Vec3(size.x, -size.y, size.z) * 0.5f;

	AuxRender3DLine(p0, p1, color, ignoreZ );
	AuxRender3DLine(p0, p2, color, ignoreZ );
	AuxRender3DLine(p0, p4, color, ignoreZ );

	AuxRender3DLine(p3, p1, color, ignoreZ );
	AuxRender3DLine(p3, p2, color, ignoreZ );
	AuxRender3DLine(p3, p7, color, ignoreZ );

	AuxRender3DLine(p6, p4, color, ignoreZ );
	AuxRender3DLine(p6, p2, color, ignoreZ );
	AuxRender3DLine(p6, p7, color, ignoreZ );

	AuxRender3DLine(p5, p4, color, ignoreZ );
	AuxRender3DLine(p5, p1, color, ignoreZ );
	AuxRender3DLine(p5, p7, color, ignoreZ );
}

//-----------------------------------------------------------------------
void gkAuxRenderer::AuxRenderGizmo(const Matrix44& xform, float size, uint8 axisHighlight, bool ignoreZ, uint8 type)
{
	Vec3 vCenter = xform.GetRow(3);
	Vec3 vXaxis = xform.GetRow(0).GetNormalized();
	Vec3 vYaxis = xform.GetRow(1).GetNormalized();
	Vec3 vZaxis = xform.GetRow(2).GetNormalized();

	AuxRender3DLine( vCenter, vCenter + vXaxis * size, axisHighlight == 1 ? ColorF(1,1,0,1) : ColorF(1,0,0,1), ignoreZ);
	if (type < 2)
		AuxRenderPyramid( vCenter + vXaxis * size, vXaxis, size * 0.1f, axisHighlight == 1 ? ColorF(1,1,0,1) : ColorF(1,0,0,1), ignoreZ);
	else
		AuxRender3DBoxSolid( vCenter + vXaxis * size, size * 0.2f, axisHighlight == 1 ? ColorF(1,1,0,1) : ColorF(1,0,0,1), ignoreZ);

	AuxRender3DLine( vCenter, vCenter + vYaxis * size, axisHighlight == 2 ? ColorF(1,1,0,1) : ColorF(0,1,0,1), ignoreZ);
	if (type < 2)
		AuxRenderPyramid( vCenter + vYaxis * size, vYaxis, size * 0.1f, axisHighlight == 2 ? ColorF(1,1,0,1) : ColorF(0,1,0,1), ignoreZ);
	else
		AuxRender3DBoxSolid( vCenter+ vYaxis * size, size * 0.2f, axisHighlight == 1 ? ColorF(1,1,0,1) : ColorF(0,1,0,1), ignoreZ);

	AuxRender3DLine( vCenter, vCenter + vZaxis * size, axisHighlight == 3 ? ColorF(1,1,0,1) : ColorF(0,0,1,1), ignoreZ);
	if (type < 2)
		AuxRenderPyramid( vCenter + vZaxis * size, vZaxis, size * 0.1f, axisHighlight == 3 ? ColorF(1,1,0,1) : ColorF(0,0,1,1), ignoreZ);
	else
		AuxRender3DBoxSolid( vCenter + vZaxis * size, size * 0.2f, axisHighlight == 1 ? ColorF(1,1,0,1) :  ColorF(0,0,1,1), ignoreZ);
}
//-----------------------------------------------------------------------
void gkAuxRenderer::_FlushAllHelper( LPDIRECT3DDEVICE9 pDevice, bool clean )
{
	gkShaderPtr pShader = gkShaderManager::ms_AuxRenderer;

	Matrix44 viewMat = gkRendererD3D9::getShaderContent().getViewMatrix();
	Matrix44 projMat = gkRendererD3D9::getShaderContent().getProjectionMatrix();

	getRenderer()->SetRenderState(D3DRS_ZENABLE, TRUE);
	getRenderer()->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	getRenderer()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	getRenderer()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	getRenderer()->getDevice()->SetVertexDeclaration( gkRendererD3D9::m_generalDeclP4F4 );

	//pDevice->SetRenderState(D3D)

	// Bloommerge
	GKHANDLE hTech = pShader->FX_GetTechniqueByName("RenderLine");
	pShader->FX_SetTechnique( hTech );
	{
		pShader->FX_SetValue( "g_mWorldViewProj", &(viewMat * projMat), sizeof(Matrix44) );
		pShader->FX_Commit();

		UINT cPasses;
		pShader->FX_Begin( &cPasses, 0 );
		for( UINT p = 0; p < cPasses; ++p )
		{
			pShader->FX_BeginPass( p );

			// list array
			// render直接渲染点列
			if (!m_vecLineVertexBuffer_Render.empty())
			{
				int size = m_vecLineVertexBuffer_Render.size();
				pDevice->DrawPrimitiveUP(D3DPT_LINELIST, m_vecLineVertexBuffer_Render.size() / 2, &(m_vecLineVertexBuffer_Render[0]), sizeof(GK_HELPER_LINEVERTEX));
			}
			
			if (!m_vecSolidVertexBuffer_Render.empty())
			{
				int size = m_vecSolidVertexBuffer_Render.size();
				pDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, m_vecSolidVertexBuffer_Render.size() / 3, &(m_vecSolidVertexBuffer_Render[0]), sizeof(GK_HELPER_LINEVERTEX));
			}
			


			// off depth
			getRenderer()->SetRenderState(D3DRS_ZENABLE, FALSE);
			if (!m_vecOverHudLineVertexBuffer_Render.empty())
			{
				int size = m_vecOverHudLineVertexBuffer_Render.size();
				pDevice->DrawPrimitiveUP(D3DPT_LINELIST, m_vecOverHudLineVertexBuffer_Render.size() / 2, &(m_vecOverHudLineVertexBuffer_Render[0]), sizeof(GK_HELPER_LINEVERTEX));
			}
			

			if (!m_vecOverHudSolidVertexBuffer_Render.empty())
			{
				int size = m_vecOverHudSolidVertexBuffer_Render.size();
				pDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, m_vecOverHudSolidVertexBuffer_Render.size() / 3, &(m_vecOverHudSolidVertexBuffer_Render[0]), sizeof(GK_HELPER_LINEVERTEX));
			}
			

			if (clean)
			{
				m_vecLineVertexBuffer_Render.clear();
				m_vecSolidVertexBuffer_Render.clear();
				m_vecOverHudLineVertexBuffer_Render.clear();
				m_vecOverHudSolidVertexBuffer_Render.clear();
			}

			pShader->FX_EndPass();
		}
		pShader->FX_End();
	}
	
	// on depth
	getRenderer()->SetRenderState(D3DRS_ZENABLE, TRUE);
	getRenderer()->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	// Bloommerge
	hTech = pShader->FX_GetTechniqueByName("RenderFrame");
	pShader->FX_SetTechnique( hTech );
	{
		UINT cPasses;
		pShader->FX_Begin( &cPasses, 0 );
		for( UINT p = 0; p < cPasses; ++p )
		{
			pShader->FX_BeginPass( p );


			// 画线框
			for (uint32 i=0; i < m_vecMeshFrameBuffer_Render.size(); ++i)
			{
				uint32 subRenderableCount = m_vecMeshFrameBuffer_Render[i]->getSubRenderableCount();
				for (uint32 r=0; r < subRenderableCount; ++r)
				{
					gkRenderable* renderable = m_vecMeshFrameBuffer_Render[i]->getSubRenderable(r);

					uint32 uSize = 0;
					if (renderable && !renderable->getSkinnedMatrix( 0, uSize ))
					{
						gkRenderOperation op;
						renderable->getRenderOperation( op );

						Matrix44 world;
						renderable->getWorldTransforms( &world );

						pShader->FX_SetFloat4( "g_auxcolor", Vec4(0,0,0,0.1f) );
						pShader->FX_SetValue( "g_mWorldViewProj", &( world * viewMat * projMat), sizeof(Matrix44) );
						pShader->FX_Commit();

						getRenderer()->_render(op);
					}
				}

			}
			

			
			if (clean)
			{
				m_vecMeshFrameBuffer_Render.clear();
			}

			pShader->FX_EndPass();
		}
		pShader->FX_End();
	}


	

	getRenderer()->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

	getRenderer()->SetRenderState(D3DRS_ZENABLE, FALSE);
	//getRenderer()->SetRenderState( D3DRS_LIGHTING, FALSE );
	getRenderer()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	getRenderer()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	getRenderer()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	getRenderer()->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
}


void gkAuxRenderer::_FlushAllHelperDeferred( LPDIRECT3DDEVICE9 pDevice, bool clean )
{

	gkShaderPtr pShader = gkShaderManager::ms_AuxRenderer;

	Matrix44 viewMat = gkRendererD3D9::getShaderContent().getViewMatrix();
	Matrix44 projMat = gkRendererD3D9::getShaderContent().getProjectionMatrix();

	getRenderer()->SetRenderState(D3DRS_ZENABLE, FALSE);
	getRenderer()->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	getRenderer()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	getRenderer()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	getRenderer()->getDevice()->SetVertexDeclaration( gkRendererD3D9::m_generalDeclPt4F4 );

	// Bloommerge
	GKHANDLE hTech = pShader->FX_GetTechniqueByName("Render2D");
	pShader->FX_SetTechnique( hTech );
	{
		UINT cPasses;
		pShader->FX_Begin( &cPasses, 0 );
		for( UINT p = 0; p < cPasses; ++p )
		{
			pShader->FX_BeginPass( p );

			

			if (!m_vecScreenBoxVertexBuffer_Render.empty())
			{
				int size = m_vecScreenBoxVertexBuffer_Render.size();
				pDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, m_vecScreenBoxVertexBuffer_Render.size() / 3, &(m_vecScreenBoxVertexBuffer_Render[0]), sizeof(GK_HELPER_2DVERTEX));
			}

			if (!m_vecScreenLineVertexBuffer_Render.empty())
			{
				int size = m_vecScreenLineVertexBuffer_Render.size();
				pDevice->DrawPrimitiveUP(D3DPT_LINELIST, m_vecScreenLineVertexBuffer_Render.size() / 2, &(m_vecScreenLineVertexBuffer_Render[0]), sizeof(GK_HELPER_2DVERTEX));
			}

			if (clean)
			{
				m_vecScreenBoxVertexBuffer_Render.clear();
				m_vecScreenLineVertexBuffer_Render.clear();
			}


			pShader->FX_EndPass();
		}
		pShader->FX_End();
	}

	//getRenderer()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	//getRenderer()->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);


	

	GKHANDLE hTechT = pShader->FX_GetTechniqueByName("Render2DTexture");
	GKHANDLE hTechA = pShader->FX_GetTechniqueByName("Render2DTextureAlpha");
	GKHANDLE hTechD = pShader->FX_GetTechniqueByName("Render2DTextureDirect");
	

	getRenderer()->SetRenderState(D3DRS_ZENABLE, FALSE);
	if (!m_vecScreenBoxTexturedVertexBuffer_Render.empty())
	{
		int count = m_vecScreenBoxTexturedVertexBuffer_Render.size() / 6;
		//pDevice->SetFVF( D3DFVF_XYZRHW | D3DFVF_TEX1 );

		for (int i=0; i < count; ++i)
		{

			ITexture* tex = m_vecScreenBoxTexture_Render[i];

			if (tex->format() == eTF_A8)
			{
				pShader->FX_SetTechnique( hTechA );
			}
			else if(!tex->dynamic())
			{
				pShader->FX_SetTechnique( hTechT );
			}
			else
			{
				pShader->FX_SetTechnique( hTechD );
			}

			
			{
				UINT cPasses;
				pShader->FX_Begin( &cPasses, 0 );
				for( UINT p = 0; p < cPasses; ++p )
				{
					pShader->FX_BeginPass( p );

					getRenderer()->getDevice()->SetVertexDeclaration( gkRendererD3D9::m_generalDeclPt4F2 );


					getRenderer()->getDevice()->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
					getRenderer()->getDevice()->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
					tex->Apply(0,0);

					pDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 2, &(m_vecScreenBoxTexturedVertexBuffer_Render[i * 6]), sizeof(GK_HELPER_2DVERTEX_TEXTURED));

					//pDevice->SetTexture(0, NULL);

					pShader->FX_EndPass();
				}
				pShader->FX_End();
			}

		}
	}

	if (clean)
	{
		m_vecScreenBoxTexture_Render.clear();
		m_vecScreenBoxTexturedVertexBuffer_Render.clear();
	}


	getRenderer()->SetRenderState(D3DRS_ZENABLE, FALSE);
	getRenderer()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	getRenderer()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	getRenderer()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	getRenderer()->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
}


//-----------------------------------------------------------------------
void gkAuxRenderer::AuxRenderAABB( const AABB& aabb, ColorF& color /*= ColorF(1.0,1.0,1.0,1.0)*/, bool ignoreZ )
{

	//Vec3 vecAABB = aabb.GetCenter();
	AuxRender3DBoxFrame( aabb.GetCenter(), aabb.GetSize(), color, ignoreZ );
}
//-----------------------------------------------------------------------
void gkAuxRenderer::AuxRenderPyramid( const Vec3& center, const Vec3& dir, const float radius, ColorF& color /*= ColorF(1.0,1.0,1.0,1.0)*/ , bool ignoreZ)
{
	SetDrawColor(color);

	// sturcture

	//      0
	//     /|\
	//    / | \
	//   / 4|  \
    //  /__ |_-`3
	// 1    2


	Vec3 crosser = Vec3(0.5f,0.5f,0.5f);
	Vec3 right = dir % crosser; // -1
	right.normalize();
	Vec3 down = dir % right; // -2
	down.normalize();

	GK_HELPER_LINEVERTEX point[5];
	point[0].m_vPosition  = center + dir * radius * 1.5f;
	point[1].m_vPosition = center + right * radius * 0.5f;
	point[2].m_vPosition = center + down * radius * 0.5f;
	point[3].m_vPosition = center - right * radius * 0.5f;
	point[4].m_vPosition = center - down * radius * 0.5f;

	point[0].m_dwDiffuse = m_curColor;
	point[1].m_dwDiffuse = m_curColor;
	point[2].m_dwDiffuse = m_curColor;
	point[3].m_dwDiffuse = m_curColor;
	point[4].m_dwDiffuse = m_curColor;


	if (ignoreZ)
	{
		// 填6个面
		// face#1
		m_vecOverHudSolidVertexBuffer.push_back(point[0]);
		m_vecOverHudSolidVertexBuffer.push_back(point[1]);
		m_vecOverHudSolidVertexBuffer.push_back(point[2]);

		m_vecOverHudSolidVertexBuffer.push_back(point[0]);
		m_vecOverHudSolidVertexBuffer.push_back(point[2]);
		m_vecOverHudSolidVertexBuffer.push_back(point[3]);

		m_vecOverHudSolidVertexBuffer.push_back(point[0]);
		m_vecOverHudSolidVertexBuffer.push_back(point[3]);
		m_vecOverHudSolidVertexBuffer.push_back(point[4]);

		m_vecOverHudSolidVertexBuffer.push_back(point[0]);
		m_vecOverHudSolidVertexBuffer.push_back(point[4]);
		m_vecOverHudSolidVertexBuffer.push_back(point[1]);

		m_vecOverHudSolidVertexBuffer.push_back(point[4]);
		m_vecOverHudSolidVertexBuffer.push_back(point[2]);
		m_vecOverHudSolidVertexBuffer.push_back(point[1]);

		m_vecOverHudSolidVertexBuffer.push_back(point[4]);
		m_vecOverHudSolidVertexBuffer.push_back(point[3]);
		m_vecOverHudSolidVertexBuffer.push_back(point[2]);
	}
	else
	{
		// 填6个面
		// face#1
		m_vecSolidVertexBuffer.push_back(point[0]);
		m_vecSolidVertexBuffer.push_back(point[1]);
		m_vecSolidVertexBuffer.push_back(point[2]);

		m_vecSolidVertexBuffer.push_back(point[0]);
		m_vecSolidVertexBuffer.push_back(point[2]);
		m_vecSolidVertexBuffer.push_back(point[3]);

		m_vecSolidVertexBuffer.push_back(point[0]);
		m_vecSolidVertexBuffer.push_back(point[3]);
		m_vecSolidVertexBuffer.push_back(point[4]);

		m_vecSolidVertexBuffer.push_back(point[0]);
		m_vecSolidVertexBuffer.push_back(point[4]);
		m_vecSolidVertexBuffer.push_back(point[1]);

		m_vecSolidVertexBuffer.push_back(point[4]);
		m_vecSolidVertexBuffer.push_back(point[2]);
		m_vecSolidVertexBuffer.push_back(point[1]);

		m_vecSolidVertexBuffer.push_back(point[4]);
		m_vecSolidVertexBuffer.push_back(point[3]);
		m_vecSolidVertexBuffer.push_back(point[2]);
	}

}
//-----------------------------------------------------------------------
void gkAuxRenderer::AuxRender3DBoxSolid( const Vec3& center, float length, ColorF& color /*= ColorF(1.0,1.0,1.0,1.0)*/, bool ignoreZ /*= false*/ )
{
	AuxRender3DBoxSolid(center, Vec3(length, length, length), color, ignoreZ);
}
//-----------------------------------------------------------------------
void gkAuxRenderer::AuxRender3DBoxSolid( const Vec3& center, Vec3& size, ColorF& color /*= ColorF(1.0,1.0,1.0,1.0)*/, bool ignoreZ /*= false*/ )
{
	SetDrawColor( color );

	GK_HELPER_LINEVERTEX point[8];
	point[0].m_vPosition = center + Vec3(-size.x, size.y, -size.z) * 0.5f;
	point[1].m_vPosition  = center + Vec3(-size.x, -size.y, -size.z) * 0.5f;
	point[2].m_vPosition  = center + Vec3(size.x, size.y, -size.z) * 0.5f;
	point[3].m_vPosition  = center + Vec3(size.x, -size.y, -size.z) * 0.5f;
	point[4].m_vPosition  = center + Vec3(-size.x, size.y, size.z) * 0.5f;
	point[5].m_vPosition = center + Vec3(-size.x, -size.y, size.z) * 0.5f;
	point[6].m_vPosition = center + Vec3(size.x, size.y, size.z) * 0.5f;
	point[7].m_vPosition = center + Vec3(size.x, -size.y, size.z) * 0.5f;

	point[0].m_dwDiffuse = m_curColor;
	point[1].m_dwDiffuse = m_curColor;
	point[2].m_dwDiffuse = m_curColor;
	point[3].m_dwDiffuse = m_curColor;
	point[4].m_dwDiffuse = m_curColor;
	point[5].m_dwDiffuse = m_curColor;
	point[6].m_dwDiffuse = m_curColor;
	point[7].m_dwDiffuse = m_curColor;

	if (ignoreZ)
	{
	// 填12个面
	// face#1
		m_vecOverHudSolidVertexBuffer.push_back(point[0]);
		m_vecOverHudSolidVertexBuffer.push_back(point[2]);
		m_vecOverHudSolidVertexBuffer.push_back(point[1]);

		m_vecOverHudSolidVertexBuffer.push_back(point[2]);
		m_vecOverHudSolidVertexBuffer.push_back(point[3]);
		m_vecOverHudSolidVertexBuffer.push_back(point[1]);

		// face#2
		m_vecOverHudSolidVertexBuffer.push_back(point[2]);
		m_vecOverHudSolidVertexBuffer.push_back(point[6]);
		m_vecOverHudSolidVertexBuffer.push_back(point[3]);

		m_vecOverHudSolidVertexBuffer.push_back(point[6]);
		m_vecOverHudSolidVertexBuffer.push_back(point[7]);
		m_vecOverHudSolidVertexBuffer.push_back(point[3]);

		// face#3
		m_vecOverHudSolidVertexBuffer.push_back(point[6]);
		m_vecOverHudSolidVertexBuffer.push_back(point[4]);
		m_vecOverHudSolidVertexBuffer.push_back(point[7]);

		m_vecOverHudSolidVertexBuffer.push_back(point[4]);
		m_vecOverHudSolidVertexBuffer.push_back(point[5]);
		m_vecOverHudSolidVertexBuffer.push_back(point[7]);

		// face#4
		m_vecOverHudSolidVertexBuffer.push_back(point[4]);
		m_vecOverHudSolidVertexBuffer.push_back(point[0]);
		m_vecOverHudSolidVertexBuffer.push_back(point[5]);

		m_vecOverHudSolidVertexBuffer.push_back(point[0]);
		m_vecOverHudSolidVertexBuffer.push_back(point[1]);
		m_vecOverHudSolidVertexBuffer.push_back(point[5]);

		// face#5
		m_vecOverHudSolidVertexBuffer.push_back(point[4]);
		m_vecOverHudSolidVertexBuffer.push_back(point[6]);
		m_vecOverHudSolidVertexBuffer.push_back(point[0]);

		m_vecOverHudSolidVertexBuffer.push_back(point[6]);
		m_vecOverHudSolidVertexBuffer.push_back(point[2]);
		m_vecOverHudSolidVertexBuffer.push_back(point[0]);

		// face#6
		m_vecOverHudSolidVertexBuffer.push_back(point[5]);
		m_vecOverHudSolidVertexBuffer.push_back(point[1]);
		m_vecOverHudSolidVertexBuffer.push_back(point[7]);

		m_vecOverHudSolidVertexBuffer.push_back(point[7]);
		m_vecOverHudSolidVertexBuffer.push_back(point[1]);
		m_vecOverHudSolidVertexBuffer.push_back(point[3]);
	}
	else
	{
		// 填12个面
		// face#1
		m_vecSolidVertexBuffer.push_back(point[0]);
		m_vecSolidVertexBuffer.push_back(point[2]);
		m_vecSolidVertexBuffer.push_back(point[1]);

		m_vecSolidVertexBuffer.push_back(point[2]);
		m_vecSolidVertexBuffer.push_back(point[3]);
		m_vecSolidVertexBuffer.push_back(point[1]);

		// face#2
		m_vecSolidVertexBuffer.push_back(point[2]);
		m_vecSolidVertexBuffer.push_back(point[6]);
		m_vecSolidVertexBuffer.push_back(point[3]);

		m_vecSolidVertexBuffer.push_back(point[6]);
		m_vecSolidVertexBuffer.push_back(point[7]);
		m_vecSolidVertexBuffer.push_back(point[3]);

		// face#3
		m_vecSolidVertexBuffer.push_back(point[6]);
		m_vecSolidVertexBuffer.push_back(point[4]);
		m_vecSolidVertexBuffer.push_back(point[7]);

		m_vecSolidVertexBuffer.push_back(point[4]);
		m_vecSolidVertexBuffer.push_back(point[5]);
		m_vecSolidVertexBuffer.push_back(point[7]);

		// face#4
		m_vecSolidVertexBuffer.push_back(point[4]);
		m_vecSolidVertexBuffer.push_back(point[0]);
		m_vecSolidVertexBuffer.push_back(point[5]);

		m_vecSolidVertexBuffer.push_back(point[0]);
		m_vecSolidVertexBuffer.push_back(point[1]);
		m_vecSolidVertexBuffer.push_back(point[5]);

		// face#5
		m_vecSolidVertexBuffer.push_back(point[4]);
		m_vecSolidVertexBuffer.push_back(point[6]);
		m_vecSolidVertexBuffer.push_back(point[0]);

		m_vecSolidVertexBuffer.push_back(point[6]);
		m_vecSolidVertexBuffer.push_back(point[2]);
		m_vecSolidVertexBuffer.push_back(point[0]);

		// face#6
		m_vecSolidVertexBuffer.push_back(point[5]);
		m_vecSolidVertexBuffer.push_back(point[1]);
		m_vecSolidVertexBuffer.push_back(point[7]);

		m_vecSolidVertexBuffer.push_back(point[7]);
		m_vecSolidVertexBuffer.push_back(point[1]);
		m_vecSolidVertexBuffer.push_back(point[3]);
	}



}
//-----------------------------------------------------------------------
void gkAuxRenderer::AuxRenderMeshFrame( IGameObjectRenderLayer* layer, ColorF& color /*= ColorF(1.0,1.0,1.0,1.0)*/, bool ignoreZ /*= false*/ )
{
	if (layer)
	{
		m_vecMeshFrameBuffer.push_back(layer);
	}
	
}

void gkAuxRenderer::_cleanBuffer()
{
	m_vecMeshFrameBuffer_Render.clear();
	m_vecMeshFrameBuffer.clear();

	m_vecLineVertexBuffer_Render.clear();
	m_vecLineVertexBuffer.clear();

	m_vecSolidVertexBuffer_Render.clear();
	m_vecSolidVertexBuffer.clear();

	m_vecOverHudLineVertexBuffer_Render.clear();
	m_vecOverHudLineVertexBuffer.clear();

	m_vecOverHudSolidVertexBuffer_Render.clear();
	m_vecOverHudSolidVertexBuffer.clear();

	m_vecTextRenderList_Render.clear();
	m_vecTextRenderList.clear();

	m_vecScreenBoxVertexBuffer_Render.clear();
	m_vecScreenBoxVertexBuffer.clear();

	m_vecScreenLineVertexBuffer_Render.clear();
	m_vecScreenLineVertexBuffer.clear();

	m_vecScreenBoxTexturedVertexBuffer_Render.clear();
	m_vecScreenBoxTexturedVertexBuffer.clear();

	m_vecScreenBoxTexture_Render.clear();
	m_vecScreenBoxTexture.clear();
}

void gkAuxRenderer::_swapBufferForRendering()
{
	m_vecMeshFrameBuffer_Render = m_vecMeshFrameBuffer;
	m_vecMeshFrameBuffer.clear();

	std::vector<GK_HELPER_LINEVERTEX> voidvector;
	m_vecLineVertexBuffer_Render = m_vecLineVertexBuffer;//.assign(m_vecLineVertexBuffer.begin(), m_vecLineVertexBuffer.end());
	m_vecLineVertexBuffer.clear();
	//m_vecLineVertexBuffer.swap(0);
	m_vecSolidVertexBuffer_Render = m_vecSolidVertexBuffer;//.assign(m_vecSolidVertexBuffer.begin(), m_vecSolidVertexBuffer.end());
	m_vecSolidVertexBuffer.clear();
	//m_vecSolidVertexBuffer.swap(0);

	m_vecOverHudLineVertexBuffer_Render = m_vecOverHudLineVertexBuffer;//.assign(m_vecOverHudLineVertexBuffer.begin(), m_vecOverHudLineVertexBuffer.end());
	m_vecOverHudLineVertexBuffer.clear();
	//m_vecOverHudLineVertexBuffer.swap(0);
	m_vecOverHudSolidVertexBuffer_Render = m_vecOverHudSolidVertexBuffer;//.assign(m_vecOverHudSolidVertexBuffer.begin(), m_vecOverHudSolidVertexBuffer.end());
	m_vecOverHudSolidVertexBuffer.clear();
	//m_vecOverHudSolidVertexBuffer_Render.swap(0);

	m_vecTextRenderList_Render = m_vecTextRenderList;
	m_vecTextRenderList.clear();

	m_vecScreenBoxVertexBuffer_Render = m_vecScreenBoxVertexBuffer;
	m_vecScreenBoxVertexBuffer.clear();

	m_vecScreenLineVertexBuffer_Render = m_vecScreenLineVertexBuffer;
	m_vecScreenLineVertexBuffer.clear();

	m_vecScreenBoxTexturedVertexBuffer_Render = m_vecScreenBoxTexturedVertexBuffer;
	m_vecScreenBoxTexturedVertexBuffer.clear();

	m_vecScreenBoxTexture_Render = m_vecScreenBoxTexture;
	m_vecScreenBoxTexture.clear();

}
//////////////////////////////////////////////////////////////////////////
void gkAuxRenderer::AuxRenderText(const TCHAR* text, int posx, int posy, const IFtFont* font, const ColorB& color, uint32 alignment, uint32 style )
{
	if (style)
	{
		gEnv->pFont->DrawString( text, font, Vec2(posx + 1, posy + 1), ColorB(0,0,0,127), alignment );
	}
	//
	gEnv->pFont->DrawString( text, font, Vec2(posx, posy), color, alignment );
	//gEnv->pFont->DrawString( it->text, it->font, Vec2(it->posx, it->posy), it->color, it->alignment );
}
//////////////////////////////////////////////////////////////////////////
void gkAuxRenderer::_FlushAllText( LPDIRECT3DDEVICE9 pDevice )
{
	if( !m_vecTextRenderList_Render.empty())
	{
		TextRenderList::iterator it = m_vecTextRenderList_Render.begin();
		for (; it != m_vecTextRenderList_Render.end(); ++it)
		{
			//gEnv->pFont->DrawString( it->text, it->font, Vec2(it->posx, it->posy), it->color, it->alignment );

			//getRenderer()->FX_RenderText( it->text, it->posx, it->posy, it->font, it->alignment, ColorB(it->color) );
		}
		m_vecTextRenderList_Render.clear();
	}
}

void gkAuxRenderer::AuxRenderScreenBox( const Vec2& pos, const Vec2& wh, const ColorB& color )
{
// 	GK_HELPER_2DVERTEX point[4];
// 	point[0].m_vPosition = gEnv->pRenderer->ScreenPosToViewportPos(Vec3(pos.x, pos.y, 0.5));
// 	point[1].m_vPosition = gEnv->pRenderer->ScreenPosToViewportPos(Vec3(pos.x + wh.x, pos.y,0.5));
// 	point[2].m_vPosition = gEnv->pRenderer->ScreenPosToViewportPos(Vec3(pos.x, pos.y + wh.y,0.5));
// 	point[3].m_vPosition = gEnv->pRenderer->ScreenPosToViewportPos(Vec3(pos.x + wh.x, pos.y + wh.y,0.5));
// 
// 	point[0].m_dwDiffuse = color.pack_argb8888();
// 	point[1].m_dwDiffuse = color.pack_argb8888();
// 	point[2].m_dwDiffuse = color.pack_argb8888();
// 	point[3].m_dwDiffuse = color.pack_argb8888();
// 
// 	m_vecScreenBoxVertexBuffer.push_back( point[0] );
// 	m_vecScreenBoxVertexBuffer.push_back( point[2] );
// 	m_vecScreenBoxVertexBuffer.push_back( point[1] );
// 	m_vecScreenBoxVertexBuffer.push_back( point[1] );
// 	m_vecScreenBoxVertexBuffer.push_back( point[2] );
// 	m_vecScreenBoxVertexBuffer.push_back( point[3] );

	// unique to texture routine

	GK_HELPER_2DVERTEX_TEXTURED point[4];
	point[0].m_vPosition = gEnv->pRenderer->ScreenPosToViewportPos(Vec3(pos.x, pos.y, 0.5));
	point[1].m_vPosition = gEnv->pRenderer->ScreenPosToViewportPos(Vec3(pos.x + wh.x, pos.y,0.5));
	point[2].m_vPosition = gEnv->pRenderer->ScreenPosToViewportPos(Vec3(pos.x, pos.y + wh.y,0.5));
	point[3].m_vPosition = gEnv->pRenderer->ScreenPosToViewportPos(Vec3(pos.x + wh.x, pos.y + wh.y,0.5));

	point[0].m_vTexcorrd = Vec2(0,0);
	point[1].m_vTexcorrd = Vec2(1,0);
	point[2].m_vTexcorrd = Vec2(0,1);
	point[3].m_vTexcorrd = Vec2(1,1);

	point[0].m_dwDiffuse = color.pack_argb8888();
	point[1].m_dwDiffuse = color.pack_argb8888();
	point[2].m_dwDiffuse = color.pack_argb8888();
	point[3].m_dwDiffuse = color.pack_argb8888();

	m_vecScreenBoxTexturedVertexBuffer.push_back( point[0] );
	m_vecScreenBoxTexturedVertexBuffer.push_back( point[2] );
	m_vecScreenBoxTexturedVertexBuffer.push_back( point[1] );
	m_vecScreenBoxTexturedVertexBuffer.push_back( point[1] );
	m_vecScreenBoxTexturedVertexBuffer.push_back( point[2] );
	m_vecScreenBoxTexturedVertexBuffer.push_back( point[3] );

	m_vecScreenBoxTexture.push_back( gkTextureManager::ms_DefaultWhite.getPointer() );
}

void gkAuxRenderer::AuxRenderScreenBox( const Vec2& pos, const Vec2& wh, const ColorB& color, ITexture* texture )
{
	GK_HELPER_2DVERTEX_TEXTURED point[4];
	point[0].m_vPosition = gEnv->pRenderer->ScreenPosToViewportPos(Vec3(pos.x, pos.y, 0.5));
	point[1].m_vPosition = gEnv->pRenderer->ScreenPosToViewportPos(Vec3(pos.x + wh.x, pos.y,0.5));
	point[2].m_vPosition = gEnv->pRenderer->ScreenPosToViewportPos(Vec3(pos.x, pos.y + wh.y,0.5));
	point[3].m_vPosition = gEnv->pRenderer->ScreenPosToViewportPos(Vec3(pos.x + wh.x, pos.y + wh.y,0.5));

	point[0].m_vTexcorrd = Vec2(0,0);
	point[1].m_vTexcorrd = Vec2(1,0);
	point[2].m_vTexcorrd = Vec2(0,1);
	point[3].m_vTexcorrd = Vec2(1,1);

	point[0].m_dwDiffuse = color.pack_argb8888();
	point[1].m_dwDiffuse = color.pack_argb8888();
	point[2].m_dwDiffuse = color.pack_argb8888();
	point[3].m_dwDiffuse = color.pack_argb8888();

	m_vecScreenBoxTexturedVertexBuffer.push_back( point[0] );
	m_vecScreenBoxTexturedVertexBuffer.push_back( point[2] );
	m_vecScreenBoxTexturedVertexBuffer.push_back( point[1] );
	m_vecScreenBoxTexturedVertexBuffer.push_back( point[1] );
	m_vecScreenBoxTexturedVertexBuffer.push_back( point[2] );
	m_vecScreenBoxTexturedVertexBuffer.push_back( point[3] );

	m_vecScreenBoxTexture.push_back( texture );
}

void gkAuxRenderer::AuxRenderScreenLine(const Vec2& from, const Vec2& to, const ColorB& color)
{
	GK_HELPER_2DVERTEX point[2];
	point[0].m_vPosition = gEnv->pRenderer->ScreenPosToViewportPos(Vec3(from.x, from.y, 0.5));
	point[1].m_vPosition = gEnv->pRenderer->ScreenPosToViewportPos(Vec3(to.x, to.y,0.5));

	point[0].m_dwDiffuse = color.pack_argb8888();
	point[1].m_dwDiffuse = color.pack_argb8888();

	m_vecScreenLineVertexBuffer.push_back( point[0] );
	m_vecScreenLineVertexBuffer.push_back( point[1] );
}


