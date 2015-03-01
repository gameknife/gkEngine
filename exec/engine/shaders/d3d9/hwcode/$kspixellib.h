//////////////////////////////////////////////////////////////////////////
//
// K&K Studio GameKnife ENGINE Source File
//
// Name:   	ksPixelLib.fx
// Desc:	像素处理，基类
// 
// Author:  Kaiming
// Date:	2011/11/4 
// Modify:	2011/11/4
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _ksPixelLib_h_
#define _ksPixelLib_h_

void pipe_init(inout fragPass pPass, in vert2FragGeneral IN)
{
	pPass.nQuality = GetShaderQuality();

	pPass.IN.baseTC = IN.baseTC;
	pPass.IN.bumpTC = pPass.IN.baseTC;


	pPass.IN.vTangent = float4(1,0,0,1);
	pPass.IN.vBinormal = float4(0,1,0,1);
	pPass.IN.vNormal.xyz = float4(0,0,1,1);

	IN.screenTC.xyz /= IN.screenTC.w;
	pPass.IN.screenProj = float4(IN.screenTC.xy - g_fScreenSize.xy * 0.5 ,0,0);
	pPass.IN.vView.xyz = g_camPos.xyz - IN.worldPos;

	pPass.bSSRL = false;

	pipe_set_para_impl( pPass );     

	pPass.fSpecPow = g_Gloss;	
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void pipe_compose( inout fragPass pPass, inout float3 cFinal )
{  
	float3 cDiffuse = ( pPass.cAmbientAcc.xyz + pPass.cDiffuseAcc.xyz ) * pPass.cDiffuseMap.xyz;  // 3 alu

	// here, now do not have the mat color [11/8/2011 Kaiming]
	cDiffuse.xyz *= g_MatDiffuse.xyz;

	if( pPass.nReflectionMapping > 0 || pPass.bSSRL )
	{
		// apply shading to environment map
		pPass.cEnvironment.xyz *= (1 + 1*( pPass.cAmbientAcc.xyz + pPass.cDiffuseAcc.xyz ));                 // 2 alu
		pPass.cSpecularAcc.xyz += pPass.cEnvironment.xyz;                                // 1 alu        
	}

	float3 cSpecular = pPass.cSpecularAcc.xyz * pPass.cGlossMap.xyz;                    // 2 alu

	cSpecular.xyz *= g_MatSpecular.xyz;

	cSpecular.xyz *= pPass.fFresnel;

	cFinal.xyz += cDiffuse;
	cFinal.xyz += cSpecular;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void pipe_quality_setting( inout fragPass pPass )
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
// 从deferred shading 的 light buffer中，采样光照
void pipe_get_light_buffer( inout fragPass pPass )
{
	pPass.cNormalMapRT = 2 * (tex2D( samSceneNormal, pPass.IN.screenProj.xy ) - 0.5);
	pPass.cDiffuseAccRT = tex2D( samLightDiffuseLayer, pPass.IN.screenProj.xy );

	if( pPass.bDeferredSpecularShading )
	{
		pPass.cSpecularAccRT = tex2D( samLightSpecLayer, pPass.IN.screenProj.xy );
	}

	pPass.cDiffuseAcc = pPass.cDiffuseAccRT;
	pPass.cSpecularAcc = pPass.cSpecularAccRT;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

float4 pipe_process(inout fragPass pPass)
{ 
	float4 cOut = 0;     

	pipe_quality_setting( pPass );
	pipe_get_light_buffer( pPass );

	float lt = tex2D(samShadowMask, pPass.IN.screenProj.xy + g_fScreenSize.xy * float2(0.0,0.0));
	float rb = tex2D(samShadowMask, pPass.IN.screenProj.xy + g_fScreenSize.xy * float2(2.0,2.0));

	pPass.cShadowOcclMap = min(lt, rb);

	pPass.vView = normalize(pPass.IN.vView.xyz);
	pPass.vNormal.xyz = normalize( pPass.cNormalMapRT.xyz );

	pPass.fNdotE = ( dot(pPass.vView.xyz, pPass.vNormal.xyz));
	pPass.vReflVec = (2.0h * pPass.fNdotE * pPass.vNormal.xyz) - pPass.vView.xyz;  

	// pass setup virtual implement
	pipe_start_impl(pPass);
	
	// view indep roughness
	//pPass.fSpecPow *= (1.0 + pow( saturate( 1.0 - dot(pPass.vView.xyz, -normalize(g_camDir)) ), 0.2 ) * 5.0 );
	//pPass.fSpecPow = 10.0f;
	// fresnel
	pPass.fFresnel = pPass.fFresnel_Bias + pPass.fFresnel_Scale * pow(1.0f - pPass.fNdotE, pPass.fFresnel_Pow); // 4 inst
	pPass.fSpecPow *= (1.0 + pow(1.0f - pPass.fNdotE, 2.5) * 5.0);
	// load the environment map
	if( pPass.nReflectionMapping > 0 )
	{
		if( pPass.nReflectionMapping == REFLECT_CUBE)
		{
			pPass.cEnvironment = GetEnvironmentCMap(IBLcubemapSampler, pPass.vReflVec.xzy, pPass.fSpecPow);   
		}
		else
			if( pPass.nReflectionMapping == REFLECT_SPHERE)
			{
				pPass.cEnvironment = GetEnvironment2DMap(IBLenvmapSampler, pPass.vReflVec.xy);      
			}
	}

	// SSRL
	if( pPass.bSSRL )
	{
		float fEdgeMipFix = saturate(1-pPass.fNdotE);
		fEdgeMipFix *= fEdgeMipFix;
		fEdgeMipFix *= fEdgeMipFix;
		float fGlossinessLod = 6.16231 - 0.497418 * sqrt(pPass.fSpecPow);
		fGlossinessLod = max(3.16231 - 6.16231*fEdgeMipFix,fGlossinessLod);
		pPass.cEnvironment += 0.2f * g_EnvironmentBia * tex2Dlod(samSceneColor,  float4(pPass.IN.screenProj.xy, 0, fGlossinessLod));

	}

	// Set default alpha
	pPass.fAlpha = pPass.cDiffuseMap.w;

	float fFallOff = 1;
	float fOcclShadow = pPass.cShadowOcclMap.x;

	float3 vLight, vLightWS;
	vLight = normalize( g_LightPos.xyz );
	vLightWS = vLight * 10000.0f;
	
	float3 filterColor = 1;

	// Compute diffuse
	float fNdotL = dot(vLight.xyz, pPass.vNormal.xyz);                   
	{ 
		fragLightPass pLight = (fragLightPass) 0;

		pLight.cDiffuse = g_LightDiffuse.rgb;       
		pLight.cSpecular = g_LightDiffuse.rgb;     

		pLight.vLight = vLight;

		pLight.fNdotL = fNdotL;
		pLight.fOcclShadow = fOcclShadow;

		pLight.fFallOff = fFallOff;
		pLight.cFilter = filterColor;

		pipe_per_light_impl(pPass, pLight);

		cOut.xyz += pLight.cOut.xyz;
	}

	if( pPass.bCustomComposition == false )
	{
		pipe_compose( pPass, cOut.xyz );
	}

	pipe_end_impl(pPass, cOut.xyz);

	cOut.w = g_Opacity;

	return cOut;
}

//============================================================================================

#endif