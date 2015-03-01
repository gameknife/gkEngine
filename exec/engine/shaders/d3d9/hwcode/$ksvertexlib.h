//////////////////////////////////////////////////////////////////////////
//
// K&K Studio GameKnife ENGINE Source File
//
// Name:   	ksVertexLib.fx
// Desc:		
// 
// Author:  Kaiming
// Date:	2011/11/8 
// Modify:	2011/11/8
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _ksVertexLib_h_
#define _ksVertexLib_h_

float4 HPosToScreenTC(float4 HPos)
{
	float4 ScrTC = HPos;
	ScrTC.xy = (HPos.xy * float2(1.0,-1.0) + HPos.ww) * 0.5;
 	ScrTC.xy += g_fScreenSize.xy * HPos.ww;
	return ScrTC;
}

void skinPosition( float3 skinPos, float blendWeightsArray[4], int indexArray[4], out float3 position )
{
	position = 0.0f;
	float lastWeight = 0.0f;
	for (int iBone = 0; iBone < 3; iBone++)
	{
		lastWeight = lastWeight + blendWeightsArray[iBone];
		position += mul( float4(skinPos,1), g_mLocalMatrixArray[indexArray[iBone]]) * blendWeightsArray[iBone];
	}
	lastWeight =  1.0f - lastWeight; 

	position += (mul( float4(skinPos,1), g_mLocalMatrixArray[indexArray[3]] ) * lastWeight);
}  

void skinNormal( float3 skinPos, float blendWeightsArray[4], int indexArray[4], out float3 position )
{
	position = 0.0f;
	float lastWeight = 0.0f;
	for (int iBone = 0; iBone < 3; iBone++)
	{
		lastWeight = lastWeight + blendWeightsArray[iBone];
		position += mul( float4(skinPos,0), g_mLocalMatrixArray[indexArray[iBone]]) * blendWeightsArray[iBone];
	}
	lastWeight =  1.0f - lastWeight; 

	position += (mul( float4(skinPos,0), g_mLocalMatrixArray[indexArray[3]] ) * lastWeight);
}  


struct app2vertCommon
{
	float3 Position		: POSITION;
	float4 Texcoord		: TEXCOORD0;
	float4 Tangent		: TEXCOORD1;
	float3 Binormal		: TEXCOORD2;
	float4 BoneWeight	: BLENDWEIGHT;
	int4 BoneIndex		: BLENDINDICES;
};


struct app2vertGeneral
{  
	app2vertCommon vertCommon;
};

 struct vert2FragGeneral
 {  
 	float4 HPosition	: POSITION;
 	float4 baseTC		: TEXCOORD0;
	float3 worldPos		: TEXCOORD1;
	float4 screenTC		: TEXCOORD2;

	float4 detailTC     : TEXCOORD3;
 };

 struct vert2FragZpass
 {  
	 float4 HPosition	: POSITION;
	 float4 baseTC		: TEXCOORD0;
	 float4 TangentWS	: TEXCOORD1;
	 float3 BinormalWS	: TEXCOORD2;
	 float4 PosTex		: TEXCOORD3;
	 float3 WorldPos	: TEXCOORD4;

 };

 struct vert2FragZpassV
 {  
	 float4 HPosition	: POSITION;
	 float4 baseTC		: TEXCOORD0;
	 float4 TangentWS	: TEXCOORD1;
	 float3 BinormalWS	: TEXCOORD2;
	 float4 PosTex		: TEXCOORD3;

	  int2 ScreenPos		: VPOS;
 };


 struct vert2FragShadow
 {  
	 float4 HPosition	: POSITION;
	 float4 baseTC		: TEXCOORD0;
 };

 struct vert2FragRefl
 {  
	 float4 HPosition	: POSITION;
	 float4 baseTC		: TEXCOORD0;
	 float clip			: TEXCOORD1;
 };

///////////////// Shared vertex shader computations //////////////////

void vs_shared_output( in app2vertGeneral IN, inout vert2FragGeneral OUT, bool bUseBump )
{
	// Common data
	float3 position = IN.vertCommon.Position.xyz;
#if SKIN
	float blendWeightsArray[4] = (float[4])IN.vertCommon.BoneWeight;
	int   indexArray[4] = (int[4])IN.vertCommon.BoneIndex;
	skinPosition( IN.vertCommon.Position.xyz, blendWeightsArray, indexArray, position);    
#endif
	OUT.HPosition = mul( float4(position,1), g_mWorldViewProj);
	OUT.baseTC = (IN.vertCommon.Texcoord + float4(g_UVParam.zw,0,0)) * float4(g_UVParam.xy,1,1);
	OUT.screenTC = HPosToScreenTC(OUT.HPosition);
	OUT.worldPos = mul( float4(position,1), g_mWorld);
}

void vs_shared_output_detail( in app2vertGeneral IN, inout vert2FragGeneral OUT, bool bUseBump )
{
	// Common data
	OUT.HPosition = mul( float4(IN.vertCommon.Position.xyz,1), g_mWorldViewProj);
	OUT.baseTC = (IN.vertCommon.Texcoord + float4(g_UVParam.zw,0,0)) * float4(g_UVParam.xy,1,1);
	OUT.detailTC = (IN.vertCommon.Texcoord + float4(0,0,0,0)) * float4(512,512,1,1);

	OUT.screenTC = HPosToScreenTC(OUT.HPosition);
	OUT.worldPos = mul( float4(IN.vertCommon.Position.xyz,1), g_mWorld);
	//HPosToScreenTC(OUT.HPosition);
}

void vs_shared_output_zpass( in app2vertGeneral IN, inout vert2FragZpass OUT, bool bUseBump )
{
	// Common data
	OUT.WorldPos = IN.vertCommon.Position.xyz;
	float3 tangent = IN.vertCommon.Tangent.xyz;
	float3 binormal = IN.vertCommon.Binormal.xyz;


#if SKIN
	float blendWeightsArray[4] = (float[4])IN.vertCommon.BoneWeight;
	int   indexArray[4] = (int[4])IN.vertCommon.BoneIndex;
	skinPosition( IN.vertCommon.Position.xyz, blendWeightsArray, indexArray, OUT.WorldPos); 
	skinNormal( IN.vertCommon.Tangent.xyz, blendWeightsArray, indexArray, tangent);
	skinNormal( IN.vertCommon.Binormal.xyz, blendWeightsArray, indexArray, binormal);
#endif

	OUT.HPosition = mul( float4(OUT.WorldPos,1), g_mWorldViewProj);
	OUT.WorldPos = mul( float4(OUT.WorldPos,1), g_mWorld);

	OUT.baseTC = (IN.vertCommon.Texcoord + float4(g_UVParam.zw,0,0)) * float4(g_UVParam.xy,1,1);

	OUT.TangentWS = float4( mul( tangent,  (float3x3) g_mWorld ), IN.vertCommon.Tangent.w );
	OUT.BinormalWS = mul( binormal,  (float3x3) g_mWorld );

	OUT.PosTex.xy = OUT.HPosition.zw;
}

void vs_shared_output_zpass_autoflip( in app2vertGeneral IN, inout vert2FragZpass OUT, bool bUseBump )
{
	// Common data
	OUT.HPosition = mul( float4(IN.vertCommon.Position.xyz,1), g_mWorldViewProj);
	OUT.baseTC = (IN.vertCommon.Texcoord + float4(g_UVParam.zw,0,0)) * float4(g_UVParam.xy,1,1);

	OUT.TangentWS = float4( mul( IN.vertCommon.Tangent.xyz,  (float3x3) g_mWorld ), IN.vertCommon.Tangent.w );
	OUT.BinormalWS = mul( IN.vertCommon.Binormal.xyz,  (float3x3) g_mWorld );

	float3 normalWS = cross(OUT.TangentWS.xyz, OUT.BinormalWS) * OUT.TangentWS.w;

	OUT.WorldPos = mul( float4(IN.vertCommon.Position.xyz,1), g_mWorld);

	OUT.PosTex.xy = OUT.HPosition.zw;
}

/////////////////////// eof ///
#endif