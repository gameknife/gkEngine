#include "RendererD3D9StableHeader.h"
#include "gkLightProbeSystem.h"

gkLightProbeSystem::gkLightProbeSystem(void)
{
	m_offline_gen_request = false;
}

gkLightProbeSystem::~gkLightProbeSystem(void)
{

}

void gkLightProbeSystem::Update()
{
	if(m_offline_gen_request)
	{
		getRenderer()->RP_GenCubemap( m_offline_gen_pos, m_tmp_cubemap, m_cubemap_filter_tmp, false, m_offline_gen_name.c_str() );
		m_offline_gen_request = false;
	}
	else
	{
// 		if (gEnv->pProfiler->getFrameCount() % 10)
// 		{
// 			getRenderer()->RP_GenCubemap(Vec3(0, 0, 1.2), m_tmp_cubemap, m_cubemap_filter_tmp, false);
// 		}
		
	}
}

gkTexturePtr gkLightProbeSystem::GetCubeMap(Vec3 position)
{
	return m_tmp_cubemap;
}

void gkLightProbeSystem::SavePositionCubeMap(Vec3 position, const TCHAR* texturename)
{
	m_offline_gen_pos = position;
	m_offline_gen_request = true;
	m_offline_gen_name = texturename;
}

void gkLightProbeSystem::Init()
{
	gkNameValuePairList createlist;

	createlist[_T("d3dpool")] =	_T("D3DX_DEFAULT");
	createlist[_T("cube")] =		_T("true");
	createlist[_T("usage")] =		_T("RENDERTARGET");
	createlist[_T("size")] =		_T("512");
	createlist[_T("sizey")] =		_T("512");
	createlist[_T("format")] =		_T("A8R8G8B8");

	m_tmp_cubemap = getRenderer()->getTextureMngPtr()->create(_T("RT_CUBEMAP_TMP"), _T("internal"), &createlist);
	m_cubemap_filter_tmp = getRenderer()->getTextureMngPtr()->create(_T("RT_CUBEMAP_FILTER_TMP"), _T("internal"), &createlist);
}

void gkLightProbeSystem::Destroy()
{
	m_tmp_cubemap->unload();
	m_cubemap_filter_tmp->unload();
}