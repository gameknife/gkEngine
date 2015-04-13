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

#ifndef _BasePara_h_
#define _BasePara_h_

//--------------------------------------------------------------------------------------
// Global Shared Matrixs
//--------------------------------------------------------------------------------------
float4x4 g_mWorld      			: 		WORLD;
float4x4 g_mWorldI				:		WORLDI;
float4x4 g_mView       			: 		VIEW;
float4x4 g_mViewI				:		VIEWI;
float4x4 g_mProjection 			: 		PROJECTION;
float4x4 g_mWorldViewProj 		: 		WORLDVIEWPROJ;
float4x4 g_mWorldView 			: 		WORLDVIEW;

#define HKG_MAX_BLEND_MATRICES 56
float4x4 g_mLocalMatrixArray[HKG_MAX_BLEND_MATRICES];

#define COMPRESS_NORMAL

//////////////////////////////////////////////////////////////////////////
// Global Shared Vecs
shared float4 g_fScreenSize = float4( 0.000781, 0.001389, 0.25, 8000);
shared float4 g_camPos;
shared float4 g_camDir;
// Global Time
float g_fTime : TIME;
int g_FrameCount;

// internal value
uniform float g_ColorMask = 0;

float4 g_UVParam = float4(1,1,0,0);


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

float3 g_SunPos;

float4 g_LightDiffuse;

float4 g_LightSpecular = float4(1.0f, 1.0f, 1.0f, 1.0f);

float4 g_SkyLight;

float g_LightIntensity = 1.0f;

float4 g_LightAttenuation = float4(0.0f, 0.0f, 0.0f, 1.0f);

// Exposed Color define
float4 g_MatDiffuse<
	string UIName = "Diffuse Color";
> = float4(0.5f, 0.5f, 0.5f, 1.0f);

float4 g_MatSpecular<
	string UIName = "Specular Color";
> = float4(0.1f, 0.1f, 0.1f, 1.0f);

float g_Gloss<
	string UIName = "Glossness";
	string UIType = "FloatSpinner";
	float UIMin = 0.0f;
	float UIMax = 2550.0f;	
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
> = 1.0f;

float g_EnvironmentBia<
string UIName = "EnvironmentBia";
string UIType = "FloatSpinner";
float UIMin = 0.0f;
float UIMax = 2.0f;	
> = 1.0f;

float g_DetailTilling<
	string UIName = "DetailTilling";
string UIType = "FloatSpinner";
float UIMin = 0.1f;
float UIMax = 100.0f;	
> = 40.0f;

float g_DetailAmount<
	string UIName = "DetailAmount";
string UIType = "FloatSpinner";
float UIMin = -2.0f;
float UIMax = 2.0f;	
> = 1.0f;

float g_AlphaRef<
	string UIName = "AlphaRef";
string UIType = "FloatSpinner";
float UIMin = 0.0f;
float UIMax = 1.0f;	
> = 0.0f;

float g_Glow<
	string UIName = "Glow";
string UIType = "FloatSpinner";
float UIMin = 0.0f;
float UIMax = 100.0f;	
> = 1.0f;

float g_Opacity<
	string UIName = "Opacity";
string UIType = "FloatSpinner";
float UIMin = 0.0f;
float UIMax = 1.0f;	
> = 1.0f;


#define QUALITY_LOW      0
#define QUALITY_MEDIUM   1
#define QUALITY_HIGH   	 2
#define QUALITY_VERYHIGH 3

int GetShaderQuality()
{
	int nQuality;
 	nQuality = QUALITY_MEDIUM;
	return nQuality;
}

#endif