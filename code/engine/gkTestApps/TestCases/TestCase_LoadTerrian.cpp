#include "TestCaseFramework.h"
#include "TestCase_Template.h"
#include "ITerrianSystem.h"
#include "I3DEngine.h"
#include "ICamera.h"
#include "TestCaseUtil_Cam.h"

TEST_CASE_FASTIMPL_HEAD( TestCase_LoadTerrian, eTcc_Loading )

ITerrianSystem* terrian;
Vec3 m_backupPos;
Quat m_backupRot;

virtual void OnInit() 
{
	terrian = NULL;

	// set camera
	ICamera* maincam = gEnv->p3DEngine->getMainCamera();
	m_backupPos = maincam->getPosition();
	m_backupRot = maincam->getOrientation();

	maincam->setPosition(0, -30, 2);
	maincam->setOrientation( Quat::CreateRotationXYZ( Ang3( DEG2RAD(-5), 0,0 ) ) );
}

virtual bool OnUpdate() 
{
	gEnv->pInGUI->gkGUIButton( _T("PRESS [L] TO CREATE"), Vec2(gEnv->pRenderer->GetScreenWidth() / 2 - 100, 100), 200, 60, ColorB(255,255,255,255), ColorB(0,0,0,128));
	if (terrian)
	{
		terrian->Update();
	}

	return true;
}

virtual void OnDestroy() 
{
	ICamera* maincam = gEnv->p3DEngine->getMainCamera();
	maincam->setPosition( m_backupPos );
	maincam->setOrientation( m_backupRot );

	gEnv->p3DEngine->destroyTerrian( terrian );
	terrian = NULL;

	gEnv->pSystem->cleanGarbage();
}

virtual void OnInputEvent( const SInputEvent &event ) 
{
	if (event.keyId == eKI_L && event.state == eIS_Pressed)
	{
		float now = gEnv->pTimer->GetAsyncCurTime();

		terrian = gEnv->p3DEngine->createTerrian();
		if (terrian)
		{
			terrian->ModifyVegetationDensityMap(_T("/terrian/basic_terrian/vegs.raw"));
			terrian->Create(_T("/terrian/basic_terrian/default.raw"), 8, 512);

			terrian->ModifyColorMap(_T("/terrian/basic_terrian/color.tga"));
			terrian->ModifyAreaMap(_T("/terrian/basic_terrian/divide.tga"));
			terrian->ModifyDetailMap(_T("/terrian/basic_terrian/detail.tga"), 0);
			terrian->ModifyDetailMap(_T("/terrian/basic_terrian/detail_ddn.tga"), 1);
			terrian->ModifyDetailMap(_T("/terrian/basic_terrian/detail_s.tga"), 2);
		}

		now = gEnv->pTimer->GetAsyncCurTime() - now;
		gkLogMessage(_T("Load Terrian use %.2fms."), now * 1000);
	}


	HandleFreeCam( event );
}

TEST_CASE_FASTIMPL_TILE( TestCase_LoadTerrian )