// dllmain.cpp : Defines the entry point for the DLL application.
#include "RendererD3D9StableHeader.h"
#include "dllmain.h"
#include "ISystem.h"
#include "gkRendererD3D9.h"
#ifndef _STATIC_LIB
#include "gkPlatform_impl.h"
#endif

#include "gkMemoryLeakDetecter.h"

gkRendererD3D9* g_RendererModule;
#ifndef _STATIC_LIB
extern "C" void GKRENDERERD3D9_API gkModuleInitialize(SSystemGlobalEnvironment* pEnv) throw()
{
	if (pEnv)
	{
		gEnv = pEnv;
		g_RendererModule = new gkRendererD3D9();
		pEnv->pRenderer = g_RendererModule;
	}
}

extern "C" void GKRENDERERD3D9_API gkModuleUnload(void)
{
	delete g_RendererModule;
}
#else
void gkLoadStaticModule_gkRendererD3D9( SSystemGlobalEnvironment* pEnv )
{
	if (pEnv)
	{
		gEnv = pEnv;
		g_RendererModule = new gkRendererD3D9();
		pEnv->pRenderer = g_RendererModule;
	}
}

void gkFreeStaticModule_gkRendererD3D9()
{	
	delete g_RendererModule;
}
#endif
