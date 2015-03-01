#include "ISystem.h"

#ifndef _STATIC_LIB
#include "gkPlatform_impl.h"
#endif

#include "gkFont.h"

gkFont* g_font = NULL;
#ifndef _STATIC_LIB
extern "C" void DLL_EXPORT gkModuleInitialize(SSystemGlobalEnvironment* pEnv) throw()
{
	if (pEnv)
	{
		gEnv = pEnv;
		g_font = new gkFont();
		pEnv->pFont = g_font;
	}

}

extern "C" void DLL_EXPORT gkModuleUnload(void)
{
	delete g_font;
}
#else
void gkLoadStaticModule_gkFont( SSystemGlobalEnvironment* pEnv )
{
	if (pEnv)
	{
		gEnv = pEnv;
		g_font = new gkFont();
		pEnv->pFont = g_font;
	}
}

void gkFreeStaticModule_gkFont()
{
	delete g_font;
}
#endif