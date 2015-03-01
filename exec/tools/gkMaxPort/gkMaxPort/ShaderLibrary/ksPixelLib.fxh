//////////////////////////////////////////////////////////////////////////
//
// K&K Studio GameKnife ENGINE Source File
//
// Name:   	ksPixelLib.fx
// Desc:		
// 
// Author:  Kaiming
// Date:	2011/11/4 
// Modify:	2011/11/4
// 
//////////////////////////////////////////////////////////////////////////

void frag_unify(inout fragPass pPass, in vert2FragGeneral IN)
{
  // Set quality level (LOW or HIGH)
  pPass.nQuality = GetShaderQuality();

  pPass.IN.baseTC = IN.baseTC;
  pPass.IN.bumpTC = pPass.IN.baseTC;
#ifdef _MAX_
  pPass.IN.vTangent = float4(IN.vTangent, 1);
  pPass.IN.vBinormal = float4(IN.vBinormal, 1);
  pPass.IN.vNormal.xyz = IN.vNormal;
#endif
  
  //pPass.IN.vView = IN.vView;
  pPass.IN.screenProj = float4(IN.vpos * g_fScreenSize, 0, 0);
#ifdef _MAX_
  pPass.IN.vView = float4( IN.vView ,1);
#else
  pPass.IN.vView = normalize(-(tex2D( samSceneDepth, pPass.IN.screenProj.xy )));
#endif

  frag_unify_parameters( pPass );     
  
  pPass.fSpecPow = g_Gloss;	
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void frag_final_composition( inout fragPass pPass, inout float3 cFinal )
{  
  float3 cDiffuse = ( pPass.cAmbientAcc.xyz + pPass.cDiffuseAcc.xyz ) * pPass.cDiffuseMap.xyz;  // 3 alu

  // here, now do not have the mat color [11/8/2011 Kaiming]
  cDiffuse.xyz *= g_MatDiffuse.xyz;

      
   if( pPass.nReflectionMapping )
   {
     // apply shading to environment map
     pPass.cEnvironment.xyz *= ( pPass.cAmbientAcc.xyz + pPass.cDiffuseAcc.xyz );                 // 2 alu
     pPass.cSpecularAcc.xyz += pPass.cEnvironment.xyz;                                // 1 alu        
   }
  
  float3 cSpecular = pPass.cSpecularAcc.xyz * pPass.cGlossMap.xyz;                    // 2 alu
  //if( pPass.nQuality != QUALITY_LOW && !pPass.bSkipMaterial)
  cSpecular.xyz *= g_MatSpecular.xyz;

  cSpecular.xyz *= pPass.fFresnel;
  
  cFinal.xyz += cDiffuse;
  cFinal.xyz += cSpecular;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void frag_quality_setup( inout fragPass pPass )
{
  // Set quality level (LOW, MEDIUM, HIGH or MAX)
//  pPass.nQuality = GetShaderQuality();

  // Check quality settings and disable stuff 

//   if( pPass.nQuality == QUALITY_LOW )
//   {
//     pPass.bDiffuseBump = false;
//     pPass.bOffsetBumpMapping = false;
//     pPass.bParallaxOcclusionMapping = false;
//     pPass.bRenormalizeNormal = pPass.bForceRenormalizeNormal;
//     pPass.nReflectionMapping = false;
//     pPass.bDetailBumpMapping = false;
//   }

  //if( pPass.nQuality == QUALITY_MEDIUM )
  //{
    pPass.bDiffuseBump = false;
    pPass.bParallaxOcclusionMapping = false;
    //pPass.bOffsetBumpMapping = false;        
  //}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
// Setup final fog. Per-vertex for alpha blended objects and per-object volumetric fog

void frag_fog_setup( inout fragPass pPass, inout float4 cOut)
{
// #if %_RT_FOG && (!%_RT_DECAL_TEXGEN_2D || !%_RT_DECAL_TEXGEN_3D)
//   ComputeGlobalFogPS(cOut.xyz, pPass.IN.vView.w);

//   cOut.xyz = pPass.IN.AvgFogVolumeContrib.xyz + cOut.xyz * pPass.IN.AvgFogVolumeContrib.w;
//   cOut.w *= pPass.IN.vView.w;
// #endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void frag_get_light_buffers( inout fragPass pPass )
{
  // Alpha blend processed as regular foward rendering
  //#if !%_RT_FOG
		// getnormal
	//pPass.cNormalMapRT = GetWorldSpaceNormals(tex2Dproj( samSceneNormal, pPass.IN.screenProj.xyzw), true);
#ifdef _MAX_	
	pPass.mTangentToWS = float3x3(pPass.IN.vTangent.xyz, pPass.IN.vBinormal.xyz, pPass.IN.vNormal.xyz);
	//tangent2world = transpose(tangent2world);

	// sampler form normal
	float3 NormalT = NormalCalc(tex2D(samNormal, pPass.IN.baseTC.xy).xyz, 1.0f);
	NormalT.xyz = NormalT.yxz;
	
	pPass.cNormalMapRT = float4( normalize(mul(NormalT, pPass.mTangentToWS)), 1);
#else
	pPass.cNormalMapRT = tex2D( samSceneNormal, pPass.IN.screenProj.xy );
#endif
    //#if %_RT_AMBIENT
    //pPass.cDiffuseAccRT = DecodeLightBuffer( tex2Dproj( samLightDiffuseLayer, pPass.IN.screenProj.xyzw) ); 
#ifdef _MAX_	
	pPass.cDiffuseAccRT = g_SkyLight;
#else
	pPass.cDiffuseAccRT = tex2D( samLightDiffuseLayer, pPass.IN.screenProj.xy );
#endif
      // Re-scale range
      //pPass.cDiffuseAccRT *= PS_HDR_RANGE_ADAPT_LBUFFER_MAX;

      //#if !%_RT_SHADER_LOD 
    if( pPass.bDeferredSpecularShading )
	{
          //pPass.cSpecularAccRT = DecodeLightBuffer( tex2Dproj( samLightSpecLayer, pPass.IN.screenProj.xyzw) ); 
		  pPass.cSpecularAccRT = tex2D( samLightSpecLayer, pPass.IN.screenProj.xy );
          // Re-scale range
          //pPass.cSpecularAccRT *= PS_HDR_RANGE_ADAPT_LBUFFER_MAX;
	}
      //#endif

    pPass.cDiffuseAcc = pPass.cDiffuseAccRT;
    pPass.cSpecularAcc = pPass.cSpecularAccRT;

	//pPass.cDiffuseAcc = 1;

    //#endif
  //#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void frag_hdr_setup(inout fragPass pPass, inout float4 cOut)
{
//   // Re-scale range
//   if( !pPass.bRefractionMap )
//     cOut.xyz *= PS_HDR_RANGE_ADAPT_MAX;
// 
// #if PS3
//    #if !%_RT_FOG && %_RT_HDR_MODE
// 
//     bool bMultipassAlphaBlend = false;
//     bool bMultipassAdditiveBlend = false;
//     #if %TEMP_TERRAIN || %DECAL
//       bMultipassAlphaBlend = true;
//     #endif
//     #if !%_RT_AMBIENT
//       bMultipassAdditiveBlend = true;
//     #endif
//     
//     if ( bMultipassAlphaBlend || bMultipassAdditiveBlend )
//     {
//       // Custom blending for PS3 (decode and blend)
//       float4 cDstRT = DecodeHDRBuffer( tex2Dproj( HDRTargetEncodedSampler, pPass.IN.screenProj.xyzw ) ); 
//       if( bMultipassAlphaBlend )
//         cOut = lerp( cDstRT, cOut, cOut.w );
//       if( bMultipassAdditiveBlend )
//         cOut += cDstRT;
//     }
// 
//     cOut = EncodeHDRBuffer( cOut );
// 
//   #endif
// #endif

}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void frag_ambient( inout fragPass pPass, float3 vNormal )
{

// #if %_RT_AMBIENT
// 	
// 	#if %_RT_FOG

// 		float3 amb = pPass.IN.Ambient.xyz;    
// 
// 		if( pPass.bHemisphereLighting )  
// 		{
// 			float fBlendFactor = (vNormal.z*0.25h+0.75h); // 1 inst
// 			amb.xyz *= fBlendFactor; // 1 inst
// 		}
// 
// 		// custom ambient pass
// 		frag_custom_ambient(pPass, amb);
// 
// 	#else

		// we still need to separately add emissive color in case of deferred ambient

	// implement later [11/8/2011 Kaiming]
// 		if(pPass.bDontUseEmissive == false)
// 			pPass.cAmbientAcc.xyz += MatEmissiveColor.xyz;

//	#endif

//#endif  // _RT_AMBIENT

}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

float4 frag_shared_output(inout fragPass pPass)
{ 
  float4 cOut = 0;     
  
  frag_quality_setup( pPass );
  frag_get_light_buffers( pPass );

#ifdef _MAX_
  pPass.cShadowOcclMap = 1;
#else
  pPass.cShadowOcclMap = tex2D(samShadowMask, pPass.IN.screenProj.xy);  
#endif
  

  pPass.vView = normalize(pPass.IN.vView.xyz);                                                           // 3 alu   
  pPass.vNormal.xyz = pPass.cNormalMapRT.xyz;


  // Store some constant coeficients
  pPass.fNdotE = ( dot(pPass.vView.xyz, pPass.vNormal.xyz));                                        // 1 alu
  
  // change [11/8/2011 Kaiming]
	//pPass.fSpecPow = MatSpecColor.w;	
  
  pPass.vReflVec = (2.0h * pPass.fNdotE * pPass.vNormal.xyz) - pPass.vView.xyz;                      // 2 alu  		    
        

  // do custom pass setup
  frag_custom_begin(pPass);
  
  // Set fresnel
  pPass.fFresnel = pPass.fFresnel_Bias + pPass.fFresnel_Scale * pow(1.0f - pPass.fNdotE, pPass.fFresnel_Pow); // 4 inst

   // load the environment map
  if( pPass.nReflectionMapping > 0 )
  {
	  if( pPass.nReflectionMapping == REFLECT_CUBE)
	  {
		  pPass.cEnvironment = GetEnvironmentCMap(IBLcubemapSampler, pPass.vReflVec.xyz, pPass.fSpecPow);    
	  }
	  else
		  if( pPass.nReflectionMapping == REFLECT_SPHERE)
		  {
			  // should be transformed to view space - but adds quite a lot instructions
			  pPass.cEnvironment = GetEnvironment2DMap(IBLenvmapSampler, pPass.vReflVec.xy);      
		  }
  }
  
  // Set default alpha
  pPass.fAlpha = pPass.cDiffuseMap.w;// * pPass.IN.Ambient.w;

  float3 vAmbientNormal = pPass.vNormal.xyz;

    
    //float fOcclShadow = saturate( 1 - dot(pPass.cShadowOcclMap, ShadowChanMask) );         // 1 alu
	float fOcclShadow = pPass.cShadowOcclMap.x;
    // disable for now - looks very wrong with SRGB active
    // make shadowed areas more interesting
    //float fOcclShadow = saturate( 1 - dot(pPass.cShadowOcclMap, ShadowChanMask) + 0.05);         // 1 alu
    //if( pPass.bDisableInShadowShading  )
    //  fOcclShadow = saturate( 1 - dot(pPass.cShadowOcclMap, ShadowChanMask));                    // 1 alu
        
    float fFallOff = 1;
    float3 vLight, vLightWS;        

    
    vLight = normalize( g_LightPos.xyz );
	vLightWS = vLight * 10000.0f;
	// per object light, here, deferred light, pass it

    // Get projector filter color if needed
    float3 filterColor = 1;
    
    // Compute diffuse
    float fNdotL = dot(vLight.xyz, pPass.vNormal.xyz);                                                   // 1 alu
  
//   #if %DYN_BRANCHING
//     if (fFallOff > 0 && fNdotL > 0) // -> some shading might require wrapped NdotL value
//   #endif                           
    { 
      // Initialize light pass structure with per-light shared data
      fragLightPass pLight = (fragLightPass) 0;
      
      //pLight.nType = nType;
                  
      pLight.cDiffuse = g_LightDiffuse.rgb;       
      pLight.cSpecular = g_LightSpecular.rgb;     

      pLight.vLight = vLight;
      
      pLight.fNdotL = fNdotL;
      pLight.fOcclShadow = fOcclShadow;
      
      pLight.fFallOff = fFallOff;
      pLight.cFilter = filterColor;
                                                                   
      // per shader custom shading 
      frag_custom_per_light(pPass, pLight);

      cOut.xyz += pLight.cOut.xyz;                                                                  // 1 alu
    }      
//   }
// #endif

  pPass.cAOMapVal = 1.f;

  // Get ambient term
  frag_ambient( pPass, vAmbientNormal );

  if( pPass.bCustomComposition == false )
  {
    frag_final_composition( pPass, cOut.xyz );
  }
  
  frag_custom_end(pPass, cOut.xyz);
  
//   if (pPass.bVertexColors)
//   {
//     cOut.xyz *= pPass.IN.Color.xyz;
//   }
  
// #if %_RT_AMBIENT
//   if (pPass.bAlphaGlow)
//   {
//     cOut.xyz += pPass.cDiffuseMap.w * pPass.cDiffuseMap.xyz * pPass.fAlphaGlow_Multiplier;
//   }
// #endif

  // distance blend out
  cOut.w = pPass.fAlpha;
	//cOut.w = 1;
  // Setup final fog
  frag_fog_setup( pPass, cOut);

  // Setup hdr
  frag_hdr_setup( pPass, cOut);

  return cOut;
}

//============================================================================================

