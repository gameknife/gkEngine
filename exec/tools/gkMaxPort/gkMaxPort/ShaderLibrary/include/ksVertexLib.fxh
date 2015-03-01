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


 struct vert2FragGeneralV
 {  
 	float4 HPosition  : POSITION;
 	float4 baseTC   : TEXCOORD0; 
 	//float4 vpos /*: VPOS*/;
#ifdef _MAX_
	float3 vTangent  : TEXCOORD1;
	float3 vBinormal  : TEXCOORD2;
	float3 vNormal  : TEXCOORD3;

	float3 vView	: TEXCOORD4;
#endif
 };

struct vert2FragGeneral
{  
	float4 HPosition  : POSITION;
	float4 baseTC   : TEXCOORD0; 
	float2 vpos : VPOS;

#ifdef _MAX_
	float3 vTangent  : TEXCOORD1;
	float3 vBinormal  : TEXCOORD2;
	float3 vNormal  : TEXCOORD3;

	float3 vView	: TEXCOORD4;
#endif
};


///////////////// Shared vertex shader computations //////////////////

void vs_shared_output( in app2vertGeneral IN, inout vert2FragGeneralV OUT, bool bUseBump )
{
	// Common data
	OUT.HPosition = mul( IN.vertCommon.Position, g_mWorldViewProj);
	OUT.baseTC = IN.vertCommon.baseTC;
#ifdef _MAX_
	OUT.vTangent = mul(IN.vertCommon.vTangent, g_mWorld);
	OUT.vBinormal = mul(IN.vertCommon.vBinormal, g_mWorld);
	OUT.vNormal = mul(IN.vertCommon.vNormal, g_mWorld);

	OUT.vView = normalize(g_mViewI[3] - mul(IN.vertCommon.Position ,g_mWorld));
#endif
}

/////////////////////// eof ///
