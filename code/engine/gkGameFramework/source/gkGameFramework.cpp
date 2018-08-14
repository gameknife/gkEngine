#include "StableHeader.h"
#include "gkGameFramework.h"
#include "IGame.h"
#include "IRenderer.h"
#include "IAuxRenderer.h"
#include "ITimer.h"
#include "I3DEngine.h"
#include "ICamera.h"
#include "gkIniParser.h"

#define ANDROID_MOVETUMBE_STALLPIXEL 5

#ifdef _STATIC_LIB
IGame* gkLoadStaticModule_gkGame(SSystemGlobalEnvironment* gEnv);
void gkFreeStaticModule_gkGame();
#endif

//////////////////////////////////////////////////////////////////////////
gkGameFramework::gkGameFramework( void ):
#ifndef _STATIC_LIB
	m_pFuncGameEnd(NULL),
	m_pFuncGameStart(NULL),
#endif
	m_hHandleGame(NULL),
	m_pGame(NULL)
{
	m_uDeviceRot = -1;
	m_uDeviceMove = -1;
    m_vecMoveDir.set(0,0);

	m_deferredDestroy = 0;
    
    m_closeNextTime = false;
}

//////////////////////////////////////////////////////////////////////////
	bool gkGameFramework::Init( ISystemInitInfo& sii )
	{
		return gEnv->pSystem->Init(sii);
	}
//////////////////////////////////////////////////////////////////////////
bool gkGameFramework::PostInit( HWND hWnd, ISystemInitInfo& sii )
{
	gEnv->pSystem->PostInit(hWnd, sii);
	gEnv->pInputManager->addInputEventListener(this);

	return true;
}
//////////////////////////////////////////////////////////////////////////
void gkGameFramework::Run()
{
#if defined( OS_WIN32 )
	for(;;)
	{
		MSG msg;

		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			if (msg.message != WM_QUIT)
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				break;
			}
		}
		else
		{
			if (!Update())
			{
				// need to clean the message loop (WM_QUIT might cause problems in the case of a restart)
				// another message loop might have WM_QUIT already so we cannot rely only on this 
				while(PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
				break;
			}
		}
	}

#endif
}
//////////////////////////////////////////////////////////////////////////
bool gkGameFramework::Destroy()
{
	gEnv->pSystem->Destroy();
	return true;
}
//////////////////////////////////////////////////////////////////////////
bool gkGameFramework::Update()
{
	if (m_deferredDestroy > 0)
	{
		m_deferredDestroy--;
		if(m_deferredDestroy == 0)
		{
			DestroyGameImmediate();
			gEnv->pRenderer->RT_CleanRenderSequence();
		}
	}
	// render thread start here
	if( gEnv->pSystem->Update() )
	{
		if (m_pGame)
		{
			m_pGame->OnUpdate();
		}

		//float fFrameTime = gEnv->pTimer->GetFrameTime();
		//AndroidMoveUpdate(fFrameTime);

		// render thread end here
		gEnv->pSystem->PostUpdate();
	}



    if (m_closeNextTime) {
        return false;
    }


    
	return true;
}
//////////////////////////////////////////////////////////////////////////
void gkGameFramework::LoadLevel( const TCHAR* filename )
{
	gEnv->pSystem->LoadScene_s(filename, NULL);
}
//////////////////////////////////////////////////////////////////////////
bool gkGameFramework::InitGame(const TCHAR* dllname)
{
	gkLogMessage(_T("load Game..."));
	// ensure destroyed
	DestroyGameImmediate();

#ifndef _STATIC_LIB
	// check the dll name
	TCHAR name[MAX_PATH];

	gkIniParser startupFile( _T("config/startup.cfg") );

	startupFile.Parse();

	gkStdString ret = startupFile.FetchValue(  _T("launcher"), _T("gamedll") );

	if(ret != _T(""))
	{
		_tcscpy(name, ret.c_str());
	}
	else
	{
		_tcscpy(name, _T("TestCases"));
	}

	gkLogMessage(_T("load Game: %s"), name);

	gkStdString dllPath = gEnv->rootPath;
	dllPath += "\\bin64\\";
	// create system.dll first
	gkOpenModule( m_hHandleGame, name, dllPath.c_str());

	if (m_hHandleGame)
	{
		m_pFuncGameStart = (GET_GAMEDLL)DLL_GETSYM(m_hHandleGame, "gkModuleInitialize");
		m_pFuncGameEnd = (DESTROY_GAMEDLL)DLL_GETSYM(m_hHandleGame, "gkModuleUnload");


		m_pGame = m_pFuncGameStart(gEnv);
		m_pGame->OnInit();
	}
	else
	{
		gkLogWarning(_T("gkFramework::Load GameDll %s failed."), name );
	}
#else
    m_pGame = gkLoadStaticModule_gkGame(gEnv);
    m_pGame->OnInit();
#endif

	return true;
}

//////////////////////////////////////////////////////////////////////////
bool gkGameFramework::DestroyGameImmediate()
{
	if (m_pGame)
	{
		m_pGame->OnDestroy();
		m_pGame = NULL;
	}
#ifndef _STATIC_LIB
// 	if (m_pFuncGameEnd)
// 	{
// 		m_pFuncGameEnd();
// 		m_pFuncGameEnd = NULL;
// 	}
    
#else
    //gkFreeStaticModule_gkGame();
    
#endif
	if(m_hHandleGame)
	{
		gkFreeModule(m_hHandleGame);
		m_hHandleGame = 0;	
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
bool gkGameFramework::DestroyGame(bool deferred)
{
	m_deferredDestroy = 5;
	if (!deferred)
	{
		DestroyGameImmediate();
	}
	return true;
}
//////////////////////////////////////////////////////////////////////////
void gkGameFramework::OnLevelLoaded()
{
	if (m_pGame)
	{
		m_pGame->OnLevelLoaded();
	}
}

bool gkGameFramework::OnInputEvent( const SInputEvent &event )
{
 	if (m_pGame)
 	{
 		return false;
 	}

	if (gEnv->pSystem->IsEditor())
	{
		return false;
	}

	float fFrameTime = gEnv->pTimer->GetFrameTime();
	static float speed = 5.0f;

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

			else if (event.keyId == eKI_MouseX && holding)
			{
				Quat qBefore = gEnv->p3DEngine->getMainCamera()->getDerivedOrientation();
				Ang3 aRot(qBefore);

				aRot.z -= event.value * 0.002f;
				//aRot.x -= vMouseDelta.y * 0.002f;

				Quat qRot = Quat::CreateRotationXYZ(aRot);
				qRot.Normalize();
				gEnv->p3DEngine->getMainCamera()->setOrientation(qRot);				
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
	case eDI_Android:
    case eDI_IOS:
	{
		return AndroidFreeModeControl( event );

		break;
	}
        default:
            break;
            
            
	}
	return false;
}

bool gkGameFramework::AndroidFreeModeControl( const SInputEvent &event )
{
	int32 halfWidth = gEnv->pRenderer->GetScreenWidth() / 2;
	//int32 halfHeight = gEnv->pRenderer->GetScreenHeight() / 2;
	//float fFrameTime = gEnv->pTimer->GetFrameTime();

	if (event.keyId == eKI_Android_Touch && event.state == eIS_Pressed)
	{
		// judge left or right
		if ( event.value < halfWidth )
		{
			// left
			m_uDeviceMove = event.deviceIndex;
			m_vecMoveStartPos = Vec2( event.value, event.value2 );
		}
		else if ( event.value >= halfWidth )
		{
			m_uDeviceRot = event.deviceIndex;
			
		}

		//gkLogMessage(_T("Touch: %d | pos: %.1f, %.1f"), event.deviceIndex, event.value, event.value2);

		return false;
	}
	if (event.keyId == eKI_Android_Touch && event.state == eIS_Released)
	{
 		if ( m_uDeviceMove == event.deviceIndex )
 		{
 			m_uDeviceMove = -1;
            m_vecMoveDir.set(0,0);
 		}
 		else if ( m_uDeviceRot == event.deviceIndex  )
 		{
 			m_uDeviceRot = -1; 
 		}

		//gkLogMessage(_T("Release: %d"), event.deviceIndex);
		return false;
	}
	if (event.keyId == eKI_Android_DragX && event.state == eIS_Changed)
	{
		//gkLogMessage(_T("moving: %d"), event.deviceIndex);
		if (event.deviceIndex == m_uDeviceRot)
		{
			//gkLogMessage(_T("Rotating: %d"), event.deviceIndex);
			Quat qBefore = gEnv->p3DEngine->getMainCamera()->getDerivedOrientation();
			Ang3 aRot(qBefore);

			aRot.z -= event.value * 0.002f;
			//aRot.x -= vMouseDelta.y * 0.002f;

			Quat qRot = Quat::CreateRotationXYZ(aRot);
			qRot.Normalize();
			gEnv->p3DEngine->getMainCamera()->setOrientation(qRot);
		}
 		else if (event.deviceIndex == m_uDeviceMove)
 		{
			//gkLogMessage(_T("MoveX: %d | %.1f, %.1f"), event.deviceIndex, event.value, event.value2);
 			if ( event.value2 < (m_vecMoveStartPos.x - ANDROID_MOVETUMBE_STALLPIXEL) )
 			{
 				//moveleft
 				m_vecMoveDir.x = event.value2 - (m_vecMoveStartPos.x - ANDROID_MOVETUMBE_STALLPIXEL);
 			}
 			else if (  event.value2 > (m_vecMoveStartPos.x + ANDROID_MOVETUMBE_STALLPIXEL) )
 			{
 				// moveright             
                m_vecMoveDir.x = event.value2 - (m_vecMoveStartPos.x - ANDROID_MOVETUMBE_STALLPIXEL);
 			}
 			else {
                m_vecMoveDir.x = 0;
            }
 		}

		return false;
	}
	if (event.keyId == eKI_Android_DragY && event.state == eIS_Changed)
	{
		if (event.deviceIndex == m_uDeviceRot)
		{
			Quat qBefore = gEnv->p3DEngine->getMainCamera()->getDerivedOrientation();
			Ang3 aRot(qBefore);

			aRot.x -= event.value * 0.002f;
			//aRot.x -= vMouseDelta.y * 0.002f;

			Quat qRot = Quat::CreateRotationXYZ(aRot);
			qRot.Normalize();
			gEnv->p3DEngine->getMainCamera()->setOrientation(qRot);
		}
 		else if (event.deviceIndex == m_uDeviceMove)
 		{
			//gkLogMessage(_T("MoveY: %d | %.1f, %.1f"), event.deviceIndex, event.value, event.value2);
  			if ( event.value2 < (m_vecMoveStartPos.y - ANDROID_MOVETUMBE_STALLPIXEL) )
  			{
  				//moveback
                m_vecMoveDir.y = (m_vecMoveStartPos.y - ANDROID_MOVETUMBE_STALLPIXEL) - event.value2;
  				
  			}
  			else if (  event.value2 > (m_vecMoveStartPos.y + ANDROID_MOVETUMBE_STALLPIXEL) )
  			{
  				// moveforward
  				m_vecMoveDir.y = (m_vecMoveStartPos.y - ANDROID_MOVETUMBE_STALLPIXEL) - event.value2;
  			}
            else {
                m_vecMoveDir.y = 0;
            }
 
 		}
		return false;
	}

	return false;
}

bool gkGameFramework::AndroidMoveUpdate(float fElapsedTime)
{
    gEnv->p3DEngine->getMainCamera()->moveLocal( Vec3(fElapsedTime * m_vecMoveDir.x * 0.1f, fElapsedTime * m_vecMoveDir.y * 0.1f, 0) );
    
    return true;
}

