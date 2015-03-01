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
	float2 fGrad;
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

	pPass.fFresnel_Pow = g_Fresnel;
	pPass.fFresnel_Bias = g_FresnelBia;
	pPass.fFresnel_Scale = g_FresnelScale;

	float depth = length( pPass.IN.vView.xyz ) / g_fScreenSize.w;
	pPass.pCustom.fGrad = float2(depth, depth);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void pipe_start_impl( inout fragPass pPass )
{
	float4 baseTC = pPass.IN.baseTC;
	float4 detailTC = pPass.IN.terrainDetailTC;

	float4 tercolor = pow(tex2D(samDiffuse, baseTC.xy), 2.2f);

	float4 divide = tex2D(samCustom2, baseTC.xy);
	
	float4 dTCBase = float4( frac(detailTC).xy, 0,0);
	float4 detailcolorbase = pow(tex2Dgrad(samCustom1, float2(0.000f, 0.000f) + dTCBase * 0.499f, pPass.pCustom.fGrad, pPass.pCustom.fGrad), 2.2f);
	float4 detailcolor1 = pow(tex2Dgrad(samCustom1, float2(0.50f, 0.00f) + dTCBase * 0.499f, pPass.pCustom.fGrad, pPass.pCustom.fGrad), 2.2f);
	float4 detailcolor2 = pow(tex2Dgrad(samCustom1, float2(0.00f, 0.50f) + dTCBase * 0.499f, pPass.pCustom.fGrad, pPass.pCustom.fGrad), 2.2f);
	pPass.cDiffuseMap = detailcolorbase;
	pPass.cDiffuseMap = lerp(pPass.cDiffuseMap, detailcolor1, divide.x);
	pPass.cDiffuseMap = lerp(pPass.cDiffuseMap, detailcolor2, divide.y);

// 	float cGlossMap  = detail0.w;
// 	cGlossMap = lerp(cGlossMap, detail1.w, divide.x);
// 	cGlossMap = lerp(cGlossMap, detail2.w, divide.y);

	pPass.cGlossMap = pPass.cDiffuseMap.wwww;
	pPass.cDiffuseMap = tercolor * float4(GetLuminanceD( pPass.cDiffuseMap, 0.5), 1.0);
	//pPass.cGlossMap *= 1.0;
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
	vs_shared_output_detail(IN, OUT, true);
	return OUT;
}

pixout GeneralPS(vert2FragGeneral IN)
{
	pixout OUT = (pixout)0;

	// Initialize fragPass structure
	fragPass pPass = (fragPass) 0;

	pipe_init(pPass, IN);
	pPass.IN.terrainDetailTC = IN.detailTC;

	float4 cFinal = pipe_process(pPass);
	
	HDROutput(OUT, cFinal, 1);

	return OUT;	
}

vert2FragZpass ZPassVS( app2vertGeneral	IN	)
{	
	vert2FragZpass OUT = (vert2FragZpass)0; 
	vs_shared_output_zpass(IN, OUT, true);
	return OUT;
}

pixout_zpass ZPassPS(vert2FragZpass IN)
{
	pixout_zpass OUT = (pixout_zpass)0;

	float fDepth = saturate( IN.PosTex.x / g_fScreenSize.w );
	float2 grad = float2(fDepth, fDepth);

	// 放开手脚写独立的ZPASS处理！
	float3 normalTS = float3(0,0,1);

	// add detail normal
	float4 tcDetail = float4( frac(IN.baseTC.xy * 512), 0, 0);
	float4 divide = tex2D(samCustom2, IN.baseTC.xy);

	float3 detailcolorbase = NormalCalc(tex2Dgrad(samDetail, float2(0.00f, 0.00f) + tcDetail * 0.499f,grad,grad).xyz, 1.0);
	float3 detailcolor1 = NormalCalc(tex2Dgrad(samDetail, float2(0.50f, 0.00f) + tcDetail * 0.499f,grad,grad).xyz, 1.0);
	float3 detailcolor2 = NormalCalc(tex2Dgrad(samDetail, float2(0.00f, 0.50f) + tcDetail * 0.499f,grad,grad).xyz, 1.0);

	normalTS.xy += detailcolorbase.xy;
	normalTS.xy = lerp(normalTS.xy, detailcolor1.xy, divide.x);
	normalTS.xy = lerp(normalTS.xy, detailcolor2.xy, divide.y);

	normalTS = normalize(normalTS);

	float3 normalWS = cross(IN.TangentWS.xyz, IN.BinormalWS) * IN.TangentWS.w;
	float3x3 Tangent2World = float3x3(IN.TangentWS.xyz, IN.BinormalWS, normalWS);
	normalTS = mul(normalTS, Tangent2World);

	OUT.Normal = float4(normalize(normalTS) * 0.5 + 0.5, g_Gloss / 255.0 );

	OUT.Position = fDepth;
	

	return OUT;	
}

pixout_zpass_ds ZPass_DSPS(vert2FragZpass IN)
{
	pixout_zpass_ds OUT = (pixout_zpass_ds)0;

	float fDepth = saturate( IN.PosTex.x / g_fScreenSize.w );
	float2 grad = float2(fDepth, fDepth);

	// 放开手脚写独立的ZPASS处理！
	float3 normalTS = float3(0,0,1);

	// add detail normal
	float2 detailTC = IN.baseTC.xy * 512;
	float4 tcDetail = float4( frac(detailTC), 0, 0);
	float4 divide = tex2D(samCustom2, IN.baseTC.xy);

	float3 detailcolorbase = NormalCalc(tex2Dgrad(samDetail, float2(0.00f, 0.00f) + tcDetail * 0.499f,grad,grad).xyz, 1.0);
	float3 detailcolor1 = NormalCalc(tex2Dgrad(samDetail, float2(0.50f, 0.00f) + tcDetail * 0.499f,grad,grad).xyz, 1.0);
	float3 detailcolor2 = NormalCalc(tex2Dgrad(samDetail, float2(0.00f, 0.50f) + tcDetail * 0.499f,grad,grad).xyz, 1.0);

	normalTS.xy += detailcolorbase.xy;
	normalTS.xy = lerp(normalTS.xy, detailcolor1.xy, divide.x);
	normalTS.xy = lerp(normalTS.xy, detailcolor2.xy, divide.y);

	normalTS = normalize(normalTS);

	float3 normalWS = cross(IN.TangentWS.xyz, IN.BinormalWS) * IN.TangentWS.w;
	float3x3 Tangent2World = float3x3(IN.TangentWS.xyz, IN.BinormalWS, normalWS);
	normalTS = mul(normalTS, Tangent2World);

	OUT.Normal = float4(normalize(normalTS) * 0.5 + 0.5, g_Gloss / 255.0 );

	OUT.Position = fDepth;

	// terrian multilayer albeto
	//OUT.Albeto = pow(tex2D(samDiffuse, IN.baseTC.xy), 2.2f);

	float4 tercolor = pow(tex2D(samDiffuse, IN.baseTC.xy), 2.2f);


	float4 dTCBase = float4( frac(detailTC).xy, 0,0);
	float4 cDiffuseMap = pow(tex2Dgrad(samCustom1, float2(0.000f, 0.000f) + dTCBase * 0.499f, grad,grad), 2.2f);
	float4 cdetailcolor1 = pow(tex2Dgrad(samCustom1, float2(0.50f, 0.00f) + dTCBase * 0.499f, grad,grad), 2.2f);
	float4 cdetailcolor2 = pow(tex2Dgrad(samCustom1, float2(0.00f, 0.50f) + dTCBase * 0.499f, grad,grad), 2.2f);
	cDiffuseMap = lerp(cDiffuseMap, cdetailcolor1, divide.x);
	cDiffuseMap = lerp(cDiffuseMap, cdetailcolor2, divide.y);

	cDiffuseMap.xyz = tercolor.xyz * GetLuminanceD( cDiffuseMap.xyz, 0.5);

	OUT.Albeto = cDiffuseMap;

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

technique ZPass
{
	pass P0
	{
		VertexShader = compile vs_3_0 ZPassVS();
		PixelShader = compile ps_3_0 ZPassPS();
	}
}

technique ZPass_DS
{
	pass P0
	{
		VertexShader = compile vs_3_0 ZPassVS();
		PixelShader = compile ps_3_0 ZPass_DSPS();
	}
}

#include "$ksshadowpass.fx"
#include "$ksreflpass.fx"
