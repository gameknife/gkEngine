#include "gkStableHeader.h"
#include "gk3DEngine.h"
#include "gkSceneNode.h"
#include "gkSun.h"
#include "gkSceneManager.h"
#include "gkCamera.h"
//#include "gkD3DMathExtended.h"
//#include "gkPostProcessManager.h"

#include <iostream>  
#include <sstream> 
#include "ISystemProfiler.h"

//-----------------------------------------------------------------------
// gkSun::gkSun( void ):gkLight()
// 	,m_pAttachSceneNode(NULL)
// 	,m_pAttachSceneNode1(NULL)
// 	,m_pAttachSceneNode2(NULL)
// 	,m_pBindSceneNode(NULL)
// 	,m_fArc(45.0f)
// 	,m_fDir(35.0f)
// 	,m_fTime(9.0f)
// 	,SUN_RADIUS(100)
// 	,m_pCascade1Cam(NULL)
// 	,m_pCascade2Cam(NULL)
// {
// 
// }
//-----------------------------------------------------------------------
gkSun::gkSun( gkStdString name ):gkLight(name)
	,m_pAttachSceneNode(NULL)
	,m_pAttachSceneNode1(NULL)
	,m_pAttachSceneNode2(NULL)
	,m_pBindSceneNode(NULL)
	,m_fArc(45.0f)
	,m_fDir(35.0f)
	,m_fTime(9.0f)
	,SUN_RADIUS(3000)
	,m_pCascade1Cam(NULL)
	,m_pCascade2Cam(NULL)
{
	gkStdStringstream wssName;
	wssName << name << _T("_shadowcam");
	gkCamera* shadowCam = getSceneMngPtr()->createCamera(_T("MainLightCamera"));

	m_pCascade1Cam = getSceneMngPtr()->createCamera(_T("MainLightCamera_Cascade1"));
	m_pCascade2Cam = getSceneMngPtr()->createCamera(_T("MainLightCamera_Cascade2"));

	// 建立一个光源节点，控制光源的朝向和位置
	wssName.clear();
	wssName << name << _T("_scenenode");
	m_pAttachSceneNode = getSceneMngPtr()->getRootSceneNode()->createChildSceneNode(wssName.str());
	wssName << _T("_0");
	m_pAttachSceneNode1 = getSceneMngPtr()->getRootSceneNode()->createChildSceneNode(wssName.str());
	wssName << _T("_1");
	m_pAttachSceneNode2 = getSceneMngPtr()->getRootSceneNode()->createChildSceneNode(wssName.str());


	// 设置阴影参数
	shadowCam->setProjectionType(PT_ORTHOGRAPHIC);
	shadowCam->setNearClipDistance(SUN_RADIUS - 300);
	shadowCam->setFarClipDistance(SUN_RADIUS + 25);
	shadowCam->setOrthoHeight(4096*3);
	shadowCam->setAspect(1.0f);
	shadowCam->setFOVy(DEG2RAD(0.15f));

	// cascade1Cam
	m_pCascade1Cam->setProjectionType(PT_ORTHOGRAPHIC);
	m_pCascade1Cam->setNearClipDistance(SUN_RADIUS - 400);
	m_pCascade1Cam->setFarClipDistance(SUN_RADIUS + 50);
	m_pCascade1Cam->setOrthoHeight(4096*3);
	m_pCascade1Cam->setAspect(1.0f);
	m_pCascade1Cam->setFOVy(DEG2RAD(0.75f));

	// cascade2Cam
	m_pCascade2Cam->setProjectionType(PT_ORTHOGRAPHIC);
	m_pCascade2Cam->setNearClipDistance(SUN_RADIUS - 500);
	m_pCascade2Cam->setFarClipDistance(SUN_RADIUS + 500);
	m_pCascade2Cam->setOrthoHeight(4096*3);
	m_pCascade2Cam->setAspect(1.0f);
	m_pCascade2Cam->setFOVy(DEG2RAD(5.5f));

	// 将光源挂载摄像机设置到光源节点上
	m_pAttachSceneNode->attachObject(shadowCam);
	// 将光源设置上去
	m_pAttachSceneNode->attachObject(this);

	m_pAttachSceneNode1->attachObject(m_pCascade1Cam);
	m_pAttachSceneNode2->attachObject(m_pCascade2Cam);

	// 挂载光源摄像机, 这里实际上就说明了我要将此light应用shadowmap，那么在函数中就向rendersystem提请一个RTT
	_setCameraRelative(shadowCam);

	m_pCascade1Cam->setLightCamera(true);
	m_pCascade2Cam->setLightCamera(true);

	m_vOffset.zero();
}
//-----------------------------------------------------------------------
gkSun::~gkSun( void )
{

}
//-----------------------------------------------------------------------
void gkSun::bindSceneNode( gkSceneNode* pNode )
{
	m_pBindSceneNode = pNode;
}
//-----------------------------------------------------------------------
void gkSun::setSunArc( float fArc )
{
	m_fArc = fArc;
	setTime(m_fTime);
}
//-----------------------------------------------------------------------
void gkSun::setSunDir( float fDir )
{
	m_fDir = fDir;
	setTime(m_fTime);
}
//-----------------------------------------------------------------------
void gkSun::setTime( float fTime )
{
	if ( fabsf(m_fTime - fTime) < VEC_EPSILON)
	{
		return;
	}
	m_fTime = fTime;
	if (m_pAttachSceneNode)
	{
		m_pAttachSceneNode->setOrientation(Quat(1,0,0,0));
		m_pAttachSceneNode->setPosition(0,0,0);

		float fAngle = (m_fTime - 6) * 15;
		Ang3 aRot(-DEG2RAD(fAngle), 0, DEG2RAD(m_fDir));
		//m_pAttachSceneNode->rotate(Vec3(-1,0,0), DEG2RAD(fAngle), gkTransformSpace::TS_LOCAL);
		//m_pAttachSceneNode->rotate(Vec3(0,-1,0), DEG2RAD(m_fDir), gkTransformSpace::TS_PARENT);
		//m_pAttachSceneNode->rotate(Vec3(0,0,-1), DEG2RAD(m_fArc), gkTransformSpace::TS_LOCAL);

		Quat qRot = Quat::CreateRotationXYZ(aRot);
		qRot.Normalize();
		m_pAttachSceneNode->setOrientation(qRot);

		// sync cascade cam
		if( gEnv->pProfiler->getFrameCount() % GSM_SHADOWCASCADE1_DELAY == 0);
			m_pAttachSceneNode1->setOrientation(qRot);

		if( gEnv->pProfiler->getFrameCount() % GSM_SHADOWCASCADE2_DELAY == 0);
			m_pAttachSceneNode2->setOrientation(qRot);


		m_pAttachSceneNode->translate(Vec3(0,-(float)SUN_RADIUS,0), TS_LOCAL);

		m_vSunVector = m_pAttachSceneNode->getPosition();

		//  [4/6/2011 Kaiming-Desktop]
		// 在夜晚的时候截获gkPostProcess关闭阴影
		// 在傍晚时做shadow过渡


// REFACTOR FIX [8/9/2011 Kaiming-Desktop]

// 		if(m_fTime < 6.0f || m_fTime > 18.0f)
// 			gk3DEngine::getSingleton().getPostProcessMngPtr()->setShadowMapSoften(false);
// 		else
// 		{
// 			gk3DEngine::getSingleton().getPostProcessMngPtr()->setShadowMapSoften(true);
// 			float fDepth = 0.6f;
// 			if (m_fTime > 6.0f && m_fTime < 8.0f)
// 			{
// 				fDepth = (m_fTime - 6.0f) * 0.3f;l
// 			}
// 			if (m_fTime > 16.0f && m_fTime < 18.0f)
// 			{
// 				fDepth = (18.0f - m_fTime) * 0.3f;
// 			}
// 			gk3DEngine::getSingleton().getPostProcessMngPtr()->setShadowDepth(fDepth);
// 		}
	}
}
//-----------------------------------------------------------------------
void gkSun::update( void ) const
{
	// FRIST sync pos
  	ICamera* cam = gEnv->p3DEngine->getMainCamera();
  	if (cam)
  	{
		// here the magic number, from test
		Vec3 desirePoint = cam->getPosition() + cam->getOrientation().GetColumn1() * 3;
		desirePoint.z = cam->getPosition().z;

		Vec3 desirePoint1 = cam->getPosition() + cam->getOrientation().GetColumn1() * 12;
		desirePoint1.z = cam->getPosition().z;

		Vec3 desirePoint2 = cam->getPosition();
		//desirePoint2.z = cam->getPosition().z;

		Vec3 off = m_vSunVector + m_vOffset;

  		m_pAttachSceneNode->setPosition(desirePoint + off );

		//sync cascade cam
		if( gEnv->pProfiler->getFrameCount() % GSM_SHADOWCASCADE1_DELAY == 0);
			m_pAttachSceneNode1->setPosition(desirePoint1 + off);

		//sync cascade cam
		if( gEnv->pProfiler->getFrameCount() % GSM_SHADOWCASCADE2_DELAY == 0);
			m_pAttachSceneNode2->setPosition(desirePoint2 + off);

		// ensure it update
		//m_pCameraToBeRelativeTo->getViewMatrix();
  	}

	//m_pCameraToBeRelativeTo->setOrientation(Quat::CreateIdentity());
// 	if (m_pBindSceneNode)
// 	{
// 		m_pAttachSceneNode->setPosition(m_pBindSceneNode->getPosition() + m_vSunVector);
// 		m_pAttachSceneNode->lookat(Vec3(0,0,0));
// 	}
	// detrive
	gkLight::update();
}

void gkSun::_updateRenderSequence( IRenderSequence* queue )
{

}
