sampler2D tex0 : register(s0)=
	sampler_state
{
	AddressU = Clamp;
	AddressV = Clamp;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
};

float2 EyeToSourceUVScale, EyeToSourceUVOffset;
float4x4 EyeRotationStart, EyeRotationEnd;

float2 TimewarpTexCoord(float2 TexCoord, float4x4 rotMat)
{
	// Vertex inputs are in TanEyeAngle space for the R,G,B channels (i.e. after chromatic 
	// aberration and distortion). These are now "real world" vectors in direction (x,y,1) 
	// relative to the eye of the HMD.	Apply the 3x3 timewarp rotation to these vectors.
	float3 transformed = float3( mul ( rotMat, float4(TexCoord.xy, 1, 1) ).xyz);
	// Project them back onto the Z=1 plane of the rendered images.
	float2 flattened = (transformed.xy / transformed.z);
	// Scale them into ([0,0.5],[0,1]) or ([0.5,0],[0,1]) UV lookup space (depending on eye)
	return(EyeToSourceUVScale * flattened + EyeToSourceUVOffset);
}          

void GeneralVS( float2 vPos : POSITION,
	float2 timeWrap_vignette : TEXCOORD0,
	float2 tc0_r : TEXCOORD1,
	float2 tc1_g : TEXCOORD2,
	float2 tc2_b : TEXCOORD3,
	out float4 oPos : POSITION,
	out float oVignette : TEXCOORD0,
	out float2 oTex0 : TEXCOORD1,
	out float2 oTex1 : TEXCOORD2,
	out float2 oTex2 : TEXCOORD3)
{
	float4x4 lerpedEyeRot = lerp(EyeRotationStart, EyeRotationEnd, timeWrap_vignette.x);
	oTex0  = TimewarpTexCoord(tc0_r,lerpedEyeRot);
	oTex1  = TimewarpTexCoord(tc1_g,lerpedEyeRot);
	oTex2  = TimewarpTexCoord(tc2_b,lerpedEyeRot);
	oPos = float4(vPos.xy, 0.5, 1.0);    
	oVignette = timeWrap_vignette.y;
}

float4 GeneralPS(float vig : TEXCOORD0,
	float2 tc0 : TEXCOORD1,
	float2 tc1 : TEXCOORD2,
	float2 tc2 : TEXCOORD3
	) : COLOR0
{
	float R = tex2D(tex0, tc0.xy).r;
	float G = tex2D(tex0, tc1.xy).g;
	float B = tex2D(tex0, tc2.xy).b;
	return float4(R *vig,G *vig,B *vig,1);  

	//return float4(vig,vig,vig,1.0);	
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
