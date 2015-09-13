//--------------------------------------------------------------------------------------
// 	File: ksPixelLightning.fx
//	Desc: The effect file for basic pixel lightning. 
//
//  The files of KnifeShader Lab.
// 	It could be used in 3dsMax, FxComposer & DirectX
//
// 	by Kaimingyi 2010.8.18
//--------------------------------------------------------------------------------------
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

}; 

#include "$InOutDefine.h"
#include "$BasePara.h"
#include "$Light_Calc.h"
#include "$ksVertexLib.h"

#include "$SamplerDef.h"

void pipe_set_para_impl( inout fragPass pPass )
{
	pPass.bDeferredSpecularShading = true;

#if SSRL
	pPass.bSSRL = true;
#endif

#if LIGHTMAP
	pPass.bLightmap = true;
#endif

#if ENVMAP
 	pPass.nReflectionMapping = REFLECT_CUBE;
#endif

#if DIFSPEC
	pPass.bDiffuseAlphaSpec = true;
#endif

#if SPCALPHA
	pPass.bSpecAlphaShiness = true;
#endif

#if DETAILNORMAL
	pPass.fDetailBumpScale = g_DetailAmount;
	pPass.vDetailBumpTilling = g_DetailTilling;
#endif



	pPass.fFresnel_Pow = g_Fresnel;
	pPass.fFresnel_Bias = g_FresnelBia;
	pPass.fFresnel_Scale = g_FresnelScale;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void pipe_start_impl( inout fragPass pPass )
{
	float4 baseTC = pPass.IN.baseTC;
	float4 bumpTC = pPass.IN.bumpTC; 

	pPass.cDiffuseMap = tex2D(samDiffuse, baseTC.xy);
	if (pPass.bDiffuseAlphaSpec)
	{
		pPass.cGlossMap = pPass.cDiffuseMap.a;
	}
	else
	{
		if (pPass.bSpecAlphaShiness)
		{
			float4 gloss = tex2D(samSpec, baseTC.xy);
			pPass.fSpecPow *= gloss.a;
			pPass.fFresnel_Bias *= saturate( gloss.a * 1.5 );

			pPass.cGlossMap = pow(gloss, 2.2f);
		}
		else
		{
			pPass.cGlossMap = tex2D(samSpec, baseTC.xy);
		}
	}
	
	pPass.cDiffuseMap = pow(pPass.cDiffuseMap, 2.2);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void pipe_per_light_impl( inout fragPass pPass, inout fragLightPass pLight )
{
	pLight.fNdotL = saturate(pLight.fNdotL);

	float3 cDiffuse = pLight.cDiffuse.xyz * pLight.fNdotL;

	float fSpec = SimpleBRDF( pPass.vNormal, pPass.vView, pLight.vLight, pPass.fSpecPow) * pLight.fNdotL;

	// Final specular term
	float3 cSpecular = pLight.cSpecular.xyz * fSpec;

	float3 cK = pLight.fOcclShadow * pLight.fFallOff * pLight.cFilter;

	if (pPass.bLightmap)
	{
		float4 lightmap_color = tex2D(samCustom1, pPass.IN.baseTC.zw);
	
		pPass.cDiffuseAcc.xyz += lightmap_color * lightmap_color;
		pPass.cSpecularAcc.xyz += cSpecular.xyz * cK.xyz;
	}
	else
	{
		pPass.cDiffuseAcc.xyz += cDiffuse.xyz * cK.xyz;
		pPass.cSpecularAcc.xyz += cSpecular.xyz * cK.xyz; 
	}

}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void pipe_end_impl( inout fragPass pPass, inout float3 cFinal )
{
}

#include "$ksPixelLib.h" 
 
//--------------------------------------------------------------------------------------
// Vertex Shader
//-------------- ------------------------------------------------------------------------
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
	
	HDROutput(OUT, cFinal * g_Glow, 1);

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
    }
}

#include "$kszpass.fx"
#include "$ksshadowpass.fx"
#include "$ksreflpass.fx"
#include "$ksfastcubegen.fx"