//-----------------------------------------------------------------------------
// File: ksHDRProcess.fx
//
// Desc: 新的HDR渲染流程，参考Crytek 3.3的调和方法
//
// Author: Yi Kaiming 2011/10/23
//-----------------------------------------------------------------------------
#include "$VolumeFog.h"
#include "$postlib.h"

float4x4 g_mViewI : VIEWI;


sampler2D baseMap : register(s0)=
	sampler_state
{
	AddressU = Clamp;
	AddressV = Clamp;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
};



sampler2D lumMap0 : register(s0)=
	sampler_state
{
	AddressU = Clamp;
	AddressV = Clamp;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = Point;
};

sampler2D lumMap1 : register(s1)=
	sampler_state
{
	AddressU = Clamp;
	AddressV = Clamp;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = Point;
};

sampler2D bloom0Map : register(s0)=
	sampler_state
{
	AddressU = Clamp;
	AddressV = Clamp;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
};

sampler2D bloom1Map : register(s1)=
	sampler_state
{
	AddressU = Clamp;
	AddressV = Clamp;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
};

sampler2D bloom2Map : register(s2)=
	sampler_state
{
	AddressU = Clamp;
	AddressV = Clamp;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
};

sampler2D streakMap : register(s3)=
	sampler_state
{
	AddressU = Clamp;
	AddressV = Clamp;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
};

sampler2D prevBloomMap : register(s4)=
	sampler_state
{
	AddressU = Clamp;
	AddressV = Clamp;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
};


sampler2D bloomMap0 : register(s2)=
	sampler_state
{
	AddressU = Clamp;
	AddressV = Clamp;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
};

sampler2D vignettingMap : register(s3)=
	sampler_state
{
	AddressU = Clamp;
	AddressV = Clamp;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
};

sampler2D sunshaftMap : register(s4)=
	sampler_state
{
	AddressU = Clamp;
	AddressV = Clamp;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
};

sampler2D sceneDepth : register(s5)=
	sampler_state
{
	AddressU = Clamp;
	AddressV = Clamp;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = Point;
};

sampler2D sceneDOF : register(s6)=
	sampler_state
{
	AddressU = Clamp;
	AddressV = Clamp;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = Point;
};

sampler2D noiseMap : register(s7)=
	sampler_state
{
	AddressU = Wrap;
	AddressV = Wrap;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = Point;
};

sampler2D colorChartMap : register(s8)=
	sampler_state
{
	AddressU = Clamp;
	AddressV = Clamp;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
	BorderColor = 0x00000000;
};



sampler2D zMap : register(s0)=
	sampler_state
{
	AddressU = Clamp;
	AddressV = Clamp;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Point;
};



// EyeAdpatedFactor, BrightOffset, BrightThreshold, BrightLevel
float4 HDRParams0 = float4(0.85f, 5.0f, 3.3f, 0.85f);
// EyeAdaptedBase, HDRLevel, HDROffset, 
float4 HDRParams1 = float4(0.0f, 6.0f, 10.0f, 1.0f);
// SceneLum, SceneScale, HDRConstract, 
float4 HDRParams2 = float4(1.0f, 1.0f, 1.0f, 1.0f);
// (*, *, vignetting threshold, vignetting amount)
float4 HDRParams3 = float4(1.0f, 1.0f, 1.0f, 1.0f);				
// BlueShift Color
float3 HDRParams4 = float3( 0.8f, 0.8f, 1.0f);
// BloomColor, GrainAmount
float4 HDRParams5 = float4(0.8f, 0.8f, 1.4f, 1.0f);
// SunshaftColor
float4 HDRParams6 = float4(255.0/255.0, 180.0/255.0, 2.0/255.0, 1.0f);
// StreakColor
float4 HDRParams7 = float4(0.05f, 0.05f, 0.1f, 1.0f);

float4 HDRDofParams = float4(0.5f, 1.0f, 100.f, 1.0f); // 焦距近， 焦距远， 远， 近

// kernel offset
float4 SampleLumOffsets0;
float4 SampleLumOffsets1;

// kernel offset & weight
float4 SampleOffsets[16];// < register = c0; >
float4 SampleWeights[16];// < register = c16; >

float4 PS_ScreenSize = float4(1280, 720, 0, 0);

float4 dofParamsFocus = float4(0, 2000, 0, 1);
float4 FrameRand = float4(0, 0, 0, 0);




// RangeReducedAdaptedLum
float EyeAdaption( float fSceneLuminance )
{
	float ret = lerp(HDRParams1.x,fSceneLuminance,HDRParams0.x);
	return ret;
}

float4 ApplyBlueShift(in vert2frag IN, in float4 cSceneIN, in float fLum )
{
	float4 cScene = cSceneIN;
	// Blue shift for stocopic (dark) scenes
	float fLumBlendMul = 5.0;                   
	float3 cBlueTarget = float3( 0.8f, 0.8f, 1.4f);

	cBlueTarget = HDRParams4;
	fLumBlendMul = 32;
	cBlueTarget *= fLum;
	cScene.rgb = lerp(cBlueTarget, cScene.rgb,  saturate(HDRParams3.x + fLumBlendMul * fLum));

	return cScene;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
float3 FilmToneMap(float3 color)
{
	color = max(0, color - 0.004f);
	color = (color * (6.2h * color + 0.5h)) / (color * (6.2h * color + 1.7h)+ 0.06h);

	// result has 1/2.2 baked in - can be optimized further
	return pow(color, 2.2h);
}

#define PARA_C 0.48

float3 FilmToneMapEX(float3 color)
{
	float3 x = color;
	float3 t1 = 6.2 * x * x;
	float3 t2 = PARA_C * x;
	float3 y = (t1 + t2) / (t1 + 4.1 * t2 + 0.05) + (0.634 * PARA_C - 0.247) * x;

	return pow(y, 2.2h);
}

// Filmic tone mapping
float4 FilmMapping( in vert2frag IN, in float4 cScene, in float4 cBloom, in float fAdaptedLum, in float fVignetting )
{
	float fLum = GetLuminance( cScene.rgb );
	fAdaptedLum = EyeAdaption( fAdaptedLum );

	// Exposure
	float fAdaptedLumDest = HDRParams1.y / (1e-6 + 1.0h + HDRParams1.z * fAdaptedLum);

	//cScene = ApplyBlueShift( IN, cScene, fLum );

	// Tone mapping
	cScene.xyz = FilmToneMapEX( fVignetting * (fAdaptedLumDest * cScene.xyz + cBloom)  );
	//cScene.xyz = FilmToneMapEX(cScene.xyz);

	return cScene;
}

// Filmic ToneMapping http://filmicgames.com/archives/75",
const float A = 0.15;
const float B = 0.50;
const float C = 0.10;
const float D = 0.20;
const float E = 0.02;
const float F = 0.30;
const float W = 1000.0;

float3 Uncharted2Tonemap(float3 x)
{
	return ((x*(A*x + C*B) + D*E) / (x*(A*x + B) + D*F)) - E / F;
}

float4 FilmMappingUncharted2(in vert2frag IN, in float4 cScene, in float4 cBloom, in float fAdaptedLum, in float fVignetting)
{
	float fLum = GetLuminance(cScene.rgb);
	fAdaptedLum = EyeAdaption(fAdaptedLum);

	// Exposure
	float fAdaptedLumDest = HDRParams1.y / (1e-6 + 1.0h + HDRParams1.z * fAdaptedLum);

	float3 whiteScale = 1.0 / Uncharted2Tonemap(float3(W, W, W));
		float3 texColor = fVignetting * (fAdaptedLumDest * cScene.xyz + cBloom).rgb;
		float3 curr = Uncharted2Tonemap((log2(2.0 / pow(0.2, 4.0)))*texColor);
		float3 color = curr*whiteScale;
		cScene.rgb = color;

	return cScene;
}

// Exponentional mappping 
float4 ExpMapping( in vert2frag IN, in float4 cScene, in float4 cBloom, in float fAdaptedLum, in float fVignetting )
{
	float fLum = GetLuminance( cScene.rgb );
	fAdaptedLum = EyeAdaption( fAdaptedLum );

	// Exposure
	float fAdaptedLumDest = HDRParams1.y / (1e-6 + 1.0h + HDRParams1.z * fAdaptedLum);

	// mute blueshift here
	cScene = ApplyBlueShift( IN, cScene, fLum );

	// Tone mapping		
	cScene.xyz = 1 - exp2( -fVignetting *(fAdaptedLumDest * cScene.xyz + cBloom));

	return cScene;
}

pixout_fp HDRSampleLumInitialPS(vert2frag IN)
{
	pixout_fp OUT = (pixout_fp) 0;

	float fRecipSampleCount = 0.25h;
	float2 vLumInfo = float2(0.0h, 64.0h);//, -64.0h); 

	float3 cTex = tex2D(baseMap, IN.baseTC.xy + SampleLumOffsets0.xy).rgb * HDRParams2.y;
	float fLum = GetLuminance(cTex.rgb);

	vLumInfo.x += fLum;

	vLumInfo.y = min(vLumInfo.y, fLum);

	cTex = tex2D(baseMap, IN.baseTC.xy + SampleLumOffsets0.zw).rgb * HDRParams2.y;
	fLum = GetLuminance(cTex.rgb);

	vLumInfo.x += fLum;

	vLumInfo.y = min(vLumInfo.y, fLum);

	cTex = tex2D(baseMap, IN.baseTC.xy + SampleLumOffsets1.xy).rgb * HDRParams2.y;
	fLum = GetLuminance(cTex.rgb);

	vLumInfo.x += fLum;

	vLumInfo.y = min(vLumInfo.y, fLum);

	cTex = tex2D(baseMap, IN.baseTC.xy + SampleLumOffsets1.zw).rgb * HDRParams2.y;
	fLum = GetLuminance(cTex.rgb);

	vLumInfo.x += fLum;

	vLumInfo.y = min(vLumInfo.y, fLum);

	// clamp to acceptable range
	OUT.Color.xy = min(vLumInfo.xy * float2(fRecipSampleCount, 1), 64);

	return OUT;
}

pixout_fp HDRSampleLumIterativePS(vert2frag IN)
{
	pixout_fp OUT = (pixout_fp) 0;

	int nIter = 16;
	float nRecipIter = 1.0h / (float) nIter;

	float4 vResampleSum = 0.0f; 
	for(int i=0; i<nIter; i++)
	{
		// Compute the sum of luminance throughout the sample points
		float2 vTex = GetLuminanceMap(baseMap, IN.baseTC.xy+SampleOffsets[i].xy);
		vResampleSum.xy += vTex.xy;
	}


	vResampleSum.xy *= nRecipIter;

	OUT.Color = vResampleSum;

	return OUT;
}

pixout HDRBrightPassFilterPS(vert2frag IN)
{
	pixout OUT;

	float4 vSample;
	vSample = tex2D(baseMap, IN.baseTC.xy) * HDRParams2.y;
	vSample.rgb = (vSample.rgb> 10000.0f)? float3(1, 1, 1): vSample.rgb;

	float fAdaptedLum = GetLuminanceMap(lumMap1, float2(0.5f, 0.5f));

	float Level = HDRParams0.w;
	float BrightOffset = HDRParams0.y;
	float BrightThreshold = HDRParams0.z;

	float fAdaptedLumDest = EyeAdaption(fAdaptedLum);

	vSample.rgb *= Level/(fAdaptedLumDest + 1e-6);
	vSample.rgb -= BrightThreshold;
	vSample.rgb = max(vSample.rgb, (float3)0.0);

	vSample.rgb /= (BrightOffset + vSample.rgb);

	vSample.rgb *= 8.0f;

	OUT.Color = vSample;

	return OUT;
}

float2 GetViggnetingVectorized(float4 uv)
{
	float2 vViggneting = saturate( 1 - float2( dot( uv.xy * 2.h - 1.h, uv.xy * 2.h - 1.h ),
		dot( uv.zw * 2.h - 1.h, uv.zw * 2.h - 1.h ))  );
	return vViggneting;
}

pixout HDRBloomGenPS(vert2frag IN)
{
	pixout OUT = (pixout) 0;

	float4 cBloom0 = tex2D(bloom0Map, IN.baseTC.xy);
	float4 cBloom1 = tex2D(bloom1Map, IN.baseTC.xy);
	float4 cBloom2 = tex2D(bloom2Map, IN.baseTC.xy);
	OUT.Color += (cBloom0  + cBloom1  + cBloom2 );

	// gen lensflare
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Compute inner lens reflection/flares
	//#if 0
	float3 cInnerLensRefl = 0;

	// add ghosting flares (aka-lens flares) - 5 flares seems enough for visual cue  
	float2 baseTC=IN.baseTC.xy-0.5h;
	float4 flaresTC0 = baseTC.xyxy * float4( 0.5h, 0.5h, -1.0h, -1.0h ) + 0.5h;
	float4 flaresTC1 = baseTC.xyxy * float4(-0.45h, -0.45h, -0.25h, -0.25h ) + 0.5h;
	float4 vFlaresVignetting = float4( GetViggnetingVectorized( flaresTC0 ),
		GetViggnetingVectorized( flaresTC1 ) );

	float2 uv= flaresTC0.xy;
	float fVignetting=vFlaresVignetting.x;
	float4 tap=0;//tex2D(sceneMap1,uv);//*float4(128,196,255,255)/255.0;  
	tap.rgb=tex2D(bloom1Map,(uv -0.5)*0.96 + 0.5).rgb;//*float4(196,128,255,255)/255.0;  
	//tap.g=tex2D(sceneMap1,(uv -0.5)*0.98 + 0.5).g;
	//tap.b=tex2D(sceneMap1,(uv -0.5)*1.0 + 0.5).b;
	cInnerLensRefl+= tap*fVignetting;//*float4(196,128,255,255)/255.0;

	uv= flaresTC0.zw;
	fVignetting=vFlaresVignetting.y;
	tap=0;//tex2D(sceneMap1,uv);//*float4(128,255,196,255)/255.0;  
	tap.rgb=tex2D(bloom1Map,(uv -0.5)*0.96 + 0.5).rgb;//*float4(196,128,255,255)/255.0;  
	//tap.g=tex2D(sceneMap1,(uv -0.5)*0.98 + 0.5).g;
	//tap.b=tex2D(sceneMap1,(uv -0.5)*1.0 + 0.5).b;	
	cInnerLensRefl+= tap*fVignetting;//*float4(196,128,255,255)/255.0;

	uv= flaresTC1.xy;
	fVignetting=vFlaresVignetting.z;
	tap=0;//tex2D(sceneMap1,uv);//*float4(255,196,128,255)/255.0;  
	tap.rgb=tex2D(bloom1Map,(uv -0.5)*0.96 + 0.5).rgb;//*float4(196,128,255,255)/255.0;  
	//tap.g=tex2D(sceneMap1,(uv -0.5)*0.98 + 0.5).g;
	//tap.b=tex2D(sceneMap1,(uv -0.5)*1.0 + 0.5).b;	
	cInnerLensRefl+= tap*fVignetting;//*float4(196,128,255,255)/255.0;

	uv= flaresTC1.zw;
	fVignetting=vFlaresVignetting.w;
	tap.rgb=tex2D(bloom1Map,(uv -0.5)*0.96+ 0.5).rgb;//*float4(196,128,255,255)/255.0;  
	//tap.g=tex2D(sceneMap1,(uv -0.5)*0.98 + 0.5).g;
	//tap.b=tex2D(sceneMap1,(uv -0.5)*1.0 + 0.5).b;	
	cInnerLensRefl+= tap*fVignetting;//*float4(196,128,255,255)/255.0;

	OUT.Color.xyz+=cInnerLensRefl*HDRParams6 * 0.2;
	//#endif

	// add streaks
	float3 cStreaks=0;
	cStreaks.rgb = tex2D(streakMap,(IN.baseTC.xy -0.5)*0.97 + 0.5);
	//cStreaks.g = tex2D(sceneMap3,(IN.baseTC.xy -0.5)*0.985 + 0.5).g;
	//cStreaks.b = tex2D(sceneMap3,(IN.baseTC.xy -0.5)*1.0 + 0.5).b;
	OUT.Color.rgb += cStreaks * HDRParams7;

	// Blend cache with previous frame
	float4 cPrevFrame = tex2D(prevBloomMap, IN.baseTC.xy);
	OUT.Color.rgb = lerp(cPrevFrame, OUT.Color.rgb, 0.7f);


	return OUT;
}


pixout HDRStreakInitialPS(vert2frag IN)
{
	pixout OUT;

	float4 cBloom0 = tex2D(bloom0Map, IN.baseTC.xy);
	float4 cBloom1 = tex2D(bloom1Map, IN.baseTC.xy);
	float4 cBloom2 = tex2D(bloom2Map, IN.baseTC.xy);
	OUT.Color = (cBloom0  + cBloom1  + cBloom2 );
	//OUT.Color = sqrt( OUT.Color );

	return OUT;
}

pixout HDRFinalScenePS(vert2frag IN)
{
	pixout OUT = (pixout)0;

	// get org color
	float4 vSample = tex2D(baseMap, IN.baseTC.xy);
	float specialMask = vSample.a;

	// get vignetting
	float fVignetting = tex2D(vignettingMap, IN.baseTC.xy);  

	// mute grain, now
	// Apply bayer pattern for grain simulation (todo: try out Kodak latest pattern -> less noise/green)
	float4 cGrain = tex2D(noiseMap, ((IN.baseTC.xy+ FrameRand.xy) / 64.0) * PS_ScreenSize.xy);
	vSample.rgb *= 1.0 + cGrain.rgb * HDRParams5.w;

	// use argb16f, instead
	//float4 cBloom = DecodeRGBK( tex2D(bloomMap0, IN.baseTC.xy), SCENE_HDR_MULTIPLIER , true);
	float4 cBloom = tex2D(bloomMap0, IN.baseTC.xy);
	cBloom *= HDRParams5 * 3.0 / 8.0;

	////////////////////////////////////////////////////////////////////////////////////////////
	float fAdaptedLum = GetLuminanceMap(lumMap1, float2(0.5h, 0.5h));

	// exposure
	//vSample = FilmMappingUncharted2(IN, vSample, cBloom, fAdaptedLum, fVignetting);
	//vSample = FilmMapping(IN, vSample, cBloom, fAdaptedLum, fVignetting);
	vSample = ExpMapping(IN, vSample, cBloom, fAdaptedLum, fVignetting);
	OUT.Color.rgb = vSample;	

	// and we add sun shaft here
	float sunmask = tex2D( sunshaftMap, IN.baseTC.xy).x;
	OUT.Color.rgb += HDRParams6.rgb * sunmask * 2.0;
	//OUT.Color.rgb = saturate(OUT.Color.rgb);

	OUT.Color.a = specialMask;
	// gamma cheap approx
	OUT.Color.rgb = pow(OUT.Color.rgb, 1.0 / 2.2 );

	// Apply dither pattern (prevent any banding artefacts from tone map stage)
	float fDither = cGrain.w * 2.0h - 1.0h;
	OUT.Color.rgb = floor(OUT.Color.rgb * 255.0 + fDither) / 255.0;

	// Apply color chart
	TexColorChart2D(colorChartMap, OUT.Color.xyz);

	return OUT;
}

float4 ElapsedTime;

pixout_fp HDRCalculateAdaptedLumPS(vert2frag IN)
{
	pixout_fp OUT = (pixout_fp) 0;

	float4 vAdaptedLum = GetLuminanceMap(lumMap0, 0.5h);
	float4 vCurrentLum = GetLuminanceMap(lumMap1, 0.5h);

	// Check for bad fp conditions
	if( vCurrentLum.x * ElapsedTime.w != 0.0h )
		vCurrentLum=1.0f;

	if( vAdaptedLum.x * ElapsedTime.w != 0.0h )
		vAdaptedLum=1.0f;

	float4 vNewAdaptation = max(0, vAdaptedLum + (vCurrentLum - vAdaptedLum) *  ElapsedTime.yzzz);
	OUT.Color = vNewAdaptation;

	return OUT;
}


float4 PI_sunShaftsParams;
float4x4 SunShafts_ViewProj;
float4 SunShafts_SunPos;
float4 SunShafts_SunCol;
float4 RainColor;
float4x4 mColorMatrix;
float4 ScratchParams;

float4 getDepth(float2 Tex)
{
	float sceneDepth = tex2D(zMap, Tex).x;

	return float4(sceneDepth, sceneDepth, sceneDepth, 1 - sceneDepth.x);
}

float4 getSimpleDepth(float2 Tex)
{
	return tex2D(zMap, Tex);
}


pixout SunShaftsIteratePS(vert2frag IN)
{
	pixout OUT;

	// just get the sun pos on screen
	float2 sunPosProj = SunShafts_SunPos.xy; //((IN.sunPos.xy / IN.sunPos.z));
	//sunPosProj = float2(0.7f, 0.2f);

	float fSign = SunShafts_SunPos.w; //(IN.sunPos.w);
	//fSign = 0.8f;

	float2 sunVec = ( sunPosProj.xy - IN.baseTC.xy);

	float fAspectRatio =  1.333 * PS_ScreenSize.y /PS_ScreenSize.x;

	float sunDist = saturate(fSign) * saturate( 1 - saturate(length(sunVec * float2(1, fAspectRatio))*PI_sunShaftsParams.y));// * 
	//saturate(saturate(fSign)*0.6+0.4  ) );
	// *(1.0 - 0.2*(1- sin(AnimGenParams) ) pass variation per constant
	float2 sunDir =  ( sunPosProj.xy - IN.baseTC.xy);


	float4 accum = 0; 
	sunDir.xy *= PI_sunShaftsParams.x * (fSign);
	//sunDir.xy *= 1.0f * (fSign);

	float2 jitter = 0;

	float4 depth = getDepth( IN.baseTC.xy );      
	accum += depth;


	depth = getSimpleDepth( IN.baseTC.xy + sunDir.xy * (1.0+jitter) );      
	accum += depth * (1.0-1.0/8.0);

	depth = getSimpleDepth(IN.baseTC.xy + sunDir.xy * (2.0+jitter) );      
	accum += depth * (1.0-2.0/8.0);

	depth = getSimpleDepth(IN.baseTC.xy + sunDir.xy * (3.0+jitter) );      
	accum += depth * (1.0-3.0/8.0);

	depth = getSimpleDepth(IN.baseTC.xy + sunDir.xy * (4.0+jitter) );      
	accum += depth * (1.0-4.0/8.0);

	depth = getSimpleDepth(IN.baseTC.xy + sunDir.xy * (5.0+jitter) );      
	accum += depth * (1.0-5.0/8.0);

	depth = getSimpleDepth(IN.baseTC.xy + sunDir.xy * (6.0+jitter) );      
	accum += depth * (1.0-6.0/8.0);

	depth = getSimpleDepth(IN.baseTC.xy + sunDir.xy * (7.0+jitter) );      
	accum += depth * (1.0-7.0/8.0);

	accum /= 8.0;

	//OUT.Color = accum * 2 * float4(sunDist.xxx, 1);
	OUT.Color = accum.x * 2 * sunDist.x;
	//OUT.Color.w += 1.0 - saturate( saturate( fSign * 0.1 + 0.9 ) );
	//OUT.Color.w = 1;

	return OUT;
}


pixout SunShaftsGenPS(vert2frag IN)
{
	pixout OUT;

	// just get the sun pos on screen
	float2 sunPosProj = SunShafts_SunPos.xy; //((IN.sunPos.xy / IN.sunPos.z));
	//sunPosProj = float2(0.7f, 0.2f);

	float fSign = SunShafts_SunPos.w; //(IN.sunPos.w);
	//fSign = 0.8f;

	float2 sunVec = ( sunPosProj.xy - IN.baseTC.xy);

	float fAspectRatio =  1.333 * PS_ScreenSize.y /PS_ScreenSize.x;

	float sunDist = saturate(fSign) * saturate( 1 - saturate(length(sunVec * float2(1, fAspectRatio))*PI_sunShaftsParams.y));// * 
	//saturate(saturate(fSign)*0.6+0.4  ) );
	// *(1.0 - 0.2*(1- sin(AnimGenParams) ) pass variation per constant
	float2 sunDir =  ( sunPosProj.xy - IN.baseTC.xy);


	float4 accum = 0; 
	sunDir.xy *= PI_sunShaftsParams.x * (fSign);
	//sunDir.xy *= 1.0f * (fSign);

	float2 jitter = 0;

	float4 depth = getDepth( IN.baseTC.xy );      
	accum += depth;


	depth = getDepth( IN.baseTC.xy + sunDir.xy * (1.0+jitter) );      
	accum += depth * (1.0-1.0/8.0);

	depth = getDepth(IN.baseTC.xy + sunDir.xy * (2.0+jitter) );      
	accum += depth * (1.0-2.0/8.0);

	depth = getDepth(IN.baseTC.xy + sunDir.xy * (3.0+jitter) );      
	accum += depth * (1.0-3.0/8.0);

	depth = getDepth(IN.baseTC.xy + sunDir.xy * (4.0+jitter) );      
	accum += depth * (1.0-4.0/8.0);

	depth = getDepth(IN.baseTC.xy + sunDir.xy * (5.0+jitter) );      
	accum += depth * (1.0-5.0/8.0);

	depth = getDepth(IN.baseTC.xy + sunDir.xy * (6.0+jitter) );      
	accum += depth * (1.0-6.0/8.0);

	depth = getDepth(IN.baseTC.xy + sunDir.xy * (7.0+jitter) );      
	accum += depth * (1.0-7.0/8.0);

	accum /= 8.0;

	//OUT.Color = accum * 2 * float4(sunDist.xxx, 1);
	OUT.Color = accum.x * 2 * sunDist.x;
	//OUT.Color.w += 1.0 - saturate( saturate( fSign * 0.1 + 0.9 ) );
	//OUT.Color.w = 1;

	return OUT;
}

//===================================================================================

void FogPassCommon (in vert2frag IN, out float sceneDepth, out float4 localFogColor, out float3 worldPos, out float3 cameraToWorldPos)
{
	float2 sDepthRG;
	float devDepth;

	//sceneDepth = tex2D(zMap, IN.baseTC.xy).z;
	worldPos = 0;

	float4 worldpos = GetWorldPos(zMap, IN.baseTC.xy, IN.farVerts.xyz );
	float4 ScenePos = float4(worldpos.xyz, 1);
	float water = ScenePos.z;
	ScenePos -= g_mViewI[3];

	cameraToWorldPos = ScenePos.xyz;//sceneDepth * IN.CamVec.xyz;
	sceneDepth = worldpos.w;
	//worldPos = cameraToWorldPos + vfViewPos.xyz;

	localFogColor = GetVolumetricFogColor(worldPos, cameraToWorldPos, devDepth);

	// 	if( water < -0.005f )
	// 	{
	// 		localFogColor.rgb *= (10.0 - water) / 30.0;
	// 		localFogColor.a = saturate(0.982 - pow(water / 1000, 0.1));
	// 	}
}

pixout FogPassPS(vert2frag IN)
{
	pixout OUT;

	float sceneDepth;
	float4 localFogColor;
	float3 worldPos, cameraToWorldPos;

	FogPassCommon(IN, sceneDepth, localFogColor, worldPos, cameraToWorldPos);
	localFogColor.a = 1.0 - localFogColor.a;

	// Re-scale range
	//localFogColor.xyz *= HDRParams2.y;

	HDROutput(OUT, localFogColor, 1);

	//OUT.Color.a = 0;
	return OUT;
}

///////////////// pixel shader //////////////////
float GetDepthBlurinessBiased(float fDepth)
{
	float f=0; 

	// 0 - in focus
	// 1 or -1 - completely out of focus

	// 如果大于了景深后片
	if(fDepth>(float)HDRDofParams.y)
	{
		// here we out the far
		f=(fDepth-(float)HDRDofParams.y)/(float)HDRDofParams.z; // max range
		f=saturate(f);
	}
	// 如果小于了景深后片
	else if(fDepth<(float)HDRDofParams.x)
	{
		// here we out the near
		f=(1-fDepth/HDRDofParams.x)/HDRDofParams.w;  // min range
		f=saturate(f);

		f *= -1;
	}

	return f;
}

pixout BlendDOFPS(vert2frag IN)
{
	pixout OUT;  

	// 取得绝对深度 0 - far
	float depthNormalized = tex2D(zMap, IN.baseTC.xy).x * g_fScreenSize.w;
	float depth = (GetDepthBlurinessBiased(depthNormalized))*HDRDofParams.w;	  


	float3 cScreen = max( tex2D(_tex1, IN.baseTC.xy).xyz, 0);


	//cScreen = max( min( cScreen, (float3)10000000 ), 0 );
	// do same NAN check as in hdr pass
	//cScreen.rgb = (cScreen.rgb> 10000.0f)? float3(1, 1, 1): cScreen.rgb;

	OUT.Color.xyzw = float4( cScreen.xyz, abs(depth) );
	//OUT.Color.w = 0.5;
	return OUT;
}

pixout CopyDepthToAlphaNoMaskPS(vert2frag IN)
{
	pixout OUT;  

	// 取得绝对深度 0 - far
	float depthNormalized = tex2D(zMap, IN.baseTC.xy).x * g_fScreenSize.w;
	float depth = (GetDepthBlurinessBiased(depthNormalized))*HDRDofParams.w;	  


	float3 cScreen = max( tex2D(_tex1, IN.baseTC.xy).xyz, 0);


	//cScreen = max( min( cScreen, (float3)10000000 ), 0 );
	// do same NAN check as in hdr pass
	cScreen.rgb = (cScreen.rgb> 10000.0f)? float3(1, 1, 1): cScreen.rgb;

	OUT.Color.xyzw = float4( cScreen.xyz, saturate(depth*0.5+0.5) );
	//OUT.Color.w = 0.5;
	return OUT;
}

technique HDRSampleLumInitial
{
	pass p0
	{
		VertexShader = null;

		CullMode = none;
		Zenable = false;
		ZWriteEnable = false;

		PixelShader = compile ps_3_0 HDRSampleLumInitialPS();
	}
}

technique HDRSampleLumIterative
{
	pass p0
	{
		VertexShader = null;

		ZEnable = false;
		ZWriteEnable = false;
		CullMode = None;

		PixelShader = compile ps_3_0 HDRSampleLumIterativePS();
	}
}

technique HDRBrightPassFilter
{
	pass p0
	{
		VertexShader = null;

		ZEnable = false;
		ZWriteEnable = false;
		CullMode = None;

		PixelShader = compile ps_3_0 HDRBrightPassFilterPS();
	}
}

technique HDRBloomGen
{
	pass p0
	{
		VertexShader = null;

		ZEnable = false;
		ZWriteEnable = false;
		CullMode = None;

		PixelShader = compile ps_3_0 HDRBloomGenPS();
	}
}

technique HDRStreakInitial
{
	pass p0
	{
		VertexShader = null;

		ZEnable = false;
		ZWriteEnable = false;
		CullMode = None;

		PixelShader = compile ps_3_0 HDRStreakInitialPS();
	}
}

technique SunShaftsGen
{
	pass p0
	{
		VertexShader = null;

		ZEnable = false;
		ZWriteEnable = false;
		CullMode = None;

		PixelShader = compile ps_3_0 SunShaftsGenPS();
	}
}

technique SunShaftsIterate
{
	pass p0
	{
		VertexShader = null;

		ZEnable = false;
		ZWriteEnable = false;
		CullMode = None;

		PixelShader = compile ps_3_0 SunShaftsIteratePS();
	}
}

technique HDRCalculateAdaptedLum
{
	pass p0
	{
		VertexShader = null;

		ZEnable = false;
		ZWriteEnable = false;
		CullMode = None;

		PixelShader = compile ps_3_0 HDRCalculateAdaptedLumPS();
	}
}



technique HDRFinalScene
{
	pass p0
	{
		VertexShader = null;

		ZEnable = false;
		ZWriteEnable = false;
		CullMode = None;

		PixelShader = compile ps_3_0 HDRFinalScenePS();
	}
}

technique FogPass
{
	pass p0
	{
		VertexShader = null;

		ZEnable = false;
		ZWriteEnable = false;
		CullMode = None;

		AlphaTestEnable = false;
		AlphaBlendEnable = true;
		//BlendOp = ADD;
		SrcBlend = SRCALPHA;
		DestBlend = ONE;	

		PixelShader = compile ps_3_0 FogPassPS();
	}
}

////////////////// technique /////////////////////

technique CopyDepthToAlphaNoMask
{
	pass p0
	{        
		VertexShader = null;

		ZEnable = false;
		ZWriteEnable = false;
		CullMode = None;

		PixelShader = compile ps_3_0 CopyDepthToAlphaNoMaskPS();    
	}
}

technique BlendDOF
{
	pass p0
	{        
		VertexShader = null;

		ZEnable = false;
		ZWriteEnable = false;
		CullMode = None;

		PixelShader = compile ps_3_0 BlendDOFPS();    
	}
}

