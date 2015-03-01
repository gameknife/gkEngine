
////////////////////////////////////////////////////////////////////////////////////////////////////
// Tools
////////////////////////////////////////////////////////////////////////////////////////////////////
#define ONE_OVER_PI 0.31831
#define ONE_OVER_TWO_PI 0.159155

#define PIXEL_STEP 0.5 / 256.0
#define CONST_BIA 0.0005

#define HDR_SCALE 16.0

////////////////////////////////////////////////////////////////////////////////////////////////////
// EnvMap Samplers
////////////////////////////////////////////////////////////////////////////////////////////////////

vec4 DecodeRGBK(in vec4 Color, float fMultiplier)
{
	Color.rgb *= Color.a * fMultiplier;
	return Color;
}

vec4 GetEnvironmentCMap(samplerCube envMap, in vec3 envTC, in float fSpecPower)
{
	float fGlossinessLod = 6.16231 - 0.497418 * sqrt(fSpecPower);
	envTC.z *= -1.0;
#ifdef GL330
	vec4 envColor = DecodeRGBK(textureCube( envMap, envTC.xzy, fGlossinessLod ), 16.0);
#else
	envTC.z *= -1.0;
	vec4 envColor = DecodeRGBK(textureCubeLod( envMap, envTC.xyz, fGlossinessLod ), 16.0);
#endif
	
	return envColor;
}

float Blinn( vec3 N, vec3 V, vec3 L, float Exp)
{
	float fNormFactor = Exp * ONE_OVER_TWO_PI + ONE_OVER_PI;
	vec3 H = normalize(V + L);	
	return fNormFactor * pow(clamp(dot(N, H), 0.0, 1.0), Exp);
}

float saturate( float val )
{
	return clamp( val, 0.0, 1.0);
}

vec4 saturate( vec4 val )
{
	return clamp( val, vec4(0.0,0.0,0.0,0.0), vec4(1.0,1.0,1.0,1.0) );
}

float smoothstep_opt(in float maxi, in float x)
{
	x = saturate( x / maxi );
	return  x * x  * (3.0 - 2.0 * x);
}

void nvLambSkin(float ldn,	vec4 DiffColor,	vec4 SubColor, float RollOff, out vec4 Diffuse,	out vec4 Subsurface) 
{
	float diffComp = max(0.0,ldn);
	Diffuse = vec4((diffComp * DiffColor).xyz,1.0);
	float subLamb = smoothstep(-RollOff,1.0,ldn) - smoothstep_opt(1.0,ldn);
	subLamb = max(0.0,subLamb);
	Subsurface = SubColor * subLamb;
}

float GetFresnel(float NdotI, float bias, float power)
{
	float facing = (1.0 - NdotI);
	return saturate(bias + (1.0-bias)*pow(facing, power));
}

float SampleDepth( in sampler2D sampler, in vec2 texcoord)
{
#ifdef GL330
    float depth = texture2D(sampler, texcoord).x;
#else
	const vec4 bitShifts = vec4(1.0/(256.0*256.0*256.0), 1.0/(256.0*256.0), 1.0/256.0, 1.0);
	vec4 rawrgba = texture2D(sampler, texcoord);
	float depth = dot(rawrgba.xyzw , bitShifts);
#endif
    
    return depth;
}
