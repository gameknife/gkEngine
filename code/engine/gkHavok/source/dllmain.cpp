// dllmain.cpp : Defines the entry point for the DLL application.
#include "StableHeader.h"

#include "dllmain.h"
#include "gkhavok.h"

#ifndef _STATIC_LIB
#include "gkPlatform_impl.h"
#include <Common/Base/keycode.cxx>
#undef HK_FEATURE_PRODUCT_AI
#undef HK_FEATURE_PRODUCT_ANIMATION
#undef HK_FEATURE_PRODUCT_CLOTH
#undef HK_FEATURE_PRODUCT_DESTRUCTION
#undef HK_FEATURE_PRODUCT_BEHAVIOR
#include <Common/Base/Config/hkProductFeatures.cxx>

int GetEngineOffset( char const* name)
{
	return 0xEFFFC5F4;
}

#ifdef _DEBUG
const hkQuadFloat32 g_vectorfConstants[HK_QUADREAL_END];
#endif

#else
const hkQuadFloat32 g_vectorfConstants[HK_QUADREAL_END];
#endif



gkHavok* g_Havok;
#ifndef _STATIC_LIB
extern "C" void GKHAVOK_API gkModuleInitialize(SSystemGlobalEnvironment* pEnv) throw()
{
	if (pEnv)
	{
		gEnv = pEnv;
		g_Havok = new gkHavok();
		pEnv->pPhysics = g_Havok;
	}

}

extern "C" void GKHAVOK_API gkModuleUnload(void)
{
	delete g_Havok;
}

#else

void gkLoadStaticModule_gkHavok(SSystemGlobalEnvironment* pEnv)
{
	if (pEnv)
	{
		gEnv = pEnv;
		g_Havok = new gkHavok();
		pEnv->pPhysics = g_Havok;
	}
}

void gkFreeStaticModule_gkHavok()
{
	delete g_Havok;
}

#endif

