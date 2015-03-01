//////////////////////////////////////////////////////////////////////////
//
// K&K Studio GameKnife ENGINE Source File
//
// Name:   	VolumeFog.fxh
// Desc:	Fog include
// 
// Author:  Kaiming
// Date:	2011/11/25 
// Modify:	2011/11/25
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _VolumeFog_h_
#define _VolumeFog_h_

float4 vfParams = float4(0.05,0.0004,0.0004,0.01);
float4 vfRampParams = float4(1,0.1,1,0);

float3 vfColGradBase = float3(0.5,0.75,1.5);		//: PB_FogColGradColBase;
//float3 vfColGradDelta = float3(0.1,0.15,0.3) * 0;		//: PB_FogColGradColBase;
float3 vfColGradDelta = float3(0,0,0);		//: PB_FogColGradColDelta;

float4
ComputeVolumetricFog( in float3 cameraToWorldPos )
{
#define atmosphereScale								vfParams.x
#define volFogHeightDensityAtViewer 	float(vfParams.y)
#define fogDensity										float(vfParams.z)
#define artistTweakDensityOffset			float(vfParams.w)

	float fogInt = 1.h;

   	static const float c_slopeThreshold = 0.01f;
   	if( abs( cameraToWorldPos.z ) > c_slopeThreshold )
   	{
   		float t = atmosphereScale * cameraToWorldPos.z;
   		fogInt *= ( 1.f - exp( -t ) ) / t;
   	}

	// NOTE: volFogHeightDensityAtViewer = log2(e) * fogDensity * exp( -atmosphereScale * ( vfViewPos.z - waterLevel ) );
	float l = length( cameraToWorldPos );
	float u = l * volFogHeightDensityAtViewer;
	fogInt = fogInt * u - artistTweakDensityOffset;

	float f = saturate( exp2( -fogInt ) );
	//return f;

// 	float r = saturate(l * vfRampParams.x + vfRampParams.y);
// 	r = r * (2-r);
// 	//r = smoothstep(0, 1, r);
// 	r = r * vfRampParams.z + vfRampParams.w;
// 	f = (1-f) * r;

	return (f);
}

float4 
GetVolumetricFogColorDistanceBased( in float3 worldPos, in float3 cameraToWorldPos )
{
	float fog = ComputeVolumetricFog(cameraToWorldPos);
	float l = saturate(normalize(cameraToWorldPos.xyz).z);
	float3 fogColor = vfColGradBase + l * vfColGradDelta;
	return float4(fogColor, fog);
}

float4
GetVolumetricFogColor( in float3 worldPos, in float3 cameraToWorldPos, in float sceneDepth )
{
// #if %_RT_SKYLIGHT_BASED_FOG
// 	return GetVolumetricFogColorSkyLightBased( worldPos, cameraToWorldPos, sceneDepth );
// #else
	return GetVolumetricFogColorDistanceBased( worldPos, cameraToWorldPos );
//#endif
}

#endif
