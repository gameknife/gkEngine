// 3ds max effect file
// Simple Lighting Model

// This is used by 3dsmax to load the correct parser
//string ParamID = "0x0";

//DxMaterial specific 
#include "include\BasePara.fxh"
#include "include\Light_Calc.fxh"

float g_fOutline<
	string UIName = "Outline Width";
	string UIType = "FloatSpinner";
	float UIMin = 0.0f;
	float UIMax = 0.1f;	
>  = 0.002f;

bool g_bSpecular <
	string UIName = "Specular on/off";
> = true;

texture texFresnel < 
	string UIName = "FresnelMap";
>;

sampler1D samFresnel = 
sampler_state
{
    Texture = <texFresnel>;
	AddressU = Clamp;
    AddressV = Clamp;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
};

// transformations

void VertexScene( float4 inPos 		: POSITION,
				float2 inTex0 		: TEXCOORD0,
                float3 inNormal 	: NORMAL,
				out float4 oPos 	: POSITION,              
                out float2 oTex0 	: TEXCOORD0,
				out float3 oNormal	: TEXCOORD1,
				out float3 oLightVector : TEXCOORD2,
				out float3 oEyeVector:  TEXCOORD3,
				out float4 oPosTex	: TEXCOORD4
				)
{	
    oPos = mul( inPos, g_mWorldViewProj );
	oPosTex = mul( inPos, g_mWorldView );
	oTex0 = inTex0;
	oNormal = inNormal; // normal to worldspace
	oEyeVector = normalize(mul(g_mViewI[3], (float3x3)g_mWorldI) - inPos);  // get eyePos in worldspace
	oLightVector = mul(g_LightPos, (float3x3)g_mWorldI) -  inPos;
}

void PixelScene_Tex(
                float2 inTex0 	: TEXCOORD0,
				float3 inNormal	: TEXCOORD1,
				float3 inLightVector : TEXCOORD2,
				float3 inEyeVector:  TEXCOORD3,
				float4 inPosTex: TEXCOORD4,	
				out float4 oColor 	: COLOR0,  
				out float4 oNormal 	: COLOR1,
				out float4 oPosition 	: COLOR2 )
{
	//calculate the diffuse
	float4 texCol = tex2D(samDiffuse, inTex0);
	float diffuse;
		//calculate the specular
	if(g_bSpecular)
	{
		diffuse = Light_Point_Specular(inNormal, inLightVector, inEyeVector);
	}
	else
	{
		diffuse = Light_Point_Diffuse(inNormal, inLightVector);
	}
	
	float4 lightColor = tex1D(samFresnel, diffuse);
	

	
	
	oColor = saturate(lightColor * texCol);
	oNormal = float4( normalize( mul(inNormal, (float3x3)g_mWorldView)), 1.0f);
	oPosition = inPosTex;
}

technique RenderScene_Tex
{
    pass P0
    {
        // shaders
        //CullMode = None;
        //CullMode = CCW;
        VertexShader = compile vs_2_0 VertexScene();
        PixelShader  = compile ps_2_0 PixelScene_Tex();
    }  
}


