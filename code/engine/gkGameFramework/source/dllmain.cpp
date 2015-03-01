// dllmain.cpp : Defines the entry point for the DLL application.
#include "StableHeader.h"
#include "dllmain.h"
#include "gkGameFramework.h"
#ifndef _STATIC_LIB
#include "gkPlatform_impl.h"
#endif

#ifdef _STATIC_LIB

#if defined (OS_IOS)

ISystem* gkLoadStaticModule_gkSystem();
void gkFreeStaticModule_gkSystem();

#else
#include "..\gkSystem\include\dllmain.h"
#endif

#endif

gkGameFramework* g_GameFramework;

typedef ISystem* (*GET_SYSTEM)(void);
typedef void (*DESTROY_END)(void);

GET_SYSTEM pFuncStart;
DESTROY_END pFuncEnd;
HINSTANCE hHandle;

#ifndef _STATIC_LIB
extern "C" GKGAMEFRAMEWORK_API IGameFramework* gkModuleInitialize() throw()
{
	// create system.dll first
	hHandle = 0;
	gkOpenModule(hHandle, _T("gkSystem"));
	pFuncStart = (GET_SYSTEM)DLL_GETSYM(hHandle, "gkModuleInitialize");
	pFuncEnd = (DESTROY_END)DLL_GETSYM(hHandle, "gkModuleUnload");

	ISystem* system = pFuncStart();

	SSystemGlobalEnvironment* pEnv = system->getENV();
	if (pEnv)
	{
		gEnv = pEnv;
		g_GameFramework = new gkGameFramework();
		pEnv->pGameFramework = g_GameFramework;
	}

	return g_GameFramework;
}

extern "C" void GKGAMEFRAMEWORK_API gkModuleUnload(void)
{
	delete g_GameFramework;

	pFuncEnd();
	DLL_FREE(hHandle);
}
#else
IGameFramework* gkLoadStaticModule_gkGameFramework()
{
	ISystem* system = gkLoadStaticModule_gkSystem();
	SSystemGlobalEnvironment* pEnv = system->getENV();
	if (pEnv)
	{
		gEnv = pEnv;
		g_GameFramework = new gkGameFramework();
		pEnv->pGameFramework = g_GameFramework;
	}

	return g_GameFramework;
}

void gkFreeStaticModule_gkGameFramework()
{
	delete g_GameFramework;
	
	//gkFreeStaticModule_gkSystem();
}
#endif
