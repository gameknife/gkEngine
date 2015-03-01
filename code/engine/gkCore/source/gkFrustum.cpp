#include "gkStableHeader.h"
#include "gkFrustum.h"

#include "gkNode.h"
#include "gk_Camera.h"
#include "IRenderer.h"
//#include "gkD3DMathExtended.h"

///
gkFrustum::gkFrustum():m_yProjType(PT_PERSPECTIVE)
						,m_fFOV(gf_PI * 0.25f)
						,m_fFarDist(2000.0f)
						,m_fNearDist(0.025f)
						,m_fAspect(1.7778f)
						,m_fOrthoHeight(500.0f)
						,m_fFocalLength(1.0f)
						,m_bRecalcFrustum(true)
						,m_bRecalcView(true)
						,m_bRecalcFrustumPlanes(true)
{
	m_qLastParentOrientation.SetIdentity();
	m_vLastParentPosition = Vec3(0,0,0);
	
	// Alter superclass members
	m_pParentNode = NULL;

	updateView();
	updateFrustum();
}

gkFrustum::~gkFrustum()
{

}

void gkFrustum::setFOVy( float fovy )
{
	m_fFOV = fovy;
	invalidateFrustum();
}

float gkFrustum::getFOVy( void ) const
{
	return m_fFOV;
}

//-----------------------------------------------------------------------
void gkFrustum::setFarClipDistance(float farPlane)
{
	m_fFarDist = farPlane;
	invalidateFrustum();
}

//-----------------------------------------------------------------------
float gkFrustum::getFarClipDistance(void) const
{
	return m_fFarDist;
}

//-----------------------------------------------------------------------
void gkFrustum::setNearClipDistance(float nearPlane)
{
	if (nearPlane <= 0)
	{
		gkLogError( _T("Near clip distance must be greater than zero.") );
	}
	m_fNearDist = nearPlane;
	invalidateFrustum();
}

//-----------------------------------------------------------------------
float gkFrustum::getNearClipDistance(void) const
{
	return m_fNearDist;
}

//-----------------------------------------------------------------------
void gkFrustum::setAspect( float aspect )
{
	m_fAspect = aspect;
	invalidateFrustum();
}
//-----------------------------------------------------------------------
float gkFrustum::getAspect( void ) const
{
	return m_fAspect;
}

void gkFrustum::invalidateFrustum() const
{
	m_bRecalcFrustum = true;
	m_bRecalcFrustumPlanes = true;
}
//-----------------------------------------------------------------------
void gkFrustum::invalidateView( void ) const
{
	m_bRecalcView = true;
	m_bRecalcFrustumPlanes = true;
}

//-----------------------------------------------------------------------
bool gkFrustum::isViewOutOfDate(void) const
{
	// Attached to node?
	if (m_pParentNode)
	{
		if (m_bRecalcView ||
			m_pParentNode->_getDerivedOrientation() != m_qLastParentOrientation ||
			m_pParentNode->_getDerivedPosition() != m_vLastParentPosition)
		{
			// Ok, we're out of date with SceneNode we're attached to
			m_qLastParentOrientation = m_pParentNode->_getDerivedOrientation();
			m_vLastParentPosition = m_pParentNode->_getDerivedPosition();
			m_bRecalcView = true;
		}
	}

	return m_bRecalcView;
}

//-----------------------------------------------------------------------
bool gkFrustum::isFrustumOutOfDate(void) const
{
	return m_bRecalcFrustum;
}

//-----------------------------------------------------------------------
void gkFrustum::updateFrustum( void ) const
{
	if (m_bRecalcFrustum)
	{
		updateFrustumImpl();
	}
}

//-----------------------------------------------------------------------
void gkFrustum::updateFrustumImpl( void ) const
{
	// ----------------------
	// 更新投射矩阵
	// ----------------------

	// 构造projMatrix
	// remark 以后可能使用自定义视图，所以这里留一个判断

	if (true)
	{
		if ( m_yProjType == PT_PERSPECTIVE )
		{
			m_testFrustum.SetFrustum(gEnv->pRenderer->GetScreenWidth(), gEnv->pRenderer->GetScreenHeight(), m_fFOV, m_fNearDist, m_fFarDist );
		}
		else if (m_yProjType == PT_ORTHOGRAPHIC)
		{
			m_testFrustum.SetFrustum(m_fOrthoHeight,m_fOrthoHeight, m_fFOV, m_fNearDist, m_fFarDist );
		}
	}

	m_bRecalcFrustum = false;

	// 通知更新视锥的6个面
	m_bRecalcFrustumPlanes = true;
}

//-----------------------------------------------------------------------
void gkFrustum::updateView( void ) const
{
	if (isViewOutOfDate())
	{
		updateViewImpl();
		updateFrustumPlanes();
	}

	if(isFrustumOutOfDate())
	{
		updateFrustum();
	}
}

//-----------------------------------------------------------------------
void gkFrustum::updateViewImpl( void ) const
{
	// ----------------------
	// 更新视图矩阵
	// ----------------------
	m_bRecalcView = false;

	// 构造viewMatrix
	// remark 以后可能使用自定义视图，所以这里留一个判断
	if (true)
	{
		Matrix34 mat;
		// 
		mat.Set(Vec3(1,1,1), getOrientationForViewUpdate(), getPositionForViewUpdate());

		m_testFrustum.SetMatrix(mat);
	}

	// Signal to update frustum clipping planes
	m_bRecalcFrustumPlanes = true;
}

//-----------------------------------------------------------------------
void gkFrustum::updateFrustumPlanes( void ) const
{
	updateView();
	updateFrustum();

	if (m_bRecalcFrustumPlanes)
	{
		updateFrustumPlanesImpl();
	}
}
//-----------------------------------------------------------------------
void gkFrustum::updateFrustumPlanesImpl( void ) const
{
// 	Matrix44 matT = m_matViewMatrix;
//     	matT.m01 = matT.m01 + matT.m02;
//     	matT.m02 = matT.m01 - matT.m02;
//     	matT.m01 = matT.m01 - matT.m02;
//     
//     	matT.m11 = matT.m11 + matT.m12;
//     	matT.m12 = matT.m11 - matT.m12;
//     	matT.m11 = matT.m01 - matT.m12;
//     
//     	matT.m21 = matT.m21 + matT.m22;
//     	matT.m22 = matT.m21 - matT.m22;
//     	matT.m21 = matT.m21 - matT.m22;
//    
//     	matT.m31 = matT.m31 + matT.m32;
//     	matT.m32 = matT.m31 - matT.m32;
//     	matT.m31 = matT.m31 - matT.m32;
 
 // 	matT.m20 *= -1;
 // 	matT.m21 *= -1;
 // 	matT.m22 *= -1;
 
 
 // 	matT.m30 *= -1;
 // 	matT.m31 *= -1;
 // 	matT.m32 *= -1;
 
 //	matT.Transpose();
 //	Matrix34 mat(matT);


	// -------------------------
	// Update the frustum planes
	// -------------------------

	// D3D应该是右乘 [10/22/2010 Kaiming-Desktop]
// 	Matrix44 combo = m_matViewMatrix * m_matProjMatrix;
// 
// 	m_plFrustumPlanes[FRUSTUM_PLANE_LEFT].n = Vec3(combo.m30 + combo.m00, combo.m31 + combo.m01, combo.m32 + combo.m02);
// 	m_plFrustumPlanes[FRUSTUM_PLANE_LEFT].d = combo.m33 + combo.m03;
// 
// 	m_plFrustumPlanes[FRUSTUM_PLANE_RIGHT].n = Vec3(combo.m30 - combo.m00, combo.m31 - combo.m01, combo.m32 - combo.m02);
// 	m_plFrustumPlanes[FRUSTUM_PLANE_RIGHT].d = combo.m33 - combo.m03;

// 	m_plFrustumPlanes[FRUSTUM_PLANE_TOP].a = combo._41 - combo._21;
// 	m_plFrustumPlanes[FRUSTUM_PLANE_TOP].b = combo._42 - combo._22;
// 	m_plFrustumPlanes[FRUSTUM_PLANE_TOP].c = combo._43 - combo._23;
// 	m_plFrustumPlanes[FRUSTUM_PLANE_TOP].d = combo._44 - combo._24;
// 
// 	m_plFrustumPlanes[FRUSTUM_PLANE_BOTTOM].a = combo._41 + combo._21;
// 	m_plFrustumPlanes[FRUSTUM_PLANE_BOTTOM].b = combo._42 + combo._22;
// 	m_plFrustumPlanes[FRUSTUM_PLANE_BOTTOM].c = combo._43 + combo._23;
// 	m_plFrustumPlanes[FRUSTUM_PLANE_BOTTOM].d = combo._44 + combo._24;
// 
// 	m_plFrustumPlanes[FRUSTUM_PLANE_NEAR].a = combo._41 + combo._31;
// 	m_plFrustumPlanes[FRUSTUM_PLANE_NEAR].b = combo._42 + combo._32;
// 	m_plFrustumPlanes[FRUSTUM_PLANE_NEAR].c = combo._43 + combo._33;
// 	m_plFrustumPlanes[FRUSTUM_PLANE_NEAR].d = combo._44 + combo._34;
// 
// 	m_plFrustumPlanes[FRUSTUM_PLANE_FAR].a = combo._41 - combo._31;
// 	m_plFrustumPlanes[FRUSTUM_PLANE_FAR].b = combo._42 - combo._32;
// 	m_plFrustumPlanes[FRUSTUM_PLANE_FAR].c = combo._43 - combo._33;
// 	m_plFrustumPlanes[FRUSTUM_PLANE_FAR].d = combo._44 - combo._34;

	// Renormalise any normals which were not unit length
// 	for(int i=0; i<6; i++ ) 
// 	{
// 		Vec3 tempV = m_plFrustumPlanes[i].n;
// 		float length = tempV.GetLength();
// 		m_plFrustumPlanes[i].d /= length;
// 	}

	m_bRecalcFrustumPlanes = false;
}

//-----------------------------------------------------------------------
void gkFrustum::setProjectionType(ProjectionType pt)
{
	m_yProjType = pt;
	invalidateFrustum();
}

//-----------------------------------------------------------------------
ProjectionType gkFrustum::getProjectionType(void) const
{
	return m_yProjType;
}

//-----------------------------------------------------------------------
void gkFrustum::getWorldTransforms( Matrix44* xform ) const
{
	if (m_pParentNode)
		*xform = m_pParentNode->_getFullTransform();
	else
		xform->SetIdentity();

	xform->Transpose();
}

//-----------------------------------------------------------------------
const gkMaterialPtr& gkFrustum::getMaterial( void ) const
{
	// TODO：以后实现可视化视锥
	static gkMaterialPtr nullPtr;
	return nullPtr;
}

//-----------------------------------------------------------------------
void gkFrustum::getRenderOperation( gkRenderOperation& op )
{
	// TODO：以后实现可视化视锥
}

//-----------------------------------------------------------------------
void gkFrustum::_updateRenderSequence( IRenderSequence* queue )
{
	// TODO：以后实现可视化视锥
}
//-----------------------------------------------------------------------
float gkFrustum::getSquaredViewDepth( const ICamera* cam ) const
{
	// TODO:
	return 0;
}
//-----------------------------------------------------------------------
const AABB& gkFrustum::getAABB( void ) const
{
	return m_WorldAABB;
}
//-----------------------------------------------------------------------
bool gkFrustum::checkRenderable( const AABB& aabb )
{
	return (CULL_EXCLUSION != m_testFrustum.IsAABBVisible_F(aabb));
}
