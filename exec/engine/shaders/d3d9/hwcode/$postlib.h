//////////////////////////////////////////////////////////////////////////
//
// yikaiming (C) 2014
// gkENGINE Source File 
//
// Name:   	$postlib.h
// Desc:	
// 	
// 
// Author:  gameKnife
// Date:	2015/2/25
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _postlib_h_
#define _postlib_h_

#include "$Light_Calc.h"

shared float4 g_fScreenSize = float4( 0.000781, 0.001389, 0, 0);
shared float4 g_camPos;

shared float4x4 g_mProjection 			: 		PROJECTION;

float4 PS_lefttop;
float4 PS_ddx;
float4 PS_ddy;

float4 GetWorldPos( in sampler2D depthSampler, in float2 tc, in float3 farclippos )
{
	float depth = tex2D(depthSampler, tc).x;
	return float4( depth * farclippos.xyz + g_camPos.xyz, depth );
}

float4 GetWorldPosOffseting( in sampler2D depthSampler, in float2 tc )
{
	return GetWorldPos(depthSampler, tc, PS_lefttop.xyz + PS_ddx.xyz * tc.x + PS_ddy.xyz * tc.y);
}

float4 GetViewSpacePos( in sampler2D depthSampler, in float2 tc, in float3 farclippos )
{
	float depth = tex2D(depthSampler, tc).x;
	return float4( depth * farclippos.xyz, depth );
}

float GetDepth( in sampler2D depthSampler, in float2 tc )
{
	return tex2D(depthSampler, tc).x;
}

// Color grading/charts tex lookup
void TexColorChart2D(in sampler2D chartSampler, inout float3 cImage)
{
	float chartDim	= 16.0;
	float3 scale = float3(chartDim - 1.0, chartDim - 1.0, chartDim - 1.0) / chartDim;
	float3 bias = float3(0.5, 0.5, 0.0) / chartDim;

	float4 lookup = float4(saturate(cImage) * scale + bias, 0);

	// convert input color into 2d color chart lookup address 
	float slice = lookup.z * chartDim;	
	float sliceFrac = frac(slice);	
	float sliceIdx = slice - sliceFrac;

	lookup.x = (lookup.x + sliceIdx) / chartDim;

	// lookup adjacent slices
	float3 col0 = tex2Dlod(chartSampler, lookup); 	
	lookup.x += 1.0 / chartDim;
	float3 col1 = tex2Dlod(chartSampler, lookup);

	// linearly blend between slices 	 	
	cImage = col0 + (col1 - col0) * sliceFrac;
}

void reprojectPos(inout float4 pos)
{
	pos = mul( pos, g_mProjection );
	pos.xyz = pos.xyz / pos.w;

	pos.x = (pos.x + 1.0) * 0.5;
	pos.y = 1.0 - (pos.y + 1.0) * 0.5;
}


#endif

