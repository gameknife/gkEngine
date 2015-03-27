#include "StableHeader.h"
#include "gkShaderManager.h"
#include "gkShader.h"

template<> gkShaderManager* Singleton<gkShaderManager>::ms_Singleton = 0;

gkShaderPtr gkShaderManager::ms_HDRProcess;
gkShaderPtr gkShaderManager::ms_PostCommon;
gkShaderPtr gkShaderManager::ms_SSAO;
gkShaderPtr gkShaderManager::ms_ShadowMaskGen;
gkShaderPtr gkShaderManager::ms_ShadowStencilGen;
gkShaderPtr gkShaderManager::ms_DeferredSnow;

gkShaderPtr gkShaderManager::ms_SpecilEffects;

gkShaderPtr gkShaderManager::ms_ShadowMapGen;
gkShaderPtr gkShaderManager::ms_GeneralZpass;
gkShaderPtr gkShaderManager::ms_GeneralZpassSkinned;

gkShaderPtr gkShaderManager::ms_ReflGen;

gkShaderPtr gkShaderManager::ms_ShowDepth;
gkShaderPtr gkShaderManager::ms_AuxRenderer;
gkShaderPtr gkShaderManager::ms_AuxRenderer_Tex;

gkShaderPtr gkShaderManager::ms_DefaultRender;


gkShaderPtr gkShaderManager::ms_postcopy;
gkShaderPtr gkShaderManager::ms_postbrightpass;
gkShaderPtr gkShaderManager::ms_postbloom;
gkShaderPtr gkShaderManager::ms_postbloomdof;
gkShaderPtr	gkShaderManager::ms_postgaussian;
gkShaderPtr	gkShaderManager::ms_postmsaa;

gkShaderPtr gkShaderManager::ms_postcvtlineardepth;
gkShaderPtr gkShaderManager::ms_postssao;
gkShaderPtr gkShaderManager::ms_postforprocess;
gkShaderPtr gkShaderManager::ms_DeferredLighting_Ambient;
gkShaderPtr gkShaderManager::ms_DeferredLighting_PointLight;
gkShaderPtr gkShaderManager::ms_Simple;

gkShaderManager* gkShaderManager::getSingletonPtr(void)
{
	return ms_Singleton;
}

gkShaderManager& gkShaderManager::getSingleton(void)
{
	assert(ms_Singleton);
	return (*ms_Singleton);
}

gkShaderManager::gkShaderManager(void)
{
	ms_PostCommon = create(_T("postcommon"), _T(""));

	ms_ShadowMapGen = create(_T("ksgeneratesm"), _T(""));

	ms_AuxRenderer = create(_T("ksaux@0"), _T(""));
	ms_AuxRenderer_Tex = create(_T("ksaux@1"), _T(""));

	ms_postcopy = create(_T("post_copy"), _T(""));
	ms_postbrightpass = create(_T("post_brightpass"), _T(""));
	ms_postbloom = create(_T("post_bloom@0"), _T(""));
	ms_postbloomdof = create(_T("post_bloom@1"), _T(""));
	ms_postgaussian = create(_T("post_gaussian"), _T(""));

	ms_postcvtlineardepth = create(_T("post_cvtlineardepth"), _T(""));
	ms_postssao = create(_T("post_ssao"), _T(""));
	ms_postforprocess = create(_T("post_fogprocess"), _T(""));

	ms_ShadowMaskGen = create(_T("ksshadowmaskgen"), _T(""));

	ms_DeferredLighting_Ambient = create(_T("ksdeferredlighting@0"), _T(""));
	ms_DeferredLighting_PointLight = create(_T("ksdeferredlighting@1"), _T(""));

	ms_Simple = create(_T("kssimple"), _T(""));

	ms_postmsaa = create(_T("post_msaa"), _T(""));
}

gkShaderManager::~gkShaderManager(void)
{
	removeAll();
}

IResource* gkShaderManager::createImpl(const gkStdString& name, gkResourceHandle handle, const gkStdString& group, gkNameValuePairList* params)
{
	gkShaderGLES2* pShader;
	pShader = new gkShaderGLES2(this, name, handle);
	return pShader;
}
//-----------------------------------------------------------------------
void gkShaderManager::resetAll()
{
	gkResourceMap::iterator i, iend;
	i = m_mapResources.begin();
	iend = m_mapResources.end();
	for (; i != iend; ++i)
	{
		static_cast<gkShaderPtr>(i->second)->onReset();
	}
}
//-----------------------------------------------------------------------
void gkShaderManager::lostAll()
{
	gkResourceMap::iterator i, iend;
	i = m_mapResources.begin();
	iend = m_mapResources.end();
	for (; i != iend; ++i)
	{
		static_cast<gkShaderPtr>(i->second)->onLost();
	}
}