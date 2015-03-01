//-----------------------------------------------------------------------------
// File: ksGenerateSM.fx
//
// Desc: äÖÈ¾shadowmapµÄshader
//
// Author: Yi Kaiming 2010/12/29
//-----------------------------------------------------------------------------
#include "$BasePara.h"

#define SHADOW_EPSILON 0.0f

texture texDiffuse;

sampler2D samDiffuse = sampler_state
{
	Texture = <texDiffuse>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
};

void skinPosition( float3 skinPos, float blendWeightsArray[4], int indexArray[4], out float3 position)
{
	// calculate the pos/normal using the "normal" weights 
	// and accumulate the weights to calculate the last weight
	position = 0.0f;
	float lastWeight = 0.0f;
	for (int iBone = 0; iBone < 3; iBone++)
	{
		lastWeight = lastWeight + blendWeightsArray[iBone];
		position += mul( float4(skinPos,1), g_mLocalMatrixArray[indexArray[iBone]] ) * blendWeightsArray[iBone];
	}

	lastWeight =  1.0f - lastWeight; 

	// Now that we have the calculated weight, add in the final influence
	position += (mul( float4(skinPos,1), g_mLocalMatrixArray[indexArray[3]] ) * lastWeight);		
}

//-----------------------------------------------------------------------------
// Vertex Shader: VertShadow
// Desc: Process vertex for the shadow map
//-----------------------------------------------------------------------------
void VertShadow( float4 inPos : POSITION,
				 float4 inNorm : TEXCOORD0,
				 float4 inTex0 	: TEXCOORD1,    
				 out float4 oPos : POSITION)
{
    //
    // Compute the projected coordinates
    //
    oPos = mul( float4(inPos.xyz, 1), g_mWorldViewProj );
}

void VertShadowSkinned( float4 inPos : POSITION,
	float4 inNorm : TEXCOORD0,
	float4 inTex0 	: TEXCOORD1,    
	float4 inBlendWeights : BLENDWEIGHT,
	int4 inBlendIndices : BLENDINDICES,
	out float4 oPos : POSITION)
{

	float3 position;

	float blendWeightsArray[4] = (float[4])inBlendWeights;    
	int   indexArray[4] = (int[4])inBlendIndices;    
	skinPosition( inPos.xyz, blendWeightsArray, indexArray, position );     

	//
	// Compute the projected coordinates
	//
	oPos = mul( float4(position, 1), g_mWorldViewProj );
}

//-----------------------------------------------------------------------------
// Pixel Shader: PixShadow
// Desc: Process pixel for the shadow map
//-----------------------------------------------------------------------------
void PixShadow(
			   float2 inTex0 	: TEXCOORD0, 
			   float2 inDepth : TEXCOORD1,
               out float4 oColor : COLOR0 )
{

	//clip( tex2D(samDiffuse, inTex0).a - 0.35f);
    //
    // Depth is z / w
    //
	//oColor = saturate( inDepth.x / inDepth.y );

	// use hardware, just return
	oColor = 0;
}

//-----------------------------------------------------------------------------
// Technique: ZPass
// Desc: Renders the shadow map
//-----------------------------------------------------------------------------
technique ShadowZPass
{
    pass p0
    {
        VertexShader = compile vs_3_0 VertShadow();
        PixelShader = compile ps_3_0 PixShadow();
    }
}

technique ShadowZPassSkinned
{
	pass p0
	{
		VertexShader = compile vs_3_0 VertShadowSkinned();
		PixelShader = compile ps_3_0 PixShadow();
	}
}
