#include "TangentSpaceViewer.h"
#include "ISystem.h"
#include "IGameObject.h"
#include "IGameObjectLayer.h"
#include "ITimer.h"


#ifndef _STATIC_LIB
#include "gkPlatform_impl.h"
#endif
#include "I3DEngine.h"
#include "ICamera.h"
#include "IXmlUtil.h"
#include "ITimeOfDay.h"
#include "IRenderer.h"
#include "IAuxRenderer.h"
#include "IGameFramework.h"
#include "ITerrianSystem.h"
#include "ITask.h"


TangentSpaceViewer* g_Game;

extern "C" DLL_EXPORT IGame* gkModuleInitialize(SSystemGlobalEnvironment* pEnv)
{
	if (pEnv)
	{
		gEnv = pEnv;
		g_Game = new TangentSpaceViewer();
		//pEnv->pGameFramework = g_Game;
		return g_Game;
	}
	return NULL;
}

extern "C" DLL_EXPORT void gkModuleUnload(void)
{
	SAFE_DELETE( g_Game );
}

struct TestTask : public ITask
{
	virtual void Execute() 
	{
		gkLogMessage(_T("Task %d start"), (uint32)this );

		//Sleep( 5000 );

		gkLogMessage(_T("Task %d end"), (uint32)this);
	}

};

TangentSpaceViewer::TangentSpaceViewer(void)
{
	mainModel = NULL;
	m_camDist = 20.0f;
	m_camHeight = 0;
}


TangentSpaceViewer::~TangentSpaceViewer(void)
{
}

bool TangentSpaceViewer::OnInit()
{
	// set camera
	ICamera* maincam = gEnv->p3DEngine->getMainCamera();
	maincam->setPosition(Vec3(0,-3,1.8f));
	maincam->setFOVy(DEG2RAD(45.f));
	maincam->setOrientation(Quat::CreateRotationXYZ(Ang3(DEG2RAD(-5),0,0)));
	maincam->setNearPlane(0.05f);
	maincam->setFarPlane(4000.f);

	//gEnv->pGameFramework->LoadLevel(_T("void"));

	// set env setting
	gEnv->p3DEngine->getTimeOfDay()->loadTODSequence(_T("engine/config/default.tod"), true);

	// create skysphere
	Vec3 pos = Vec3(0,0,-0.0005);
	Quat rot = Quat::CreateIdentity();

	IGameObject* sky = gEnv->pGameObjSystem->CreateStaticGeoGameObject( _T("skysphere"), _T("engine/assets/meshs/skysphere.obj"), pos, rot);
	if (sky)
	{
		IGameObjectRenderLayer* pRenderLayer = sky->getRenderLayer();
		if (pRenderLayer)
		{
			gkLogMessage(_T("Load Material"));
			pRenderLayer->setMaterialName(_T("engine/assets/skysphere.mtl"));
			gkLogMessage(_T("Material Loaded"));

			pRenderLayer->setRenderPipe( RENDER_LAYER_SKIES_EARLY );
			pRenderLayer->enableCastShadow(false);
			//pRenderLayer->setVisible(false);
		}

		sky->setScale(2000,2000,2000);
	}

	


	//return true;

// 
// 
// // 	bg = gEnv->pGameObjSystem->CreateLightGameObject( _T("Light01"), Vec3(10,-30,1.3), 4, ColorF(5,2,1,1), true   );
// // 	bg = gEnv->pGameObjSystem->CreateLightGameObject( _T("Light02"), Vec3(4,-26,1.3), 4, ColorF(5,2,1,1), true   );
// // 
// // 	bg = gEnv->pGameObjSystem->CreateLightGameObject( _T("Light03"), Vec3(-4,-3,10), 15, ColorF(5,2,1,1), false   );
// // 	bg = gEnv->pGameObjSystem->CreateLightGameObject( _T("Light04"), Vec3(30,-3,5), 10, ColorF(0.1,1,0.05,1), false   );
// // 
// 	terrian = gEnv->p3DEngine->createTerrian(_T("/terrian/basic_terrian/default.raw"), 8, 512 );
// 	
// 	terrian->ModifyVegetationDensityMap(_T("/terrian/basic_terrian/vegs.raw"));
// 
// 	terrian->Create(_T("/terrian/basic_terrian/default.raw"), 8, 512);
// 
// 	terrian->ModifyColorMap(_T("/terrian/basic_terrian/color.tga"));
// 	terrian->ModifyAreaMap(_T("/terrian/basic_terrian/divide.tga"));
// 	terrian->ModifyDetailMap(_T("/terrian/basic_terrian/detail.tga"), 0);
// 	terrian->ModifyDetailMap(_T("/terrian/basic_terrian/detail_ddn.tga"), 1);
// 	terrian->ModifyDetailMap(_T("/terrian/basic_terrian/detail_s.tga"), 2);
	
	

// 	terrian->setMaterialName(_T("objects/basic/grid10.mtl"));
// 	terrian->showBBox(true);
	gEnv->pInputManager->addInputEventListener(this);

	gkLogMessage(_T("Tangent Space Viewer Initialized."));

	return true;
}

bool TangentSpaceViewer::OnDestroy()
{


	gEnv->pInputManager->removeEventListener(this);
	return true;
}

bool TangentSpaceViewer::OnUpdate()
{



// 	if (terrian)
// 	{
// 		terrian->Update();
// 	}
	

	// set camera
// 	ICamera* maincam = gEnv->p3DEngine->getMainCamera();
// 
//  	Vec3 focusPoint;
//  	focusPoint.zero();
// 	if (mainModel)
// 	{
// 		focusPoint = mainModel->getAABB().GetCenter();
// 
// 		focusPoint.z += m_camHeight;
// 
// 		m_camHeight = clamp(m_camHeight, -mainModel->getAABB().GetRadius() / 2.f, mainModel->getAABB().GetRadius() / 2.f);
// 	}
// 
// 
//  	maincam->setPosition(focusPoint + Vec3(0,0,m_camHeight));
// 
// 	m_camDist = clamp(m_camDist, 0.2f, 1000.f);
// 	maincam->moveLocal(Vec3(0,-m_camDist,0));
// 
// 	gEnv->pRenderer->getAuxRenderer()->AuxRender3DGird(Vec3(0,0,0), 100, 0.1f, ColorF(0,0,0,0.3f));
// 
// 	// GUI CONTROL
// 	UpdateGUI();


	return true;
}

bool TangentSpaceViewer::OnLevelLoaded()
{
	return true;
}

bool TangentSpaceViewer::OnInputEvent( const SInputEvent &input_event )
{

	return HandleFreeCam(input_event);

	return false;
}

void TangentSpaceViewer::UpdateGUI()
{
	return;

	int middleW = gEnv->pRenderer->GetScreenWidth() / 2;
	int middleH = gEnv->pRenderer->GetScreenHeight() / 2;
	int playMenuStart = gEnv->pRenderer->GetScreenWidth() - 200;
	static bool animated = true;
	static TCHAR currAnim[MAX_PATH] = _T("idle");
	
	ColorB buttonTextColor = ColorB(220,220,220,200);

	if (mainModel)
	{
		IGameObjectAnimLayer* layer = reinterpret_cast<IGameObjectAnimLayer*>(mainModel->getGameObjectLayer(eGL_AnimLayer));

		if (layer)
		{
			if (animated)
			{
				if ( gEnv->pInGUI->gkGUIButton(_T("STOP ANIM"), Vec2(playMenuStart, 100), 120, 25, buttonTextColor, ColorB(5,6,50,180)) )
				{
					layer->stopAllAnimation(0);
					gkLogMessage(_T("Button Pressed."));
					animated = false;
				}
			}
			else
			{
				if ( gEnv->pInGUI->gkGUIButton(_T("PLAY ANIM"), Vec2(playMenuStart, 100), 120, 25, buttonTextColor, ColorB(5,6,50,180)) )
				{
					layer->playAnimation(currAnim, 0.1f);
					gkLogMessage(_T("Button Pressed."));
					animated = true;
				}
			}

			int count = layer->getAnimationCount();

			for (int i=0; i < count; ++i)
			{
				const TCHAR* name = layer->getAnimationName(i);
				ColorB textColor = buttonTextColor;
				if ( !_tcsicmp(name, currAnim) )
				{
					textColor = ColorB(255,255,255,255);
				}
				if( gEnv->pInGUI->gkGUIButton( name, Vec2(playMenuStart, 130 + 27 * i), 120, 25, textColor, ColorB(0,0,0,180) ) )
				{
					_tcscpy(currAnim, name);
					if (animated)
					{
						layer->stopAllAnimation(0.1f);
						layer->playAnimation(currAnim, 0.1f);
					}
				}
			}
		}
	}

}

void TangentSpaceViewer::CreateScene()
{
	IGameObject* bg;
	Vec3 pos;
	Quat rot;
 	 //set model chr
//  	 	mainModel = gEnv->pGameObjSystem->CreateAnimGameObject( _T("modelleft"), _T("objects/characters/prophet/prophet.chr"), Vec3(0,0,0), Quat::CreateRotationZ(DEG2RAD(180)));
//  	 	if (mainModel)
//  	 	{
//  	 		IGameObjectRenderLayer* pRenderLayer = mainModel->getRenderLayer();
//  	 		if (pRenderLayer)
//  	 		{
//  	 			gkLogMessage(_T("Load Material"));
//  	 			pRenderLayer->setMaterialName(_T("objects/characters/prophet/prophet.mtl"));
//  	 			gkLogMessage(_T("Material Loaded"));
//  	 		}
//  	 
//  	 		//mainModel->showBBox(true);
//  	 		mainModel->setOrientation( Quat::CreateRotationZ(DEG2RAD(180)) );
//  	 		IGameObjectAnimLayer* anilayer = reinterpret_cast<IGameObjectAnimLayer*>( mainModel->getGameObjectLayer(eGL_AnimLayer) );
//  	 		anilayer->playAnimation(_T("walk_f"), 0);
//  	 	}
//  
//  	mainModel = gEnv->pGameObjSystem->CreateStaticGeoGameObject( _T("modelleft"), _T("objects/scene/b_daoguan.obj"), pos, rot);
 
 	 	pos = Vec3(0,0,-0.0005);
 	 	rot = Quat::CreateIdentity();
 	 	bg  = gEnv->pGameObjSystem->CreateStaticGeoGameObject( _T("modelbg"), _T("objects/scene/b_daoguan.obj"), pos, rot );
 	 	if (bg)
 	 	{
 	 		IGameObjectRenderLayer* pRenderLayer = bg->getRenderLayer();
 	 		if (pRenderLayer)
 	 		{
 	 			gkLogMessage(_T("Load Material"));
 	 			pRenderLayer->setMaterialName(_T("objects/scene/b_daoguan.mtl"));
 	 			gkLogMessage(_T("Material Loaded"));
 	 		}
 	 	}
 	 	bg->setPosition(0,5,0);
 	 
 	 	//return true;
 	 
 	 	bg  = gEnv->pGameObjSystem->CreateStaticGeoGameObject( _T("rongtree"), _T("objects/scene/v_rongtree01.obj"), pos, rot);
 	 	if (bg)
 	 	{
 	 		IGameObjectRenderLayer* pRenderLayer = bg->getRenderLayer();
 	 		if (pRenderLayer)
 	 		{
 	 			gkLogMessage(_T("Load Material"));
 	 			pRenderLayer->setMaterialName(_T("objects/scene/v_rongtree01.mtl"));
 	 			gkLogMessage(_T("Material Loaded"));
 	 		}
 	 	}
 	 	bg->setPosition(13,-3.5,0);
 	 	bg->setScale(0.8,0.8,0.8);
 	 
 	 	bg  = gEnv->pGameObjSystem->CreateStaticGeoGameObject( _T("rongtree1"), _T("objects/scene/v_rongtree01.obj"), pos, rot);
 	 	if (bg)
 	 	{
 	 		IGameObjectRenderLayer* pRenderLayer = bg->getRenderLayer();
 	 		if (pRenderLayer)
 	 		{
 	 			gkLogMessage(_T("Load Material"));
 	 			pRenderLayer->setMaterialName(_T("objects/scene/v_rongtree01.mtl"));
 	 			gkLogMessage(_T("Material Loaded"));
 	 		}
 	 	}
 	 	bg->setPosition(30,-3.5,0);
 	 	bg->setOrientation( Quat::CreateRotationZ( DEG2RAD(135) ));
 	 	bg  = gEnv->pGameObjSystem->CreateStaticGeoGameObject( _T("rongtree2"), _T("objects/scene/v_rongtree01.obj"), pos, rot);
 	 	if (bg)
 	 	{
 	 		IGameObjectRenderLayer* pRenderLayer = bg->getRenderLayer();
 	 		if (pRenderLayer)
 	 		{
 	 			gkLogMessage(_T("Load Material"));
 	 			pRenderLayer->setMaterialName(_T("objects/scene/v_rongtree01.mtl"));
 	 			gkLogMessage(_T("Material Loaded"));
 	 		}
 	 	}
 	 	bg->setPosition(30,3.5,0);
 	 	bg->setOrientation( Quat::CreateRotationZ( DEG2RAD(45) ));
 	 	bg  = gEnv->pGameObjSystem->CreateStaticGeoGameObject( _T("rongtree2a"), _T("objects/scene/v_rongtree01.obj"), pos, rot);
 	 	if (bg)
 	 	{
 	 		IGameObjectRenderLayer* pRenderLayer = bg->getRenderLayer();
 	 		if (pRenderLayer)
 	 		{
 	 			gkLogMessage(_T("Load Material"));
 	 			pRenderLayer->setMaterialName(_T("objects/scene/v_rongtree01.mtl"));
 	 			gkLogMessage(_T("Material Loaded"));
 	 		}
 	 	}
 	 	bg->setPosition(30,10,0);
 	 	bg->setOrientation( Quat::CreateRotationZ( DEG2RAD(-30) ));
 	 
 	 	bg  = gEnv->pGameObjSystem->CreateStaticGeoGameObject( _T("rongtree2a1"), _T("objects/scene/v_rongtree01.obj"), pos, rot);
 	 	if (bg)
 	 	{
 	 		IGameObjectRenderLayer* pRenderLayer = bg->getRenderLayer();
 	 		if (pRenderLayer)
 	 		{
 	 			gkLogMessage(_T("Load Material"));
 	 			pRenderLayer->setMaterialName(_T("objects/scene/v_rongtree01.mtl"));
 	 			gkLogMessage(_T("Material Loaded"));
 	 		}
 	 	}
 	 	bg->setPosition(10,20,0);
 	 	bg->setOrientation( Quat::CreateRotationZ( DEG2RAD(-30) ));
 	 
 	 	bg  = gEnv->pGameObjSystem->CreateStaticGeoGameObject( _T("rongtree2a3"), _T("objects/scene/v_rongtree01.obj"), pos, rot);
 	 	if (bg)
 	 	{
 	 		IGameObjectRenderLayer* pRenderLayer = bg->getRenderLayer();
 	 		if (pRenderLayer)
 	 		{
 	 			gkLogMessage(_T("Load Material"));
 	 			pRenderLayer->setMaterialName(_T("objects/scene/v_rongtree01.mtl"));
 	 			gkLogMessage(_T("Material Loaded"));
 	 		}
 	 	}
 	 	bg->setPosition(0,20,0);
 	 	bg->setOrientation( Quat::CreateRotationZ( DEG2RAD(0) ));
 	 
 	 	bg  = gEnv->pGameObjSystem->CreateStaticGeoGameObject( _T("rongtree2a2"), _T("objects/scene/v_rongtree01.obj"), pos, rot);
 	 	if (bg)
 	 	{
 	 		IGameObjectRenderLayer* pRenderLayer = bg->getRenderLayer();
 	 		if (pRenderLayer)
 	 		{
 	 			gkLogMessage(_T("Load Material"));
 	 			pRenderLayer->setMaterialName(_T("objects/scene/v_rongtree01.mtl"));
 	 			gkLogMessage(_T("Material Loaded"));
 	 		}
 	 	}
 	 	bg->setPosition(-10,20,0);
 	 	bg->setOrientation( Quat::CreateRotationZ( DEG2RAD(30) ));
 	 
 	 	bg  = gEnv->pGameObjSystem->CreateStaticGeoGameObject( _T("rongtree2a4"), _T("objects/scene/v_rongtree01.obj"), pos, rot);
 	 	if (bg)
 	 	{
 	 		IGameObjectRenderLayer* pRenderLayer = bg->getRenderLayer();
 	 		if (pRenderLayer)
 	 		{
 	 			gkLogMessage(_T("Load Material"));
 	 			pRenderLayer->setMaterialName(_T("objects/scene/v_rongtree01.mtl"));
 	 			gkLogMessage(_T("Material Loaded"));
 	 		}
 	 	}
 	 	bg->setPosition(-5,30,0);
 	 	bg->setOrientation( Quat::CreateRotationZ( DEG2RAD(50) ));
 	 
 	 	bg  = gEnv->pGameObjSystem->CreateStaticGeoGameObject( _T("rongtree2a5"), _T("objects/scene/v_rongtree01.obj"), pos, rot);
 	 	if (bg)
 	 	{
 	 		IGameObjectRenderLayer* pRenderLayer = bg->getRenderLayer();
 	 		if (pRenderLayer)
 	 		{
 	 			gkLogMessage(_T("Load Material"));
 	 			pRenderLayer->setMaterialName(_T("objects/scene/v_rongtree01.mtl"));
 	 			gkLogMessage(_T("Material Loaded"));
 	 		}
 	 	}
 	 	bg->setPosition(5,30,0);
 	 	bg->setOrientation( Quat::CreateRotationZ( DEG2RAD(-50) ));
 	 
 	 
 	 	bg  = gEnv->pGameObjSystem->CreateStaticGeoGameObject( _T("bamboo"), _T("objects/scene/v_bamboo01.obj"), pos, rot);
 	 	if (bg)
 	 	{
 	 		IGameObjectRenderLayer* pRenderLayer = bg->getRenderLayer();
 	 		if (pRenderLayer)
 	 		{
 	 			gkLogMessage(_T("Load Material"));
 	 			pRenderLayer->setMaterialName(_T("objects/scene/v_bamboo01.mtl"));
 	 			gkLogMessage(_T("Material Loaded"));
 	 		}
 	 	}
 	 	bg->setPosition(-13,-2,0);
 	 	bg->setScale(0.7,0.7,0.7);
 	 
 	 	bg  = gEnv->pGameObjSystem->CreateStaticGeoGameObject( _T("bamboo1"), _T("objects/scene/v_bamboo01.obj"), pos, rot);
 	 	if (bg)
 	 	{
 	 		IGameObjectRenderLayer* pRenderLayer = bg->getRenderLayer();
 	 		if (pRenderLayer)
 	 		{
 	 			gkLogMessage(_T("Load Material"));
 	 			pRenderLayer->setMaterialName(_T("objects/scene/v_bamboo01.mtl"));
 	 			gkLogMessage(_T("Material Loaded"));
 	 		}
 	 	}
 	 	bg->setPosition(-18,0,0);
 	 	bg->setScale(0.6,0.6,0.6);
 	 	bg->setOrientation( Quat::CreateRotationZ( DEG2RAD(95) ));
 	 
 	 
 	 	bg  = gEnv->pGameObjSystem->CreateStaticGeoGameObject( _T("bamboo2"), _T("objects/scene/v_bamboo01.obj"), pos, rot);
 	 	if (bg)
 	 	{
 	 		IGameObjectRenderLayer* pRenderLayer = bg->getRenderLayer();
 	 		if (pRenderLayer)
 	 		{
 	 			gkLogMessage(_T("Load Material"));
 	 			pRenderLayer->setMaterialName(_T("objects/scene/v_bamboo01.mtl"));
 	 			gkLogMessage(_T("Material Loaded"));
 	 		}
 	 	}
 	 	bg->setPosition(-25,-5,0);
 	 	bg->setScale(0.8,0.8,0.8);
 	 	bg->setOrientation( Quat::CreateRotationZ( DEG2RAD(45) ));
 	 
 	 	bg  = gEnv->pGameObjSystem->CreateStaticGeoGameObject( _T("rongtree3"), _T("objects/scene/v_rongtree01.obj"), pos, rot);
 	 	if (bg)
 	 	{
 	 		IGameObjectRenderLayer* pRenderLayer = bg->getRenderLayer();
 	 		if (pRenderLayer)
 	 		{
 	 			gkLogMessage(_T("Load Material"));
 	 			pRenderLayer->setMaterialName(_T("objects/scene/v_rongtree01.mtl"));
 	 			gkLogMessage(_T("Material Loaded"));
 	 		}
 	 	}
 	 	bg->setPosition(-30,5,0);
 	 	bg->setOrientation( Quat::CreateRotationZ( DEG2RAD(-45) ));
 	 
 	 	bg  = gEnv->pGameObjSystem->CreateStaticGeoGameObject( _T("rongtree4"), _T("objects/scene/v_rongtree01.obj"), pos, rot);
 	 	if (bg)
 	 	{
 	 		IGameObjectRenderLayer* pRenderLayer = bg->getRenderLayer();
 	 		if (pRenderLayer)
 	 		{
 	 			gkLogMessage(_T("Load Material"));
 	 			pRenderLayer->setMaterialName(_T("objects/scene/v_rongtree01.mtl"));
 	 			gkLogMessage(_T("Material Loaded"));
 	 		}
 	 	}
 	 	bg->setPosition(-60,8,0.5);
 	 	bg->setOrientation( Quat::CreateRotationZ( DEG2RAD(45) ));
 	 
 	 	bg  = gEnv->pGameObjSystem->CreateStaticGeoGameObject( _T("rongtree5"), _T("objects/scene/v_rongtree01.obj"), pos, rot);
 	 	if (bg)
 	 	{
 	 		IGameObjectRenderLayer* pRenderLayer = bg->getRenderLayer();
 	 		if (pRenderLayer)
 	 		{
 	 			gkLogMessage(_T("Load Material"));
 	 			pRenderLayer->setMaterialName(_T("objects/scene/v_rongtree01.mtl"));
 	 			gkLogMessage(_T("Material Loaded"));
 	 		}
 	 	}
 	 	bg->setPosition(-70,0,0.5);
 	 	bg->setOrientation( Quat::CreateRotationZ( DEG2RAD(135) ));
 	 
 	 	bg  = gEnv->pGameObjSystem->CreateStaticGeoGameObject( _T("rongtree6"), _T("objects/scene/v_rongtree01.obj"), pos, rot);
 	 	if (bg)
 	 	{
 	 		IGameObjectRenderLayer* pRenderLayer = bg->getRenderLayer();
 	 		if (pRenderLayer)
 	 		{
 	 			gkLogMessage(_T("Load Material"));
 	 			pRenderLayer->setMaterialName(_T("objects/scene/v_rongtree01.mtl"));
 	 			gkLogMessage(_T("Material Loaded"));
 	 		}
 	 	}
 	 	bg->setPosition(-5,-40,0);
 	 	bg->setOrientation( Quat::CreateRotationZ( DEG2RAD(25) ));
 	 
 	 	bg  = gEnv->pGameObjSystem->CreateStaticGeoGameObject( _T("taoshu01"), _T("objects/scene/v_taoshu.obj"), pos, rot);
 	 	if (bg)
 	 	{
 	 		IGameObjectRenderLayer* pRenderLayer = bg->getRenderLayer();
 	 		if (pRenderLayer)
 	 		{
 	 			gkLogMessage(_T("Load Material"));
 	 			pRenderLayer->setMaterialName(_T("objects/scene/v_taoshu.mtl"));
 	 			gkLogMessage(_T("Material Loaded"));
 	 		}
 	 	}
 	 	bg->setPosition(10,-20,0);
 	 	bg->setOrientation( Quat::CreateRotationZ( DEG2RAD(25) ));
 	 	bg->setScale( Vec3(0.3,0.3,0.3) );
 }

 bool TangentSpaceViewer::HandleFreeCam( const SInputEvent &input_event )
 {
	 float fFrameTime = gEnv->pTimer->GetFrameTime();
	 static float speed = 1.0f;

	 switch ( input_event.deviceId )
	 {
	 case eDI_Mouse:
		 {
			 static bool holdingR = false;
			 static bool holdingL = false;
			 static bool holdingM = false;
			 if (input_event.keyId == eKI_Mouse2)
			 {
				 if (input_event.state == eIS_Down)
				 {
					 holdingR = true;
				 }
				 else
				 {
					 holdingR = false;
				 }
			 }
			 else if (input_event.keyId == eKI_Mouse1)
			 {
				 if (input_event.state == eIS_Down)
				 {
					 holdingL = true;
				 }
				 else
				 {
					 holdingL = false;
				 }
			 }
			 else if (input_event.keyId == eKI_Mouse3)
			 {
				 if (input_event.state == eIS_Down)
				 {
					 holdingM = true;
				 }
				 else
				 {
					 holdingM = false;
				 }
			 }
			 else if (input_event.keyId == eKI_MouseX && holdingR)
			 {
				 Quat qBefore = gEnv->p3DEngine->getMainCamera()->getDerivedOrientation();
				 Ang3 aRot(qBefore);
				 aRot.z -= input_event.value * 0.005f;
				 Quat qRot = Quat::CreateRotationXYZ(aRot);
				 qRot.Normalize();
				 gEnv->p3DEngine->getMainCamera()->setOrientation(qRot);				
			 }
			 else if (input_event.keyId == eKI_MouseX && holdingL)
			 {
				 if ( mainModel )
				 {
					 Quat qBefore = mainModel->getOrientation();
					 Ang3 aRot(qBefore);
					 aRot.z += input_event.value * 0.005f;
					 Quat qRot = Quat::CreateRotationXYZ(aRot);
					 qRot.Normalize();
					 mainModel->setOrientation(qRot);
				 }
			 }
			 else if (input_event.keyId == eKI_MouseY && (holdingR || holdingL))
			 {
				 Quat qBefore = gEnv->p3DEngine->getMainCamera()->getDerivedOrientation();
				 Ang3 aRot(qBefore);
				 aRot.x -= input_event.value * 0.005f;
				 Quat qRot = Quat::CreateRotationXYZ(aRot);
				 qRot.Normalize();
				 gEnv->p3DEngine->getMainCamera()->setOrientation(qRot);				
			 }
			 else if (input_event.keyId == eKI_MouseY && (holdingM))
			 {
				 m_camHeight -= input_event.value * 0.005f;			
			 }
			 else if (input_event.keyId == eKI_MouseWheelUp)
			 {
				 m_camDist -= 0.2f;
			 }
			 else if (input_event.keyId == eKI_MouseWheelDown)
			 {
				 m_camDist += 0.2f;
			 }
			 break;
		 }
	 case eDI_Keyboard:
		 {

			 if (input_event.keyId == eKI_T && input_event.state == eIS_Pressed)
			 {
				 TestTask* task = new TestTask;
				 gEnv->pCommonTaskDispatcher->PushTask( task );
			 }
			 else if (input_event.keyId == eKI_L && input_event.state == eIS_Pressed)
			 {
				 Vec3 pos;
				 Quat rot;
				 static int x = 0;
				 pos = Vec3(x,0,0);
				 rot = Quat::CreateIdentity();

				 TCHAR name[255];
				 _stprintf(name, _T("chr_%d"), x);

				 float now = gEnv->pTimer->GetAsyncCurTime();

				 mainModel  = gEnv->pGameObjSystem->CreateStaticGeoGameObject( name, _T("objects/characters/faraa/faraa_rig.gmf"), pos, rot );
				 if (mainModel)
				 {
					 IGameObjectRenderLayer* pRenderLayer = mainModel->getRenderLayer();
					 if (pRenderLayer)
					 {
						 pRenderLayer->setMaterialName(_T("objects/characters/faraa/faraa.mtl"));
					 }
				 }
				 mainModel->setPosition(pos);
				 x += 1;

				 now = gEnv->pTimer->GetAsyncCurTime() - now;

				 gkLogMessage(_T("Load Chr use %.2fms."), now * 1000);
			 }
			 else if (input_event.keyId == eKI_K && input_event.state == eIS_Pressed)
			 {
				 float now = gEnv->pTimer->GetAsyncCurTime();

				 CreateScene();

				 now = gEnv->pTimer->GetAsyncCurTime() - now;

				 gkLogMessage(_T("Load Chr use %.2fms."), now * 1000);
			 }

			 if (input_event.keyId == eKI_LShift && input_event.state == eIS_Down)
			 {
				 speed = 50.0f;
			 }
			 else if (input_event.keyId == eKI_LShift && input_event.state == eIS_Released)
			 {
				 speed = 5.0f;
			 }
			 else if (input_event.keyId == eKI_W)
			 {
				 gEnv->p3DEngine->getMainCamera()->moveLocal( Vec3(0, fFrameTime * speed, 0) );
			 }
			 else if (input_event.keyId == eKI_S)
			 {
				 gEnv->p3DEngine->getMainCamera()->moveLocal( Vec3(0, -fFrameTime * speed, 0 ) );
			 }
			 else if (input_event.keyId == eKI_A)
			 {
				 gEnv->p3DEngine->getMainCamera()->moveLocal( Vec3(-fFrameTime * speed, 0, 0) );
			 }
			 else if (input_event.keyId == eKI_D)
			 {
				 gEnv->p3DEngine->getMainCamera()->moveLocal( Vec3(fFrameTime * speed, 0, 0 ) );
			 }


			 // 			if (event.keyId == eKI_LShift && event.state == eIS_Down)
			 // 			{
			 // 				speed = 50.0f;
			 // 			}
			 // 			else if (event.keyId == eKI_LShift && event.state == eIS_Released)
			 // 			{
			 // 				speed = 5.0f;
			 // 			}
			 // 			else if (event.keyId == eKI_W)
			 // 			{
			 // 				gEnv->p3DEngine->getMainCamera()->moveLocal( Vec3(0, fFrameTime * speed, 0) );
			 // 			}
			 // 			else if (event.keyId == eKI_S)
			 // 			{
			 // 				gEnv->p3DEngine->getMainCamera()->moveLocal( Vec3(0, -fFrameTime * speed, 0 ) );
			 // 			}
			 // 			else if (event.keyId == eKI_A)
			 // 			{
			 // 				gEnv->p3DEngine->getMainCamera()->moveLocal( Vec3(-fFrameTime * speed, 0, 0) );
			 // 			}
			 // 			else if (event.keyId == eKI_D)
			 // 			{
			 // 				gEnv->p3DEngine->getMainCamera()->moveLocal( Vec3(fFrameTime * speed, 0, 0 ) );
			 // 			}
			 break;
		 }
	 case eDI_XBox:
		 {
			 if (input_event.keyId == eKI_Xbox_TriggerL && input_event.state == eIS_Changed)
			 {
				 speed = (input_event.value) * 45.0 + 15;
				 return false;
			 }

			 if (input_event.keyId == eKI_Xbox_ThumbLY && input_event.state == eIS_Changed)
			 {
				 float thumbspeed = (input_event.value) * speed;
				 gEnv->p3DEngine->getMainCamera()->moveLocal( Vec3(0, fFrameTime * thumbspeed, 0) );
				 return false;
			 }

			 if (input_event.keyId == eKI_Xbox_ThumbLX && input_event.state == eIS_Changed)
			 {
				 float thumbspeed = (input_event.value) * speed;
				 gEnv->p3DEngine->getMainCamera()->moveLocal( Vec3(fFrameTime * thumbspeed, 0, 0) );
				 return false;
			 }

			 if (input_event.keyId == eKI_Xbox_ThumbRX && input_event.state == eIS_Changed)
			 {
				 Quat qBefore = gEnv->p3DEngine->getMainCamera()->getDerivedOrientation();
				 Ang3 aRot(qBefore);

				 aRot.z -= input_event.value * 0.015f;
				 //aRot.x -= vMouseDelta.y * 0.002f;

				 Quat qRot = Quat::CreateRotationXYZ(aRot);
				 qRot.Normalize();
				 gEnv->p3DEngine->getMainCamera()->setOrientation(qRot);			
				 return false;
			 }

			 if (input_event.keyId == eKI_Xbox_ThumbRY && input_event.state == eIS_Changed)
			 {
				 Quat qBefore = gEnv->p3DEngine->getMainCamera()->getDerivedOrientation();
				 Ang3 aRot(qBefore);

				 aRot.x += input_event.value * 0.015f;
				 //aRot.x -= vMouseDelta.y * 0.002f;

				 Quat qRot = Quat::CreateRotationXYZ(aRot);
				 qRot.Normalize();
				 gEnv->p3DEngine->getMainCamera()->setOrientation(qRot);		
				 return false;
			 }
			 break;
		 }
	 case eDI_Android:
	 case eDI_IOS:
		 {
			 break;
		 }
	 }

	 return false;
 }
