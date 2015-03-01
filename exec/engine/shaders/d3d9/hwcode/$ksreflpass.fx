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

#ifndef _ksreflpass_fx_
#define _ksreflpass_fx_

#include "$BasePara.h"
#include "$SamplerDef.h"
#include "$Light_Calc.h"
#include "$ksVertexLib.h"

float4 g_clipplane = float4(0,0,-1,-1);

vert2FragRefl ReflPassVS( app2vertGeneral	IN	)
{	
	vert2FragRefl OUT = (vert2FragRefl)0; 

	float4 oPos = mul( float4(IN.vertCommon.Position.xyz, 1.0), g_mWorld );
 	oPos.z = 2.0 * g_clipplane.w - oPos.z;
	OUT.clip = dot(float4(oPos.xyz,1.0), g_clipplane);

	oPos = mul( oPos, g_mView );
	oPos = mul( oPos, g_mProjection );

	
	
	OUT.HPosition = oPos;
	OUT.baseTC = float4(IN.vertCommon.Texcoord.xy, 0, 0);

	

	return OUT;
}

pixout ReflPassPS(vert2FragRefl IN)
{
	pixout OUT = (pixout)0;

	if(IN.clip < 0)
		discard;

	float4 color = tex2D(samDiffuse, IN.baseTC.xy);
#if DIFSPEC
	
#else
	clip(color.a - 0.3);
#endif
	
	//
	// Depth is z / w
	//
	OUT.Color = pow( color, 2.2f);

	return OUT;	
}

//-----------------------------------------------------------------------------
// Technique: ZPass
// Desc: Renders the shadow map
//-----------------------------------------------------------------------------
technique ReflGenPass
{
    pass p0
    {
        VertexShader = compile vs_3_0 ReflPassVS();
        PixelShader = compile ps_3_0 ReflPassPS();
    }
}


#endif
