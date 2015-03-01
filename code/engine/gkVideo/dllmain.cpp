#include "ISystem.h"

#ifndef _STATIC_LIB
#include "gkPlatform_impl.h"
#endif

#include "gkVideo.h"

gkVideo* g_video = NULL;
#ifndef _STATIC_LIB
extern "C" void DLL_EXPORT gkModuleInitialize(SSystemGlobalEnvironment* pEnv) throw()
{
	if (pEnv)
	{
		gEnv = pEnv;
		g_video = new gkVideo();
		pEnv->pVideo = g_video;
	}

}

extern "C" void DLL_EXPORT gkModuleUnload(void)
{
	delete g_video;
}
#else
void gkLoadStaticModule_gkTrackBus( SSystemGlobalEnvironment* pEnv )
{
	if (pEnv)
	{
		gEnv = pEnv;
		g_video = new gkVideo();
		pEnv->pVideo = g_video;
	}
}

void gkFreeStaticModule_gkTrackBus()
{
	delete g_video;
}
#endif