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


float3 sunPosition = float3(1, 1, 1);
float4 skyv2_params = float4(1.0, 3.0, 0.1, 0.6);


//float turbidity = 1.0;
//float reileigh = 3.0;
//float mieCoefficient = 0.1;
//float mieDirectionalG = 0.6;

// constants for atmospheric scattering
const float e = 2.71828182845904523536028747135266249775724709369995957;
const float pi = 3.141592653589793238462643383279502884197169;

const float n = 1.0003; // refractive index of air
const float N = 2.545E25; // number of molecules per unit volume for air at
// 288.15K and 1013mb (sea level -45 celsius)
const float pn = 0.035;	// depolatization factor for standard air

// wavelength of used primaries, according to preetham",
const float3 lambda = float3(680E-9, 550E-9, 450E-9);

// mie stuff
// K coefficient for the primaries
const float3 K = float3(0.686, 0.678, 0.666);
const float v = 4.0;

// optical length at zenith for molecules",
const float rayleighZenithLength = 8.4E3;
const float mieZenithLength = 1.25E3;
const float3 up = float3(0.0, 0.0, 1.0);

const float EE = 1000.0;
const float sunAngularDiameterCos = 0.999956676946448443553574619906976478926848692873900859324;
// 66 arc seconds -> degrees, and the cosine of that

// earth shadow hack",
const float cutoffAngle = 1.61107315568707;//pi / 1.95;
const float steepness = 1.5;
//
//
//float3 totalRayleigh(float3 lambda)
//{
//	float a = (8.0 * pow(pi, 3.0) * pow(pow(n, 2.0) - 1.0, 2.0) * (6.0 + 3.0 * pn));
//	float3 b = 
//	return  / (3.0 * N * pow(lambda, float3(4.0)) * (6.0 - 7.0 * pn));
//}

// see http://blenderartists.org/forum/showthread.php?321110-Shaders-and-Skybox-madness
// A simplied version of the total Reayleigh scattering to works on browsers that use ANGLE",
float3 simplifiedRayleigh()
{
	return 0.0005 / float3(94, 40, 18);
	// return 0.00054532832366 / (3.0 * 2.545E25 * pow(vec3(680E-9, 550E-9, 450E-9), vec3(4.0)) * 6.245);
}

float rayleighPhase(float cosTheta)
{
	return (3.0 / (16.0*pi)) * (1.0 + pow(cosTheta, 2.0));
	//	return (1.0 / (3.0*pi)) * (1.0 + pow(cosTheta, 2.0));",
	//	return (3.0 / 4.0) * (1.0 + pow(cosTheta, 2.0));",
}

float3 totalMie(float3 lambda, float3 K, float T)
{
	float c = (0.2 * T ) * 10E-18;
	return 0.434 * c * pi * pow((2.0 * pi) / lambda, float3(v - 2.0, v - 2.0, v - 2.0)) * K;
}

float hgPhase(float cosTheta, float g)
{
return (1.0 / (4.0*pi)) * ((1.0 - pow(g, 2.0)) / pow(1.0 - 2.0*g*cosTheta + pow(g, 2.0), 1.5));
}

float sunIntensity(float zenithAngleCos)
{
return EE * max(0.0, 1.0 - exp(-((cutoffAngle - acos(zenithAngleCos))/steepness)));
}

// float logLuminance(vec3 c)",
// {",
// 	return log(c.r * 0.2126 + c.g * 0.7152 + c.b * 0.0722);",
// }",

// Filmic ToneMapping http://filmicgames.com/archives/75",
float A = 0.15;
float B = 0.50;
float C = 0.10;
float D = 0.20;
float E = 0.02;
float F = 0.30;
float W = 1000.0;

float3 Uncharted2Tonemap(float3 x)
{
return ((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-E/F;
}

float3 relisky(float3 vWorldPosition)
{
	float3 sunDirection = normalize(g_SunPos.xyz);
		float reileighCoefficient = skyv2_params.y;

	float sunfade = 1.0 - clamp(1.0 - exp((g_SunPos.z / 450000.0)), 0.0, 1.0);

		// luminance =  1.0 ;// vWorldPosition.y / 450000. + 0.5; //sunPosition.y / 450000. * 1. + 0.5;",
		// gl_FragColor = vec4(sunfade, sunfade, sunfade, 1.0);",

		reileighCoefficient = reileighCoefficient - (1.0* (1.0-sunfade));

		float sunE = sunIntensity(dot(sunDirection, up));

		// extinction (absorbtion + out scattering) ",
		// rayleigh coefficients",

		// "vec3 betaR = totalRayleigh(lambda) * reileighCoefficient;",
		float3 betaR = simplifiedRayleigh() * reileighCoefficient;

		// mie coefficients",
		float3 betaM = totalMie(lambda, K, skyv2_params.x) * skyv2_params.z;

		// optical length",
		// cutoff angle at 90 to avoid singularity in next formula.",
		float zenithAngle = acos(max(0.0, dot(up, normalize(vWorldPosition - g_camPos.xyz))));
		float sR = rayleighZenithLength / (cos(zenithAngle) + 0.15 * pow(93.885 - ((zenithAngle * 180.0) / pi), -1.253));
		float sM = mieZenithLength / (cos(zenithAngle) + 0.15 * pow(93.885 - ((zenithAngle * 180.0) / pi), -1.253));



		// combined extinction factor	",
		float3 Fex = exp(-(betaR * sR + betaM * sM));

		// in scattering",
		float cosTheta = dot(normalize(vWorldPosition - g_camPos.xyz), sunDirection);

		float rPhase = rayleighPhase(cosTheta*0.5+0.5);
		float3 betaRTheta = betaR * rPhase;

			float mPhase = hgPhase(cosTheta, skyv2_params.w);
		float3 betaMTheta = betaM * mPhase;


			float3 Lin = pow(sunE * ((betaRTheta + betaMTheta) / (betaR + betaM)) * (1.0 - Fex), float3(1.5,1.5,1.5));
			Lin *= lerp(float3(1.0,1.0,1.0), pow(sunE * ((betaRTheta + betaMTheta) / (betaR + betaM)) * Fex, float3(0.5,0.5,0.5)), clamp(pow(1.0 - dot(up, sunDirection), 5.0), 0.0, 1.0));

		//nightsky",
		float3 direction = normalize(vWorldPosition - g_camPos.xyz);
		float theta = acos(direction.z); // elevation --> y-axis, [-pi/2, pi/2]",
		float phi = atan(direction.x / direction.y); // azimuth --> x-axis [-pi/2, pi/2]",
		float2 uv = float2(phi, theta) / float2(2.0*pi, pi) + float2(0.5, 0.0);
		// vec3 L0 = texture2D(skySampler, uv).rgb+0.1 * Fex;",
		float3 L0 = float3(0.1,0.1,0.1) * Fex;

		// composition + solar disc",
		//if (cosTheta > sunAngularDiameterCos)",
		float sundisk = smoothstep(sunAngularDiameterCos,sunAngularDiameterCos+0.00002,cosTheta);
		// if (normalize(vWorldPosition - g_camPos).y>0.0)",
		L0 += (sunE * 19000.0 * Fex)*sundisk;

		float3 texColor = (Lin + L0);
		texColor *= 0.04 ;
		texColor += float3(0.0, 0.001, 0.0025)*0.3;


		float3 retColor = pow(texColor, float3(1.0 / (1.2 + (1.2*sunfade)), 1.0 / (1.2 + (1.2*sunfade)), 1.0 / (1.2 + (1.2*sunfade))));


		return retColor;
}


void VertexScene( float4 inPos 		: POSITION,
				float4 inTex 		: TEXCOORD0,
				out float4 oPos		: POSITION,
                out float2 oTex0 	: TEXCOORD0,
				out float3 oNormal	: TEXCOORD1
				)
{	
    oPos = mul( float4(inPos.xyz, 1), g_mWorldViewProj );
	oTex0 = float2(inTex.x, inTex.y);
	oNormal = mul(float4(inPos.xyz, 1), g_mWorld).xyz;
}

pixout PixelScene(   
                float2 inTex0 	: TEXCOORD0,
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


	mulColor.rgb = relisky(inNormal);

	//HDROutput(OUT, mulColor, 0);

	OUT.Color.rgb = mulColor.rgb *  mulColor.rgb;// *2.0;
	OUT.Color.a = 1.0;

	OUT.Color = pow(OUT.Color, float4(g_SRGBRevert,g_SRGBRevert,g_SRGBRevert,1));

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

technique FastCubeGenPass
<
int MatDoubleSide = 1;
>
{
	pass P0
	{
		//ZWriteEnable = false;
		VertexShader = compile vs_3_0 VertexScene();
		PixelShader = compile ps_3_0 PixelScene();
		cullmode = CCW;
	}
}

#include "$kszpass.fx"
