#include "stdafx.h"
#include "gkEditor.h"
#include "Managers\gkObjectManager.h"
#include "WorkspaceView.h"
#include "MainFrm.h"
#include "ITimer.h"
#include "IAuxRenderer.h"
#include "I3DEngine.h"
#include "ICamera.h"
#include "IResourceManager.h"
#include "StartBanner.h"
#include "Managers\gkCameraPosManager.h"
#include "gkFilePath.h"

static IEditor* s_pEditor = NULL;
//////////////////////////////////////////////////////////////////////////
IEditor* GetIEditor()
{
	return s_pEditor;
}

//-----------------------------------------------------------------------
gkEditor::gkEditor()
{
	s_pEditor = this;
	m_pObjMng = new gkObjectManager;
	m_pCamPosMng = new gkCameraPosManager;

	m_bIsSceneEmpty = true;
	m_bGameMode = false;


	// register check [12/27/2011 Kaiming]

}
//-----------------------------------------------------------------------
gkEditor::~gkEditor()
{
	SAFE_DELETE( m_pObjMng );
	SAFE_DELETE( m_pCamPosMng ); 
}

void gkEditor::Init(CMainFrame* pMainFrame)
{
	GK_ASSERT( pMainFrame ); 
	if (pMainFrame)
	{
		m_pMainFrame = pMainFrame;
		m_pMainVP = (CWorkspaceView*) pMainFrame->GetActiveView();
		GK_ASSERT(m_pMainVP);
	}

	gEnv->pInputManager->addInputEventListener(this);

	m_camTargetPos = Vec3(0,0, 5);
	m_camTargetQuat = Quat::CreateIdentity();
	m_camDamping = -1.0f;


	ICamera* maincam = gEnv->p3DEngine->getMainCamera();
	maincam->setFOVy(DEG2RAD(60.f));

	maincam->setNearPlane(0.025f);
	maincam->setFarPlane(4000.f);

	m_defaultFont = gEnv->pFont->CreateFont( _T("engine/fonts/msyh.ttf"), 12, GKFONT_OUTLINE_1PX );
}
//-----------------------------------------------------------------------
void gkEditor::Update()
{
	if (m_bIsSceneEmpty)
	{
 		gEnv->pRenderer->getAuxRenderer()->AuxRender3DGird(Vec3(0,0,0), 41, 1, ColorF(0.f, 0.f, 0.f, 0.2f));		
	}

	m_pObjMng->Update();

	if (m_pMainVP)
	 m_pMainVP->Update();

	for (TEditorUpdateListener::iterator i = m_listeners.begin(); i != m_listeners.end(); ++i)
	{
		(*i)->OnUpdate( gEnv->pTimer->GetFrameTime() );
	}

	//////////////////////////////////////////////////////////////////////////
	// update camera

	if (m_camDamping > 0)
	{
		ICamera* cam = gEnv->p3DEngine->getMainCamera();
		Vec3 posCam = cam->getPosition();
		Quat rotCam = cam->getDerivedOrientation();

		if ( posCam.IsEquivalent( m_camTargetPos,  0.01f ) && rotCam.IsEquivalent( m_camTargetQuat, 0.01f ))
		{
			m_camTargetQuat = rotCam;
			m_camTargetPos = posCam;
			m_camDamping = -1.0f;
			return;
		}

		posCam = Vec3::CreateLerp( posCam, m_camTargetPos, m_camDamping );
		rotCam = Quat::CreateSlerp( rotCam, m_camTargetQuat, m_camDamping );

		cam->setPosition( posCam );
		cam->setOrientation( rotCam );
	}

	m_mouseDelta.zero();
}
//-----------------------------------------------------------------------
void gkEditor::SetObjectSelMode( uint8 selmode )
{
	gkObjectManager::ms_objselmode = selmode;
}

void gkEditor::addUpdateListener( IEditorUpdateListener* listener )
{
	m_listeners.push_back(listener);
}

void gkEditor::removeUpdateListener( IEditorUpdateListener* listener )
{
	TEditorUpdateListener::iterator it = std::find(m_listeners.begin(),m_listeners.end(),listener);
	if (it != m_listeners.end()) m_listeners.erase( it );
}
//////////////////////////////////////////////////////////////////////////
void gkEditor::markSceneLoaded( int nProgress )
{
	m_bIsSceneEmpty = false;
	m_pMainFrame->SetProgressBar(nProgress);
}
//////////////////////////////////////////////////////////////////////////
void gkEditor::loadScene( LPCTSTR lpszPathName )
{
	_tcscpy_s( backupSceneFile, lpszPathName );
	gEnv->pSystem->LoadScene_s(lpszPathName, &m_pLoadCallback);

	gkStdString relpath = gkGetGameRelativePath( lpszPathName );

	TCHAR todfilename[MAX_PATH];
	_tcscpy_s(todfilename, relpath.c_str());
	todfilename[_tcslen(todfilename) - 4] = 0;
	_tcscat_s(todfilename, _T(".cam"));

	m_pCamPosMng->LoadUserDefineFile(todfilename);

}
//////////////////////////////////////////////////////////////////////////
void gkEditor::saveScene( LPCTSTR lpszPathName )
{
	TCHAR  bakFilename[MAX_PATH];
	_tcscpy_s(bakFilename, lpszPathName);
	_tcscat_s(bakFilename, _T(".bak"));
	CopyFile( lpszPathName, bakFilename, false );
	gEnv->pSystem->SaveScene(lpszPathName);

	// cam pos mng, save [5/24/2012 Kaiming]
	// write the same name tod file [1/8/2012 Kaiming]
	gkStdString relScenePath = gkGetGameRelativePath(lpszPathName);
	TCHAR todfilename[MAX_PATH];
	_tcscpy_s(todfilename, relScenePath.c_str());
	todfilename[_tcslen(todfilename) - 4] = 0;
	_tcscat_s(todfilename, _T(".cam"));
	relScenePath = gkGetGameRootDir() + todfilename;

	m_pCamPosMng->SaveUserDefineFile(relScenePath.c_str());
}

//////////////////////////////////////////////////////////////////////////
void CSceneLoadCallback::OnUpdateProgress( int nProgress )
{
	GetIEditor()->markSceneLoaded(nProgress);
}
//////////////////////////////////////////////////////////////////////////
void CSceneLoadCallback::OnInitProgress( const TCHAR* progressText )
{
	CStartBanner::SetText(progressText);
}


bool gkEditor::OnInputEvent( const SInputEvent &event )
{
	static bool lctrlhold = false;
	float fFrameTime = gEnv->pTimer->GetFrameTime();
	static float speed = 5.0f;


	switch( event.deviceId )
	{
	case eDI_Keyboard:
		{
			if (event.keyId == eKI_LCtrl)
			{

				if (event.state == eIS_Pressed)
				{
					lctrlhold = true;

					//gkLogMessage( typeid(IGameObject).name() );

				}
				else
				{
					lctrlhold = false;
				}
				return false;
			}
			if (event.keyId == eKI_G && event.state == eIS_Pressed && lctrlhold && !m_bGameMode)
			{
				gEnv->pGameFramework->InitGame(NULL);
				m_bGameMode = true;

				return false;
			}
			if (event.keyId == eKI_Escape && event.state == eIS_Pressed && m_bGameMode)
			{
				gEnv->pGameFramework->DestroyGame();
				m_bGameMode = false;

				return false;
			}
		}
	}

	if (m_bGameMode)
	{
		return false;
	}

	switch ( event.deviceId )
	{
	case eDI_Mouse:
		{
			static bool holding = false;
			if (event.keyId == eKI_Mouse2)
			{
				if (event.state == eIS_Down)
				{
					holding = true;
				}
				else
				{
					holding = false;
				}
			}
			else if (event.keyId == eKI_MouseX)
			{
				m_mouseDelta.x = event.value;

				if (holding)
				{
					Quat qBefore = gEnv->p3DEngine->getMainCamera()->getDerivedOrientation();
					Ang3 aRot(qBefore);

					aRot.z -= event.value * 0.002f;
					//aRot.x -= vMouseDelta.y * 0.002f;

					Quat qRot = Quat::CreateRotationXYZ(aRot);
					qRot.Normalize();
					gEnv->p3DEngine->getMainCamera()->setOrientation(qRot);	
				}

			
			}
			else if (event.keyId == eKI_MouseY && holding)
			{
				Quat qBefore = gEnv->p3DEngine->getMainCamera()->getDerivedOrientation();
				Ang3 aRot(qBefore);

				aRot.x -= event.value * 0.002f;
				//aRot.x -= vMouseDelta.y * 0.002f;

				Quat qRot = Quat::CreateRotationXYZ(aRot);
				qRot.Normalize();
				gEnv->p3DEngine->getMainCamera()->setOrientation(qRot);				
			}
			break;
		}
	case eDI_Keyboard:
		{
			if (event.keyId == eKI_LShift && event.state == eIS_Down)
			{
				speed = 50.0f;
			}
			else if (event.keyId == eKI_LShift && event.state == eIS_Released)
			{
				speed = 5.0f;
			}
			else if (event.keyId == eKI_W)
			{
				gEnv->p3DEngine->getMainCamera()->moveLocal( Vec3(0, fFrameTime * speed, 0) );
			}
			else if (event.keyId == eKI_S)
			{
				gEnv->p3DEngine->getMainCamera()->moveLocal( Vec3(0, -fFrameTime * speed, 0 ) );
			}
			else if (event.keyId == eKI_A)
			{
				gEnv->p3DEngine->getMainCamera()->moveLocal( Vec3(-fFrameTime * speed, 0, 0) );
			}
			else if (event.keyId == eKI_D)
			{
				gEnv->p3DEngine->getMainCamera()->moveLocal( Vec3(fFrameTime * speed, 0, 0 ) );
			}
			else if (event.keyId == eKI_P && event.state == eIS_Pressed)
			{
				gkTexturePtr colorGrading1 = gEnv->pSystem->getTextureMngPtr()->load(_T("default_colorchart_flipstyle.tga"), _T("internal"));
				gEnv->pRenderer->FX_ColorGradingTo(colorGrading1, 0.5f);
			}
			else if (event.keyId == eKI_LBracket && event.state == eIS_Pressed)
			{
				gkTexturePtr colorGrading1 = gEnv->pSystem->getTextureMngPtr()->load(_T("default_colorchart.tga"), _T("internal"));
				gEnv->pRenderer->FX_ColorGradingTo(colorGrading1, 0.5f);
			}
			else if (event.keyId == eKI_Up && event.state == eIS_Down)
			{
				float amount = gEnv->p3DEngine->getSnowAmount();
				amount += 0.5f * fFrameTime;
				if (amount > 1.0f)
				{
					amount = 1.0f;
				}
				gEnv->p3DEngine->setSnowAmount(amount);
			}
			else if (event.keyId == eKI_Down && event.state == eIS_Down)
			{
				float amount = gEnv->p3DEngine->getSnowAmount();
				amount -= 0.5f * fFrameTime;
				if (amount < 0.0f)
				{
					amount = 0.0f;
				}
				gEnv->p3DEngine->setSnowAmount(amount);
			}
			break;
		}
	case eDI_XBox:
		{
			if (event.keyId == eKI_Xbox_TriggerL && event.state == eIS_Changed)
			{
				speed = (event.value) * 45.0 + 15;
				return false;
			}

			if (event.keyId == eKI_Xbox_ThumbLY && event.state == eIS_Changed)
			{
				float thumbspeed = (event.value) * speed;
				gEnv->p3DEngine->getMainCamera()->moveLocal( Vec3(0, fFrameTime * thumbspeed, 0) );
				return false;
			}

			if (event.keyId == eKI_Xbox_ThumbLX && event.state == eIS_Changed)
			{
				float thumbspeed = (event.value) * speed;
				gEnv->p3DEngine->getMainCamera()->moveLocal( Vec3(fFrameTime * thumbspeed, 0, 0) );
				return false;
			}

			if (event.keyId == eKI_Xbox_ThumbRX && event.state == eIS_Changed)
			{
				Quat qBefore = gEnv->p3DEngine->getMainCamera()->getDerivedOrientation();
				Ang3 aRot(qBefore);

				aRot.z -= event.value * 0.015f;
				//aRot.x -= vMouseDelta.y * 0.002f;

				Quat qRot = Quat::CreateRotationXYZ(aRot);
				qRot.Normalize();
				gEnv->p3DEngine->getMainCamera()->setOrientation(qRot);			
				return false;
			}

			if (event.keyId == eKI_Xbox_ThumbRY && event.state == eIS_Changed)
			{
				Quat qBefore = gEnv->p3DEngine->getMainCamera()->getDerivedOrientation();
				Ang3 aRot(qBefore);

				aRot.x += event.value * 0.015f;
				//aRot.x -= vMouseDelta.y * 0.002f;

				Quat qRot = Quat::CreateRotationXYZ(aRot);
				qRot.Normalize();
				gEnv->p3DEngine->getMainCamera()->setOrientation(qRot);		
				return false;
			}
			break;
		}
	}
	return false;
}
//////////////////////////////////////////////////////////////////////////
gkSwissKnifeBar* gkEditor::getSwissKnifeBar()
{
	return &(m_pMainFrame->m_barSwissKnife);
}
//////////////////////////////////////////////////////////////////////////
void gkEditor::moveCameraTo( const Vec3& pos, const Quat& rot, float Damping /*= 0.1f */ )
{
	m_camTargetPos = pos;
	m_camTargetQuat = rot;
	m_camDamping = Damping;
}
//////////////////////////////////////////////////////////////////////////
void gkEditor::focusGameObject( IGameObject* pTarget, float Damping /*= 0.1f */ )
{
	if (!pTarget)
	{
		return;
	}
	ICamera* cam = gEnv->p3DEngine->getMainCamera();

	float radius = pTarget->getAABB().GetRadius();

	Vec3 targetpos = pTarget->getWorldPosition() - cam->getDerivedOrientation().GetColumn1() * (radius * 1.2 + 2.0f);

	moveCameraTo( targetpos, cam->getDerivedOrientation(), 0.25f );
}

void gkEditor::Destroy()
{
	m_pObjMng->Destroy();
}


