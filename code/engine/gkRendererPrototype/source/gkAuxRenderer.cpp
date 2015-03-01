#include "StableHeader.h"
#include "gkAuxRenderer.h"
#include "ICamera.h"
//#include "RenderRes\gkMesh.h"
#include "IEntity.h"
#include "gkRendererPrototype.h"
//#include "RenderRes\gkShaderManager.h"
#include "IGameObjectLayer.h"


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
void gkAuxRenderer::AuxRender3DGird( const Vec3& center, int row /*= 19*/, float gap /*= 5.0f */ ,ColorF& color, bool ignoreZ )
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
// void gkAuxRenderer::_renderAllHelper( LPDIRECT3DDEVICE9 pDevice )
// {
// // 	if( gEnv->pProfiler->getFrameCount() % 2 == 0 )
// // 		return;
// 
// 	// get the main camera viewproj matrix
// 	{
// 		//CCamera* cam = gEnv->p3DEngine->getCamera(_T("MainCamera"))->getCCam();
// 		//gEnv->pRenderer->setCamera(cam);
// 	}
// 
// 	//D3DXMatrixInverse( &mViewToLightProj, NULL, &mViewToLightProj );
// 
// 
// 	static Matrix44 viewMat = gkRendererPrototype::getShaderContent().getViewMatrix();
// 	static Matrix44 projMat = gkRendererPrototype::getShaderContent().getProjectionMatrix();
// 
// 	if (gEnv->pProfiler->getFrameCount() % 2 == 0)
// 	{
// 		viewMat = gkRendererPrototype::getShaderContent().getViewMatrix();
// 		projMat = gkRendererPrototype::getShaderContent().getProjectionMatrix();
// 	}
// 
// 	pDevice->SetTransform(D3DTS_VIEW, (D3DXMATRIX*)(&viewMat));
// 	pDevice->SetTransform(D3DTS_PROJECTION, (D3DXMATRIX*)(&projMat));
// 
// 	pDevice->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, TRUE);
// 	pDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
// 	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
// 
// 	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
// 	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
// 	// list array
// 	// render直接渲染点列
// 	if (!m_vecLineVertexBuffer_Render.empty())
// 	{
// 		pDevice->SetTexture( 0, NULL );
// 		// Turn off D3D lighting
// 
// 		pDevice->SetFVF(GKFVF_HELPER_LINEVERTEX);
// 		int size = m_vecLineVertexBuffer_Render.size();
// 		pDevice->DrawPrimitiveUP(D3DPT_LINELIST, m_vecLineVertexBuffer_Render.size() / 2, &(m_vecLineVertexBuffer_Render[0]), sizeof(GK_HELPER_LINEVERTEX));
// 	}
// 	m_vecLineVertexBuffer_Render.clear();
// 
//  	if (!m_vecSolidVertexBuffer_Render.empty())
//  	{
//  		pDevice->SetTexture( 0, NULL );
//  		// Turn off D3D lighting
//  		//pDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
//  		pDevice->SetFVF(GKFVF_HELPER_LINEVERTEX);
//  		int size = m_vecSolidVertexBuffer_Render.size();
//  		pDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, m_vecSolidVertexBuffer_Render.size() / 3, &(m_vecSolidVertexBuffer_Render[0]), sizeof(GK_HELPER_LINEVERTEX));
//  	}
//  	m_vecSolidVertexBuffer_Render.clear();
// 
// 	// render mesh frame [8/29/2011 Kaiming-Desktop]
// 	if (!m_vecMeshFrameBuffer_Render.empty())
// 	{
// 		pDevice->SetTexture( 0, NULL );
// 		// Turn off D3D lighting
// 		//pDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
// 		pDevice->SetRenderState(D3DRS_FILLMODE , D3DFILL_WIREFRAME );
// 
// 		gkShaderPtr pShader = gkShaderManager::ms_AuxRenderer;
// 		// Bloommerge
// 		GKHANDLE hTech = pShader->FX_GetTechniqueByName("RenderScene");
// 		pShader->FX_SetTechnique( hTech );
// 		{
// 			pShader->FX_Commit();
// 
// 			UINT cPasses;
// 			pShader->FX_Begin( &cPasses, 0 );
// 			for( UINT p = 0; p < cPasses; ++p )
// 			{
// 				pShader->FX_BeginPass( p );
// 				pShader->FX_SetValue( "g_mProjection", &projMat , sizeof(Matrix44) );
// 				pShader->FX_SetValue( "g_auxcolor", &(ColorF(0.2f,0.2f,0.2f,0.6f)), sizeof(ColorF) );
// 				for (uint32 i =0; i < m_vecMeshFrameBuffer_Render.size(); ++i)
// 				{
// 					Matrix44 matworldview = m_vecMeshFrameBuffer_Render[i]->GetWorldMatrix() * viewMat;
// 
// 					
// 					pShader->FX_SetValue( "g_mWorldViewProj", &(matworldview * projMat), sizeof(Matrix44) );
// 					pShader->FX_SetValue( "g_mWorldView", &(matworldview), sizeof(Matrix44) );
// 
// 					pShader->FX_Commit();
// 					// little tricks, move wireframe a bit to camera, to solve the line dot [8/29/2011 Kaiming-Desktop]
// 					m_vecMeshFrameBuffer_Render[i]->getMesh()->GetD3DXMesh()->DrawSubset(0);
// 				}
// 				pShader->FX_EndPass();
// 			}
// 			pShader->FX_End();
// 		}
// 		
// 		pDevice->SetRenderState(D3DRS_FILLMODE , D3DFILL_SOLID );
// 		D3DXMATRIX matIdentity;
// 		D3DXMatrixIdentity(&matIdentity);
// 		pDevice->SetTransform(D3DTS_WORLD, &matIdentity);
// 	}
// 	m_vecMeshFrameBuffer_Render.clear();
// 
// 
// 	// off depth
// 	pDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
// 	if (!m_vecOverHudLineVertexBuffer_Render.empty())
// 	{
// 		pDevice->SetTexture( 0, NULL );
// 		// Turn off D3D lighting
// 		//pDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
// 
// 		pDevice->SetFVF(GKFVF_HELPER_LINEVERTEX);
// 		int size = m_vecOverHudLineVertexBuffer_Render.size();
// 		pDevice->DrawPrimitiveUP(D3DPT_LINELIST, m_vecOverHudLineVertexBuffer_Render.size() / 2, &(m_vecOverHudLineVertexBuffer_Render[0]), sizeof(GK_HELPER_LINEVERTEX));
// 	}
// 	m_vecOverHudLineVertexBuffer_Render.clear();
// 
// 	if (!m_vecOverHudSolidVertexBuffer_Render.empty())
// 	{
// 		pDevice->SetTexture( 0, NULL );
// 		// Turn off D3D lighting
// 		//pDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
// 		pDevice->SetFVF(GKFVF_HELPER_LINEVERTEX);
// 		int size = m_vecOverHudSolidVertexBuffer_Render.size();
// 		pDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, m_vecOverHudSolidVertexBuffer_Render.size() / 3, &(m_vecOverHudSolidVertexBuffer_Render[0]), sizeof(GK_HELPER_LINEVERTEX));
// 	}
// 	m_vecOverHudSolidVertexBuffer_Render.clear();
// 
// 	// on depth
// 	pDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
// 
// 	
// }

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
void gkAuxRenderer::AuxRenderMeshFrame( IGameObject* entity, ColorF& color /*= ColorF(1.0,1.0,1.0,1.0)*/, bool ignoreZ /*= false*/ )
{
	if (entity && entity->getRenderLayer())
	{
		m_vecMeshFrameBuffer.push_back(entity->getRenderLayer());
	}
	
}

void gkAuxRenderer::_swapBufferForRendering()
{
	m_vecMeshFrameBuffer_Render = m_vecMeshFrameBuffer;
	std::vector<IgkEntity*> voidvec;
	m_vecMeshFrameBuffer.clear();
	//m_vecMeshFrameBuffer.swap(0);

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

}

void gkAuxRenderer::AuxRenderText( const TCHAR* text, int posx, int posy, ColorF& color /*= ColorF(1.0,1.0,1.0,0.7) */ )
{

}



