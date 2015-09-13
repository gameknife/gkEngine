//-----------------------------------------------------------------------------
// File: $ksZpass.fx
//
// Desc: 渲染shadowmap的shader
//
// Author: Yi Kaiming 2010/12/29
//-----------------------------------------------------------------------------

#include "$BasePara.h"
#include "$SamplerDef.h"
#include "$Light_Calc.h"
#include "$ksVertexLib.h"

vert2FragZpass ZPassVS( app2vertGeneral	IN	)
{	
	vert2FragZpass OUT = (vert2FragZpass)0; 
	vs_shared_output_zpass(IN, OUT, true);
	return OUT;
}

pixout_zpass ZPassPS(vert2FragZpassV IN)
{
	pixout_zpass OUT = (pixout_zpass)0;

	float alpha = tex2D(samDiffuse, IN.baseTC.xy).a;
	GetDotAlpha(alpha, IN.ScreenPos.xy);
	clip(alpha - 0.9f);

	float fDepth = saturate( IN.PosTex.x / g_fScreenSize.w );
	float2 grad = float2(fDepth, fDepth);

	// 放开手脚写独立的ZPASS处理！
	float3 normalTS = NormalCalc(tex2D(samNormal, IN.baseTC.xy).xyz, 1.0f);
	normalTS = normalize(normalTS);

	float3 normalWS = cross(IN.TangentWS.xyz, IN.BinormalWS) * IN.TangentWS.w;
	float3x3 Tangent2World = float3x3(IN.TangentWS.xyz, IN.BinormalWS, normalWS);
	normalTS = mul(normalTS, Tangent2World);

	OUT.Normal = float4(normalize(normalTS) * 0.5 + 0.5, g_Gloss / 255.0 );
	OUT.Position = fDepth;
	
	float3 view = normalize( g_camPos.xyz - IN.WorldPos );
	float fNdotE = dot(view, OUT.Normal.xyz);
	OUT.Normal.w = g_Gloss / 255.0 * (1.0 + pow(1.0f - fNdotE, 2.5) * 2.0);
	return OUT;	
}

pixout_zpass_ds ZPass_DSPS(vert2FragZpass IN)
{
	pixout_zpass_ds OUT = (pixout_zpass_ds)0;

	float fDepth = saturate( IN.PosTex.x / g_fScreenSize.w );
	float2 grad = float2(fDepth, fDepth);

	// 放开手脚写独立的ZPASS处理！
	float3 normalTS = NormalCalc(tex2D(samNormal, IN.baseTC.xy).xyz, 1.0f);
	normalTS = normalize(normalTS);

	float3 normalWS = cross(IN.TangentWS.xyz, IN.BinormalWS) * IN.TangentWS.w;
	float3x3 Tangent2World = float3x3(IN.TangentWS.xyz, IN.BinormalWS, normalWS);
	normalTS = mul(normalTS, Tangent2World);

	OUT.Normal = float4(normalize(normalTS) * 0.5 + 0.5, g_Gloss / 255.0 );

	OUT.Position = fDepth;

	OUT.Albeto.xyz = pow(tex2D(samDiffuse, IN.baseTC.xy), 2.2f) * g_MatDiffuse;
	OUT.Albeto.w = GetLuminance( g_MatSpecular );

	return OUT;	
}
//-----------------------------------------------------------------------------
// Technique: ZPass
// Desc: Renders the shadow map
//-----------------------------------------------------------------------------
technique ZPass
{
    pass p0
    {
        VertexShader = compile vs_3_0 ZPassVS();
        PixelShader = compile ps_3_0 ZPassPS();

		AlphaBlendEnable = false;
    }
}

//////////////////////////////////////////////////////////////////////////
// DeferredShading
technique ZPass_DS
{
	pass p0
	{
		VertexShader = compile vs_3_0 ZPassVS();
		PixelShader = compile ps_3_0 ZPass_DSPS();

		AlphaBlendEnable = false;
	}
}
