// dllmain.cpp : Defines the entry point for the DLL application.
#include "StableHeader.h"
#include "dllmain.h"
#ifndef _STATIC_LIB

#ifdef RENDERAPI_GL330
#include <glo/glo_impl.hpp>
#endif

#include "gkPlatform_impl.h"

#endif

#include "gkRendererGL330.h"

gkRendererGL* g_RendererModule;

#ifndef _STATIC_LIB
extern "C" void DLL_EXPORT gkModuleInitialize(SSystemGlobalEnvironment* pEnv) throw()
{
	if (pEnv)
	{
		gEnv = pEnv;
		g_RendererModule = new gkRendererGL();
		pEnv->pRenderer = g_RendererModule;
	}
}

extern "C" void DLL_EXPORT gkModuleUnload(void)
{
	delete g_RendererModule;
}

#else
void gkLoadStaticModule_gkRendererGL330( SSystemGlobalEnvironment* pEnv )
{
	if (pEnv)
	{
		gEnv = pEnv;
		g_RendererModule = new gkRendererGL();
		pEnv->pRenderer = g_RendererModule;
	}
}

void gkFreeStaticModule_gkRendererGL330()
{
	delete g_RendererModule;
}
#endif