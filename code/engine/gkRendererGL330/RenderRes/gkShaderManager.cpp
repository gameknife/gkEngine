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

gkShaderPtr gkShaderManager::ms_postcvtlineardepth;
gkShaderPtr gkShaderManager::ms_postssao;
gkShaderPtr gkShaderManager::ms_postforprocess;

gkShaderManager* gkShaderManager::getSingletonPtr(void)
{
	return ms_Singleton;
}

gkShaderManager& gkShaderManager::getSingleton( void )
{
	assert( ms_Singleton );  
	return ( *ms_Singleton );  
}

gkShaderManager::gkShaderManager( void )
{
	ms_HDRProcess = create(_T("ksHDRProcess"), _T(""));
	ms_PostCommon = create(_T("postcommon"), _T(""));

	ms_SSAO = create(_T("ksSSAO"), _T(""));

	ms_ShadowMaskGen = create(_T("ksShadowMaskGen"), _T(""));
	ms_ShadowStencilGen = create(_T("ksShadowStencilGen"), _T(""));

	ms_DeferredSnow = create(_T("ksDeferredSnow"), _T(""));

	ms_ShadowMapGen = create(_T("ksgeneratesm"), _T(""));

	
	ms_GeneralZpass = create(_T("ksZpass"), _T(""));

	ms_GeneralZpassSkinned = create(_T("ksZpassSkinned"), _T(""));

	ms_ReflGen = create(_T("ksReflGen"), _T(""));
	
	ms_SpecilEffects = create(_T("ksSpecilEffects"), _T(""));

	ms_ShowDepth = create(_T("showdepthmap"), _T(""));

	ms_AuxRenderer = create(_T("ksaux"), _T(""));

    ms_AuxRenderer_Tex = create(_T("ksaux@1"), _T(""));
    
	ms_DefaultRender = create(_T("ksDefault"), _T(""));

	ms_postcopy = create(_T("post_copy"), _T(""));
	ms_postbrightpass = create(_T("post_brightpass"), _T(""));
	ms_postbloom = create(_T("post_bloom"), _T(""));
	ms_postbloomdof = create(_T("post_bloom"), _T(""));
	ms_postgaussian = create(_T("post_gaussian"), _T(""));

    ms_postcvtlineardepth = create(_T("post_cvtlineardepth"), _T(""));
    ms_postssao = create(_T("post_ssao"), _T(""));
    
    ms_postforprocess = create(_T("post_fogprocess"), _T("")  );
}

gkShaderManager::~gkShaderManager( void )
{
	removeAll();
}

IResource* gkShaderManager::createImpl( const gkStdString& name, gkResourceHandle handle, const gkStdString& group, gkNameValuePairList* params )
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