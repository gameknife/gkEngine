// dllmain.cpp : Defines the entry point for the DLL application.
#include "StableHeader.h"
#include "dllmain.h"
#include "gkGameObjectSystem.h"

#ifndef _STATIC_LIB
#include "gkPlatform_impl.h"
#endif

gkGameObjectSystem* g_goSystem;
#ifndef _STATIC_LIB
extern "C" void DLL_EXPORT gkModuleInitialize(SSystemGlobalEnvironment* pEnv) throw()
{
	if (pEnv)
	{
		gEnv = pEnv;
		g_goSystem = new gkGameObjectSystem();
		pEnv->pGameObjSystem = g_goSystem;
	}

}

extern "C" void DLL_EXPORT gkModuleUnload(void)
{
	delete g_goSystem;
}

#else
void gkLoadStaticModule_gkGameObjectSystem( SSystemGlobalEnvironment* pEnv )
{
	if (pEnv)
	{
		gEnv = pEnv;
		g_goSystem = new gkGameObjectSystem();
		pEnv->pGameObjSystem = g_goSystem;
	}
}

void gkFreeStaticModule_gkGameObjectSystem()
{
	delete g_goSystem;
}
#endif