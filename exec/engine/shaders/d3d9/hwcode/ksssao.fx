//-----------------------------------------------------------------------------
// File: PP_SSAO.fx
//
// Desc: 屏幕空间环境光遮蔽技术, new version
//
// Author: Yi Kaiming 2011/10/25
//-----------------------------------------------------------------------------

// 目前是fullAO, 1080p下的质量比较担忧..可能倒是会改成大于多少就采用floatAO吧

#include "$postlib.h"

float4x4 matView;

sampler2D normalMap : register(s0)=
sampler_state
{
    AddressU = Clamp;
    AddressV = Clamp;
    MinFilter = Point;
    MagFilter = Point;
    MipFilter = Point;
};

sampler2D tex0 : register(s0)=
	sampler_state
{
	AddressU = Clamp;
	AddressV = Clamp;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
};

sampler2D positionMap : register(s1)=
sampler_state
{
    AddressU = Clamp;
    AddressV = Clamp;
    MinFilter = Point;
    MagFilter = Point;
    MipFilter = Point;
};
sampler2D rotSampler : register(s2)=
	sampler_state
{
	AddressU = Wrap;
	AddressV = Wrap;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = Point;
};

float2 PixelKernel[4] =
{
    { 0,  1},
    { 1,  0},
    { 0, -1},
    {-1,  0}
};

float4 AOSetting = float4(1.0f, 1.0f, 5.0f, 1.0f);
float4 PS_ScreenSize = float4(1280.0, 720.0, 0.0, 0.0);

const float radius = 0.06;

// Kernel definition
const float3 kernel[32] = {
	float3(-0.556641,-0.037109,-0.654297), 
	float3(0.173828,0.111328,0.064453),    
	float3(0.001953,0.082031,-0.060547),   
	float3(0.220703,-0.359375,-0.062500),  
	float3(0.242188,0.126953,-0.250000),   
	float3(0.070313,-0.025391,0.148438),   
	float3(-0.078125,0.013672,-0.314453),  
	float3(0.117188,-0.140625,-0.199219),  
	float3(-0.251953,-0.558594,0.082031),  
	float3(0.308594,0.193359,0.324219),    
	float3(0.173828,-0.140625,0.031250),   
	float3(0.179688,-0.044922,0.046875),   
	float3(-0.146484,-0.201172,-0.029297),  
	float3(-0.300781,0.234375,0.539063),   
	float3(0.228516,0.154297,-0.119141),   
	float3(-0.119141,-0.003906,-0.066406),  
	float3(-0.218750,0.214844,-0.250000),  
	float3(0.113281,-0.091797,0.212891),   
	float3(0.105469,-0.039063,-0.019531),  
	float3(-0.705078,-0.060547,0.023438),  
	float3(0.021484,0.326172,0.115234),    
	float3(0.353516,0.208984,-0.294922),   
	float3(-0.029297,-0.259766,0.089844),  
	float3(-0.240234,0.146484,-0.068359),  
	float3(-0.296875,0.410156,-0.291016),  
	float3(0.078125,0.113281,-0.126953),   
	float3(-0.152344,-0.019531,0.142578),  
	float3(-0.214844,-0.175781,0.191406),  
	float3(0.134766,0.414063,-0.707031),   
	float3(0.291016,-0.833984,-0.183594),  
	float3(-0.058594,-0.111328,0.457031),  
	float3(-0.115234,-0.287109,-0.259766),	
};
//-----------------------------------------------------------------------------
// Pixel Shader: PostProcessPS
// Desc: Performs post-processing effect that detects and highlights edges.
//-----------------------------------------------------------------------------
//float g_constract = 1.5f;

float doAmbientOcclusion(in float2 tex,in float2 offset, in float3 p, in float3 cnorm, in float3 FarClip)  
{  
	// 取得到p的 向量
	float3 diff = GetWorldPos(positionMap, tex + offset, FarClip.xyz - PS_ddx.xyz*offset.x + PS_ddy.xyz*offset.y) - p; 
	float d = length(diff); 
	diff = diff / d; 
	d *= AOSetting.x; 
	// 取得到p的 距离

	// 计算对p的遮罩贡献
	return max(0.0f ,saturate( dot(cnorm,diff) )- AOSetting.y) * ( 1.0 / (1.0 + d) ) * AOSetting.z;  
}  

float4 SSAOPS( float4 Tex : TEXCOORD0,
				float4 FarClip : TEXCOORD1) : COLOR0
{
	// 取得该像素的pos
	float4 pos = GetViewSpacePos(positionMap, Tex.xy, FarClip.xyz);

	// 取得该像素的normal
	float3 normal = NormalCalcFast(tex2D(normalMap, Tex.xy).xyz);  
	// 取得一个随机数
	float3 rand = normalize(tex2D( rotSampler, Tex.xy * PS_ScreenSize.xy * 0.25).xyz * 2.0f - 1.0f);  
	float3 viewNormal = mul( normal, (float3x3)matView);
	viewNormal =  reflect(viewNormal , rand); 


	float4 aoacc = 0.0f;
	float rad = AOSetting.w * 0.00001f / pos.w;
	rad = max(rad, 0.04);
	//**SSAO Calculation**//
	// 迭代四次
	int iterations = 8;
	float3 vSample[4];
	for (int j = 0; j < iterations; j+=4)  
	{  
		// get a random
		vSample[0] = reflect(kernel[j+0] , rand);  
		vSample[1] = reflect(kernel[j+1] , rand);
		vSample[2] = reflect(kernel[j+2] , rand);
		vSample[3] = reflect(kernel[j+3] , rand);

		// 确保coord在内部
		vSample[0] = (dot(viewNormal, vSample[0]) >= 0.0f) ? vSample[0] : -vSample[0];
		vSample[1] = (dot(viewNormal, vSample[1]) >= 0.0f) ? vSample[1] : -vSample[1];
		vSample[2] = (dot(viewNormal, vSample[2]) >= 0.0f) ? vSample[2] : -vSample[2];
		vSample[3] = (dot(viewNormal, vSample[3]) >= 0.0f) ? vSample[3] : -vSample[3];

		vSample[0] *= rad;
		vSample[1] *= rad;
		vSample[2] *= rad;
		vSample[3] *= rad;

		// 取得到p的 向量
		float3 diff0 = GetViewSpacePos(positionMap, Tex.xy + vSample[0], FarClip.xyz + PS_ddx.xyz*vSample[0].x + PS_ddy.xyz*vSample[0].y) - pos.xyz; 
		float3 diff1 = GetViewSpacePos(positionMap, Tex.xy + vSample[1], FarClip.xyz + PS_ddx.xyz*vSample[1].x + PS_ddy.xyz*vSample[1].y) - pos.xyz;
		float3 diff2 = GetViewSpacePos(positionMap, Tex.xy + vSample[2], FarClip.xyz + PS_ddx.xyz*vSample[2].x + PS_ddy.xyz*vSample[2].y) - pos.xyz; 
		float3 diff3 = GetViewSpacePos(positionMap, Tex.xy + vSample[3], FarClip.xyz + PS_ddx.xyz*vSample[3].x + PS_ddy.xyz*vSample[3].y) - pos.xyz;

		float4 length4 = float4(length(diff0), length(diff1), length(diff2), length(diff3)); 

		diff0 = diff0 / length4.x; 
		diff1 = diff1 / length4.y; 
		diff2 = diff2 / length4.z; 
		diff3 = diff3 / length4.w; 

		length4 = max(0.5, length4);

		length4 *= AOSetting.x; 
		// 取得到p的 距离

		// 计算对p的遮罩贡献
		float4 occlude;
		occlude.x = dot(normal, diff0);
		occlude.y = dot(normal, diff1);
		occlude.z = dot(normal, diff2);
		occlude.w = dot(normal, diff3);

		occlude = max(0.0, occlude - AOSetting.y) * ( 1.0 / (1.0 + length4 * length4) );

		aoacc += occlude;
		// SIMD
	}
	
	float ao = dot( aoacc, AOSetting.z) / (float)iterations;  	
	ao = saturate( ao );	
	ao = 1.0f - ao;
	//ao *= ao;

	//ao = pow(ao, AOSetting.y);

    return ao;
}

float4 blurKernel1;
float4 blurKernel2;

float4 SSAO_BlurPS( float4 Tex : TEXCOORD0,
	float4 FarClip : TEXCOORD1) : COLOR0
{
	float ao0 = tex2D(tex0, Tex + blurKernel1.xy);
	float ao1 = tex2D(tex0, Tex + blurKernel1.zw);
	float ao2 = tex2D(tex0, Tex + blurKernel2.xy);
	float ao3 = tex2D(tex0, Tex + blurKernel2.zw);

	float centerDepth = GetDepth(positionMap, Tex);
	float4 depth4;
	depth4.x = GetDepth(positionMap, Tex + blurKernel1.xy);
	depth4.y = GetDepth(positionMap, Tex + blurKernel1.zw);
	depth4.z = GetDepth(positionMap, Tex + blurKernel2.xy);
	depth4.w = GetDepth(positionMap, Tex + blurKernel2.zw);
	float4 weight4 = saturate(1.0 - 10.0 * abs(depth4 / centerDepth - 1.h));

	float4 occlude = float4(ao0, ao1, ao2, ao3);

	//float final = dot(1, occlude) * 0.25f;
	float final;
	float totalWeight = dot(weight4, 1);
	final = dot(.25, occlude);
	if(totalWeight > 0.01)
		final = saturate(dot(weight4, occlude) / totalWeight);


	return final;
}

//-----------------------------------------------------------------------------
// Technique: PostProcess
// Desc: Performs post-processing effect that detects and highlights edges.
//-----------------------------------------------------------------------------
technique SSAO
{
    pass p0
    {
        VertexShader = null;
        PixelShader = compile ps_3_0 SSAOPS();
		ZEnable = false;
		AlphaBlendEnable = false;
		AlphaTestEnable = false;
    }
}

technique SSAO_Blur
{
	pass p0
	{
		VertexShader = null;
		PixelShader = compile ps_3_0 SSAO_BlurPS();
		ZEnable = false;
		AlphaBlendEnable = false;
		AlphaTestEnable = false;
	}
}
