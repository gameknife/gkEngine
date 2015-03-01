//-----------------------------------------------------------------------------
// File: ParticleCommon.fx
//
// Desc: GPUÁ£×ÓµÄ¼ÆËãshader
//
// Author: Yi Kaiming 2011/10/23
//-----------------------------------------------------------------------------

//////////////////////////////// Common samplers ////////////////
#include "$BasePara.h"
#include "$SamplerDef.h"

sampler VTFSampler: register(s0);

//--------------------------------------------------------------------------------------
// Vertex shader output structure
//--------------------------------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Position   : POSITION;   // vertex position 
    float2 TextureUV  : TEXCOORD0;  // vertex texture coords 
};

VS_OUTPUT RenderParticleVS( float2 vTexCoord0 : TEXCOORD0,
						 float2 vTexCoord1 : TEXCOORD1)
{
    VS_OUTPUT Output;
    
    // Transform the position from object space to homogeneous projection space

	float4 posVTF = 1;
	posVTF = tex2Dlod(VTFSampler, float4(vTexCoord1,1,1));

	posVTF.xyz *= 3.0;
	//posVTF.xyz = 0;

	float2 offset = vTexCoord0 - float2(0.5, 0.5);
	posVTF.xyz += g_mViewI[0].xyz * (offset.x * -0.1/* * posVTF.w*/);
	posVTF.xyz += g_mViewI[1].xyz * (offset.y * 0.1/* * posVTF.w*/);

	Output.Position = mul(float4(posVTF.xyz, 1), g_mWorldViewProj);
	    
    // Just copy the texture coordinate through
    Output.TextureUV = vTexCoord0; 
    
    return Output;    
}

//--------------------------------------------------------------------------------------
// Pixel shader output structure
//--------------------------------------------------------------------------------------
struct PS_OUTPUT
{
    float4 RGBColor : COLOR0;  // Pixel color    
};

PS_OUTPUT RenderParticlePS( VS_OUTPUT In ) 
{ 
	PS_OUTPUT Output;

	// Lookup mesh texture and modulate it with diffuse
	Output.RGBColor = tex2D(samDiffuse, In.TextureUV);
	clip( Output.RGBColor.a - 0.01 );
	Output.RGBColor.xyz *= 2;

	return Output;
}

technique RenderScene
{
    pass P0
    {          
        VertexShader = compile vs_3_0 RenderParticleVS();
        PixelShader  = compile ps_3_0 RenderParticlePS(); 
    }
}
