//////////////////////////////////////////////////////////////////////////
//
// K&K Studio GameKnife ENGINE Source File
//
// Name:   	ksSpecilEffects.fx
// Desc:	Ãÿ ‚–ßπ˚	
// 
// Author:  Kaiming
// Date:	2011/11/24 
// Modify:	2011/11/24
// 
//////////////////////////////////////////////////////////////////////////

// SHADOW_EPSILON means the depth offset, because faces has thick too, or it will shadow its self

sampler2D SamplerSceneColor : register(s0)=
sampler_state
{
	AddressU = Clamp;
	AddressV = Clamp;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = Point;
};

sampler1D SamplerOutColor : register(s1)=
sampler_state
{
	AddressU = Clamp;
	AddressV = Clamp;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = Point;
};

sampler2D SamplerGlow : register(s0)=
sampler_state
{
	AddressU = Clamp;
	AddressV = Clamp;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
};


float4 OutGlowMaskGenPS( float2 Tex : TEXCOORD0) : COLOR0
{
	float mask = tex2D(SamplerSceneColor, Tex).a + 0.05f;

	if(mask < 0.1f)
	{
		discard;	
	}


	float3 outcolor = tex1D(SamplerOutColor, mask);

	return float4(outcolor, 1); // 2 inst
}

float4 OutGlowBlendPS( float2 Tex : TEXCOORD0) : COLOR0
{
	return tex2D(SamplerGlow, Tex) * 3.0;
}

technique OutGlowMaskGen
{
    pass p0
    {
        VertexShader = null;
        PixelShader = compile ps_3_0 OutGlowMaskGenPS();
        ZEnable = false;
    }
}

technique OutGlowBlend
{
	pass p0
	{
		VertexShader = null;
		PixelShader = compile ps_3_0 OutGlowBlendPS();
		ZEnable = false;

		AlphaBlendEnable = true;
		BlendOp = ADD;
		SrcBlend = ONE;
		DestBlend = ONE;
	}
}

