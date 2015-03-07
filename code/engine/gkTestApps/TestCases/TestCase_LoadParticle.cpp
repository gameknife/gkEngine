#include "TestCaseFramework.h"
#include "TestCase_Template.h"
#include "ICamera.h"
#include "I3DEngine.h"
#include "TestCaseUtil_Cam.h"
#include "IFont.h"

TEST_CASE_FASTIMPL_HEAD( TestCase_LoadParticle, eTcc_Loading )

	IGameObject* m_particle;
	IFtFont* font;
	float m_camdist;

virtual void OnInit() 
{
	m_camdist = 20.0f;

	ICamera* maincam = gEnv->p3DEngine->getMainCamera();
	maincam->setPosition(0, -20, 2);
	maincam->setOrientation( Quat::CreateRotationXYZ( Ang3( DEG2RAD(-5), 0,0 ) ) );

	m_particle = gEnv->pGameObjSystem->CreateParticleGameObject( _T("objects/particle/default.ptc"), Vec3(0,0,0), Quat::CreateIdentity() );
}

virtual bool OnUpdate() 
{
	return true;
}

virtual void OnDestroy() 
{
	gEnv->pGameObjSystem->DestoryGameObject( m_particle );
	gEnv->pSystem->cleanGarbage();
}

virtual void OnInputEvent( const SInputEvent &event ) 
{
	HandleModelViewCam(event, 0.002f, m_camdist);
}

TEST_CASE_FASTIMPL_TILE( TestCase_LoadParticle )