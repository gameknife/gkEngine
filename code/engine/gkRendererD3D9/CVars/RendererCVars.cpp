#include "RendererD3D9StableHeader.h"


gkRendererCVars* g_pRendererCVars=0;

gkRendererCVars::gkRendererCVars()
{
	REGISTER_CVARINT_DESC(r_aux, 
		_T("Toggles Aux Helper Rendering.\n")
		_T("Usage: r_aux [0/1]\n")
		_T("Default is 1 (on). Set to 0 to diable aux rendering."), 1);

	REGISTER_CVARINT_DESC(r_disinfo, 
		_T("Toggles Display Info Rendering.\n")
		_T("Usage: r_disinfo [0/1]\n")
		_T("Default is 1 (on). Set to 0 to diable aux rendering."), 1);

	REGISTER_CVARINT_DESC(r_defaultmat, 
		_T("Toggles Display Info Rendering.\n")
		_T("Usage: r_disinfo [0/1]\n")
		_T("Default is 1 (on). Set to 0 to diable aux rendering."), 0);


	REGISTER_CVARINT_DESC(r_MTRendering, 
		_T("Toggles MultiThread Rendering.\n")
		_T("Usage: r_MTRendering [0/1]\n")
		_T("Default is 0 (on). Set to 1 to enable."), 0);


	

	REGISTER_CVARFLOAT_DESC( r_SSAOScale,
		_T("SSAO对比度.\n")
		_T("Usage: r_SSAOConstract [0.0 - 2.0]\n")
		_T("默认为1.0"), 0.1f, 10.0f, 1.0f);

	REGISTER_CVARFLOAT_DESC( r_SSAOBias,
		_T("SSAO对比度.\n")
		_T("Usage: r_SSAOConstract [0.0 - 2.0]\n")
		_T("默认为1.0"), 0.0f, 0.5f, 0.02f);

	REGISTER_CVARFLOAT_DESC( r_SSAOAmount,
		_T("SSAO对比度.\n")
		_T("Usage: r_SSAOConstract [0.0 - 2.0]\n")
		_T("默认为1.0"), 0.1f, 20.0f, 1.0f);

	REGISTER_CVARFLOAT_DESC(r_SSAORadius, 
		_T("SSAO采样半径\n")
		_T("Usage: r_SSAORadius [0.0 - 5.0]\n")
		_T("默认为1.5 (米)."), 0.01f, 10.0f, 1.5f);	

	
	REGISTER_CVARFLOAT_DESC(r_shadowSlopeBias, 
		_T("阴影坡度偏移\n")
		_T("Usage: r_shadowmapsize [32 - 2048]\n")
		_T("默认为1024."), 0.0, 90.0, 0.05);	

	REGISTER_CVARFLOAT_DESC(r_shadowmapsize, 
		_T("阴影贴图大小\n")
		_T("Usage: r_shadowmapsize [32 - 2048]\n")
		_T("默认为1024."), 32, 2048, 1024);	
	
	REGISTER_CVARFLOAT_DESC(r_ssaodownscale,
		_T("SSAO降采样次数\n")
		_T("Usage: r_ssaodownscale [0 - 2]\n")
		_T("默认为0"), 0, 2, 0);	

	REGISTER_CVARFLOAT_DESC(r_shadowmaskdownscale,
		_T("SHADOWMAP降采样次数\n")
		_T("Usage: r_shadowmapdownscale [0 - 2]\n")
		_T("默认为0"), 0, 2, 0);	


	REGISTER_CVARINT_DESC(sw_HDRMode, 
		_T("Toggles Aux Helper Rendering.\n")
		_T("Usage: r_aux [0/1]\n")
		_T("Default is 1 (on). Set to 0 to diable aux rendering."), 1);
	

	REGISTER_CVARFLOAT(r_HDRLevel);
	REGISTER_CVARFLOAT(r_HDRBrightThreshold);
	REGISTER_CVARFLOAT(r_HDROffset);
	REGISTER_CVARFLOAT_DESC(r_HDRGrain, _T("HDR噪点，提高亮度"), 1.0, 5.0, 2.0);

	REGISTER_CVARINT_DESC(r_PostMsaa, _T("后处理超采样抗锯齿，0:关, 1:开"), 1);
	REGISTER_CVARINT_DESC(r_PostMsaaMode, _T("后处理超采样抗锯齿模式，0:普通, 1:SMAA1TX, 2:SMAA2TX"), 1);

	
	REGISTER_CVARINT_DESC(r_HDRRendering, _T("HDR渲染，0:关, 1:开"), 1);
	REGISTER_CVARINT_DESC(r_SSAO, _T("屏幕空间遮蔽，0:关, 1:开"), 1);
	REGISTER_CVARINT_DESC(r_Shadow, _T("全局阳光阴影，0:关, 1:开"), 1);

	REGISTER_CVARFLOAT_DESC(r_GSMShadowConstbia, _T("阳光阴影偏移"), 0.0005, 0.1, 0.001);
	REGISTER_CVARFLOAT_DESC(r_GSMShadowPenumbraStart, _T("阳光半影开始距离"), 0.00001, 0.5, 0.001);
	REGISTER_CVARFLOAT_DESC(r_GSMShadowPenumbraLengh, _T("阳光半影距离"), 0.0, 50.0, 10.0);

	REGISTER_CVARINT_DESC(r_DebugDynTex, _T("调试动态纹理，0:关, 1:开"), 0);
	REGISTER_CVARINT_DESC(r_DebugDynTexType, _T("调试动态纹理类别，0:全, 1:HDR, 2:阴影, 3:延迟光照, 4:后处理"), 0);

	REGISTER_CVARINT_DESC(r_ProfileGpu, _T("监控GPU时间，0:关, 1:各模块统计显示"), 0);

	REGISTER_CVARINT_DESC(r_dof, _T("DOF开关，0:关, 1:开"), 1);

	REGISTER_CVARINT_DESC(r_ssrl, _T("屏幕空间反射开关，0:关, 1:开"), 1);
	

	REGISTER_CVARINT_DESC(r_ShadingMode, _T("着色方式，0:延迟光照，1:延迟着色 2:前向渲染"), 0);
	
	REGISTER_CVARINT_DESC(r_sharpenpass, _T("附加锐化，0:关，1:开 2:自动(降像素渲染时开启)"), 2);
	
	REGISTER_CVARINT_DESC(r_stereo, _T("立体，0:关，1：开"), 0);

	REGISTER_CVARFLOAT_DESC(r_pixelscale,
		_T("像素缩放比\n")
		_T("Usage: r_pixelscale [0.5]\n")
		_T("默认为1.0 (原始比例)."), 0.25, 1.0, 1.0);
}

gkRendererCVars::~gkRendererCVars()
{

}