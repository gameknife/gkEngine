/**
 * Copyright (C) 2011 Jorge Jimenez (jorge@iryoku.com)
 * Copyright (C) 2011 Jose I. Echevarria (joseignacioechevarria@gmail.com) 
 * Copyright (C) 2011 Belen Masia (bmasia@unizar.es) 
 * Copyright (C) 2011 Fernando Navarro (fernandn@microsoft.com) 
 * Copyright (C) 2011 Diego Gutierrez (diegog@unizar.es)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *    1. Redistributions of source code must retain the above copyright notice,
 *       this list of conditions and the following disclaimer.
 *
 *    2. Redistributions in binary form must reproduce the following disclaimer
 *       in the documentation and/or other materials provided with the 
 *       distribution:
 *
 *      "Uses SMAA. Copyright (C) 2011 by Jorge Jimenez, Jose I. Echevarria,
 *       Tiago Sousa, Belen Masia, Fernando Navarro and Diego Gutierrez."
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS 
 * IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS OR CONTRIBUTORS 
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are 
 * those of the authors and should not be interpreted as representing official
 * policies, either expressed or implied, of the copyright holders.
 */


/**
 * Setup mandatory defines. Use a real macro here for maximum performance!
 */
// #ifndef SMAA_PIXEL_SIZE // It's actually set on runtime, this is for compilation time syntax checking.
// #define SMAA_PIXEL_SIZE float2(1.0 / 1280.0, 1.0 / 720.0)
// #endif

float2 SMAA_PIXEL_SIZE;

/**
 * This can be ignored; its purpose is to support interactive custom parameter
 * tweaking.
 */
float threshld;
float maxSearchSteps;
float maxSearchStepsDiag;
float cornerRounding;

int4 subsampleIndices;

#ifdef SMAA_PRESET_CUSTOM
#define SMAA_THRESHOLD threshld
#define SMAA_MAX_SEARCH_STEPS maxSearchSteps
#define SMAA_MAX_SEARCH_STEPS_DIAG maxSearchStepsDiag
#define SMAA_CORNER_ROUNDING cornerRounding
#define SMAA_FORCE_DIAGONAL_DETECTION 1
#define SMAA_FORCE_CORNER_DETECTION 1
#endif

// Set the HLSL version:
#define SMAA_HLSL_3 1

// And include our header!
#include "SMAA.h"


/**
 * Input vars and textures.
 */

/**
 * DX9 samplers.
 */
sampler2D colorTex : register(s0)=sampler_state{
    AddressU  = Clamp; AddressV = Clamp;
    MipFilter = Point; MinFilter = Linear; MagFilter = Linear;
    //SRGBTexture = true;
};

sampler2D colorTexG : register(s0)=sampler_state{
    //Texture = <colorTex2D>;
    AddressU  = Clamp; AddressV = Clamp;
    MipFilter = Linear; MinFilter = Linear; MagFilter = Linear;
    //SRGBTexture = false;
};

sampler2D depthTex : register(s1)=sampler_state{
   // Texture = <depthTex2D>;
    AddressU  = Clamp; AddressV = Clamp;
    MipFilter = Linear; MinFilter = Linear; MagFilter = Linear;
    //SRGBTexture = false;
};

sampler2D edgesTex : register(s0)=sampler_state{
   // Texture = <edgesTex2D>;
    AddressU = Clamp; AddressV = Clamp;
    MipFilter = Linear; MinFilter = Linear; MagFilter = Linear;
   // SRGBTexture = false;
};

sampler2D blendTex : register(s1)=sampler_state{
    //Texture = <blendTex2D>;
    AddressU = Clamp; AddressV = Clamp;
    MipFilter = Linear; MinFilter = Linear; MagFilter = Linear;
    //SRGBTexture = false;
};

sampler2D areaTex : register(s1)=sampler_state{
    //Texture = <areaTex2D>;
    AddressU = Clamp; AddressV = Clamp; AddressW = Clamp;
    MipFilter = Linear; MinFilter = Linear; MagFilter = Linear;
    //SRGBTexture = false;
};

sampler2D searchTex : register(s2)=sampler_state{
    //Texture = <searchTex2D>;
    AddressU = Clamp; AddressV = Clamp; AddressW = Clamp;
    MipFilter = Point; MinFilter = Point; MagFilter = Point;
    //SRGBTexture = false;
};


/**
 * Function wrappers
 */
void DX9_SMAAEdgeDetectionVS(inout float4 position : POSITION,
                             inout float2 texcoord : TEXCOORD0,
                             out float4 offset[3] : TEXCOORD1) {
    SMAAEdgeDetectionVS(position, position, texcoord, offset);
}

void DX9_SMAABlendingWeightCalculationVS(inout float4 position : POSITION,
                                         inout float2 texcoord : TEXCOORD0,
                                         out float2 pixcoord : TEXCOORD1,
                                         out float4 offset[3] : TEXCOORD2) {
    SMAABlendingWeightCalculationVS(position, position, texcoord, pixcoord, offset);
}

void DX9_SMAANeighborhoodBlendingVS(inout float4 position : POSITION,
                                    inout float2 texcoord : TEXCOORD0,
                                    out float4 offset[2] : TEXCOORD1) {
    SMAANeighborhoodBlendingVS(position, position, texcoord, offset);
}


float4 DX9_SMAALumaEdgeDetectionPS(float4 position : SV_POSITION,
                                   float2 texcoord : TEXCOORD0,
                                   float4 offset[3] : TEXCOORD1,
                                   uniform SMAATexture2D colorGammaTex) : COLOR {
    return SMAALumaEdgeDetectionPS(texcoord, offset, colorGammaTex);
}

float4 DX9_SMAAColorEdgeDetectionPS(float4 position : SV_POSITION,
                                    float2 texcoord : TEXCOORD0,
                                    float4 offset[3] : TEXCOORD1,
                                    uniform SMAATexture2D colorGammaTex) : COLOR {
    return SMAAColorEdgeDetectionPS(texcoord, offset, colorGammaTex);
}

float4 DX9_SMAADepthEdgeDetectionPS(float4 position : SV_POSITION,
                                    float2 texcoord : TEXCOORD0,
                                    float4 offset[3] : TEXCOORD1,
                                    uniform SMAATexture2D depthTex) : COLOR {
    return SMAADepthEdgeDetectionPS(texcoord, offset, depthTex);
}

float4 DX9_SMAABlendingWeightCalculationPS(float4 position : SV_POSITION,
                                           float2 texcoord : TEXCOORD0,
                                           float2 pixcoord : TEXCOORD1,
                                           float4 offset[3] : TEXCOORD2,
                                           uniform SMAATexture2D edgesTex, 
                                           uniform SMAATexture2D areaTex, 
                                           uniform SMAATexture2D searchTex) : COLOR {
    return SMAABlendingWeightCalculationPS(texcoord, pixcoord, offset, edgesTex, areaTex, searchTex, subsampleIndices);
}

float4 PostMsaaParams = float4(1,1,1,1);

float4 DX9_SMAANeighborhoodBlendingPS(float4 position : SV_POSITION,
                                      float2 texcoord : TEXCOORD0,
                                      float4 offset[2] : TEXCOORD1,
                                      uniform SMAATexture2D colorTex,
                                      uniform SMAATexture2D blendTex) : COLOR {
	float4 final = SMAANeighborhoodBlendingPS(texcoord, offset, colorTex, blendTex);
    return final;
	//return pow(final, 2.2);
}

void DX9_SMAAResolveVS(inout float4 position : POSITION,
	inout float2 texcoord : TEXCOORD0,
	out float4 offset[3] : TEXCOORD1) {
		SMAAResolveVS(position, position, texcoord);
}

float4 DX9_SMAAResolvePS(float4 position : SV_POSITION,
	float2 texcoord : TEXCOORD0,
	float4 offset[2] : TEXCOORD1,
	uniform SMAATexture2D colorTex,
	uniform SMAATexture2D blendTex) : COLOR {
	return SMAAResolvePS(texcoord - PostMsaaParams.xy, texcoord - PostMsaaParams.zw, colorTex, blendTex);
}

/**
 * Time for some techniques!
 */
technique LumaEdgeDetection {
    pass LumaEdgeDetection {
        VertexShader = compile vs_3_0 DX9_SMAAEdgeDetectionVS();
        PixelShader = compile ps_3_0 DX9_SMAALumaEdgeDetectionPS(colorTexG);
        ZEnable = false;        
        SRGBWriteEnable = false;
        AlphaBlendEnable = false;
        AlphaTestEnable = false;

        // We will be creating the stencil buffer for later usage.
        StencilEnable = true;
        StencilPass = REPLACE;
		StencilFunc = ALWAYS;
        StencilRef = 255;
    }
}

technique ColorEdgeDetection {
    pass ColorEdgeDetection {
        VertexShader = compile vs_3_0 DX9_SMAAEdgeDetectionVS();
        PixelShader = compile ps_3_0 DX9_SMAAColorEdgeDetectionPS(colorTexG);
        ZEnable = false;        
        SRGBWriteEnable = false;
        AlphaBlendEnable = false;
        AlphaTestEnable = false;

        // We will be creating the stencil buffer for later usage.
        StencilEnable = true;
        StencilPass = REPLACE;
		StencilFunc = ALWAYS;
        StencilRef = 255;
    }
}

technique DepthEdgeDetection {
    pass DepthEdgeDetection {
        VertexShader = compile vs_3_0 DX9_SMAAEdgeDetectionVS();
        PixelShader = compile ps_3_0 DX9_SMAADepthEdgeDetectionPS(depthTex);
        ZEnable = false;        
        SRGBWriteEnable = false;
        AlphaBlendEnable = false;
        AlphaTestEnable = false;

        // We will be creating the stencil buffer for later usage.
        StencilEnable = true;
        StencilPass = REPLACE;
		StencilFunc = ALWAYS;
        StencilRef = 255;
    }
}

technique BlendWeightCalculation {
    pass BlendWeightCalculation {
        VertexShader = compile vs_3_0 DX9_SMAABlendingWeightCalculationVS();
        PixelShader = compile ps_3_0 DX9_SMAABlendingWeightCalculationPS(edgesTex, areaTex, searchTex);
        ZEnable = false;
        SRGBWriteEnable = false;
        AlphaBlendEnable = false;
        AlphaTestEnable = false;

        // Here we want to process only marked pixels.
        StencilEnable = true;
        StencilPass = KEEP;
        StencilFunc = EQUAL;
        StencilRef = 255;
    }
}

technique NeighborhoodBlending {
    pass NeighborhoodBlending {
        VertexShader = compile vs_3_0 DX9_SMAANeighborhoodBlendingVS();
        PixelShader = compile ps_3_0 DX9_SMAANeighborhoodBlendingPS(colorTex, blendTex);
        ZEnable = false;
        SRGBWriteEnable = false;
        AlphaBlendEnable = false;
        AlphaTestEnable = false;

        // Here we want to process all the pixels.
        StencilEnable = false;
    }
}

technique TemporalResolve {
	pass TemporalResolve {
		VertexShader = compile vs_3_0 DX9_SMAAResolveVS();
		PixelShader = compile ps_3_0 DX9_SMAAResolvePS(colorTex, blendTex);
		ZEnable = false;
		SRGBWriteEnable = false;
		AlphaBlendEnable = false;
		AlphaTestEnable = false;

		// Here we want to process all the pixels.
		StencilEnable = false;
	}
}

struct pixout
{
	float4 Color  : COLOR0;
};

struct vert2frag
{
	float4 HPosition  : POSITION;
	float4 baseTC     : TEXCOORD0;
	float4 farVerts   : TEXCOORD1;
};

sampler2D _tex0 : register(s0)=
	sampler_state
{
	AddressU = Clamp;
	AddressV = Clamp;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = None;
};

sampler2D _tex1 : register(s1)=
	sampler_state
{
	AddressU = Clamp;
	AddressV = Clamp;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = None;
};

float4 PS_ScreenSize = float4(1,1,1.0/1280.0,1.0/720.0);

float4 kh = float4(4,4,4,4);
float4 kl = float4(2,2,2,2);

///////////////// pixel shader //////////////////
pixout PostMSAAPS(vert2frag IN)
{
	pixout OUT = (pixout) 0;

	//////////////////////////////////////////////////////////////////////////
	// simple 
	float4 cCurrFrame = tex2D(_tex1, IN.baseTC.xy - PostMsaaParams.xy);
	float4 cPrevFrame = tex2D(_tex0, IN.baseTC.xy - PostMsaaParams.zw);

#if SMAA1TX
	//////////////////////////////////////////////////////////////////////////
	// preserve sharp edge
	float4 cTL = tex2D(_tex1, IN.baseTC.xy - PostMsaaParams.xy + float2(-1, -1) * PS_ScreenSize.zw);
	float4 cBL = tex2D(_tex1, IN.baseTC.xy - PostMsaaParams.xy + float2(-1, 1) * PS_ScreenSize.zw);
	float4 cTR = tex2D(_tex1, IN.baseTC.xy - PostMsaaParams.xy + float2(1, -1) * PS_ScreenSize.zw);
	float4 cBR = tex2D(_tex1, IN.baseTC.xy - PostMsaaParams.xy + float2(1, 1) * PS_ScreenSize.zw);

	float4 cMax = max(cTL, max(cTR, max(cBL, cBR))); 
	float4 cMin = min(cTL, min(cTR, min(cBL, cBR))); 

	float4 wk = abs((cTL+cTR+cBL+cBR)*0.25-cCurrFrame); 

	OUT.Color =  lerp(cCurrFrame, clamp(cPrevFrame, cMin, cMax), saturate(rcp(lerp(kl, kh, wk))) ); 
#else
	OUT.Color = ( cCurrFrame + cPrevFrame ) * 0.5h;
#endif
	return OUT;
}

technique PostMSAA
{
	pass p0
	{
		VertexShader = null;
		ZEnable = false;
		ZWriteEnable = false;
		CullMode = None;
		PixelShader = compile ps_3_0 PostMSAAPS();     
	}
}
