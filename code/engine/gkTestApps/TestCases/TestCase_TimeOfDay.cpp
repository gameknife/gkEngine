#include "TestCaseFramework.h"
#include "TestCase_Template.h"
#include "ITerrianSystem.h"
#include "I3DEngine.h"
#include "ICamera.h"
#include "TestCaseUtil_Cam.h"
#include "ITimeOfDay.h"
#include "IGameFramework.h"

TEST_CASE_FASTIMPL_HEAD( TestCase_TimeOfDay, eTcc_Rendering )

ITerrianSystem* terrian;
std::vector<IGameObject*> m_creations;
Vec3 m_backupPos;
Quat m_backupRot;

virtual void OnInit() 
{
	// set camera
	ICamera* maincam = gEnv->p3DEngine->getMainCamera();
	m_backupPos = maincam->getPosition();
	m_backupRot = maincam->getOrientation();

	maincam->setPosition(-3, -36, 2);
	maincam->setOrientation( Quat::CreateRotationXYZ( Ang3( DEG2RAD(-5), 0,0 ) ) );

	gEnv->p3DEngine->getTimeOfDay()->setSpeed( 0.5 );

	float now = gEnv->pTimer->GetAsyncCurTime();


	now = gEnv->pTimer->GetAsyncCurTime() - now;
	gkLogMessage(_T("Load Terrian use %.2fms."), now * 1000);

	gEnv->pGameFramework->LoadLevel( _T("level/daoguan/daoguan.gks") );
}

virtual bool OnUpdate() 
{

	TCHAR info[512];
	_stprintf( info, _T("time: %.2f"), gEnv->p3DEngine->getTimeOfDay()->getTime() );
	gEnv->pInGUI->gkGUIButton( info, Vec2(gEnv->pRenderer->GetScreenWidth() / 2 - 100, 100), 200, 60, ColorB(255,255,255,255), ColorB(0,0,0,128));



	return true;
}

virtual void OnDestroy() 
{
	gEnv->pGameFramework->LoadLevel( _T("void") );
	gEnv->p3DEngine->getTimeOfDay()->setSpeed( 0.0 );
}

virtual void OnInputEvent( const SInputEvent &event ) 
{
	if (event.keyId == eKI_L && event.state == eIS_Pressed)
	{

	}


	HandleFreeCam( event );
}

TEST_CASE_FASTIMPL_TILE( TestCase_TimeOfDay )