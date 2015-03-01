#include "gkStableHeader.h"

#include "dllmain.h"
#ifndef _STATIC_LIB
#include "gkPlatform_impl.h"
#endif
#include "ISystem.h"
#include "gkMemoryLeakDetecter.h"
#include "gk3DEngine.h"


gk3DEngine* g_3DEngineModule;

#ifndef _STATIC_LIB
extern "C" void GAMEKNIFECORE_API gkModuleInitialize(SSystemGlobalEnvironment* pEnv) throw()
{
	if (pEnv)
	{
		gEnv = pEnv;
		gkLogMessage(_T("gk3DEngine initializing..."));
		g_3DEngineModule = new gk3DEngine();
		pEnv->p3DEngine = g_3DEngineModule;
	}
}

extern "C" void GAMEKNIFECORE_API gkModuleUnload(void)
{
	delete g_3DEngineModule;
}
#else
void gkLoadStaticModule_gkCore( SSystemGlobalEnvironment* pEnv )
{
	if (pEnv)
	{
		gEnv = pEnv;
		g_3DEngineModule = new gk3DEngine();
		pEnv->p3DEngine = g_3DEngineModule;
	}
}

void gkFreeStaticModule_gkCore()
{
	delete g_3DEngineModule;
}

#endif
