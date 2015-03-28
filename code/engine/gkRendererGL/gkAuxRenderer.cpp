#include "StableHeader.h"
#include "gkAuxRenderer.h"
#include "ICamera.h"
#include "gkRendererGL330.h"
#include "IGameObjectLayer.h"
#include "RenderRes/gkShaderManager.h"
#include "IMesh.h"
#include "gkVirtualAPI.h"

#ifdef RENDERAPI_GLES2
#include "glExtension.h"
#endif



#define AUX_VERTEX_ARRAY				0
#define AUX_COLOR_ARRAY					1
#define AUX_TEXCOORD_ARRAY				1
//REFACTOR FIX

gkAuxRendererGLES2::gkAuxRendererGLES2( void )
{
	m_vecLineVertexBuffer.clear();
	m_vecSolidVertexBuffer.clear();
	m_vecOverHudLineVertexBuffer.clear();
	m_vecOverHudSolidVertexBuffer.clear();
	ColorF color = ColorF(1, 1, 1, 1);
	SetDrawColor( color );
}

gkAuxRendererGLES2::~gkAuxRendererGLES2( void )
{
	
}
//-----------------------------------------------------------------------
void gkAuxRendererGLES2::SetDrawColor( ColorF& color )
{
	m_curColor = (DWORD)color.pack_argb8888();
}
//-----------------------------------------------------------------------
void gkAuxRendererGLES2::AuxRender3DLine( const Vec3& from, const Vec3& to, ColorF& color, bool ignoreZ)
{
	SetDrawColor(color);
	GK_HELPER_2DVERTEX vertexFrom(from, m_curColor);
	GK_HELPER_2DVERTEX vertexTo(to, m_curColor);
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

void gkAuxRendererGLES2::AuxRenderSkeleton( const Vec3& from, const Vec3& to, ColorF& color /*= ColorF(1.0,1.0,1.0,1.0)*/, float radius /*= 0.05f*/, bool ignoreZ /*= false */ )
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

	ColorF redcol = ColorF(1,0,0,1);
	AuxRender3DLine(to, jointPt + dirInPlane, redcol, true);
	AuxRender3DLine(to, jointPt + dirInPlane1, redcol, true);
	AuxRender3DLine(to, jointPt + dirInPlane2, redcol, true);

	AuxRender3DLine(jointPt + dirInPlane, jointPt + dirInPlane2, redcol, true);
	AuxRender3DLine(jointPt + dirInPlane1, jointPt + dirInPlane2, redcol, true);
	AuxRender3DLine(jointPt + dirInPlane, jointPt + dirInPlane1, redcol, true);



}


//-----------------------------------------------------------------------
void gkAuxRendererGLES2::AuxRender3DGird( const Vec3& center, int row /*= 19*/, float gap /*= 5.0f */ ,ColorF& color, bool ignoreZ )
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
void gkAuxRendererGLES2::AuxRender3DCircle( const Vec3& center, const Vec3& normal, float radius, uint32 side, ColorF& color /*= ColorF(1.0,1.0,1.0,1.0)*/, bool ignoreZ /*= false*/ )
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

void gkAuxRendererGLES2::AuxRender3DBoxFrameRotated( const Vec3& center, const Quat& rot, float length, ColorF& color /*= ColorF(1.0,1.0,1.0,1.0)*/, bool ignoreZ /*= false*/ )
{
	AuxRender3DBoxFrameRotated(center , rot, Vec3(length, length, length), color, ignoreZ);
}

void gkAuxRendererGLES2::AuxRender3DBoxFrameRotated( const Vec3& center, const Quat& rot, const Vec3& size, ColorF& color /*= ColorF(1.0,1.0,1.0,1.0)*/, bool ignoreZ /*= false*/ )
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
void gkAuxRendererGLES2::AuxRender3DBoxFrame( const Vec3& center, float length ,ColorF& color, bool ignoreZ )
{
	Vec3 size = Vec3(length, length, length);
	AuxRender3DBoxFrame(center, size, color, ignoreZ);
}
//-----------------------------------------------------------------------
void gkAuxRendererGLES2::AuxRender3DBoxFrame( const Vec3& center, Vec3& size, ColorF& color /*= ColorF(1.0,1.0,1.0,1.0)*/, bool ignoreZ )
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
void gkAuxRendererGLES2::AuxRenderGizmo(const Matrix44& xform, float size, uint8 axisHighlight, bool ignoreZ, uint8 type)
{
	Vec3 vCenter = xform.GetRow(3);
	Vec3 vXaxis = xform.GetRow(0).GetNormalized();
	Vec3 vYaxis = xform.GetRow(1).GetNormalized();
	Vec3 vZaxis = xform.GetRow(2).GetNormalized();
 
	ColorF colorYellow = ColorF(1,1,0,1);
	ColorF colorRed = ColorF(1,0,0,1);
	ColorF colorBlue = ColorF(0,0,1,1);
	ColorF colorGreen = ColorF(0,1,0,1);

 	AuxRender3DLine( vCenter, vCenter + vXaxis * size, axisHighlight == 1 ?  colorYellow : colorRed, ignoreZ);
 	if (type < 2)
 		AuxRenderPyramid( vCenter + vXaxis * size, vXaxis, size * 0.1f, axisHighlight == 1 ? colorYellow : colorRed, ignoreZ);
 	else
 		AuxRender3DBoxSolid( vCenter + vXaxis * size, size * 0.2f, axisHighlight == 1 ? colorYellow : colorRed, ignoreZ);
 
 	AuxRender3DLine( vCenter, vCenter + vYaxis * size, axisHighlight == 2 ? colorYellow : colorGreen, ignoreZ);
 	if (type < 2)
		AuxRenderPyramid( vCenter + vYaxis * size, vYaxis, size * 0.1f, axisHighlight == 2 ? colorYellow : colorGreen, ignoreZ);
	else
		AuxRender3DBoxSolid( vCenter+ vYaxis * size, size * 0.2f, axisHighlight == 1 ? colorYellow : colorGreen, ignoreZ);
 
 	AuxRender3DLine( vCenter, vCenter + vZaxis * size, axisHighlight == 3 ? colorYellow : colorBlue, ignoreZ);
 	if (type < 2)
 		AuxRenderPyramid( vCenter + vZaxis * size, vZaxis, size * 0.1f, axisHighlight == 3 ? colorYellow : colorBlue, ignoreZ);
 	else
 		AuxRender3DBoxSolid( vCenter + vZaxis * size, size * 0.2f, axisHighlight == 1 ? colorYellow : colorBlue, ignoreZ);
}
//-----------------------------------------------------------------------
// void gkAuxRenderer::_renderAllHelper( LPDIRECT3DDEVICE9 pDevice )
// {
// // 	if( gEnv->pProfiler->getFrameCount() % 2 == 0 )
// // 		return;
// 
// 	// get the main camera viewproj matrix
// 	{
// 		//CCamera* cam = gEnv->p3DEngine->getMainCamera()->getCCam();
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
// 		pDevice->DrawPrimitiveUP(D3DPT_LINELIST, m_vecLineVertexBuffer_Render.size() / 2, &(m_vecLineVertexBuffer_Render[0]), sizeof(GK_HELPER_2DVERTEX));
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
//  		pDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, m_vecSolidVertexBuffer_Render.size() / 3, &(m_vecSolidVertexBuffer_Render[0]), sizeof(GK_HELPER_2DVERTEX));
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
// 		pDevice->DrawPrimitiveUP(D3DPT_LINELIST, m_vecOverHudLineVertexBuffer_Render.size() / 2, &(m_vecOverHudLineVertexBuffer_Render[0]), sizeof(GK_HELPER_2DVERTEX));
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
// 		pDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, m_vecOverHudSolidVertexBuffer_Render.size() / 3, &(m_vecOverHudSolidVertexBuffer_Render[0]), sizeof(GK_HELPER_2DVERTEX));
// 	}
// 	m_vecOverHudSolidVertexBuffer_Render.clear();
// 
// 	// on depth
// 	pDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
// 
// 	
// }

//-----------------------------------------------------------------------
void gkAuxRendererGLES2::AuxRenderAABB( const AABB& aabb, ColorF& color /*= ColorF(1.0,1.0,1.0,1.0)*/, bool ignoreZ )
{

	Vec3 vecAABB = aabb.GetCenter();
    Vec3 vecSize = aabb.GetSize();
	AuxRender3DBoxFrame( vecAABB, vecSize, color, ignoreZ );
}
//-----------------------------------------------------------------------
void gkAuxRendererGLES2::AuxRenderPyramid( const Vec3& center, const Vec3& dir, const float radius, ColorF& color /*= ColorF(1.0,1.0,1.0,1.0)*/ , bool ignoreZ)
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

	GK_HELPER_2DVERTEX point[5];
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
void gkAuxRendererGLES2::AuxRender3DBoxSolid( const Vec3& center, float length, ColorF& color /*= ColorF(1.0,1.0,1.0,1.0)*/, bool ignoreZ /*= false*/ )
{
	//AuxRender3DBoxSolid(center, Vec3(length, length, length), color, ignoreZ);
}
//-----------------------------------------------------------------------
void gkAuxRendererGLES2::AuxRender3DBoxSolid( const Vec3& center, Vec3& size, ColorF& color /*= ColorF(1.0,1.0,1.0,1.0)*/, bool ignoreZ /*= false*/ )
{

	GK_HELPER_2DVERTEX point[8];
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
void gkAuxRendererGLES2::AuxRenderMeshFrame( IGameObjectRenderLayer* entity, ColorF& color /*= ColorF(1.0,1.0,1.0,1.0)*/, bool ignoreZ /*= false*/ )
{
	if (entity)
	{
		m_vecMeshFrameBuffer.push_back(entity);
	}
	
}

void gkAuxRendererGLES2::_cleanBuffer()
{

}

void gkAuxRendererGLES2::_swapBufferForRendering()
{
	m_vecMeshFrameBuffer_Render = m_vecMeshFrameBuffer;
	m_vecMeshFrameBuffer.clear();
	//m_vecMeshFrameBuffer.swap(0);

	std::vector<GK_HELPER_2DVERTEX> voidvector;
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

	m_vecScreenBoxVertexBuffer_Render = m_vecScreenBoxVertexBuffer;
	m_vecScreenBoxVertexBuffer.clear();

    m_vecScreenBoxTexturedVertexBuffer_Render = m_vecScreenBoxTexturedVertexBuffer;
    m_vecScreenBoxTexturedVertexBuffer.clear();
    
    m_vecScreenBoxTexture_Render = m_vecScreenBoxTexture;
    m_vecScreenBoxTexture.clear();
}

void gkAuxRendererGLES2::AuxRenderText( const TCHAR* text, int posx, int posy, const IFtFont* font, const ColorB& color /*= ColorB(255,255,255,200 ) */, uint32 alignment /*= eTra_HLeft | eTra_VTop */, uint32 style  )
{
	gEnv->pFont->DrawString( text, font, Vec2(posx + 1, posy + 1), ColorB(0,0,0,127), alignment );
	gEnv->pFont->DrawString(text, font, Vec2(posx, posy), color, alignment);
}

void gkAuxRendererGLES2::_FlushAllHelper()
{
	//return;
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
// 	glDisable(GL_DEPTH_TEST);
//     glDisable(GL_CULL_FACE);
    
	gkShaderManager::ms_AuxRenderer->FX_Begin(0,0);

	/* Set blending mode */
// 	glEnable(GL_BLEND);
// 	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// update aux value
	//////////////////////////////////////////////////////////////////////////
	if( m_vecLineVertexBuffer_Render.size() > 0 )
	{
		glBindBuffer( GL_ARRAY_BUFFER, m_lineBuffer );
		
		int size = m_vecLineVertexBuffer_Render.size() * sizeof(GK_HELPER_2DVERTEX);
		
		GLvoid* Data;

		gkVirtualAPI::gkVAPI_MapBuffer( &Data, GL_ARRAY_BUFFER, size, gkVirtualAPI::eVAPI_MapWrite );

		if (Data)
		{
			memcpy(Data, m_vecLineVertexBuffer_Render.data(),  size);
		}

		gkVirtualAPI::gkVAPI_UnMapBuffer( Data, GL_ARRAY_BUFFER, size );

		glBindBuffer( GL_ARRAY_BUFFER, 0 );
	}


	//////////////////////////////////////////////////////////////////////////
	if(m_vecSolidVertexBuffer_Render.size() > 0)
	{
		glBindBuffer( GL_ARRAY_BUFFER, m_vertexBuffer );
		int size = m_vecSolidVertexBuffer_Render.size() * sizeof(GK_HELPER_2DVERTEX);

		GLvoid* Data = NULL;

		gkVirtualAPI::gkVAPI_MapBuffer( &Data, GL_ARRAY_BUFFER, size, gkVirtualAPI::eVAPI_MapWrite );

		

		if (Data)
		{
			memcpy(Data, m_vecSolidVertexBuffer_Render.data(),  size);
		}

		gkVirtualAPI::gkVAPI_UnMapBuffer( Data, GL_ARRAY_BUFFER, size );

		glBindBuffer( GL_ARRAY_BUFFER, 0 );
	}


	//////////////////////////////////////////////////////////////////////////
	if(m_vecScreenBoxVertexBuffer_Render.size() > 0)
	{
		glBindBuffer( GL_ARRAY_BUFFER, m_hudBoxBuffer );
		int size = m_vecScreenBoxVertexBuffer_Render.size() * sizeof(GK_HELPER_2DVERTEX);

		GLvoid* Data = NULL;

		gkVirtualAPI::gkVAPI_MapBuffer( &Data, GL_ARRAY_BUFFER, size, gkVirtualAPI::eVAPI_MapWrite );

		

		if (Data)
		{
			memcpy(Data, m_vecScreenBoxVertexBuffer_Render.data(),  size);
		}

		gkVirtualAPI::gkVAPI_UnMapBuffer( Data, GL_ARRAY_BUFFER, size );

		glBindBuffer( GL_ARRAY_BUFFER, 0 );
	}

	//////////////////////////////////////////////////////////////////////////
	// draw
	Matrix44 viewMat = gkRendererGL::getShaderContent().getViewMatrix();
	Matrix44 projMat = gkRendererGL::getShaderContent().getProjectionMatrix();
	Matrix44 MVPMat = viewMat * projMat;
	gkShaderManager::ms_AuxRenderer->FX_SetMatrix( "modelViewProjectionMatrix", MVPMat );

	gkVirtualAPI::gen_or_bind_vao( m_lineVAO, m_lineBuffer );
	apply_2dvert_layout(true);
	glDrawArraysInstanced( GL_LINES, 0, m_vecLineVertexBuffer_Render.size(),1 );
	gkVirtualAPI::unbind_vao( );

	MVPMat.SetIdentity();
	gkShaderManager::ms_AuxRenderer->FX_SetMatrix( "modelViewProjectionMatrix", MVPMat );

	// draw polys
	gkVirtualAPI::gen_or_bind_vao( m_vertexVAO, m_vertexBuffer );
	apply_2dvert_layout(true);
	glDrawArraysInstanced( GL_TRIANGLES, 0, m_vecSolidVertexBuffer_Render.size(),1 );
	gkVirtualAPI::unbind_vao();

	
	gkVirtualAPI::gen_or_bind_vao( m_hudBoxVAO, m_hudBoxBuffer );
	apply_2dvert_layout(true);
	glDrawArraysInstanced( GL_TRIANGLES, 0, m_vecScreenBoxVertexBuffer_Render.size(),1 );
	gkVirtualAPI::unbind_vao();



	gkShaderManager::ms_AuxRenderer->FX_End();
    
    
    gkShaderManager::ms_AuxRenderer_Tex->FX_Begin(0, 0);
    
    
    
    if (!m_vecScreenBoxTexturedVertexBuffer_Render.empty())
    {
        int count = m_vecScreenBoxTexturedVertexBuffer_Render.size() / 6;
        
        for (int i=0; i < count; ++i)
        {
            ITexture* tex = m_vecScreenBoxTexture_Render[i];
            tex->Apply(0,2);
            
            //////////////////////////////////////////////////////////////////////////
			{
                glBindBuffer( GL_ARRAY_BUFFER, m_hudBoxTexBuffer );

                int size = 6 * sizeof(GK_HELPER_2DVERTEX_TEXTURED);

				GLvoid* Data = NULL;

				gkVirtualAPI::gkVAPI_MapBuffer( &Data, GL_ARRAY_BUFFER, size, gkVirtualAPI::eVAPI_MapWrite );

				
                
                if (Data)
                {
                    memcpy(Data, &(m_vecScreenBoxTexturedVertexBuffer_Render[i * 6]),  size);
                }
                
				gkVirtualAPI::gkVAPI_UnMapBuffer( Data, GL_ARRAY_BUFFER, size );

                glBindBuffer( GL_ARRAY_BUFFER, 0 );
                
            }
            
			gkVirtualAPI::gen_or_bind_vao( m_hudBoxTexVAO, m_hudBoxTexBuffer );
			apply_2dvert_texed_layout(true);
            glDrawArraysInstanced( GL_TRIANGLES, 0, 6,1 );
            gkVirtualAPI::unbind_vao();
        }
    }
    
    gkShaderManager::ms_AuxRenderer_Tex->FX_End();
    
}

void gkAuxRendererGLES2::_FlushAllText()
{




}

void gkAuxRendererGLES2::init()
{
	m_lineVAO = 0;
	m_vertexVAO = 0;
	m_hudBoxVAO = 0;
	m_hudBoxTexVAO = 0;


	glGenBuffers(1, &m_lineBuffer);
	glGenBuffers(1, &m_vertexBuffer);
	glGenBuffers(1, &m_hudBoxBuffer);
    glGenBuffers(1, &m_hudBoxTexBuffer);

	//////////////////////////////////////////////////////////////////////////
	gkVirtualAPI::gen_or_bind_vao(m_lineVAO, m_lineBuffer);
	apply_2dvert_layout();
	gkVirtualAPI::unbind_vao();

	//////////////////////////////////////////////////////////////////////////
	gkVirtualAPI::gen_or_bind_vao(m_vertexVAO, m_vertexBuffer);
	apply_2dvert_layout();
	gkVirtualAPI::unbind_vao();

	//////////////////////////////////////////////////////////////////////////
	gkVirtualAPI::gen_or_bind_vao(m_hudBoxVAO, m_hudBoxBuffer);
	apply_2dvert_layout();
	gkVirtualAPI::unbind_vao();

	//////////////////////////////////////////////////////////////////////////
	gkVirtualAPI::gen_or_bind_vao(m_hudBoxTexVAO, m_hudBoxTexBuffer);
	apply_2dvert_texed_layout();
	gkVirtualAPI::unbind_vao();
}

void gkAuxRendererGLES2::destroy()
{
	glDeleteBuffers(1, &m_lineBuffer);
	glDeleteBuffers(1, &m_vertexBuffer);
	glDeleteBuffers(1, &m_hudBoxBuffer);
    glDeleteBuffers(1, &m_hudBoxTexBuffer);
}

void gkAuxRendererGLES2::AuxRenderScreenBox( const Vec2& pos, const Vec2& wh, const ColorB& color )
{
	GK_HELPER_2DVERTEX point[4];
	// make the screen to -1 - 1 proj space
	float sw = getRenderer()->GetScreenWidth();
	float sh = getRenderer()->GetScreenHeight();

	point[0].m_vPosition = Vec3(pos.x, pos.y, 0);
	point[1].m_vPosition = Vec3(pos.x + wh.x, pos.y,0);
	point[2].m_vPosition = Vec3(pos.x, pos.y + wh.y,0);
	point[3].m_vPosition = Vec3(pos.x + wh.x, pos.y + wh.y,0);

	for (uint32 i=0; i < 4; ++i)
	{
		point[i].m_vPosition.x = point[i].m_vPosition.x / sw * 2.0 - 1.0;
		point[i].m_vPosition.y = 1.0 - point[i].m_vPosition.y / sh * 2.0;
	}

	point[0].m_dwDiffuse = color.pack_abgr8888();
	point[1].m_dwDiffuse = color.pack_abgr8888();
	point[2].m_dwDiffuse = color.pack_abgr8888();
	point[3].m_dwDiffuse = color.pack_abgr8888();

	m_vecScreenBoxVertexBuffer.push_back( point[0] );
	m_vecScreenBoxVertexBuffer.push_back( point[2] );
	m_vecScreenBoxVertexBuffer.push_back( point[1] );
	m_vecScreenBoxVertexBuffer.push_back( point[1] );
	m_vecScreenBoxVertexBuffer.push_back( point[2] );
	m_vecScreenBoxVertexBuffer.push_back( point[3] );
}

void gkAuxRendererGLES2::AuxRenderScreenBox( const Vec2& pos, const Vec2& wh, const ColorB& color, ITexture* texture )
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
    
    if( texture->dynamic() )
    {
        point[0].m_vTexcorrd = Vec2(0,1);
        point[1].m_vTexcorrd = Vec2(1,1);
        point[2].m_vTexcorrd = Vec2(0,0);
        point[3].m_vTexcorrd = Vec2(1,0);
    }
    
    m_vecScreenBoxTexturedVertexBuffer.push_back( point[0] );
    m_vecScreenBoxTexturedVertexBuffer.push_back( point[2] );
    m_vecScreenBoxTexturedVertexBuffer.push_back( point[1] );
    m_vecScreenBoxTexturedVertexBuffer.push_back( point[1] );
    m_vecScreenBoxTexturedVertexBuffer.push_back( point[2] );
    m_vecScreenBoxTexturedVertexBuffer.push_back( point[3] );
    
    m_vecScreenBoxTexture.push_back( texture );

}
void gkAuxRendererGLES2::apply_2dvert_layout(bool rendering)
{
	if(rendering)
	{
#ifdef RENDERAPI_GL330
		return;
#else
		if (gkGLExtension::EXT_VAO)
		{
			return;
		}
#endif
	}
	glEnableVertexAttribArray(AUX_VERTEX_ARRAY);
	glEnableVertexAttribArray(AUX_COLOR_ARRAY);
	glVertexAttribPointer(AUX_VERTEX_ARRAY, 3, GL_FLOAT, GL_FALSE, sizeof(GK_HELPER_2DVERTEX), GLF_BUFFER_OFFSET(0));
	glVertexAttribPointer(AUX_COLOR_ARRAY, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(GK_HELPER_2DVERTEX), GLF_BUFFER_OFFSET(sizeof(Vec3)) );
}
void gkAuxRendererGLES2::apply_2dvert_texed_layout(bool rendering)
{
	if(rendering)
	{
#ifdef RENDERAPI_GL330
		return;
#else
		if (gkGLExtension::EXT_VAO)
		{
			return;
		}
#endif
	}
	glEnableVertexAttribArray(AUX_VERTEX_ARRAY);
	glEnableVertexAttribArray(AUX_COLOR_ARRAY);
	glVertexAttribPointer(AUX_VERTEX_ARRAY, 3, GL_FLOAT, GL_FALSE, sizeof(GK_HELPER_2DVERTEX_TEXTURED), GLF_BUFFER_OFFSET(0));
	glVertexAttribPointer(AUX_TEXCOORD_ARRAY, 2, GL_FLOAT, GL_FALSE, sizeof(GK_HELPER_2DVERTEX_TEXTURED), GLF_BUFFER_OFFSET(sizeof(Vec3)) );
}
