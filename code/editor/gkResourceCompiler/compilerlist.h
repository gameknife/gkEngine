#include "gmfCompiler.h"
#include "atcCompiler.h"
#include "shaderCompiler.h"
#include "txtCompiler.h"


gmfCompiler g_gmfCompiler;
#ifdef OS_WIN32
pvrCompiler g_gatcCompiler;
shaderCompiler g_shaderCompiler;
#endif
utf8Compiler g_utf8Compiler;