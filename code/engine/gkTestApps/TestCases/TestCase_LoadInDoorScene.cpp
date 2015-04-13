#include "TestCaseFramework.h"
#include "TestCase_Template.h"
#include "ITerrianSystem.h"
#include "I3DEngine.h"
#include "ICamera.h"
#include "TestCaseUtil_Cam.h"
#include "IGameFramework.h"
#include "ITimeOfDay.h"

TEST_CASE_FASTIMPL_HEAD( TestCase_InDoorRendering, eTcc_Rendering )

virtual void OnInit() 
{
	// set camera
	ICamera* maincam = gEnv->p3DEngine->getMainCamera();
	maincam->setFOVy( DEG2RAD(60.0f) );

	//maincam->setPosition(-2, -36, 2);
	//maincam->setOrientation( Quat::CreateRotationXYZ( Ang3( DEG2RAD(-5), 0,0 ) ) );


	//<CameraRecord Index="1" Pos="-0.74809 3.01126 1.06231" Rot="0.01166 -0.05927 -0.97985 0.19036"/>
	maincam->setPosition(-0.74809, 3.01126, 1.06231);
	maincam->setOrientation( Quat::CreateRotationXYZ( Ang3( DEG2RAD(-5), 0, DEG2RAD(190) ) ) );// * Quat(0.01166, -0.05927, -0.97985, 0.19036) );

	//gEnv->pGameFramework->LoadLevel( _T("level/daoguan/daoguan.gks") );
	gEnv->pGameFramework->LoadLevel( _T("level/conf_room/conf_room.gks") );
	//gEnv->p3DEngine->getTimeOfDay()->setSpeed( 0.5 );
}

virtual bool OnUpdate() 
{
	if (gEnv->pInGUI->gkGUIButton(_T("Rewind"), Vec2(gEnv->pRenderer->GetScreenWidth() / 2 - 150, 100), 90, 40, ColorB(255, 255, 255, 255), ColorB(0, 0, 0, 127)))
	{
		gEnv->p3DEngine->getTimeOfDay()->setTime(gEnv->p3DEngine->getTimeOfDay()->getTime() - 0.1);
	}

	if (gEnv->pInGUI->gkGUIButton(_T("Forward"), Vec2(gEnv->pRenderer->GetScreenWidth() / 2 + 60, 100), 90, 40, ColorB(255, 255, 255, 255), ColorB(0, 0, 0, 127)))
	{
		gEnv->p3DEngine->getTimeOfDay()->setTime(gEnv->p3DEngine->getTimeOfDay()->getTime() + 0.1);
	}

	TCHAR time[255];
	_stprintf(time, _T("%.2f"), gEnv->p3DEngine->getTimeOfDay()->getTime());

	if (gEnv->pInGUI->gkGUIButton(time, Vec2(gEnv->pRenderer->GetScreenWidth() / 2 - 40, 100), 80, 40, ColorB(255, 255, 255, 255), ColorB(0, 0, 0, 127)))
	{
		//gEnv->p3DEngine->getTimeOfDay()->setTime(gEnv->p3DEngine->getTimeOfDay()->getTime() + 0.1);
	}

	return true;
}

virtual void OnDestroy() 
{
	gEnv->pGameFramework->LoadLevel( _T("void") );
	gEnv->pSystem->cleanGarbage();
}

virtual void OnInputEvent( const SInputEvent &event ) 
{
	HandleFreeCam( event );
	//HandleModelViewCam(event, 0.002f);
}

TEST_CASE_FASTIMPL_TILE( TestCase_InDoorRendering )