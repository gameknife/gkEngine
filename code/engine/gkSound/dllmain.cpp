#include "ISystem.h"

#ifndef _STATIC_LIB
#include "gkPlatform_impl.h"
#endif

#include "gkSound.h"

gkSound* g_sound = NULL;
#ifndef _STATIC_LIB
extern "C" void DLL_EXPORT gkModuleInitialize(SSystemGlobalEnvironment* pEnv) throw()
{
	if (pEnv)
	{
		gEnv = pEnv;
		g_sound = new gkSound();
		pEnv->pSound = g_sound;
	}

}

extern "C" void DLL_EXPORT gkModuleUnload(void)
{
	delete g_sound;
}
#else
void gkLoadStaticModule_gkTrackBus( SSystemGlobalEnvironment* pEnv )
{
	if (pEnv)
	{
		gEnv = pEnv;
		g_sound = new gkSound();
		pEnv->pSound = g_sound;
	}
}

void gkFreeStaticModule_gkTrackBus()
{
	delete g_sound;
}
#endif