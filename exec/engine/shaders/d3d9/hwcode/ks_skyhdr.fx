//--------------------------------------------------------------------------------------
// 	File: ksPixelLightning.fx
//	Desc: The effect file for basic pixel lightning. 
//
//  The files of KnifeShader Lab.
// 	It could be used in 3dsMax, FxComposer & DirectX
//
// 	by Kaimingyi 2010.8.18
//--------------------------------------------------------------------------------------


float Script : STANDARDSGLOBAL <
	string UIWidget = "none";
	string ScriptClass = "object";
	string ScriptLayer = "RENDERLAYER_SKY";
	string ScriptOutput = "color";
	string Script = "Technique=Default;";
> = 0.8; // version #


#include "$BasePara.h"
#include "$SamplerDef.h"
#include "$Light_Calc.h"
#include "$ksVertexLib.h"

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------

float g_CloudConstrat<
	string UIName = "g_CloudConstrat";
	string UIType = "FloatSpinner";
	float UIMin = 0.0f;
	float UIMax = 10.0f;	
> = 1.0f;

float g_CloudBright<
	string UIName = "CloudBright";
	string UIType = "FloatSpinner";
	float UIMin = 0.0f;
	float UIMax = 10.0f;	
> = 1.0f;

float g_ZenithHeight<
	string UIName = "ZenithHeight";
	string UIType = "FloatSpinner";
	float UIMin = -1.0f;
	float UIMax = 1.0f;	
> = 0.5f;

float4 g_SkyBottom;
float4 g_SkyTop;

void VertexScene( float4 inPos 		: POSITION,
				float4 inTex 		: TEXCOORD0,
				out float4 oPos		: POSITION,
                out float2 oTex0 	: TEXCOORD0,
				out float3 oNormal	: TEXCOORD1
				)
{	
    oPos = mul( float4(inPos.xyz, 1), g_mWorldViewProj );
	oTex0 = float2(inTex.x, inTex.y);
	oNormal = inPos.xyz;
}

pixout PixelScene(   
                float2 inTex0 	: TEXCOORD0    ,
				float3 inNormal 	: TEXCOORD1  
				)
{	
	pixout OUT = (pixout)0;

	float4 top = g_SkyTop;
	float4 bottom = g_SkyBottom;

 	float3 LUMINANCE_VECTOR  = float3 (0.2125f, 0.7154f, 0.0721f);
 	float LumBottom = dot(bottom, LUMINANCE_VECTOR);
	float LumTop = dot(top, LUMINANCE_VECTOR);

 	float zenith = inTex0.y;

 	zenith = saturate( zenith + 0.2);
	zenith = pow(zenith, 5.0);
 	float4 mulColor = (zenith * bottom + (1 - zenith) * top) * 0.5 * (LumTop + LumBottom);
	//mulColor = g_SkyBottom;
	HDROutput(OUT, mulColor, 0);


	// cubemap
//	float4 color = texCUBElod(IBLcubemapSampler, float4(normalize(inNormal.xzy), 0) );
	//color.a = 1;
//	OUT.Color = ( DecodeRGBK( color, 16.0f ) );

	//HDROutput( OUT, OUT.Color, 1.0);

	return OUT;	
}

float4 g_clipplane = float4(0,0,-1,-1);

vert2FragRefl ReflPassVS( app2vertGeneral	IN	)
{	
	vert2FragRefl OUT = (vert2FragRefl)0; 

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

	float4 top = g_SkyTop;
	float4 bottom = g_SkyBottom;

	float3 LUMINANCE_VECTOR  = float3 (0.2125f, 0.7154f, 0.0721f);
	float LumBottom = dot(bottom, LUMINANCE_VECTOR);
	float LumTop = dot(top, LUMINANCE_VECTOR);

	float zenith = IN.baseTC.y;

	zenith = saturate( zenith + 0.2);
	zenith = pow(zenith, 5.0);
	float4 mulColor = (zenith * bottom + (1 - zenith) * top) * 0.5 * (LumTop + LumBottom);
	//mulColor = g_SkyBottom;
	HDROutput(OUT, mulColor, 0);

	//
	// Depth is z / w
	//
	OUT.Color = pow( OUT.Color, 1.0 / 2.2f);

	return OUT;	
}

//-----------------------------------------------------------------------------
// Technique: ZPass
// Desc: Renders the shadow map
//-----------------------------------------------------------------------------
technique ReflGenPass
{
	pass p0
	{
		VertexShader = compile vs_3_0 ReflPassVS();
		PixelShader = compile ps_3_0 ReflPassPS();
	}
}

//--------------------------------------------------------------------------------------
// Techniques
//--------------------------------------------------------------------------------------
technique RenderScene
<
    int MatDoubleSide = 1;
>
{
    pass P0
    {
		ZWriteEnable = false;
        VertexShader = compile vs_3_0 VertexScene();
		PixelShader = compile ps_3_0 PixelScene();
		cullmode = CCW;
    }
}

#include "$kszpass.fx"
