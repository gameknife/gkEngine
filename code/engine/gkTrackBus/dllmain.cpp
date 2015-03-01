#include "StableHeader.h"

#include "ISystem.h"

#ifndef _STATIC_LIB
#include "gkPlatform_impl.h"
#endif

#include "gkTrackBus.h"

gkTrackBus* g_trackbus = NULL;
#ifndef _STATIC_LIB
extern "C" void DLL_EXPORT gkModuleInitialize(SSystemGlobalEnvironment* pEnv) throw()
{
	if (pEnv)
	{
		gEnv = pEnv;
		g_trackbus = new gkTrackBus();
		pEnv->pTrackBus = g_trackbus;
	}

}

extern "C" void DLL_EXPORT gkModuleUnload(void)
{
	delete g_trackbus;
}
#else
void gkLoadStaticModule_gkTrackBus( SSystemGlobalEnvironment* pEnv )
{
	if (pEnv)
	{
		gEnv = pEnv;
		g_font = new gkFont();
		pEnv->pFont = g_font;
	}
}

void gkFreeStaticModule_gkTrackBus()
{
	delete g_font;
}
#endif