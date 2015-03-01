//-----------------------------------------------------------------------------
// File: ksHDRProcess.fx
//
// Desc: 新的HDR渲染流程，参考Crytek3.3的调和方法
//
// Author: Yi Kaiming 2011/10/23
//-----------------------------------------------------------------------------

//////////////////////////////// Common samplers ////////////////
#include "./$postlib.h"

float4 PostMsaaParams = float4(1,1,1,1);
float4 PS_ScreenSize = float4(1,1,1.0/1280.0,1.0/720.0);

float4 kh = float4(3,3,3,3);
float4 kl = float4(2,2,2,2);
//float4 PS_ScreenSize = float4(1,1,1.0,1.0);
///////////////// pixel shader //////////////////
pixout PostMSAAPS(vert2frag IN)
{
	pixout OUT = (pixout) 0;
	//OUT.Color = tex2D(_tex0L, IN.baseTC.xy);


	//////////////////////////////////////////////////////////////////////////
	// simple 
	float4 cCurrFrame = tex2D(_tex1, IN.baseTC.xy - PostMsaaParams.xy);
	float4 cPrevFrame = tex2D(_tex0, IN.baseTC.xy - PostMsaaParams.zw);

#if SMAA1TX
	//////////////////////////////////////////////////////////////////////////
	// preserve sharp edge
	float4 cTL = tex2D(_tex1, IN.baseTC.xy - PostMsaaParams.xy + float2(-0.5, -0.5) * PS_ScreenSize.zw);
	float4 cBL = tex2D(_tex1, IN.baseTC.xy - PostMsaaParams.xy + float2(-0.5, 0.5) * PS_ScreenSize.zw);
	float4 cTR = tex2D(_tex1, IN.baseTC.xy - PostMsaaParams.xy + float2(0.5, -0.5) * PS_ScreenSize.zw);
	float4 cBR = tex2D(_tex1, IN.baseTC.xy - PostMsaaParams.xy + float2(0.5, 0.5) * PS_ScreenSize.zw);

	float4 cMax = max(cTL, max(cTR, max(cBL, cBR))); 
	float4 cMin = min(cTL, min(cTR, min(cBL, cBR))); 

	float4 wk = abs((cTL+cTR+cBL+cBR)*0.25-cCurrFrame); 

	OUT.Color =  lerp(cCurrFrame, clamp(cPrevFrame, cMin, cMax), saturate(rcp(lerp(kl, kh, wk))) ); 
#else
	OUT.Color = ( cCurrFrame + cPrevFrame ) * 0.5h;
#endif
	return OUT;
}

technique PostMSAA
{
	pass p0
	{
		VertexShader = null;

		ZEnable = false;
		ZWriteEnable = false;
		CullMode = None;


		PixelShader = compile ps_3_0 PostMSAAPS();     
	}
}
