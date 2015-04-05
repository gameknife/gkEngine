//-----------------------------------------------------------------------------
// File: SSRL.fx
//
// Desc: 屏幕空间反射
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

pixout SSRL_PS(vert2frag IN)
{
	pixout OUT;

	float4 rawNormal = tex2D(_tex0LL, IN.baseTC.xy);
	float3 vNorm = 2 * (rawNormal.rgb - 0.5);
	vNorm = normalize(vNorm);

	float4 rawPos = GetWorldPos( _tex1LL, IN.baseTC.xy, IN.farVerts );
	float3 vPos = rawPos.xyz;

	clip( 0.2 - rawPos.w );

	float3 vView = normalize(vPos - g_camPos);
	//float3 vView = normalize( g_camPos - vPos );

	float3 vRefl = normalize(reflect( vView, vNorm ));
	float4 sampleColor = 0;
	float multi = 1;
	for(int i=0; i < 8; i++)
	{
		float4 newPos = 1.0;
		newPos.xyz = vPos.xyz + vRefl * pow(i, 4.0) * 0.04;
		newPos.w = 1.0;
		reprojectPos(newPos);

		if (newPos.x < 0 || newPos.x > 1 || newPos.y < 0 || newPos.y > 1)
		{
			multi = 0;
		}

		float4 screenPos = GetWorldPosOffseting( _tex1LL, newPos.xy);
		screenPos.w = 1.0;
		reprojectPos(screenPos);

		if(screenPos.z - newPos.z < -0.00001 )
		{
			float3 tgtNormal = tex2D(_tex0LL,newPos.xy);
			tgtNormal = 2 * (tgtNormal.rgb - 0.5);
			tgtNormal = normalize(tgtNormal);

			if ( dot(tgtNormal, -vRefl) > 0.01)
			{
				sampleColor += tex2D(_tex2LL, newPos.xy) * multi;
				multi = 0;
			}
		}
	}

	OUT.Color = sampleColor;

	return OUT;
}

pixout SSRL2D_PS(vert2frag IN)
{
	pixout OUT;

	float4 rawNormal = tex2D(_tex0LL, IN.baseTC.xy);
	float3 vNorm = 2 * (rawNormal.rgb - 0.5);
	vNorm = normalize(vNorm);

	float4 rawPos = GetWorldPos( _tex1LL, IN.baseTC.xy, IN.farVerts );
	float3 vPos = rawPos.xyz;

	clip( 0.2 - rawPos.w );

	float3 vView = normalize(vPos - g_camPos);
	//float3 vView = normalize( g_camPos - vPos );

	float3 vRefl = normalize(reflect( vView, vNorm ));
	
//	float2 vRefl2D = mul( vRefl, g_mProjection ).xy;
//	float proj_len = length(vRefl2D);
//	proj_len = max( 0.01, 1.0 / proj_len);

	float4 sampleColor = 0;//float4(0.5,0.6,0.8,1);
	for(int i=0; i < 8; i++)
	{
		float4 newPos = 1.0;
		newPos.xyz = vPos.xyz + vRefl * pow(i, 3.0) * 0.1;
		newPos.w = 1.0;
		reprojectPos(newPos);

		if (newPos.x < 0 || newPos.x > 1 || newPos.y < 0 || newPos.y > 1)
		{
			break;
		}

		float4 screenPos = GetWorldPosOffseting( _tex1LL, newPos.xy);
		screenPos.w = 1.0;
		reprojectPos(screenPos);


		sampleColor = tex2Dgrad(_tex2LL, newPos.xy, float2(0,0), float2(0,0));

		if(screenPos.z - newPos.z < -0.00001 && screenPos.z - newPos.z > -0.01)
		{
			float3 tgtNormal = tex2Dgrad(_tex0LL,newPos.xy, float2(0,0), float2(0,0));
			tgtNormal = 2 * (tgtNormal.rgb - 0.5);
			tgtNormal = normalize(tgtNormal);

			if ( dot(tgtNormal, -vRefl) > 0.01)
			{
				break;
			}
		}

		//sampleColor = 1;
	}

	OUT.Color = sampleColor;
	return OUT;
}
//-----------------------------------------------------------------------------
// Technique: PostProcess
// Desc: Performs post-processing effect that detects and highlights edges.
//-----------------------------------------------------------------------------
technique SSRL
{
    pass p0
    {
        VertexShader = null;
        PixelShader = compile ps_3_0 SSRL2D_PS();
		ZEnable = false;
		AlphaBlendEnable = false;
		AlphaTestEnable = false;
    }
}

