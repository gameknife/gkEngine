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

#ifndef _SamplerDef_h_
#define _SamplerDef_h_



//////////////////////////////////////////////////////////////////////////
// Sampler defines
//////////////////////////////////////////////////////////////////////////
// diffuse map: s0
// texture texDiffuse : DIFFUSEMAP
// < 
//     string name = "";
// 	string UIName = "|---- DiffuseMap";
// 	int Texcoord = 0;
// >;

sampler2D samDiffuse: register(s0) = sampler_state
{
	//Texture = <texDiffuse>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
	AddressU = WRAP;
	AddressV = WRAP;
};

// normal map: s1
// texture texNormal : NORMALMAP< 
// 	string UIName = "|---- NormalMap";
// 	int Texcoord = 0;
// >;

sampler2D samNormal : register(s1) = sampler_state
{
	//Texture = <texNormal>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
	AddressU = WRAP;
	AddressV = WRAP;
};

//  texture texSpec : SPECULARMAP< 
//  string UIName = "|---- SpecularMap";
//  int Texcoord = 0;
//  >;
 
 sampler2D samSpec : register(s2) = sampler_state
 {
 	//Texture = <texSpec>;
 	MinFilter = Linear;
 	MagFilter = Linear;
 	MipFilter = Linear;
	AddressU = WRAP;
	AddressV = WRAP;
 };
 
// texture texDetail : DetailMap< 
// string UIName = "|---- DetailMap";
// int Texcoord = 0;
// >;
sampler2D samDetail : register(s3) = sampler_state
{
 	//Texture = <texDetail>;
 	MinFilter = Linear;
 	MagFilter = Linear;
 	MipFilter = Linear;
	AddressU = WRAP;
	AddressV = WRAP;
};

// preserve: s4 - s5
// texture texCustom1< 
// string UIName = "|---- Custom1";
// int Texcoord = 0;
// >;
sampler2D samCustom1 : register(s4) = sampler_state
{
 	//Texture = <texCustom1>;
 	MinFilter = Linear;
 	MagFilter = Linear;
 	MipFilter = Linear;
};

// texture texCustom2< 
// string UIName = "|---- Custom2";
// int Texcoord = 0;
// >;
sampler2D samCustom2 : register(s5) = sampler_state
{
 	//Texture = <texCustom2>;
 	MinFilter = Linear;
 	MagFilter = Linear;
 	MipFilter = Linear;
};

// cubemap
// texture IBLenvmap
// <
// 	string ResourceName = "";
// 	string UIName = "|---- Env Map";
// >;

sampler2D IBLenvmapSampler : register(s6)  = sampler_state
{
	//Texture = <IBLenvmap>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
};

// texture IBLcubemap
// <
// 	string ResourceName = "";
// 	string UIName = "|---- Cube Map";
// 	string ResourceType = "Cube";
// >;

samplerCUBE IBLcubemapSampler : register(s7)  = sampler_state
{
	//Texture = <IBLcubemap>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};



// engine tex: s8 - s16
sampler2D samLightDiffuseLayer: register(s8) = sampler_state
{
	AddressU = Clamp;
    AddressV = Clamp;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
};

sampler2D samLightSpecLayer: register(s9) = sampler_state
{
	AddressU = Clamp;
    AddressV = Clamp;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
};

sampler2D samSceneColor: register(s10) = sampler_state
{
	AddressU = Clamp;
    AddressV = Clamp;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
};

sampler2D samReflect: register(s14) = sampler_state
{
	AddressU = Clamp;
	AddressV = Clamp;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = Point;
};

sampler2D samSceneDepth: register(s11) = sampler_state
{
	AddressU = Clamp;
    AddressV = Clamp;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = Point;
};

sampler2D samSceneNormal: register(s12) = sampler_state
{
	AddressU = Clamp;
	AddressV = Clamp;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = Point;
};

sampler2D samShadowMask: register(s13) = sampler_state
{
	AddressU = Clamp;
	AddressV = Clamp;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
};

sampler2D samBlend25: register(s14) = sampler_state
{
	AddressU = Wrap;
	AddressV = Wrap;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = Point;
};

sampler2D samBlend50: register(s15) = sampler_state
{
	AddressU = Wrap;
	AddressV = Wrap;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = Point;
};



sampler2D _tex0 : register(s0)=
	sampler_state
{
	AddressU = Clamp;
	AddressV = Clamp;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = None;
};

sampler2D _tex1 : register(s1)=
	sampler_state
{
	AddressU = Clamp;
	AddressV = Clamp;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = None;
};

sampler2D _tex2 : register(s2)=
	sampler_state
{
	AddressU = Clamp;
	AddressV = Clamp;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = None;
};
sampler2D _tex3 : register(s3)=
	sampler_state
{
	AddressU = Clamp;
	AddressV = Clamp;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = Point;
};
sampler2D _tex4 : register(s4);
sampler2D _tex5 : register(s5);
sampler2D _tex6 : register(s6);
sampler2D _tex7 : register(s7);

sampler2D _tex0LL : register(s0)=
	sampler_state
{
	AddressU = Clamp;
	AddressV = Clamp;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = None;
};

sampler2D _tex1LL : register(s1)=
	sampler_state
{
	AddressU = Clamp;
	AddressV = Clamp;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = None;
};

sampler2D _tex2LL : register(s2)=
	sampler_state
{
	AddressU = Clamp;
	AddressV = Clamp;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = None;
};


#endif