#include "gkStableHeader.h"
#include "gkSceneManager.h"
#include "gk3DEngine.h"
#include "gkRenderable.h"
//#include "gkshaderparamdatasource.h"
//#include "gkD3D9RenderSystem.h"
#include "IRenderSequence.h"
#include "gkCamera.h"
#include "gkSceneNode.h"
#include "gkRenderOperation.h"
#include "gkrenderablelist.h"
#include "gkEntity.h"
//#include "gkhelperobjectmanager.h"
//#include "gkShaderManager.h"
#include "gkLight.h"
#include "gkSun.h"

#include "../gkParticleInstance.h"

//#include "gkmisc.h"
#ifdef OS_WIN32
	#include <MMSystem.h>
#endif

//#include "NVPerfSDK.h"
#include "gk_GeoIntersect.h"
#include "ISystemProfiler.h"
#include "IAuxRenderer.h"

#include "gkMemoryLeakDetecter.h"




//-----------------------------------------------------------------------
gkSceneManager::gkSceneManager( void ):
	m_pUpdatingRenderSequence(NULL)
	,m_pSceneRoot(NULL)
	,m_bFindVisibleObjects(true)
	,m_MovableNameGenerator(NULL)
	,m_pSun(NULL)
{
	m_vCurrentLightVector = Vec3(1,1,-1);
	m_yLightType = gkLight::LT_DIRECTIONAL;

	MovableObjectMap* newCollection = new MovableObjectMap;
	m_mapMovableObjectCollectionMap[_T("GameObject")] = newCollection;
	newCollection = new MovableObjectMap;
	m_mapMovableObjectCollectionMap[_T("Light")] = newCollection;

	m_MovableNameGenerator = new gkNameGenerator(_T("GameKnife/MO"));

	m_pRayQueryer = new gkDefaultRaySceneQuery(this);
}
//-----------------------------------------------------------------------
gkSceneManager::~gkSceneManager( void )
{
	destroyAllCameras();
	clearScene();

	{
		for (MovableObjectCollectionMap::iterator i = m_mapMovableObjectCollectionMap.begin();
			i != m_mapMovableObjectCollectionMap.end(); ++i)
		{
			SAFE_DELETE( i->second );
		}
		m_mapMovableObjectCollectionMap.clear();
	}

	SAFE_DELETE( m_pSun );

	SAFE_DELETE( m_MovableNameGenerator );

	SAFE_DELETE( m_pRayQueryer );
}

//-----------------------------------------------------------------------
void gkSceneManager::clearScene(void)
{
	//destroyAllStaticGeometry();
	destroyAllMovableObjects();

	// Clear root node of all children
	getRootSceneNode()->removeAllChildren();
	getRootSceneNode()->detachAllObjects();
	SAFE_DELETE( m_pSceneRoot );

	// Delete all SceneNodes, except root that is
	for (SceneNodeList::iterator i = m_mapSceneNodes.begin();
		i != m_mapSceneNodes.end(); ++i)
	{
		SAFE_DELETE( i->second );
	}
	m_mapSceneNodes.clear();
}

inline bool LightDepthCompare(gkRenderLight lhs,   gkRenderLight rhs)
{
	Vec3 campos = gEnv->p3DEngine->getMainCamera()->getDerivedPosition();
	Vec3 camdir = gEnv->p3DEngine->getMainCamera()->getDerivedDirection();
	Vec3 lhsVec = lhs.m_vPos - campos;
	Vec3 rhsVec = rhs.m_vPos - campos;
	lhsVec = Vec3::CreateProjection(lhsVec, camdir);
	rhsVec = Vec3::CreateProjection(rhsVec, camdir);

	if (lhsVec.GetLength() > rhsVec.GetLength())
		return true;

	return false;
} 

//-----------------------------------------------------------------------
IRenderSequence* gkSceneManager::getRenderSequence(bool deferred)
{
	return m_pUpdatingRenderSequence;
}

//-----------------------------------------------------------------------
ICamera* gkSceneManager::createICamera(const gkStdString& wstrName)
{
	// Check name not used
	if (m_pCameraList.find(wstrName) != m_pCameraList.end())
	{
		gkLogWarning( _T("A camera with the name [%s] already exists\n"), wstrName.c_str() );

	}

	gkCamera *c = new gkCamera(wstrName);
	m_pCameraList.insert(CameraList::value_type(wstrName, c));

	return c;
}

//-----------------------------------------------------------------------
gkCamera* gkSceneManager::createCamera(const gkStdString& wstrName)
{
	// Check name not used
	if (m_pCameraList.find(wstrName) != m_pCameraList.end())
	{
		gkLogWarning( _T("A camera with the name [%s] already exists\n"), wstrName.c_str() );
	}

	gkCamera *c = new gkCamera(wstrName);
	m_pCameraList.insert(CameraList::value_type(wstrName, c));

	return c;
}
//-----------------------------------------------------------------------
gkCamera* gkSceneManager::getCamera(const gkStdString& wstrName) const
{
	CameraList::const_iterator i = m_pCameraList.find(wstrName);
	if (i == m_pCameraList.end())
	{
		return NULL;
	}
	else
	{
		return i->second;
	}
}
//-----------------------------------------------------------------------
bool gkSceneManager::hasCamera(const gkStdString& wstrName) const
{
	return (m_pCameraList.find(wstrName) != m_pCameraList.end());
}

//-----------------------------------------------------------------------
void gkSceneManager::destroyCamera(gkCamera *cam)
{
	destroyCamera(cam->getName());
}

//-----------------------------------------------------------------------
void gkSceneManager::destroyCamera(const gkStdString& wstrName)
{
	// Find in list
	CameraList::iterator i = m_pCameraList.find(wstrName);
	if (i != m_pCameraList.end())
	{
		SAFE_DELETE( i->second );
		m_pCameraList.erase(i);
	}
}

//-----------------------------------------------------------------------
void gkSceneManager::destroyAllCameras(void)
{

	CameraList::iterator i = m_pCameraList.begin();
	for (; i != m_pCameraList.end(); ++i)
	{
		SAFE_DELETE( i->second );
	}
	m_pCameraList.clear();
}
//-----------------------------------------------------------------------
void gkSceneManager::_updateSceneGraph()
{
	getRootSceneNode()->_update(true, false);
}
//-----------------------------------------------------------------------
void gkSceneManager::_findVisibleObjects( gkCamera* cam )
{
	// Tell nodes to find, cascade down all nodes
	getRootSceneNode()->_findVisibleObjects(cam, getRenderSequence());
}
//-----------------------------------------------------------------------
gkSceneNode* gkSceneManager::createSceneNodeImpl(void)
{
	return new gkSceneNode();
}
//-----------------------------------------------------------------------
gkSceneNode* gkSceneManager::createSceneNodeImpl(const gkStdString& name)
{
	return new gkSceneNode(name);
}
//-----------------------------------------------------------------------
gkSceneNode* gkSceneManager::getRootSceneNode(void)
{
	if (!m_pSceneRoot)
	{
		// Create root scene node
		m_pSceneRoot = createSceneNodeImpl(_T("SceneRoot"));
	}

	return m_pSceneRoot;
}
//-----------------------------------------------------------------------
gkSceneNode* gkSceneManager::createSceneNode( void )
{
	gkSceneNode* pSceneNode = createSceneNodeImpl();
//  加入一个scenenode维护的snlist，便于方便控制
 	assert(m_mapSceneNodes.find(pSceneNode->getName()) == m_mapSceneNodes.end());
 	m_mapSceneNodes[pSceneNode->getName()] = pSceneNode;
	return pSceneNode;
}
//-----------------------------------------------------------------------
gkSceneNode* gkSceneManager::createSceneNode( const gkStdString& name )
{
// 使用snlist来查询此节点是否被创建过
	SceneNodeList::iterator iter = m_mapSceneNodes.find(name);
	if (iter != m_mapSceneNodes.end())
	{
		gkLogWarning( _T("SceneNode重复创建！") );
		return iter->second;
	}

	gkSceneNode* pSceneNode = createSceneNodeImpl(name);
	m_mapSceneNodes[pSceneNode->getName()] = pSceneNode;
	return pSceneNode;
}
//-----------------------------------------------------------------------
void gkSceneManager::destroySceneNode( const gkStdString& name )
{
	SceneNodeList::iterator iter = m_mapSceneNodes.find(name);
	if (iter != m_mapSceneNodes.end())
	{
		iter->second->detachAllObjects();

		gkNode* nodeFather = iter->second->getParent();
		if (nodeFather)
		{
			nodeFather->removeChild(iter->second);
		}

		SAFE_DELETE(iter->second);
		m_mapSceneNodes.erase(iter);
	}
}
//-----------------------------------------------------------------------
void gkSceneManager::destroySceneNode( gkSceneNode* sn )
{
	destroySceneNode(sn->getName());
}
//-----------------------------------------------------------------------
gkSceneNode* gkSceneManager::getSceneNode(const gkStdString& name) const
{
	SceneNodeList::const_iterator i = m_mapSceneNodes.find(name);

	if (i == m_mapSceneNodes.end())
	{
		gkLogWarning( _T("找不到SceneNode. @gkSceneManager::getSceneNode\n") );

	}

	return i->second;

}
//-----------------------------------------------------------------------
bool gkSceneManager::hasSceneNode(const gkStdString& name) const
{
	return (m_mapSceneNodes.find(name) != m_mapSceneNodes.end());
}
//-----------------------------------------------------------------------
gkMovableObject* gkSceneManager::createMovableObject( const gkStdString& name, const gkStdString& typeName, const NameValuePairList* params /*= 0*/ )
{
	// Nasty hack to make generalised Camera functions work without breaking add-on SMs
	if (typeName == _T("Camera"))
	{
		// REFACTOR FIX [8/10/2011 Kaiming-Desktop]
		return createCamera(name);
		//return 0;
	}
	gkMovableObjectFactory* factory = gk3DEngine::getSingleton().getMovableObjectFactory(typeName);
	// Check for duplicate names
	MovableObjectMap* objectMap = getMovableObjectCollection(typeName);

	{
		if (objectMap->find(name) != objectMap->end())
		{
			gkLogWarning( _T("已经存在指定的可移动物体了@SceneManager::getMovableObjectCollection\n") );

			return NULL;
		}

		gkMovableObject* newObj = factory->createInstance(name, params);
		(*objectMap)[name] = newObj;
		return newObj;
	}
}
//-----------------------------------------------------------------------
gkMovableObject* gkSceneManager::createMovableObject( const gkStdString& typeName, const NameValuePairList* params /*= 0*/ )
{
	gkStdString newName = m_MovableNameGenerator->generate();

	return createMovableObject(newName, typeName, params );
}
//-----------------------------------------------------------------------
gkEntity* gkSceneManager::createEntity( const gkStdString& entityName )
{
	return static_cast<gkEntity*>(
		createMovableObject(entityName, gkEntityFactory::FACTORY_TYPE_NAME));
}
//-----------------------------------------------------------------------
gkSceneManager::MovableObjectMap* gkSceneManager::getMovableObjectCollection( const gkStdString& typeName )
{
	MovableObjectCollectionMap::const_iterator i = 
		m_mapMovableObjectCollectionMap.find(typeName);
	if (i == m_mapMovableObjectCollectionMap.end())
	{
		// create
		MovableObjectMap* newCollection = new MovableObjectMap;
		m_mapMovableObjectCollectionMap[typeName] = newCollection;
		return newCollection;
	}
	else
	{
		return i->second;
	}
}
//-----------------------------------------------------------------------
const gkSceneManager::MovableObjectMap* gkSceneManager::getMovableObjectCollection( const gkStdString& typeName ) const
{
	MovableObjectCollectionMap::const_iterator i = 
		m_mapMovableObjectCollectionMap.find(typeName);
	if (i == m_mapMovableObjectCollectionMap.end())
	{
		// const版本，不做改变
		gkLogWarning( _T("严重错误:getMovableObject的时候没有找到组") );
		return NULL;
	}
	else
	{
		return i->second;
	}
}

//---------------------------------------------------------------------
gkSceneManager::MovableObjectIterator 
gkSceneManager::getMovableObjectIterator(const gkStdString& typeName)
{
	MovableObjectMap* objectMap = getMovableObjectCollection(typeName);
	// Iterator not thread safe! Warned in header.
	return MovableObjectIterator(objectMap->begin(), objectMap->end());
}

//-----------------------------------------------------------------------
gkEntity* gkSceneManager::createEntity()
{
	gkStdString newName = m_MovableNameGenerator->generate();
	return createEntity(newName);
	 
}
//-----------------------------------------------------------------------
bool gkSceneManager::hasMovableObject( const gkStdString& name, const gkStdString& typeName ) const
{
	// Nasty hack to make generalised Camera functions work without breaking add-on SMs
	if (typeName == _T("Camera"))
	{
		return hasCamera(name);
	}

		MovableObjectCollectionMap::const_iterator i = 
		m_mapMovableObjectCollectionMap.find(typeName);
	if (i == m_mapMovableObjectCollectionMap.end())
		return false;

	{
			return (i->second->find(name) != i->second->end());
	}
}
//-----------------------------------------------------------------------
void gkSceneManager::destroyMovableObject( const gkStdString& name, const gkStdString& typeName )
{
	// Nasty hack to make generalised Camera functions work without breaking add-on SMs
    if (typeName == _T("Camera"))
	{
		destroyCamera(name);
		return;
	}

	MovableObjectMap* objectMap = getMovableObjectCollection(typeName);
	gkMovableObjectFactory* factory = 
		gk3DEngine::getSingleton().getMovableObjectFactory(typeName);

	{
		MovableObjectMap::iterator mi = objectMap->find(name);
		if (mi != objectMap->end())
		{
			factory->destroyInstance(mi->second);
			objectMap->erase(mi);
		}
	}
}
//-----------------------------------------------------------------------
void gkSceneManager::destroyMovableObject( gkMovableObject* m )
{
	destroyMovableObject(m->getName(), m->getMovableType());
}
//-----------------------------------------------------------------------
void gkSceneManager::destroyAllMovableObjectsByType( const gkStdString& typeName )
{
	// Nasty hack to make generalised Camera functions work without breaking add-on SMs
	if (typeName == _T("Camera"))
	{
		destroyAllCameras();
		return;
	}
	MovableObjectMap* objectMap = getMovableObjectCollection(typeName);
	gkMovableObjectFactory* factory = 
		gk3DEngine::getSingleton().getMovableObjectFactory(typeName);

	{
		MovableObjectMap::iterator i = objectMap->begin();
		for (; i != objectMap->end(); ++i)
		{
			factory->destroyInstance(i->second);
		}
		objectMap->clear();
	}
}
//-----------------------------------------------------------------------
void gkSceneManager::destroyAllMovableObjects( void )
{
	MovableObjectCollectionMap::iterator ci = m_mapMovableObjectCollectionMap.begin();

	for(;ci != m_mapMovableObjectCollectionMap.end(); ++ci)
	{
		MovableObjectMap* coll = ci->second;

			if (gk3DEngine::getSingleton().hasMovableObjectFactory(ci->first))
			{
				// Only destroy if we have a factory instance; otherwise must be injected
				gkMovableObjectFactory* factory = 
					gk3DEngine::getSingleton().getMovableObjectFactory(ci->first);
				MovableObjectMap::iterator i = coll->begin();
				for (; i != coll->end(); ++i)
				{
					factory->destroyInstance(i->second);
				}
			}
			coll->clear();
	}
}
//-----------------------------------------------------------------------
gkMovableObject* gkSceneManager::getMovableObject( const gkStdString& name, const gkStdString& typeName ) const
{
	// Nasty hack to make generalised Camera functions work without breaking add-on SMs
	if (typeName == _T("Camera"))
	{
		return getCamera(name);
	}

	const MovableObjectMap* objectMap = getMovableObjectCollection(typeName);

	{
		MovableObjectMap::const_iterator mi = objectMap->find(name);
		if (mi == objectMap->end())
		{
			gkLogWarning( _T("没有找到Movableobject@gkSceneManager::getMovableObject\n") );
			return NULL;
		}
		return mi->second;
	}
}
//-----------------------------------------------------------------------
gkEntity* gkSceneManager::getGameObject( const gkStdString& gameobjectName )
{
	return static_cast<gkEntity*>(getMovableObject(gameobjectName, gkEntityFactory::FACTORY_TYPE_NAME));
}

gkParticleInstance* gkSceneManager::createParticle( const gkStdString& lightName )
{
	// delegate to factory implementation
	return static_cast<gkParticleInstance*>(
		createMovableObject(lightName, gkParticleFactory::FACTORY_TYPE_NAME));
}

gkParticleInstance* gkSceneManager::createParticle()
{
	gkStdString newName = m_MovableNameGenerator->generate();
	return createParticle(newName);
}
//-----------------------------------------------------------------------
gkLight* gkSceneManager::createLight( const gkStdString& lightName )
{
	// delegate to factory implementation
	return static_cast<gkLight*>(
		createMovableObject(lightName, gkLightFactory::FACTORY_TYPE_NAME));
}
//-----------------------------------------------------------------------
gkLight* gkSceneManager::createLight()
{
	gkStdString newName = m_MovableNameGenerator->generate();
	return createLight(newName);
}
//-----------------------------------------------------------------------
gkLight* gkSceneManager::getLight( const gkStdString& lightName )
{
	return static_cast<gkLight*>(getMovableObject(lightName, gkLightFactory::FACTORY_TYPE_NAME));
}
//-----------------------------------------------------------------------
gkSun* gkSceneManager::createSun()
{
	m_pSun = new gkSun(_T("SingleSun"));

	m_pSun->bindSceneNode(getRootSceneNode());

	return m_pSun;
}
//-----------------------------------------------------------------------
gkSun* gkSceneManager::getSun() const
{
	return m_pSun;
}
SRayhitResult gkSceneManager::getRayHitEntity( const Ray& ray, uint32 ignoreFirstNum /*= 0*/ )
{
	m_pRayQueryer->setRay(ray);
	m_pRayQueryer->setSortByDistance(true);

	gkRaySceneQueryResult& result = m_pRayQueryer->execute();

	SRayhitResult ret;
	
	if (ignoreFirstNum < result.size())
	{
		ret.nNum = result.size() - ignoreFirstNum;

		for (uint32 i= ignoreFirstNum; i< result.size(); ++i)
		{
			gkEntity* pRenderLayer = static_cast<gkEntity*>(result[i].movable);
			ret.pObjects.push_back( pRenderLayer->getParentGameObject() );
		}
	}

	return ret;
}

//////////////////////////////////////////////////////////////////////////
// main thread

// backup & Clear RenderSeq
void gkSceneManager::_sceneStart(gkCamera* pMainCamera)
{
}

// rebuild RenderSeq
void gkSceneManager::_prepareRenderSequences()
{
	uint32 s_framecounter = gEnv->pProfiler->getFrameCount();

	if (!m_pUpdatingRenderSequence)
	{
		return;
	}
	m_pUpdatingRenderSequence->clear();
	// Tell params about camera

	gkCamera* mainCam = (gkCamera*)get3DEngine()->getMainCamera();//getCamera( _T("MainCamera") );
	gkCamera* mainLightCam = getCamera( _T("MainLightCamera"));

	gkSun* pMainLight = getSun();

	if (pMainLight)
	{
		gEnv->pRenderer->RC_SetSunDir(pMainLight->getDerivedDirection(true));
	}

	if (mainCam)
	{
		mainCam->updateView();
		getRenderSequence()->setCamera(mainCam, eRFMode_General);
		getRenderSequence()->setRenderableFillMode(eRFMode_General);
		_findVisibleObjects(mainCam);
	}

	if (mainLightCam)
	{
		// 填充渲染队列
		// 使用各种裁剪算法来向renderSequence加入renderable

		// 如果是主摄像机，我们为其填充一个简易队列，来渲染Zprepass [10/2/2011 Kaiming]

		// 如果是SunShadow摄像机,我们为CSM的每一个CASCADE,做一次rendersequence, 然后backup得到的Zpass队列，清空。 [10/26/2011 Kaiming]

		mainLightCam->updateView();
			getRenderSequence()->setCamera(mainLightCam, eRFMode_ShadowCas0);
			getRenderSequence()->setCamera(getSun()->getCascade1(), eRFMode_ShadowCas1);
			getRenderSequence()->setCamera(getSun()->getCascade2(), eRFMode_ShadowCas2);
			// 这是sun shadow
			getRenderSequence()->setRenderableFillMode(eRFMode_ShadowCas0);
			_findVisibleObjects(mainLightCam);

			getSun()->getCascade1()->updateView();
			getRenderSequence()->setRenderableFillMode(eRFMode_ShadowCas1);
			_findVisibleObjects(getSun()->getCascade1());

			//  				if (s_framecounter % 61 == 0)
			//  				{
			getSun()->getCascade2()->updateView();
			getRenderSequence()->setRenderableFillMode(eRFMode_ShadowCas2);
			_findVisibleObjects(getSun()->getCascade2());
			//				}


		getRenderSequence()->setRenderableFillMode(eRFMode_General);
	}

	getRenderSequence()->markFinished();
}

// query [8/22/2011 Kaiming-Desktop]
//---------------------------------------------------------------------
gkDefaultRaySceneQuery::
gkDefaultRaySceneQuery(gkSceneManager* creator) : gkRaySceneQuery(creator)
{
}
//---------------------------------------------------------------------
gkDefaultRaySceneQuery::~gkDefaultRaySceneQuery()
{
}
//---------------------------------------------------------------------
gkRaySceneQueryResult& gkDefaultRaySceneQuery::execute()
{
	clearResults();
	// Note that becuase we have no scene partitioning, we actually
	// perform a complete scene search even if restricted results are
	// requested; smarter scene manager queries can utilise the paritioning 
	// of the scene in order to reduce the number of intersection tests 
	// required to fulfil the query

	// Iterate over all movable types
	gkSceneManager::MovableObjectIterator objItA = m_pParentSceneMgr->getMovableObjectIterator(gkEntityFactory::FACTORY_TYPE_NAME);

	while (objItA.hasMoreElements())
	{
		gkMovableObject* a = objItA.getNext();


		if( !a->getVisible() )
		{
			continue;;
		}
		// skip whole group if type doesn't match
// 		if (!(a->getTypeFlags() & m_uQueryTypeMask))
// 			break;
// 
// 		if( (a->getQueryFlags() & m_uQueryMask)/* && a->isInScene()*/)
// 		{
			// Do ray / box test
// 			Vec3 insectPoint;
// 			insectPoint.Set(0,0,0);

			// first low guality
//			uint8 result = Intersect::Ray_AABB( m_vRay, a->getWorldAABB(true), insectPoint);

			//--- 0x00 = no intersection (output undefined)        --------------------------
			//--- 0x01 = intersection (intersection point in output)              --------------
			//--- 0x02 = start of Lineseg is inside the AABB (ls.start is output) 

// 			if (result == 0x01)
// 			{
				// here may intersection, high guality raycast if needed
				// hg raycast
				{
		 			Vec3 insectPoint;
		 			insectPoint.Set(0,0,0);

					 //first low guality
					uint8 result = Intersect::Ray_AABB( m_vRay, a->getWorldAABB(true), insectPoint);

		 			//if (result == 0x01)
					if (result > 0x00)
		 			{
						if( gEnv->pRenderer->GetRendererAPI() == ERdAPI_D3D9 && a->getMovableType() == gkEntityFactory::FACTORY_TYPE_NAME )
						{
							if (static_cast<gkEntity*>(a)->getRenderLayer() == RENDER_LAYER_SKIES_EARLY)
							{
								continue;
							}

							std::vector<float> distlist;
							distlist.reserve(1000);
							distlist.assign(1000, 0);

							Matrix34 world = a->_getParentNodeFullTransform();

							const gkMeshPtr mesh = static_cast<gkEntity*>(a)->getMesh();
							if( !mesh.isNull() && mesh->RaycastMesh_WorldSpace( m_vRay, world, distlist) )
							{
								queryResult(a, distlist[0]);
								continue;
							}
						}
						else
						{
							float distance = m_vRay.origin.GetDistance(insectPoint);
							queryResult(a, distance);
						}
					}
				}


//			}
//		}
	}

	return gkRaySceneQuery::execute();

}
