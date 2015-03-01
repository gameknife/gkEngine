sampler2D tex0 : register(s0)=
	sampler_state
{
	AddressU = Clamp;
	AddressV = Clamp;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = Point;
};

void GeneralVS( float2 vPos : POSITION,
	float2 timeWrap_vignette : TEXCOORD0,
	float2 tc0_r : TEXCOORD1,
	float2 tc1_g : TEXCOORD2,
	float2 tc2_b : TEXCOORD3,
	out float4 oPos : POSITION,
	out float2 oTex : TEXCOORD0
	)
{
	oPos = float4(vPos.xy, 0.5, 1.0);    
	oTex = timeWrap_vignette;
}

float4 GeneralPS(float2 tc : TEXCOORD0) : COLOR0
{
	float3 RGB = tex2D(tex0, tc);
	return float4(RGB,1);
}

//--------------------------------------------------------------------------------------
// Techniquesv
//--------------------------------------------------------------------------------------
technique RenderScene
{
	pass P0
	{
		VertexShader = compile vs_3_0 GeneralVS();
		PixelShader = compile ps_3_0 GeneralPS();
	}
}

technique RenderScene_Skinned
{
	pass P0
	{
		VertexShader = compile vs_3_0 GeneralVS();
		PixelShader = compile ps_3_0 GeneralPS();
	}
}
