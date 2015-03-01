//-----------------------------------------------------------------------------
// File: ksHDRProcess.fx
//
// Desc: 
//
// Author: Yi Kaiming 2011/10/23
//-----------------------------------------------------------------------------

#include "./$postlib.h"

float4 HPosToScreenTC(float4 HPos)
{
	float4 ScrTC = HPos;
	ScrTC.xy = (HPos.xy * float2(1.0,-1.0) + HPos.ww) * 0.5;
	ScrTC.xy += g_fScreenSize.xy * HPos.ww;
	return ScrTC;
}

float4x4 g_mWorldViewProj 		: 		WORLDVIEWPROJ;
float4x4 g_mViewI   			: 		VIEWI;

float4 g_LightDiffuse = float4(0,1,0,1);
float4 g_LightSpecular = float4(0,1,0,1);
// skylight
float4 g_Ambient = float4(80 / 255.0, 120 / 255.0, 152 / 255.0, 1.0);
// groundlight
float4 g_cAmbGround = float4(30 / 255.0, 45 / 255.0, 57 / 255.0, 1.0);

shared float3 g_LightPos;
// x: minHeight: start
// y: maxHeight: end
// z: 1 / y
float4 g_vAmbHeightParams = float4(0.0f, 1.0f, 1 / 60.0f, 1.0f);

sampler2D g_samSceneNormal : register(s0) = sampler_state
{
	AddressU = Clamp;
	AddressV = Clamp;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = Point;
};
sampler2D g_samScenePosition : register(s1) = sampler_state
{
	AddressU = Clamp;
	AddressV = Clamp;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = Point;
};
sampler2D g_samShadowmask : register(s2) = sampler_state
{
	AddressU = Clamp;
	AddressV = Clamp;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
};
sampler2D g_samRandomStep : register(s2)  = sampler_state
{
	AddressU = WRAP;
	AddressV = WRAP;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = Point;
};

sampler2D g_samAO : register(s3)  = sampler_state
{
	AddressU = Clamp;
	AddressV = Clamp;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = Point;
};

float Light_Point_Diffuse(float3 NormDir,  // WorldSpace
	float3 LightDir // WorldSpace No Normal
	)
{
	// 计算夹角向量
	// Compute float vector
	LightDir = normalize(LightDir);
	NormDir = normalize(NormDir);

	// 计算漫反射部分
	// Diffuse
	float fLightDiffuse = saturate((dot(LightDir, NormDir) ));

	// 合成最终颜色
	// Compute final lighting
	return fLightDiffuse;
}

float Light_Point_Specular(float3 NormDir, 
	float3 LightDir,
	float3 EyeDir, 
	float SpecularPow
	)
{
	// 计算夹角向量
	// Compute float vector
	LightDir = normalize(LightDir);
	NormDir = normalize(NormDir);
	EyeDir = normalize(EyeDir);
	float3 floatVect = normalize(LightDir + EyeDir);

	// 计算高光部分
	// Specular
	float fLightSpecular =  pow( saturate(dot(NormDir, floatVect)),SpecularPow);

	// 计算漫反射部分
	// Diffuse
	float fLightDiffuse = saturate((dot(LightDir, NormDir) ));

	// 合成最终颜色
	// Compute final lighting
	return saturate(saturate(4 * fLightDiffuse) * fLightSpecular);
}

void DeferredSunPass( float2 Tex : TEXCOORD0,
	float4 FarClip : TEXCOORD1,
	out float4 oDif : COLOR0,
	out float4 oSpec : COLOR1)
{
	float4 rawNormal = tex2D(g_samSceneNormal, Tex);
	float3 vNorm = 2 * (rawNormal.rgb - 0.5);
	vNorm = normalize(vNorm);

	// simulate sky light, semi-sphere lighting
	float3 cAmbient = g_Ambient.rgb;
	cAmbient *= (vNorm.z*0.25f+0.75f); // 2 inst

	float3 vPositionWS = GetWorldPos(g_samScenePosition, Tex, FarClip.xyz);

	float3 cGroundAmbient = g_cAmbGround.rgb;
	cGroundAmbient *= (vNorm.z*0.25h+0.75h); // 2 inst
	oDif.rgb = lerp( cGroundAmbient , cAmbient , saturate( (vPositionWS.z - g_vAmbHeightParams.x) * g_vAmbHeightParams.z ) );
	//oDif.rgb *= 1.0;
	oDif.rgb *= tex2D(g_samAO, Tex).x;

	// plus the sun light
	float3 vLight = normalize( g_LightPos.xyz );
	float NdotL = saturate( dot(vLight, vNorm) );
	float shadowmask = tex2D(g_samShadowmask, Tex).x;


	oDif.rgb += g_LightDiffuse * NdotL * shadowmask;

	float3 vView = normalize(vPositionWS - g_camPos.xyz);
	float fNdotE = dot(-vView.xyz,vNorm.xyz);
	float fFresnel = 0.2f + 1.0f * pow(1.0f - fNdotE, 5.0f); // 4 inst

	// clear the spec target using MRT
	float spec = SimpleBRDF( vNorm, -vView, vLight, rawNormal.w * 255.0 ) * NdotL;
	oSpec = g_LightSpecular * spec * shadowmask * fFresnel;

	//oDif = cAmbient;
	oDif.a = 1;

}

void AmbientPass( float2 Tex : TEXCOORD0,
				  float4 FarClip : TEXCOORD1,
							out float4 oDif : COLOR0,
							out float4 oSpec : COLOR1)
{
	float3 vNorm = 2 * (tex2D(g_samSceneNormal, Tex).rgb - 0.5);

 	vNorm = normalize(vNorm);
 
 	// simulate sky light, semi-sphere lighting
 	float3 cAmbient = g_Ambient.rgb;
 	cAmbient *= (vNorm.z*0.25f+0.75f); // 2 inst

 	float3 vPositionWS = GetWorldPos(g_samScenePosition, Tex, FarClip.xyz);
 
 	float3 cGroundAmbient = g_cAmbGround.rgb;
 	cGroundAmbient *= (vNorm.z*0.25h+0.75h); // 2 inst
 	oDif.rgb = lerp( cGroundAmbient , cAmbient , saturate( (vPositionWS.z - g_vAmbHeightParams.x) * g_vAmbHeightParams.z ) );
 	//oDif.rgb *= 1.0;
 	oDif.rgb *= tex2D(g_samAO, Tex).x;
	// clear the spec target using MRT
	oSpec = 0;

	//oDif = cAmbient;
	oDif.a = 1;
	//OUT.Diffuse.rgb = cAmbient;

	//oDif.rgb = GetWorldPos(g_samScenePosition, Tex, FarClip.xyz);
}

struct vert2fragLight
{
	float4 HPosition	: POSITION;
	float4 screenTC		: TEXCOORD0;
};

struct pixout_light
{
	float4 difAcc : COLOR0;
	float4 spcAcc : COLOR1;
};

vert2fragLight VertexScene_LightPass( float4 inPos : POSITION )
{
	vert2fragLight OUT = (vert2fragLight)0;
    OUT.HPosition = mul( float4(inPos.xyz, 1), g_mWorldViewProj );
	OUT.screenTC = HPosToScreenTC(OUT.HPosition);

	return OUT;
}
 
pixout_light GeneralSingleLightPass( vert2fragLight IN)
{
	pixout_light OUT = (pixout_light)0;

	IN.screenTC.xyz /= IN.screenTC.w;
	float2 texcoord = IN.screenTC.xy - g_fScreenSize.xy * 0.5;

	float4 rawNormal = tex2D(g_samSceneNormal, texcoord);
	float3 vNorm = 2 * (rawNormal.rgb - 0.5);

	float3 vPositionWS = GetWorldPos(g_samScenePosition, texcoord, PS_lefttop.xyz + PS_ddx.xyz * texcoord.x + PS_ddy.xyz * texcoord.y);

	// FIXME: PASS LIGHTPOS HERE
	float3 lightVec = g_LightPos - vPositionWS.xyz;
	float att = saturate( max(0, g_LightDiffuse.w - abs(length(lightVec)) ) / g_LightDiffuse.w );

	OUT.difAcc.rgb = 2 * att * att * g_LightDiffuse.rgb * Light_Point_Diffuse(vNorm, lightVec);
	OUT.difAcc.a = 1;

	OUT.spcAcc.rgb = 2 * att * att * g_LightSpecular.rgb * SimpleBRDF(vNorm, normalize( g_camPos.xyz - vPositionWS.xyz) , normalize(lightVec), rawNormal.w * 255);
	OUT.spcAcc.a = 1;

	//OUT.difAcc.rgb = 1;
	//OUT.spcAcc = 0;

	return OUT;
}

pixout_light GeneralSingleLightGlobalShdowPass( vert2fragLight IN)
{
	pixout_light OUT = (pixout_light)0;

	IN.screenTC.xyz /= IN.screenTC.w;
	float2 texcoord = IN.screenTC.xy - g_fScreenSize.xy * 0.5;

	float4 rawNormal = tex2D(g_samSceneNormal, texcoord);
	float3 vNorm = 2 * (rawNormal.rgb - 0.5);

	float3 vPositionWS = GetWorldPos(g_samScenePosition, texcoord, PS_lefttop.xyz + PS_ddx.xyz * texcoord.x + PS_ddy.xyz * texcoord.y);

	// FIXME: PASS LIGHTPOS HERE
	float3 lightVec = g_LightPos - vPositionWS.xyz;
	float att = saturate( max(0, g_LightDiffuse.w - abs(length(lightVec)) ) / g_LightDiffuse.w );

	float globalShadowMask = tex2D(_tex3, texcoord);

	OUT.difAcc.rgb = 2 * att * att * g_LightDiffuse.rgb * Light_Point_Diffuse(vNorm, lightVec) * globalShadowMask;
	OUT.difAcc.a = 1;
	
	OUT.spcAcc.rgb = 2 * att * att * g_LightSpecular.rgb * SimpleBRDF(vNorm, normalize( g_camPos.xyz - vPositionWS.xyz) , normalize(lightVec), rawNormal.w * 255) * globalShadowMask;
	OUT.spcAcc.a = 1;

	//OUT.difAcc.rgb = 1;
	//OUT.spcAcc = 0;

	return OUT;
}

#define SAMPLE_NUM 16

float shadowLength = 0.08;
uniform bool bFakeShadow = false;

pixout_light GeneralSingleLightWithShadowPass( vert2fragLight IN )
{
	pixout_light OUT = (pixout_light)0;

	IN.screenTC.xyz /= IN.screenTC.w;
	float2 texcoord = IN.screenTC.xy - g_fScreenSize.xy * 0.5;

	float3 vNorm = 2 * (tex2D(g_samSceneNormal, texcoord).rgb - 0.5);

	float3 vPositionWS = GetWorldPosOffseting(g_samScenePosition, texcoord);

	// FIXME: PASS LIGHTPOS HERE
	float3 lightVec = g_LightPos - vPositionWS.xyz;
	float att = saturate( max(0, g_LightDiffuse.w - abs(length(lightVec)) ) / g_LightDiffuse.w );
	att = pow(att, 5);
	OUT.difAcc.rgb = 2 * att * g_LightDiffuse.rgb * Light_Point_Diffuse(vNorm, lightVec);
	OUT.difAcc.a = 1;

	OUT.spcAcc.rgb = 2 * att * g_LightSpecular.rgb * SimpleBRDF(vNorm, normalize(g_camPos.xyz - vPositionWS.xyz), lightVec, 50);
	OUT.spcAcc.a = 1;

	//if(bFakeShadow)
	{
		// point light fake shadow [1/7/2012 Kaiming]
		float stepoffset = tex2D(g_samRandomStep, texcoord * 1280);


		float sum = 1;
		for(int i=0; i < SAMPLE_NUM; i++)
		{
			float4 newPos = 1.0;
			newPos.xyz = vPositionWS.xyz + normalize(lightVec) * shadowLength * (i + 0.1 + stepoffset * 2.0);
			newPos.w = 1.0;
			reprojectPos(newPos);

			float4 screenPos = GetWorldPosOffseting(g_samScenePosition, newPos.xy);
			screenPos.w = 1.0;
			reprojectPos(screenPos);

			if(screenPos.z - newPos.z < 0 && screenPos.z - newPos.z > -0.0005 )
				sum *= lerp(0.45, 0.8, i / (float)SAMPLE_NUM);
		}

		//sum = saturate(pow(sum, 3.0) * 2.0);
		sum = saturate(sum);

		OUT.difAcc.rgb *= sum;
		OUT.spcAcc.rgb *= sum;
	}

	return OUT;
}

void LightStencilGen( out float4 oDif : COLOR0)
{
	//clip(-1);
	oDif = float4(0,0,0,1);
}

technique RenderLightPass
{
    pass p0
    {
        VertexShader = null;
        PixelShader = compile ps_3_0 AmbientPass();
        CullMode = none;
    }
}

technique RenderLightPassDS
{
	pass p0
	{
		VertexShader = null;
		PixelShader = compile ps_3_0 DeferredSunPass();
		CullMode = none;
	}
}



technique RenderLightStencilPass
{
	pass p0
	{
		VertexShader = compile vs_3_0 VertexScene_LightPass();
		PixelShader = compile ps_3_0 LightStencilGen();
	}
};

technique RenderLightGeneralPass
{
	pass p0
	{
		VertexShader = compile vs_3_0 VertexScene_LightPass();
		PixelShader = compile ps_3_0 GeneralSingleLightPass();
	}
};

technique RenderLightGeneralGlobalShadowPass
{
	pass p0
	{
		VertexShader = compile vs_3_0 VertexScene_LightPass();
		PixelShader = compile ps_3_0 GeneralSingleLightGlobalShdowPass();
	}
};

technique RenderLightFakeShadowPass
{
	pass p0
	{
		VertexShader = compile vs_3_0 VertexScene_LightPass();
		PixelShader = compile ps_3_0 GeneralSingleLightWithShadowPass();
	}
};
