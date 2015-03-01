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

//#define GAUSSIAN

#include "$InOutDefine.h"
#include "$BasePara.h"
#include "$Light_Calc.h"
#include "$ksVertexLib.h"

float BigWavesScale<
	string UIName = "BigWaveScale";
string UIType = "FloatSpinner";
float UIMin = 0.0f;
float UIMax = 1.0f;	
> = 0.5f;

float SmallWavesScale<
	string UIName = "BigWaveScale";
string UIType = "FloatSpinner";
float UIMin = 0.0f;
float UIMax = 1.0f;	
> = 1.0f;

float4 g_WaterColor = float4(1.0/255.0,6.0/255.0,3.0/255.0,1);

#include "$SamplerDef.h"



struct vert2FragWater
{  
	float4 HPosition	: POSITION;
	float4 baseTC		: TEXCOORD0;
	float3 worldPos		: TEXCOORD1;
	float4 screenTC		: TEXCOORD2;

	float4 waveTC0     : TEXCOORD3;
	float4 waveTC1     : TEXCOORD4;
};

void vs_water_output( in app2vertGeneral IN, inout vert2FragWater OUT, bool bUseBump )
{
	// Common data
	OUT.HPosition = mul( float4(IN.vertCommon.Position.xyz,1), g_mWorldViewProj);
	OUT.baseTC = (float4(IN.vertCommon.Texcoord.x, IN.vertCommon.Texcoord.y, 0, 0 ) + float4(g_UVParam.zw,0,0)) * float4(g_UVParam.xy,1,1);
	OUT.screenTC = HPosToScreenTC(OUT.HPosition);
	OUT.worldPos = mul( float4(IN.vertCommon.Position.xyz,1), g_mWorld);

	float WavesSpeed = 0.02f;
	float2 vTranslation= float2(g_fTime * WavesSpeed * 5, 0);                       

	float2 vTranslation0=vTranslation;      
	float2 vTranslation1=vTranslation;

	float4 vTex = float4( OUT.baseTC.xy * 20, 0, 1 );                          

	OUT.waveTC0.xy = vTex.xy*4 + vTranslation0.xy*2;
	OUT.waveTC0.wz = vTex.xy*2.0 + vTranslation1.xy*3;
	OUT.waveTC1.xy = (vTex.xy*2.0*4 + vTranslation0.xy*2.0);
	OUT.waveTC1.wz = (vTex.xy*4.0*4 + vTranslation1.xy*3.0);

}



//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------

vert2FragWater GeneralVS( app2vertGeneral	IN	)
{	
	vert2FragWater OUT = (vert2FragWater)0; 
	vs_water_output(IN, OUT, true);
	return OUT;
}

pixout GeneralPS(vert2FragWater IN)
{
	pixout OUT = (pixout)0;

	IN.screenTC.xyz /= IN.screenTC.w;
	float4 screenProj = float4(IN.screenTC.xy - g_fScreenSize.xy * 0.5 ,0,0);

	
	
	float3 bumpNormal = float3(0,0,1);
	float3 bumpColorA = float3(0,0,1);
	float3 bumpColorB = float3(0,0,1);
	float3 bumpColorC = float3(0,0,1);
	float3 bumpColorD = float3(0,0,1);
	float3 bumpLowFreq = float3(0,0,1);
	float3 bumpHighFreq = float3(0,0,1);

	// merge big waves
	bumpColorA.xy = tex2D(samNormal, IN.waveTC0.xy).xy;           
	bumpColorB.xy = tex2D(samNormal, IN.waveTC0.wz).xy;           

	bumpLowFreq.xy = (bumpColorA + bumpColorB) * BigWavesScale - BigWavesScale;                       // 2 alu  

	// merge small waves
	bumpColorC.xy = tex2D(samNormal, IN.waveTC1.xy).xy;
	bumpColorD.xy = tex2D(samNormal, IN.waveTC1.wz).xy;

	bumpHighFreq.xy = (bumpColorC.xy + bumpColorD.xy) * SmallWavesScale - SmallWavesScale;            // 2 alu

	//float BumpScale = 0.8f;
	// merge all waves
	bumpNormal.xy = bumpLowFreq + bumpHighFreq;                                                     // 1 alu  
	bumpNormal.xy *= 1.0;                                                                     // 1 alu

	// just assume bump normal z is 1 and renormalize

	bumpNormal = normalize( bumpNormal );

	float3 sunNormal= bumpNormal.xyz;  

	// Clamp max bump to 0.15
	bumpNormal.xyz = sunNormal.xyz;
	bumpNormal.xy *= 0.15;

	sunNormal = normalize(sunNormal);
	//sunNormal = normalize(mul(sunNormal, g_mView));

	// Mirror reflection projection
	float3 reflectColor = 0;


// 	float4 ScenePos = float4(tex2D( samSceneDepth, IN. ).xyz, 1);
// 	float4 Viewdir = normalize(-ScenePos);
// 	ScenePos = mul(ScenePos, g_mViewI);

	float3 viewDir = normalize( IN.worldPos.xyz - g_camPos );
	viewDir = normalize(viewDir);
	float fres = fresnel( float3(0,0,1), -viewDir, 4.0, 0.0, 2.0  );

 	float sceneDepth = tex2D(samSceneDepth, screenProj).x * g_fScreenSize.w;
 	float waterDepth = IN.screenTC.w;
 	float depth = max( sceneDepth - waterDepth, 0 ) / dot( viewDir, g_camDir ); 	

	//depth += 0.00000002;
	depth = saturate(depth);
	//depth += 0.00000002;
	//float depth = 0.0 - IN.worldPos.z;


	float SoftIntersectionFactor = 0.5;

	//float FogDepth = pow( saturate( depth * 0.5 ), 1.0);// pow( saturate(depth * 0.1), 0.02);
	depth *= 2;
	//depth = saturate(depth);
	float FogDepth = 1.f - ( 1.f - exp( -depth ) ) / (depth + 0.1);

	FogDepth = 1.0 - 1.0 / exp( depth );

	//FogDepth = 0;
#ifdef GAUSSIAN
	for (int i = 0; i < g_cKernelSize; i++)
	{    
		reflectColor += tex2D( samReflect, screenProj.xy + PixelKernel[i].xy * g_fScreenSize.y * 4 ).rgb * BlurWeights[i];
	}
#else
	reflectColor = tex2D(samReflect, screenProj.xy + bumpNormal.xy).rgb;
#endif

	float3 refractColor = tex2D(samSceneColor, screenProj.xy + bumpNormal.xy * 0.1).xyz;

	// add the waterfog to refract
	refractColor = refractColor * (1 - FogDepth) + g_WaterColor * FogDepth;

	// Compute final water color             
	OUT.Color.rgb = lerp(refractColor, 0.5f * reflectColor, 0.2 * fres) ;
	//OUT.Color.rgb = reflectColor;
	OUT.Color.a = 1.0;
	//oColor.rgb += 0.4 * fres * reflColor;
	// add the spec part
	//oColor.rgb += tex2D(samLightSpecLayer, Tex).rgb * g_Specular;


	float3 vLight = g_LightPos;//normalize( mul( g_LightPos, g_mView).xyz);
	vLight = normalize(vLight);
	// Sun light
 	float sunSpecular = SimpleBRDF( sunNormal, -viewDir.xyz, vLight, 450.0f );                        
 	float3 vSunGlow = sunSpecular * g_LightSpecular.rgb * 0.02f;
// 
// 	// Add sun specular term
 	OUT.Color.xyz += vSunGlow.xyz;    

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
