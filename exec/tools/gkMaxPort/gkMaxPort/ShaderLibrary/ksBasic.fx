//--------------------------------------------------------------------------------------
// 	File: ksBasic.fx
//	Desc: The effect file for basic ambient lightning. 
//
//  The files of KnifeShader Lab.
// 	It could be used in 3dsMax, FxComposer & DirectX
//
// 	by Kaimingyi 2010.8.18
//--------------------------------------------------------------------------------------

struct fragPassCustom
{
	// Custom per pass data     
	float3 vTangent;        // perturbed tangent vector
	float3 vBinormal;       // perturbed binormal vector

	float fSpecMultiplier;
	float2 vSurfaceRoughness;
};

struct fragLightPassCustom
{
	float fDummy;
}; 


#include "include\InOutDefine.fxh"
#include "include\BasePara.fxh"
#include "include\Light_Calc.fxh"

//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------
// shared float4x4 g_mWorld      			: 		WORLD;
// shared float4x4 g_mWorldI				:		WorldInverse;
// shared float4x4 g_mView       			: 		VIEW;
// shared float4x4 g_mViewI				:		ViewInverse;
// shared float4x4 g_mProjection 			: 		PROJECTION;
// shared float4x4 g_mWorldViewProj 		: 		WORLDVIEWPROJ;
// shared float4x4 g_mWorldView 			: 		WORLDVIEW;
// 
// float4 g_LightDiffuse<
// 	string UIName = "Ambient Color";
// > = float4(0.1f, 0.1f, 0.1f, 1.0f);

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------

void VertexScene( float4 inPos : POSITION,
				float2 inTex0 : TEXCOORD0,
                float3 inNormal : NORMAL,
				out float4 oPos : POSITION,
                out float4 oColor : COLOR0,               
                out float2 oTex0 : TEXCOORD0 )
{
    oPos = mul( inPos, g_mWorldViewProj );
	oTex0 =inTex0;
	oColor = g_LightDiffuse;
}

//--------------------------------------------------------------------------------------
// Techniques
//--------------------------------------------------------------------------------------
technique RenderScene
{
    pass P0
    {
        VertexShader = compile vs_3_0 VertexScene();
    }
}
