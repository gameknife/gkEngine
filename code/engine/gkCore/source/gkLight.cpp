#include "gkStableHeader.h"
#include "gkLight.h"
#include "gkNode.h"
#include "gkCamera.h"
//#include "gkD3DMathExtended.h"
#include "IRenderTarget.h"
#include "gkSceneManager.h"
#include "gk3DEngine.h"
#include "IRenderSequence.h"
//#include "gkd3d9rendersystem.h"
//#include "gkrendertexture.h"



//-----------------------------------------------------------------------
gkLight::gkLight( gkStdString name ): 
	gkMovableObject(name),
	m_yLightType(LT_DIRECTIONAL),
	m_vPosition(Vec3(0,0,0)),
	m_vDiffuse(ColorF(1,1,1,1)),
	m_vAnimatedDiffuse(ColorF(1,1,1,1)),
	m_vSpecular(ColorF(1,1,1,1)),
	m_vDirection(Vec3(0,0,1)),
	m_fLightPower(1.0f),
	mIndexInFrame(0),
	m_fShadowNearClipDist(-1),
	m_fShadowFarClipDist(-1),
	m_vDerivedPosition(Vec3(0,0,0)),
	m_vDerivedDirection(Vec3(0,0,1)),
	m_vDerivedCamRelativePosition(Vec3(0,0,0)),
	m_vDerivedCamRelativeDirection(Vec3(0,0,1)),
	m_bDerivedCamRelativeDirty(false),
	m_pCameraToBeRelativeTo(NULL),
	m_bDerivedTransformDirty(false),
	m_fRadius(5),
	m_bFakeShadow(false),
	m_bGlobalShadow(false),
	m_fIntensity(1),
	m_fIntensitySpec(1)
{

}
//-----------------------------------------------------------------------
gkLight::~gkLight( void )
{

}
//-----------------------------------------------------------------------
void gkLight::setLightType(gkLightTypes type)
{
	m_yLightType = type;
}
//-----------------------------------------------------------------------
gkLight::gkLightTypes gkLight::getLightType(void) const
{
	return m_yLightType;
}
//-----------------------------------------------------------------------
void gkLight::setPosition(float x, float y, float z)
{
	m_vPosition.x = x;
	m_vPosition.y = y;
	m_vPosition.z = z;
	m_bDerivedTransformDirty = true;
	m_bDerivedCamRelativeDirty = true;

}
//-----------------------------------------------------------------------
void gkLight::setPosition(const Vec3& vec)
{
	m_vPosition = vec;
	m_bDerivedTransformDirty = true;
	m_bDerivedCamRelativeDirty = true;
}
//-----------------------------------------------------------------------
const Vec3& gkLight::getPosition(void) const
{
	return m_vPosition;
}
//-----------------------------------------------------------------------
void gkLight::setDirection(float x, float y, float z)
{
	m_vDirection.x = x;
	m_vDirection.y = y;
	m_vDirection.z = z;
	m_bDerivedTransformDirty = true;
}
//-----------------------------------------------------------------------
void gkLight::setDirection(const Vec3& vec)
{
	m_vDirection = vec;
	m_bDerivedTransformDirty = true;
}
//-----------------------------------------------------------------------
const Vec3& gkLight::getDirection(void) const
{
	return m_vDirection;
}
//-----------------------------------------------------------------------
void gkLight::_updateRenderSequence( IRenderSequence* queue )
{
	// do nothing

	// add to scenemng's point light list [10/16/2011 Kaiming]
	gkRenderLight pRenderLight;
 
	pRenderLight.m_vPos = getDerivedPosition();
	pRenderLight.m_vDir = getDerivedDirection();
	pRenderLight.m_vDiffuse = getDiffuseColor() * m_fIntensity;
	pRenderLight.m_vSpecular = getDiffuseColor() * m_fIntensitySpec;
	pRenderLight.m_fRadius = getRadius();
	pRenderLight.m_bFakeShadow = getFakeShadow();
	pRenderLight.m_bGlobalShadow = getGlobalShadow();

	queue->addRenderLight(pRenderLight);

	m_pParent->setScale( m_fRadius, m_fRadius, m_fRadius );

}
//-----------------------------------------------------------------------
const gkStdString& gkLight::getMovableType() const
{
	return gkLightFactory::FACTORY_TYPE_NAME;
}
//-----------------------------------------------------------------------
const Vec3& gkLight::getDerivedPosition(bool cameraRelative) const
{
	update();
	if (cameraRelative && m_pCameraToBeRelativeTo)
	{
		return m_vDerivedCamRelativePosition;
	}
	else
	{
		return m_vDerivedPosition;
	}
}
//-----------------------------------------------------------------------
const Vec3& gkLight::getDerivedDirection(bool cameraRelative) const
{
	update();
	if (cameraRelative && m_pCameraToBeRelativeTo)
	{
		return m_vDerivedCamRelativeDirection;
	}	
	return m_vDerivedDirection;
}
//-----------------------------------------------------------------------
void gkLight::update( void ) const
{
	// 如果挂载了cam，直接取cam的位置和方向
	if (m_pCameraToBeRelativeTo /*&& m_bDerivedCamRelativeDirty*/)
	{
		m_vDerivedCamRelativePosition = m_pCameraToBeRelativeTo->getDerivedPosition();
		m_vDerivedCamRelativeDirection = m_pCameraToBeRelativeTo->getDerivedDirection();
		m_bDerivedCamRelativeDirty = false;

		return;
	}

	// 否则根据父节点更新

// 	if (m_bDerivedTransformDirty)
// 	{
	if (m_pParentNode)
	{
		// Ok, update with SceneNode we're attached to
		const Quat& parentOrientation = m_pParentNode->_getDerivedOrientation();
		const Vec3& parentPosition = m_pParentNode->_getDerivedPosition();
		m_vDerivedDirection = parentOrientation * m_vDirection;
		m_vDerivedPosition = parentOrientation * m_vPosition + parentPosition;
	}
	else
	{
		// 再否则，自己更新
		m_vDerivedPosition = m_vPosition;
		m_vDerivedDirection = m_vDirection;
	}

	m_bDerivedTransformDirty = false;
/*	}*/

}
//-----------------------------------------------------------------------
float gkLight::_deriveShadowNearClipDistance( const gkCamera* maincam ) const
{
	if (m_fShadowNearClipDist > 0)
		return m_fShadowNearClipDist;
	else
		return maincam->getNearClipDistance();
}
//-----------------------------------------------------------------------
float gkLight::_deriveShadowFarClipDistance( const gkCamera* maincam ) const
{
	if (m_fShadowFarClipDist > 0)
		return m_fShadowFarClipDist;
	else
		return maincam->getFarClipDistance();
}
//-----------------------------------------------------------------------
void gkLight::_setCameraRelative( gkCamera* cam )
{
	// 建立附属camera的时候同时申请一个RT
	// TODO: 需改进
	if(cam)
	{
		m_pCameraToBeRelativeTo = cam;
		m_bDerivedCamRelativeDirty = true;
		cam->setLightCamera(true);
	}

}
//-----------------------------------------------------------------------
void gkLight::setAmbientColor( float red, float green, float blue )
{
	m_vAmbient = ColorF(red, green, blue, 1);
}
//-----------------------------------------------------------------------
const ColorF& gkLight::getAmbientColor( void ) const
{
	return m_vAmbient;
}
//-----------------------------------------------------------------------
void gkLight::setDiffuseColor( float red, float green, float blue )
{
	m_vDiffuse = ColorF(red, green, blue, 1);
	m_vAnimatedDiffuse = m_vDiffuse;
}
//-----------------------------------------------------------------------
const ColorF& gkLight::getDiffuseColor( void ) const
{
	return m_vAnimatedDiffuse;
}
//-----------------------------------------------------------------------
const AABB& gkLight::getAABB( void ) const
{
	m_WorldAABB.Reset();
	m_WorldAABB.Add(Vec3(0,0,0), m_fRadius);

	return m_WorldAABB;
}
//////////////////////////////////////////////////////////////////////////
void gkLight::Destroy()
{
	// detach from parent
	//m_pParent->detachFromFather();

	// detach all child
	//m_pParent->detachAllChild();

	// remove father scenenode at the same time [3/29/2012 Kaiming]
	//getSceneMngPtr()->destroySceneNode(getParentSceneNode());

	getSceneMngPtr()->destroyMovableObject(this);
}
//////////////////////////////////////////////////////////////////////////
void gkLight::UpdateLayer( float fElpasedTime )
{
	float offset = Random(0.7f, 1.0f);
	m_vAnimatedDiffuse = m_vDiffuse;// * offset;
	m_vAnimatedDiffuse.a = 1;
}

//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
gkStdString gkLightFactory::FACTORY_TYPE_NAME = _T("Light");
//-----------------------------------------------------------------------
const gkStdString& gkLightFactory::getType(void) const
{
	return FACTORY_TYPE_NAME;
}
//-----------------------------------------------------------------------
gkMovableObject* gkLightFactory::createInstanceImpl( const gkStdString& name, 
												const NameValuePairList* params,
												void* userdata)
{
	gkLight* light = new gkLight(name);
	return light;
}
//-----------------------------------------------------------------------
void gkLightFactory::destroyInstance( gkMovableObject* obj)
{
	SAFE_DELETE( obj );
}