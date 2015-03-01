#include "RendererD3D9StableHeader.h"
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

gkShaderPtr gkShaderManager::ms_ReflGen;

gkShaderPtr gkShaderManager::ms_DeferredLighting;
gkShaderPtr gkShaderManager::ms_DeferredShading;
gkShaderPtr gkShaderManager::ms_AuxRenderer;

gkShaderPtr gkShaderManager::ms_DefaultRender;
gkShaderPtr gkShaderManager::ms_ParticleUpdate;

gkShaderPtr gkShaderManager::ms_SSRL;

gkShaderPtr gkShaderManager::ms_post_msaa;
gkShaderPtr gkShaderManager::ms_post_msaa_smaa1tx;
//gkShaderPtr gkShaderManager::ms_post_msaa_smaa2tx;

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
	ms_PostCommon = create(_T("ksCommonPost"), _T(""));

	ms_SSAO = create(_T("ksSSAO"), _T(""));

	ms_ShadowMaskGen = create(_T("ksShadowMaskGen"), _T(""));
	ms_ShadowStencilGen = create(_T("ksShadowStencilGen"), _T(""));

	ms_DeferredSnow = create(_T("ksDeferredSnow"), _T(""));

	ms_ShadowMapGen = create(_T("ksGenerateSM"), _T(""));

	ms_ReflGen = create(_T("ksReflGen"), _T(""));
	
	ms_SpecilEffects = create(_T("ksSpecilEffects"), _T(""));

	ms_DeferredLighting = create(_T("ksDeferredLighting"), _T(""));

	ms_DeferredShading = create(_T("ksDeferredShading"), _T(""));

	ms_AuxRenderer = create(_T("ksAux"), _T(""));

	ms_DefaultRender = create(_T("ksDefault"), _T(""));

	ms_ParticleUpdate = create(_T("ParticleUpdate"), _T(""));

	ms_SSRL = create(_T("ksSSRL.fx"), _T(""));

	ms_post_msaa = create(_T("post_msaa"), _T(""));

	ms_post_msaa_smaa1tx = create(_T("post_msaa@1"), _T("")); 

	//ms_post_msaa_smaa2tx = create(_T("post_msaa"), _T(""));
}

gkShaderManager::~gkShaderManager( void )
{
	removeAll();
}

IResource* gkShaderManager::createImpl( const gkStdString& name, gkResourceHandle handle, const gkStdString& group, gkNameValuePairList* params )
{
	gkShader* pShader;
	pShader = new gkShader(this, name, handle);
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