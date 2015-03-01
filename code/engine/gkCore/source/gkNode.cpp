#include "gkStableHeader.h"
#include "gkNode.h"

gkNameGenerator gkNode::ms_NameGenerator(_T("gkNode_"));
gkNode::QueuedUpdates gkNode::m_vecQueuedUpdates;

gkNode::gkNode( void )
:m_pParent(NULL),
m_bNeedParentUpdate(false),
m_bNeedChildUpdate(false),
m_bParentNotified(false),
m_bQueuedForUpdate(false),
m_bInheritOrientation(true),
m_bInheritScale(false),
m_bCachedTransformOutOfDate(true)
{
	m_vPosition = Vec3(0,0,0);
	m_qOrientation = Quat(1,0,0,0);
	m_vScale = Vec3(1,1,1);
	m_wstrName = ms_NameGenerator.generate();
	m_matCachedTransform.SetIdentity();
	needUpdate();
}

gkNode::gkNode( gkStdString wstrName )
:m_pParent(NULL),
m_bNeedParentUpdate(false),
m_bNeedChildUpdate(false),
m_bParentNotified(false),
m_bQueuedForUpdate(false),
m_bInheritOrientation(true),
m_bInheritScale(false),
m_bCachedTransformOutOfDate(true)
{
	m_vPosition = Vec3(0,0,0);
	m_qOrientation = Quat(1,0,0,0);
	m_vScale = Vec3(1,1,1);
	m_wstrName = wstrName;
	m_matCachedTransform.SetIdentity();
	needUpdate();
}

gkNode::~gkNode( void )
{
	removeAllChildren();
	if(m_pParent)
		m_pParent->removeChild(this);
}

//-----------------------------------------------------------------------
gkNode* gkNode::getParent(void) const
{
	return m_pParent;
}

//-----------------------------------------------------------------------
void gkNode::setParent(gkNode* parent)
{
	bool different = (parent != m_pParent);

	m_pParent = parent;
	// Request update from parent
	m_bParentNotified = false ;
	needUpdate();
}

//-----------------------------------------------------------------------
void gkNode::_update(bool updateChildren, bool parentHasChanged)
{
	// always clear information about parent notification
	m_bParentNotified = false ;

	// Short circuit the off case
	if (!updateChildren && !m_bNeedParentUpdate && !m_bNeedChildUpdate && !parentHasChanged )
	{
		return;
	}


	// See if we should process everyone
	if (m_bNeedParentUpdate || parentHasChanged)
	{
		// Update transforms from parent
		_updateFromParent();
	}

	if (m_bNeedChildUpdate || parentHasChanged)
	{

		ChildNodeMap::iterator it, itend;
		itend = m_mapChildren.end();
		for (it = m_mapChildren.begin(); it != itend; ++it)
		{
			gkNode* child = it->second;
			child->_update(true, true);
		}
		m_setChildrenToUpdate.clear();
	}
	else
	{
		// Just update selected children

		ChildUpdateSet::iterator it, itend;
		itend = m_setChildrenToUpdate.end();
		for(it = m_setChildrenToUpdate.begin(); it != itend; ++it)
		{
			gkNode* child = *it;
			child->_update(true, false);
		}

		m_setChildrenToUpdate.clear();
	}

	m_bNeedChildUpdate = false;

}
//-----------------------------------------------------------------------
void gkNode::_updateFromParent(void) const
{
	updateFromParentImpl();
}
//-----------------------------------------------------------------------
void gkNode::updateFromParentImpl(void) const
{
	if (m_pParent)
	{
		// Update orientation
		const Quat& parentOrientation = m_pParent->_getDerivedOrientation();
		if (m_bInheritOrientation)
		{
			// Combine orientation with that of parent
			m_qDerivedOrientation = parentOrientation * m_qOrientation;
		}
		else
		{
			// No inheritence
			m_qDerivedOrientation = m_qOrientation;
		}

		// Update scale
		const Vec3& parentScale = m_pParent->_getDerivedScale();
		if (m_bInheritScale)
		{
			// Scale own position by parent scale, NB just combine
			// as equivalent axes, no shearing
			m_vDerivedScale = parentScale.CompMul(m_vScale);
		}
		else
		{
			// No inheritence
			m_vDerivedScale = m_vScale;
		}

		// Change position vector based on parent's orientation & scale
		m_vDerivedPosition = parentOrientation * parentScale.CompMul(m_vPosition);

		// Add altered position vector to parents
		m_vDerivedPosition += m_pParent->_getDerivedPosition();
	}
	else
	{
		// Root node, no parent
		m_qDerivedOrientation = m_qOrientation;
		m_vDerivedPosition = m_vPosition;
		m_vDerivedScale = m_vScale;
	}

	m_bCachedTransformOutOfDate = true;
	m_bNeedParentUpdate = false;

}

//-----------------------------------------------------------------------
void gkNode::needUpdate(bool forceParentUpdate)
{

	m_bNeedParentUpdate = true;
	m_bNeedChildUpdate = true;
	m_bCachedTransformOutOfDate = true;

	// Make sure we're not root and parent hasn't been notified before
	if (m_pParent && (!m_bParentNotified || forceParentUpdate))
	{
		m_pParent->requestUpdate(this, forceParentUpdate);
		m_bParentNotified = true ;
	}

	// all children will be updated
	m_setChildrenToUpdate.clear();
}
//-----------------------------------------------------------------------
void gkNode::requestUpdate(gkNode* child, bool forceParentUpdate)
{
	// If we're already going to update everything this doesn't matter
	if (m_bNeedChildUpdate)
	{
		return;
	}

	m_setChildrenToUpdate.insert(child);
	// Request selective update of me, if we didn't do it before
	if (m_pParent && (!m_bParentNotified || forceParentUpdate))
	{
		m_pParent->requestUpdate(this, forceParentUpdate);
		m_bParentNotified = true ;
	}
}
//-----------------------------------------------------------------------
void gkNode::cancelUpdate(gkNode* child)
{
	m_setChildrenToUpdate.erase(child);

	// Propogate this up if we're done
	if (m_setChildrenToUpdate.empty() && m_pParent && !m_bNeedChildUpdate)
	{
		m_pParent->cancelUpdate(this);
		m_bParentNotified = false ;
	}
}
//-----------------------------------------------------------------------
void gkNode::queueNeedUpdate(gkNode* n)
{
	// Don't queue the node more than once
	if (!n->m_bQueuedForUpdate)
	{
		n->m_bQueuedForUpdate = true;
		m_vecQueuedUpdates.push_back(n);
	}
}
//-----------------------------------------------------------------------
void gkNode::processQueuedUpdates(void)
{
	for (QueuedUpdates::iterator i = m_vecQueuedUpdates.begin();
		i != m_vecQueuedUpdates.end(); ++i)
	{
		// Update, and force parent update since chances are we've ended
		// up with some mixed state in there due to re-entrancy
		gkNode* n = *i;
		n->m_bQueuedForUpdate = false;
		n->needUpdate(true);
	}
	m_vecQueuedUpdates.clear();
}

uint32 gkNode::getChildCount() const
{
	return m_mapChildren.size();
}

//-----------------------------------------------------------------------
gkNode* gkNode::getChild(const gkStdString& name) const
{
	ChildNodeMap::const_iterator i = m_mapChildren.find(name);

	if (i == m_mapChildren.end())
	{
		gkLogError( _T("Child node named [%s] does not exist.\n"), name.c_str() );
	}
	return i->second;

}
//-----------------------------------------------------------------------
gkNode* gkNode::removeChild(const gkStdString& name)
{
	ChildNodeMap::iterator i = m_mapChildren.find(name);

	if (i == m_mapChildren.end())
	{
		gkLogError( _T("Child node named [%s] does not exist.\n"), name.c_str() );
	}

	gkNode* ret = i->second;
	// Cancel any pending update
	cancelUpdate(ret);

	m_mapChildren.erase(i);
	ret->setParent(NULL);

	return ret;
}
//-----------------------------------------------------------------------
gkNode* gkNode::createChild(const Vec3& translate, const Quat& rotate)
{
	gkNode* newNode = createChildImpl();
	newNode->translate(translate);
	newNode->rotate(rotate);
	this->addChild(newNode);

	return newNode;
}
//-----------------------------------------------------------------------
gkNode* gkNode::createChild(const gkStdString& name, const Vec3& translate, const Quat& rotate)
{
	gkNode* newNode = createChildImpl(name);
	newNode->translate(translate);
	newNode->rotate(rotate);
	this->addChild(newNode);

	return newNode;
}
//-----------------------------------------------------------------------
void gkNode::addChild(gkNode* child)
{
	if (child->m_pParent)
	{
		gkLogError( _T("Node named [%s] already was a child of [].\n"), child->getName().c_str(), child->m_pParent->getName().c_str() );
	}

	m_mapChildren.insert(ChildNodeMap::value_type(child->getName(), child));
	child->setParent(this);

}
//-----------------------------------------------------------------------
unsigned short gkNode::numChildren(void) const
{
	return static_cast< unsigned short >( m_mapChildren.size() );
}
//-----------------------------------------------------------------------
gkNode* gkNode::getChild(unsigned short index) const
{
	if( index < m_mapChildren.size() )
	{
		ChildNodeMap::const_iterator i = m_mapChildren.begin();
		while (index--) ++i;
		return i->second;
	}
	else
		return NULL;
}
//-----------------------------------------------------------------------
gkNode* gkNode::removeChild(unsigned short index)
{
	gkNode* ret;
	if (index < m_mapChildren.size())
	{
		ChildNodeMap::iterator i = m_mapChildren.begin();
		while (index--) ++i;
		ret = i->second;
		// cancel any pending update
		cancelUpdate(ret);

		m_mapChildren.erase(i);
		ret->setParent(NULL);
		return ret;
	}
	else
	{
		gkLogError( _T("Child index out of bounds.") );
	}
	return 0;
}
//-----------------------------------------------------------------------
gkNode* gkNode::removeChild(gkNode* child)
{
	if (child)
	{
		ChildNodeMap::iterator i = m_mapChildren.find(child->getName());
		// ensure it's our child
		if (i != m_mapChildren.end() && i->second == child)
		{
			// cancel any pending update
			cancelUpdate(child);

			m_mapChildren.erase(i);
			child->setParent(NULL);
		}
	}
	return child;
}

//-----------------------------------------------------------------------
void gkNode::removeAllChildren(void)
{
	ChildNodeMap::iterator i, iend;
	iend = m_mapChildren.end();
	for (i = m_mapChildren.begin(); i != iend; ++i)
	{
		i->second->setParent(NULL);
	}
	m_mapChildren.clear();
	m_setChildrenToUpdate.clear();
}
//-----------------------------------------------------------------------
void gkNode::translate( const Vec3& d, gkTransformSpace relativeTo /*= TS_PARENT*/ )
{
	switch(relativeTo)
	{
	case TS_LOCAL:
		// position is relative to parent so transform downwards
		m_vPosition +=  m_qOrientation * d;
		break;
	case TS_WORLD:
		// position is relative to parent so transform upwards
		if (m_pParent)
		{
			Quat qTemp = m_pParent->_getDerivedOrientation().GetInverted();
			Vec3 vTemp = qTemp * d;
			m_vPosition += vTemp.sub(m_pParent->_getDerivedScale());
		}
		else
		{
			m_vPosition += d;
		}
		break;
	case TS_PARENT:
		m_vPosition += d;
		break;
	}
	needUpdate();
}
//-----------------------------------------------------------------------
void gkNode::translate( float x, float y, float z, gkTransformSpace relativeTo /*= TS_PARENT*/ )
{
	Vec3 vTemp(x,y,z);
	translate(vTemp, relativeTo);
}
//-----------------------------------------------------------------------
void gkNode::rotate( const Vec3& axis, float angle, gkTransformSpace relativeTo /*= TS_LOCAL*/ )
{
	Quat q = Quat::CreateRotationAA(angle, axis);
	rotate(q, relativeTo);
}
//-----------------------------------------------------------------------
void gkNode::rotate( const Quat& q, gkTransformSpace relativeTo /*= TS_LOCAL*/ )
{
	switch(relativeTo)
	{
	case TS_PARENT:
		// Rotations are normally relative to local axes, transform up
		m_qOrientation = m_qOrientation * q.GetNormalized();
		break;
	case TS_WORLD:
		{
			// Rotations are normally relative to local axes, transform up
			m_qOrientation = m_qOrientation * _getDerivedOrientation().GetInverted() * q.GetNormalized() * _getDerivedOrientation();
		}
		break;
	case TS_LOCAL:
		// Note the order of the mult, i.e. q comes after
		m_qOrientation = q.GetNormalized() * m_qOrientation;
		break;
	}
	needUpdate();
}
//-----------------------------------------------------------------------
const Matrix34& gkNode::_getFullTransform(void) const
{
	if (m_bCachedTransformOutOfDate)
	{
		Matrix34 tmp;
		tmp.SetIdentity();
		tmp.Set(_getDerivedScale(), _getDerivedOrientation(), _getDerivedPosition());
		m_matCachedTransform = tmp;
		// Use derived values
		//D3DXMatrixTransformation(&m_matCachedTransform, NULL, NULL, &_getDerivedScale(),NULL, &_getDerivedOrientation(), &_getDerivedPosition());
		m_bCachedTransformOutOfDate = false;
	}
	return m_matCachedTransform;
}

const Quat & gkNode::getOrientation() const
{
	return m_qOrientation;
}

//-----------------------------------------------------------------------
void gkNode::setOrientation( const Quat & q )
{
	//assert(!q.isNaN() && "Invalid orientation supplied as parameter");
	m_qOrientation = q.GetNormalized();
	needUpdate();
}
//-----------------------------------------------------------------------
void gkNode::setOrientation( float w, float x, float y, float z )
{
	setOrientation(Quat(w, x, y, z));
}
//-----------------------------------------------------------------------
void gkNode::setPosition(const Vec3& pos)
{
	//assert(!pos.isNaN() && "Invalid vector supplied as parameter");
	m_vPosition = pos;
	needUpdate();
}


//-----------------------------------------------------------------------
void gkNode::setPosition(float x, float y, float z)
{
	Vec3 v(x,y,z);
	setPosition(v);
}

//-----------------------------------------------------------------------
const Vec3 & gkNode::getPosition(void) const
{
	return m_vPosition;
}

//-----------------------------------------------------------------------
void gkNode::setScale(const Vec3& scale)
{
	//assert(!scale.isNaN() && "Invalid vector supplied as parameter");
	m_vScale = scale;
	needUpdate();
}
//-----------------------------------------------------------------------
void gkNode::setScale(float x, float y, float z)
{
	setScale(Vec3(x, y, z));
}
//-----------------------------------------------------------------------
const Vec3 & gkNode::getScale(void) const
{
	return m_vScale;
}

//-----------------------------------------------------------------------
Vec3 gkNode::convertWorldToLocalPosition( const Vec3 &worldPos )
{
	if (m_bNeedParentUpdate)
	{
		_updateFromParent();
	}
	Vec3 vTmp = m_qDerivedOrientation.GetInverted() * m_vDerivedPosition;
	vTmp.x /= m_vDerivedScale.x;
	vTmp.y /= m_vDerivedScale.y;
	vTmp.z /= m_vDerivedScale.z;

	return vTmp;
}
//-----------------------------------------------------------------------
Vec3 gkNode::convertLocalToWorldPosition( const Vec3 &localPos )
{
	if (m_bNeedParentUpdate)
	{
		_updateFromParent();
	}
	Vec3 vTmp =  m_qDerivedOrientation * localPos.CompMul(m_vDerivedScale);
	return vTmp + m_vDerivedPosition;
}
//-----------------------------------------------------------------------
Quat gkNode::convertWorldToLocalOrientation( const Quat &worldOrientation )
{
	if (m_bNeedParentUpdate)
	{
		_updateFromParent();
	}
	return m_qDerivedOrientation.GetInverted() * worldOrientation;
}
//-----------------------------------------------------------------------
Quat gkNode::convertLocalToWorldOrientation( const Quat &localOrientation )
{
	if (m_bNeedParentUpdate)
	{
		_updateFromParent();
	}
	return m_qDerivedOrientation * localOrientation;

}

//-----------------------------------------------------------------------
void gkNode::_setDerivedPosition( const Vec3& pos )
{
	//find where the node would end up in parent's local space
	setPosition( m_pParent->convertWorldToLocalPosition( pos ) );
}
//-----------------------------------------------------------------------
void gkNode::_setDerivedOrientation( const Quat& q )
{
	if(m_bInheritOrientation)
	{
		//find where the node would end up in parent's local space
		setOrientation( m_pParent->convertWorldToLocalOrientation( q ) );
	}
	else
	{
		setOrientation( q );
	}
}

//-----------------------------------------------------------------------
const Quat & gkNode::_getDerivedOrientation(void) const
{
	if (m_bNeedParentUpdate)
	{
		_updateFromParent();
	}
	return m_qDerivedOrientation;
}
//-----------------------------------------------------------------------
const Vec3 & gkNode::_getDerivedPosition(void) const
{
	if (m_bNeedParentUpdate)
	{
		_updateFromParent();
	}
	return m_vDerivedPosition;
}
//-----------------------------------------------------------------------
const Vec3 & gkNode::_getDerivedScale(void) const
{
	if (m_bNeedParentUpdate)
	{
		_updateFromParent();
	}
	return m_vDerivedScale;
}
//-----------------------------------------------------------------------
void gkNode::lookat( const Vec3& point , bool bHeadup)
{
	Vec3 vAxis = point - _getDerivedPosition();
	vAxis.NormalizeFast();

	Quat qTmp = Quat::CreateRotationVDir(vAxis);

	// getchild(0)就是炮塔
	_setDerivedOrientation(qTmp);
}
//-----------------------------------------------------------------------
void gkNode::smoothLookat( const Vec3& point, float fDamping, bool bHeadup /*= true*/ )
{
	Vec3 vAxis = point - _getDerivedPosition();
	vAxis.NormalizeFast();

	Quat qTmp = Quat::CreateRotationVDir(vAxis);

	// 得到了目标四元数
	// 插值
	qTmp.SetSlerp(getOrientation(), qTmp, fDamping);

	_setDerivedOrientation(qTmp);
}

