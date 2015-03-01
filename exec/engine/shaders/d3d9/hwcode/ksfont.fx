sampler2D tex0 : register(s0)=
	sampler_state
{
	AddressU = Clamp;
	AddressV = Clamp;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
};

void GeneralVS(float3 pos : POSITION, 
	float2 Tex : TEXCOORD0,
	float4 Color : TEXCOORD1,
	out float4 oPos : POSITION,
	out float2 oTex : TEXCOORD0,
	out float4 oColor : TEXCOORD1
	)
{
	oPos = float4(pos, 1.0);
	oTex = Tex;
	oColor = Color;
}

float4 GeneralPS(float2 Tex : TEXCOORD0, float4 Color : TEXCOORD1) : COLOR0
{
	return float4(Color.xyz, tex2D(tex0, Tex).a * Color.w);	
}

//--------------------------------------------------------------------------------------
// Techniques
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
