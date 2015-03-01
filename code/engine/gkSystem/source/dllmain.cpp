// dllmain.cpp : Defines the entry point for the DLL application.
#include "gkSystemStableHeader.h"
#include "dllmain.h"
#ifndef _STATIC_LIB
#include "gkPlatform_impl.h"
#endif
#include "gkSystem.h"

#if defined( OS_ANDROID )
#include <android/log.h>
#define  LOG_TAG    "gkENGINE"
#define  LOGW(...)  __android_log_print(ANDROID_LOG_WARN,LOG_TAG,__VA_ARGS__)
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#endif

#include "gkMemoryLeakDetecter.h"


gkSystem* g_System = NULL;

#ifndef _STATIC_LIB
extern "C" 
{
	GKSYSTEM_API ISystem*  gkModuleInitialize(void) throw()
	{
		//LOGE("gkModuleInitialize.");
		gEnv = new SSystemGlobalEnvironment;
		memset(gEnv,0,sizeof(SSystemGlobalEnvironment));
		g_System = new gkSystem();

		gEnv->pSystem = g_System;
		//LOGE("gkModuleInitialized.");
		return g_System;
	}
};

extern "C" void GKSYSTEM_API gkModuleUnload(void)
	{
		SAFE_DELETE( g_System );
		SAFE_DELETE( gEnv );
	}

#else
ISystem* gkLoadStaticModule_gkSystem()
{
	//LOGE("gkModuleInitialize.");
	gEnv = new SSystemGlobalEnvironment;
	memset(gEnv,0,sizeof(SSystemGlobalEnvironment));
	g_System = new gkSystem();

	gEnv->pSystem = g_System;
	//LOGE("gkModuleInitialized.");
	return g_System;
}

void gkFreeStaticModule_gkSystem()
{
	SAFE_DELETE( g_System );
	SAFE_DELETE( gEnv );
}
#endif
