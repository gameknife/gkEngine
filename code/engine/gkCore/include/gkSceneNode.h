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
// Name:   	gkSceneNode.h
// Desc:	场景节点，gkEngine真正的节点实现
// 描述:	
// 
// Author:  Kaiming-Laptop
// Date:	2010/9/10
// 
//////////////////////////////////////////////////////////////////////////

#ifndef gkSceneNode_h_4f3f696a_7bcc_4769_9e6e_dfa3928a3d5b
#define gkSceneNode_h_4f3f696a_7bcc_4769_9e6e_dfa3928a3d5b

#include "gkPrerequisites.h"

#include "gkNode.h"
#include "gkIterator.h"
//#include "gkMisc.h"			// GameKnifeToolkit.dll


class GAMEKNIFECORE_API gkSceneNode : public gkNode {
public:
	typedef gkHashMap<gkStdString, gkMovableObject*> ObjectMap;
	typedef MapIterator<ObjectMap> ObjectIterator;
	typedef ConstMapIterator<ObjectMap> ConstObjectIterator;

protected:
	ObjectMap m_mapObjectsByName;

	/// 今后关于边界盒的信息
// 	bool mShowBoundingBox;
// 	bool mHideBoundingBox;

	/** @copydoc Node::updateFromParentImpl. */
	void updateFromParentImpl(void) const;

	/** See Node. */
	gkNode* createChildImpl(void);

	/** See Node. */
	gkNode* createChildImpl(const gkStdString& name);

	/** See Node */
	void setParent(gkNode* parent);

	/** Internal method for setting whether the node is in the scene 
	graph.
	*/
	virtual void setInSceneGraph(bool inGraph);


	/// Whether to yaw around a fixed axis.
	bool m_bYawFixed;
	/// Fixed axis to yaw around
	Vec3 m_vYawFixedAxis;

	bool m_bIsInSceneGraph;
public:
	gkSceneNode();
	gkSceneNode(const gkStdString& name);
	~gkSceneNode();

	virtual bool isInSceneGraph();

	// 添加一个node级别的可见性设置 [4/17/2011 Kaiming-Desktop]
	virtual void setVisible(bool visible, bool cascade = true);

	/** 连接实例物件
	*/
	virtual void attachObject(gkMovableObject* obj);

	/** 返回此节点连接的个数
	*/
	virtual BYTE numAttachedObjects(void) const;

	/** 取得指定index的连接物件
	*/
	virtual gkMovableObject* getAttachedObject(BYTE index);

	/** 取得指定name的连接物件
	*/
	virtual gkMovableObject* getAttachedObject(const gkStdString& name);

	/** 取消指定index的物件链接
	*/
	virtual gkMovableObject* detachObject(BYTE index);
	/** 取消指定指针的物件链接
	*/
	virtual void detachObject(gkMovableObject* obj);

	/** 取消指定name的物件链接
	*/
	virtual gkMovableObject* detachObject(const gkStdString& name);

	/** 取消所有物件链接
	*/
	virtual void detachAllObjects(void);

	/** 内部的更新函数
	@note
	由上层节点调用，一般来说用户不要调用这些更新函数
	*/
	virtual void _update(bool updateChildren, bool parentHasChanged);

	/** 查找可见物体并添加到渲染序列的内部函数
	@remarks
	由scenemanager调用的查找函数，一般来说用户不要调用这些更新函数
	*/
	virtual void _findVisibleObjects(gkCamera* cam, IRenderSequence* queue, bool includeChildren = true, bool displayNodes = false);

	/** Retrieves an iterator which can be used to efficiently step through the objects 
	attached to this node.
	@remarks
	取得一个迭代器
	*/
	virtual ObjectIterator getAttachedObjectIterator(void);
	/** Retrieves an iterator which can be used to efficiently step through the objects 
	attached to this node.
	@remarks
	取得一个const迭代器
	*/
	virtual ConstObjectIterator getAttachedObjectIterator(void) const;

	/** 移除并摧毁指定name的子节点
	@remarks
	这里包含了摧毁
	*/
	virtual void removeAndDestroyChild(const gkStdString& name);

	virtual void removeAndDestroyAllChildren(void);


	//////////////////////////////////////
	// Name:  gkSceneNode::createChildSceneNode
	// Desc:  创建一个不带名字的子节点
	// Returns:   gkSceneNode*
	// Parameter: const Vector3 & translate 初始化位置
	// Parameter: const Quaternion & rotate 初始化朝向
	//////////////////////////////////////
	virtual gkSceneNode* createChildSceneNode(
		const Vec3& translate = Vec3(0,0,0), 
		const Quat& rotate = Quat(1,0,0,0) );

	//////////////////////////////////////
	// Name:  gkSceneNode::createChildSceneNode
	// Desc:  创建一个带名字的子节点
	// Returns:   gkSceneNode*
	// Parameter: const gkStdString & name 节点名字
	// Parameter: const D3DXVECTOR3 & translate 初始化位置
	// Parameter: const D3DXQUATERNION & rotate 初始化朝向
	//////////////////////////////////////
	virtual gkSceneNode* createChildSceneNode(const gkStdString& name, 
		const Vec3& translate = Vec3(0,0,0), 
		const Quat& rotate = Quat(1,0,0,0) );

// TODO: 一些专用的函数
// 	virtual void setDirection(float x, float y, float z, 
// 		TransformSpace relativeTo = TS_LOCAL, 
// 		const D3DXVECTOR3& localDirectionVector = D3DXVECTOR3(0,0,-1));
// 
// 	virtual void setDirection(const D3DXVECTOR3& vec, TransformSpace relativeTo = TS_LOCAL, 
// 		const D3DXVECTOR3& localDirectionVector = D3DXVECTOR3(0,0,-1));
// 
// 	virtual void lookAt( const D3DXVECTOR3& targetPoint, TransformSpace relativeTo,
// 		const D3DXVECTOR3& localDirectionVector =  D3DXVECTOR3(0,0,-1));
};

#endif // gkSceneNode_h_4f3f696a_7bcc_4769_9e6e_dfa3928a3d5b
