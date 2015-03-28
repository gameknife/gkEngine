// gkSystem.cpp : Defines the exported functions for the DLL application.
//

#include "gkSystemStableHeader.h"
#include "gkSystem.h"
#include "IRenderer.h"
#include "IInputManager.h"

#include "IResourceManager.h"
#include "gkSceneBuilder.h"
#include "ICamera.h"
#include "gkLog.h"
#include "IAuxRenderer.h"

#ifdef OS_WIN32
#include "INetworkLayer.h"
#	if defined(USE_CRASHRPT)
#	include "CrashRpt/CrashRpt.h"
#	endif
#include "IHavok.h"
#endif

#include "I3DEngine.h"


#include "ITimeOfDay.h"
#include "SystemCVars.h"


#include "IGameObject.h"
#include "IGameObjectLayer.h"
#include "IGameFramework.h"
#include "IAnimation.h"
#include "gkMeshFileLoader.h"
#include "gkInGUI.h"

#include "../gkSystemInfo.h"
#include "../gkTaskDispatcher.h"

#ifdef OS_WIN32
#	include "FileChangeMonitor.h"
#endif
#include "gkIniParser.h"
#include "IFont.h"
#include "ITrackBus.h"
#include "IStereoDevice.h"

extern CMTRand_int32 g_random_generator;

#ifdef _STATIC_LIB

#ifdef OS_IOS
void gkLoadStaticModule_gkCore(SSystemGlobalEnvironment* gEnv);
void gkFreeStaticModule_gkCore();

//void gkLoadStaticModule_gkRendererGLES2(SSystemGlobalEnvironment* gEnv);
//void gkFreeStaticModule_gkRendererGLES2();

#ifdef OS_APPLE
void gkLoadStaticModule_gkRendererGL330(SSystemGlobalEnvironment* gEnv);
void gkFreeStaticModule_gkRendererGL330();
#else
void gkLoadStaticModule_gkRendererGLES2(SSystemGlobalEnvironment* gEnv);
void gkFreeStaticModule_gkRendererGLES2();
#endif

void gkLoadStaticModule_gkGameObjectSystem(SSystemGlobalEnvironment* gEnv);
void gkFreeStaticModule_gkGameObjectSystem();

void gkLoadStaticModule_gkInput(SSystemGlobalEnvironment* gEnv);
void gkFreeStaticModule_gkInput();

//void gkLoadStaticModule_gkHavok(SSystemGlobalEnvironment* gEnv);
//void gkFreeStaticModule_gkHavok();

void gkLoadStaticModule_gkFont(SSystemGlobalEnvironment* pEnv);
void gkFreeStaticModule_gkFont();

void gkLoadStaticModule_gkAnimation(SSystemGlobalEnvironment* pEnv);
void gkFreeStaticModule_gkAnimation();

#else

void gkLoadStaticModule_gkCore(SSystemGlobalEnvironment* gEnv);
void gkFreeStaticModule_gkCore();

void gkLoadStaticModule_gkRendererD3D9(SSystemGlobalEnvironment* gEnv);
void gkFreeStaticModule_gkRendererD3D9();

void gkLoadStaticModule_gkGameObjectSystem(SSystemGlobalEnvironment* gEnv);
void gkFreeStaticModule_gkGameObjectSystem();

void gkLoadStaticModule_gkInput(SSystemGlobalEnvironment* gEnv);
void gkFreeStaticModule_gkInput();

void gkLoadStaticModule_gkHavok(SSystemGlobalEnvironment* gEnv);
void gkFreeStaticModule_gkHavok();

void gkLoadStaticModule_gkFont(SSystemGlobalEnvironment* pEnv);
void gkFreeStaticModule_gkFont();

void gkLoadStaticModule_gkAnimation(SSystemGlobalEnvironment* pEnv);
void gkFreeStaticModule_gkAnimation();

void gkLoadStaticModule_gkTrackBus(SSystemGlobalEnvironment* pEnv);
void gkFreeStaticModule_gkTrackBus();

void gkLoadStaticModule_gkSound(SSystemGlobalEnvironment* pEnv);
void gkFreeStaticModule_gkSound();
#endif

#endif



#include "gkMemoryLeakDetecter.h"


//////////////////////////////////////////////////////////////////////////

#if defined(OS_WIN32) && defined(USE_CRASHRPT)
//////////////////////////////////////////////////////////////////////////
// crash rpt
// Define the callback function that will be called on crash
BOOL WINAPI CrashCallback(LPVOID /*lpvState*/)
{  
	// Add two files to the error report: 
	// log file and application configuration file  
	crAddFile2(_T("log.txt"), NULL, _T("Log File"), CR_AF_MAKE_FILE_COPY);  
	crAddFile2(_T("config.ini"), NULL, _T("Configuration File"), CR_AF_MAKE_FILE_COPY);

	// We want the screenshot of the entire desktop is to be 
	// added on crash
	crAddScreenshot(CR_AS_VIRTUAL_SCREEN);   

	// Add a named property that means what graphics adapter is
	// installed on user's machine
	crAddProperty(_T("VideoCard"), _T("nVidia GeForce 8600 GTS"));

	return TRUE;
}

void CrashRptHead()
{
	// Define CrashRpt configuration parameters
	CR_INSTALL_INFO info;  
	memset(&info, 0, sizeof(CR_INSTALL_INFO));  
	info.cb = sizeof(CR_INSTALL_INFO);    
	info.pszAppName = _T("gkLauncher");  
	info.pszAppVersion = _T("1.0.0");  
	info.pszEmailSubject = _T("gkLauncher 1.0.0 Error Report");  
	info.pszEmailTo = _T("kaimingyi@gmail.com");    
	info.pszUrl = _T("http://myapp.com/tools/crashrpt.php");  
	info.pfnCrashCallback = CrashCallback;   
	info.uPriorities[CR_HTTP] = 3;  // First try send report over HTTP 
	info.uPriorities[CR_SMTP] = 2;  // Second try send report over SMTP  
	info.uPriorities[CR_SMAPI] = 1; // Third try send report over Simple MAPI    
	// Install all available exception handlers, use HTTP binary transfer encoding (recommended).
	info.dwFlags |= CR_INST_ALL_EXCEPTION_HANDLERS;
	info.dwFlags |= CR_INST_HTTP_BINARY_ENCODING; 
	info.dwFlags |= CR_INST_APP_RESTART; 
	info.pszRestartCmdLine = _T("/restart");
	// Define the Privacy Policy URL 
	info.pszPrivacyPolicyURL = _T("http://myapp.com/privacypolicy.html"); 

	// Install exception handlers
	int nResult = crInstall(&info);    
	if(nResult!=0)  
	{    
		// Something goes wrong. Get error message.
		TCHAR szErrorMsg[512];    
		szErrorMsg[0]=0;    
		crGetLastErrorMsg(szErrorMsg, 512);    
		printf("%s\n", szErrorMsg);    
		return;
	} 
}

void CrashRptTile()
{
	crUninstall();
}
#endif

//////////////////////////////////////////////////////////////////////////


//-----------------------------------------------------------------------
gkSystem::gkSystem():m_bEditor(false)
	,m_bIsSceneEmpty(true)
	,m_threadLoadAquire(false)
	,m_pLoadCallback(NULL)
	,m_bNeedGarbage(false)
{
    gEnv->pSystemInfo = new SSystemInfo();
	InitSystemInfo();

	gEnv->pTimer = &m_Timer;
	gEnv->pCVManager = &m_CVManager;
	gEnv->pProfiler = &m_Profiler;
	gEnv->pFileSystem = &m_ResFileManager;

    m_pLog = new gkLog(this);
	gEnv->pLog = m_pLog;

	m_wstrCurSceneFile = _T("");

	m_vecMeshReloadTasks.clear();

	memset( &m_moduleHandles, 0, sizeof(SModuleHandles) );

	m_bInitialized = false;
}

//-----------------------------------------------------------------------
gkSystem::~gkSystem( void )
{
	SAFE_DELETE( m_pLog );
	SAFE_DELETE( gEnv->pSystemInfo );
}


bool gkSystem::Init( ISystemInitInfo& sii )
{
	gkLogMessage(_T("Initializing gkSystem..."));
    
    std::vector<gkStdString> result;

	gkStdString path;

#ifdef OS_IOS
    path = gkGetExecRootDir();
#else
	path = gkGetExecRootDir() + _T("paks/");
#endif

	enum_all_files_in_folder(path.c_str(), result, false);

    for (int i=0; i < result.size(); ++i) {

		gkStdString file = result[i];

		const TCHAR* strLastSlash = _tcsrchr( file.c_str(), _T( '.' ) );
		if( strLastSlash )
		{
			if ( !_tcsicmp( strLastSlash, _T(".gpk") ) )
			{
				gkNormalizePath(file);
				gkStdString filename = gkGetFilename( file.c_str() );

#ifdef OS_IOS
				
#else
				file = _T("/paks") + file;
#endif

				gkLogMessage( _T("pak file [%s] loaded."), filename.c_str() );
				m_ResFileManager.openPak( file.c_str() );
			}
		}
    }
    
    

#ifdef OS_WIN32
	m_fileMonitor = new FileChangeMonitor();
	m_fileMonitor->StartMonitor();
	gEnv->pFileChangeMonitor = m_fileMonitor;
#endif
	m_bEditor = (sii.hInstance != 0);

	//CrashRptHead();

	m_bIsSceneEmpty = true;

	/************************************************************************/
	/* Module Loading
	/************************************************************************/
#ifdef WIN32
	TCHAR wszRenderDll[MAX_PATH] = _T("gkRendererD3D9");

	if (!IsEditor())
	{
		gkStdString cfgfile = gkGetExecRootDir() + _T("media/config/startup.cfg");
		GetPrivateProfileString( _T("launcher"), _T("renderer"), _T("gkRendererD3D9"), wszRenderDll, MAX_PATH, cfgfile.c_str() );
	}
#else 
	TCHAR wszRenderDll[MAX_PATH] = _T("gkRendererGLES2");
#endif

#ifndef _STATIC_LIB
	if ( !_tcsicmp( wszRenderDll, _T("gkRendererD3D9") ))
	{
		LOAD_MODULE_GLOBAL( m_moduleHandles.hRenderer, gkRendererD3D9 );
	}
	else if (  !_tcsicmp( wszRenderDll, _T("gkRendererGLES2") ) )
	{
		LOAD_MODULE_GLOBAL( m_moduleHandles.hRenderer, gkRendererGLES2 );
	}
	else if (  !_tcsicmp( wszRenderDll, _T("gkRendererGL330") ) )
	{
		LOAD_MODULE_GLOBAL( m_moduleHandles.hRenderer, gkRendererGL330 );
	}
#else
	//LOAD_MODULE_GLOBAL( m_moduleHandles.hRenderer, gkRendererGLES2 );
#ifdef OS_APPLE
    LOAD_MODULE_GLOBAL( m_moduleHandles.hRenderer, gkRendererGL330 );
#elif defined( OS_IOS )
    LOAD_MODULE_GLOBAL( m_moduleHandles.hRenderer, gkRendererGLES2 );
#else
	LOAD_MODULE_GLOBAL( m_moduleHandles.hRenderer, gkRendererD3D9 );
#endif
    
#endif
	

	LOAD_MODULE_GLOBAL(m_moduleHandles.h3DEngine, gkCore);
	gEnv->p3DEngine->Init();


	LOAD_MODULE_GLOBAL(m_moduleHandles.hInputLayer, gkInput);

	//LoadLib(m_moduleHandles.hNetworkLayer,		_T("gkNetwork"));


	//////////////////////////////////////////////////////////////////////////
	// load physics module
#ifndef MUTE_PHYSICS
#	ifdef WIN32
	TCHAR wszPhysicDll[MAX_PATH] = _T("");

	gkStdString cfgfile = gkGetExecRootDir() + _T("media/config/startup.cfg");
	GetPrivateProfileString( _T("launcher"), _T("physicsengine"), _T(""), wszPhysicDll, MAX_PATH, cfgfile.c_str() );
#	else 
	TCHAR wszPhysicDll[MAX_PATH] = _T("gkHavok");
#	endif // WIN32

#	ifndef _STATIC_LIB
	gkLoadModule( m_moduleHandles.hPhysics, wszPhysicDll );
#	else
	//LOAD_MODULE_GLOBAL( m_moduleHandles.hPhysics, gkHavok );
#	endif // _STATIC_LIB

#endif // MUTE_PHYSICS    
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	// load animation module
	{
#	ifdef WIN32
		TCHAR wszAnimationDll[MAX_PATH] = _T("gkAnimationHavok");

		gkStdString cfgfile = gkGetExecRootDir() + _T("media/config/startup.cfg");
		GetPrivateProfileString( _T("launcher"), _T("animationengine"), _T("gkAnimationHavok"), wszAnimationDll, MAX_PATH, cfgfile.c_str() );
#	else 
		TCHAR wszAnimationDll[MAX_PATH] = _T("gkAnimationHavok");
#	endif // WIN32

#ifdef OS_APPLE
        
#else
#	ifndef _STATIC_LIB
		gkLoadModule( m_moduleHandles.hAnimation, wszAnimationDll );
#	else
		LOAD_MODULE_GLOBAL( m_moduleHandles.hAnimation,				gkAnimation );
#	endif // _STATIC_LIB
#endif
    
    }
	//////////////////////////////////////////////////////////////////////////

	LOAD_MODULE_GLOBAL(m_moduleHandles.hGameObjectSystem,			gkGameObjectSystem);

	LOAD_MODULE_GLOBAL(m_moduleHandles.hFont,						gkFont);

#ifdef OS_WIN32
	LOAD_MODULE_GLOBAL(m_moduleHandles.hTrackBus,					gkTrackBus);

	LOAD_MODULE_GLOBAL(m_moduleHandles.hSound,						gkSound);

	//LOAD_MODULE_GLOBAL(m_moduleHandles.hVideo,						gkVideo);

	//LOAD_MODULE_GLOBAL(m_moduleHandles.hStereoDevice,				gkStereoDevice);

	{
		TCHAR wszStereoDevDll[MAX_PATH] = _T("null");
		gkStdString cfgfile = gkGetExecRootDir() + _T("media/config/startup.cfg");
		GetPrivateProfileString( _T("launcher"), _T("stereodevice"), _T("null"), wszStereoDevDll, MAX_PATH, cfgfile.c_str() );

		gkLoadModule( m_moduleHandles.hStereoDevice, wszStereoDevDll );
	}


#endif
    
	m_CVManager.initInput();
	gkLogMessage(_T("CVar Manager Initialized."));

	m_pSystemCVars = new gkSystemCVars;
	g_pSystemCVars = m_pSystemCVars;
	gkLogMessage(_T("System CVar Initialized."));

	m_CVManager.reloadAllParameters();

	// init Timer
	if (!m_Timer.Init(this))
		return (false);
	m_Timer.ResetTimer();

	gkLogMessage(_T("Timer Initialized."));


	gkIniParser startupFile( _T("config/startup.cfg") );
	startupFile.Parse();
	gkStdString ret = startupFile.FetchValue(  _T("launcher"), _T("width") );
	if( !ret.empty() )
	{
		gkStdStringstream ss(ret);
		ss >> sii.fWidth;
	}
	ret = startupFile.FetchValue(  _T("launcher"), _T("height") );
	if( !ret.empty() )
	{
		gkStdStringstream ss(ret);
		ss >> sii.fHeight;
	}



	TCHAR szProgressInfo[MAX_PATH];



	/************************************************************************/
	/* Physic System Initialize
	/************************************************************************/
#if !defined( MUTE_PHYSICS ) && defined( OS_WIN32 )
	_stprintf(szProgressInfo, _T("Initializing gkPhysics:%s ..."), wszPhysicDll);
	if (m_moduleHandles.hPhysics)
	{
		// init Havok
		if( sii.pProgressCallBack )
			sii.pProgressCallBack->OnInitProgress( szProgressInfo );
		gEnv->pPhysics->InitPhysics();
		Sleep(500);

		gkLogMessage(_T("Physics Initialized."));
	}
#else
	gEnv->pPhysics = NULL;
#endif

	gkTaskDispatcher* taskDispatcher = new gkTaskDispatcher();
	gEnv->pCommonTaskDispatcher = taskDispatcher;
	taskDispatcher->Init();

	_stprintf(szProgressInfo, _T("Initializing gkRenderer:%s ..."), wszRenderDll);
	/************************************************************************/
	/* Render System Initialize
	/************************************************************************/
	if (m_moduleHandles.hRenderer)
	{
		if( sii.pProgressCallBack )
			sii.pProgressCallBack->OnInitProgress( szProgressInfo );
		m_hWnd = gEnv->pRenderer->Init(sii);

		if (m_hWnd == NULL)
		{
			gkLogError(_T("FATAL ERROR: Renderer Load Failed. exit."));
			return false;
		}

		gkLogMessage(_T("Renderer Initialized."));
	}

#ifdef OS_WIN32
	/************************************************************************/
	/* Network System Initialize
	/************************************************************************/
	if (m_moduleHandles.hNetworkLayer)
	{
		if( sii.pProgressCallBack )
			sii.pProgressCallBack->OnInitProgress( _T("Initializing Network...") );
		gEnv->pNetworkLayer->InitSocket();

		gkLogMessage(_T("Network Initialized."));
	}
#endif
	/************************************************************************/
	/*  Animation System Initialize
	/************************************************************************/
	if( gEnv->pAnimation )
	{
		gEnv->pAnimation->InitAnimation();
	}

	/************************************************************************/
	/*  Font System Initialize
	/************************************************************************/
	if( gEnv->pFont )
	{
		gEnv->pFont->Init();
	}
	/************************************************************************/
	/* ResourceManager Initialize
	/************************************************************************/
	m_pTextureManager = gEnv->pRenderer->getResourceManager(GK_RESOURCE_MANAGER_TYPE_TEXTURE);
	m_pMaterialManager = gEnv->pRenderer->getResourceManager(GK_RESOURCE_MANAGER_TYPE_MATERIAL);
	m_pMeshManager = gEnv->pRenderer->getResourceManager(GK_RESOURCE_MANAGER_TYPE_MESH);
	m_pShaderManager = gEnv->pRenderer->getResourceManager(GK_RESOURCE_MANAGER_TYPE_SHADER);

	gkLogMessage(_T("Resource Managers Initialized."));

	/************************************************************************/
	/* Create MainCamera & Sun
	/************************************************************************/
	if (m_moduleHandles.h3DEngine)
	{
		ICamera* cam = gEnv->p3DEngine->createCamera(_T("MainCamera"));
		gEnv->p3DEngine->setMainCamera(cam);
		gkLogMessage(_T("3D Engine Camera Created."));

		gkLogMessage(_T("3D Engine Window Created."));
		gEnv->p3DEngine->createSun();
		gkLogMessage(_T("3D Engine Sun Created."));	
	}
	gkLogMessage(_T("3D Engine Initialized."));

	m_ingui = new gkInGUI;
	m_ingui->Init();
	gEnv->pInGUI = m_ingui;

	if (gEnv->pStereoDevice)
	{
		gEnv->pStereoDevice->Init();
	}

	return true;
}


bool gkSystem::PostInit( HWND hWnd, ISystemInitInfo& sii )
{
	// test staff, if not a editor, load default scene

	if (m_moduleHandles.hInputLayer)
	{
		// init Input
		if( sii.pProgressCallBack )
			sii.pProgressCallBack->OnInitProgress( _T("Initializing InputSystem...") );

		if (!IsEditor())
		{
			gEnv->pInputManager->Init(m_hWnd);
			gEnv->pInputManager->setHWND(m_hWnd);
		}
		else
		{
			m_hWnd = hWnd;
			gEnv->pInputManager->setHWND(hWnd);
			gEnv->pInputManager->Init(hWnd);
		}
		gkLogMessage(_T("InputLayer Initialized."));
	}


	m_CVManager.reloadAllParameters();

	g_random_generator.seed(timeGetTime());

	gkLogMessage(_T("gkSystem Initialized."));
	
	m_bInitialized = true;
	return true;
}

bool gkSystem::Destroy()
{
	m_ingui->Shutdown();
	SAFE_DELETE( gEnv->pInGUI );


	// gameobject destroy
	gEnv->pGameObjSystem->destroy();
		
	// save console vars first.
	if (IsEditor())
	{
		gEnv->pCVManager->saveAllParameters();
	}
	
	if( gEnv->pFont )
	{
		gEnv->pFont->Destroy();
	}

	
	// unload Network Layer
#ifdef OS_WIN32
	if ( gEnv->pNetworkLayer )
	{
		gEnv->pNetworkLayer->ClearSocket();
	}
#endif

	// unload Input Layer
	gEnv->pInputManager->Destroy();

	gEnv->p3DEngine->Destroy();
	UNLOAD_MODULE_GLOBAL(m_moduleHandles.h3DEngine,			gkCore);
	
	// unload renderer first [8/2/2011 Kaiming-Desktop]
	gEnv->pRenderer->Destroy();
	

	gkTaskDispatcher* taskDispatcher = static_cast<gkTaskDispatcher*>(gEnv->pCommonTaskDispatcher);
	taskDispatcher->Destroy();
	delete taskDispatcher;

	// unload Physics Layer
#ifdef OS_WIN32
	if (gEnv->pPhysics)
		gEnv->pPhysics->DestroyPhysics();
#endif
	
	
	//UNLOAD_MODULE_GLOBAL(m_moduleHandles.hRenderer,			gkRendererGLES2);
    //UNLOAD_MODULE_GLOBAL(m_moduleHandles.hRenderer,			gkRendererGL330);
#ifdef OS_APPLE
    UNLOAD_MODULE_GLOBAL( m_moduleHandles.hRenderer, gkRendererGL330 );
#elif defined( OS_IOS )
    UNLOAD_MODULE_GLOBAL( m_moduleHandles.hRenderer, gkRendererGLES2 );
#else
	UNLOAD_MODULE_GLOBAL( m_moduleHandles.hRenderer, gkRendererD3D9 );
#endif
	//FreeLib(m_moduleHandles.hNetworkLayer);

	UNLOAD_MODULE_GLOBAL(m_moduleHandles.hInputLayer,			gkInput);


#ifdef OS_WIN32
	UNLOAD_MODULE_GLOBAL(m_moduleHandles.hPhysics,			gkHavok);
#endif
    
	UNLOAD_MODULE_GLOBAL(m_moduleHandles.hGameObjectSystem,	gkGameObjectSystem);

	UNLOAD_MODULE_GLOBAL(m_moduleHandles.hFont,				gkFont);

#ifdef OS_WIN32
	UNLOAD_MODULE_GLOBAL(m_moduleHandles.hTrackBus,			gkTrackBus);

	UNLOAD_MODULE_GLOBAL(m_moduleHandles.hSound,				gkSound);
	//UNLOAD_MODULE_GLOBAL(m_moduleHandles.hVideo,				gkVideo);

	//UNLOAD_MODULE_GLOBAL(m_moduleHandles.hStereoDevice, VOID);
#endif


    
	// releas system cvars
	SAFE_DELETE( m_pSystemCVars );

#ifdef OS_WIN32
	m_fileMonitor->StopMonitor();
	SAFE_DELETE( m_fileMonitor );
#endif




	//CrashRptTile();

	return true;
}

bool gkSystem::Update()
{
	if (!m_bInitialized)
	{
		return false;
	}

	gEnv->pProfiler->setFrameBegin();

	gEnv->pProfiler->setElementStart(ePe_SThread_Cost);

#ifdef OS_WIN32
	m_fileMonitor->Update();
#endif

	if (m_threadLoadAquire)
	{
		LoadScene(m_wstrThreadCacheSceneFile.c_str());
		m_threadLoadAquire = false;
		gEnv->pRenderer->RT_SkipOneFrame();
	}

	if (!m_vecMeshReloadTasks.empty())
	{
		for (uint8 i=0; i<m_vecMeshReloadTasks.size(); ++i)
		{
			getMeshMngPtr()->reload(m_vecMeshReloadTasks[i]);
		}
		m_vecMeshReloadTasks.clear();
	}
  
	bool needSkip = false;
	needSkip = needSkip || ( getMaterialMngPtr() && getMaterialMngPtr()->prepare() );
	//////////////////////////////////////////////////////////////////////////
	// game obj system sync update
	needSkip = needSkip || ( gEnv->pGameObjSystem->syncupdate() );

	//////////////////////////////////////////////////////////////////////////
	// 3d engine sync update
	needSkip = needSkip || ( gEnv->p3DEngine->syncupdate() );

	if (m_bNeedGarbage)
	{
		gEnv->pSystem->getMaterialMngPtr()->cleanGarbage();
		gEnv->pSystem->getTextureMngPtr()->cleanGarbage();
		gEnv->pSystem->getMeshMngPtr()->cleanGarbage();

		needSkip = true;
		m_bNeedGarbage = false;

		gEnv->pRenderer->RT_CleanRenderSequence();
		return false;
	}

	if( needSkip )
	{
		gEnv->pRenderer->RT_SkipOneFrame();
	}

	//////////////////////////////////////////////////////////////////////////
	// update Time Module
	m_Timer.UpdateOnFrameStart();
	float fFrameTime = m_Timer.GetRealFrameTime();
	//////////////////////////////////////////////////////////////////////////

	m_ingui->Update(fFrameTime);

	gkTaskDispatcher* taskDispatcher = static_cast<gkTaskDispatcher*>(gEnv->pCommonTaskDispatcher);
	taskDispatcher->Update();

	//////////////////////////////////////////////////////////////////////////
	// Rendering Start

	// 1. Swap RenderSequence
	//gEnv->p3DEngine->_SwapRenderSequence();

	// 2. Strat RenderThread or Just MainThread Rendering
	if( gEnv->pStereoDevice )
	{
		gEnv->pStereoDevice->OnFrameBegin();
	}


	gEnv->pProfiler->setElementEnd(ePe_SThread_Cost);
	gEnv->pProfiler->setElementStart(ePe_MThread_Cost);


	gEnv->pProfiler->setElementStart(ePe_MT_Part1);
	gEnv->pRenderer->RT_StartRender();

	
	// rendering started in other thread
	//////////////////////////////////////////////////////////////////////////

#ifdef OS_WIN32
	//////////////////////////////////////////////////////////////////////////
	// update Havok
	// physx and animation update

	gEnv->pProfiler->setElementStart( ePe_Physic_Cost );

	if (gEnv->pPhysics)
	{
		gEnv->pPhysics->UpdatePhysics(fFrameTime);
	}

	gEnv->pProfiler->setElementStart( ePe_Physic_Cost );
#endif

    if (gEnv->pAnimation) {
        gEnv->pAnimation->_updateAnimation(fFrameTime);
    }

	gEnv->pProfiler->setElementEnd(ePe_MT_Part1);
	gEnv->pProfiler->setElementStart(ePe_MT_Part2);
	//////////////////////////////////////////////////////////////////////////
	// gameobjectsystem preupdate
	gEnv->pGameObjSystem->preupdate(fFrameTime);

	//////////////////////////////////////////////////////////////////////////
	// update Input Module
	gEnv->pInputManager->Update(fFrameTime);
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// update trackbus
    if(gEnv->pTrackBus)
        gEnv->pTrackBus->Update(fFrameTime);
	//////////////////////////////////////////////////////////////////////////

	return true;
}

//////////////////////////////////////////////////////////////////////////
bool gkSystem::PostUpdate()
{
	float fFrameTime = m_Timer.GetRealFrameTime();
	//////////////////////////////////////////////////////////////////////////
	// update ConsoleVar Manager
	m_CVManager.update(fFrameTime);


	//////////////////////////////////////////////////////////////////////////
	// update 3DEngine
	// tod system, terrian system, etc..

	gEnv->pProfiler->setElementStart( ePe_3DEngine_Cost );

	gEnv->pProfiler->setStartSceneManage();
	gEnv->p3DEngine->update(fFrameTime);
	gEnv->pProfiler->setEndSceneManage();

	gEnv->pProfiler->setElementEnd( ePe_3DEngine_Cost );


	gEnv->pGameObjSystem->update(fFrameTime);

#ifdef OS_WIN32
	//////////////////////////////////////////////////////////////////////////
	// update Havok
	// physx and animation update

	if (gEnv->pPhysics)
	{
		gEnv->pPhysics->PostUpdatePhysics();
	}
#endif
	
	gEnv->pProfiler->setElementEnd(ePe_MT_Part2);
	gEnv->pProfiler->setElementStart(ePe_MT_Part3);

	//////////////////////////////////////////////////////////////////////////
	// sync render thread
	gEnv->pProfiler->displayInfo();

	//if ( g_pRendererCVars->r_disinfo)
	{
		// Console
		if (gEnv->pCVManager->isConsoleEnable())
		{
			gEnv->pCVManager->render();
		}
	}
    gEnv->pProfiler->setElementEnd(ePe_MT_Part3);


	gEnv->pProfiler->setElementStart( ePe_Font_Cost );

	if( gEnv->pFont )
	{
		 gEnv->pFont->Flush();
	}

	gEnv->pProfiler->setElementEnd( ePe_Font_Cost );
	
	if( gEnv->pStereoDevice )
	{
		gEnv->pStereoDevice->Flush();
	}

	
	gEnv->pProfiler->setElementEnd(ePe_MThread_Cost);
	gEnv->pRenderer->RT_EndRender();

	if( gEnv->pStereoDevice )
	{
		gEnv->pStereoDevice->OnFrameEnd();
	}
   
	gEnv->pProfiler->setFrameEnd();
	
	return true;
}

//-----------------------------------------------------------------------
void gkSystem::SetEditorHWND( HWND drawHwnd, HWND mainHwnd)
{
	//gEnv->pRenderer->SetCurrContent(drawHwnd);
	//gEnv->pInputManager->setHWND(drawHwnd);

	m_bEditor = true;
}
//-----------------------------------------------------------------------
bool gkSystem::LoadScene( const TCHAR* filename )
{
	// find the last slash [9/27/2011 Kaiming]
	TCHAR* strStart = const_cast<TCHAR*>(filename);
	TCHAR* strLastSlash = NULL; 
	strLastSlash = _tcsrchr( strStart, L'\\' );
	if( strLastSlash )
		strStart = ++strLastSlash;


	gkLogMessage(_T("loading scene..."));

	// build new scene if new file input [9/4/2011 Kaiming-Desktop]
	if (m_wstrCurSceneFile != strStart)
	{
		//////////////////////////////////////////////////////////////////////////
		// destroying...

		// terrian
		gEnv->p3DEngine->destroyTerrianSync( gEnv->p3DEngine->getTerrian() );

		// GameObject call destroy
		gEnv->pGameObjSystem->destroy();
#ifdef OS_WIN32
		// Animation destroy
		if (gEnv->pAnimation)
			gEnv->pAnimation->DestroyAllAnimLayer();
#endif		
		// RenderLayer destroy
		gEnv->p3DEngine->destroyAllRenderLayer();

		// LightLayer destroy
		gEnv->p3DEngine->destroyAllLightLayer();

		

		// Texture destroy
		//getTextureMngPtr()->unloadAllByGroup(_T("external"));

		// Material destroy
		//getMaterialMngPtr()->unloadAllByGroup(_T("external"));

		//getMaterialMngPtr()->load(_T("material/_default.mtl"));

		// Mesh destroy
		//getMeshMngPtr()->unloadAll();

		// Shader not
		//getShaderMngPtr()->unloadAll();

		//cleanGarbage();


		//////////////////////////////////////////////////////////////////////////
		// rebuilding...

		// should move down to new module
		// test for scene resource loading
		gkSceneBuilder::buildSceneFromFile(_T("engine/assets/skybox_sphere.gks"), false, true, Vec3(0,0,0), RENDER_LAYER_SKIES_EARLY, true);
	}

	// param2: syncmode

	if ( _tcsicmp( filename, _T("void") ) )
	{
		gkSceneBuilder::buildSceneFromFile(filename, m_wstrCurSceneFile == strStart);
		m_wstrCurSceneFile = strStart;
	}


	m_bIsSceneEmpty = false;

	gEnv->pGameFramework->OnLevelLoaded();

	return true;
}

bool gkSystem::LoadScene_s( const TCHAR* filename, IProgressCallback* pCallback )
{
	m_wstrThreadCacheSceneFile = filename;

	m_threadLoadAquire = true;

	m_pLoadCallback = pCallback;

	return true;
}

bool gkSystem::reloadResource_s( uint8 type, const gkStdString& name )
{
	switch(type)
	{
	case GK_RESOURCE_MANAGER_TYPE_MESH:
		m_vecMeshReloadTasks.push_back(name);
		break;
	}

	return true;
}

bool gkSystem::SaveScene( const TCHAR* filename )
{
	gkSceneBuilder builder;
	builder.saveSceneToFile(filename);

	return true;
}


//////////////////////////////////////////////////////////////////////////
void gkSystem::updateProgress( int progress )
{
	if (m_pLoadCallback)
	{
		m_pLoadCallback->OnUpdateProgress(progress);
	}
}


void gkSystem::ReturnMeshLoader( IMeshLoader* target )
{
	delete target;
}

IMeshLoader* gkSystem::getOBJMeshLoader()
{
	return (new CObjMeshLoader());
}

IMeshLoader* gkSystem::getHKXMeshLoader()
{
	IMeshLoader* ret = NULL;

	ret = gEnv->pAnimation->getHKXMeshLoader();

	return ret;
}

void gkSystem::cleanGarbage()
{
	m_bNeedGarbage = true;
}


