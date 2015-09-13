//--------------------------------------------------------------------------------------
// 	File: ksPixelLightning.fx
//	Desc: The effect file for basic pixel lightning. 
//
//  The files of KnifeShader Lab.
// 	It could be used in 3dsMax, FxComposer & DirectX
//
// 	by Kaimingyi 2010.8.18
//--------------------------------------------------------------------------------------


#define WAVE_SPEED 1.0
#define BENT_AMOUNT 2000.0
#define DELAY_DIST 0.1

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

	pPass.fFresnel_Pow = g_Fresnel;
	pPass.fFresnel_Bias = g_FresnelBia;
	pPass.fFresnel_Scale = g_FresnelScale;;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void pipe_start_impl( inout fragPass pPass )
{
	float4 baseTC = pPass.IN.baseTC;
	pPass.cDiffuseMap = pow(tex2D(samDiffuse, baseTC.xy), 2.2f);
	pPass.cGlossMap = 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void pipe_per_light_impl( inout fragPass pPass, inout fragLightPass pLight )
{
	pLight.fNdotL = saturate(pLight.fNdotL);
	pLight.fNdotL = pLight.fNdotL * 0.5 + 0.5;

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
	
	//IN.vertCommon.Position.xyz += float3(0.2f * sin(g_fTime * WAVE_SPEED + (IN.vertCommon.Position.y + IN.vertCommon.Position.x)* DELAY_DIST) * pow(1 - IN.vertCommon.Texcoord.y, BENT_AMOUNT), 0,0);
	vs_shared_output_zpass_autoflip(IN, OUT, true);
	return OUT;
}

pixout_zpass ZPassPS(vert2FragZpassV IN)
{
	pixout_zpass OUT = (pixout_zpass)0;

	float alpha = tex2D(samDiffuse, IN.baseTC.xy).a;
	GetDotAlpha(alpha, IN.ScreenPos.xy);
	clip(alpha - 0.9f);


	float3 normalTS = float3(0,0,1);
	normalTS = normalize(normalTS);

	float3 normalWS = cross(IN.TangentWS.xyz, IN.BinormalWS) * IN.TangentWS.w;
	float3x3 Tangent2World = float3x3(IN.TangentWS.xyz, IN.BinormalWS, normalWS);
	normalTS = mul(normalTS, Tangent2World);

	// change if opposite with cam
	normalWS = normalize(normalTS);

	OUT.Normal = float4(normalWS * 0.5 + 0.5, g_Gloss );

	OUT.Position = saturate( IN.PosTex.x / g_fScreenSize.w );
	

	return OUT;	
}


pixout_zpass_ds ZPass_DSPS(vert2FragZpassV IN)
{
	pixout_zpass_ds OUT = (pixout_zpass_ds)0;

	OUT.Albeto = tex2D(samDiffuse, IN.baseTC.xy);
	OUT.Albeto.xyz = pow(OUT.Albeto.xyz, 2.2f);
	OUT.Albeto.xyz *= g_MatDiffuse;
	
	float alpha = OUT.Albeto.a;
	OUT.Albeto.w = 0;

	float2 screenCoord = float2( IN.ScreenPos.x % 2, IN.ScreenPos.y % 2);
	screenCoord = screenCoord * 0.5 + 0.25;

	float mask1 = 1.f - tex2D(samBlend25, screenCoord).r;
	float mask2 = 1.f - tex2D(samBlend50, screenCoord).r;
// 
	if (alpha < 0.1f)
	{
		alpha = 0;
	}
	else if (alpha < 0.3f)
	{
		alpha *= (mask1 * 100);
	}
	else if (alpha < 0.5f)
	{
		alpha *= ( mask2 * 100);
	}
	else
	{
		alpha = 1;
	}

	clip(alpha - 0.9f);

	float3 normalTS = float3(0,0,1);

	normalTS = normalize(normalTS);

	float3 normalWS = cross(IN.TangentWS.xyz, IN.BinormalWS) * IN.TangentWS.w;
	float3x3 Tangent2World = float3x3(IN.TangentWS.xyz, IN.BinormalWS, normalWS);
	normalTS = mul(normalTS, Tangent2World);

	// change if opposite with cam
	normalWS = normalize(normalTS);

	OUT.Normal = float4(normalWS * 0.5 + 0.5, g_Gloss );

	OUT.Position = saturate( IN.PosTex.x / g_fScreenSize.w );

	

	return OUT;	
}

vert2FragShadow ShadowPassVS( app2vertGeneral	IN	)
{	
	vert2FragShadow OUT = (vert2FragShadow)0; 

	OUT.HPosition = mul( float4(IN.vertCommon.Position.xyz, 1), g_mWorldViewProj );
	OUT.baseTC = IN.vertCommon.Texcoord;

	return OUT;
}


pixout ShadowPassPS(vert2FragShadow IN)
{
	pixout OUT = (pixout)0;

	clip( tex2D(samDiffuse, IN.baseTC.xy).a - 0.35f);
	//
	// Depth is z / w
	//
	OUT.Color = 0;

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
		CullMode = None;
		PixelShader = compile ps_3_0 GeneralPS();
    }
}

technique ZPass
{
	pass P0
	{
		VertexShader = compile vs_3_0 ZPassVS();
		CullMode = None;
		PixelShader = compile ps_3_0 ZPassPS();
	}
}

technique ZPass_DS
{
	pass P0
	{
		VertexShader = compile vs_3_0 ZPassVS();
		CullMode = None;
		PixelShader = compile ps_3_0 ZPass_DSPS();
	}
}

technique ShadowPass
{
	pass P0
	{
		VertexShader = compile vs_3_0 ShadowPassVS();
		CullMode = None;
		PixelShader = compile ps_3_0 ShadowPassPS();
	}
}

#include "$ksreflpass.fx"