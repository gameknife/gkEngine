//-----------------------------------------------------------------------------
// File: ksCommonPost.fx
//
// Desc: 模糊，锐化，调色，colorchart等图像后处理方法
//
// Author: Yi Kaiming 2011/10/23
//-----------------------------------------------------------------------------

//////////////////////////////// Common samplers ////////////////
#include "./$postlib.h"

// Use when explicitly binding from code (_tex to void name colision)
sampler2D _tex0L : register(s0)=
sampler_state
{
    AddressU =	Clamp;
    AddressV = Clamp;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
};

sampler2D _tex0LOD : register(s0) =
sampler_state
{
	AddressU = Clamp;
	AddressV = Clamp;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Point;
};

samplerCUBE _tex0CubeLOD : register(s0) =
sampler_state
{
	AddressU = Clamp;
	AddressV = Clamp;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Point;
};

sampler2D _tex1L : register(s1)=
sampler_state
{
	AddressU = Clamp;
	AddressV = Clamp;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
};

sampler2D _tex0cch : register(s0)=
	sampler_state
{
	AddressU = Clamp;
	AddressV = Clamp;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = None;
	BorderColor = 0x00000000;
};

sampler2D _tex1cch : register(s1)=
	sampler_state
{
	AddressU = Clamp;
	AddressV = Clamp;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = None;
	BorderColor = 0x00000000;
};

float4 PI_psOffsets[8]/* < vsregister = c4; > */;
float4 psWeights[8];

float4 texToTexParams0;
float4 texToTexParams1;
float4 texToTexParams2;
float4 texToTexParams3;

float source_lod;

float3 GetRotate(float3 org, float3 axis, float ang)
{
	float3 zax = axis * dot(org, axis);
	float3 xax = org - zax;
	float3 yax = cross(axis, xax);
	return normalize(xax*cos(ang) + yax*sin(ang) + zax);
}

pixout_fp GaussBlurBilinearCUBEPS(vert2frag IN)
{
	pixout_fp OUT;

	float4 sum = 0;

	float4 col = texCUBElod(_tex0CubeLOD, float4(GetRotate(normalize(IN.baseTC.xyz), PI_psOffsets[0].xyz, PI_psOffsets[0].w), source_lod));
	float keepalpha = col.a;
	sum += col * psWeights[0].x;

	col = texCUBElod(_tex0CubeLOD, float4(GetRotate(normalize(IN.baseTC.xyz), PI_psOffsets[1].xyz, PI_psOffsets[1].w), source_lod));
	sum += col * psWeights[1].x;

	col = texCUBElod(_tex0CubeLOD, float4(GetRotate(normalize(IN.baseTC.xyz), PI_psOffsets[2].xyz, PI_psOffsets[2].w), source_lod));
	sum += col * psWeights[2].x;

	col = texCUBElod(_tex0CubeLOD, float4(GetRotate(normalize(IN.baseTC.xyz), PI_psOffsets[3].xyz, PI_psOffsets[3].w), source_lod));
	sum += col * psWeights[3].x;

	col = texCUBElod(_tex0CubeLOD, float4(GetRotate(normalize(IN.baseTC.xyz), PI_psOffsets[4].xyz, PI_psOffsets[4].w), source_lod));
	sum += col * psWeights[4].x;

	col = texCUBElod(_tex0CubeLOD, float4(GetRotate(normalize(IN.baseTC.xyz), PI_psOffsets[5].xyz, PI_psOffsets[5].w), source_lod));
	sum += col * psWeights[5].x;

	col = texCUBElod(_tex0CubeLOD, float4(GetRotate(normalize(IN.baseTC.xyz), PI_psOffsets[6].xyz, PI_psOffsets[6].w), source_lod));
	sum += col * psWeights[6].x;

	col = texCUBElod(_tex0CubeLOD, float4(GetRotate(normalize(IN.baseTC.xyz), PI_psOffsets[7].xyz, PI_psOffsets[7].w), source_lod));
	sum += col * psWeights[7].x;

	OUT.Color = sum;
	OUT.Color.a = keepalpha;
	return OUT;
}

pixout_fp GaussBlurBilinearLODPS(vert2frag IN)
{
	pixout_fp OUT;

	float4 sum = 0;

		// simply keep alpha [11/26/2011 Kaiming]


		float4 col = tex2Dlod(_tex0LOD, float4(IN.baseTC.xy + PI_psOffsets[0].xy, 0, source_lod));
	float keepalpha = col.a;

	sum += col * psWeights[0].x;

	col = tex2Dlod(_tex0LOD, float4(IN.baseTC.xy + PI_psOffsets[1].xy, 0, source_lod));
	sum += col * psWeights[1].x;

	col = tex2Dlod(_tex0LOD, float4(IN.baseTC.xy + PI_psOffsets[2].xy, 0, source_lod));
	sum += col * psWeights[2].x;

	col = tex2Dlod(_tex0LOD, float4(IN.baseTC.xy + PI_psOffsets[3].xy, 0, source_lod));
	sum += col * psWeights[3].x;

	col = tex2Dlod(_tex0LOD, float4(IN.baseTC.xy + PI_psOffsets[4].xy, 0, source_lod));
	sum += col * psWeights[4].x;

	col = tex2Dlod(_tex0LOD, float4(IN.baseTC.xy + PI_psOffsets[5].xy, 0, source_lod));
	sum += col * psWeights[5].x;

	col = tex2Dlod(_tex0LOD, float4(IN.baseTC.xy + PI_psOffsets[6].xy, 0, source_lod));
	sum += col * psWeights[6].x;

	col = tex2Dlod(_tex0LOD, float4(IN.baseTC.xy + PI_psOffsets[7].xy, 0, source_lod));
	sum += col * psWeights[7].x;

	OUT.Color = sum;
	OUT.Color.a = keepalpha;
	return OUT;
}

pixout_fp GaussBlurBilinearPS(vert2frag IN)
{
  pixout_fp OUT;

  float4 sum = 0;

  // simply keep alpha [11/26/2011 Kaiming]
  

	float4 col = tex2D(_tex0L, IN.baseTC.xy + PI_psOffsets[0].xy);
	float keepalpha = col.a;
	sum += col * psWeights[0].x;  

	col = tex2D(_tex0L, IN.baseTC.xy + PI_psOffsets[1].xy);
	sum += col * psWeights[1].x;  
	
	col = tex2D(_tex0L, IN.baseTC.xy + PI_psOffsets[2].xy);
	sum += col * psWeights[2].x;  

	col = tex2D(_tex0L, IN.baseTC.xy + PI_psOffsets[3].xy);
	sum += col * psWeights[3].x;

	col = tex2D(_tex0L, IN.baseTC.xy + PI_psOffsets[4].xy);
	sum += col * psWeights[4].x;  
	
	col = tex2D(_tex0L, IN.baseTC.xy + PI_psOffsets[5].xy);
	sum += col * psWeights[5].x;  
	
	col = tex2D(_tex0L, IN.baseTC.xy + PI_psOffsets[6].xy);
	sum += col * psWeights[6].x;  
	
	col = tex2D(_tex0L, IN.baseTC.xy + PI_psOffsets[7].xy);
	sum += col * psWeights[7].x;
	
  OUT.Color = sum;
  OUT.Color.a = keepalpha;
  return OUT;
}

pixout BlurDirectionalPS(vert2frag IN)
{
  pixout OUT;


	float2 tcJitter = 1;//(tex2D(_tex1L, IN.baseTC.xy * (PS_ScreenSize.xy/64.0) )*2-1)*0.03 + 0.985;

	float4 cAcc = tex2D(_tex0L, IN.baseTC.xy + tcJitter * texToTexParams0.xy);// * 0.1h;
  cAcc += tex2D(_tex0L, IN.baseTC.xy + tcJitter * texToTexParams0.zw);// * 0.2h;

  cAcc += tex2D(_tex0L, IN.baseTC.xy + tcJitter * texToTexParams1.xy);// * 0.3h;
  cAcc += tex2D(_tex0L, IN.baseTC.xy + tcJitter * texToTexParams1.zw);// * 0.5h;

	cAcc += tex2D(_tex0L, IN.baseTC.xy);// * 0.5h;

  cAcc += tex2D(_tex0L, IN.baseTC.xy + tcJitter * texToTexParams2.xy);// * 0.5h;
  cAcc += tex2D(_tex0L, IN.baseTC.xy + tcJitter * texToTexParams2.zw);// * 0.3h;

  cAcc += tex2D(_tex0L, IN.baseTC.xy + tcJitter * texToTexParams3.xy);// * 0.2h;
  cAcc += tex2D(_tex0L, IN.baseTC.xy + tcJitter * texToTexParams3.zw);// * 0.1h;

	// normalize
	cAcc *= 1.0h / 9.0h; //0.125h;//0.4545h;

  OUT.Color = cAcc;
  
  return OUT;
}

pixout SharpenPS(vert2frag IN)
{
	pixout OUT;

	float4 blurred = tex2D(_tex0, IN.baseTC.xy);
	float4 curr = tex2D(_tex1, IN.baseTC.xy);



	OUT.Color = lerp( blurred, curr, 1.5 );

	return OUT;
}


technique Sharpen
{
	pass p0
	{
		VertexShader = null;
		ZEnable = false;
		ZWriteEnable = false;
		CullMode = None;
		PixelShader = compile ps_3_0 SharpenPS();    
	}
}

technique BlurDirectional
{
  pass p0
  {
    VertexShader = null;
		ZEnable = false;
		ZWriteEnable = false;
		CullMode = None;
    PixelShader = compile ps_3_0 BlurDirectionalPS();    
  }
}

// Optimized gauss blur version, making use of bilinear filtering
technique GaussBlurBilinear
{
  pass p0
  {
    VertexShader = null;

		ZEnable = false;
		ZWriteEnable = false;
		CullMode = None;

    PixelShader = compile ps_3_0 GaussBlurBilinearPS();
  }
}

technique GaussBlurBilinearLOD
{
	pass p0
	{
		VertexShader = null;

		ZEnable = false;
		ZWriteEnable = false;
		CullMode = None;

		PixelShader = compile ps_3_0 GaussBlurBilinearLODPS();
	}
}

technique GaussBlurBilinearCubeLOD
{
	pass p0
	{
		VertexShader = null;

		ZEnable = false;
		ZWriteEnable = false;
		CullMode = None;

		PixelShader = compile ps_3_0 GaussBlurBilinearCUBEPS();
	}
}






float4 HPosToScreenTC_PS(float4 HPos)
{
	float4 ScrTC = HPos;
	ScrTC.xy = (HPos.xy * float2(1,-1) + HPos.ww  ) * 0.5 + 0.5;

	return ScrTC;
}

float4 PS_ScreenSize = float4(1,1,1.0/1280.0,1.0/720.0);
float4 ColorGradingParams0 = float4(10,0.95f,220,0);
float4 ColorGradingParams1 = float4(255,0.8f,0,0);

void AdjustLevels( inout float3 cImage )
{
	float fMinInput = ColorGradingParams0.x;
	float fGammaInput = 1.0 / ColorGradingParams0.y;
	float fMaxInput = ColorGradingParams0.z;
	float fMinOutput = ColorGradingParams0.w;
	float fMaxOutput = ColorGradingParams1.x;

	cImage.xyz = pow( max( (cImage.xyz * 255.0f - fMinInput) / (fMaxInput - fMinInput), 0.0f), fGammaInput);
	cImage.xyz *= (fMaxOutput - fMinOutput);
	cImage.xyz += fMinOutput;
	cImage.xyz /= 255.0;
}

void AdjustSaturation( inout float3 cImage )
{
	float3 LUMINANCE_VECTOR  = float3 (0.2125f, 0.7154f, 0.0721f);

	float fFinalLum = dot(cImage, LUMINANCE_VECTOR); 	
	cImage.rgb = lerp((float3)fFinalLum, cImage, ColorGradingParams1.y); 
}


float fTransitionLerp = 0.5f;

pixout TransitionColorChartPS(vert2frag IN)
{
	pixout OUT;

	float4 cImage0 = float4( tex2Dlod(_tex0cch, float4(IN.baseTC.xy, 0, 1)).xyz, 1.0);
	float4 cImage1 = float4( tex2Dlod(_tex1cch, float4(IN.baseTC.xy, 0, 1)).xyz, 1.0);

	cImage0 = cImage0 + (cImage1 - cImage0) * fTransitionLerp;

	OUT.Color = cImage0;

	return OUT;
}

pixout CombineColorGradingWithColorChartPS(vert2frag IN)
{
	pixout OUT;

	float4 cImage = float4( tex2Dlod(_tex0cch, float4(IN.baseTC.xy, 0, 0)).xyz, 1.0);
	// Levels adjustment
	AdjustLevels( cImage.xyz );
	AdjustSaturation( cImage.xyz );

	OUT.Color = cImage;

	return OUT;
}

float4 g_modifier;
pixout ShowDepthBufferPS(vert2frag IN)
{
	pixout OUT;

	float depth = tex2D(_tex0cch, IN.baseTC.xy ).x * g_modifier.x + g_modifier.y;
	OUT.Color = depth;// * 0.5;

	return OUT;
}

////////////////// technique /////////////////////
technique TransitionColorChart
{
	pass p0
	{
		VertexShader = null;

		ZEnable = false;
		ZWriteEnable = false;
		CullMode = None;

		PixelShader = compile ps_3_0 TransitionColorChartPS();
	}
}

technique MergeColorChart
{
	pass p0
	{
		VertexShader = null;

		ZEnable = false;
		ZWriteEnable = false;
		CullMode = None;

		PixelShader = compile ps_3_0 CombineColorGradingWithColorChartPS();
	}
}

technique ShowDepthBuffer
{
	pass p0
	{
		VertexShader = null;

		ZEnable = false;
		ZWriteEnable = false;
		CullMode = None;

		PixelShader = compile ps_3_0 ShowDepthBufferPS();
	}
}

float4 PixCopy( float2 Tex : TEXCOORD0 ) : COLOR
{
	// just simple copy
	//return float4(0,0,0,1);
	return tex2D( _tex0L, Tex ).rgba;
}

technique SimpleCopyBlended
{
	pass p0
	{
		VertexShader = null;
		PixelShader = compile ps_3_0 PixCopy();
	}
}




