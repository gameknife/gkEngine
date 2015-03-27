#include "StableHeader.h"
#include "gkMeshManager.h"
#include "gkMeshGLES2.h"

template<> gkMeshManager* Singleton<gkMeshManager>::ms_Singleton = 0;

gkMeshPtr gkMeshManager::ms_generalSphere;

gkMeshManager* gkMeshManager::getSingletonPtr(void)
{
	return ms_Singleton;
}

gkMeshManager& gkMeshManager::getSingleton(void)
{
	assert(ms_Singleton);
	return (*ms_Singleton);
}

gkMeshManager::gkMeshManager(void)
{
	ms_generalSphere = create(_T("/engine/assets/meshs/skySphere.obj"), _T("internal"));
}

gkMeshManager::~gkMeshManager(void)
{
	removeAll();
}

IResource* gkMeshManager::createImpl(const gkStdString& name, gkResourceHandle handle, const gkStdString& group, gkNameValuePairList* params)
{
	gkMeshGLES2* pMesh;
	pMesh = new gkMeshGLES2(this, name, handle, group, params);
	return pMesh;
}
