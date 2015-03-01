// dllmain.cpp : Defines the entry point for the DLL application.
#include "StableHeader.h"
#include "dllmain.h"

#include "gkPhysX.h"

#ifndef _STATIC_LIB
#include "gkPlatform_impl.h"
#endif


#include "gkMemoryLeakDetecter.h"


gkPhysX* g_PhysXLayer;

extern "C" void GKPHYSX_API gkModuleInitialize(SSystemGlobalEnvironment* pEnv) throw()
{
	if (pEnv)
	{
		gEnv = pEnv;
		g_PhysXLayer = new gkPhysX();
		pEnv->pPhysics = g_PhysXLayer;
	}

}

extern "C" void GKPHYSX_API gkModuleUnload(void)
{
	delete g_PhysXLayer;
}
