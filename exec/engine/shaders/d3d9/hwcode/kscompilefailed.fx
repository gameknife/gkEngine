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
	// Custom per pass data     
	float3 vTangent;        // perturbed tangent vector
	float3 vBinormal;       // perturbed binormal vector

	float fSpecMultiplier;
	float2 vSurfaceRoughness;
};

struct fragLightPassCustom
{

}; 

#include "$InOutDefine.h"
#include "$BasePara.h"
#include "$Light_Calc.h"
#include "$ksVertexLib.h"

#include "$SamplerDef.h"

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
	float greyScale = saturate((IN.baseTC.x - 0.5) * 10000) * 2 - 1;
	greyScale = saturate((IN.baseTC.y - 0.5) * 10000) * 2 - 1;
	OUT.Color = greyScale * 0.1 + 0.2;
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
