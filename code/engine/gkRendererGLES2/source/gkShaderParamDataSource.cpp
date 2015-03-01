#include "StableHeader.h"
#include "gkShaderParamDataSource.h"
#include "gkRenderable.h"
#include "gk_Camera.h"
#include "ICamera.h"
#include "../RenderCamera.h"
#include "gkRendererGLES2.h"

//-----------------------------------------------------------------------
gkShaderParamDataSource::gkShaderParamDataSource()
: m_pCurrentRenderable(NULL)
, m_pCurrentCamera(NULL)
, m_bIsWorldMatrixOutOfDate(true)
, m_bIsViewMatrixOutOfDate(true)
, m_bIsProjectionMatrixOutOfDate(true)
, m_bIsWorldViewMatrixOutOfDate(true)
, m_bIsViewProjMatrixOutOfDate(true)
, m_bIsWorldViewProjMatrixOutOfDate(true)
, m_bIsInverseWorldMatrixOutOfDate(true)
, m_bIsInverseWorldViewMatrixOutOfDate(true)
, m_bIsInverseViewMatrixOutOfDate(true)
, m_bIsInverseTransposeWorldMatrixOutOfDate(true)
, m_bIsInverseTransposeWorldViewMatrixOutOfDate(true)
, m_bIsCameraPositionOutOfDate(true)
, m_bIsCameraPositionObjectSpaceOutOfDate(true)
{
	m_pCurrentCamera = new CRenderCamera;
	m_pShadowsCamera[0] = new CRenderCamera;
	m_pShadowsCamera[1] = new CRenderCamera;
	m_pShadowsCamera[2] = new CRenderCamera;
}
//-----------------------------------------------------------------------
gkShaderParamDataSource::~gkShaderParamDataSource()
{
	SAFE_DELETE( m_pCurrentCamera );
	SAFE_DELETE( m_pShadowsCamera[0] );
	SAFE_DELETE( m_pShadowsCamera[1] );
	SAFE_DELETE( m_pShadowsCamera[2] );
}
//-----------------------------------------------------------------------
void gkShaderParamDataSource::setCurrentRenderable( const gkRenderable* rend )
{
	m_pCurrentRenderable = rend;

	// 有关world的矩阵全部过期
	m_bIsWorldMatrixOutOfDate = true;
	m_bIsWorldViewMatrixOutOfDate = true;
	m_bIsWorldViewProjMatrixOutOfDate = true;
	m_bIsInverseWorldMatrixOutOfDate = true;
	m_bIsInverseWorldViewMatrixOutOfDate = true;
	m_bIsInverseTransposeWorldMatrixOutOfDate = true;
	m_bIsInverseTransposeWorldViewMatrixOutOfDate = true;
}
//-----------------------------------------------------------------------
void gkShaderParamDataSource::setWorldMatrices( const Matrix44* m )
{
	m_matWorldMatrix = *m;
	
	m_bIsWorldMatrixOutOfDate = false;
}
//-----------------------------------------------------------------------
void gkShaderParamDataSource::setMainCamera( const CCamera* cam )
{
	const Matrix34& m34 = cam->GetMatrix();
	m_pCam = cam;
	CRenderCamera c;

 	  float Near = cam->GetNearPlane(), Far = cam->GetFarPlane();
 	  float wT = tanf(cam->GetFov()*0.5f)*Near, wB = -wT;
 	  float wR = wT * cam->GetProjRatio(), wL = -wR;
 	  m_pCurrentCamera->Frustum( wL + cam->GetAsymL(), wR + cam->GetAsymR(), wB + cam->GetAsymB(), wT + cam->GetAsymT(), Near, Far );
 
 	  Vec3 vEye = cam->GetPosition();
 	  Vec3 vAt = vEye + Vec3(m34(0,1), m34(1,1), m34(2,1));
 	  Vec3 vUp = Vec3(m34(0,2), m34(1,2), m34(2,2));
 	  m_pCurrentCamera->LookAt(vEye, vAt, vUp);

	  m_vCamDir = (vAt - vEye).GetNormalized();
	  m_vCamPos = vEye;
    
    
    // update the far clip
    Vec3 lookDir = vAt - vEye;
    lookDir.normalize();
    Vec3 m_farCenter = lookDir * Far;
    Vec3 rightDir = vUp % lookDir;
    rightDir.normalize();
    Vec3 headDir = lookDir % rightDir;
    headDir.normalize();
    
    float h = tanf( cam->GetFov() * 0.5f ) * Far;
    float w = h * cam->GetProjRatio();
    
    rightDir *= w;
    headDir *=  h;
    
    m_ScreenFarVerts[0] = Vec3(m_farCenter + rightDir - headDir);
    m_ScreenFarVerts[1] = Vec3( m_farCenter - rightDir - headDir);
    m_ScreenFarVerts[2] = Vec3( m_farCenter + rightDir + headDir);
    m_ScreenFarVerts[3] = Vec3( m_farCenter - rightDir + headDir);
    
    m_viewparam = Vec4( getRenderer()->GetScreenWidth(), getRenderer()->GetScreenHeight(), cam->GetNearPlane(), cam->GetFarPlane());
 	
 	// 有关view的矩阵全部过期
 	m_bIsViewMatrixOutOfDate = true;
 	m_bIsProjectionMatrixOutOfDate = true;
 	m_bIsWorldViewMatrixOutOfDate = true;
 	m_bIsWorldViewProjMatrixOutOfDate = true;
 	m_bIsInverseViewMatrixOutOfDate = true;
 	m_bIsInverseWorldViewMatrixOutOfDate = true;
 	m_bIsInverseTransposeWorldViewMatrixOutOfDate = true;


	// backup the main cam's matrix here [10/25/2011 Kaiming]

// 
	m_matBackupViewMatrix = getViewMatrix();
	m_matBackupViewMatrix.Transpose();

	m_matBackupProjMatrix = getProjectionMatrix();
	m_matBackupProjMatrix.Transpose();

	m_matBackupViewProjMatrix = m_matBackupProjMatrix * m_matBackupViewMatrix;
}

void gkShaderParamDataSource::setShadowCascadeCamera( const CCamera* cam, uint8 index )
{
	const Matrix34& m34 = cam->GetMatrix();

 	  float Near = cam->GetNearPlane(), Far = cam->GetFarPlane();
 	  float wT = tanf(cam->GetFov()*0.5f)*Near, wB = -wT;
 	  float wR = wT * cam->GetProjRatio(), wL = -wR;
 	  m_pShadowsCamera[index]->Frustum( wL + cam->GetAsymL(), wR + cam->GetAsymR(), wB + cam->GetAsymB(), wT + cam->GetAsymT(), Near, Far );
 
 	  Vec3 vEye = cam->GetPosition();
 	  Vec3 vAt = vEye + Vec3(m34(0,1), m34(1,1), m34(2,1));
 	  Vec3 vUp = Vec3(m34(0,2), m34(1,2), m34(2,2));
 	  m_pShadowsCamera[index]->LookAt(vEye, vAt, vUp);
 
}

//-----------------------------------------------------------------------
// void gkShaderParamDataSource::setCurrentRenderTarget( const gkRenderTarget* target )
// {
// 	m_pCurrentRenderTarget = target;
// }
//-----------------------------------------------------------------------
const Matrix44& gkShaderParamDataSource::getWorldMatrix( void ) const
{
	if (m_bIsWorldMatrixOutOfDate)
	{
		m_pCurrentRenderable->getWorldTransforms(&m_matWorldMatrix);
		m_bIsWorldMatrixOutOfDate = false;
	}
	return m_matWorldMatrix;
}
//-----------------------------------------------------------------------
const Matrix44& gkShaderParamDataSource::getViewMatrix( void ) const
{
	if (m_bIsViewMatrixOutOfDate)
	{
		m_pCurrentCamera->GetModelviewMatrix((float*)(&m_matViewMatrix));
		m_bIsViewMatrixOutOfDate = false;
	}
	return m_matViewMatrix;
}
//-----------------------------------------------------------------------
const Matrix44& gkShaderParamDataSource::getProjectionMatrix( void ) const
{
	if (m_bIsProjectionMatrixOutOfDate)
	{
		// hacking way of ortho frustum [10/4/2011 Kaiming]
		//if (m_pCam->GetFov() > 0.15f)
			m_pCurrentCamera->GetProjectionMatrix((float*)(&m_matProjectionMatrix));

			// MSAA process [11/27/2011 Kaiming]
			if( g_pRendererCVars->r_PostMsaa )
			{
//				static int nFrameID = -1;
				static Vec3 pOffset = Vec3(0,0,0);
// 				if( nFrameID != gEnv->p )
// 				{
// 					Vec2 offs[4] =
// 					{
// 						Vec2(0.96f,0.25f),
// 						Vec2(-0.25f,0.96f),
// 						Vec2(-0.96f,-0.25f),
// 						Vec2(0.25f,-0.96f),
// 					};

				// cr2
					Vec2 offs[2] =
					{
						Vec2(-1.f,-1.f),
						Vec2(1.f,1.f),
// 						Vec2(-0.96f,-0.25f),
// 						Vec2(0.25f,-0.96f),
					};

					int nCurrID = gEnv->pProfiler->getFrameCount() % 2; // select 2x msaa or 4x msaa
					pOffset.x = (offs[nCurrID].x / (float)gEnv->pRenderer->GetScreenWidth()) * .5f;
					pOffset.y = (offs[nCurrID].y / (float)gEnv->pRenderer->GetScreenHeight()) * .5f;
// 					nFrameID = GetFrameID();
// 				}

				m_matProjectionMatrix.m20 +=pOffset.x;
				m_matProjectionMatrix.m21 +=pOffset.y;
			}
		m_bIsProjectionMatrixOutOfDate = false;
	}
	return m_matProjectionMatrix;
}
//-----------------------------------------------------------------------
const Matrix44& gkShaderParamDataSource::getInverseViewMatrix( void ) const
{
	if (m_bIsInverseViewMatrixOutOfDate)
	{
		m_matInverseViewMatrix = getViewMatrix().GetInverted();
		m_bIsInverseViewMatrixOutOfDate = false;
	}
	return m_matInverseViewMatrix;
}
//-----------------------------------------------------------------------
const Matrix44& gkShaderParamDataSource::getInverseWorldMatrix( void ) const
{
	if (m_bIsInverseWorldMatrixOutOfDate)
	{
		m_matInverseWorldMatrix = getWorldMatrix().GetInverted();
		m_bIsInverseWorldMatrixOutOfDate = false;
	}
	return m_matInverseWorldMatrix;	
}
//-----------------------------------------------------------------------
const Matrix44& gkShaderParamDataSource::getWorldViewProjMatrix( void ) const
{
	if (m_bIsWorldViewProjMatrixOutOfDate)
	{
		m_matWorldViewProjMatrix = getWorldViewMatrix() * getProjectionMatrix();
		m_bIsWorldViewProjMatrixOutOfDate = false;
	}
	return m_matWorldViewProjMatrix;
}
//-----------------------------------------------------------------------
const Matrix44& gkShaderParamDataSource::getWorldViewMatrix( void ) const
{
	if (m_bIsWorldViewMatrixOutOfDate)
	{
		m_matWorldViewMatrix = getWorldMatrix() * getViewMatrix();
		m_bIsWorldViewMatrixOutOfDate = false;
	}
	return m_matWorldViewMatrix;
}

//////////////////////////////////////////////////////////////////////////
const Matrix44& gkShaderParamDataSource::getViewProjMatrix( void ) const
{
	if (m_bIsViewProjMatrixOutOfDate)
	{
		m_matViewProjMatrix = getViewMatrix() * getProjectionMatrix();
		m_bIsViewProjMatrixOutOfDate = false;
	}
	return m_matViewProjMatrix;
}

//-----------------------------------------------------------------------
void gkShaderParamDataSource::setLightDir( const Vec3& lightdir )
{
	m_vLightDir = lightdir.GetNormalized();/*.GetRotated(Vec3(1,0,0), -g_PI * 0.5f);*/
	Matrix44 ViewT = getViewMatrix();
	ViewT.Transpose();
	m_vLightDirInViewSpace = ViewT.TransformVector(m_vLightDir);
}

const Matrix44& gkShaderParamDataSource::getViewMatrix_ShadowCascade( uint8 index ) const
{
	if (index < 3)
	{
		m_pShadowsCamera[index]->GetModelviewMatrix((float*)(&(m_matViewMatrix_ShadowCascade[index])));
	}
	return m_matViewMatrix_ShadowCascade[index];
}

const Matrix44& gkShaderParamDataSource::getProjectionMatrix_ShadowCascade( uint8 index ) const
{
	if (index < 3)
	{
		m_pShadowsCamera[index]->GetProjectionMatrix((float*)(&(m_matProjectionMatrix_ShadowCascade[index])));
	}
	return m_matProjectionMatrix_ShadowCascade[index];
}

const Matrix44& gkShaderParamDataSource::getWorldViewProjMatrix_ShadowCascade( uint8 index ) const
{
	if (index < 3)
	{
		m_matWorldViewProjMatrix_ShadowCascade[index] = getWorldViewMatrix_ShadowCascade(index) * getProjectionMatrix_ShadowCascade(index);
	}
	return m_matWorldViewProjMatrix_ShadowCascade[index];	
}

const Matrix44& gkShaderParamDataSource::getWorldViewMatrix_ShadowCascade( uint8 index ) const
{
	if (index < 3)
	{
		m_matWorldViewMatrix_ShadowCascade[index] = getWorldMatrix() * getViewMatrix_ShadowCascade(index);
	}
	return m_matWorldViewMatrix_ShadowCascade[index];	
}

const Matrix44& gkShaderParamDataSource::getInverseViewMatrix_ShadowCascade( uint8 index ) const
{
	if (index < 3)
	{
		m_matInverseViewMatrix_ShadowCascade[index] = getViewMatrix_ShadowCascade(index).GetInverted();
	}
	return m_matInverseViewMatrix_ShadowCascade[index];	
}

// TODO: 余下的取值函数以后补全
