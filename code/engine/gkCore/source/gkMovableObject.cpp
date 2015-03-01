#include "gkStableHeader.h"
#include "gkMovableObject.h"
#include "gkSceneNode.h"
#include "IRenderSequence.h"

#include "gkMemoryLeakDetecter.h"

//-----------------------------------------------------------------------
gkMovableObject::gkMovableObject()
: m_pManager(0)
, m_pParentNode(0)
, m_bIsVisible(true)
, m_bIsCastShadow(true)
, m_bIsReceiveShadow(false)
, m_bIsRenderLayerSet(false)
, m_yRenderLayer(RENDER_LAYER_OPAQUE)
, m_yHideMask(GK_CAMERA_VIEWMASK_DEFAUT)
, m_uQueryFlag(0)
, m_showAABB(false)
{
	m_WorldAABB.Reset();
}
//-----------------------------------------------------------------------
gkMovableObject::gkMovableObject(const gkStdString& name)
: m_wstrName(name)
, m_pManager(0)
, m_pParentNode(0)
, m_bIsVisible(true)
, m_bIsCastShadow(true)
, m_bIsReceiveShadow(false)
, m_bIsRenderLayerSet(false)
, m_yRenderLayer(RENDER_LAYER_OPAQUE)
, m_yHideMask(GK_CAMERA_VIEWMASK_DEFAUT)
, m_uQueryFlag(0)
, m_showAABB(false)
{
	m_WorldAABB.Reset();
}
//-----------------------------------------------------------------------
gkMovableObject::~gkMovableObject()
{
	if (m_pParentNode)
	{
		// May be we are a lod entity which not in the parent node child object list,
		// call this method could safely ignore this case.
		static_cast<gkSceneNode*>(m_pParentNode)->detachObject(this);
	}
}

gkNode* gkMovableObject::getParentNode( void ) const
{
	return m_pParentNode;
}

gkSceneNode* gkMovableObject::getParentSceneNode( void ) const
{
	// 暂时返回cast [9/26/2010 Kaiming-Laptop]
	return static_cast<gkSceneNode*>(m_pParentNode);
}

bool gkMovableObject::isAttached( void ) const
{
	return (m_pParentNode != NULL);
}

//-----------------------------------------------------------------------
void gkMovableObject::_notifyAttached(gkNode* parent)
{
	//assert(!m_pParentNode || !parent);

	bool different = (parent != m_pParentNode);

	m_pParentNode = parent;
}
//-----------------------------------------------------------------------
const Matrix34& gkMovableObject::_getParentNodeFullTransform( void ) const
{
	if(m_pParentNode)
	{
		// object attached to a sceneNode
		return m_pParentNode->_getFullTransform();
	}
	// fallback
	GK_ASSERT(0);
	static Matrix34 matIdent;
	matIdent.SetIdentity();
	return matIdent;
}
//-----------------------------------------------------------------------
void gkMovableObject::setRenderLayer( BYTE queueID )
{
	m_bIsRenderLayerSet = true;
	m_yRenderLayer = queueID;
}
//-----------------------------------------------------------------------
BYTE gkMovableObject::getRenderLayer()
{
	return m_yRenderLayer;
}
//-----------------------------------------------------------------------
const AABB& gkMovableObject::getWorldAABB(bool derive) const
{
	if (derive)
	{
		m_WorldAABB = this->getAABB();
		m_WorldAABB = AABB::CreateTransformedAABB( _getParentNodeFullTransform(), m_WorldAABB);
	}

	return m_WorldAABB;
}

//-----------------------------------------------------------------------
gkMovableObject* gkMovableObjectFactory::createInstance( const gkStdString& name, const NameValuePairList* param )
{
	gkMovableObject* m = createInstanceImpl(name, param);
	return m;

}