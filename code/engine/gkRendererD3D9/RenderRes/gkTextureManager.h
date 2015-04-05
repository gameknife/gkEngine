//////////////////////////////////////////////////////////////////////////
/*
Copyright (c) 2011-2015 Kaiming Yi
	
	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:
	
	The above copyright notice and this permission notice shall be included in
	all copies or substantial portions of the Software.
	
	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
	THE SOFTWARE.
	
*/
//////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////
//
// Name:   	gkTextureManager.h
// Desc:	todo...
// 描述:	
// 
// Author:  Kaiming-Laptop
// Date:	2010/9/17
// 
//////////////////////////////////////////////////////////////////////////
#ifndef gkTextureManager_h_434d73c0_a6ed_4b5e_afa3_6e7787163879
#define gkTextureManager_h_434d73c0_a6ed_4b5e_afa3_6e7787163879

#include "RendererD3D9Prerequisites.h"
#include "IResourceManager.h"
#include "ITexture.h"
//#include "gkSingleton.h"

class gkTextureManager : public IResourceManager//, public Singleton<gkTextureManager>
{
public:
// 	static gkTextureManager& getSingleton(void);
// 	static gkTextureManager* getSingletonPtr(void);

	gkTextureManager(void);
	virtual ~gkTextureManager(void);

	virtual void resizeall();
	virtual void resetAll();
	virtual void lostAll();

	//////////////////////////////////////////////////////////////////////////
	// scene staff
	static gkTexturePtr ms_SceneTarget0;
	static gkTexturePtr ms_BackBuffersStereoOut;
	static gkTexturePtr ms_BackBuffer;

	static gkTexturePtr ms_BackBuffer_Mono;

	static gkTexturePtr ms_BackBufferA_LeftEye;
	static gkTexturePtr ms_BackBufferB_LeftEye;

	static gkTexturePtr ms_SMAA_Edge;
	static gkTexturePtr ms_SMAA_Blend;

	static gkTexturePtr ms_BackBufferA_RightEye;
	static gkTexturePtr ms_BackBufferB_RightEye;

	static gkTexturePtr ms_SceneNormal;
	static gkTexturePtr ms_SceneDepth;
	static gkTexturePtr ms_SceneAlbeto;

	static gkTexturePtr ms_SceneTargetBlur;
	static gkTexturePtr ms_SceneTargetBlurTmp;

	static gkTexturePtr ms_HDRTarget0;
	

	static gkTexturePtr ms_BackBufferQuad;
	static gkTexturePtr ms_BackBufferQuadTmp;

	static gkTexturePtr ms_SSAOTargetTmp;
	static gkTexturePtr ms_SSAOTarget;
	static gkTexturePtr ms_ShadowMask;
	static gkTexturePtr ms_ShadowMaskBlur;
	// light buffer
	static gkTexturePtr ms_SceneDifAcc;
	static gkTexturePtr ms_SceneSpecAcc;

	// before water, opaque backup (refract map)
	static gkTexturePtr ms_SceneTargetTmp0;
	// reflect map
	static gkTexturePtr ms_ReflMap0;
	static gkTexturePtr ms_ReflMap0Tmp;
	// ssao 
	static gkTexturePtr ms_SSAOTargetHalf;
	static gkTexturePtr ms_SSAOBluredHalf;

	//////////////////////////////////////////////////////////////////////////
	// shadowmap
	static gkTexturePtr ms_ShadowCascade0;
	static gkTexturePtr ms_ShadowCascade1;
	static gkTexturePtr ms_ShadowCascade2;
	static gkTexturePtr ms_ShadowCascade_color;
	//////////////////////////////////////////////////////////////////////////
	// hdr pipeline static texture require [10/21/2011 Kaiming]
	
	// target scaled
	static gkTexturePtr ms_HDRTargetScaledHalf;
	static gkTexturePtr ms_HDRTargetScaledHalfBlur; // for dof
	static gkTexturePtr ms_HDRTargetScaledQuad; // for tonemap

	// tonemap
	static gkTexturePtr ms_HDRToneMap64;
	static gkTexturePtr ms_HDRToneMap16;
	static gkTexturePtr ms_HDRToneMap04;
	static gkTexturePtr ms_HDRToneMap01;

	// eyeadaptions
	static gkTexturePtr ms_HDREyeAdaption[8];

	// BrightPass
	static gkTexturePtr ms_HDRBrightPassQuad;
	static gkTexturePtr ms_HDRBrightPassQuadTmp;
	static gkTexturePtr ms_HDRBrightPassQuadQuad;
	static gkTexturePtr ms_HDRBrightPassQuadQuadTmp;
	static gkTexturePtr ms_HDRBrightPassQuadQuadQuad;
	static gkTexturePtr ms_HDRBrightPassQuadQuadQuadTmp;

	// Bloom
	static gkTexturePtr ms_HDRStreak;
	static gkTexturePtr ms_HDRBloom0Quad;
	static gkTexturePtr ms_HDRBloom1Quad;

	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// RT for special effect
	static gkTexturePtr ms_OutGlowMask;
	static gkTexturePtr ms_OutGlowMaskHalf;
	static gkTexturePtr ms_OutGlowMaskHalfSwap;

	

	static gkTexturePtr ms_PrevColorChart;
	static gkTexturePtr ms_CurrColorChart;
	static gkTexturePtr ms_MergedColorChart;

	static gkTexturePtr ms_DefaultColorChart;
	static gkTexturePtr ms_DefaultDiffuse;
	static gkTexturePtr ms_DefaultNormal;
	static gkTexturePtr ms_RotSamplerAO;

	static gkTexturePtr ms_DefaultWhite;

	static gkTexturePtr ms_Blend25[2];
	static gkTexturePtr ms_Blend50[2];

// 	static gkTexturePtr ms_TestCubeRT;
// 	static gkTexturePtr ms_TestCubeRTTmp;

protected:
	virtual IResource* createImpl(const gkStdString& name, gkResourceHandle handle, const gkStdString& group, gkNameValuePairList* params = NULL);

// 
// protected:
// 	virtual void 
};



#endif // gkTextureManager_h_434d73c0_a6ed_4b5e_afa3_6e7787163879