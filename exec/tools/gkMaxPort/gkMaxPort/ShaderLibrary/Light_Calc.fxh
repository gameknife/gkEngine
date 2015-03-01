#define HDR_OVERBRIGHT 8
#define HDR_FAKE_MAXOVERBRIGHT 16
#define HDR_EXP_OFFSET 128
#define HDR_EXP_BASE   1.06

//////////////////////////////// Phong model /////////////////////////////////////
// - Phong model has good properties for plastic and some metallic surfaces. 
// - Good for general use. Very cheap.

// Optimized phong, use if mirrowed reflection vector pre-computed
float Phong(float3 R, float3 L, float Exp)
{	
	float fNormFactor = Exp * 0.159155f + 0.31831f;			// 1 ALU
	return fNormFactor *  pow(saturate(dot(L, R)), Exp);					// 4 ALU
	// 5 ALU
}

float Phong(float3 N, float3 V, float3 L, float Exp)
{
	float3 R = reflect(-V, N);	// 3 ALU
	return Phong(R, L, Exp);	// 5 ALU
	// 8 ALU
}

//////////////////////////////// Blinn model /////////////////////////////////////
// - Blinn model has good properties for plastic and some metallic surfaces. 
// - Good for general use. Very cheap.
// *NOTE* We should also multiply by the clamped N.L factor. However this is 
// delegated to the shader part for performance reasons
float Blinn(float3 N, float3 V, float3 L, float Exp)
{
	float fNormFactor = Exp * 0.159155h + 0.31831h;					// 1 ALU
	float3 H = normalize(V + L);																			// 2 ALU
	return fNormFactor * pow(saturate(dot(N, H)), Exp);							// 4 ALU
	// 7 ALU
}

//////////////////////////////// Simple BRDF ////////////////////////////////
// Note that this function is used for evey computation of the simple Phong-like BRDF model 

float SimpleBRDF(float3 N, float3 V, float3 L, float Exp)
{
	return Blinn(N, V, L, Exp);
	//return Phong(N, V, L, Exp);
}

float3 NormalCalc(float3 mapNorm, float BumpScale)
{
	float3 v = {0.5f, 0.5f, 1.0f};
	mapNorm = lerp(v, mapNorm, BumpScale );
	mapNorm = ( mapNorm * 2.0f ) - 1.0f;
	return mapNorm;
}

float4 HDRScale(float4 c)
{
    return float4(
                    c.r * g_HDRScalar,
                    c.g * g_HDRScalar,
                    c.b * g_HDRScalar,
                    c.a
                );
}

void HDROutput( out pixout OUT, float4 Color, float fDepth)
{
	OUT.Color = HDRScale(Color);
	OUT.Color.rgb = pow(OUT.Color.rgb, 1.0/2.2);
	// #if %_RT_HDR_ENCODE  
	// 	OUT.Color = EncodeRGBK(OUT.Color, SCENE_HDR_MULTIPLIER);
	// #endif
}


float desaturate(float3 color)
{
	float luminance;
	luminance = dot(color,float3(0.299,0.587,0.114)); //desaturate by dot multiplying with luminance weights.
	return luminance;
}

//Fresnel falloff function for all round application
float fresnel(float3 normal, float3 eyevec, float power, float bias)
{
	float fresnel = saturate(abs(dot(normal,eyevec))); //get fallof by dot product between normal and eye, absolute to prevent falloff to go negative on backside of object 
	fresnel = 1 - fresnel; //invert falloff to get white instead of black on edges
	fresnel = pow(fresnel, power); //power falloff to sharpen effect
	fresnel += bias; // add bias to falloff, this is mostly for cubemap reflections like in carpaint
	
	return saturate(fresnel);
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// EnvMap Samplers
////////////////////////////////////////////////////////////////////////////////////////////////////

float4 DecodeRGBK(in float4 Color, const float fMultiplier, bool bUsePPP= false)
{
	if( bUsePPP )
		Color.rgb *= Color.rgb * (Color.a * Color.a) * fMultiplier;
	else
		Color.rgb *= Color.a * fMultiplier;

	return Color;
}

float4 GetEnvironmentCMap(samplerCUBE envMap, in float3 envTC, in float fNdotE, in float fSpecPower)
{
	// Use higher mip on edges
	float fEdgeMipFix = saturate(1-fNdotE);
	fEdgeMipFix *= fEdgeMipFix;
	fEdgeMipFix *= fEdgeMipFix;

	//float fGlossinessLod = 8.h + log2(1.h-exp2(-1.h/fSpecPower));
	// Approximation is used instead for the sake of performance ( 5 alu less )
	float fGlossinessLod = 6.16231h - 0.497418h * sqrt(fSpecPower);
	fGlossinessLod = max(6.16231h*fEdgeMipFix,fGlossinessLod);

	float4 envColor = DecodeRGBK(texCUBElod( envMap, float4(envTC, fGlossinessLod) ), HDR_FAKE_MAXOVERBRIGHT);

	return envColor;
}

float4 GetEnvironmentCMap(samplerCUBE envMap, in float3 envTC, in float fSpecPower)
{
	//float fGlossinessLod = 8.h + log2(1.h-exp2(-1.h/fSpecPower));
	// Approximation is used instead for the sake of performance ( 5 alu less )
	float fGlossinessLod = 6.16231h - 0.497418h * sqrt(fSpecPower);
	float4 envColor = DecodeRGBK(texCUBElod( envMap, float4(envTC, fGlossinessLod) ), HDR_FAKE_MAXOVERBRIGHT);

	return envColor;
}

float4 GetEnvironment2DMap(sampler2D envMap, in float2 envTC)
{
	float4 envColor = tex2D(envMap, envTC.xy);

	return envColor;
}

