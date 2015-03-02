//////////////////////////////////////////////////////////////////////////
//
// yikaiming (C) 2014
// gkENGINE Source File 
//
// Name:   	$ksshadowpass.fx
// Desc:	
// 	
// 
// Author:  gameKnife
// Date:	2015/2/27
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _ksshadowpass_fx_
#define _ksshadowpass_fx_

#include "$BasePara.h"
#include "$SamplerDef.h"
#include "$Light_Calc.h"
#include "$ksVertexLib.h"

vert2FragShadow ShadowPassVS( app2vertGeneral	IN	)
{	
	vert2FragShadow OUT = (vert2FragShadow)0; 

	vs_shadow_output( IN, OUT );

	//OUT.HPosition = mul( float4(IN.vertCommon.Position.xyz, 1), g_mWorldViewProj );
	//OUT.baseTC = float4(IN.vertCommon.Texcoord.xy, OUT.HPosition.z, OUT.HPosition.w);

	return OUT;
}

pixout ShadowPassPS(vert2FragShadow IN)
{
	pixout OUT = (pixout)0;
#if defined( DIFSPEC ) || defined( DIFDMASK )

#else
	clip( tex2D(samDiffuse, IN.baseTC.xy).a - 0.35f);
#endif

	
	//
	// Depth is z / w
	//
	OUT.Color = 0;

	return OUT;	
}

//-----------------------------------------------------------------------------
// Technique: ZPass
// Desc: Renders the shadow map
//-----------------------------------------------------------------------------
technique ShadowPass
{
	pass p0
	{
		VertexShader = compile vs_3_0 ShadowPassVS();
		PixelShader = compile ps_3_0 ShadowPassPS();
	}
}

#endif
