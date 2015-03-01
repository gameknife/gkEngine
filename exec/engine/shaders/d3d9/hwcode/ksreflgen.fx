//-----------------------------------------------------------------------------
// File: ksReflGen.fx
//
// Desc: äÖÈ¾reflectmapµÄshader
//
// Author: Yi Kaiming 2010/12/29
//-----------------------------------------------------------------------------
#include "$BasePara.h"

texture texDiffuse;

float g_fWaterLevel = -1;

sampler2D samDiffuse = sampler_state
{
	Texture = <texDiffuse>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
};

//-----------------------------------------------------------------------------
// Vertex Shader: VertShadow
// Desc: Process vertex for the shadow map
//-----------------------------------------------------------------------------
void Vertpass( float4 inPos : POSITION,
				float2 inTex0 		: TEXCOORD0,
				out float4 oPos 	: POSITION,              
                out float2 oTex0 	: TEXCOORD0)
{
	oPos = mul( inPos, g_mWorld );
	oPos.z = 2.0 * g_fWaterLevel - oPos.z;
	if( oPos.z > 1.0)
		 oPos.z = 100;
	//oPos.z = min( oPos.z, );
	oPos = mul( oPos, g_mView );
	oPos = mul( oPos, g_mProjection );
	oTex0 = inTex0;
}

//-----------------------------------------------------------------------------
// Pixel Shader: PixShadow
// Desc: Process pixel for the shadow map
//-----------------------------------------------------------------------------
void Pixpass(
			   float2 inTex0 	: TEXCOORD0, 
               out float4 oColor 	: COLOR0
			  )
{
	float4 color = tex2D(samDiffuse, inTex0);
	clip(color.a - 0.3);
	oColor = pow( color, 2.2f);
}

//-----------------------------------------------------------------------------
// Technique: ZPass
// Desc: Renders the shadow map
//-----------------------------------------------------------------------------
technique BasicPass
{
    pass p0
    {
        VertexShader = compile vs_2_0 Vertpass();
        PixelShader = compile ps_2_0 Pixpass();
		CullMode = CCW;
		//ZEnable = FALSE;
		//Zwriteable = FALSE;
		//ZWriteEnable = FALSE;
		//StencilEnable = FALSE;
    }
}