#include "gkPlatform.h"
#include "ISystem.h"
#include "gkRendererVulkan.h"

#ifndef _STATIC_LIB
#include "gkPlatform_impl.h"
#endif

static gkRendererVulkan* g_vkRendererModule = NULL;

#ifndef _STATIC_LIB
extern "C" __declspec(dllexport) void gkModuleInitialize(SSystemGlobalEnvironment* pEnv) throw()
{
	if (pEnv)
	{
		gEnv = pEnv;
		g_vkRendererModule = new gkRendererVulkan();
		pEnv->pRenderer = g_vkRendererModule;
	}
}

extern "C" __declspec(dllexport) void gkModuleUnload(void)
{
	delete g_vkRendererModule;
	g_vkRendererModule = NULL;
}
#else
void gkLoadStaticModule_gkRendererVulkan(SSystemGlobalEnvironment* pEnv)
{
	if (pEnv)
	{
		gEnv = pEnv;
		g_vkRendererModule = new gkRendererVulkan();
		pEnv->pRenderer = g_vkRendererModule;
	}
}

void gkFreeStaticModule_gkRendererVulkan()
{
	delete g_vkRendererModule;
	g_vkRendererModule = NULL;
}
#endif
