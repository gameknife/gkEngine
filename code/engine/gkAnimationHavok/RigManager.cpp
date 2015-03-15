#include "StableHeader.h"
#include "RigManager.h"

#include <Animation/Animation/hkaAnimationContainer.h>
#include <Animation/Animation/Rig/hkaPose.h>
#include <Common/Serialize/Util/hkLoader.h>
#include <Common/Serialize/Util/hkRootLevelContainer.h>
#include "gkHavokAssetManagementUtil.h"

//////////////////////////////////////////////////////////////////////////
hkaSkeleton* gkRigManager::loadRig( const TCHAR* name )
{

	// check if we had one
	RigMap::iterator it = m_mapRigs.find(name);

	if (it != m_mapRigs.end())
	{
		return it->second;
	}

	CHAR szPath[MAX_PATH] = "";
#ifdef UNICODE
	WideCharToMultiByte(CP_ACP, 0,  name, -1, szPath, MAX_PATH, NULL, NULL);
#else
	_tcscpy_s( szPath, MAX_PATH, name );
#endif

	hkStringBuf assetFile(szPath); hkAssetManagementUtil::getFilePath(assetFile);
	hkRootLevelContainer* container = getAnimationPtr()->getGlobalLoader()->load( szPath );
	HK_ASSERT2(0x27343437, container != HK_NULL , "Could not load asset");

	if (!container)
	{
		return false;
	}

	hkaAnimationContainer* ac = reinterpret_cast<hkaAnimationContainer*>( container->findObjectByType( hkaAnimationContainerClass.getName() ));

	HK_ASSERT2(0x27343435, ac && (ac->m_skeletons.getSize() > 0), "No skeleton loaded");
	hkaSkeleton* skeleton = ac->m_skeletons[0];

	m_mapRigs.insert(RigMap::value_type(name, skeleton));

	return skeleton;
}
//////////////////////////////////////////////////////////////////////////
gkRigManager::gkRigManager()
{

}

//////////////////////////////////////////////////////////////////////////
gkRigManager::~gkRigManager()
{
		
}
//////////////////////////////////////////////////////////////////////////
void gkRigManager::Init()
{

}
//////////////////////////////////////////////////////////////////////////
void gkRigManager::Destroy()
{

}
