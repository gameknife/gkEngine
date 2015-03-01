//--------------------------------------------------------------------------------------
// 	File: ksVertexLightning.fx
//	Desc: The effect file for basic vertex lightning. 
//
//  The files of KnifeShader Lab.
// 	It could be used in 3dsMax, FxComposer & DirectX
//
// 	by Kaimingyi 2010.8.18
//--------------------------------------------------------------------------------------


#include "$BasePara.h"

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------

void VertexScene( float4 inPos : POSITION,
		  float2 inTex0 : TEXCOORD0,
		  float4 inColor0 : COLOR1,
		  out float4 oPos 	: POSITION,              
          out float2 oTex0 	: TEXCOORD0,
		  out float4 oColor0    : TEXCOORD1)
{	
    oPos = mul( inPos, g_mWorldViewProj );
    oTex0 = inTex0;
    oColor0 = inColor0;
}

void PixelScene( float2 inTex0 	: TEXCOORD0,
		 float4 inColor0 : TEXCOORD1,
				out float4 oColor 	: COLOR0           
				)
{	
	oColor = tex2D(samDiffuse, inTex0);
	oColor.a *= inColor0.a;
}

//--------------------------------------------------------------------------------------
// Techniques
//--------------------------------------------------------------------------------------
technique RenderScene
{
    pass P0
    {
		//ZEnable = 0;
		//AlphaBlendEnable = TRUE;
		//CullMode = none;
		VertexShader = compile vs_2_0 VertexScene();
		//AlphaBlendEnable = TRUE;
		PixelShader = compile ps_2_0 PixelScene();
		//AlphaBlendEnable = TRUE;

    }
}