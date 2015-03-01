//-----------------------------------------------------------------------------
// File: PP_ShadowMap.fx
//
// Desc: ShadowMaskÉú³Épass,È«ÆÁRT
//
// Author: Yi Kaiming 2010/12/29
//-----------------------------------------------------------------------------

#include "$postlib.h"
#define SMAP_SIZE 1024

#define SHADOW_EPSILON 0.0008f
#define SOFT_SIZE 0.006f
#define PENUMBRA 0.030f
#define PENUMBRA_START 0.005f
#define HIGH_QUALITY

// SHADOW_EPSILON means the depth offset, because faces has thick too, or it will shadow its self

float4x4 g_mViewToLightProj0;
float3 g_mLightDir;
sampler2D positionMap : register(s0)=
sampler_state
{
	AddressU = Clamp;
	AddressV = Clamp;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = Point;
};

//-----------------------------------------------------------------------------
// Pixel Shader: PixScene
// Desc: Process pixel (do per-pixel lighting) for enabled scene
//-----------------------------------------------------------------------------
float4 PostProcessPS( float2 Tex : TEXCOORD0,
	float4 FarClip : TEXCOORD1) : COLOR0
{
	float4 vPos = GetWorldPos(positionMap, Tex.xy, FarClip.xyz);
	vPos.w = 1;

	float4 vPosLight = mul(vPos, g_mViewToLightProj0);
	
    //transform from RT space to texture space.
    float2 ShadowTexC = 0.5 * vPosLight.xy / vPosLight.w + float2( 0.5, 0.5 );
    ShadowTexC.y = 1.0f - ShadowTexC.y;
	
	vPosLight.z /= vPosLight.w;
        
    // early judge
    if(ShadowTexC.x < 0.0f || ShadowTexC.x > 1.0f || ShadowTexC.y < 0.0f || ShadowTexC.y > 1.0f)
    {
		discard;	
	}

	// judge normal
// 	float3 rawNormal = 2.0 * (tex2D(_tex1, Tex) - 0.5f);
// 	if ( abs(dot(rawNormal, g_mLightDir)) < 0.1 )
// 	{
// 		discard;
// 	}


	return 1;
}

technique ShadowStencilGen
{
    pass p0
    {
        VertexShader = null;
        PixelShader = compile ps_3_0 PostProcessPS();
        ZEnable = false;
    }
}
