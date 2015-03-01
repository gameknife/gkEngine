// dllmain.cpp : Defines the entry point for the DLL application.
#include "StableHeader.h"
#include "dllmain.h"
#include "gkGameDemo.h"

#ifndef _STATIC_LIB
#include "gkPlatform_impl.h"
#endif

gkGameDemo* g_Game;

extern "C" GKGAMEDEMO_API IGame* gkModuleInitialize(SSystemGlobalEnvironment* pEnv) throw()
{
	if (pEnv)
	{
		gEnv = pEnv;
		g_Game = new gkGameDemo();
		//pEnv->pGameFramework = g_Game;
		return g_Game;
	}
	return NULL;
}

extern "C" void GKGAMEDEMO_API gkModuleUnload(void)
{
	delete g_Game;
}

