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
    MinFilter = Point;
    MagFilter = Point;
    MipFilter = Point;
};

// transformations

void VertexScene( float4 inPos 		: POSITION,
				float2 inTex0 		: TEXCOORD0,
                float3 inNormal 	: NORMAL,
				out float4 oPos 	: POSITION,              
                out float2 oTex0 	: TEXCOORD0,
				out float3 oNormal	: TEXCOORD1,
				out float3 oLightVector : TEXCOORD2,
				out float3 oEyeVector:  TEXCOORD3
				)
{	
    oPos = mul( inPos, g_mWorldViewProj );
	oTex0 = inTex0;
	oNormal = inNormal; // normal to worldspace
	oEyeVector = normalize(mul(g_mViewI[3], (float3x3)g_mWorldI) - inPos);  // get eyePos in worldspace
	oLightVector = mul(g_LightPos, (float3x3)g_mWorldI) -  inPos;
}


void VS0(
	float4 Position: POSITION0,
	float3 Normal:   NORMAL0,
	out float4 oPosition: POSITION0)
{
	oPosition = mul(Position, g_mWorldViewProj);
	float3 norm = mul (Normal,g_mWorldView);
    norm.x *= g_mProjection[0][0];
	norm.y *= g_mProjection[1][1];
	oPosition.xy += norm.xy * oPosition.z * g_fOutline;
	//oColor = float4(0,0,0,1);	
}

float4 PS0() : COLOR0
{
	return float4(0,0,0,1);
}



void PixelScene_Tex(
                float2 inTex0 	: TEXCOORD0,
				float3 inNormal	: TEXCOORD1,
				float3 inLightVector : TEXCOORD2,
				float3 inEyeVector:  TEXCOORD3,
				out float4 oColor 	: COLOR0     )
{
	//calculate the diffuse
	float4 texCol = tex2D(samDiffuse, inTex0);
	float diffuse = Light_Point_Diffuse(inNormal, inLightVector);
	float4 lightColor = tex1D(samFresnel, diffuse);
	float4 specular4 = {0,0,0,0};
	
	//calculate the specular
	if(g_bSpecular)
	{
		float specular = Light_Point_Specular(inNormal, inLightVector, inEyeVector);
		specular = saturate((specular - 0.8) * 1000.0);
		specular4 = specular * float4(1.0, 1.0, 1.0, 0.0);	
	}
	
	oColor = saturate(lightColor * texCol) + specular4;
	//oNormal = float4( mul(Normal, (float3x3)g_mWorldView), 1.0f);
	//oPosition = PosTex;
}

technique RenderScene_Tex
{
    pass P0
    {
        // shaders
        //CullMode = None;
        CullMode = CW;
        VertexShader = compile vs_2_0 VertexScene();
        PixelShader  = compile ps_2_0 PixelScene_Tex();
    }  

	pass P1
    {
        // shaders
        CullMode = CCW;
        VertexShader = compile vs_2_0 VS0();
        PixelShader  = compile ps_2_0 PS0();
    } 
}


