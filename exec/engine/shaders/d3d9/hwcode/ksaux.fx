shared float4x4 g_mWorldViewProj 		: 		WORLDVIEWPROJ;
uniform float4 g_auxcolor;

//////////////////////////////////////////////////////////////////////////
// skin function

#define HKG_MAX_BLEND_MATRICES 56
float4x4 g_mLocalMatrixArray[HKG_MAX_BLEND_MATRICES];

sampler2D samTex0: register(s0);

void skinPosition( float3 skinPos, float blendWeightsArray[4], int indexArray[4], out float3 position )
{
	// calculate the pos/normal using the "normal" weights 
	// and accumulate the weights to calculate the last weight
	position = 0.0f;

	float lastWeight = 0.0f;
	for (int iBone = 0; iBone < 3; iBone++)
	{
		lastWeight = lastWeight + blendWeightsArray[iBone];

		position += mul( float4(skinPos,1), g_mLocalMatrixArray[indexArray[iBone]]) * blendWeightsArray[iBone];
	}
	lastWeight =  1.0f - lastWeight; 

	// Now that we have the calculated weight, add in the final influence
	position += (mul( float4(skinPos,1), g_mLocalMatrixArray[indexArray[3]] ) * lastWeight);
}  


void VertFrame( float4 vPos : POSITION,
				float4 vNormal : TEXCOORD0,
                out float4 oPos : POSITION,
				out float4 oColor : TEXCOORD0)
{
   	oPos = mul(vPos, g_mWorldViewProj);
	oPos.z -= 0.0002;

	oColor = g_auxcolor;
}

void VertFrameAnim( float4 vPos : POSITION,
	float4 vNormal : TEXCOORD0,
	float4 BoneWeight : BLENDWEIGHT,
	int4 BoneIndex : BLENDINDICES,
	out float4 oPos : POSITION,
	out float4 oColor : TEXCOORD0)
{
	float3 position;

	float blendWeightsArray[4] = (float[4])BoneWeight;
	int   indexArray[4] = (int[4])BoneIndex;
	skinPosition( vPos.xyz, blendWeightsArray, indexArray, position);    

	oPos = mul(float4(position, 1), g_mWorldViewProj);
	oPos.z -= 0.0002;

	oColor = g_auxcolor;
}

void VertLine( float4 vPos : POSITION,
	float4 vNormal : TEXCOORD0,
	out float4 oPos : POSITION,
	out float4 oColor : TEXCOORD0)
{
	oPos = mul(vPos, g_mWorldViewProj);
	oPos.z -= 0.0002;

	oColor = vNormal;
}

void PixScene( float4 inColor : TEXCOORD0,
				out float4 oColor : COLOR0
) 
{
	oColor = inColor;	
}

void PixSceneTexture( float2 inTex : TEXCOORD0,
	float4 inColor : TEXCOORD1,
	out float4 oColor : COLOR0
	) 
{
	oColor = tex2D(samTex0, inTex) * inColor;
}

void PixSceneTextureAlpha( float2 inTex : TEXCOORD0,
	out float4 oColor : COLOR0
	) 
{
	oColor = tex2D(samTex0, inTex).a;	
}

void Vert2D( float4 vPos : POSITION,
	float4 vColor : TEXCOORD0,
	out float4 oPos : POSITION,
	out float4 oColor : TEXCOORD0)
{
	oPos = vPos;
	oColor = vColor;
}

void Vert2DTex( float4 vPos : POSITION,
	float2 vTex : TEXCOORD0,
	float4 vColor : TEXCOORD1,
	out float4 oPos : POSITION,
	out float2 oTex	: TEXCOORD0,
	out float4 oColor : TEXCOORD1)
{
	oPos = vPos;
	oTex = vTex;
	oColor = vColor;
}

//--------------------------------------------------------------------------------------
// Techniques
//--------------------------------------------------------------------------------------
technique RenderFrame
{
    pass P0
    {
        VertexShader = compile vs_2_0 VertFrame();
        PixelShader  = compile ps_2_0 PixScene();
    }
}

technique RenderFrameAnim
{
	pass P0
	{
		VertexShader = compile vs_2_0 VertFrame();
		PixelShader  = compile ps_2_0 PixScene();
	}
}

technique RenderLine
{
	pass P0
	{
		VertexShader = compile vs_2_0 VertLine();
		PixelShader  = compile ps_2_0 PixScene();
	}
}

technique Render2D
{
	pass P0
	{
		VertexShader = compile vs_2_0 Vert2D();
		PixelShader  = compile ps_2_0 PixScene();
	}
}

technique Render2DTexture
{
	pass P0
	{
		VertexShader = compile vs_2_0 Vert2DTex();
		PixelShader  = compile ps_2_0 PixSceneTexture();
	}
}

technique Render2DTextureAlpha
{
	pass P0
	{
		VertexShader = compile vs_2_0 Vert2DTex();
		PixelShader  = compile ps_2_0 PixSceneTextureAlpha();
	}
}