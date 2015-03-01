#include "RendererD3D9StableHeader.h"
#include "gkMeshManager.h"
#include "gkMesh.h"

template<> gkMeshManager* Singleton<gkMeshManager>::ms_Singleton = 0;

gkMeshPtr gkMeshManager::ms_generalSphere;

gkMeshManager* gkMeshManager::getSingletonPtr(void)
{
	return ms_Singleton;
}

gkMeshManager& gkMeshManager::getSingleton( void )
{
	assert( ms_Singleton );  
	return ( *ms_Singleton );  
}

gkMeshManager::gkMeshManager( void )
{
	ms_generalSphere = create(_T("/engine/assets/meshs/skySphere.obj"), _T("internal"));
}

gkMeshManager::~gkMeshManager( void )
{
	removeAll();
}

IResource* gkMeshManager::createImpl( const gkStdString& name, gkResourceHandle handle, const gkStdString& group, gkNameValuePairList* params )
{
	gkMesh* pMesh;
	pMesh = new gkMesh(this, name, handle, group, params);
	return pMesh;
}

void gkMeshManager::resetAll()
{
	gkResourceMap::iterator i, iend;
	i = m_mapResources.begin();
	iend = m_mapResources.end();
	for (; i != iend; ++i)
	{		
		static_cast<gkMeshPtr>(i->second)->onReset();
	}
}

void gkMeshManager::lostAll()
{
	gkResourceMap::iterator i, iend;
	i = m_mapResources.begin();
	iend = m_mapResources.end();
	for (; i != iend; ++i)
	{		
		static_cast<gkMeshPtr>(i->second)->onLost();
	}
}
