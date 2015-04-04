//////////////////////////////////////////////////////////////////////////
//
// yikaiming (C) 2014
// gkENGINE Source File 
//
// Name:   	$Light_Calc.h
// Desc:	
// 	
// 
// Author:  gameKnife
// Date:	2015/2/25
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _Light_Calc_h_
#define _Light_Calc_h_

#include "$samplerdef.h"

#define HDR_OVERBRIGHT 8
#define HDR_FAKE_MAXOVERBRIGHT 8
#define HDR_EXP_OFFSET 128
#define HDR_EXP_BASE   1.06

// HDR SCALAR
shared float g_HDRScalar = 5.0f;
shared float g_SRGBRevert = 1.0f;

struct pixout
{
	float4 Color  : COLOR0;
};

struct pixout_fp
{
	float4 Color  : COLOR0;
};

struct pixout_zpass
{
	float4 Position  : COLOR0;
	float4 Normal  : COLOR1;
};

struct pixout_zpass_ds
{
	float4 Position  : COLOR0;
	float4 Normal  : COLOR1;
	float4 Albeto  : COLOR2;
};

struct vert2frag
{
	float4 HPosition  : POSITION;
	float4 baseTC     : TEXCOORD0;
	float4 farVerts   : TEXCOORD1;
};

#define ONE_OVER_PI 0.31831h
#define ONE_OVER_TWO_PI 0.159155h

// Optimized phong, use if mirrowed reflection vector pre-computed
float Phong(float3 R, float3 L, float Exp)
{	

	float fNormFactor = Exp * ONE_OVER_TWO_PI + ONE_OVER_TWO_PI;
	return fNormFactor *  pow(saturate(dot(L, R)), Exp);
}

float Phong(float3 N, float3 V, float3 L, float Exp)
{
	float3 R = reflect(-V, N);
	return Phong(R, L, Exp);
}

float Blinn(float3 N, float3 V, float3 L, float Exp)
{
	// in klayge this G() is exp * 0.125 + 2 * 0.125;
	float fNormFactor = Exp * ONE_OVER_TWO_PI + ONE_OVER_PI;
	float3 H = normalize(V + L);
	return fNormFactor * pow(saturate(dot(N, H)), Exp);
}

float SimpleBRDF(float3 N, float3 V, float3 L, float Exp)
{
	return Blinn(N, V, L, Exp);
	//return Phong(N, V, L, Exp);
}

float3 NormalCalc(float3 mapNorm, float BumpScale)
{
	float3 v = {0.5f, 0.5f, 1.0f};
	mapNorm = lerp(v, mapNorm, BumpScale );
	mapNorm = ( mapNorm * 2.0f ) - 1.0f;
	return mapNorm;
}

float3 NormalCalcFast(float3 mapNorm)
{
	mapNorm = ( mapNorm * 2.0f ) - 1.0f;
	return mapNorm;
}

float4 HDRScale(float4 c)
{
    return float4(
                    c.r * g_HDRScalar,
                    c.g * g_HDRScalar,
                    c.b * g_HDRScalar,
                    c.a
                );
}

void HDROutput( out pixout OUT, float4 Color, float fDepth)
{
	OUT.Color = HDRScale(Color);
	OUT.Color = pow(OUT.Color, float4(g_SRGBRevert,g_SRGBRevert,g_SRGBRevert,1));
}

float desaturate(float3 color)
{
	float luminance;
	luminance = dot(color,float3(0.299,0.587,0.114));
	return luminance;
}

float fresnel(float3 normal, float3 eyevec, float power, float bias, float scale)
{
	return bias + scale * pow(1.0h - dot(normal, eyevec), power);
}

float4 DecodeRGBK(in float4 Color, const float fMultiplier, bool bUsePPP= false)
{
	if( bUsePPP )
		Color.rgb *= Color.rgb * (Color.a * Color.a) * fMultiplier;
	else
		Color.rgb *= Color.a * fMultiplier;

	return Color;
}

float4 EncodeRGBE( float3 color, float fMultiplier )
{
	float fLen = max( color.x, max( color.y, color.z ) ) ;  
	float fExp = ceil( log(fLen) / log(1.06) );

	float4 ret;
	ret.w = (fExp + 128.0) / 256.0 / fMultiplier;
	ret.xyz = color.xyz / pow( 1.06, fExp);

	return ret;   
}

float4 GetEnvironmentCMap(samplerCUBE envMap, in float3 envTC, in float fNdotE, in float fSpecPower)
{
	float fEdgeMipFix = saturate(1-fNdotE);
	fEdgeMipFix *= fEdgeMipFix;
	fEdgeMipFix *= fEdgeMipFix;

	float fGlossinessLod = 6.16231h - 0.497418h * sqrt(fSpecPower);
	fGlossinessLod = max(6.16231h*fEdgeMipFix,fGlossinessLod);

	float4 envColor = DecodeRGBK(texCUBElod(envMap, float4(envTC.xyz, fGlossinessLod)), HDR_FAKE_MAXOVERBRIGHT);

	return envColor;
}

float4 GetEnvironmentCMap(samplerCUBE envMap, in float3 envTC, in float fSpecPower)
{
	float fGlossinessLod = 6.16231h - 0.497418h * sqrt(fSpecPower);
	float4 envColor = DecodeRGBK(texCUBElod(envMap, float4(envTC.xyz, fGlossinessLod)), HDR_FAKE_MAXOVERBRIGHT);

	return envColor;
}

float4 GetEnvironment2DMap(sampler2D envMap, in float2 envTC)
{
	float4 envColor = tex2D(envMap, envTC.xy);
	return envColor;
}

float smoothstep_opt(in float maxi, in float x)
{
  x = saturate( x / maxi );
  return  x * x  * (3.0 - 2.0 * x);
}

float GetFresnel(float NdotI, float bias, float power)
{
  float facing = (1.0 - NdotI);
  return saturate(bias + (1-bias)*pow(facing, power));
}

float GetFresnelOpt(float NdotI, float bias, float power)
{
  float facing = (1.0 - NdotI);
  return bias + pow(facing, power);  // opt: removed (1-bias) mul
}

void nvLambSkin(float ldn,
			  float4 DiffColor,
			  float4 SubColor,
			  float RollOff,
			  out float4 Diffuse,
			  out float4 Subsurface
) {
    float diffComp = max(0,ldn);
    Diffuse = float4((diffComp * DiffColor).xyz,1);
    float subLamb = smoothstep(-RollOff,1.0,ldn) - smoothstep_opt(1.0,ldn);
    subLamb = max(0.0,subLamb);
    Subsurface = subLamb * SubColor;
}

float GetLuminance( float3 color )
{
	return dot( color, float3( 0.2126h, 0.7152h, 0.0722h ) );
}

float3 GetLuminanceD( float3 color, float desaturate )
{
	float3 result = lerp( dot( color, float3( 0.2126h, 0.7152h, 0.0722h ) ), color, desaturate);
	return result;
}

float4 GetLuminanceMap( sampler2D lum, float2 tc )
{
	return tex2D(lum, tc);
}

#endif