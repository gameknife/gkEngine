//-----------------------------------------------------------------------------
// File: PP_SSAO.fx
//
// Desc: 屏幕空间环境光遮蔽技术, new version
//
// Author: Yi Kaiming 2011/10/25
//-----------------------------------------------------------------------------

// 目前是fullAO, 1080p下的质量比较担忧..可能倒是会改成大于多少就采用floatAO吧

#include "$postlib.h"
#include "$Light_calc.h"

sampler2D normalMap : register(s0)=
sampler_state
{
    AddressU = Clamp;
    AddressV = Clamp;
    MinFilter = Point;
    MagFilter = Point;
    MipFilter = Point;
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
sampler2D shadowMap : register(s3)=
	sampler_state
{
	AddressU = Clamp;
	AddressV = Clamp;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = Point;
};
sampler2D colorMap : register(s4)=
	sampler_state
{
	AddressU = Clamp;
	AddressV = Clamp;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = Point;
};

float4 AOSetting = float4(1.0f, 1.0f, 5.0f, 1.0f);
float4 PS_ScreenSize = float4(1280.0, 720.0, 0.0, 0.0);
float4 g_light;

float4 PS_ddx;
float4 PS_ddy;

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

float doPure(in float2 tex,in float2 offset, in float3 p, in float3 cnorm, in float3 FarClip, in float index)  
{  
	// 取得到p的 向量
	float3 pos = GetWorldPos(positionMap, tex + offset, FarClip.xyz - PS_ddx.xyz*offset.x + PS_ddy.xyz*offset.y);
	float3 normal = NormalCalcFast(tex2D(normalMap, tex + offset).xyz);
	float visiblity = tex2D(shadowMap, tex + offset).x;
	//normal = normalize(normal);

	// 取得距离和方向
	float d = length(pos - p); 
	float3 transDir = (pos - p) / d;

	// 如果在后方，直接return 0
	if ( dot(cnorm, transDir) - 0.1 < 0 )
	{
		return 1;
	}

	//float cosS = saturate( dot(-transDir, normal) );
	//float cosR = saturate( dot(transDir, cnorm) );

	float ret = 1 - 0.1 / (d * d);

	return saturate(ret);
	//d *= AOSetting.x; 
}  

float doDirect(in float2 tex,in float2 offset, in float3 p, in float3 cnorm, in float3 FarClip, in float index)  
{  
	// 取得到p的 向量
	float3 pos = GetWorldPos(positionMap, tex + offset, FarClip.xyz - PS_ddx.xyz*offset.x + PS_ddy.xyz*offset.y);
	float3 normal = NormalCalcFast(tex2D(normalMap, tex + offset).xyz);
	float visiblity = tex2D(shadowMap, tex + offset).x;
	//normal = normalize(normal);

	// 取得距离和方向
	float d = length(pos - p); 
	float3 transDir = (pos - p) / d;

	//d = saturate(d);
	//d = max(0.1, d);

	// 如果在后方，直接return 0
	if ( dot(cnorm, transDir) - 0.1 < 0 )
	{
		return saturate( dot(normal, g_light) ) * dot(cnorm, g_light) * visiblity;
	}

	
	return 0;
	//d *= AOSetting.x; 
}  

float doBounce(in float2 tex,in float2 offset, in float3 p, in float3 cnorm, in float3 FarClip, in float index)  
{  
	// 取得到p的 向量
	float3 pos = GetWorldPos(positionMap, tex + offset, FarClip.xyz - PS_ddx.xyz*offset.x + PS_ddy.xyz*offset.y);
	float3 normal = NormalCalcFast(tex2D(normalMap, tex + offset).xyz);
	float visiblity = tex2D(shadowMap, tex + offset).x;

	// 取得距离和方向
	float d = length(pos - p); 
	float3 transDir = (pos - p) / d;

	//d = saturate(d);
	d = max(0.1, d);

	// 如果在后方，直接return 0
	if ( dot(cnorm, transDir) < 0 )
	{
		return 0;
	}

	float cosS = saturate( dot(-transDir, normal) );
	float cosR = saturate( dot(transDir, cnorm) );

	// 在前方，利用公式计算间接光照
	float dirColor = tex2D(colorMap, tex + offset).x;
	float indColor = dirColor * (1 - visiblity) * (1 * cosS * cosR) / (d * d);

	return indColor;
	//d *= AOSetting.x; 
}  

float4 SSDO_DirectPS( float4 Tex : TEXCOORD0,
	float4 FarClip : TEXCOORD1) : COLOR0
{
	// 取得该像素的pos
	float4 pos = GetWorldPos(positionMap, Tex.xy, FarClip.xyz);

	// 取得该像素的normal
	float3 normal = NormalCalcFast(tex2D(normalMap, Tex.xy).xyz);  
	// 取得一个随机数
	float3 rand = normalize(tex2D( rotSampler, Tex.zw).xyz * 2.0f - 1.0f);  

	float rad = (AOSetting.w * 0.5) / pos.w;
	rad = max(rad, 0.04);

	// 间接光照
	float4 indirect = 0;

	//**SSAO Calculation**//
	// 迭代四次
	int iterations = 16;  
	for (int j = 0; j < iterations; j+=4)  
	{  
		// get a random
		float2 coord1 = reflect(kernel[j+0] , rand) * rad;  
		float2 coord2 = reflect(kernel[j+1] , rand) * rad;
		float2 coord3 = reflect(kernel[j+2] , rand) * rad;
		float2 coord4 = reflect(kernel[j+3] , rand) * rad;

		// 	float2 coord1 = (kernel[j+0]) * rad;  
		// 	float2 coord2 = (kernel[j+1]) * rad;
		// 	float2 coord3 = (kernel[j+2]) * rad;
		// 	float2 coord4 = (kernel[j+3]) * rad;

		//ao += doAmbientOcclusion(Tex,coord1*0.25, pos, normal);  
		indirect += doDirect(Tex.xy, coord1, pos.xyz, normal, FarClip, j);  
		indirect += doDirect(Tex.xy, coord2, pos.xyz, normal, FarClip, j);
		indirect += doDirect(Tex.xy, coord3, pos.xyz, normal, FarClip, j);
		indirect += doDirect(Tex.xy, coord4, pos.xyz, normal, FarClip, j);
		//ao += doAmbientOcclusion(Tex,coord2, 	  pos, normal);  
	}   

	indirect = indirect / iterations;
	//ao = pow(ao, AOSetting.y);

	return saturate( indirect.x );
}

float4 SSDOPS( float4 Tex : TEXCOORD0,
				float4 FarClip : TEXCOORD1) : COLOR0
{
	// 取得该像素的pos
	float4 pos = GetWorldPos(positionMap, Tex.xy, FarClip.xyz);

	// 取得该像素的normal
	float3 normal = NormalCalcFast(tex2D(normalMap, Tex.xy).xyz);  
	// 取得一个随机数
	float3 rand = normalize(tex2D( rotSampler, Tex.zw).xyz * 2.0f - 1.0f);  

	float rad = (AOSetting.w * 1.0) / pos.w;
	rad = max(rad, 0.04);

	// 间接光照
	float4 indirect = 0;

	//**SSAO Calculation**//
	// 迭代四次
	int iterations = 16;  
	for (int j = 0; j < iterations; j+=4)  
	{  
	// get a random
	float2 coord1 = reflect(kernel[j+0] , rand) * rad;  
	float2 coord2 = reflect(kernel[j+1] , rand) * rad;
	float2 coord3 = reflect(kernel[j+2] , rand) * rad;
	float2 coord4 = reflect(kernel[j+3] , rand) * rad;

	//ao += doAmbientOcclusion(Tex,coord1*0.25, pos, normal);  
	indirect += doPure(Tex.xy, coord1, pos.xyz, normal, FarClip, j);  
	indirect += doPure(Tex.xy, coord2, pos.xyz, normal, FarClip, j);
	indirect += doPure(Tex.xy, coord3, pos.xyz, normal, FarClip, j);
	indirect += doPure(Tex.xy, coord4, pos.xyz, normal, FarClip, j);
	//ao += doAmbientOcclusion(Tex,coord2, 	  pos, normal);  
	}   
	
	indirect = indirect / iterations;
	//ao = pow(ao, AOSetting.y);

    return saturate( indirect.x );
}

float4 SSDO_BouncePS( float4 Tex : TEXCOORD0,
	float4 FarClip : TEXCOORD1) : COLOR0
{
	// 取得该像素的pos
	float4 pos = GetWorldPos(positionMap, Tex.xy, FarClip.xyz);

	// 取得该像素的normal
	float3 normal = NormalCalcFast(tex2D(normalMap, Tex.xy).xyz);  
	// 取得一个随机数
	float3 rand = normalize(tex2D( rotSampler, Tex.zw).xyz * 2.0f - 1.0f);  

	float rad = (AOSetting.w * 4.0) / pos.w;
	rad = max(rad, 0.04);

	// 间接光照
	float4 indirect = 0;

	//**SSAO Calculation**//
	// 迭代四次
	int iterations = 8;  
	for (int j = 0; j < iterations; j+=4)  
	{  
		// get a random
		float2 coord1 = reflect(kernel[j+0] , rand) * rad;  
		float2 coord2 = reflect(kernel[j+1] , rand) * rad;
		float2 coord3 = reflect(kernel[j+2] , rand) * rad;
		float2 coord4 = reflect(kernel[j+3] , rand) * rad;

		indirect += doBounce(Tex.xy, coord1, pos.xyz, normal, FarClip, j);  
		indirect += doBounce(Tex.xy, coord2, pos.xyz, normal, FarClip, j);
		indirect += doBounce(Tex.xy, coord3, pos.xyz, normal, FarClip, j);
		indirect += doBounce(Tex.xy, coord4, pos.xyz, normal, FarClip, j);
	}   
	
	float color = tex2D(colorMap, Tex.xy).x + indirect / iterations;

	return color;
}

//-----------------------------------------------------------------------------
// Technique: PostProcess
// Desc: Performs post-processing effect that detects and highlights edges.
//-----------------------------------------------------------------------------
technique SSDO_Direct
{
    pass p0
    {
        VertexShader = null;
        PixelShader = compile ps_3_0 SSDO_DirectPS();
		ZEnable = false;
		AlphaBlendEnable = false;
		AlphaTestEnable = false;
    }
}

technique SSDO
{
	pass p0
	{
		VertexShader = null;
		PixelShader = compile ps_3_0 SSDOPS();
		ZEnable = false;
		AlphaBlendEnable = false;
		AlphaTestEnable = false;
	}
}

technique SSDO_Bounce
{
	pass p0
	{
		VertexShader = null;
		PixelShader = compile ps_3_0 SSDO_BouncePS();
		ZEnable = false;
		AlphaBlendEnable = false;
		AlphaTestEnable = false;
	}
}
