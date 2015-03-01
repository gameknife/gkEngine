#include "StableHeader.h"
#include "gkGameDemo.h"
#include "I3DEngine.h"
#include "IXmlUtil.h"
#include "ITimeOfDay.h"
#include "IHavok.h"
//////////////////////////////////////////////////////////////////////////
gkGameDemo::gkGameDemo( void )
{
}
//////////////////////////////////////////////////////////////////////////
bool gkGameDemo::OnInit()
{
	// load from startup.cfg

	// set camera
	ICamera* maincam = gEnv->p3DEngine->getMainCamera();
	maincam->setPosition(Vec3(0,-10,5.8f));
	maincam->setFOVy(DEG2RAD(60.f));
	maincam->setOrientation(Quat::CreateRotationXYZ(Ang3(DEG2RAD(-25),0,0)));
	maincam->setNearPlane(0.025f);
	maincam->setFarPlane(4000.f);

	gEnv->pGameFramework->LoadLevel(_T("level/dreamland/dreamland.gks"));
	gEnv->pInputManager->addInputEventListener(this);
	return true;
}
//////////////////////////////////////////////////////////////////////////
bool gkGameDemo::OnDestroy()
{
	//m_pMainPlayer->Destroy();

	//gEnv->pInputManager->removeEventListener(this);
	return true;
}
//////////////////////////////////////////////////////////////////////////
bool gkGameDemo::OnUpdate()
{
	float fFrameTime = gEnv->pTimer->GetFrameTime();

	return true;
}
//////////////////////////////////////////////////////////////////////////
bool gkGameDemo::OnLevelLoaded()
{

	return true;
}
//////////////////////////////////////////////////////////////////////////
bool gkGameDemo::OnInputEvent( const SInputEvent &event )
{
	


	return false;
}
