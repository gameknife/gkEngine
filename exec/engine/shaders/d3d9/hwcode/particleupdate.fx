//-----------------------------------------------------------------------------
// File: ParticleCommon.fx
//
// Desc: GPU粒子的计算shader
//
// Author: Yi Kaiming 2011/10/23
//-----------------------------------------------------------------------------

//////////////////////////////// Common samplers ////////////////
#include "./$postlib.h"

sampler tex0: register(s0) = 
	sampler_state
{
	MipFilter = POINT;
	MinFilter = POINT;
	MagFilter = POINT;
};

sampler tex1: register(s1) = 
	sampler_state
{
	MipFilter = POINT;
	MinFilter = POINT;
	MagFilter = POINT;
};

//--------------------------------------------------------------------------------------
// Pixel shader output structure
//--------------------------------------------------------------------------------------
struct PS_OUTPUT
{
    float4 RGBColor : COLOR0;  // Pixel color    
};

struct PS_OUTPUT_MRT
{
	float4 RGBColor0 : COLOR0;  // Pixel color   
	float4 RGBColor1 : COLOR1;  // Pixel color
};

float4 g_staticForce;
float4 g_timeVar;

float4 g_attract0;
float4 g_attract1;
float4 g_attract2;

float3 Verlet(in float3 prev, in float3 curr, in float3 force)
{
	return curr + 0.9f * (curr - prev) + force * g_timeVar.x * g_timeVar.x;
}

//--------------------------------------------------------------------------------------
// This shader outputs the pixel's color by modulating the texture's
// color with diffuse material color
//--------------------------------------------------------------------------------------
PS_OUTPUT Verlet_StaticForcePS( float2 TextureUV : TEXCOORD0 ) 
{ 
	PS_OUTPUT Output = (PS_OUTPUT)0;

	// Lookup mesh texture and modulate it with diffuse
	float3 prevPos = tex2D(tex0, TextureUV);
	float3 currPos = tex2D(tex1, TextureUV);

	float3 vec0 = g_attract0.xyz - currPos;
	float3 vec1 = g_attract1.xyz - currPos;
	float3 vec2 = g_attract2.xyz - currPos;

	vec0 = normalize(vec0) / length(vec0);
	vec1 = normalize(vec1) / length(vec1);
	vec2 = normalize(vec2) / length(vec2);

	float3 mergeForce = vec0 + vec1 + vec2;
	mergeForce = normalize(mergeForce);

	float3 nextPos = Verlet(prevPos, currPos, mergeForce);

	Output.RGBColor.xyz = nextPos;

	return Output;
}

PS_OUTPUT RandomPS( float2 TextureUV : TEXCOORD0 ) 
{ 
	PS_OUTPUT Output = (PS_OUTPUT)0;

	float3 randomPos = tex2D( tex0, TextureUV.xy );
	Output.RGBColor.xyz = randomPos;

	return Output;
}

int g_emitCount;
float4 g_emitPos;
float4 g_emitParam;
float4 g_emitIndex;

PS_OUTPUT_MRT EmitPS( float2 TextureUV : TEXCOORD0, float2 vPos :VPOS )
{
	PS_OUTPUT_MRT Output = (PS_OUTPUT_MRT)0;
	float4 prevVel = tex2D(tex0, TextureUV);
	float4 prevPos = tex2D(tex1, TextureUV);

	// 根据发射数量来发射
//   	for ( int i=0; i < g_emitCount; ++i )
//   	{
 		if ( length(vPos - g_emitIndex.xy) < 0.001/* && prevVel.w < 0.001*/)
 		{
			// 更新速度
			prevVel.xyz = g_emitParam.xyz;
			prevVel.w = 0.001;

			// 更新位置
			prevPos.xyz = g_emitPos.xyz;
 		}
// 	}


	Output.RGBColor0 = prevVel;
	Output.RGBColor1 = prevPos;
	return Output;
}

PS_OUTPUT_MRT ExpirePS( float2 TextureUV : TEXCOORD0 )
{
	PS_OUTPUT_MRT Output = (PS_OUTPUT_MRT)0;
	float4 prevVel = tex2D(tex0, TextureUV);
	float4 prevPos = tex2D(tex1, TextureUV);

	// 5s 后死亡
	if ( prevVel.w > 1000.0)
	{
		prevVel = 0;
		// HIDE
		prevPos = float4(-10000,-10000,-10000,prevPos.w);

		//prevPos = float4(0,0,0,prevPos.w);
	}

	Output.RGBColor0 = prevVel;
	Output.RGBColor1 = prevPos; 
	return Output;
}

PS_OUTPUT Eular_AttractionPS( float2 TextureUV : TEXCOORD0 ) 
{ 
	PS_OUTPUT Output = (PS_OUTPUT)0;

	// Lookup mesh texture and modulate it with diffuse
	float4 prevVel = tex2D(tex0, TextureUV);
	if (prevVel.w < 0.0001)
	{
		Output.RGBColor = prevVel;

		return Output;
	}
	prevVel.w += g_timeVar.x;

	float3 currPos = tex2D(tex1, TextureUV);

	float3 vec0 = g_attract0.xyz - currPos;
	float3 vec1 = g_attract1.xyz - currPos;
	float3 vec2 = g_attract2.xyz - currPos;

	vec0 = normalize(vec0) / length(vec0);
	vec1 = normalize(vec1) / length(vec1);
	vec2 = normalize(vec2) / length(vec2);

	float3 mergeForce = vec0 + vec1 + vec2;

	float3 nextPos = prevVel.xyz + mergeForce * 2.0 * g_timeVar.x;

	Output.RGBColor.xyz = nextPos;
	Output.RGBColor.w = prevVel.w;

	return Output;
}

PS_OUTPUT Eular_UpdatePS( float2 TextureUV : TEXCOORD0 ) 
{ 
	PS_OUTPUT Output = (PS_OUTPUT)0;

	// Lookup mesh texture and modulate it with diffuse
	float4 prevVel = tex2D(tex0, TextureUV);
	float4 prevPos = tex2D(tex1, TextureUV);

	float3 nextPos = prevPos.xyz + prevVel.xyz * g_timeVar.x;

	Output.RGBColor.xyz = nextPos;
	Output.RGBColor.w = prevPos.w;
	return Output;
}

technique Verlet_StaticForce
{
	pass P0
	{
		VertexShader = null;
		PixelShader  = compile ps_3_0 Verlet_StaticForcePS(); 
	}
};

technique Eular_Attraction
{
	pass P0
	{
		VertexShader = null;
		PixelShader  = compile ps_3_0 Eular_AttractionPS(); 
	}
};

technique Eular_Update
{
	pass P0
	{
		VertexShader = null;
		PixelShader  = compile ps_3_0 Eular_UpdatePS(); 
	}
};

technique Emit
{
	pass P0
	{
		VertexShader = null;
		PixelShader  = compile ps_3_0 EmitPS(); 
	}
};

technique Expire
{
	pass P0
	{
		VertexShader = null;
		PixelShader  = compile ps_3_0 ExpirePS(); 
	}
};

technique Random
{
	pass P0
	{
		VertexShader = null;
		PixelShader  = compile ps_3_0 RandomPS(); 
	}
};
