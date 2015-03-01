//////////////////////////////////////////////////////////////////////////
//
// K&K Studio GameKnife ENGINE Source File
//
// Name:   	BasePara.fxh
// Desc:	That's the default param defines
// 
// Author:  Kaiming
// Date:	2011/11/8 
// Modify:	2011/11/8
// 
//////////////////////////////////////////////////////////////////////////


//--------------------------------------------------------------------------------------
// Global Shared Matrixs
//--------------------------------------------------------------------------------------
shared float4x4 g_mWorld      			: 		WORLD;
shared float4x4 g_mWorldI				:		WORLDI;
shared float4x4 g_mView       			: 		VIEW;
shared float4x4 g_mViewI				:		VIEWI;
shared float4x4 g_mProjection 			: 		PROJECTION;
shared float4x4 g_mWorldViewProj 		: 		WORLDVIEWPROJ;
shared float4x4 g_mWorldView 			: 		WORLDVIEW;

//////////////////////////////////////////////////////////////////////////
// Global Shared Vecs
shared float4 g_fScreenSize = float4( 0.000781, 0.001389, 0, 0);
// Global Time
shared float g_fTime : TIME;


// internal value
uniform float g_ColorMask = 0;


// lightinfo. Expose for 3ds Max
// lightinfo. Expose for 3ds Max
#ifdef _MAX_
float3 g_LightPos : POSITION <  
string UIName = "Light Position"; 
string Object = "PointLight";
string Space = "World";
int refID = 0;
> = float3(-500,-500,-500);
#else
float3 g_LightPos = float3(-500,-500,-500);
#endif

float4 g_LightDiffuse<
	string UIName = "global:SunColor";
> = float4(1.0f, 1.0f, 1.0f, 1.0f);
float4 g_LightSpecular = float4(1.0f, 1.0f, 1.0f, 1.0f);
float4 g_SkyLight<
	string UIName = "global:SkyColor";
> = float4(0.2f, 0.2f, 0.2f, 1.0f);
float g_LightIntensity = 1.0f;

// HDR SCALAR
float g_HDRScalar = 1.0f;

// Exposed Color define
float4 g_MatDiffuse<
	string UIName = "Diffuse Color";
> = float4(0.1f, 0.1f, 0.1f, 1.0f);

float4 g_MatSpecular<
	string UIName = "Specular Color";
> = float4(0.1f, 0.1f, 0.1f, 1.0f);

float g_Gloss<
	string UIName = "Glossness";
	string UIType = "FloatSpinner";
	float UIMin = 0.0f;
	float UIMax = 255.0f;	
> = 32.0f;

float g_Fresnel<
string UIName = "Fresnel";
string UIType = "FloatSpinner";
float UIMin = 0.0f;
float UIMax = 5.0f;	
> = 5.0f;

float g_FresnelScale<
string UIName = "FresnelScale";
string UIType = "FloatSpinner";
float UIMin = 0.0f;
float UIMax = 50.0f;	
> = 1.0f;

float g_FresnelBia<
string UIName = "FresnelBia";
string UIType = "FloatSpinner";
float UIMin = 0.0f;
float UIMax = 1.0f;	
> = 0.5f;

// float g_DetailTillX<
// string UIName = "DetailTillX";
// string UIType = "FloatSpinner";
// float UIMin = 0.0f;
// float UIMax = 50.0f;	
// > = 10.0f;
// 
// float g_DetailTillY<
// string UIName = "DetailTillY";
// string UIType = "FloatSpinner";
// float UIMin = 0.0f;
// float UIMax = 50.0f;	
// > = 10.0f;
// 
// float g_DetailAmount<
// string UIName = "DetailAmount";
// string UIType = "FloatSpinner";
// float UIMin = 0.1f;
// float UIMax = 10.0f;	
// > = 1.0f;



//////////////////////////////////////////////////////////////////////////
// Sampler defines
//////////////////////////////////////////////////////////////////////////
// diffuse map: s0
texture texDiffuse : DIFFUSEMAP
< 
    string name = "";
	string UIName = "ColorMap";
	int Texcoord = 0;
>;

sampler2D samDiffuse: register(s0) = sampler_state
{
	Texture = <texDiffuse>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
	AddressU = WRAP;
	AddressV = WRAP;
};

// normal map: s1
texture texNormal : NORMALMAP< 
	string UIName = "NormalMap";
	int Texcoord = 0;
>;

sampler2D samNormal : register(s1) = sampler_state
{
	Texture = <texNormal>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
	AddressU = WRAP;
	AddressV = WRAP;
};

 texture texSpec : SPECULARMAP< 
 string UIName = "SpecularMap";
 int Texcoord = 0;
 >;
 
 sampler2D samSpec : register(s2) = sampler_state
 {
 	Texture = <texSpec>;
 	MinFilter = Linear;
 	MagFilter = Linear;
 	MipFilter = Linear;
	AddressU = WRAP;
	AddressV = WRAP;
 };
 
// texture texDetail : DetailMap< 
// string UIName = "DetailMap";
// int Texcoord = 0;
// >;
// 
// sampler2D samDetail : register(s3) = sampler_state
// {
// 	Texture = <texDetail>;
// 	MinFilter = Linear;
// 	MagFilter = Linear;
// 	MipFilter = Linear;
// };

// spec map: s2
// spec

// preserve: s3 - s5
// detail, etc

// engine tex: s6 - s16
sampler2D samLightDiffuseLayer: register(s6) = sampler_state
{
	AddressU = Clamp;
    AddressV = Clamp;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = Point;
};

sampler2D samLightSpecLayer: register(s7) = sampler_state
{
	AddressU = Clamp;
    AddressV = Clamp;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = Point;
};

sampler2D samSceneColor: register(s8) = sampler_state
{
	AddressU = Clamp;
    AddressV = Clamp;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = Point;
};

sampler2D samSceneDepth: register(s9) = sampler_state
{
	AddressU = Clamp;
    AddressV = Clamp;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = Point;
};

sampler2D samSceneNormal: register(s10) = sampler_state
{
	AddressU = Clamp;
	AddressV = Clamp;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = Point;
};

sampler2D samShadowMask: register(s11) = sampler_state
{
	AddressU = Clamp;
	AddressV = Clamp;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = Point;
};


texture IBLcubemap
	<
	string ResourceName = "";
string UIName = "     Cubemap";
string ResourceType = "Cube";
	>;

	samplerCUBE IBLcubemapSampler : register(s3)  = sampler_state
	{
		Texture = <IBLcubemap>;
		MinFilter = LINEAR;
		MagFilter = LINEAR;
		MipFilter = LINEAR;
		AddressU = WRAP;
		AddressV = WRAP;
	};

	texture IBLenvmap
		<
		string ResourceName = "";
	string UIName = "     2D Env";
		>;

		sampler2D IBLenvmapSampler : register(s3)  = sampler_state
		{
			Texture = <IBLenvmap>;
			MinFilter = LINEAR;
			MagFilter = LINEAR;
			MipFilter = LINEAR;
		};

#define QUALITY_LOW      0
#define QUALITY_MEDIUM   1
#define QUALITY_HIGH   	 2
#define QUALITY_VERYHIGH 3

int GetShaderQuality()
{
	int nQuality;
//#if !%_RT_QUALITY && !%_RT_QUALITY1
//	nQuality = QUALITY_LOW;
// #elif %_RT_QUALITY && !%_RT_QUALITY1
 	nQuality = QUALITY_MEDIUM;
// #elif !%_RT_QUALITY && %_RT_QUALITY1
// 	nQuality = QUALITY_HIGH;
// #else
// 	// #warning Unknown shader quality mode
// 	nQuality = QUALITY_HIGH;
// #endif  
	return nQuality;
}
