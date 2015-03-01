//////////////////////////////////////////////////////////////////////////
/*
Copyright (c) 2011-2015 Kaiming Yi
	
	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:
	
	The above copyright notice and this permission notice shall be included in
	all copies or substantial portions of the Software.
	
	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
	THE SOFTWARE.
	
*/
//////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////
//
// Name:   	gkNode.h
// Desc:	GK的核心，场景中的所有可渲染对象均属于NODE
//			一颗树结构
//			root
//			|	\       \
//			node node	node
// 描述:	
// 
// Author:  Kaiming-Laptop
// Date:	2010/9/10
// 
//////////////////////////////////////////////////////////////////////////

#ifndef gkNode_h_90a10bcf_e9c4_48b8_b566_75f8d4285e9c
#define gkNode_h_90a10bcf_e9c4_48b8_b566_75f8d4285e9c

#include "gkPrerequisites.h"

#include "gkRenderable.h"
#include "gkIterator.h"
//#include "gkMisc.h"			// GameKnifeToolkit.dll


class GAMEKNIFECORE_API gkNode {

public:

	// 存储子节点的容器 [9/25/2010 Kaiming-Laptop]
	typedef gkHashMap<gkStdString, gkNode*> ChildNodeMap;
	typedef MapIterator<ChildNodeMap> ChildNodeIterator;
	typedef ConstMapIterator<ChildNodeMap> ConstChildNodeIterator;

protected:
	gkNode* m_pParent;
	/// Collection of pointers to direct children; hashmap for efficiency
	ChildNodeMap m_mapChildren;

	typedef std::set< gkNode*,std::less<gkNode*> > ChildUpdateSet;
	/// List of children which need updating, used if self is not out of date but children are
	mutable ChildUpdateSet m_setChildrenToUpdate;

	//////////////////////////////////////////////////////////////////////////
	/// 标志
	/// 缓存的TRANSFORM是否过期
	mutable bool m_bNeedParentUpdate;
	/// 是否需要更新子节点
	mutable bool m_bNeedChildUpdate;
	/// 父节点是否已经需要更新
	mutable bool m_bParentNotified ;
	/// 是否已经在待更新的队列中
	mutable bool m_bQueuedForUpdate;

	/// 节点名称
	gkStdString m_wstrName;

	/// 局部方位信息
	mutable Quat m_qOrientation;

	/// 局部位置信息
	mutable Vec3 m_vPosition;

	/// 局部缩放信息
	mutable Vec3 m_vScale;

	/// 方位是否是继承自父节点
	bool m_bInheritOrientation;

	/// 缩放是否是继承自父节点
	bool m_bInheritScale;

	/// Only available internally - notification of parent.
	virtual void setParent(gkNode* parent);

	/** 缓存的联合后变换信息
	通过SceneManager或者父节点调用updateFromParent来更新
	*/
	/// 缓存的方位信息
	mutable Quat m_qDerivedOrientation;
	/// 缓存的位置信息
	mutable Vec3 m_vDerivedPosition;
	/// 缓存的缩放信息
	mutable Vec3 m_vDerivedScale;

	/** Internal method for creating a new child node - must be overridden per subclass. */
	virtual gkNode* createChildImpl(void) = 0;

	/** Internal method for creating a new child node - must be overridden per subclass. */
	virtual gkNode* createChildImpl(const gkStdString& name) = 0;

	//////////////////////////////////////////////////////////////////////////
	/// 制作角色动画骨骼时使用，暂不实现
// 	/// The position to use as a base for keyframe animation
// 	D3DXVECTOR3 mInitialPosition;
// 	/// The orientation to use as a base for keyframe animation
// 	D3DXQUATERNION mInitialOrientation;
// 	/// The scale to use as a base for keyframe animation
// 	D3DXVECTOR3 mInitialScale;

	/// Cached derived transform as a 4x4 matrix
	mutable Matrix34 m_matCachedTransform;
	mutable bool m_bCachedTransformOutOfDate;

	typedef std::vector<gkNode*> QueuedUpdates;
	static QueuedUpdates m_vecQueuedUpdates;

	// 调试时显示的工具信息 -以后实现
	//DebugRenderable* mDebug;
	static gkNameGenerator ms_NameGenerator;


public:
	gkNode( void );
	gkNode( gkStdString wstrName);
	virtual ~gkNode( void );

	/// Returns the name of the node.
	const gkStdString& getName(void) const {return m_wstrName;}

	/// Gets this node's parent (NULL if this is the root).
	virtual gkNode* getParent(void) const;


	//////////////////////////////////////////////////////////////////////////
	/// 变换信息的读取函数
	/// 方位
	/// Returns a quaternion representing the nodes orientation.
	virtual const Quat & getOrientation() const;
	/// Sets the orientation of this node via a quaternion.
	virtual void setOrientation( const Quat& q );
	/// Sets the orientation of this node via quaternion parameters.
	virtual void setOrientation( float w, float x, float y, float z);
	/// Resets the nodes orientation (local axes as world axes, no rotation).
	//virtual void resetOrientation(void);

	/// 位置
	/// Sets the position of the node relative to it's parent.
	virtual void setPosition(const Vec3& pos);
	virtual void setPosition(float x, float y, float z);
	/// Gets the position of the node relative to it's parent.
	virtual const Vec3 & getPosition(void) const;

	/// 缩放
	/// Sets the scaling factor applied to this node.
	virtual void setScale(const Vec3& scale);
	virtual void setScale(float x, float y, float z);
	/// Gets the scaling factor of this node.
	virtual const Vec3 & getScale(void) const;

	/// 位移
	virtual void translate(const Vec3& d, gkTransformSpace relativeTo = TS_PARENT);
	virtual void translate(float x, float y, float z, gkTransformSpace relativeTo = TS_PARENT);

	/// 旋转
	virtual void rotate(const Vec3& axis, float angle, gkTransformSpace relativeTo = TS_LOCAL);
	virtual void rotate(const Quat& q, gkTransformSpace relativeTo = TS_LOCAL);

	/// lookat
	/// head up
	virtual void lookat(const Vec3& point, bool bHeadup = true);
	virtual void smoothLookat(const Vec3& point, float fDamping, bool bHeadup = true);

	//////////////////////////////////////////////////////////////////////////
	// 更新相关
	virtual	void needUpdate(bool forceParentUpdate = FALSE);
	virtual	void cancelUpdate(gkNode* child);
	virtual	void queueNeedUpdate(gkNode* n);
	virtual	void processQueuedUpdates(void);
	virtual	void requestUpdate(gkNode* child, bool forceParentUpdate  = FALSE);

	//////////////////////////////////////////////////////////////////////////
	// child相关
	virtual gkNode* getChild(const gkStdString& name) const;
	virtual gkNode* getChild(unsigned short index) const;
	virtual uint32 getChildCount() const;					//  [12/4/2011 Kaiming]
	virtual gkNode* removeChild(const gkStdString& name);
	virtual gkNode* removeChild(unsigned short index);
	virtual gkNode* removeChild(gkNode* child);
	virtual void removeAllChildren(void);
	virtual gkNode* createChild(const Vec3& translate, const Quat& rotate);
	virtual gkNode* createChild(const gkStdString& name, const Vec3& translate, const Quat& rotate);
	virtual void addChild(gkNode* child);
	virtual unsigned short numChildren(void) const;

	//////////////////////////////////////////////////////////////////////////
	/// 更改设置
	/// 设置方位信息是否要继承自父节点.
	/// 多数时候，其实方位不继承自父节点更利于直观控制
 	virtual void setInheritOrientation(bool inherit)
	{
		m_bInheritOrientation = inherit;
		needUpdate();
	}
 	virtual bool getInheritOrientation(void) const
	{
		return m_bInheritOrientation;
	}

	/// 设置缩放信息是否要继承自父节点.
	/// 多数时候，其实缩放不继承自父节点更利于直观控制
	virtual void setInheritScale(bool inherit)
	{
		m_bInheritScale = inherit;
		needUpdate();
	}

	virtual bool getInheritScale(void) const
	{
		return m_bInheritScale;
	}

	// update相关
	Vec3 convertWorldToLocalPosition( const Vec3 &worldPos );
	Vec3 convertLocalToWorldPosition( const Vec3 &localPos );
	Quat convertWorldToLocalOrientation( const Quat &worldOrientation );
	Quat convertLocalToWorldOrientation( const Quat &localOrientation );

	//////////////////////////////////////////////////////////////////////////
	/// 渲染时便于直接取得绝对信息的函数
	/// 得到联合之后的世界绝对朝向
	void _setDerivedPosition( const Vec3& pos );
	void _setDerivedOrientation( const Quat& q );


	virtual const Quat& _getDerivedOrientation(void) const;

	/// 得到联合之后的世界绝对位置
	virtual const Vec3& _getDerivedPosition(void) const;

	/// 得到联合之后的世界绝对缩放
	virtual const Vec3& _getDerivedScale(void) const;

	/// 得到改节点的绝对变换信息
	virtual const Matrix34& _getFullTransform(void) const;

	//////////////////////////////////////
	// Name:  gkNode::_update
	// Desc:  内部更新函数，SceneManager调用
	//		  一般不允许外部程序调用
	// Returns:   void
	// Parameter: bool updateChildren 是否更新子节点
	// Parameter: bool parentHasChanged 父节点有否更新
	//////////////////////////////////////
	virtual void _update(bool updateChildren, bool parentHasChanged);

	virtual void _updateFromParent(void) const;

	virtual void updateFromParentImpl(void) const;

};

#endif // gkNode_h_90a10bcf_e9c4_48b8_b566_75f8d4285e9c
