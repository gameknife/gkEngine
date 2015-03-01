//--------------------------------------------------------------------------------------
// 	File: ksPixelLightning.fx
//	Desc: The effect file for basic pixel lightning. 
//
//  The files of KnifeShader Lab.
// 	It could be used in 3dsMax, FxComposer & DirectX
//
// 	by Kaimingyi 2010.8.18
//--------------------------------------------------------------------------------------
string ParamID = "0x003";

#define _MAX_

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

#include "InOutDefine.fxh"
#include "BasePara.fxh"
#include "Light_Calc.fxh"
#include "ksVertexLib.fxh"

void frag_unify_parameters( inout fragPass pPass )
{

	pPass.bRenormalizeNormal = true;
	pPass.bForceRenormalizeNormal = true;
	pPass.bHemisphereLighting = true;
	pPass.bDisableInShadowShading = true;;
	pPass.bDeferredSpecularShading = true;

 	pPass.nReflectionMapping = REFLECT_CUBE;
// 	pPass.nReflectionMapping = REFLECT_SPHERE;  

	pPass.fFresnel_Pow = g_Fresnel;
	pPass.fFresnel_Bias = g_FresnelBia;
	pPass.fFresnel_Scale = g_FresnelScale;;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void frag_custom_begin( inout fragPass pPass )
{
	float4 baseTC = pPass.IN.baseTC;
	float4 bumpTC = pPass.IN.bumpTC;

	// load the diffuse texture
	pPass.cDiffuseMap = pow(tex2D(samDiffuse, baseTC.xy), 2.2f);

	// load the gloss
	pPass.cGlossMap = 1;
// #if %GLOSS_MAP
 	pPass.cGlossMap = pow(tex2D(samSpec, baseTC.xy), 2.2f);
// 
// #if %SPECULARPOW_GLOSSALPHA
 	//pPass.fSpecPow *= pPass.cGlossMap.a;
// #endif
// 
// #endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void frag_custom_per_light( inout fragPass pPass, inout fragLightPass pLight )
{
	pLight.fNdotL = saturate(pLight.fNdotL);

	float3 cDiffuse = pLight.cDiffuse.xyz * pLight.fNdotL;                                            // 1 alu

	float fSpec = SimpleBRDF( pPass.vNormal, pPass.vView, pLight.vLight, pPass.fSpecPow) * pLight.fNdotL;; // 7 alu

	// Final specular term
	float3 cSpecular = pLight.cSpecular.xyz * fSpec;                                                   // 1 alu

	float3 cK = pLight.fOcclShadow * pLight.fFallOff * pLight.cFilter;                                // 2 alu 

	pPass.cDiffuseAcc.xyz += cDiffuse.xyz * cK.xyz;                                                   // 1 alu
	pPass.cSpecularAcc.xyz += cSpecular.xyz * cK.xyz;                                                 // 1 alu

}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

// remove cAmbient, store in cAmbientAcc
void frag_custom_ambient( inout fragPass pPass, inout float3 cAmbient)
{    
	pPass.cAmbientAcc.xyz += cAmbient.xyz;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void frag_custom_end( inout fragPass pPass, inout float3 cFinal )
{

}




#include "ksPixelLib.fxh"


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------

vert2FragGeneralV GeneralVS( app2vertGeneral	IN	)
{	
	vert2FragGeneralV OUT = (vert2FragGeneralV)0; 
	vs_shared_output(IN, OUT, true);

	return OUT;
}

pixout GeneralPS(vert2FragGeneral IN)
{
	pixout OUT = (pixout)0;

	// Initialize fragPass structure
	fragPass pPass = (fragPass) 0;

	frag_unify(pPass, IN);

	float4 cFinal = frag_shared_output(pPass);

	HDROutput(OUT, cFinal, 1);

	return OUT;	
}

//--------------------------------------------------------------------------------------
// Techniques
//--------------------------------------------------------------------------------------
technique RenderScene_Tex_DirectionalLight
{
    pass P0
    {
        VertexShader = compile vs_3_0 GeneralVS();
		PixelShader = compile ps_3_0 GeneralPS();
		AlphaRef = 10;
		AlphaTestEnable = true;
		AlphaBlendEnable = true;
		CullMode = CW;
    }
}
