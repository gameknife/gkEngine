// dllmain.cpp : Defines the entry point for the DLL application.
#include "StableHeader.h"
#include "dllmain.h"
#include "ISystem.h"
#include "gkRendererGLES2.h"
#ifndef _STATIC_LIB
#include "gkPlatform_impl.h"
#endif

#include "gkMemoryLeakDetecter.h"

gkRendererGLES2* g_RendererModule;

#ifndef _STATIC_LIB
extern "C" void GKRENDERERGLES2API gkModuleInitialize(SSystemGlobalEnvironment* pEnv) throw()
{
	if (pEnv)
	{
		gEnv = pEnv;
		g_RendererModule = new gkRendererGLES2();
		pEnv->pRenderer = g_RendererModule;
	}
}

extern "C" void GKRENDERERGLES2API gkModuleUnload(void)
{
	delete g_RendererModule;
}

#else
void gkLoadStaticModule_gkRendererGLES2( SSystemGlobalEnvironment* pEnv )
{
	if (pEnv)
	{
		gEnv = pEnv;
		g_RendererModule = new gkRendererGLES2();
		pEnv->pRenderer = g_RendererModule;
	}
}

void gkFreeStaticModule_gkRendererGLES2()
{
	delete g_RendererModule;
}
#endif