//////////////////////////////////////////////////////////////////////////
//
// yikaiming (C) 2014
// gkENGINE Source File 
//
// Name:   	$ksreflpass.fx
// Desc:	
// 	
// 
// Author:  gameKnife
// Date:	2015/2/27
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _ksfastcubegenpass_fx_
#define _ksfastcubegenpass_fx_

#include "$BasePara.h"
#include "$SamplerDef.h"
#include "$Light_Calc.h"
#include "$ksVertexLib.h"

vert2FragRefl FastCubeGenPassVS( app2vertGeneral	IN	)
{	
	vert2FragRefl OUT = (vert2FragRefl)0; 

	float4 oPos = mul( float4(IN.vertCommon.Position.xyz, 1.0), g_mWorld );

	oPos = mul( oPos, g_mView );
	oPos = mul( oPos, g_mProjection );

	OUT.HPosition = oPos;
	OUT.baseTC = (IN.vertCommon.Texcoord + float4(g_UVParam.zw,0,0)) * float4(g_UVParam.xy,1,1); //float4(IN.vertCommon.Texcoord.xy, 0, 0);

	return OUT;
}

pixout FastCubeGenPassPS(vert2FragRefl IN)
{
	pixout OUT = (pixout)0;

	float4 color = tex2D(samDiffuse, IN.baseTC.xy) * g_MatDiffuse;
// #if DIFSPEC
// 	
// #else
// 	clip(color.a - 0.3);
// #endif
	
	//
	// Depth is z / w
	//
	OUT.Color = EncodeRGBE(color, 16.0);

	return OUT;	
}

//-----------------------------------------------------------------------------
// Technique: ZPass
// Desc: Renders the shadow map
//-----------------------------------------------------------------------------
technique FastCubeGenPass
{
    pass p0
    {
        VertexShader = compile vs_3_0 FastCubeGenPassVS();
        PixelShader = compile ps_3_0 FastCubeGenPassPS();
    }
}


#endif
