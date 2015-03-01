//////////////////////////////////////////////////////////////////////////
// basic struct

#define REFLECT_OFF    0
#define REFLECT_CUBE   1
#define REFLECT_SPHERE 2


////////////////////////////////////////////////////////////////////////////////////////////////////
// Common vertex attributes

// Note: user should #NOT# use directly any app2vert structures for vertex data modifications and
//	use streamPos instead if any modification required

struct app2vertCommon
{
	float4 Position  : POSITION;
	float4 baseTC   : TEXCOORD0; 

#ifdef _MAX_
	float3 vTangent  : TANGENT;
	float3 vBinormal  : BINORMAL;
	float3 vNormal  : NORMAL;
#endif

//	float4 Color     : COLOR;



};


struct app2vertGeneral
{  
	app2vertCommon vertCommon;

// 	INST_STREAM_CUSTOM
// 
// #if %TEMP_VEGETATION
// 
// #if D3D10 || PS3
// #if %_RT_NOZPASS && %_RT_ALPHATEST
// 		float4 InstAlphaTest   : TEXCOORDN;
// #endif
// #endif
// 
// #endif

};

struct fragInput
{
	//===============================================================================
	// input attribute data
	float4 baseTC;
	float4 basesectorTC;
	float4 bumpTC;
	float4 terrainDetailTC;

	float4 vTangent;
	float4 vBinormal;
	float4 vNormal;
	float4 vView;

	float4 screenProj;
	float4 projTC;

	float4 Color;
	float4 Color1;
	float4 VisTerrainCol;
	float4 SunRefl;
	float4 Ambient;

	float4 AlphaTest;

	float3 DistAtten;
	float4 AvgFogVolumeContrib;

	float4 OutdoorAOInfo;
};

struct fragPass
{
	fragInput IN;

	// Usage flags fo shared stuff  
	bool bCustomComposition;      // dont use shared final shading composition
	bool bRenormalizeNormal;
	bool bForceRenormalizeNormal;
	bool bDontUseBump;
	bool bDiffuseBump;
	bool bDetailBumpMapping;
	bool bDetailBumpMappingMasking;
	bool bOffsetBumpMapping;
	bool bParallaxOcclusionMapping;  
	bool bVertexColors;      // apply vertex color to final result
	bool bAlphaGlow;         // use diffuse texture alpha has glow
	bool bHemisphereLighting;
	bool bDontUseEmissive;
	bool bRefractionMap; 
	bool bTerrainPass;  
	bool bDisableInShadowShading;  
	bool bDisableAlphaTestD3D10; 
	bool bDeferredSpecularShading;
	bool bSkipMaterial;
	int  nReflectionMapping; // reflection mapping type (0: off, 1: cube map, 2: spherical)
	float fBumpHeightScale;
	float fSelfShadowStrength;

	float2 vDetailBumpTilling; // detail diffuse_bump mapping tilling
	float fDetailBlendAmount;	// detail blend amount
	float fDetailBumpScale; 	 // detail bump mapping scale

	float fLod;		 					 // lod scale: 1 max, 0 disabled

	// shared fields
	int nQuality;           // shader quality level

	float3x3 mTangentToWS;  // tangent to world space transformation matrix - might be required for some vectors
	float3 vView;           // eye vector, fFogFrac
	float3 vNormal;         // normal vector
	float3 vNormalDiffuse;  // diffuse normal vector
	float3 vReflVec;        // reflection vector

	float3 cBumpMap;        // tangent space normal map
	float4 cDiffuseMap;     // diffuse map
	float4 cGlossMap;       // specular/gloss map
	float3 cEnvironment;    // environment map
	float4 cShadowOcclMap;  // shadow map

	float fNdotE;           // per pass constant NdotE
	float fSpecPow;         // specular power
	float fAlpha;           // opacity   
	float fAlphaTestRef;    // instanced alpha test value
	float cAOMapVal;        // value from AO target

	// deferred rendering RTs
	float4 cNormalMapRT;    // normal maps render target
	float4 cDiffuseAccRT;   // diffuse accumulation render target
	float4 cSpecularAccRT;   // specular accumulation render target

	// Note: parser bug, assumes fReflectionAmount, fFresnelBias, etc, are same as tweakable parameters, therefore the _ was added

	float fReflection_Amount;  // amount of reflection  
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

struct pixout
{
	float4 Color  : COLOR0;
};

struct pixout_fp
{
	float4 Color  : COLOR0;
};

// struct vert2frag
// {
// 	float4 HPosition  : POSITION;
// 	float4 baseTC     : TEXCOORD0;
// };


