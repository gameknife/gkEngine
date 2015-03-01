//////////////////////////////////////////////////////////////////////////
//
// K&K Studio GameKnife ENGINE Source File
//
// Name:   	ksDeferredSnow.fx
// Desc:	后处理雪天，初步试验
// 
// Author:  Kaiming
// Date:	2011/11/22 
// Modify:	2011/11/22
// 
//////////////////////////////////////////////////////////////////////////

// SHADOW_EPSILON means the depth offset, because faces has thick too, or it will shadow its self

float4x4 g_mWorldViewProj 		: 		WORLDVIEWPROJ;
float4x4 g_mWorldView 			: 		WORLDVIEW;
float4x4 g_mViewI   			: 		VIEWI;

sampler2D SamplerSceneNormal : register(s0)=
sampler_state
{
	AddressU = Clamp;
	AddressV = Clamp;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = Point;
};

sampler2D SamplerSceneShadow : register(s1)=
sampler_state
{
	AddressU = Clamp;
	AddressV = Clamp;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = Point;
};

sampler2D SamplerScenePosition : register(s2)=
sampler_state
{
	AddressU = Clamp;
	AddressV = Clamp;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = Point;
};

sampler2D SamplerFrostBite : register(s3)=
sampler_state
{
	AddressU = WRAP;
	AddressV = WRAP;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear; 
};

sampler2D SamplerFrostMask : register(s4)=
sampler_state
{
	AddressU = WRAP;
	AddressV = WRAP;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear; 
};

float g_fSnowAmount = 0.3;

//-----------------------------------------------------------------------------
// Pixel Shader: PixScene
// Desc: Process pixel (do per-pixel lighting) for enabled scene
//-----------------------------------------------------------------------------
float4 PostProcessPS( float2 Tex : TEXCOORD0) : COLOR0
{

	float3 NormalWS = tex2D(SamplerSceneNormal, Tex);
	float Shadow = tex2D(SamplerSceneShadow, Tex);

	NormalWS = mul(NormalWS, g_mViewI); // get the WS norm
	//NormalWS = normalize(NormalWS);

	float3 ScenePosWS = tex2D(SamplerScenePosition, Tex);

	clip( ScenePosWS.z + 4000 );
	//ScenePosWS.w = 1;
	ScenePosWS = mul(ScenePosWS, g_mViewI);
	ScenePosWS += g_mViewI[3];
	float4 frostTex = dot(abs(NormalWS), float3(0,0,1)) * tex2D(SamplerFrostBite, ScenePosWS.xy * 0.1) + dot(abs(NormalWS), float3(0,1,0)) * tex2D(SamplerFrostBite, ScenePosWS.xz * 0.02);
	float4 frostMask = tex2D(SamplerFrostMask, ScenePosWS.xy * 0.02);

	float4 snowColor =  pow(frostTex, 4.0);

	// here, we can judge it is in snow or out, and this value can measure the shell's thickness
	float sum = pow(max(0, (NormalWS.z*0.5f + g_fSnowAmount)), 4.8);
	for (int i=0; i < 3; ++i)
	{
		float3 NormalWSDown = tex2D(SamplerSceneNormal, Tex + float2(0, i * 3.0 / 720.0));
		NormalWSDown = mul(NormalWSDown, g_mViewI); // get the WS norm
		//NormalWSDown = normalize(NormalWSDown);
		float snow = pow(max(0, (NormalWSDown.z*0.5f + g_fSnowAmount)), 4.8);
		sum += snow;
	}

	snowColor *= sum * 0.8 * (Shadow + 0.15);
	snowColor.a = saturate(sum * frostMask.x);
	return snowColor; // 2 inst
}

technique DeferredSnow
{
    pass p0
    {
        VertexShader = null;
        PixelShader = compile ps_3_0 PostProcessPS();
        ZEnable = false;
		
 		AlphaTestEnable = false;
  		AlphaBlendEnable = true;
 		//BlendOp = ADD;
  		SrcBlend = SRCALPHA;
  		DestBlend = ONE;		
    }
}
