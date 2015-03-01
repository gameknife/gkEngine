#include "gkStableHeader.h"
#include "gk3DEngine.h"
#include "gkSceneManager.h"
#include "gkVtxLayerManager.h"

#include "gkLight.h"
#include "gkMovableObject.h"
#include "gkEntity.h"

#include "ICamera.h"
#include "gkCamera.h"
#include "gkSceneNode.h"
#include "gkSun.h"

#include "gkTimeOfDayManager.h"
#include "gkStaticObjRenderable.h"

#include "gkMemoryLeakDetecter.h"
#include "gkTerrianBlockRenderable.h"
#include "gkTerrian.h"
#include "gkParticleInstance.h"



template<> gk3DEngine* Singleton<gk3DEngine>::ms_Singleton = 0;

gk3DEngine* gk3DEngine::getSingletonPtr(void)
{
	return ms_Singleton;
}

gk3DEngine& gk3DEngine::getSingleton( void )
{
	assert( ms_Singleton );  
	return ( *ms_Singleton );  
}

gk3DEngine::gk3DEngine( void ):m_pSceneMng(NULL)
,m_pGameObjectFactory(NULL)
,m_pLightFactory(NULL)
,m_pParticleFactory(NULL)
,m_fSnowAmount(0.0f)
,m_mainCam(NULL)
{

}

gk3DEngine::~gk3DEngine( void )
{

}

//---------------------------------------------------------------------
void gk3DEngine::addMovableObjectFactory(gkMovableObjectFactory* fact,
								   bool overrideExisting)
{
	MovableObjectFactoryMap::iterator facti = m_mapMovableObjectFactoryMap.find(
		fact->getType());
	if (!overrideExisting && facti != m_mapMovableObjectFactoryMap.end())
	{
		gkLogWarning( _T("一个移动物件工厂已经存在。Root::addMovableObjectFactory") );
	}

	// Save
	m_mapMovableObjectFactoryMap[fact->getType()] = fact;

	// Success!
}
//-----------------------------------------------------------------------
gkMovableObjectFactory* gk3DEngine::getMovableObjectFactory( gkStdString typeName )
{
	MovableObjectFactoryMap::iterator i =
		m_mapMovableObjectFactoryMap.find(typeName);
	if (i == m_mapMovableObjectFactoryMap.end())
	{
		gkLogWarning( _T("没有找到对应的工厂@gkRoot::getMovableObjectFactory") );
	}
	return i->second;
}
//-----------------------------------------------------------------------
void gk3DEngine::removeMovableObjectFactory( gkMovableObjectFactory* fact )
{
	MovableObjectFactoryMap::iterator i = m_mapMovableObjectFactoryMap.find(
		fact->getType());
	if (i != m_mapMovableObjectFactoryMap.end())
	{
		m_mapMovableObjectFactoryMap.erase(i);
	}
}
//-----------------------------------------------------------------------
bool gk3DEngine::hasMovableObjectFactory( const gkStdString& typeName ) const
{
	return !(m_mapMovableObjectFactoryMap.find(typeName) == m_mapMovableObjectFactoryMap.end());
}

//-----------------------------------------------------------------------
ICamera* gk3DEngine::createCamera( const gkStdString& wstrName )
{
	ICamera* ret = m_pSceneMng->createICamera(wstrName);


	// 创建camera并与renderTarget建立连接
	//gkSceneNode* pNode = m_pSceneMng->getRootSceneNode()->createChildSceneNode(wstrName);
	gkCamera* pMainCam = m_pSceneMng->getCamera(wstrName);

	//pNode->attachObject(pMainCam);

	// 重置一下camera位置
	//m_pSceneMng->getSceneNode(wstrName)->setPosition(10,20,-80);
	//m_pSceneMng->getSceneNode(wstrName)->setOrientation(Quat(1,0,0,0));

	pMainCam->setPosition(0,-80,20);
	
	pMainCam->setOrientation(Quat(1,0,0,0));

	return ret;
}
//-----------------------------------------------------------------------
ICamera* gk3DEngine::getCamera( const gkStdString& wstrName ) const
{
	return m_pSceneMng->getCamera(wstrName);
}
//-----------------------------------------------------------------------
bool gk3DEngine::hasCamera( const gkStdString& wstrName ) const
{
	return m_pSceneMng->hasCamera(wstrName);
}
//-----------------------------------------------------------------------
void gk3DEngine::destroyCamera( const gkStdString& wstrName )
{
	m_pSceneMng->destroyCamera(wstrName);
}
//-----------------------------------------------------------------------
void gk3DEngine::destroyAllCameras( void )
{
	m_pSceneMng->destroyAllCameras();
}
//-----------------------------------------------------------------------
IGameObjectRenderLayer* gk3DEngine::createRenderLayer()
{
	gkEntity* pEnt = m_pSceneMng->createEntity();
	gkSceneNode* pNode = m_pSceneMng->getRootSceneNode()->createChildSceneNode();
	pNode->attachObject(pEnt);

	return pEnt;
}
//-----------------------------------------------------------------------
IGameObjectRenderLayer* gk3DEngine::createRenderLayer( const gkStdString& gameobjectName, const Vec3& pos, const Quat& quat )
{

	gkEntity* pEnt = m_pSceneMng->createEntity(gameobjectName);
	gkSceneNode* pNode = m_pSceneMng->getRootSceneNode()->createChildSceneNode(pos, quat);
	pNode->attachObject(pEnt);

	return pEnt;
}


ITerrianSystem* gk3DEngine::createTerrian()
{
	// FIXME!!
	if (m_terrians.size() > 0)
	{
		gkLogError( _T("尝试创建多个地形，返回第一个") );
		return getTerrian();
	}

	gkTerrian* pEnt = new gkTerrian();
	if (pEnt)
	{
		m_terrians.push_back( pEnt );
	}
	return pEnt;
}

void gk3DEngine::destroyTerrian( ITerrianSystem* target )
{
	if (target)
	{
		m_destroyterrians.push_back( target );
	}	
}

void gk3DEngine::destroyTerrianSync( ITerrianSystem* target )
{
	if (target)
	{
		destroyTerrianImm( target );
	}
}

ITerrianSystem* gk3DEngine::getTerrian()
{
	if ( m_terrians.empty() )
	{
		return NULL;
	}

	return m_terrians.front();
}

void gk3DEngine::destroyTerrianImm( ITerrianSystem* target )
{
	Terrians::iterator it = m_terrians.begin();
	for ( ; it != m_terrians.end(); ++it )
	{
		if ( *it == target )
		{
			(*it)->Destroy();
			//delete (*it);

			m_terrians.erase(it);
			break;
		}
	}
}
//-----------------------------------------------------------------------
void gk3DEngine::removeRenderLayer( const gkStdString& meshName )
{
	m_pSceneMng->destroyMovableObject(meshName, gkEntityFactory::FACTORY_TYPE_NAME);
}
//-----------------------------------------------------------------------
void gk3DEngine::removeRenderLayer( IGameObjectRenderLayer* entity )
{
	m_pSceneMng->destroyMovableObject(static_cast<gkEntity*>(entity));
}

//-----------------------------------------------------------------------
IGameObjectRenderLayer* gk3DEngine::getRenderLayer( const gkStdString& meshName )
{
	return static_cast<gkEntity*>(m_pSceneMng->getMovableObject(meshName, gkEntityFactory::FACTORY_TYPE_NAME));
}

//-----------------------------------------------------------------------
void gk3DEngine::createSun()
{
	gkSun* pSun = m_pSceneMng->createSun();

	//pSun->bindSceneNode(m_pCameraAgent->getTransform());
	pSun->setTime(9.2f);

	pSun->setAmbientColor(22.0f/255.0f,26.0f/255.0f,30.0f/255.0f);
	pSun->setDiffuseColor(255.0f/255.0f,241.0f/255.0f,204.0f/255.0f);
}
//-----------------------------------------------------------------------
void gk3DEngine::destroyAllRenderLayer()
{
	m_pSceneMng->destroyAllMovableObjectsByType(gkEntityFactory::FACTORY_TYPE_NAME);
}
void gk3DEngine::destroyAllLightLayer()
{
	m_pSceneMng->destroyAllMovableObjectsByType(gkLightFactory::FACTORY_TYPE_NAME);
}

//-----------------------------------------------------------------------
IGameObject* gk3DEngine::getRayHitEntity( Ray& ray, uint32 index /*= 0*/ )
{
	SRayhitResult result = m_pSceneMng->getRayHitEntity(ray, 0);
	if( result.nNum > 0 && index < result.nNum )
	{
		return result.pObjects[index];
	}
    return NULL;
}

void gk3DEngine::setTimeOfDay( float time )
{
	getSceneMngPtr()->getSun()->setTime(time);
}


ITimeOfDay* gk3DEngine::getTimeOfDay()
{
	return m_pTODManager;
}


IGameObjectLightLayer* gk3DEngine::createLightLayer( IGameObject* parent, const gkStdString& lightName, Vec3& lightpos, float lightRadius, ColorF& lightColor, bool isFakeShadow )
{
	gkLight* pLight = getSceneMngPtr()->createLight();
	pLight->setPosition(Vec3(0,0,0));
	pLight->setRadius(lightRadius);
	pLight->setDiffuseColor(lightColor.r, lightColor.g, lightColor.b);
	pLight->setFakeShadow(isFakeShadow);

	// attach to it's father
	IGameObjectRenderLayer* renderLayer = parent->getRenderLayer();
	if (!renderLayer)
	{
		gkLogError(_T("Create Light Layer [%s] Failed"), lightName.c_str());
		return NULL;
	}

	gkEntity* parentEnt = static_cast<gkEntity*>(renderLayer);

	parentEnt->getParentSceneNode()->attachObject(pLight);
	parentEnt->setScale(lightRadius, lightRadius, lightRadius);

	return pLight;
}


IGameObjectParticleLayer* gk3DEngine::createParticleLayer( IGameObject* parent, const gkStdString& particleName, const gkStdString& ptcFilename )
{


	//pParticle->set

// 	pLight->setPosition(Vec3(0,0,0));
// 	pLight->setRadius(lightRadius);
// 	pLight->setDiffuseColor(lightColor.r, lightColor.g, lightColor.b);
// 	pLight->setFakeShadow(isFakeShadow);

	// attach to it's father
	IGameObjectRenderLayer* renderLayer = parent->getRenderLayer();
	if (!renderLayer)
	{
		gkLogError(_T("Create Particle Layer [%s] Failed"), particleName.c_str());
		return NULL;
	}

	gkParticleInstance* pParticle = getSceneMngPtr()->createParticle(particleName);
	pParticle->bindRenderLayer( renderLayer );
	pParticle->loadPtcFile( ptcFilename.c_str() );

	gkEntity* parentEnt = static_cast<gkEntity*>(renderLayer);
	parentEnt->getParentSceneNode()->attachObject(pParticle);
	
	return pParticle;
}


const Vec3& gk3DEngine::getSunDir()
{
	return getSceneMngPtr()->getSun()->getDerivedDirection(true);
}

void gk3DEngine::update( float fElapsedTime )
{
	//////////////////////////////////////////////////////////////////////////
	// move render sequence update here [6/20/2013 gameKnife]
	m_pSceneMng->_prepareRenderSequences();

	m_pTODManager->update(fElapsedTime);

	ITerrianSystem* terrian = getTerrian();
	if (terrian)
	{
		terrian->Update();
	}
}

void gk3DEngine::_SwapRenderSequence()
{
	if (m_pSceneMng)
	{
		m_pSceneMng->_sceneStart(  (gkCamera*)getMainCamera() );
		//gEnv->pRenderer->RC_SetTodKey(getTimeOfDay()->getCurrentTODKey());
	}
	
}

//////////////////////////////////////////////////////////////////////////
gkRenderable* gk3DEngine::createRenderable( gkMeshPtr& meshptr, E3DRenderableType type, uint32 subsetID )
{
	gkRenderable* ret = NULL;
	switch (type)
	{
	case e3RT_StaticObj:
		ret = new gkStaticObjRenderable( meshptr, subsetID );
		break;
// 	case e3RT_TerrianBlock:
// 		ret = new gkTerrianBlockRenderable();
// 		break;
	}

	return ret;
}

SRayhitResult gk3DEngine::getRayHitEntitys( Ray& ray, uint32 ignoreFirstNum /*= 0*/ )
{
	return m_pSceneMng->getRayHitEntity(ray, ignoreFirstNum);
}

bool gk3DEngine::syncupdate()
{
	m_pSceneMng->setRenderSequence( gEnv->pRenderer->RT_SwapRenderSequence() );
	m_pSceneMng->_updateSceneGraph();

	if (m_destroyterrians.empty())
	{
		return false;
	}

	for (uint32 i=0; i < m_destroyterrians.size(); ++i)
	{
		destroyTerrianImm( m_destroyterrians[i] );
	}

	m_destroyterrians.clear();




	return true;
}

void gk3DEngine::Init()
{
	// instantiate and register base movable factories
	m_pGameObjectFactory = new gkEntityFactory();
	addMovableObjectFactory(m_pGameObjectFactory, true);
	m_pLightFactory = new gkLightFactory();
	addMovableObjectFactory(m_pLightFactory, true);
	m_pParticleFactory = new gkParticleFactory();
	addMovableObjectFactory(m_pParticleFactory, true);

	// manager初始化
	m_pSceneMng =			new gkSceneManager();
	m_pTODManager =			new gkTimeOfDayManager();

	m_pTODManager->init();
}

void gk3DEngine::Destroy()
{
	SAFE_DELETE( m_pSceneMng );
	SAFE_DELETE( m_pTODManager );
	SAFE_DELETE( m_pGameObjectFactory );
	SAFE_DELETE( m_pLightFactory );
	SAFE_DELETE( m_pParticleFactory );
}

void gk3DEngine::setMainCamera( ICamera* cam )
{
	if (cam)
	{
		m_mainCam = cam;
	}
}

ICamera* gk3DEngine::getMainCamera()
{
	if (m_camStack.empty())
	{
		return m_mainCam;
	}
	else
	{
		return m_camStack.top();
	}
}

ICamera* gk3DEngine::getRenderingCamera()
{
	if (m_camStack.empty())
	{
		return getMainCamera();
	}
	else
	{
		return m_camStack.top();
	}
}






//-----------------------------------------------------------------------
gkSceneManager* getSceneMngPtr()
{
	return gk3DEngine::getSingletonPtr()->getSceneMngPtr();
}

gk3DEngine* get3DEngine()
{
	return (gk3DEngine*)(gEnv->p3DEngine);
}
