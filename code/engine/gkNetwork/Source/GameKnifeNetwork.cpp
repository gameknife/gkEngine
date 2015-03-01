// GameKnifeNetwork.cpp : Defines the exported functions for the DLL application.
//

#include "NetWorkStableHeader.h"
#include "GameKnifeNetwork.h"
#include "gkNetworkLayer.h"
#include "ISystem.h"

//#include "gkMemoryLeakDetecter.h"

gkNetworkLayer* g_NetworkModule;

extern "C" void GAMEKNIFENETWORK_API gkModuleInitialize(SSystemGlobalEnvironment* pEnv) throw()
{
	if (pEnv)
	{
		g_NetworkModule = new gkNetworkLayer();
		pEnv->pNetworkLayer = g_NetworkModule;
	}
}

extern "C" void GAMEKNIFENETWORK_API gkModuleUnload(void)
{
	delete g_NetworkModule;
}
