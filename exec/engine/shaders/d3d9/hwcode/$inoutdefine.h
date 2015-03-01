//////////////////////////////////////////////////////////////////////////
//
// yikaiming (C) 2014
// gkENGINE Source File 
//
// Name:   	$InOutDefine.h
// Desc:	
// 	
// 
// Author:  gameKnife
// Date:	2015/2/25
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _InOutDefine_h_
#define _InOutDefine_h_

//////////////////////////////////////////////////////////////////////////
// basic struct

#define REFLECT_OFF    0
#define REFLECT_CUBE   1
#define REFLECT_SPHERE 2

////////////////////////////////////////////////////////////////////////////////////////////////////
// Common vertex attributes

// Note: user should #NOT# use directly any app2vert structures for vertex data modifications and
//	use streamPos instead if any modification required

struct fragInput
{
	//===============================================================================
	// input attribute data
	float4 baseTC;
	float4 bumpTC;
	float4 terrainDetailTC;

	float4 vTangent;
	float4 vBinormal;
	float4 vNormal;
	float4 vView;

	float4 screenProj;
	float4 projTC;
};

struct fragPass
{
	fragInput IN;

	// Usage flags fo shared stuff  
	bool bCustomComposition;      // dont use shared final shading composition
	bool bDontUseEmissive;
	bool bRefractionMap; 
	bool bTerrainPass;   
	bool bDisableAlphaTestD3D10; 
	bool bDeferredSpecularShading;
	bool bSkipMaterial;
	bool bDiffuseAlphaSpec;
	bool bSpecAlphaShiness;

	bool bSSRL;
	bool bLightmap;

	int  nReflectionMapping; // reflection mapping type (0: off, 1: cube map, 2: spherical)
	float fBumpHeightScale;
	float fSelfShadowStrength;

	float2 vDetailBumpTilling; // detail diffuse_bump mapping tilling
	float fDetailBlendAmount;	// detail blend amount
	float fDetailBumpScale; 	 // detail bump mapping scale

	// shared fields
	int nQuality;           // shader quality level

	float3 vView;           // eye vector, fFogFrac
	float3 vNormal;         // normal vector
	float3 vReflVec;        // reflection vector

	float3 cBumpMap;        // tangent space normal map
	float4 cDiffuseMap;     // diffuse map
	float4 cGlossMap;       // specular/gloss map
	float3 cEnvironment;    // environment map
	float4 cShadowOcclMap;  // shadow map

	float fNdotE;           // per pass constant NdotE
	float fSpecPow;         // specular power
	float fAlpha;           // opacity   
	
	// deferred rendering RTs
	float4 cNormalMapRT;    // normal maps render target
	float4 cDiffuseAccRT;   // diffuse accumulation render target
	float4 cSpecularAccRT;   // specular accumulation render target

	float fFresnel_Bias;     // fresnel biasing
	float fFresnel_Scale;    // fresnel scale
	float fFresnel_Pow;         // final fresnel term
	float fFresnel;  


	float fAlphaGlow_Multiplier;   // apply multiplier to diffuse alpha glow 

	// Shading accumulation
	float3 cAmbientAcc;
	float3 cDiffuseAcc;
	float3 cSpecularAcc;

	// Custom per pass data   
	fragPassCustom pCustom;
};



struct fragLightPass
{
	int nType;          // light type
	float3 cDiffuse;    // light diffuse color
	float3 cSpecular;   // light specular color
	float3 cFilter;     // light filter color
	float3 vLight;      // light vector

	float fNdotL;       // normal dot light
	float fFallOff;     // light attenuation  
	float fOcclShadow;  // light shadow term

	float3 cOut;        // light final contribution  

	// ... Custom per light data ...
	fragLightPassCustom pCustom;
}; 

#endif
