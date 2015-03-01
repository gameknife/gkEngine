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

texture texSkt0 < 
string UIName = "Sketch0";
>;

sampler2D samSkt0 = sampler_state
{
	Texture = <texSkt0>;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = Point;
};


texture texSkt1 < 
string UIName = "Sketch1";
>;

sampler2D samSkt1 = sampler_state
{
	Texture = <texSkt1>;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = Point;
};


texture texSkt2 < 
string UIName = "Sketch2";
>;

sampler2D samSkt2 = sampler_state
{
	Texture = <texSkt2>;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = Point;
};


texture texSkt3 < 
string UIName = "Sketch3";
>;

sampler2D samSkt3 = sampler_state
{
	Texture = <texSkt3>;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = Point;
};


texture texSkt4 < 
string UIName = "Sketch4";
>;

sampler2D samSkt4 = sampler_state
{
	Texture = <texSkt4>;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = Point;
};

texture texSkt5 < 
string UIName = "Sketch5";
>;

sampler2D samSkt5 = sampler_state
{
	Texture = <texSkt5>;
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
	
	float diffuse = Light_Point_Diffuse(inNormal, inLightVector);
	float4 lightColor = tex1D(samFresnel, diffuse);

	float4 texCol = tex2D(samSkt0, inTex0) * (lightColor.x < 0.167f);
	texCol += tex2D(samSkt1, inTex0) * (lightColor.x > 0.166f && lightColor.x < 0.333f);
	texCol += tex2D(samSkt2, inTex0) * (lightColor.x > 0.3f && lightColor.x < 0.5f);
	texCol += tex2D(samSkt3, inTex0) * (lightColor.x > 0.45f && lightColor.x < 0.667f);
	texCol += tex2D(samSkt4, inTex0) * (lightColor.x > 0.62f && lightColor.x < 0.833f);
	texCol += tex2D(samSkt5, inTex0) * (lightColor.x > 0.8f);
	
	oColor = saturate(texCol);
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


