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

struct fragPassCustom
{
	// Custom per pass data     
	float3 vTangent;        // perturbed tangent vector
	float3 vBinormal;       // perturbed binormal vector

	float3 cRim;            // rim term  
	float fFresnel;         // fresnel term
	float fRimMultiplier;
};

struct fragLightPassCustom
{
	float fDummy;
}; 

#include "$InOutDefine.h"
#include "$BasePara.h"
#include "$Light_Calc.h"
#include "$ksVertexLib.h"
#include "$samplerdef.h"

float g_SkinScatter<
string UIName = "SkinScatter";
string UIType = "FloatSpinner";
float UIMin = 0.1f;
float UIMax = 10.0f;	
> = 2.2f;

float g_RimMultiplier<
string UIName = "RimMultiplier";
string UIType = "FloatSpinner";
float UIMin = 0.1f;
float UIMax = 50.0f;	
> = 3.0f;

float g_RimFresnel<
string UIName = "RimFresnel";
string UIType = "FloatSpinner";
float UIMin = 0.1f;
float UIMax = 10.0f;	
> = 5.0f;

void pipe_set_para_impl( inout fragPass pPass )
{
	pPass.bDeferredSpecularShading = true;

// 	pPass.nReflectionMapping = REFLECT_CUBE;
// 	pPass.nReflectionMapping = REFLECT_SPHERE;  

	pPass.fFresnel_Pow = g_Fresnel;
	pPass.fFresnel_Bias = g_FresnelBia;
	pPass.fFresnel_Scale = g_FresnelScale;;
	
	pPass.pCustom.fRimMultiplier = g_RimMultiplier;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void pipe_start_impl( inout fragPass pPass )
{
	float4 baseTC = pPass.IN.baseTC;
	float4 bumpTC = pPass.IN.bumpTC;

	// load the diffuse texture
	float4 tColor = tex2D(samDiffuse, baseTC.xy);
	pPass.cDiffuseMap = pow(tColor, 2.2f);

	// load the gloss
	pPass.cGlossMap = 1;
 	pPass.cGlossMap = tColor.a;

	  // Pre-computed rim term
	pPass.pCustom.fFresnel = GetFresnel( pPass.fNdotE, 0 , g_RimFresnel);
	pPass.pCustom.cRim =smoothstep_opt(0.5, pPass.pCustom.fFresnel) * pPass.pCustom.fRimMultiplier;   
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void pipe_per_light_impl( inout fragPass pPass, inout fragLightPass pLight )
{
	float4 DifPart;
	float4 SubPart;
	
	// HD Lambert
	nvLambSkin( pLight.fNdotL, 1, tex2D(samCustom1, pPass.IN.baseTC.xy), g_SkinScatter, DifPart, SubPart );
	float3 cDiffuse = pLight.cDiffuse.xyz * DifPart.xyz + SubPart.xyz;
	float fSpec = SimpleBRDF( pPass.vNormal, pPass.vView, pLight.vLight, pPass.fSpecPow) * pLight.fNdotL;

	// Final specular term
	float3 cSpecular = pLight.cSpecular.xyz * fSpec;                                                   // 1 alu

	float3 cK = pLight.fOcclShadow * pLight.fFallOff * pLight.cFilter;                                // 2 alu 

	pPass.cDiffuseAcc.xyz += cDiffuse.xyz * cK.xyz;                                                   // 1 alu
	pPass.cSpecularAcc.xyz += cSpecular.xyz * cK.xyz;                                                 // 1 alu

}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void pipe_end_impl( inout fragPass pPass, inout float3 cFinal )
{
	float3 cFinalRim = pPass.pCustom.cRim * pPass.cDiffuseAccRT.xyz;

	cFinalRim *= pPass.cGlossMap.xyz * g_MatSpecular.xyz;
	cFinal.xyz += cFinalRim;
}




#include "$ksPixelLib.h"


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------

vert2FragGeneral GeneralVS( app2vertGeneral	IN	)
{	
	vert2FragGeneral OUT = (vert2FragGeneral)0; 
	vs_shared_output(IN, OUT, true);

	return OUT;
}

pixout GeneralPS(vert2FragGeneral IN)
{
	pixout OUT = (pixout)0;

	// Initialize fragPass structure
	fragPass pPass = (fragPass) 0;

	pipe_init(pPass, IN);

	float4 cFinal = pipe_process(pPass);

	HDROutput(OUT, cFinal, 1);

	return OUT;	
}

//--------------------------------------------------------------------------------------
// Techniques
//--------------------------------------------------------------------------------------
technique RenderScene
{
    pass P0
    {
        VertexShader = compile vs_3_0 GeneralVS();
		PixelShader = compile ps_3_0 GeneralPS();
		AlphaRef = 0;
		AlphaTestEnable = false;
		AlphaBlendEnable = false;
    }
}

#include "$kszpass.fx"