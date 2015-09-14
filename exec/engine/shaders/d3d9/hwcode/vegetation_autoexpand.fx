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

#define EXPAND_WIDTH 4.0

struct fragLightPassCustom
{

}; 

#include "$InOutDefine.h"
#include "$BasePara.h"
#include "$Light_Calc.h"
#include "$ksVertexLib.h"

float g_ExpandWidth<
	string UIName = "Expand Width";
string UIType = "FloatSpinner";
float UIMin = 0.0f;
float UIMax = 10.0f;
> = 4.0f;

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

	float3 cDiffuse = pLight.cDiffuse.xyz * pLight.fNdotL;                                            // 1 alu
	cDiffuse *= (1.0 - pPass.IN.baseTC.y);

	float fSpec = SimpleBRDF( pPass.vNormal, pPass.vView, pLight.vLight, pPass.fSpecPow) * pLight.fNdotL;; // 7 alu

	// Final specular term
	float3 cSpecular = pLight.cSpecular.xyz * fSpec;                                                   // 1 alu

	float3 cK =  (pLight.fOcclShadow * 0.5 + 0.5) * pLight.fFallOff * pLight.cFilter;                                // 2 alu 

	pPass.cDiffuseAcc.xyz += cDiffuse.xyz * cK.xyz;                                                   // 1 alu
	pPass.cSpecularAcc.xyz += cSpecular.xyz * cK.xyz;                                                 // 1 alu

}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void pipe_end_impl( inout fragPass pPass, inout float3 cFinal )
{

}


#define WAVE_SPEED 1.0
#define BENT_AMOUNT 2000.0
#define DELAY_DIST 0.1

#include "$ksPixelLib.h"


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------

vert2FragGeneral GeneralVS( app2vertGeneral	IN	)
{	
	vert2FragGeneral OUT = (vert2FragGeneral)0; 

	float3 frontDir = mul(g_camDir, (float3x3)g_mWorldI);
	float3 rightDir = normalize( cross( float3(0,0,1), frontDir ) );
	float3 upDir = normalize( cross(frontDir, rightDir) );

	IN.vertCommon.Position.xyz += (IN.vertCommon.Texcoord.x - 0.5) * rightDir * g_ExpandWidth;
	IN.vertCommon.Position.xyz -= (IN.vertCommon.Texcoord.y - 0.5) * upDir * g_ExpandWidth;
	IN.vertCommon.Position.xyz += float3(0.2f * sin(g_fTime * WAVE_SPEED + (IN.vertCommon.Position.y + IN.vertCommon.Position.x)* DELAY_DIST) * (pow(1 - IN.vertCommon.Texcoord.x, BENT_AMOUNT) + pow(1 - IN.vertCommon.Texcoord.y, BENT_AMOUNT)), 0,0);

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

vert2FragZpass ZPassVS( app2vertGeneral	IN	)
{	
	vert2FragZpass OUT = (vert2FragZpass)0; 

	float3 frontDir = mul(g_camDir, (float3x3)g_mWorldI);
	float3 rightDir = normalize( cross( float3(0,0,1), frontDir ) );
	float3 upDir = normalize( cross(frontDir, rightDir) );

	IN.vertCommon.Position.xyz += (IN.vertCommon.Texcoord.x - 0.5) * rightDir * g_ExpandWidth;
	IN.vertCommon.Position.xyz -= (IN.vertCommon.Texcoord.y - 0.5) * upDir * g_ExpandWidth;
	IN.vertCommon.Position.xyz += float3(0.2f * sin(g_fTime * WAVE_SPEED + (IN.vertCommon.Position.y + IN.vertCommon.Position.x)* DELAY_DIST) * (pow(1 - IN.vertCommon.Texcoord.x, BENT_AMOUNT) + pow(1 - IN.vertCommon.Texcoord.y, BENT_AMOUNT)), 0,0);

	IN.vertCommon.Tangent.xyz = rightDir;
	IN.vertCommon.Binormal.xyz = -upDir;

	vs_shared_output_zpass(IN, OUT, true);
	return OUT;
}

pixout_zpass ZPassPS(vert2FragZpassV IN)
{
	pixout_zpass OUT = (pixout_zpass)0;

	float alpha = tex2D(samDiffuse, IN.baseTC.xy).a;
	GetDotAlpha(alpha, IN.ScreenPos.xy);
	clip(alpha - 0.9f);

	// 放开手脚写独立的ZPASS处理！
	float3 normalTS = float3(0,0,1);

	normalTS = normalize(normalTS);

	float3 normalWS = cross(IN.TangentWS.xyz, IN.BinormalWS) * IN.TangentWS.w;
	float3x3 Tangent2World = float3x3(IN.TangentWS.xyz, IN.BinormalWS, normalWS);
	normalTS = mul(normalTS, Tangent2World);

	OUT.Normal = float4(normalize(normalTS) * 0.5 + 0.5, g_Gloss );

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
	// 	else if (alpha < 0.6f)
	// 	{
	// 		alpha *= ( (mask2 + mask1) * 100);
	// 	}
	else
	{
		alpha = 1;
	}

	clip(alpha - 0.9f);

	// 放开手脚写独立的ZPASS处理！
	float3 normalTS = float3(0,0,1);

	normalTS = normalize(normalTS);

	float3 normalWS = cross(IN.TangentWS.xyz, IN.BinormalWS) * IN.TangentWS.w;
	float3x3 Tangent2World = float3x3(IN.TangentWS.xyz, IN.BinormalWS, normalWS);
	normalTS = mul(normalTS, Tangent2World);

	OUT.Normal = float4(normalize(normalTS) * 0.5 + 0.5, g_Gloss );

	OUT.Position = saturate( IN.PosTex.x / g_fScreenSize.w );


	return OUT;	
}

vert2FragShadow ShadowPassVS( app2vertGeneral	IN	)
{	
	vert2FragShadow OUT = (vert2FragShadow)0; 

	float3 frontDir = mul(g_camDir, (float3x3)g_mWorldI);
	float3 rightDir = normalize( cross( float3(0,0,1), frontDir ) );
	float3 upDir = normalize( cross(frontDir, rightDir) );

	IN.vertCommon.Position.xyz += (IN.vertCommon.Texcoord.x - 0.5) * rightDir * g_ExpandWidth;
	IN.vertCommon.Position.xyz -= (IN.vertCommon.Texcoord.y - 0.5) * upDir * g_ExpandWidth;
	IN.vertCommon.Position.xyz += float3(0.2f * sin(g_fTime * WAVE_SPEED + (IN.vertCommon.Position.y + IN.vertCommon.Position.x)* DELAY_DIST) * (pow(1 - IN.vertCommon.Texcoord.x, BENT_AMOUNT) + pow(1 - IN.vertCommon.Texcoord.y, BENT_AMOUNT)), 0,0);

	//
	// Compute the projected coordinates
	//
	OUT.HPosition = mul( float4(IN.vertCommon.Position.xyz, 1), g_mWorldViewProj );
	OUT.baseTC = IN.vertCommon.Texcoord;
	
	return OUT;
}


pixout ShadowPassPS(vert2FragShadow IN)
{
	pixout OUT = (pixout)0;

	clip( tex2D(samDiffuse, IN.baseTC.xy).a - 0.45f);
	//
	// Depth is z / w
	//
	OUT.Color = 0;

	return OUT;	
}

float4 g_clipplane = float4(0,0,-1,-1);

vert2FragRefl ReflPassVS( app2vertGeneral	IN	)
{	
	vert2FragRefl OUT = (vert2FragRefl)0; 


	float3 reflcamdir = g_camDir;
	reflcamdir.z = -reflcamdir.z;

	float3 frontDir = mul(reflcamdir, (float3x3)g_mWorldI);
	float3 rightDir = normalize( cross( float3(0,0,1), frontDir ) );
	float3 upDir = normalize( cross(frontDir, rightDir) );

	IN.vertCommon.Position.xyz += (IN.vertCommon.Texcoord.x - 0.5) * rightDir * g_ExpandWidth;
	IN.vertCommon.Position.xyz -= (IN.vertCommon.Texcoord.y - 0.5) * upDir * g_ExpandWidth;
	IN.vertCommon.Position.xyz += float3(0.2f * sin(g_fTime * WAVE_SPEED + (IN.vertCommon.Position.y + IN.vertCommon.Position.x)* DELAY_DIST) * (pow(1 - IN.vertCommon.Texcoord.x, BENT_AMOUNT) + pow(1 - IN.vertCommon.Texcoord.y, BENT_AMOUNT)), 0,0);

	float4 oPos = mul( float4(IN.vertCommon.Position.xyz, 1.0), g_mWorld );
	oPos.z = 2.0 * g_clipplane.w - oPos.z;
	OUT.clip = dot(float4(oPos.xyz,1.0), g_clipplane);

	oPos = mul( oPos, g_mView );
	oPos = mul( oPos, g_mProjection );

	OUT.HPosition = oPos;
	OUT.baseTC = float4(IN.vertCommon.Texcoord.xy, 0, 0);



	return OUT;
}

pixout ReflPassPS(vert2FragRefl IN)
{
	pixout OUT = (pixout)0;

	if(IN.clip < 0)
		discard;

	float4 color = tex2D(samDiffuse, IN.baseTC.xy);
#if DIFSPEC

#else
	clip(color.a - 0.3);
#endif

	OUT.Color = pow( color, 2.2f);

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

technique ReflGenPass
{
	pass p0
	{
		VertexShader = compile vs_3_0 ReflPassVS();
		PixelShader = compile ps_3_0 ReflPassPS();
	}
}
