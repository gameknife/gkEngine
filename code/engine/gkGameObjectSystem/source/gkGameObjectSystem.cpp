#include "StableHeader.h"
#include "gkGameObjectSystem.h"
#include "gkGameObject.h"
#include "IGameObjectLayer.h"
#include "I3DEngine.h"
#include "IXmlUtil.h"
#include "IHavok.h"
#include "IResourceManager.h"
#include "IMesh.h"
#include "IAnimation.h"


//////////////////////////////////////////////////////////////////////////
IGameObject* gkGameObjectSystem::CreateStaticGeoGameObject( const gkStdString& name, const gkStdString& meshfile, Vec3& pos, Quat& rot )
{
	gkGameObject* pGameObject = NULL;

	pGameObject = _createGameObjectInternal(name);
	

	IGameObjectRenderLayer* pRenderLayer = gEnv->p3DEngine->createRenderLayer(name, pos, rot);
	if (!pRenderLayer)
	{
		GK_ASSERT(0);
		return NULL;
	}
	pGameObject->setGameObjectLayer(pRenderLayer);

	gkMeshPtr pMesh = gEnv->pSystem->getMeshMngPtr()->load(meshfile);
	if (pMesh.isNull())
	{
		gkLogWarning(_T("gkGOSys::StaticGeo [ %s ] load mesh [ %s ], use default."), pGameObject->getName().c_str(), meshfile.c_str());
		pMesh = gEnv->pSystem->getMeshMngPtr()->load(_T("engine/assets/meshs/_default.obj"));
	}
	for ( uint32 i=0; i < pMesh->getSubsetCount(); ++i )
	{
		gkRenderable* pStatObj = gEnv->p3DEngine->createRenderable(pMesh, e3RT_StaticObj, i);
		pRenderLayer->modifySubRenderable(pStatObj, i);
	}
	

	pGameObject->setGameObjectSuperClass(eGOClass_STATICMESH);
	pGameObject->setGameObjectClass(_T("StaticGeo"));




	gkLogMessage(_T("gkGOSys::StaticGeo [ %s ] Created. %d SubSection."), pGameObject->getName().c_str(), pMesh->getSubsetCount());


	return pGameObject;
}

//////////////////////////////////////////////////////////////////////////
IGameObject* gkGameObjectSystem::CreateVoidGameObject( const gkStdString& name, Vec3& pos, Quat& rot )
{
	gkGameObject* pGameObject = NULL;

	pGameObject = _createGameObjectInternal(name);

	IGameObjectRenderLayer* pRenderLayer = gEnv->p3DEngine->createRenderLayer(name, pos, rot);
	if (!pRenderLayer)
	{
		GK_ASSERT(0);
		return NULL;
	}
	pGameObject->setGameObjectLayer(pRenderLayer);

	//_T("$NOTCREAT")
	gkMeshPtr pMesh = gEnv->pSystem->getMeshMngPtr()->load(_T("engine/assets/meshs/_default.obj"));
	gkRenderable* pStatObj = gEnv->p3DEngine->createRenderable(pMesh, e3RT_StaticObj, 0);
	pRenderLayer->modifySubRenderable(pStatObj, 0);
	

	
	pRenderLayer->setVisible(false);

	
	pGameObject->setGameObjectSuperClass(eGOClass_GROUP);
	pGameObject->setGameObjectClass(_T("Group"));

	gkLogMessage(_T("gkGOSys::%s [ %s ] Created."), pGameObject->getGameObjectClassName().c_str(), pGameObject->getName().c_str());


	return pGameObject;

}


IGameObject* gkGameObjectSystem::CreateParticleGameObject( const gkStdString& ptcFilename, const Vec3& pos, const Quat& rot )
{
	gkStdString newName = m_MovableNameGenerator->generate();
	return CreateParticleGameObject( newName, ptcFilename, pos, rot ); 
}

IGameObject* gkGameObjectSystem::CreateParticleGameObject( const gkStdString& name, const gkStdString& ptcFilename, const Vec3& pos, const Quat& rot )
{
	gkGameObject* pGameObject = NULL;

	pGameObject = _createGameObjectInternal(name);

	// RenderLayer
	IGameObjectRenderLayer* pRenderLayer = gEnv->p3DEngine->createRenderLayer(name, pos, rot);
	if (!pRenderLayer)
	{
		GK_ASSERT(0);
		return NULL;
	}
	pGameObject->setGameObjectLayer(pRenderLayer);

	gkMeshPtr pMesh = gEnv->pSystem->getMeshMngPtr()->load(_T("engine/assets/meshs/editorLight.obj"));
	gkRenderable* pStatObj = gEnv->p3DEngine->createRenderable(pMesh, e3RT_StaticObj, 0);
	pRenderLayer->modifySubRenderable(pStatObj, 0);
	//pRenderLayer->showBBox(true);
	pRenderLayer->setVisible(false);
	//pRenderLayer->setScale(lightRadius, lightRadius, lightRadius);
	//pRenderLayer->setMaterialName(_T("undefine"));

	pRenderLayer->setPosition( pos );
	pRenderLayer->setOrientation( rot );

	

	// ParticleLayer
	IGameObjectParticleLayer* pLightLayer = gEnv->p3DEngine->createParticleLayer(pGameObject, name, ptcFilename);
	pGameObject->setGameObjectLayer(pLightLayer);


	pGameObject->setGameObjectSuperClass(eGOClass_PARTICLE);
	pGameObject->setGameObjectClass(_T("Particle"));

	gkLogMessage(_T("gkGOSys::%s [ %s ] Created."), pGameObject->getGameObjectClassName().c_str(), pGameObject->getName().c_str());

	return pGameObject;
}

//////////////////////////////////////////////////////////////////////////
IGameObject* gkGameObjectSystem::CreateLightGameObject( const gkStdString& name, Vec3& lightpos, float lightRadius, ColorF& lightColor, bool isFakeShadow )
{
	gkGameObject* pGameObject = NULL;

	pGameObject = _createGameObjectInternal(name);

	// RenderLayer
	Quat zeroRot = Quat::CreateIdentity();
	IGameObjectRenderLayer* pRenderLayer = gEnv->p3DEngine->createRenderLayer(name, lightpos, zeroRot);
	if (!pRenderLayer)
	{
		GK_ASSERT(0);
		return NULL;
	}
	pGameObject->setGameObjectLayer(pRenderLayer);

	gkMeshPtr pMesh = gEnv->pSystem->getMeshMngPtr()->load(_T("engine/assets/meshs/editorLight.obj"));
		gkRenderable* pStatObj = gEnv->p3DEngine->createRenderable(pMesh, e3RT_StaticObj, 0);
		pRenderLayer->modifySubRenderable(pStatObj, 0);
		//pRenderLayer->showBBox(true);
		pRenderLayer->setVisible(false);
		//pRenderLayer->setScale(lightRadius, lightRadius, lightRadius);
		//pRenderLayer->setMaterialName(_T("undefine"));



	// LightLayer
	IGameObjectLightLayer* pLightLayer = gEnv->p3DEngine->createLightLayer(pGameObject, name, lightpos, lightRadius, lightColor, isFakeShadow);
	pGameObject->setGameObjectLayer(pLightLayer);


	pGameObject->setGameObjectSuperClass(eGOClass_LIGHT);
	pGameObject->setGameObjectClass(_T("Light"));

	gkLogMessage(_T("gkGOSys::%s [ %s ] Created."), pGameObject->getGameObjectClassName().c_str(), pGameObject->getName().c_str());

	return pGameObject;
}

//////////////////////////////////////////////////////////////////////////
gkGameObject* gkGameObjectSystem::_createGameObjectInternal( const gkStdString& name )
{
	// find a pos in queue [3/28/2012 Kaiming]
	uint32 pos = _findPosInQueue();
	uint32 goID = -1;
	if (pos == -1)
	{
		// there's no exsist pos for new GO, return the next.
		goID = m_queGameObject.size();
	}
	else
	{
		goID = pos;
	}

	gkGameObject* newGameObject = new gkGameObject(goID, name);

	if (pos == -1)
	{
		// no exsist pos, put at back
		m_queGameObject.push_back(newGameObject);
	}
	else
	{
		// exist pos, put into it
		m_queGameObject[goID] = newGameObject;
	}

	//TODO: put into namemap
	m_mapGameObject.insert(gkGameObjectNameMap::value_type(name, newGameObject));

	return newGameObject;
}
//////////////////////////////////////////////////////////////////////////
uint32 gkGameObjectSystem::_findPosInQueue()
{
	for(uint32 i = 0; i < m_queGameObject.size(); ++i)
	{
		if (m_queGameObject[i] == NULL)
		{
			return i;
		}
	}

	return -1;
}
//////////////////////////////////////////////////////////////////////////
IGameObject* gkGameObjectSystem::GetGameObjectById( uint32 goID )
{
	if (goID < m_queGameObject.size())
	{
		return m_queGameObject[goID];
	}

	return NULL;
}
//////////////////////////////////////////////////////////////////////////
IGameObject* gkGameObjectSystem::GetGameObjectByName( const gkStdString& name )
{
	// REFACTOR FIX, now just simple go throw queue [3/29/2012 Kaiming]
	gkGameObjectNameMap::iterator it = m_mapGameObject.find(name);
	if (it != m_mapGameObject.end())
	{
		// find
		return it->second;
	}

	return NULL;
}
//////////////////////////////////////////////////////////////////////////
bool gkGameObjectSystem::DestoryGameObject( IGameObject* pTarget )
{
	return DestoryGameObject(pTarget->getID());
}
//////////////////////////////////////////////////////////////////////////
bool gkGameObjectSystem::DestoryGameObject( uint32 id )
{
	m_destroyList.push_back( id );
	return true;
}
//////////////////////////////////////////////////////////////////////////
uint32 gkGameObjectSystem::getGameObjectCount()
{
	// return the possible large number of go
	return m_queGameObject.size();
}
//////////////////////////////////////////////////////////////////////////
gkGameObjectSystem::gkGameObjectSystem()
{
	m_queGameObject.clear();
	m_mapGameObject.clear();

	m_MovableNameGenerator = new gkNameGenerator(_T("Auto_GameObj"));
}


gkGameObjectSystem::~gkGameObjectSystem()
{
	delete m_MovableNameGenerator;
}

//////////////////////////////////////////////////////////////////////////
void gkGameObjectSystem::preupdate( float fElapsedTime )
{
	for(uint32 i = 0; i < m_queGameObject.size(); ++i)
	{
		if (m_queGameObject[i])
		{
			m_queGameObject[i]->preupdate(fElapsedTime);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
void gkGameObjectSystem::update( float fElapsedTime )
{
	for(uint32 i = 0; i < m_queGameObject.size(); ++i)
	{
		if (m_queGameObject[i])
		{
			m_queGameObject[i]->update(fElapsedTime);
		}
	}
}
//////////////////////////////////////////////////////////////////////////
void gkGameObjectSystem::destroy()
{
	for(uint32 i = 0; i < m_queGameObject.size(); ++i)
	{
		DestroyGameObjectImm(i);
	}
}

//////////////////////////////////////////////////////////////////////////
// serializer func


//////////////////////////////////////////////////////////////////////////
IGameObject* gkGameObjectSystem::CreateLightGameObject( CRapidXmlParseNode* node )
{
	if (node)
	{
		// attribute
		ColorF color(1,1,1,1);
		float radius = 1;
		bool fakeshadow = false;
		bool globalShadow = false;

		// transform
		Vec3 trans(0,0,0);
		Quat rot(1,0,0,0);
		Vec3 scale(1,1,1);

		node->GetAttribute(_T("Color"), color);
		node->GetAttribute(_T("Radius"), radius);
		node->GetAttribute(_T("FakeShadow"), fakeshadow);
		node->GetAttribute(_T("GloabalShadow"), globalShadow);

		node->GetTranslation(trans);
		node->GetOrientation(rot);
		node->GetScale(scale);

		IGameObject* go =  CreateLightGameObject(node->GetAttribute(_T("Name")), trans, radius, color, fakeshadow);

		IGameObjectLightLayer* llayer = (IGameObjectLightLayer*)(go->getGameObjectLayer( eGL_LightLayer ));
		llayer->setGlobalShadow( globalShadow );

		return go;
	}

	return NULL;
}

IGameObject* gkGameObjectSystem::CreateLightGameObject( Vec3& lightpos, float lightRadius, ColorF& lightColor, bool isFakeShadow /*= false*/ )
{
	gkStdString newName = m_MovableNameGenerator->generate();
	return CreateLightGameObject( newName, lightpos, lightRadius, lightColor, isFakeShadow );
}

//////////////////////////////////////////////////////////////////////////
IGameObject* gkGameObjectSystem::CreateVoidGameObject( CRapidXmlParseNode* node )
{
	if (node)
	{
		// transform
		Vec3 trans(0,0,0);
		Quat rot(1,0,0,0);

		node->GetTranslation(trans);
		node->GetOrientation(rot);

		return CreateVoidGameObject(node->GetAttribute(_T("Name")), trans, rot);
	}

	return NULL;
}

IGameObject* gkGameObjectSystem::CreateVoidGameObject( Vec3& pos, Quat& rot )
{
	gkStdString newName = m_MovableNameGenerator->generate();
	return CreateVoidGameObject( newName, pos, rot );
}

//////////////////////////////////////////////////////////////////////////
IGameObject* gkGameObjectSystem::CreateStaticGeoGameObject( CRapidXmlParseNode* node )
{
	if (node)
	{
		// transform
		Vec3 trans(0,0,0);
		Quat rot(1,0,0,0);
		Vec3 scale(1,1,1);

		node->GetTranslation(trans);
		node->GetOrientation(rot);
		node->GetScale(scale);

		IGameObject* statObj = CreateStaticGeoGameObject(node->GetAttribute(_T("Name")), node->GetAttribute(_T("MeshName")), trans, rot);

		if (statObj)
		{
			IGameObjectRenderLayer* pRenderLayer = statObj->getRenderLayer();
			if (pRenderLayer)
			{
				gkLogMessage(_T("Load Material"));
				pRenderLayer->setMaterialName(node->GetAttribute(_T("MaterialName")));
				gkLogMessage(_T("Material Loaded"));
			}

			statObj->setPosition(trans);
			statObj->setOrientation(rot);
			statObj->setScale(scale);

#ifdef OS_WIN32


			if (node->GetAttribute(_T("Physical")) && !_tcsicmp(node->GetAttribute(_T("Physical")), _T("true")))
			{
				IGameObjectPhysicLayer* pPhysicLayer = gEnv->pPhysics->CreatePhysicLayer();
				statObj->setGameObjectLayer(pPhysicLayer);
				pPhysicLayer->createStatic();

				
			}
			else if (node->GetAttribute(_T("Physical")) && !_tcsicmp(node->GetAttribute(_T("Physical")), _T("rigidsphere")))
			{
				IGameObjectPhysicLayer* pPhysicLayer = gEnv->pPhysics->CreatePhysicLayer();
				statObj->setGameObjectLayer(pPhysicLayer);
				pPhysicLayer->createDynamic(IGameObjectPhysicLayer::ePDT_Sphere);

				
			}


			
#endif

		}

		return statObj;
	}

	return NULL;
}

IGameObject* gkGameObjectSystem::CreateStaticGeoGameObject( const gkStdString& meshfile, Vec3& pos, Quat& rot )
{
	gkStdString newName = m_MovableNameGenerator->generate();
	return CreateStaticGeoGameObject(newName, meshfile, pos, rot);
}

//////////////////////////////////////////////////////////////////////////
IGameObject* gkGameObjectSystem::CreateAnimGameObject( CRapidXmlParseNode* node )
{
	return NULL;
}
//////////////////////////////////////////////////////////////////////////
IGameObject* gkGameObjectSystem::CreateAnimGameObject( const gkStdString& name, const gkStdString& chrfile, Vec3& pos, Quat& rot )
{
	gkGameObject* pGameObject = NULL;

	pGameObject = _createGameObjectInternal(name);

	// he will create the RenderLayer internal
	if (gEnv->pAnimation)
	{
		IGameObjectAnimLayer* pAnimLayer = gEnv->pAnimation->CreateAnimLayer(name);
		pGameObject->setGameObjectLayer(pAnimLayer);

		pAnimLayer->loadChrFile(chrfile.c_str());
	}



	pGameObject->setGameObjectSuperClass(eGOClass_SKINNEDMESH);
	pGameObject->setGameObjectClass(_T("AnimObject"));

	gkLogMessage(_T("gkGOSys::%s [ %s ] Created."), pGameObject->getGameObjectClassName().c_str(), pGameObject->getName().c_str());

	return pGameObject;
}

IGameObject* gkGameObjectSystem::CreateAnimGameObject( const gkStdString& chrfile, Vec3& pos, Quat& rot )
{
	gkStdString newName = m_MovableNameGenerator->generate();
	return CreateAnimGameObject(newName, chrfile, pos, rot);
}

bool gkGameObjectSystem::syncupdate()
{
	bool ret = false;
	if ( !m_destroyList.empty() )
	{
		ret = true;
	}

	for (uint32 i=0; i < m_destroyList.size(); ++i)
	{
		DestroyGameObjectImm(m_destroyList[i]);
	}
	
	m_destroyList.clear();	


	return ret;
}

bool gkGameObjectSystem::DestroyGameObjectImm( uint32 id )
{
	if (id < m_queGameObject.size())
	{
		gkGameObject* pTarget = m_queGameObject[id];

		// Destroy GameObject
		if (pTarget)
		{
			// remove it from namemap
			m_mapGameObject.erase(pTarget->getName());

			// destroy
			pTarget->destroy();

			// Destroy GameObject
			SAFE_DELETE(pTarget);
		}



		// Set to NULL
		m_queGameObject[id] = NULL;
		return true;
	}
	return false;
}

IGameObject* gkGameObjectSystem::CreateGameObjectFromXml( CRapidXmlParseNode* node )
{
	IGameObject* pCreated = NULL;

	if( !_tcsicmp(node->GetAttribute(_T("SuperClass")), _T("Light")))
	{
		pCreated = CreateLightGameObject(node);
	}
	else if( !_tcsicmp(node->GetAttribute(_T("SuperClass")), _T("Mesh")))
	{
		pCreated = CreateStaticGeoGameObject(node);
	}
	else if( !_tcsicmp(node->GetAttribute(_T("SuperClass")), _T("Group")))
	{
		pCreated = CreateVoidGameObject(node);
	}
	else if ( !_tcsicmp(node->GetAttribute(_T("SuperClass")), _T("Character")) )
	{
		pCreated = CreateAnimGameObject(node);
	}

	return pCreated;
}


