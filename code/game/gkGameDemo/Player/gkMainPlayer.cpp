#include "StableHeader.h"
#include "gkMainPlayer.h"
#include "IGameObjectLayer.h"
#include "IHavok.h"

#define CAM_DISTANCE_FAR 10.0f
#define CAM_DISTANCE_NEAR 0.5f
#define CAM_ROT_DAMPING 0.03f
#define PLAYER_ROT_DAMPING 0.1f
//////////////////////////////////////////////////////////////////////////
gkMainPlayer::gkMainPlayer():
		m_pPlayer(NULL),
		m_pPlayerAnim(NULL)
{
	m_nCurrPlayerStatus = eMPS_Idle;
	m_nPrevPlayerStatus = eMPS_Walk;
	m_fCamDistance = 5.0f;
	m_fSpeed = 0;
	m_fMaxSpeed = 6;
	m_fAccelaration = 6.0f;
	m_nDirection = 1;

	m_vecDirection = Vec3(0,0,0);
	m_vecFinalDirection = Vec3(0,1,0);
	m_quatDesireCam.SetIdentity();
	m_bUiMode = 0;
	m_bInitialized= 0;
}
//////////////////////////////////////////////////////////////////////////
bool gkMainPlayer::Init(const Quat& camrot)
{
	// test stuff, init a player [4/7/2012 Kaiming]
	m_pPlayer = gEnv->pGameObjSystem->GetGameObjectByName(_T("testPlayer"));
	if( !m_pPlayer )
	{
		m_pPlayer = gEnv->pGameObjSystem->CreateAnimGameObject(_T("testPlayer"), _T("objects/characters/prophet/prophet.chr"), Vec3(0,0,0), Quat::CreateIdentity());
		if (m_pPlayer)
		{
			//IGameObjectRenderLayer* pRenderLayer = m_pPlayer->getRenderLayer();
			//pRenderLayer->setMaterialName(_T("objects/characters/faraa/faraa.mtl"));



			m_pPlayerAnim = reinterpret_cast<IGameObjectAnimLayer*>(m_pPlayer->getGameObjectLayer(eGL_AnimLayer));
			m_pPlayerAnim->playAnimation(_T("idle"), 1.0f);

			// Create Physic Layer By hand
			m_pPlayerPhysic = gEnv->pPhysics->CreatePhysicLayer();
			m_pPlayer->setGameObjectLayer( m_pPlayerPhysic );

			m_pPlayerPhysic->createCharacterController();

			m_pPlayer->setPosition(0,0,10);
			m_pPlayer->setScale(1,1,1);

			m_bInitialized = true;
		}
	}

	gEnv->pInputManager->addInputEventListener(this);

	m_quatDesireCam = camrot;
	m_quatDesirePlayerOrien = m_pPlayer->getOrientation();

	return true;
}
//////////////////////////////////////////////////////////////////////////
bool gkMainPlayer::Update( float fElapsedTime, bool bUimode )
{
	m_bUiMode = bUimode;

	if (!m_bInitialized)
	{
		return false;
	}

	ICamera* cam = gEnv->p3DEngine->getMainCamera();
	
	Quat quatCurrCam = cam->getOrientation();
	Quat quatTarget = Quat::CreateSlerp(quatCurrCam, m_quatDesireCam, CAM_ROT_DAMPING);
	cam->setOrientation(quatTarget.GetNormalized());

	cam->setPosition( m_pPlayer->getWorldPosition() + Vec3(0,0,1.65f) );
	cam->moveLocal(Vec3(0,-m_fCamDistance,0));

	Quat quatCurrPlayer = m_pPlayer->getOrientation();
	Quat quatPlayerTarget = Quat::CreateSlerp( quatCurrPlayer, m_quatDesirePlayerOrien, PLAYER_ROT_DAMPING);
	m_pPlayer->setOrientation( quatPlayerTarget );

	Vec3 camPos = cam->getPosition();
	if (camPos.z < 0.4f)
	{
		camPos.z = 0.4f;
		cam->setPosition(camPos);
	}

	if (m_nCurrPlayerStatus != m_nPrevPlayerStatus)
	{
		m_nPrevPlayerStatus = m_nCurrPlayerStatus;
		m_pPlayerAnim->stopAllAnimation(0.3f);
		switch( m_nCurrPlayerStatus )
		{
		case eMPS_Idle:
			m_pPlayerAnim->playAnimation(_T("idle"), 0.3f);
			break;
		case eMPS_Walk:
			m_pPlayerAnim->playAnimation(_T("idle"), 0.3f);
			m_pPlayerAnim->playAnimation(_T("walk_f"), 0.3f);
			m_pPlayerAnim->playAnimation(_T("run_f"), 0.3f);
			m_pPlayerAnim->setAnimationSpeed(_T("run_f"), 0.8f);
			break;
		case eMPS_Run:
			m_pPlayerAnim->playAnimation(_T("idle"), 0.3f);
			m_pPlayerAnim->playAnimation(_T("walk_f"), 0.3f);
			m_pPlayerAnim->playAnimation(_T("run_f"), 0.0f);
			m_pPlayerAnim->setAnimationSpeed(_T("run_f"), 0.8f);
			break;
		}
	}

	if (m_vecDirection.IsZero(0.001f))
	{
		m_nDirection = -2;
	}
	else
	{
		m_nDirection = 1;
	}

	switch(m_nCurrPlayerStatus)
	{
	case eMPS_Idle:
		//OnPlayerMoving(fElapsedTime, -1);
		break;
	case eMPS_Walk:
		OnPlayerMoving(fElapsedTime, m_nDirection);
		break;
	case eMPS_Run:
		OnPlayerMoving(fElapsedTime, m_nDirection);
		break;
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////
bool gkMainPlayer::Destroy()
{
	// test stuff, init a player [4/7/2012 Kaiming]
	if (m_pPlayer)
	{
		gEnv->pGameObjSystem->DestoryGameObject(m_pPlayer);
	}

	gEnv->pInputManager->removeEventListener(this);
	return true;
}
//////////////////////////////////////////////////////////////////////////
bool gkMainPlayer::OnInputEvent( const SInputEvent &event )
{
	float fFrameTime = gEnv->pTimer->GetFrameTime();
	static float speed = 5.0f;

	switch ( event.deviceId )
	{
		case eDI_Mouse:
			{
				if (m_bUiMode)
				{
					return false;
				}
				static bool holding = false;
				if (event.keyId == eKI_Mouse2)
				{
					if (event.state == eIS_Down)
					{
						holding = true;
						AdjustFacing();
					}
					else
					{
						holding = false;
					}
				}

				else if (event.keyId == eKI_MouseX)
				{
					ICamera* cam = gEnv->p3DEngine->getMainCamera();
					Quat qBefore = m_quatDesireCam;
					Ang3 aRot(qBefore);

					aRot.z -= event.value * 0.002f;
					//aRot.x -= vMouseDelta.y * 0.002f;

					Quat qRot = Quat::CreateRotationXYZ(aRot);
					qRot.Normalize();

					m_quatDesireCam = qRot;
					//cam->setOrientation(qRot);
				}
				else if (event.keyId == eKI_MouseY)
				{
					ICamera* cam = gEnv->p3DEngine->getMainCamera();
					Quat qBefore = m_quatDesireCam;
					Ang3 aRot(qBefore);

					aRot.x -= event.value * 0.002f;
					//aRot.x -= vMouseDelta.y * 0.002f;

					Quat qRot = Quat::CreateRotationXYZ(aRot);
					qRot.Normalize();

					m_quatDesireCam = qRot;
					//cam->setOrientation(qRot);	
				}
				else if (event.keyId == eKI_MouseWheelUp)
				{
					m_fCamDistance -= 0.1f;
					if (m_fCamDistance < CAM_DISTANCE_NEAR)
					{
						m_fCamDistance = CAM_DISTANCE_NEAR;
					}
					//clamp( m_fCamDistance, CAM_DISTANCE_NEAR, CAM_DISTANCE_FAR);
				}
				else if (event.keyId == eKI_MouseWheelDown)
				{
					m_fCamDistance += 0.1f;
					if (m_fCamDistance > CAM_DISTANCE_FAR)
					{
						m_fCamDistance = CAM_DISTANCE_FAR;
					}
					//clamp( m_fCamDistance, CAM_DISTANCE_NEAR, CAM_DISTANCE_FAR);
				}
				break;
			}
		case eDI_Keyboard:
			{
				if (event.keyId == eKI_W && event.state == eIS_Pressed)
				{
					m_nCurrPlayerStatus = eMPS_Walk;
					m_vecDirection += Vec3(0,-1,0);
					AdjustFacing();
					return false;
				}
				if (event.keyId == eKI_A && event.state == eIS_Pressed)
				{
					m_nCurrPlayerStatus = eMPS_Walk;
					m_vecDirection += Vec3(1,0,0);
					AdjustFacing();
					return false;
				}	
				if (event.keyId == eKI_S && event.state == eIS_Pressed)
				{
					m_nCurrPlayerStatus = eMPS_Walk;
					m_vecDirection += Vec3(0,1,0);
					AdjustFacing();
					return false;
				}
				if (event.keyId == eKI_D && event.state == eIS_Pressed)
				{
					m_nCurrPlayerStatus = eMPS_Walk;
					m_vecDirection += Vec3(-1,0,0);
					AdjustFacing();
					return false;
				}

				if (event.keyId == eKI_W && event.state == eIS_Released)
				{
					m_nCurrPlayerStatus = eMPS_Walk;
					m_vecDirection -= Vec3(0,-1,0);
					LastReleaseJudge( Vec3(0,-1,0) );
					AdjustFacing();
					return false;
				}
				if (event.keyId == eKI_A && event.state == eIS_Released)
				{
					m_nCurrPlayerStatus = eMPS_Walk;
					m_vecDirection -= Vec3(1,0,0);
					LastReleaseJudge( Vec3(1,0,0) );
					AdjustFacing();
					return false;
				}	
				if (event.keyId == eKI_S && event.state == eIS_Released)
				{
					m_nCurrPlayerStatus = eMPS_Walk;
					m_vecDirection -= Vec3(0,1,0);
					LastReleaseJudge( Vec3(0,1,0) );
					AdjustFacing();
					return false;
				}
				if (event.keyId == eKI_D && event.state == eIS_Released)
				{
					m_nCurrPlayerStatus = eMPS_Walk;
					m_vecDirection -= Vec3(-1,0,0);
					LastReleaseJudge( Vec3(-1,0,0) );
					AdjustFacing();
					return false;
				}
			}

	}

	return false;
}
//////////////////////////////////////////////////////////////////////////
void gkMainPlayer::OnActionForward()
{
	m_nCurrPlayerStatus = eMPS_Walk;
}
//////////////////////////////////////////////////////////////////////////
void gkMainPlayer::OnActionBackward()
{
	m_nCurrPlayerStatus = eMPS_Walk;
}
//////////////////////////////////////////////////////////////////////////
void gkMainPlayer::OnActionTurn()
{

}
//////////////////////////////////////////////////////////////////////////
void gkMainPlayer::OnActionTurnCam()
{

}
//////////////////////////////////////////////////////////////////////////
void gkMainPlayer::UpdateCam()
{

}
//////////////////////////////////////////////////////////////////////////
void gkMainPlayer::OnPlayerMoving( float fElapsedTime, int Direction )
{
	m_fSpeed += Direction * m_fAccelaration * fElapsedTime;

	// idle     + walk           + run

	// 0      m_fMaxSpeed/2     m_fMaxSpeed

	float weightidle = 1.0f - clamp( m_fSpeed / (0.5f * m_fMaxSpeed), 0.0f, 1.0f );
	float weightwalk = clamp( m_fSpeed / (0.5f * m_fMaxSpeed), 0.0f, 1.0f );
	if (m_fSpeed > 0.5f * m_fMaxSpeed)
	{
		weightwalk = 1.0f - clamp( (m_fSpeed - 0.5f * m_fMaxSpeed) / (0.5f * m_fMaxSpeed), 0.0f, 1.0f );
	}
	float weightrun = clamp( (m_fSpeed - 0.5f * m_fMaxSpeed) / (0.5f * m_fMaxSpeed), 0.0f, 1.0f );

	m_pPlayerAnim->setAnimationWeight(_T("idle"), weightidle);
	m_pPlayerAnim->setAnimationWeight(_T("walk_f"), weightwalk);
	m_pPlayerAnim->setAnimationWeight(_T("run_f"), weightrun);


	if (m_fSpeed > m_fMaxSpeed)
	{
		m_fSpeed = m_fMaxSpeed;
		m_nCurrPlayerStatus = eMPS_Run;
	}
	if (m_fSpeed < 0)
	{
		m_fSpeed = 0;
		m_nCurrPlayerStatus = eMPS_Idle;
	}

	m_pPlayer->translate( 0, m_fSpeed * fElapsedTime, 0, TS_LOCAL );
	
}

void gkMainPlayer::AdjustFacing()
{
	ICamera* cam = gEnv->p3DEngine->getMainCamera();
	// rotate player to cam front
	Vec3 pFront = cam->getDirection();
	pFront = Vec3::CreateProjection( pFront, Vec3(0,0,1) );
	pFront.Normalize();

	Quat qTmp = Quat::CreateRotationVDir(pFront);
	
	Vec3 normalizedDir;
	if ( m_vecDirection.IsZero(0.001f) )
	{
		normalizedDir = m_vecFinalDirection.GetNormalized();
	}
	else
	{
		normalizedDir = m_vecDirection.GetNormalized();
	}

	Quat qAdding = Quat::CreateRotationV0V1(Vec3(0,1,0), normalizedDir);
	m_quatDesirePlayerOrien = (qTmp * qAdding).GetNormalized();
	//m_pPlayer->setOrientation((qTmp * qAdding).GetNormalized());
}

void gkMainPlayer::LastReleaseJudge(Vec3& lastDirection)
{
	if (m_vecDirection.IsZero(0.001f))
		m_vecFinalDirection = lastDirection;
}
//////////////////////////////////////////////////////////////////////////
bool gkMainPlayer::TeleportPlayer( const Vec3& worldpos, const Quat& rotation )
{
	if (m_pPlayerPhysic)
	{
		m_pPlayer->setOrientation(rotation);
		return m_pPlayerPhysic->teleport(worldpos);
	}
	return false;
}

