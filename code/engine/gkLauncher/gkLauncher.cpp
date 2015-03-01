//////////////////////////////////////////////////////////////////////////
//
// K&K Studio GameKnife ENGINE Source File
//
// Name:   	gkLauncher.cpp
// Desc:	A Cross-Platform Launcher Solution
// 	
// Author:  Kaiming
// Date:	2012/6/10
// Modify:	
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _gkLauncher_cpp_
#define _gkLauncher_cpp_

#include "gkPlatform.h"
#include "IGameframework.h"
#include "ISystem.h"
#include "gkPlatform_impl.h"

#ifdef _STATIC_LIB
#include "..\gkGameFramework\include\dllmain.h"
#endif
#include "gkIniParser.h"

#include "gkMemoryLeakDetecter.h"



typedef IGameFramework* (*GET_SYSTEM)(void);
typedef void (*DESTROY_END)(void);

//--------------------------------------------------------------------------------------
// Initialize everything and go into a render loop
//--------------------------------------------------------------------------------------
INT WINAPI wWinMain( HINSTANCE, HINSTANCE, LPWSTR, int )
{
	// Enable run-time memory check for debug builds.
#if (defined(DEBUG) || defined(_DEBUG)) && defined( OS_WIN32 )
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

#ifdef _STATIC_LIB
	IGameFramework* game = gkLoadStaticModule_gkGameFramework();
#else
	// load gkSystem dll
	HINSTANCE hHandle = 0;
	gkOpenModule(hHandle, _T("gkGameFramework"));//, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
	GET_SYSTEM pFuncStart = (GET_SYSTEM)DLL_GETSYM(hHandle, "gkModuleInitialize");
	IGameFramework* game = pFuncStart();
#endif

	int width = 1280;
	int height = 720;

	// init
	ISystemInitInfo sii;
	sii.fWidth = width;
	sii.fHeight = height;
	
	game->Init( sii );
	game->PostInit( NULL, sii );

	game->InitGame( NULL );

	// run
	game->Run();

	// destroy
	game->DestroyGame(false);
	game->Destroy();

	// free
#ifdef _STATIC_LIB
	gkFreeStaticModule_gkGameFramework();
#else
	gkFreeModule( hHandle );
#endif
	return 0;
}

#endif

