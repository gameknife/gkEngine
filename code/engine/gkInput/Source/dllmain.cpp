//////////////////////////////////////////////////////////////////////////
//
// Name:   	GameKnifeInput.cpp
// Desc:	todo...
// 描述:	
// 
// Author:  Kaiming-Desktop	 
// Date:	11/25/2010 	
// 
//////////////////////////////////////////////////////////////////////////
#include "InputStableHeader.h"
#include "dllmain.h"
#include "ISystem.h"
#include "gkInputManager.h"
#ifndef _STATIC_LIB
#include "gkPlatform_impl.h"
#endif

#include "gkMemoryLeakDetecter.h"

gkInputManager* g_InputModule;
#ifndef _STATIC_LIB
extern "C" void GAMEKNIFEINPUT_API gkModuleInitialize(SSystemGlobalEnvironment* pEnv) throw()
{
	if (pEnv)
	{
		gEnv = pEnv;
		g_InputModule = new gkInputManager();
		pEnv->pInputManager = g_InputModule;
	}
}

extern "C" void GAMEKNIFEINPUT_API gkModuleUnload(void)
{
	SAFE_DELETE( g_InputModule );
}

#else
void gkLoadStaticModule_gkInput( SSystemGlobalEnvironment* pEnv )
{
	if (pEnv)
	{
		gEnv = pEnv;
		g_InputModule = new gkInputManager();
		pEnv->pInputManager = g_InputModule;
	}
}

void gkFreeStaticModule_gkInput()
{
	SAFE_DELETE( g_InputModule );
}
#endif
