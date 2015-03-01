#include "StableHeader.h"
#include "gkMaterialManager.h"
#include "gkMaterial.h"

template<> gkMaterialManager* Singleton<gkMaterialManager>::ms_Singleton = 0;

gkMaterialPtr gkMaterialManager::ms_DefaultMaterial;

gkMaterialManager* gkMaterialManager::getSingletonPtr(void)
{
	return ms_Singleton;
}

gkMaterialManager& gkMaterialManager::getSingleton( void )
{
	assert( ms_Singleton );  
	return ( *ms_Singleton );  
}

gkMaterialManager::gkMaterialManager( void )
{
	ms_DefaultMaterial = create(IMAT_DEFALUTNAME, _T("internal"));
}

gkMaterialManager::~gkMaterialManager( void )
{
	removeAll();
}

IResource* gkMaterialManager::createImpl( const gkStdString& name, gkResourceHandle handle, const gkStdString& group, gkNameValuePairList* params )
{
	gkMaterial* pMaterial;
	pMaterial = new gkMaterial(this, name, handle, group, params);
	return pMaterial;
}

void gkMaterialManager::saveAllMaterial()
{
	gkResourceMap::iterator i, iend;
	i = m_mapResources.begin();
	iend = m_mapResources.end();
	for (; i != iend; ++i)
	{		
		gkMaterialPtr pMat = static_cast<gkMaterialPtr>(i->second);
		gkStdStringstream wss;
		wss<<gkGetMaterialDir()<<'\\'<<pMat->getName()<<".mtl";
		//pMat->saveToMtlFile(wss.str().c_str());
		pMat->saveToMtlFile();
	}	
}
