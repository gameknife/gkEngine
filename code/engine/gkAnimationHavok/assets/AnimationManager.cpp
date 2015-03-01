#include "StableHeader.h"
#include "AnimationManager.h"
#include <Animation/Animation/hkaAnimationContainer.h>
#include <Animation/Animation/Playback/Control/Default/hkaDefaultAnimationControl.h>
#include <Animation/Animation/Playback/hkaAnimatedSkeleton.h>
#include <Common/Serialize/Util/hkLoader.h>
#include <Common/Serialize/Util/hkRootLevelContainer.h>
#include "../utils/gkHavokAssetManagementUtil.h"

//////////////////////////////////////////////////////////////////////////
hkaAnimationBinding* gkAnimationManager::loadAnimation( const TCHAR* name )
{
	// check if we had one
	AnimationBindingMap::iterator it = m_mapAnimations.find(name);

	if (it != m_mapAnimations.end())
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
    if(!container)
        return NULL;
	HK_ASSERT2(0x27343437, container != HK_NULL , "Could not load asset");
	hkaAnimationContainer* ac = reinterpret_cast<hkaAnimationContainer*>( container->findObjectByType( hkaAnimationContainerClass.getName() ));

	HK_ASSERT2(0x27343435, ac && (ac->m_bindings.getSize() > 0), "No binding loaded");
	hkaAnimationBinding* binding = ac->m_bindings[0];

	m_mapAnimations.insert( AnimationBindingMap::value_type(name, binding) );

	return binding;
}
//////////////////////////////////////////////////////////////////////////
gkAnimationManager::gkAnimationManager()
{

}
//////////////////////////////////////////////////////////////////////////
gkAnimationManager::~gkAnimationManager()
{

}
//////////////////////////////////////////////////////////////////////////
void gkAnimationManager::Init()
{

}

//////////////////////////////////////////////////////////////////////////
void gkAnimationManager::Destroy()
{

}

