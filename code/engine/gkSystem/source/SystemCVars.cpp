#include "gkSystemStableHeader.h"
#include "SystemCVars.h"


gkSystemCVars* g_pSystemCVars=0;

gkSystemCVars::gkSystemCVars()
{
	REGISTER_CVARINT_DESC(sys_loadPakFirst, 
		_T("优先读取PAK内容.\n")
		_T("Usage: sys_loadPakFirst [0/1]\n")
		_T("默认为1 (开启)."), 1);

	REGISTER_CVARINT_DESC(sys_displayStats,
		_T("显示渲染参数.\n")
		_T("Usage: sys_displayStats [0/1]\n")
		_T("默认为1 (开启)."), 1);

// 	REGISTER_CVARFLOAT_DESC(sys_pixelScale,
// 		_T("像素缩放比\n")
// 		_T("Usage: sys_pixelScale [0.5]\n")
// 		_T("默认为1.0 (原始比例)."), 0.25, 1.0, 1.0);
	
}

gkSystemCVars::~gkSystemCVars()
{

}