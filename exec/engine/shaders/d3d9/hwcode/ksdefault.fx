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

	float fSpecMultiplier;
	float2 vSurfaceRoughness;
};

struct fragLightPassCustom
{
	float fDummy;
}; 

#include "$InOutDefine.h"
#include "$BasePara.h"
#include "$Light_Calc.h"
#include "$ksVertexLib.h"

// custom Params Define

#include "$SamplerDef.h"

// custom extra Sampler define


void pipe_set_para_impl( inout fragPass pPass )
{
	pPass.bDeferredSpecularShading = true;

 	pPass.nReflectionMapping = REFLECT_CUBE;
// 	pPass.nReflectionMapping = REFLECT_SPHERE;  

	pPass.fFresnel_Pow = g_Fresnel;
	pPass.fFresnel_Bias = g_FresnelBia;
	pPass.fFresnel_Scale = g_FresnelScale;;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void pipe_start_impl( inout fragPass pPass )
{
	float4 baseTC = pPass.IN.baseTC;
	float4 bumpTC = pPass.IN.bumpTC;

	// load the diffuse texture
	pPass.cDiffuseMap = pow(tex2D(samDiffuse, baseTC.xy), 2.2f);

	// load the gloss
	pPass.cGlossMap = 1;
	float4 gloss = tex2D(samSpec, baseTC.xy);
 	pPass.cGlossMap = pow(gloss, 2.2f);

 	pPass.fSpecPow *= gloss.a;
	pPass.fFresnel_Bias *= saturate( gloss.a * 1.5 );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void pipe_per_light_impl( inout fragPass pPass, inout fragLightPass pLight )
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

void pipe_end_impl( inout fragPass pPass, inout float3 cFinal )
{

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
		AlphaRef = 10;
		AlphaTestEnable = false;
		AlphaBlendEnable = false;
		CullMode = CW;
    }
}
