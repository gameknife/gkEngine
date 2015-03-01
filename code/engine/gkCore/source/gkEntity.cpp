#include "gkStableHeader.h"
#include "gkEntity.h"
#include "gkStaticObjRenderable.h"
#include "gkSceneNode.h"
//#include "gkmeshmanager.h"
#include "IRenderSequence.h"
#include "gkscenemanager.h"
#include "gk3DEngine.h"

#include "gkMemoryLeakDetecter.h"

//-----------------------------------------------------------------------
// gkEntityFactory
//-----------------------------------------------------------------------
gkStdString gkEntityFactory::FACTORY_TYPE_NAME = _T("GameObject");
//-----------------------------------------------------------------------
const gkStdString& gkEntityFactory::getType(void) const
{
	return FACTORY_TYPE_NAME;
}
//-----------------------------------------------------------------------
gkMovableObject* gkEntityFactory::createInstanceImpl( const gkStdString& name, const NameValuePairList* param, void* userdata)
{
	// must have mesh parameter
// 	gkMeshPtr pMesh;
// 	if (param != NULL)
// 	{
// 		NameValuePairList::const_iterator ni;
// 
// 		ni = param->find(_T("mesh"));
// 		if (ni != param->end())
// 		{
// 			// Get mesh (load if required)
// 			pMesh = gEnv->pSystem->getMeshMngPtr()->load(
// 				ni->second );
// 		}
// 	}
// 	//loaded..
// 	if (pMesh.isNull())
// 	{
// 		// 使用默认物体 [5/11/2011 Kaiming-Desktop]
// 		pMesh = gEnv->pSystem->getMeshMngPtr()->load(_T("_default.obj"));
// 		gkLogError(_T("gkEntity [ %s ] mesh file not find, use default."), name);
// 		if (pMesh.isNull())
// 		{
// 			gkLogError(_T("生成GO时读取默认模型出错，gkEntityFactory::createInstance"));
// 		}
// 	}

	// not create mesh when initialize

	return new gkEntity(name);
}
//-----------------------------------------------------------------------
void gkEntityFactory::destroyInstance( gkMovableObject* obj)
{
	SAFE_DELETE( obj );
}

//gkEntity Implement
//-----------------------------------------------------------------------
 gkEntity::gkEntity( void ):
 	m_bIsInitialized(false)
 	,m_parentEntity(NULL)
	,m_rebuildAABB(true)
	,m_nRealSubEntityCount(0)
 {
	m_pMaterial.setNull();
 }
//-----------------------------------------------------------------------
gkEntity::gkEntity( const gkStdString& name):
	gkMovableObject(name)
	,m_bIsInitialized(false)
	,m_parentEntity(NULL)
	,m_rebuildAABB(true)
	,m_nRealSubEntityCount(0)
{
	m_pMaterial.setNull();
	_initialize();
}
//-----------------------------------------------------------------------
gkEntity::~gkEntity( void )
{
	_deinitialise();
}
//-----------------------------------------------------------------------
void gkEntity::_initialize(void)
{
	if (m_bIsInitialized)
		return;


	m_vecSubEntityList.resize(MAX_MESHATTRIBUTE_COUNT, NULL);
	//TODO: 多线程后台载入

	// 确保载入了
// 	m_pMesh->load();
// 
// 	if (!m_pMesh->isLoaded())
// 		return;
// 
// 	// Build main subentity list
// 	buildSubEntityList(m_pMesh, &m_vecSubEntityList);

	// Update of bounds of the parent SceneNode, if Entity already attached
	// this can happen if Mesh is loaded in background or after reinitialisation
	if( m_pParentNode )
	{
		getParentSceneNode()->needUpdate();
	}

	m_fMaskColor = 0.f;

	m_bIsInitialized = true;
}
//-----------------------------------------------------------------------
void gkEntity::_deinitialise(void)
{
	if (!m_bIsInitialized)
		return;

	// Delete submeshes
	gkSubRenderableList::iterator i, iend;
	iend = m_vecSubEntityList.end();
	for (i = m_vecSubEntityList.begin(); i != iend; ++i)
	{
		// Delete SubEntity
		SAFE_DELETE( *i );
	}
	m_vecSubEntityList.clear();

	m_bIsInitialized = false;
}
//-----------------------------------------------------------------------
void gkEntity::buildSubEntityList( gkMeshPtr& pMesh, gkSubRenderableList* pSubEntityList )
{
// 	// Create SubEntities
// 	unsigned short i, numSubMeshes;
// 	gkStaticObjRenderable* subEnt;
// 
// 	// 目前mesh是属于一个模型，留给以后扩展
// 	numSubMeshes = 1;
// 	//numSubMeshes = mesh->getNumSubMeshes();
// 	for (i = 0; i < numSubMeshes; ++i)
// 	{
// 		subEnt = new gkStaticObjRenderable(this, pMesh);
// // 		if (subMesh->isMatInitialised())
// // 			subEnt->setMaterialName(subMesh->getMaterialName());
// 		pSubEntityList->push_back(subEnt);
// 
// 		// update AABB [8/21/2011 Kaiming-Desktop]
// 	}
}
//-----------------------------------------------------------------------
// add subentity addressable [3/22/2011 Kaiming-Desktop]
void gkEntity::modifySubRenderable( gkRenderable* newRenderable, BYTE yIdx )
{
	bool bIsExist = false;
	if( m_vecSubEntityList.size() > yIdx )
	{
		SAFE_DELETE( m_vecSubEntityList[yIdx] );
		bIsExist =true;
	}
	
	newRenderable->setParent(this);
	if (bIsExist)
	{
		m_vecSubEntityList[yIdx] = newRenderable;
		m_nRealSubEntityCount = yIdx + 1;
	}

	m_rebuildAABB = true;
}
//-----------------------------------------------------------------------
gkRenderable* gkEntity::getSubEntity( BYTE index /*= 0*/ ) const
{
	if(!m_vecSubEntityList.empty())
	{
		if(index == 0)
			return m_vecSubEntityList[0];
		else
		{
			if (index < m_vecSubEntityList.size())
			{
				return m_vecSubEntityList[index];
			}
		}
	}
	gkLogWarning(_T("没有找到对应index的SubEntity@gkEntity::getSubEntity"));
	return NULL;
}
//-----------------------------------------------------------------------
void gkEntity::_updateRenderSequence( IRenderSequence* queue )
{
	// Do nothing if not initialised yet
	if (!m_bIsInitialized)
		return;

	//TODO: LOD可以坐在这里，换用mesh下不同层级的模型
	//return;
	// addToRenderSequence
// 	gkSubRenderableList::iterator i, iend;
// 	iend = m_vecSubEntityList.end();
// 	for (i = m_vecSubEntityList.begin(); i != iend; ++i)
// 	{
// 		if (*i)
// 		{
// 			// for RenderLayer, judge his RendLayer
// 			if (m_bIsRenderLayerSet)
// 			{
// 				// if Forced, use it's forced layer
// 				queue->addToRenderSequence((*i), m_yRenderLayer);
// 			}
// 			else
// 			{
// 				// if not forced, rendersequence will handle it
// 				queue->addToRenderSequence((*i));
// 			}
// 		}
// 		//return;
// 	}

	for (int i=0; i < m_nRealSubEntityCount; ++i)
	{
	 	if (m_bIsRenderLayerSet)
	 	{
	 		// if Forced, use it's forced layer
	 		queue->addToRenderSequence( m_vecSubEntityList[i], m_yRenderLayer);
	 	}
	 	else
	 	{
	 		// if not forced, rendersequence will handle it
	 		queue->addToRenderSequence(m_vecSubEntityList[i]);
	 	}
	}

}
//-----------------------------------------------------------------------
const gkStdString& gkEntity::getMovableType() const
{
	return gkEntityFactory::FACTORY_TYPE_NAME;
}
//-----------------------------------------------------------------------
void gkEntity::setMaterialName(const gkStdString& name, BYTE index /*= 0*/ )
{
	// 如果没有给index，则给第一个，就是默认submesh赋值
	if(!m_vecSubEntityList.empty())
	{
		if(index == 0)
		{
			gkMaterialPtr material = gEnv->pSystem->getMaterialMngPtr()->getByName(name);
			if (material.isNull())
			{
				// 没找到先load一下 [5/11/2011 Kaiming-Desktop]
				material = gEnv->pSystem->getMaterialMngPtr()->load(name);
				if (material.isNull())
				{
					gkLogWarning(_T("gkEntity:[ %s ] Loading Material [ %s ] Failed."), m_wstrName.c_str(), name.c_str() );
					material = gEnv->pSystem->getMaterialMngPtr()->load(IMAT_DEFALUTNAME);
				}

			}

			m_pMaterial = material;

// 			if (m_vecSubEntityList[0])
// 				m_vecSubEntityList[0]->setMaterialName(name);
		}
		else
		{
			if (index < m_vecSubEntityList.size())
			{
				if( m_vecSubEntityList[index] )
					m_vecSubEntityList[index]->setMaterialName(name);
			}
		}
	}
}

//-----------------------------------------------------------------------
const gkStdString& gkEntity::getMaterialName() const
{
	if(!m_vecSubEntityList.empty())
	{
//		if(index == 0)
		{
			if( !m_pMaterial.isNull() )
				return m_pMaterial->getName();

			if (m_vecSubEntityList[0])
				return m_vecSubEntityList[0]->getMaterialName();
		}
// 		else
// 		{
// 			if (index < m_vecSubEntityList.size())
// 			{
// 				if (m_vecSubEntityList[index])
// 					return m_vecSubEntityList[index]->getMaterialName();
// 			}
// 		}	
	}
	static gkStdString materialName = _T("none");
	return materialName;
}

//-----------------------------------------------------------------------
const gkMeshPtr& gkEntity::getMesh(uint8 uIdx) const
{
	static gkMeshPtr NULLPtr;
	if (m_vecSubEntityList.size() > uIdx && m_vecSubEntityList[uIdx])
	{
		if (m_vecSubEntityList[uIdx])
			return m_vecSubEntityList[uIdx]->getMesh();
	}
	return NULLPtr;
}

//-----------------------------------------------------------------------
IGameObjectLayer* gkEntity::clone( const gkStdString& newName) const
{
	gkEntity* newEnt = gk3DEngine::getSingletonPtr()->getSceneMngPtr()->createEntity(newName);

	if (m_bIsInitialized)
	{
		// Copy material settings
		gkSubRenderableList::const_iterator i;
		unsigned int n = 0;
		for (i = m_vecSubEntityList.begin(); i != m_vecSubEntityList.end(); ++i, ++n)
		{
			//newEnt->modifySubRenderable(getSubEntity(n), n);
			//newEnt->setMaterialName((*i)->getMaterialName(), n);
		}
	}

	return newEnt;
}
//-----------------------------------------------------------------------
void gkEntity::setOrientation( const Quat& q )
{
	if (m_pParentNode)
	{
		m_pParentNode->setOrientation( q );
	}
}
//-----------------------------------------------------------------------
void gkEntity::setOrientation( float w, float x, float y, float z )
{
	if (m_pParentNode)
	{
		m_pParentNode->setOrientation( w, x, y, z );
	}
}
//-----------------------------------------------------------------------
const Quat & gkEntity::getOrientation() const
{
	if (m_pParentNode)
	{
		return m_pParentNode->getOrientation();
	}
	static Quat identQuat =  Quat::CreateIdentity();
	return identQuat;
}
//-----------------------------------------------------------------------
const Quat & gkEntity::getWorldOrientation() const
{
	if (m_pParentNode)
	{
		return m_pParentNode->_getDerivedOrientation();
	}
	static Quat identQuat =  Quat::CreateIdentity();
	return identQuat;
}
//-----------------------------------------------------------------------
void gkEntity::setPosition( const Vec3& pos )
{
	if (m_pParentNode)
	{
		m_pParentNode->setPosition( pos );
	}
}
//-----------------------------------------------------------------------
void gkEntity::setPosition( float x, float y, float z )
{
	if (m_pParentNode)
	{
		m_pParentNode->setPosition( x, y, z );
	}
}
//-----------------------------------------------------------------------
const Vec3 & gkEntity::getPosition( void ) const
{
	if (m_pParentNode)
	{
		return m_pParentNode->getPosition();
	}
	static Vec3 zeroVec(0,0,0);
	return zeroVec;
}
//-----------------------------------------------------------------------
const Vec3& gkEntity::getWorldPosition( void ) const
{
	if (m_pParentNode)
	{
		return m_pParentNode->_getDerivedPosition();
	}
	static Vec3 zeroVec(0,0,0);
	return zeroVec;
}
//-----------------------------------------------------------------------
void gkEntity::setScale( const Vec3& scale )
{
	if (m_pParentNode)
	{
		m_pParentNode->setScale( scale );
	}
}
//-----------------------------------------------------------------------
void gkEntity::setScale( float x, float y, float z )
{
	if (m_pParentNode)
	{
		m_pParentNode->setScale( x, y, z );
	}
}
//-----------------------------------------------------------------------
const Vec3 & gkEntity::getScale( void ) const
{
	if (m_pParentNode)
	{
		return m_pParentNode->getScale();
	}
	static Vec3 defaultScale(1, 1, 1);
	return defaultScale;
}
//-----------------------------------------------------------------------
void gkEntity::translate( const Vec3& d, gkTransformSpace relativeTo /*= TS_PARENT*/ )
{
	if (m_pParentNode)
	{
		m_pParentNode->translate( d, relativeTo );
	}
}
//-----------------------------------------------------------------------
void gkEntity::translate( float x, float y, float z, gkTransformSpace relativeTo /*= TS_PARENT*/ )
{
	if (m_pParentNode)
	{
		m_pParentNode->translate( x, y, z, relativeTo );
	}
}
//-----------------------------------------------------------------------
void gkEntity::rotate( const Vec3& axis, float angle, gkTransformSpace relativeTo /*= TS_LOCAL*/ )
{
	if (m_pParentNode)
	{
		m_pParentNode->rotate( axis, angle, relativeTo );
	}
}
//-----------------------------------------------------------------------
void gkEntity::rotate( const Quat& q, gkTransformSpace relativeTo /*= TS_LOCAL*/ )
{
	if (m_pParentNode)
	{
		m_pParentNode->rotate( q, relativeTo );
	}
}
//-----------------------------------------------------------------------
void gkEntity::lookat( const Vec3& point, bool bHeadup /*= true*/ )
{
	if (m_pParentNode)
	{
		m_pParentNode->lookat( point, bHeadup );
	}
}
//-----------------------------------------------------------------------
void gkEntity::smoothLookat( const Vec3& point, float fDamping, bool bHeadup /*= true*/ )
{
	if (m_pParentNode)
	{
		m_pParentNode->smoothLookat( point, fDamping, bHeadup );
	}
}
//-----------------------------------------------------------------------
const AABB& gkEntity::getAABB( void ) const
{
	//if (m_rebuildAABB)
	{
		m_FullAABB.Reset();

		for (uint8 i = 0; i < m_vecSubEntityList.size(); i++)
		{
			if (m_vecSubEntityList[i])
			{
				m_FullAABB.Add( m_vecSubEntityList[i]->getAABB() );
			}

		}

		m_rebuildAABB = false;
	}




	return m_FullAABB;
}


const AABB& gkEntity::getWorldAABB(void) const
{
	return gkMovableObject::getWorldAABB(true);
}

//-----------------------------------------------------------------------
void gkEntity::showAABB( bool canShow )
{
	gkMovableObject::showAABB(canShow);
}
//-----------------------------------------------------------------------
const Matrix44& gkEntity::GetWorldMatrix() const
{
	static Matrix44 mat44;
	if (m_pParentNode)
	{
		mat44 = m_pParentNode->_getFullTransform();
		mat44.Transpose();

		GK_ASSERT( mat44.IsValid() );

		return mat44;
	}

	mat44.SetIdentity();
	return mat44;
}
//-----------------------------------------------------------------------
void gkEntity::setRenderPipe( uint8 layer )
{
	gkMovableObject::setRenderLayer(layer);
}

void gkEntity::setWorldPosition( const Vec3& pos )
{
	if (m_pParentNode)
	{
		m_pParentNode->_setDerivedPosition(pos);
	}
}

void gkEntity::setWorldPosition( float x, float y, float z )
{
	setWorldPosition(Vec3(x,y,z));
}

	
void gkEntity::_setParentEntity( gkEntity* ent )
{
	m_parentEntity = ent;
}
//-----------------------------------------------------------------------
void gkEntity::attachEntity( IGameObject* ent )
{
	if (ent)
	{
		gkEntity* entity = static_cast<gkEntity*>(ent->getRenderLayer());
		if (entity)
		{
			if (entity->getParentSceneNode())
			{
				if (_getParentEntity())
					_getParentEntity()->detachChildEntity(ent);
				entity->getParentSceneNode()->getParent()->removeChild(entity->getParentSceneNode());
				getParentSceneNode()->addChild(entity->getParentSceneNode());
				//m_vecChildEntityList.push_back(entity);
				entity->_setParentEntity(this);
			}
			else
			{
				gkLogWarning(_T("Attach Entity [ %s ] in dangrous way, not given scenenode."), entity->getName().c_str());
				getParentSceneNode()->createChildSceneNode(entity->getName())->attachObject(entity);
				//m_vecChildEntityList.push_back(entity);
				entity->_setParentEntity(this);
			}
			return;
		}

	}
	
	{
		gkLogWarning(_T("Attach Entity Failed. Target Entity not exist. I am [ %s ] ."), getName().c_str());
	}
}

void gkEntity::detachChildEntity( IGameObject* ent )
{
	if (ent)
	{
		// find the entity in this list [12/4/2011 Kaiming]
		gkEntity* entity = static_cast<gkEntity*>(ent->getRenderLayer());
		if (entity)
		{
			//m_vecChildEntityList.remove(entity);
			entity->_setParentEntity(NULL);

			// attach to SceneRoot [12/4/2011 Kaiming]
			entity->getParentSceneNode()->getParent()->removeChild(entity->getParentSceneNode());
			getSceneMngPtr()->getRootSceneNode()->addChild(entity->getParentSceneNode());
		}
	}
}
//////////////////////////////////////////////////////////////////////////
void gkEntity::Destroy()
{
	if (m_pParent)
	{
		// detach from parent
		m_pParent->detachFromFather();

		// detach all child
		m_pParent->detachAllChild();
	}


	// remove father scenenode at the same time [3/29/2012 Kaiming]
	getSceneMngPtr()->destroySceneNode(getParentSceneNode());

	// remove me
	getSceneMngPtr()->destroyMovableObject(this);
	
}
//////////////////////////////////////////////////////////////////////////
int gkEntity::getSubRenderableCount()
{
	return m_vecSubEntityList.size();
}

//-----------------------------------------------------------------------

void gkEntity::enableCastShadow( bool enable )
{
	gkSubRenderableList::iterator i, iend;
	iend = m_vecSubEntityList.end();
	for (i = m_vecSubEntityList.begin(); i != iend; ++i)
	{
		// Delete SubEntity
		if ( *i )
		{
			( *i )->enableShadow(enable);
		}
		
	}
}

gkRenderable* gkEntity::getSubRenderable( uint8 uIdx )
{
	if( uIdx < m_vecSubEntityList.size())
	{
		return m_vecSubEntityList[uIdx];
	}
	else
	{
		return NULL;
	}
}
