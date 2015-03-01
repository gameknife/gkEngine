//-----------------------------------------------------------------------------
// File: ksHDRProcess.fx
//
// Desc: 新的HDR渲染流程，参考Crytek3.3的调和方法
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

sampler2D _tex1L : register(s1)=
sampler_state
{
	AddressU = Clamp;
	AddressV = Clamp;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
};

pixout GeneralDeferredShadingPS(vert2frag IN)
{
  pixout OUT;

  float4 albeto = tex2D(_tex0, IN.baseTC);
  float4 diffuse = tex2D(_tex1, IN.baseTC);
  float4 specular = tex2D(_tex2, IN.baseTC);

  // calc a general fresnel
//   float3 vView = 
//   pPass.vNormal.xyz = normalize( pPass.cNormalMapRT.xyz );
// 
//   // Store some constant coeficients
//   pPass.fNdotE = ( dot(pPass.vView.xyz, pPass.vNormal.xyz));
// 
//   // change [11/8/2011 Kaiming]
//   //pPass.fSpecPow = MatSpecColor.w;	
// 
//   pPass.vReflVec = (2.0h * pPass.fNdotE * pPass.vNormal.xyz) - pPass.vView.xyz;
// 
//   // Set fresnel
//   pPass.fFresnel = pPass.fFresnel_Bias + pPass.fFresnel_Scale * pow(1.0f - pPass.fNdotE, pPass.fFresnel_Pow); // 4 inst
// 
// 


  float4 final = albeto * diffuse + specular * albeto.w;
 
	
  OUT.Color = final;
  OUT.Color.a = 1;

  OUT.Color = HDRScale( OUT.Color );
  //HDROutput(OUT, final, 1);

  return OUT;
}

technique GeneralDeferredShading
{
	pass p0
	{
		VertexShader = null;

		ZEnable = false;
		ZWriteEnable = false;
		CullMode = None;

		PixelShader = compile ps_3_0 GeneralDeferredShadingPS();
	}
}







