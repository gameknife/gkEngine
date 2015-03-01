#include "StableHeader.h"
#include "gkGameObject.h"
#include "IGameObjectLayer.h"
#include "IMesh.h"


	Quat gkGameObject::ms_IdentityOrientation = Quat::CreateIdentity();
	Vec3 gkGameObject::ms_IdentityPos = Vec3(0,0,0);
	Vec3 gkGameObject::ms_IdentityScale = Vec3(0,0,0);
	
	Matrix44 gkGameObject::ms_IdentityMatrix = Matrix44(IDENTITY);
	//////////////////////////////////////////////////////////////////////////
	gkGameObject::gkGameObject( uint32 id, const gkStdString& name ):
		m_uID(id)
		,m_wstrName(name)
		,m_pRenderLayer(NULL)
		,m_pPhysicLayer(NULL)
		,m_pParent(NULL)
		,m_physicEnable(false)
	{
		m_lstChilds.clear();
		m_lstLayers.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	IGameObject* gkGameObject::clone( const gkStdString& newName ) const
	{
		IGameObject* ret = NULL;
		
		if ( m_pRenderLayer )
		{
			Vec3 pos = getPosition();
			Quat rot = getOrientation();

			static int name_gen = 0;
			TCHAR new_name[255];
			_tcscpy(new_name, getName().c_str());
			_stprintf( new_name, _T("%s_%d"), new_name, name_gen++ );

			ret = gEnv->pGameObjSystem->CreateStaticGeoGameObject( new_name, m_pRenderLayer->getMesh()->getName(), pos, rot );
			ret->setScale( m_pRenderLayer->getScale() );
			ret->getRenderLayer()->setMaterialName( getMaterialName() );
		}	
		


		return ret;
	}
	//////////////////////////////////////////////////////////////////////////
	void gkGameObject::attachChild( IGameObject* ent )
	{
		if (m_pRenderLayer)
		{
			gkGameObject* entity = reinterpret_cast<gkGameObject*>(ent);
			m_pRenderLayer->attachEntity(entity);
			m_lstChilds.push_back(entity);

			entity->setParent(this);			
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void gkGameObject::detachChild( IGameObject* ent )
	{
		if (m_pRenderLayer)
		{
			gkGameObject* entity = reinterpret_cast<gkGameObject*>(ent);
			m_pRenderLayer->detachChildEntity(entity);
			gkGameObjectList::iterator it = m_lstChilds.begin();
			for (; it != m_lstChilds.end(); )
			{
				if ( (*it) == entity )
				{
					
					entity->setParent(NULL);
					it = m_lstChilds.erase(it);
				}
				else
				{
					++it;
				}
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void gkGameObject::detachAllChild()
	{
		if (m_pRenderLayer)
		{
			gkGameObjectList::iterator it = m_lstChilds.begin();
			for (; it != m_lstChilds.end(); ++it)
			{
				m_pRenderLayer->detachChildEntity( *it );
				(*it)->setParent(NULL);
			}
			m_lstChilds.clear();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void gkGameObject::detachFromFather()
	{
		if (m_pParent)
		{
			m_pParent->detachChild(this);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	IGameObject* gkGameObject::getChild( uint32 index )
	{
		IGameObject* ret = NULL;

		ret = m_lstChilds[index];

		return ret;
	}
	//////////////////////////////////////////////////////////////////////////
	void gkGameObject::setOrientation( const Quat& q )
	{
		if (m_pRenderLayer)
		{
			m_pRenderLayer->setOrientation(q);
		}		
	}
	//////////////////////////////////////////////////////////////////////////
	void gkGameObject::setOrientation( float w, float x, float y, float z )
	{
		if (m_pRenderLayer)
		{
			m_pRenderLayer->setOrientation(w,x,y,z);
		}		
	}
	//////////////////////////////////////////////////////////////////////////
	const Quat & gkGameObject::getOrientation() const
	{
		if (m_pRenderLayer)
		{
			return m_pRenderLayer->getOrientation();
		}
		return ms_IdentityOrientation;
	}
	//////////////////////////////////////////////////////////////////////////
	const Quat & gkGameObject::getWorldOrientation() const
	{
		if (m_pRenderLayer)
		{
			return m_pRenderLayer->getWorldOrientation();
		}
		return ms_IdentityOrientation;
	}
	//////////////////////////////////////////////////////////////////////////
	void gkGameObject::setPosition( const Vec3& pos )
	{
		if (m_pRenderLayer)
		{
			m_pRenderLayer->setPosition(pos);
		}	
	}
	//////////////////////////////////////////////////////////////////////////
	void gkGameObject::setPosition( float x, float y, float z )
	{
		if (m_pRenderLayer)
		{
			m_pRenderLayer->setPosition(x,y,z);
		}	
	}
	//////////////////////////////////////////////////////////////////////////
	void gkGameObject::setWorldPosition( const Vec3& pos )
	{
		if (m_pRenderLayer)
		{
			m_pRenderLayer->setWorldPosition(pos);
		}	
	}
	//////////////////////////////////////////////////////////////////////////
	void gkGameObject::setWorldPosition( float x, float y, float z )
	{
		if (m_pRenderLayer)
		{
			m_pRenderLayer->setWorldPosition(x,y,z);
		}	
	}
	//////////////////////////////////////////////////////////////////////////
	const Vec3 & gkGameObject::getPosition( void ) const
	{
		if (m_pRenderLayer)
		{
			return m_pRenderLayer->getPosition();
		}
		return ms_IdentityPos;
	}
	//////////////////////////////////////////////////////////////////////////
	const Vec3& gkGameObject::getWorldPosition( void ) const
	{
		if (m_pRenderLayer)
		{
			return m_pRenderLayer->getWorldPosition();
		}
		return ms_IdentityPos;
	}
	//////////////////////////////////////////////////////////////////////////
	void gkGameObject::setScale( const Vec3& scale )
	{
		if (m_pRenderLayer)
		{
			m_pRenderLayer->setScale(scale);
		}	
	}
	//////////////////////////////////////////////////////////////////////////
	void gkGameObject::setScale( float x, float y, float z )
	{
		if (m_pRenderLayer)
		{
			m_pRenderLayer->setScale(x,y,z);
		}	
	}
	//////////////////////////////////////////////////////////////////////////
	const Vec3 & gkGameObject::getScale( void ) const
	{
		if (m_pRenderLayer)
		{
			return m_pRenderLayer->getScale();
		}
		return ms_IdentityScale;
	}
	//////////////////////////////////////////////////////////////////////////
	const Matrix44& gkGameObject::GetWorldMatrix()
	{
		if (m_pRenderLayer)
		{
			return m_pRenderLayer->GetWorldMatrix();
		}
		return ms_IdentityMatrix;
	}
	//////////////////////////////////////////////////////////////////////////
	void gkGameObject::translate( const Vec3& d, gkTransformSpace relativeTo /*= TS_PARENT*/ )
	{
		if (m_pRenderLayer)
		{
			m_pRenderLayer->translate(d, relativeTo);
		}	
	}
	//////////////////////////////////////////////////////////////////////////
	void gkGameObject::translate( float x, float y, float z, gkTransformSpace relativeTo /*= TS_PARENT*/ )
	{
		if (m_pRenderLayer)
		{
			m_pRenderLayer->translate(x, y, z, relativeTo);
		}	
	}
	//////////////////////////////////////////////////////////////////////////
	void gkGameObject::rotate( const Vec3& axis, float angle, gkTransformSpace relativeTo /*= TS_LOCAL*/ )
	{
		if (m_pRenderLayer)
		{
			m_pRenderLayer->rotate(axis, angle, relativeTo);
		}	
	}
	//////////////////////////////////////////////////////////////////////////
	void gkGameObject::rotate( const Quat& q, gkTransformSpace relativeTo /*= TS_LOCAL*/ )
	{
		if (m_pRenderLayer)
		{
			m_pRenderLayer->rotate(q, relativeTo);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void gkGameObject::lookat( const Vec3& point, bool bHeadup /*= true*/ )
	{
		if (m_pRenderLayer)
		{
			m_pRenderLayer->lookat(point, bHeadup);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void gkGameObject::smoothLookat( const Vec3& point, float fDamping, bool bHeadup /*= true*/ )
	{
		if (m_pRenderLayer)
		{
			m_pRenderLayer->smoothLookat(point, fDamping, bHeadup);
		}
	}

	//////////////////////////////////////////////////////////////////////////
	void gkGameObject::setGameObjectLayer( IGameObjectLayer* layer)
	{
		switch (layer->getType())
		{
		case eGL_RenderLayer:
			m_pRenderLayer = reinterpret_cast<IGameObjectRenderLayer*>(layer);
			m_lstLayers.push_back(layer);
			break;
		case eGL_PhysicLayer:
			m_pPhysicLayer = reinterpret_cast<IGameObjectPhysicLayer*>(layer);
			m_lstLayers.push_back(layer);
			m_physicEnable = true;
			break;			
		default:
			m_lstLayers.push_back(layer);
		}

		if(layer)
		{
			layer->_setParentGameObject( this );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void gkGameObject::setVisible( bool bVisible )
	{
		if (m_pRenderLayer)
		{
			m_pRenderLayer->setVisible(bVisible);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void gkGameObject::preupdate( float fElapsedTime )
	{
		gkGameObjectLayerList::iterator it = m_lstLayers.begin();
		for ( ; it != m_lstLayers.end(); ++it)
		{
			(*it)->PreUpdateLayer(fElapsedTime);
		}
	}

	//////////////////////////////////////////////////////////////////////////
	void gkGameObject::update( float fElapsedTime )
	{
		gkGameObjectLayerList::iterator it = m_lstLayers.begin();
		for ( ; it != m_lstLayers.end(); ++it)
		{
			(*it)->UpdateLayer(fElapsedTime);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void gkGameObject::destroy()
	{
		gkGameObjectLayerList::iterator it = m_lstLayers.begin();
		for ( ; it != m_lstLayers.end(); ++it)
		{
			(*it)->Destroy();
		}		
	}
	//////////////////////////////////////////////////////////////////////////
	IGameObjectLayer* gkGameObject::getGameObjectLayer( EGameObjectLayerType layertype )
	{
		gkGameObjectLayerList::iterator it = m_lstLayers.begin();
		for ( ; it != m_lstLayers.end(); ++it)
		{
			if((*it)->getType() == layertype)
			{
				return (*it);
			}
		}
		return NULL;
	}
	//////////////////////////////////////////////////////////////////////////
	const gkStdString& gkGameObject::getMaterialName() const
	{
		if (m_pRenderLayer)
		{
			return m_pRenderLayer->getMaterialName();
		}

		return GKNULLSTR;
	}
	//////////////////////////////////////////////////////////////////////////
	void gkGameObject::showBBox(bool bShow)
	{
		if (m_pRenderLayer)
		{
			return m_pRenderLayer->showBBox(bShow);
		}
	}

	const AABB& gkGameObject::getAABB() const
	{
		if (m_pRenderLayer)
		{
			return m_pRenderLayer->getAABB();
		}

		static AABB NULLAABB;

		return NULLAABB;
	}

	const AABB& gkGameObject::getWorldAABB() const
	{
		if (m_pRenderLayer)
		{
			return m_pRenderLayer->getWorldAABB();
		}

		static AABB NULLAABB;

		return NULLAABB;
	}

