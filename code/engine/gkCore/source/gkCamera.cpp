#include "gkStableHeader.h"
#include "gkCamera.h"
#include "gkNode.h"
//#include "gkD3DMathExtended.h"
#include "gk3DEngine.h"
#include "gkSceneManager.h"

// 定义moveable类型为camera
gkStdString gkCamera::m_wstrMovableType = _T("Camera");

gkCamera::gkCamera( const gkStdString& name)
:	m_wstrName( name )
,	m_pCullFrustum(NULL)
,	m_yHideMask(GK_CAMERA_VIEWMASK_DEFAUT)
{
	// Reasonable defaults to camera params
	m_fFOV = gf_PI * 68.0f / 180.0f;
	m_fNearDist = 0.25f;
	m_fFarDist = 4000.0f;
	m_fOrthoHeight = 100.0f;
	m_fAspect = 1.7778f;
	m_yProjType = PT_PERSPECTIVE;
	setFixedYawAxis(true);    // Default to fixed yaw, like freelook since most people expect this

	m_vPosition = Vec3(0,0,0);
	m_qOrientation = Quat(1, 0, 0, 0); 

	m_vDerivedPosition = Vec3(0,0,0);
	m_qDerivedOrientation = Quat(1, 0, 0, 0); 

	invalidateFrustum();
	invalidateView();

	m_pParentNode = NULL;

	m_bIsLightCamera = false;

	m_stereoOffset[0] = Vec3(0,0,0);
	m_stereoOffset[1] = Vec3(0,0,0);

	m_stereoOrientation = Quat(1, 0, 0, 0);
}

//-----------------------------------------------------------------------
gkCamera::~gkCamera()
{
	// do nothing
}

float gkCamera::getNearClipDistance( void ) const
{
	if (m_pCullFrustum)
	{
		return m_pCullFrustum->getNearClipDistance();
	}
	else
	{
		return gkFrustum::getNearClipDistance();
	}
}

float gkCamera::getFarClipDistance( void ) const
{
	if (m_pCullFrustum)
	{
		return m_pCullFrustum->getFarClipDistance();
	}
	else
	{
		return gkFrustum::getFarClipDistance();
	}
}

//-----------------------------------------------------------------------
bool gkCamera::isViewOutOfDate( void ) const
{
	// 覆盖gkFrustum的方法
	if (m_pParentNode != 0)
	{
		if (m_bRecalcView ||
			m_pParentNode->_getDerivedOrientation() != m_qLastParentOrientation ||
			m_pParentNode->_getDerivedPosition() != m_vLastParentPosition)
		{
			// 缓存过期，需要重新计算
			m_qLastParentOrientation = m_pParentNode->_getDerivedOrientation();
			m_vLastParentPosition = m_pParentNode->_getDerivedPosition();
			m_qRealOrientation = m_qLastParentOrientation * m_qOrientation;
// 			D3DXQUATERNION qInverse;
// 			D3DXQuaternionInverse(&qInverse, &m_qLastParentOrientation);
			m_vRealPosition = m_vPosition * m_qLastParentOrientation + m_vLastParentPosition;
			m_bRecalcView = true;
		}

		// 重新计算一下ORTHO的前后片
// 		if(m_bIsLightCamera)
// 		{
//  			float fDist = m_pParentNode->_getDerivedPosition().GetLength();
//  			m_fNearDist = fDist - 50;
//  			m_fFarDist = fDist + 50;
// 
// 			gkCamera* mainCam = getSceneMngPtr()->getMainCamera();
// 			if (mainCam)
// 			{
// 				 float dist = m_pParentNode->_getDerivedPosition().GetDistance(mainCam->getPosition());
// 				 m_fNearDist = dist - 50;
// 				 m_fFarDist = dist + 550;
// 			}
// 		}
		 
	}
	else
	{
		// 没有附着点，自己计算
		m_qRealOrientation = m_qOrientation;
		m_vRealPosition = m_vPosition;
	}

	// Deriving reflected orientation / position
	if (m_bRecalcView)
	{
		m_qDerivedOrientation = m_qRealOrientation;
		m_vDerivedPosition = m_vRealPosition;
	}

	return m_bRecalcView;
}
//-----------------------------------------------------------------------
void gkCamera::_renderScene()
{
// 	if (m_bIsLightCamera)
// 	{
// 		gk3DEngine::getSingletonPtr()->getSceneMngPtr()->_prepareRenderSequences(this);
// 		gk3DEngine::getSingletonPtr()->getSceneMngPtr()->_renderRenderSequences();
// 	}
// 	else
// 	{
//		gk3DEngine::getSingletonPtr()->getSceneMngPtr()->_prepareRenderSequences(this);
//	}

}

//-----------------------------------------------------------------------
const gkStdString& gkCamera::getName( void ) const
{
	return m_wstrName;
}

//-----------------------------------------------------------------------
void gkCamera::setPosition( float x, float y, float z )
{
	m_vPosition.x = x;
	m_vPosition.y = y;
	m_vPosition.z = z;
	invalidateView();
}

//-----------------------------------------------------------------------
void gkCamera::setPosition( const Vec3& vec )
{
	m_vPosition = vec;
	invalidateView();
}

//-----------------------------------------------------------------------
const Vec3& gkCamera::getPosition( void ) const
{
	return m_vPosition;
}

//-----------------------------------------------------------------------
void gkCamera::move( const Vec3& vec )
{
	m_vPosition = m_vPosition + vec;
	invalidateView();
}

//-----------------------------------------------------------------------
void gkCamera::moveLocal( const Vec3& vec )
{
	// Transform the axes of the relative vector by camera's local axes
	Vec3 trans = m_qOrientation * vec;
	m_vPosition = m_vPosition + trans;
	invalidateView();
}

//-----------------------------------------------------------------------
void gkCamera::setDirection( const Vec3& vec )
{
	// 如果向量为0向量，非法
	if (vec.GetLengthFast() < 0.0001f) return;

	// Remember, camera points down -Z of local axes!
	// Therefore reverse direction of direction vector before determining local Z
	Vec3 zAdjustVec = vec;
	zAdjustVec.Normalize();

	Quat targetWorldOrientation;

	if( m_bYawFixed )
	{
		targetWorldOrientation = Quat::CreateRotationVDir( zAdjustVec );
		targetWorldOrientation.Normalize();
	}
	else
	{
		assert(0);

		return;

#if 0 
		// Get axes from current quaternion
		Vec3 axes[3];
		updateView();
		gkMathQuaternionToAxes(&m_qRealOrientation, axes);
		Quat rotQuat;
		if ( D3DXVec3LengthSq(&(axes[2]+zAdjustVec)) <  0.00005f) 
		{
			// 悲剧，这是个180度转向
			// 直接指向Y轴
			D3DXQuaternionRotationAxis(&rotQuat, &(axes[1]), D3DX_PI);
		}
		else
		{
			// Derive shortest arc to new direction
			rotQuat = gkMathVec3ShortRotationTo(&(axes[2]),&zAdjustVec);
		}
		targetWorldOrientation = rotQuat * m_qRealOrientation;
#endif
	}

	// 转换到parent坐标系
	if (m_pParentNode)
	{
		m_qOrientation = m_pParentNode->_getDerivedOrientation();
		m_qOrientation.Invert();
		m_qOrientation = m_qOrientation * targetWorldOrientation;
	}
	else
	{
		m_qOrientation = targetWorldOrientation;
	}

	// TODO If we have a fixed yaw axis, we mustn't break it by using the
	// shortest arc because this will sometimes cause a relative yaw
	// which will tip the camera

	invalidateView();
}

//-----------------------------------------------------------------------
void gkCamera::setDirection( float x, float y, float z )
{
	setDirection(Vec3(x,y,z));
}

//-----------------------------------------------------------------------
Vec3 gkCamera::getDirection( void ) const
{
	// 用Z轴来给出朝向
	return m_qOrientation * Vec3(0,-1,0) ; 
}

//-----------------------------------------------------------------------
Vec3 gkCamera::getUp( void ) const
{
	// 用Y轴来给出朝向
	return Vec3(0,1,0) * m_qOrientation; 
}

//-----------------------------------------------------------------------
Vec3 gkCamera::getRight( void ) const
{
	// 用X轴来给出朝向
	return Vec3(1,0,0) * m_qOrientation; 
}

//-----------------------------------------------------------------------
const Quat& gkCamera::getDerivedOrientation(void) const
{
	updateView();
	return m_qDerivedOrientation;
}
//-----------------------------------------------------------------------
const Vec3& gkCamera::getDerivedPosition(void) const
{
	updateView();
	return m_vDerivedPosition;
}
//-----------------------------------------------------------------------
Vec3 gkCamera::getDerivedDirection(void) const
{
	// Direction points down -Z
	updateView();
	return m_qDerivedOrientation * Vec3(0,-1,0); 
}
//-----------------------------------------------------------------------
Vec3 gkCamera::getDerivedUp(void) const
{
	updateView();
	return m_qDerivedOrientation * Vec3(0,0,1); 
}
//-----------------------------------------------------------------------
Vec3 gkCamera::getDerivedRight(void) const
{
	updateView();
	return m_qDerivedOrientation * Vec3(1,0,0); 
}
//-----------------------------------------------------------------------
void gkCamera::lookAt( const Vec3& targetPoint )
{
	updateView();
	this->setDirection(targetPoint - m_vRealPosition);
}

//-----------------------------------------------------------------------
void gkCamera::lookAt( float x, float y, float z )
{
	Vec3 vTemp( x, y, z );
	this->lookAt(vTemp);
}

//-----------------------------------------------------------------------
void gkCamera::setFixedYawAxis( bool useFixed, const Vec3& fixedAxis /*= D3DXVECTOR3(0,1,0) */ )
{
	m_bYawFixed = useFixed;
	m_vYawFixedAxis = fixedAxis;
}
//-----------------------------------------------------------------------
const Quat& gkCamera::getOrientation( void ) const
{
	return m_qOrientation;
}
//-----------------------------------------------------------------------
void gkCamera::setOrientation( const Quat& q )
{
	m_qOrientation = q;
	m_qOrientation.Normalize();
	//D3DXQuaternionNormalize(&m_qOrientation, &m_qOrientation);
	invalidateView();
}
//-----------------------------------------------------------------------
void gkCamera::synchroniseBaseSettingsWith( const gkCamera* cam )
{
	this->setPosition(cam->getDerivedPosition());
	this->setProjectionType(cam->getProjectionType());
	this->setOrientation(cam->getDerivedOrientation());
	this->setAspect(cam->getAspect());
	this->setNearClipDistance(cam->getNearClipDistance());
	this->setFarClipDistance(cam->getFarClipDistance());
	this->setFOVy(cam->getFOVy());
}

void gkCamera::synchroniseBaseSettingsWith( ICamera* cam )
{
	synchroniseBaseSettingsWith(static_cast<gkCamera*>(cam));
}

//-----------------------------------------------------------------------
const gkStdString& gkCamera::getMovableType( void ) const
{
	return m_wstrMovableType;
}

void gkCamera::setNearPlane( float zNear )
{
	gkFrustum::setNearClipDistance(zNear);
}

void gkCamera::setFarPlane( float zFar )
{
	gkFrustum::setFarClipDistance(zFar);
}

float gkCamera::getNearPlane()
{
	return gkFrustum::getNearClipDistance();
}

float gkCamera::getFarPlane()
{
	return gkFrustum::getFarClipDistance();
}

void gkCamera::setStereoOffset(Vec3 leftEye, Vec3 rightEye)
{
	m_stereoOffset[0] = leftEye;
	m_stereoOffset[1] = rightEye;
}

void gkCamera::getStereoOffset(Vec3& leftEye, Vec3& rightEye)
{
	leftEye = m_stereoOffset[0];
	rightEye = m_stereoOffset[1];
}

void gkCamera::setAdditionalOrientation(Quat orien)
{
	m_stereoOrientation = orien;
	invalidateView();
}

void gkCamera::setStereoProjMatrix(Matrix44 leftMat, Matrix44 rightMat)
{
	m_stereoProjMatrix[0] = leftMat;
	m_stereoProjMatrix[1] = rightMat;
}
void gkCamera::getStereoProjMatrix(Matrix44& leftMat, Matrix44& rightMat)
{
	leftMat = m_stereoProjMatrix[0];
	rightMat = m_stereoProjMatrix[1];
}

