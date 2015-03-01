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
// Name:   	gkMovableObject.h
// Desc:	场景中可移动物体的基类
// 描述:	
// 
// Author:  Kaiming-Laptop
// Date:	2010/9/10
// 
//////////////////////////////////////////////////////////////////////////

#ifndef gkMovableObject_h_48162f6e_b2b3_43a1_9acf_23482ed19cd2
#define gkMovableObject_h_48162f6e_b2b3_43a1_9acf_23482ed19cd2

#include "gkPrerequisites.h"
#include "gk_Geo.h"

// 默认的相机hidemask，不想让相机摄入可以选择使用这个mask
#define GK_CAMERA_VIEWMASK_DEFAUT		1<<0
#define GK_CAMERA_VIEWMASK_SHADOWCAM	1<<1
#define GK_CAMERA_VIEWMASK_OVERLAY		1<<2

class GAMEKNIFECORE_API gkMovableObject {

protected:
	gkStdString m_wstrName;
	/// 所属的场景管理器
	gkSceneManager* m_pManager;
	/// node to which this object is attached
	gkNode* m_pParentNode;
	/// 是否可见
	bool m_bIsVisible;
	/// 是否产生阴影
	bool m_bIsCastShadow;
	/// 是否接受阴影
	bool m_bIsReceiveShadow;
	/// 是否设置了渲染层
	bool m_bIsRenderLayerSet;
	/// 渲染层
	BYTE m_yRenderLayer;
	/// 不可见掩码
	BYTE m_yHideMask;
	/// query flag
	uint32 m_uQueryFlag;

	/// Cached world AABB of this object
	mutable AABB m_WorldAABB;

	bool m_showAABB;



public:
	gkMovableObject(void);
	gkMovableObject(const gkStdString& name);
	virtual ~gkMovableObject(void);
	//////////////////////////////////////
	// Name:  gkMovableObject::_updateRenderSequence
	// Desc:	更新渲染队列
	// Returns:   void
	// Parameter: IRenderSequence * queue
	//////////////////////////////////////
	virtual void _updateRenderSequence(IRenderSequence* queue) = 0;

	virtual const AABB& getAABB(void) const = 0;

	virtual const AABB& getWorldAABB(bool derive) const;

	//////////////////////////////////////
	// Name:  gkMovableObject::getParentNode
	// Desc: 返回绑定的节点
	// Returns:   gkNode*
	// Parameter: void
	//////////////////////////////////////
	virtual gkNode* getParentNode(void) const;

	virtual gkSceneNode* getParentSceneNode(void) const;

	virtual bool isAttached(void) const;

	virtual const gkStdString& getName() const {return m_wstrName;}
	virtual const gkStdString& getMovableType() const = 0;

	virtual void setVisible(bool visible) {m_bIsVisible = visible;}
	virtual bool getVisible() const {return m_bIsVisible;}
	virtual void _notifyAttached(gkNode* parent);
	virtual const Matrix34& _getParentNodeFullTransform(void) const;

	void setRenderLayer(BYTE queueID);
	BYTE getRenderLayer();

	virtual void setHideMask(BYTE hideMASK) {m_yHideMask = hideMASK;}
	virtual void addHideMask(BYTE hideMASK) {m_yHideMask |= hideMASK;}
	virtual void removeHideMask(BYTE hideMASK) { m_yHideMask &= ~hideMASK;}
	virtual BYTE getHideMask() {return m_yHideMask;}

	// query flags [8/22/2011 Kaiming-Desktop]
    virtual void setQueryFlags(uint32 flags) { m_uQueryFlag = flags; }
    virtual void addQueryFlags(uint32 flags) { m_uQueryFlag |= flags; }    
    virtual void removeQueryFlags(unsigned long flags) { m_uQueryFlag &= ~flags; }   
    virtual uint32 getQueryFlags(void) const { return m_uQueryFlag; }

	// AABB drawing
	virtual void showAABB(bool canShow) {m_showAABB = canShow;}
	virtual bool getShowAABB() {return m_showAABB;}

};

// Movable工厂
class GAMEKNIFECORE_API gkMovableObjectFactory
{
protected:
	/// Type flag, allocated if requested
	unsigned long mTypeFlag;

	/// Internal implementation of create method - must be overridden
	virtual gkMovableObject* createInstanceImpl(
		const gkStdString& name,
		const NameValuePairList* param = NULL,
		void* userdata = NULL) = 0;
public:
	gkMovableObjectFactory() : mTypeFlag(0xFFFFFFFF) {}
	virtual ~gkMovableObjectFactory() {}
	/// Get the type of the object to be created
	virtual const gkStdString& getType(void) const = 0;

	/** Create a new instance of the object.
	@param name The name of the new object
	@param manager The SceneManager instance that will be holding the
	instance once created.
	@param params Name/value pair list of additional parameters required to 
	construct the object (defined per subtype). Optional.
	*/
	virtual gkMovableObject* createInstance(
		const gkStdString& name,
		const NameValuePairList* param = NULL);
	/** Destroy an instance of the object */
	virtual void destroyInstance(gkMovableObject* obj) = 0;

	/** Does this factory require the allocation of a 'type flag', used to 
	selectively include / exclude this type from scene queries?
	@remarks
	The default implementation here is to return 'false', ie not to 
	request a unique type mask from Root. For objects that
	never need to be excluded in SceneQuery results, that's fine, since
	the default implementation of MovableObject::getTypeFlags is to return
	all ones, hence matching any query type mask. However, if you want the
	objects created by this factory to be filterable by queries using a 
	broad type, you have to give them a (preferably unique) type mask - 
	and given that you don't know what other MovableObject types are 
	registered, Root will allocate you one. 
	*/
	virtual bool requestTypeFlags(void) const { return false; }
	/** Notify this factory of the type mask to apply. 
	@remarks
	This should normally only be called by Root in response to
	a 'true' result from requestTypeMask. However, you can actually use
	it yourself if you're careful; for example to assign the same mask
	to a number of different types of object, should you always wish them
	to be treated the same in queries.
	*/
	void _notifyTypeFlags(unsigned long flag) { mTypeFlag = flag; }

	/** Gets the type flag for this factory.
	@remarks
	A type flag is like a query flag, except that it applies to all instances
	of a certain type of object.
	*/
	unsigned long getTypeFlags(void) const { return mTypeFlag; }

};

#endif // gkMovableObject_h_48162f6e_b2b3_43a1_9acf_23482ed19cd2
