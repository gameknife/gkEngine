// dllmain.cpp : Defines the entry point for the DLL application.
#include "StableHeader.h"

#ifndef _STATIC_LIB
#include "gkPlatform_impl.h"
#endif

#include "gkAnimationHavok.h"

// Platform initialization
//#include <Common/Base/System/Init/PlatformInit.cxx>

// Keycode
#include <Common/Base/keycode.cxx>


// #if !defined HK_FEATURE_PRODUCT_PHYSICS
// #error Physics is needed to build this demo. It is included in the common package for reference only.
// #endif

// This excludes libraries that are not going to be linked
// from the project configuration, even if the keycodes are
// present

#undef HK_FEATURE_PRODUCT_PHYSICS
#undef HK_FEATURE_PRODUCT_PHYSICS_2012
#undef HK_FEATURE_PRODUCT_AI
//#undef HK_FEATURE_PRODUCT_ANIMATION
#undef HK_FEATURE_PRODUCT_CLOTH
#undef HK_FEATURE_PRODUCT_DESTRUCTION
#undef HK_FEATURE_PRODUCT_BEHAVIOR
//#define HK_FEATURE_REFLECTION_PHYSICS
//#define HK_CLASSES_FILE <Common/Serialize/Classlist/hkClasses.h>
// #define HK_EXCLUDE_FEATURE_MemoryTracker
// #define HK_EXCLUDE_FEATURE_SerializeDeprecatedPre700
// #define HK_EXCLUDE_FEATURE_RegisterVersionPatches 
// #define HK_EXCLUDE_LIBRARY_hkGeometryUtilities

#include <Common/Base/Config/hkProductFeatures.cxx>

//gkHavok* g_Havok;

class gkAnimationHavok* g_Animation;

gkAnimationHavok* getAnimationPtr()
{
	return g_Animation;
};

#ifndef _STATIC_LIB
extern "C" void DLL_EXPORT gkModuleInitialize(SSystemGlobalEnvironment* pEnv) throw()
{
	if (pEnv)
	{
		gEnv = pEnv;
		g_Animation = new gkAnimationHavok();
		pEnv->pAnimation = g_Animation;
	}

}

extern "C" void DLL_EXPORT gkModuleUnload(void)
{
	delete g_Animation;
}

#else

void gkLoadStaticModule_gkAnimation(SSystemGlobalEnvironment* pEnv)
{
	if (pEnv)
	{
		gEnv = pEnv;
		g_Animation = new gkAnimationHavok();
		pEnv->pAnimation = g_Animation;
	}
}

void gkLoadStaticModule_gkAnimation()
{
	delete g_Animation;
}

#endif

int GetEngineOffset( char const* name)
{
	return 0xEFFFC5F4;
}

void hkVisualizeRegister()
{
    
}
