#include "StableHeader.h"
#include "gkGameDemo.h"
#include "Player/gkMainPlayer.h"
#include "I3DEngine.h"
#include "IXmlUtil.h"
#include "ITimeOfDay.h"
#include "IHavok.h"
//////////////////////////////////////////////////////////////////////////
gkGameDemo::gkGameDemo( void )
{
	m_pMainPlayer = new gkMainPlayer();
	m_bUIMode = 0;
}
//////////////////////////////////////////////////////////////////////////
bool gkGameDemo::OnInit()
{
	// load from startup.cfg

	// set camera
	ICamera* maincam = gEnv->p3DEngine->getMainCamera();
	maincam->setPosition(Vec3(0,-50,1.8f));
	maincam->setFOVy(DEG2RAD(60.f));
	maincam->setOrientation(Quat::CreateRotationXYZ(Ang3(DEG2RAD(5),0,0)));
	maincam->setNearPlane(0.025f);
	maincam->setFarPlane(4000.f);

	gEnv->pGameFramework->LoadLevel(_T("level/daoguan/daoguan.gks"));

	gEnv->pInputManager->addInputEventListener(this);

	return true;
}
//////////////////////////////////////////////////////////////////////////
bool gkGameDemo::OnDestroy()
{
	//m_pMainPlayer->Destroy();

	gEnv->pInputManager->removeEventListener(this);
	return true;
}
//////////////////////////////////////////////////////////////////////////
bool gkGameDemo::OnUpdate()
{
	float fFrameTime = gEnv->pTimer->GetFrameTime();
	

// 	if (!m_bUIMode)
// 	{
// 		// clip the rect
// 		HWND hWnd = gEnv->pSystem->getRenderHwnd();
// 		RECT rctWindow;
// 		GetWindowRect(hWnd, &rctWindow);
// 		ClipCursor(&rctWindow);
// 	}
// 	else
// 	{
// 		ClipCursor(NULL);
// 	}

	m_pMainPlayer->Update(fFrameTime, m_bUIMode);


	return true;
}
//////////////////////////////////////////////////////////////////////////
bool gkGameDemo::OnLevelLoaded()
{
	// test stuff, init a player [4/7/2012 Kaiming]
	{
		ICamera* maincam = gEnv->p3DEngine->getMainCamera();
		Vec3 playerInitPos(2.0,3.0,19.0);
// 		if ( playerInitPos.z < 2.0f)
// 		{
// 			playerInitPos.z = 2.0f;
// 		}

		Vec3 projectedForward = Vec3::CreateProjection( maincam->getDirection(), Vec3(0,0,1));
		projectedForward.Normalize();

		Quat playerRot = Quat::CreateRotationVDir(projectedForward);
		playerRot.Normalize();
		m_pMainPlayer->Init(maincam->getOrientation());
		m_pMainPlayer->TeleportPlayer( playerInitPos, playerRot );
	}
	return true;
}
//////////////////////////////////////////////////////////////////////////
bool gkGameDemo::OnInputEvent( const SInputEvent &event )
{
	switch ( event.deviceId )
	{
	case eDI_Keyboard:
		{
			if (event.keyId == eKI_Tab && event.state == eIS_Pressed)
			{
				m_bUIMode = !m_bUIMode;
				return false;
			}
		}
	}
	return false;
}
