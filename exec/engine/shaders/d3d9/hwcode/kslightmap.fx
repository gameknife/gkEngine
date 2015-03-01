//--------------------------------------------------------------------------------------
// 	File: ksPixelLightning.fx
//	Desc: The effect file for basic pixel lightning. 
//
//  The files of KnifeShader Lab.
// 	It could be used in 3dsMax, FxComposer & DirectX
//
// 	by Kaimingyi 2010.8.18
//--------------------------------------------------------------------------------------
struct fragPassCustom
{
};

struct fragLightPassCustom
{
}; 

#include "$InOutDefine.h"
#include "$BasePara.h"
#include "$Light_Calc.h"
#include "$ksVertexLib.h"

#include "$SamplerDef.h"

void frag_unify_parameters( inout fragPass pPass )
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void frag_custom_begin( inout fragPass pPass )
{
	float4 baseTC = pPass.IN.baseTC;
	float4 bumpTC = pPass.IN.bumpTC;
	pPass.cDiffuseMap = pow(tex2D(samDiffuse, baseTC.xy), 2.2);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void frag_custom_per_light( inout fragPass pPass, inout fragLightPass pLight )
{
	pLight.fNdotL = saturate(pLight.fNdotL * 4.0);

	float3 cK = pLight.fOcclShadow * pLight.fFallOff * pLight.cFilter;

	float3 lightcolor = tex2D(samSpec, pPass.IN.baseTC.zw).xyz;

	pPass.cDiffuseAcc.xyz += lightcolor * lightcolor * cK;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

// remove cAmbient, store in cAmbientAcc
void frag_custom_ambient( inout fragPass pPass, inout float3 cAmbient)
{    
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void frag_custom_end( inout fragPass pPass, inout float3 cFinal )
{
}




#include "$ksPixelLib.h"


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------

vert2FragGeneral GeneralVS( app2vertGeneral	IN	)
{	
	vert2FragGeneral OUT = (vert2FragGeneral)0; 
	vs_shared_output(IN, OUT, true);

	return OUT;
}

pixout GeneralPS(vert2FragGeneral IN)
{
	pixout OUT = (pixout)0;

	// Initialize fragPass structure
	fragPass pPass = (fragPass) 0;

	frag_unify(pPass, IN);

	float4 cFinal = frag_shared_output(pPass);

	HDROutput(OUT, cFinal, 1);

	return OUT;	
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
