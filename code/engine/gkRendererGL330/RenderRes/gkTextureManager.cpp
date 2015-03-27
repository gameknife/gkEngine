#include "StableHeader.h"
#include "gkTextureManager.h"
#include "gkTextureGLES2.h"

// template<> gkTextureManager* Singleton<gkTextureManager>::ms_Singleton = 0;
// 
// gkTextureManager* gkTextureManager::getSingletonPtr(void)
// {
// 	return ms_Singleton;
// }
// 
// gkTextureManager& gkTextureManager::getSingleton( void )
// {
// 	assert( ms_Singleton );  
// 	return ( *ms_Singleton );  
// }

#define ENABLE_HDR_FORMAT 0

gkTexturePtr gkTextureManager::ms_ReflMap0;

gkTexturePtr gkTextureManager::ms_BackBuffer;
gkTexturePtr gkTextureManager::ms_BackBufferHolder;
gkTexturePtr gkTextureManager::ms_BackBufferA_LeftEye;
gkTexturePtr gkTextureManager::ms_BackBufferB_LeftEye;
gkTexturePtr gkTextureManager::ms_SceneNormal;
gkTexturePtr gkTextureManager::ms_SceneDepth;

gkTexturePtr gkTextureManager::ms_SceneDepth_Linear;
gkTexturePtr gkTextureManager::ms_SceneDepth_Linear_Half;

gkTexturePtr gkTextureManager::ms_SceneTargetBlur;
gkTexturePtr gkTextureManager::ms_SceneTargetBlurTmp;

gkTexturePtr gkTextureManager::ms_BackBufferQuad;
gkTexturePtr gkTextureManager::ms_BackBufferQuadTmp;

gkTexturePtr gkTextureManager::ms_ShadowCascade0;
gkTexturePtr gkTextureManager::ms_ShadowCascade1;
gkTexturePtr gkTextureManager::ms_ShadowCascade2;

gkTexturePtr gkTextureManager::ms_HDRTarget0;

gkTexturePtr gkTextureManager::ms_SSAOTarget;
gkTexturePtr gkTextureManager::ms_ShadowMask;

gkTexturePtr gkTextureManager::ms_SceneDifAcc;
gkTexturePtr gkTextureManager::ms_SceneSpecAcc;


//////////////////////////////////////////////////////////////////////////
// HDR pipe
gkTexturePtr gkTextureManager::ms_HDRTargetScaledHalf;
gkTexturePtr gkTextureManager::ms_HDRTargetScaledHalfBlur; // for dof
gkTexturePtr gkTextureManager::ms_HDRTargetScaledQuad; // for tonemap

// tonemap
gkTexturePtr gkTextureManager::ms_HDRToneMap64;
gkTexturePtr gkTextureManager::ms_HDRToneMap16;
gkTexturePtr gkTextureManager::ms_HDRToneMap04;
gkTexturePtr gkTextureManager::ms_HDRToneMap01;

gkTexturePtr gkTextureManager::ms_HDRBrightPassQuad;
gkTexturePtr gkTextureManager::ms_HDRBrightPassQuadTmp;
gkTexturePtr gkTextureManager::ms_HDRBrightPassQuadQuad;
gkTexturePtr gkTextureManager::ms_HDRBrightPassQuadQuadTmp;
gkTexturePtr gkTextureManager::ms_HDRBrightPassQuadQuadQuad;
gkTexturePtr gkTextureManager::ms_HDRBrightPassQuadQuadQuadTmp;

gkTexturePtr gkTextureManager::ms_HDRBloom0Quad;
gkTexturePtr gkTextureManager::ms_HDRBloom1Quad;
gkTexturePtr gkTextureManager::ms_HDRStreak;

gkTexturePtr gkTextureManager::ms_HDREyeAdaption[8];

gkTexturePtr gkTextureManager::ms_OutGlowMask;
gkTexturePtr gkTextureManager::ms_OutGlowMaskHalf;
gkTexturePtr gkTextureManager::ms_OutGlowMaskHalfSwap;

gkTexturePtr gkTextureManager::ms_DefaultColorChart;

gkTexturePtr gkTextureManager::ms_PrevColorChart;
gkTexturePtr gkTextureManager::ms_CurrColorChart;
gkTexturePtr gkTextureManager::ms_MergedColorChart;

gkTexturePtr gkTextureManager::ms_DefaultDiffuse;
gkTexturePtr gkTextureManager::ms_DefaultNormal;
gkTexturePtr gkTextureManager::ms_RotSamplerAO;

gkTextureManager::gkTextureManager(void)
{
	//return;
	// maybe way can create some static texture here [10/20/2011 Kaiming]
	gkNameValuePairList createlist;

	// set params
	createlist[_T("d3dpool")] = _T("D3DX_DEFAULT");
	createlist[_T("usage")] = _T("RENDERTARGET");
	createlist[_T("size")] = _T("full");
	createlist[_T("format")] = _T("D32F");

	// GBUFFER : full size ABGR16F
	ms_SceneDepth = create(_T("RT_SCENETARGET2"), _T("dyntex"), &createlist);

	createlist[_T("size")] = _T("full");

	createlist[_T("format")] = _T("A8R8G8B8");
	if (ENABLE_HDR_FORMAT)
	{
		createlist[_T("format")] = _T("A16B16G16R16F");
	}
	ms_HDRTarget0 = create(_T("RT_HDRTARGET0"), _T("dyntex"), &createlist);



	// full size ARGB8
	//createlist[_T("format")] =		_T("A16B16G16R16F");
	createlist[_T("format")] = _T("A8R8G8B8");
	createlist[_T("size")] = _T("full");

	// LOW NORMAL
	ms_SceneNormal = create(_T("RT_SCENETARGET1"), _T("dyntex"), &createlist);

	createlist[_T("format")] = _T("A8R8G8B8");
	createlist[_T("size")] = _T("full");

	// LIGHTBUFFER
	ms_SceneDifAcc = create(_T("RT_SCENEDIFACC"), _T("dyntex"), &createlist);
	ms_SceneSpecAcc = create(_T("RT_SCENESPECACC"), _T("dyntex"), &createlist);

	createlist[_T("format")] = _T("R32F");
	ms_SceneDepth_Linear = create(_T("RT_SCENEDEPTHLINEAR"), _T("dyntex"), &createlist);

	createlist[_T("size")] = _T("half");
	ms_SceneDepth_Linear_Half = create(_T("RT_SCENEDEPTHLINEAR_HALF"), _T("dyntex"), &createlist);

	createlist[_T("format")] = _T("A8R8G8B8");

	createlist[_T("size")] = _T("full");
	ms_BackBufferB_LeftEye = create(_T("RT_BACKBUFFERB"), _T("dyntex"), &createlist);
	ms_BackBufferA_LeftEye = create(_T("RT_BACKBUFFERA"), _T("dyntex"), &createlist);
	ms_BackBufferHolder = create(_T("RT_BACKBUFFERHOLDER"), _T("dyntex"), &createlist);

	// HDRBLUR BACKUP
	createlist[_T("size")] = _T("half");
	ms_SceneTargetBlur = create(_T("RT_SCENETARGET0BLUR"), _T("dyntex"), &createlist);
	ms_SceneTargetBlurTmp = create(_T("RT_SCENETARGET0TMP"), _T("dyntex"), &createlist);

	// R16F: for SunShaft Mask etc
	createlist[_T("format")] = _T("R16F");
	createlist[_T("size")] = _T("quad");
	ms_BackBufferQuad = create(_T("RT_BACKBUFFER_QUAD"), _T("dyntex"), &createlist);
	ms_BackBufferQuadTmp = create(_T("RT_BACKBUFFER_QUADTMP"), _T("dyntex"), &createlist);

	// ShadowMask & SSAO Target
	createlist[_T("size")] = _T("full");
	// R16F
	createlist[_T("format")] = _T("R16F");
	ms_ShadowMask = create(_T("RT_SHADOWMASK"), _T("dyntex"), &createlist);
	//createlist[_T("size")] =		_T("half");
	ms_SSAOTarget = create(_T("RT_SSAOTARGET"), _T("dyntex"), &createlist);


	// Refl Map
	createlist[_T("d3dpool")] = _T("D3DX_DEFAULT");
	createlist[_T("usage")] = _T("RENDERTARGET");
	createlist[_T("size")] = _T("full");
	createlist[_T("format")] = _T("A8R8G8B8");
	ms_ReflMap0 = create(_T("RT_REFLMAP"), _T("dyntex"), &createlist);

	createlist[_T("size")] = _T("full");
	ms_OutGlowMask = create(_T("RT_OUTGLOWMASK"), _T("dyntex"), &createlist);

	createlist[_T("size")] = _T("quad");
	ms_OutGlowMaskHalf = create(_T("RT_OUTGLOWMASK_HALF"), _T("dyntex"), &createlist);
	ms_OutGlowMaskHalfSwap = create(_T("RT_OUTGLOWMASK_HALFSWAP"), _T("dyntex"), &createlist);



	// HDR pipes
	createlist[_T("d3dpool")] = _T("D3DX_DEFAULT");
	createlist[_T("usage")] = _T("RENDERTARGET");
	createlist[_T("size")] = _T("half");

	createlist[_T("format")] = _T("A8R8G8B8");
	if (ENABLE_HDR_FORMAT)
	{
		createlist[_T("format")] = _T("A16B16G16R16F");
	}
	ms_HDRTargetScaledHalf = create(_T("RT_HDRTARGET_HALF"), _T("dyntex"), &createlist);
	ms_HDRTargetScaledHalfBlur = create(_T("RT_HDRTARGET_HALF_BLURED"), _T("dyntex"), &createlist);

	createlist[_T("size")] = _T("quad");
	ms_HDRBrightPassQuad = create(_T("RT_HDRBRIGHPASS_QUAD"), _T("dyntex"), &createlist);
	ms_HDRBloom0Quad = create(_T("RT_HDRBLOOM0_QUAD"), _T("dyntex"), &createlist);
	ms_HDRBloom1Quad = create(_T("RT_HDRBLOOM1_QUAD"), _T("dyntex"), &createlist);
	ms_HDRBrightPassQuadTmp = create(_T("RT_HDRBRIGHPASS_QUADTMP"), _T("dyntex"), &createlist);


	createlist[_T("size")] = _T("quadquad");
	ms_HDRBrightPassQuadQuad = create(_T("RT_HDRBRIGHPASS_QUADQUAD"), _T("dyntex"), &createlist);
	ms_HDRBrightPassQuadQuadTmp = create(_T("RT_HDRBRIGHPASS_QUADQUADTMP"), _T("dyntex"), &createlist);

	ms_HDRStreak = create(_T("RT_HDRSTREAK0_QUAD"), _T("dyntex"), &createlist);

	createlist[_T("size")] = _T("quadquadquad");
	ms_HDRBrightPassQuadQuadQuad = create(_T("RT_HDRBRIGHPASS_QUADQUADQUAD"), _T("dyntex"), &createlist);
	ms_HDRBrightPassQuadQuadQuadTmp = create(_T("RT_HDRBRIGHPASS_QUADQUADQUADTMP"), _T("dyntex"), &createlist);


	createlist[_T("size")] = _T("quad");
	ms_HDRTargetScaledQuad = create(_T("RT_HDRTARGET_QUAD"), _T("dyntex"), &createlist);

	createlist[_T("size")] = _T("64");
	createlist[_T("format")] = _T("A16B16G16R16F");
	createlist[_T("format")] = _T("G16R16F");
	ms_HDRToneMap64 = create(_T("RT_HDRTONEMAP64"), _T("dyntex"), &createlist);
	createlist[_T("size")] = _T("16");
	ms_HDRToneMap16 = create(_T("RT_HDRTONEMAP16"), _T("dyntex"), &createlist);
	createlist[_T("size")] = _T("4");
	ms_HDRToneMap04 = create(_T("RT_HDRTONEMAP04"), _T("dyntex"), &createlist);
	createlist[_T("size")] = _T("1");
	ms_HDRToneMap01 = create(_T("RT_HDRTONEMAP01"), _T("dyntex"), &createlist);

	for (int i = 0; i < 8; ++i)
	{
		createlist[_T("d3dpool")] = _T("D3DX_DEFAULT");
		createlist[_T("usage")] = _T("RENDERTARGET");
		createlist[_T("size")] = _T("1");
		createlist[_T("format")] = _T("A16B16G16R16F");
		gkStdStringstream wss;
		wss << _T("RT_HDREYEADAPT") << i;

		ms_HDREyeAdaption[i] = create(wss.str().c_str(), _T("dyntex"), &createlist);
	}


	createlist[_T("d3dpool")] = _T("D3DX_DEFAULT");
	createlist[_T("usage")] = _T("RENDERTARGET");
	createlist[_T("size")] = _T("512");
	createlist[_T("format")] = _T("D32F");
	ms_ShadowCascade0 = create(_T("RT_SHADOWCASCADE0"), _T("dyntex"), &createlist);
	ms_ShadowCascade1 = create(_T("RT_SHADOWCASCADE1"), _T("dyntex"), &createlist);
	ms_ShadowCascade2 = create(_T("RT_SHADOWCASCADE2"), _T("dyntex"), &createlist);


	createlist[_T("d3dpool")] = _T("D3DX_DEFAULT");
	createlist[_T("usage")] = _T("RENDERTARGET");
	createlist[_T("size")] = _T("256");
	createlist[_T("sizey")] = _T("16");
	createlist[_T("format")] = _T("A8R8G8B8");
	ms_PrevColorChart = create(_T("RT_PREVCOLORCHART"), _T("dyntex"), &createlist);
	ms_CurrColorChart = create(_T("RT_CURRCOLORCHART"), _T("dyntex"), &createlist);
	ms_MergedColorChart = create(_T("RT_MERGEDCOLORCHART"), _T("dyntex"), &createlist);

	ms_DefaultColorChart = create(_T("engine/assets/textures/colorchart/default_colorchart.tga"), _T("internal"));

	ms_DefaultDiffuse = create(_T("engine/assets/textures/default/grey.tga"), _T("internal"));
	ms_DefaultNormal = create(_T("engine/assets/textures/default/flat.tga"), _T("internal"));

	ms_RotSamplerAO = create(_T("engine/assets/textures/procedure/PointsOnSphere4x4.tga"), _T("internal"));

}

gkTextureManager::~gkTextureManager(void)
{
	removeAll();
}

IResource* gkTextureManager::createImpl(const gkStdString& name, gkResourceHandle handle, const gkStdString& group, gkNameValuePairList* params)
{
	gkTextureGLES2* pTexture;
	pTexture = new gkTextureGLES2(this, name, handle, group, params);
	return pTexture;
}

void gkTextureManager::resetAll()
{
	gkResourceMap::iterator i, iend;
	i = m_mapResources.begin();
	iend = m_mapResources.end();
	for (; i != iend; ++i)
	{
		static_cast<gkTexturePtr>(i->second)->onReset();
	}
}

void gkTextureManager::lostAll()
{
	gkResourceMap::iterator i, iend;
	i = m_mapResources.begin();
	iend = m_mapResources.end();
	for (; i != iend; ++i)
	{
		static_cast<gkTexturePtr>(i->second)->onLost();
	}
}

