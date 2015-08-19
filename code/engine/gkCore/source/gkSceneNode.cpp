#include "gkStableHeader.h"
#include "gkSceneNode.h"
#include "gk3DEngine.h"
#include "gkSceneManager.h"
#include "gkMovableObject.h"
#include "IRenderSequence.h"
#include "gkCamera.h"
#include "IAuxRenderer.h"
#include "gkEntity.h"


//-----------------------------------------------------------------------
gkSceneNode::gkSceneNode()
: gkNode()
, m_bYawFixed(false)
, m_bIsInSceneGraph(false)
{
	needUpdate();
}
//-----------------------------------------------------------------------
gkSceneNode::gkSceneNode(const gkStdString& name)
: gkNode(name)
, m_bYawFixed(false)
, m_bIsInSceneGraph(false)
{
	needUpdate();
}
//-----------------------------------------------------------------------
gkSceneNode::~gkSceneNode()
{
	// 简单的删除一下
	gkMovableObject* ret;
	ObjectMap::iterator itr;
	for ( itr = m_mapObjectsByName.begin(); itr != m_mapObjectsByName.end(); itr++ )
	{
		ret = itr->second;
		ret->_notifyAttached((gkSceneNode*)NULL);
	}
	m_mapObjectsByName.clear();
}
//-----------------------------------------------------------------------
void gkSceneNode::updateFromParentImpl(void) const
{
	gkNode::updateFromParentImpl();

	// TODO：通知挂载的move objects
}
//-----------------------------------------------------------------------
gkNode* gkSceneNode::createChildImpl(void)
{
	assert(getSceneMngPtr());
	return getSceneMngPtr()->createSceneNode();
}
//-----------------------------------------------------------------------
gkNode* gkSceneNode::createChildImpl(const gkStdString& name)
{
	assert(getSceneMngPtr());
	return getSceneMngPtr()->createSceneNode(name);
}
//-----------------------------------------------------------------------
void gkSceneNode::setParent( gkNode* parent )
{
	gkNode::setParent(parent);

	if (parent)
	{
		gkSceneNode* sceneParent = static_cast<gkSceneNode*>(parent);
		setInSceneGraph(sceneParent->isInSceneGraph());
	}
	else
	{
		setInSceneGraph(false);
	}
}

//-----------------------------------------------------------------------
void gkSceneNode::setInSceneGraph( bool inGraph )
{
	m_bIsInSceneGraph = inGraph;
}

//-----------------------------------------------------------------------
bool gkSceneNode::isInSceneGraph()
{
	return m_bIsInSceneGraph;
}

//-----------------------------------------------------------------------
BYTE gkSceneNode::numAttachedObjects( void ) const
{
	return (BYTE)( m_mapObjectsByName.size() );
}

//-----------------------------------------------------------------------
gkMovableObject* gkSceneNode::getAttachedObject( BYTE index )
{
	if (index < m_mapObjectsByName.size())
	{
		ObjectMap::iterator i = m_mapObjectsByName.begin();
		// Increment (must do this one at a time)            
		while (index--)++i;

		return i->second;
	}
	else
	{
		gkLogWarning(_T("Some Object in index not find."));
		return NULL;
	}
}

//-----------------------------------------------------------------------
gkMovableObject* gkSceneNode::getAttachedObject( const gkStdString& name )
{
	// Look up 
	ObjectMap::iterator i = m_mapObjectsByName.find(name);

	if (i == m_mapObjectsByName.end())
	{
		gkLogWarning(_T("Some Object in name not find."));
	}

	return i->second;
}

//-----------------------------------------------------------------------
gkMovableObject* gkSceneNode::detachObject( BYTE index )
{
	gkMovableObject* ret;
	if (index < m_mapObjectsByName.size())
	{

		ObjectMap::iterator i = m_mapObjectsByName.begin();
		// Increment (must do this one at a time)            
		while (index--)++i;

		ret = i->second;
		m_mapObjectsByName.erase(i);
		//ret->_notifyAttached((SceneNode*)0);

		// Make sure bounds get updated (must go right to the top)
		needUpdate();

		return ret;

	}
	else
	{
		gkLogWarning(_T("Object index out of bounds. Couldn't find."));
		return NULL;
	}
}

//-----------------------------------------------------------------------
void gkSceneNode::detachObject( gkMovableObject* obj )
{
	ObjectMap::iterator i, iend;
	iend = m_mapObjectsByName.end();
	for (i = m_mapObjectsByName.begin(); i != iend; ++i)
	{
		if (i->second == obj)
		{
			m_mapObjectsByName.erase(i);
			break;
		}
	}
	//obj->_notifyAttached((SceneNode*)0);

	// Make sure bounds get updated (must go right to the top)
	needUpdate();
}

//-----------------------------------------------------------------------
gkMovableObject* gkSceneNode::detachObject( const gkStdString& name )
{
	ObjectMap::iterator it = m_mapObjectsByName.find(name);
	if (it == m_mapObjectsByName.end())
	{
		//OGRE_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Object " + name + " is not attached "
		//	"to this node.", "SceneNode::detachObject");
	}
	gkMovableObject* ret = it->second;
	m_mapObjectsByName.erase(it);
	//ret->_notifyAttached((SceneNode*)0);
	// Make sure bounds get updated (must go right to the top)
	needUpdate();

	return ret;
}

//-----------------------------------------------------------------------
void gkSceneNode::detachAllObjects( void )
{
	ObjectMap::iterator itr;
	gkMovableObject* ret;
	for ( itr = m_mapObjectsByName.begin(); itr != m_mapObjectsByName.end(); itr++ )
	{
		ret = itr->second;
		ret->_notifyAttached((gkSceneNode*)NULL);
	}
	m_mapObjectsByName.clear();
	// Make sure bounds get updated (must go right to the top)
	needUpdate();
}

//-----------------------------------------------------------------------
void gkSceneNode::_update( bool updateChildren, bool parentHasChanged )
{
	gkNode::_update(updateChildren, parentHasChanged);
}

//-----------------------------------------------------------------------
void gkSceneNode::_findVisibleObjects( gkCamera* cam, IRenderSequence* sequence, bool includeChildren /*= true*/, bool displayNodes /*= false*/ )
{
	// Add all entities
	ObjectMap::iterator iobj;
	ObjectMap::iterator iobjend = m_mapObjectsByName.end();

	// Add to renderSequence
	for (iobj = m_mapObjectsByName.begin(); iobj != iobjend; ++iobj)
	{
		gkMovableObject* mo = iobj->second;


		// draw aabb
		const AABB& aabb = mo->getWorldAABB(true);
		if (mo->getShowAABB() && mo->getMovableType() == gkEntityFactory::FACTORY_TYPE_NAME && !cam->isLightCamera())
		{
			ColorF color = ColorF(0,1.0,0.2,0.9);
			gEnv->pRenderer->getAuxRenderer()->AuxRenderAABB(aabb, color);
			gEnv->pRenderer->getAuxRenderer()->AuxRenderMeshFrame( (gkEntity*)mo, color );
		}

		if ( mo -> getVisible() )
		{
			// 根据hidemask来判断，如果摄像机和物体的hidemask置位了，此相机看可以看见此物体
			if (mo->getRenderLayer() == RENDER_LAYER_SKIES_EARLY && cam->isLightCamera())
			{
				return;
			}
			if( cam->getHideMask() & mo->getHideMask() )
			{
 				if (cam->checkRenderable(aabb) || mo->getRenderLayer() == RENDER_LAYER_SKIES_EARLY)
 				{
					mo -> _updateRenderSequence( sequence );
				}
			}
		}
	}

	if (includeChildren)
	{
		ChildNodeMap::iterator child, childend;
		childend = m_mapChildren.end();
		for (child = m_mapChildren.begin(); child != childend; ++child)
		{
			gkSceneNode* sceneChild = static_cast<gkSceneNode*>(child->second);
			sceneChild->_findVisibleObjects(cam, sequence, includeChildren, 
				displayNodes);
		}
	}

// 	if (displayNodes)
// 	{
// 		// Include self in the render queue
// 		queue->addRenderable(getDebugRenderable());
// 	}
}
//-----------------------------------------------------------------------
void gkSceneNode::attachObject( gkMovableObject* obj )
{
	assert(obj);
	if(!obj)
	{
		gkLogWarning(_T("Object ref NULL!"));
		return;
	}

	if (obj->isAttached())
	{
		gkLogWarning(_T("Object already attached to a SceneNode or a Bone"));
	}

	obj->_notifyAttached(this);

	// Also add to name index
	std::pair<ObjectMap::iterator, bool> insresult = 
		m_mapObjectsByName.insert(ObjectMap::value_type(obj->getName(), obj));
	//assert(insresult.second && "Object was not attached because an object of the "
	//	"same name was already attached to this node.");

	// Make sure bounds get updated (must go right to the top)
	needUpdate();
}
//-----------------------------------------------------------------------
gkSceneNode::ObjectIterator gkSceneNode::getAttachedObjectIterator( void )
{
	return ObjectIterator(m_mapObjectsByName.begin(), m_mapObjectsByName.end());
}
//-----------------------------------------------------------------------
gkSceneNode::ConstObjectIterator gkSceneNode::getAttachedObjectIterator( void ) const
{
	return ConstObjectIterator(m_mapObjectsByName.begin(), m_mapObjectsByName.end());
}
//-----------------------------------------------------------------------
void gkSceneNode::removeAndDestroyChild( const gkStdString& name )
{
	gkSceneNode* pChild = static_cast<gkSceneNode*>(getChild(name));
	pChild->removeAndDestroyAllChildren();

	removeChild(name);
	if (getSceneMngPtr())
	{
		getSceneMngPtr()->destroySceneNode(name);
	}
}
//-----------------------------------------------------------------------
void gkSceneNode::removeAndDestroyAllChildren( void )
{
	ChildNodeMap::iterator i, iend;
	iend = m_mapChildren.end();
	for (i = m_mapChildren.begin(); i != iend;)
	{
		gkSceneNode* sn = static_cast<gkSceneNode*>(i->second);
		// increment iterator before destroying (iterator invalidated by 
		// SceneManager::destroySceneNode because it causes removal from parent)
		++i;
		sn->removeAndDestroyAllChildren();
		getSceneMngPtr()->destroySceneNode(sn->getName());
	}
	m_mapChildren.clear();
	needUpdate();
}
//-----------------------------------------------------------------------
gkSceneNode* gkSceneNode::createChildSceneNode(const Vec3& translate, 
										   const Quat& rotate)
{
	return static_cast<gkSceneNode*>(this->createChild(translate, rotate));
}
//-----------------------------------------------------------------------
gkSceneNode* gkSceneNode::createChildSceneNode(const gkStdString& name, const Vec3& translate, 
										   const Quat& rotate)
{
	return static_cast<gkSceneNode*>(this->createChild(name, translate, rotate));
}
//-----------------------------------------------------------------------
void gkSceneNode::setVisible( bool visible, bool cascade /*= true*/ )
{
	ObjectMap::iterator oi, oiend;
	oiend = m_mapObjectsByName.end();
	for (oi = m_mapObjectsByName.begin(); oi != oiend; ++oi)
	{
		oi->second->setVisible(visible);
	}
}