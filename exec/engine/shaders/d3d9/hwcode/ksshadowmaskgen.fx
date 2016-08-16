//-----------------------------------------------------------------------------
// File: PP_ShadowMap.fx
//
// Desc: ShadowMask生成pass,全屏RT
//
// Author: Yi Kaiming 2010/12/29
//-----------------------------------------------------------------------------


#define SMAP_SIZE 1024

#define SHADOW_EPSILON 0.008f
#define SOFT_SIZE 0.006f
#define HIGH_QUALITY

// SHADOW_EPSILON means the depth offset, because faces has thick too, or it will shadow its self
#include "$postlib.h"
float4x4 g_mViewToLightProj0;

// 阴影深度
float g_fShadowDepth = 1.0f;

float4 gsmShadowParam0 = {0.001f, 0.005f, 0.030f, 1.0f};		// constbia, penumbra_start, penumbra_length

// random for SSAO
float g_random_size = 64.0f;

sampler2D positionMap : register(s0)=
sampler_state
{
    AddressU = Clamp;
    AddressV = Clamp;
    MinFilter = Point;
    MagFilter = Point;
    MipFilter = None;
};


sampler2D shadowMap : register(s1)=
sampler_state
{
	AddressU = Clamp;
    AddressV = Clamp;
    MinFilter = Point;
    MagFilter = Point;
    MipFilter = None;

};

sampler2D randomMap : register(s2)=
sampler_state
{
    AddressU = WraP;
    AddressV = WraP;
    MinFilter = Point;
    MagFilter = Point;
    MipFilter = None;
};

float GetDepth(float4 org)
{
	return org.x;
	//float depth = dot(org.arg, float3(0.996093809371817670572857294849, 0.0038909914428586627756752238080039, 1.5199185323666651467481343000015e-5));

}

float2 PixelKernel[8] =
{
    { 0,  1},
    { 1,  0},
    { 0, -1},
    {-1,  0},
	{ 0.707,  0.707},
	{ 0.707,  -0.707},
	{ -0.707,  0.707},
	{ -0.707,  -0.707}
};

float2 TexelKernel[4]
<
	string ConvertPixelsToTexels = "PixelKernel";
>;

float PCF_Filter(float2 uv, float2 Tex, float4 LP, uniform sampler2D ShadowMap, 
                uniform float bias, float filterWidth, int numSamples)
{
       // compute step size for iterating through the kernel
       float stepSize = 0.4 * filterWidth / (float)numSamples;

       // compute uv coordinates for upper-left corner of the kernel
       uv = uv - float2(filterWidth * 0.2,filterWidth * 0.2);

       float sum = 0;  // sum of successful depth tests

       // now iterate through the kernel and filter
       for (int i=0; i<numSamples; i++) {
               for (int j=0; j<numSamples; j++) {
                       // get depth at current texel of the shadow map
                       float shadMapDepth = 0;
                       shadMapDepth = bias + tex2D(ShadowMap, uv + float2(i*stepSize,j*stepSize)).x;
                       // test if the depth in the shadow map is closer than
                       // the eye-view point
                       float shad = LP.z < shadMapDepth ;

                       // accumulate result
                       sum += shad;
               }
       }
       
       // return average of the samples
       return sum / (numSamples*numSamples);

}


// change the sampler method [11/21/2011 Kaiming]
//////////////////////////////////////////////////////////////////////////
//
//		each iterate, 4 directon
//		
//
//
//
//
//////////////////////////////////////////////////////////////////////////
float Jittering_Filter(float2 uv, float2 Tex, float4 LP, uniform sampler2D ShadowMapxxx, 
                uniform float bias, float filterWidth, int quality)
{
// here we go through this point for first
	float currDepth = bias + GetDepth(tex2D(ShadowMapxxx, uv));
	// test if the depth in the shadow map is closer than
	// the eye-view point
	
	float width = filterWidth;

	if(LP.z > currDepth)
		width = filterWidth * max(gsmShadowParam0.y, LP.z - currDepth) * gsmShadowParam0.z;

	// random采样要做点事情
	float2 randomMultiplyer;
	randomMultiplyer.x = 1024/64;
	randomMultiplyer.y = 1024/64;

	// 取得一个随机数
	float2 rand = normalize(tex2D( randomMap, Tex * randomMultiplyer).xy * 2.0f - 1.0f);  
	


    float sum = 0;  // sum of successful depth tests
	int iterations = 4;  


	// write the sampler iterate by hand

	// 8 dir sampler
	float2 coordAxis[4];
	float2 coordRot[4];
	coordAxis[0] = reflect(PixelKernel[0],rand);  
	coordRot[0] = float2(coordAxis[0].x * 0.707 - coordAxis[0].y * 0.707, coordAxis[0].x * 0.707 + coordAxis[0].y * 0.707);  
	coordAxis[1] = reflect(PixelKernel[1],rand);  
	coordRot[1] = float2(coordAxis[1].x * 0.707 - coordAxis[1].y * 0.707, coordAxis[1].x * 0.707 + coordAxis[1].y * 0.707);  
	coordAxis[2] = reflect(PixelKernel[2],rand);  
	coordRot[2] = float2(coordAxis[2].x * 0.707 - coordAxis[2].y * 0.707, coordAxis[2].x * 0.707 + coordAxis[2].y * 0.707);  
	coordAxis[3] = reflect(PixelKernel[3],rand);  
	coordRot[3] = float2(coordAxis[3].x * 0.707 - coordAxis[3].y * 0.707, coordAxis[3].x * 0.707 + coordAxis[3].y * 0.707);  

	float shadMapDepth = 0;
	float shad  = 0;
	// first 4 dir

	// SIMD
	float4 shadMapDepth4 = 0;
	float4 shad4 = 0;
	//for (int j = 0; j < 4; ++j)
	{
		
		shadMapDepth4.x = GetDepth(tex2D(ShadowMapxxx, uv + coordRot[0] * width));
		shadMapDepth4.y = GetDepth(tex2D(ShadowMapxxx, uv + coordRot[1] * width));
		shadMapDepth4.z = GetDepth(tex2D(ShadowMapxxx, uv + coordRot[2] * width));
		shadMapDepth4.w = GetDepth(tex2D(ShadowMapxxx, uv + coordRot[3] * width));

		shadMapDepth4 += bias;

		shad4 = sign(LP.zzzz - shadMapDepth4);
		shad4 = shad4 * 0.5 + 0.5;

		sum += dot(shad4, float4(1,1,1,1));

		//if(shad < 0.5)
		//	width = filterWidth *  max(gsmShadowParam0.y, LP.z - currDepth) * gsmShadowParam0.z;
	}
float samplersize = 4;

if (quality >= 2)
{
	samplersize = 8;
	// second 4 dir
	//for (int j = 0; j < 4; ++j)
	{
		shadMapDepth4.x = GetDepth(tex2D(ShadowMapxxx, uv + coordAxis[0] * width * 0.9f));
		shadMapDepth4.y = GetDepth(tex2D(ShadowMapxxx, uv + coordAxis[1] * width * 0.9f));
		shadMapDepth4.z = GetDepth(tex2D(ShadowMapxxx, uv + coordAxis[2] * width * 0.9f));
		shadMapDepth4.w = GetDepth(tex2D(ShadowMapxxx, uv + coordAxis[3] * width * 0.9f));

		shadMapDepth4 += bias;

		shad4 = sign(LP.zzzz - shadMapDepth4);
		shad4 = shad4 * 0.5 + 0.5;

		sum += dot(shad4, float4(1,1,1,1));
	}
}


	// high performance
if(quality >= 3)
{
	samplersize = 16;
	//for (int j = 0; j < 4; ++j)
	{
		shadMapDepth4.x = GetDepth(tex2D(ShadowMapxxx, uv + coordRot[0] * width * 0.75f));
		shadMapDepth4.y = GetDepth(tex2D(ShadowMapxxx, uv + coordRot[1] * width * 0.75f));
		shadMapDepth4.z = GetDepth(tex2D(ShadowMapxxx, uv + coordRot[2] * width * 0.75f));
		shadMapDepth4.w = GetDepth(tex2D(ShadowMapxxx, uv + coordRot[3] * width * 0.75f));

		shadMapDepth4 += bias;

		shad4 = sign(LP.zzzz - shadMapDepth4);
		shad4 = shad4 * 0.5 + 0.5;

		sum += dot(shad4, float4(1,1,1,1));
	}

	// second 4 dir
	//for (int j = 0; j < 4; ++j)
	{
		shadMapDepth4.x = GetDepth(tex2D(ShadowMapxxx, uv + coordAxis[0] * width * 0.5f));
		shadMapDepth4.y = GetDepth(tex2D(ShadowMapxxx, uv + coordAxis[1] * width * 0.5f));
		shadMapDepth4.z = GetDepth(tex2D(ShadowMapxxx, uv + coordAxis[2] * width * 0.5f));
		shadMapDepth4.w = GetDepth(tex2D(ShadowMapxxx, uv + coordAxis[3] * width * 0.5f));

		shadMapDepth4 += bias;

		shad4 = sign(LP.zzzz - shadMapDepth4);
		shad4 = shad4 * 0.5 + 0.5;

		sum += dot(shad4, float4(1,1,1,1));
	}
}

//     // return average of the samples
     sum =  sum / samplersize;

	 return 1 - sum;
}


float Point_Filter(float2 uv, float2 Tex, float4 LP, uniform sampler2D ShadowMapxxx, 
	uniform float bias, float filterWidth, int quality)
{
	// here we go through this point for first
	float currDepth = bias + GetDepth(tex2D(ShadowMapxxx, uv));
	// test if the depth in the shadow map is closer than
	// the eye-view point

	float width = filterWidth;

	if(LP.z > currDepth)
	{
		return 0;
	}

	return 1;
}

//-----------------------------------------------------------------------------
// Pixel Shader: PixScene
// Desc: Process pixel (do per-pixel lighting) for enabled scene
//-----------------------------------------------------------------------------
float4 HighPerformence( float2 Tex : TEXCOORD0,
						float4 FarClip : TEXCOORD1 ) : COLOR0
{
    float Diffuse = 1;

	//return 1;
	float4 vPos = GetWorldPos(positionMap, Tex.xy, FarClip.xyz);
	vPos.w = 1;
	float4 vPosLight = mul(vPos, g_mViewToLightProj0);
	
    //transform from RT space to texture space.
    float2 ShadowTexC = 0.5 * vPosLight.xy / vPosLight.w + float2( 0.5, 0.5 );
    ShadowTexC.y = 1.0f - ShadowTexC.y;
	


	vPosLight.z /= vPosLight.w;
        
	if (vPosLight.z > 1)
	{
		discard;
	}

	float LightAmount = Jittering_Filter(ShadowTexC, Tex, vPosLight, shadowMap, gsmShadowParam0.x, SOFT_SIZE, 1);

	Diffuse = saturate( LightAmount );
	return saturate(Diffuse + g_fShadowDepth);
}

float4 LowPerformence( float2 Tex : TEXCOORD0,
	float4 FarClip : TEXCOORD1 ) : COLOR0
{
	float Diffuse = 1;

	//return 0;

	float4 vPos = GetWorldPos(positionMap, Tex.xy, FarClip.xyz);
	vPos.w = 1;

	float4 vPosLight = mul(vPos, g_mViewToLightProj0);

	//transform from RT space to texture space.
	float2 ShadowTexC = 0.5 * vPosLight.xy / vPosLight.w + float2( 0.5, 0.5 );
	ShadowTexC.y = 1.0f - ShadowTexC.y;

	vPosLight.z /= vPosLight.w;

	if (vPosLight.z > 1)
	{
		discard;
	}

	float LightAmount = Jittering_Filter(ShadowTexC, Tex, vPosLight, shadowMap, gsmShadowParam0.x * 0.7f, SOFT_SIZE * 0.5f , 1);


	Diffuse = saturate( LightAmount );

	return saturate(Diffuse + g_fShadowDepth);		
	//	}

	//	return Diffuse;
}

technique ShadowMaskGen
{
    pass p0
    {
        VertexShader = null;
        PixelShader = compile ps_3_0 HighPerformence();
        ZEnable = false;
		AlphaBlendEnable = false;
		AlphaTestEnable = false;
    }
}

technique SimpleShadowMaskGen
{
	pass p0
	{
		VertexShader = null;
		PixelShader = compile ps_3_0 LowPerformence();
		ZEnable = false;
		AlphaBlendEnable = false;
		AlphaTestEnable = false;
	}
}

