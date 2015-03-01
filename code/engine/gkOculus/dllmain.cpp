#include "ISystem.h"

#ifndef _STATIC_LIB
#include "gkPlatform_impl.h"
#endif

#include "gkOculus.h"

gkOculus* g_device = NULL;
#ifndef _STATIC_LIB
extern "C" void DLL_EXPORT gkModuleInitialize(SSystemGlobalEnvironment* pEnv) throw()
{
	if (pEnv)
	{
		gEnv = pEnv;
		g_device = new gkOculus();
		pEnv->pStereoDevice = g_device;
	}

}

extern "C" void DLL_EXPORT gkModuleUnload(void)
{
	delete g_device;
}
#else
void gkLoadStaticModule_gkFont( SSystemGlobalEnvironment* pEnv )
{
	if (pEnv)
	{
		gEnv = pEnv;
		g_device = new gkOculus();
		pEnv->pFont = g_device;
	}
}

void gkFreeStaticModule_gkFont()
{
	delete g_device;
}
#endif