// dllmain.cpp : Defines the entry point for the DLL application.
#include "StableHeader.h"
#include "dllmain.h"
#include "ISystem.h"
#include "gkRendererPrototype.h"
#ifndef _STATIC_LIB
#include "gkPlatform_impl.h"
#endif

#include "gkMemoryLeakDetecter.h"

gkRendererPrototype* g_RendererModule;

extern "C" void GKRENDERERPROTOTYPEAPI gkModuleInitialize(SSystemGlobalEnvironment* pEnv) throw()
{
	if (pEnv)
	{
		gEnv = pEnv;
		g_RendererModule = new gkRendererPrototype();
		pEnv->pRenderer = g_RendererModule;
	}
}

extern "C" void GKRENDERERPROTOTYPEAPI gkModuleUnload(void)
{
	delete g_RendererModule;
}