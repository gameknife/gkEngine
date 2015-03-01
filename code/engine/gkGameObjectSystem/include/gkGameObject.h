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
// K&K Studio GameKnife ENGINE Source File
//
// Name:   	gkGameObject.h
// Desc:		
// 	
// Author:  Kaiming
// Date:	2012/3/27
// Modify:	2012/3/27
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _gkGameObject_h_
#define _gkGameObject_h_

#include "Prerequisites.h"
#include "IGameObject.h"

struct IGameObjectRenderLayer;
struct IGameObjectPhysicLayer;

class gkGameObject : public IGameObject
{
public:
	gkGameObject(uint32 id, const gkStdString& name);
	virtual ~gkGameObject(void) {}

	//////////////////////////////////////////////////////////////////////////
	// basic infomation

	virtual uint32 getID() const {return m_uID;}
	virtual const gkStdString& getName() const {return m_wstrName;}

	virtual EGameObjectSuperClass getGameObjectSuperClass() const {return m_uClassID;}
	virtual const gkStdString& getGameObjectClassName() const {return m_wstrClassName;}

	virtual void setGameObjectSuperClass(EGameObjectSuperClass superclass) { m_uClassID = superclass;}
	virtual void setGameObjectClass(const gkStdString& classname) {m_wstrClassName = classname;}

	//////////////////////////////////////////////////////////////////////////
	// clone
	virtual IGameObject* clone( const gkStdString& newName) const;

	//////////////////////////////////////////////////////////////////////////
	// layer set
	virtual void setGameObjectLayer(IGameObjectLayer* layer);
	virtual IGameObjectRenderLayer* getRenderLayer() {return m_pRenderLayer;}
	virtual IGameObjectPhysicLayer* getPhysicLayer() {return m_pPhysicLayer;}
	virtual IGameObjectLayer* getGameObjectLayer(EGameObjectLayerType layertype);

	//////////////////////////////////////////////////////////////////////////
	// hiracachy control
	virtual void attachChild(IGameObject* ent);
	virtual	void detachChild( IGameObject* ent );
	virtual void detachAllChild();
	virtual void detachFromFather();

	//////////////////////////////////////////////////////////////////////////
	// hirecachy access
	virtual uint32 getChildCount() {return m_lstChilds.size();}
	virtual IGameObject* getChild(uint32 index);
	virtual IGameObject* getParent() {return m_pParent;}

	//////////////////////////////////////////////////////////////////////////
	// basic transformation

	// orientation
	virtual void setOrientation( const Quat& q );
	virtual void setOrientation( float w, float x, float y, float z);

	virtual const Quat & getOrientation() const;
	virtual const Quat & getWorldOrientation() const;

	// position
	virtual void setPosition(const Vec3& pos);
	virtual void setPosition(float x, float y, float z);

	virtual void setWorldPosition(const Vec3& pos);
	virtual void setWorldPosition(float x, float y, float z);

	virtual const Vec3 &	getPosition(void) const;
	virtual const Vec3&		getWorldPosition(void) const;

	// scaling
	virtual void setScale(const Vec3& scale);
	virtual void setScale(float x, float y, float z);
	virtual const Vec3 & getScale(void) const;

	// get world matrix
	virtual const Matrix44& GetWorldMatrix();



	//////////////////////////////////////////////////////////////////////////
	// relative transformation

	// translate
	virtual void translate(const Vec3& d, gkTransformSpace relativeTo = TS_PARENT);
	virtual void translate(float x, float y, float z, gkTransformSpace relativeTo = TS_PARENT);

	// rotate
	virtual void rotate(const Vec3& axis, float angle, gkTransformSpace relativeTo = TS_LOCAL);
	virtual void rotate(const Quat& q, gkTransformSpace relativeTo = TS_LOCAL);

	/// lookat (head up)
	virtual void lookat(const Vec3& point, bool bHeadup = true);
	virtual void smoothLookat(const Vec3& point, float fDamping, bool bHeadup = true);


	//////////////////////////////////////////////////////////////////////////
	// fast RenderLayer call
	virtual void setVisible(bool bVisible);
	virtual const gkStdString& getMaterialName() const;
	virtual void showBBox(bool bShow);

	virtual bool* getPhysicEnable() {return &m_physicEnable;}

public:

	void setParent(IGameObject* parent) {m_pParent = reinterpret_cast<gkGameObject*>(parent);}
	void preupdate( float fElapsedTime );
	void update(float fElapsedTime);
	void destroy();

	virtual const AABB& getAABB() const;
	virtual const AABB& getWorldAABB() const;
private:


	uint32 m_uID;
	gkStdString m_wstrName;

	EGameObjectSuperClass m_uClassID;
	gkStdString m_wstrClassName;

	gkGameObject* m_pParent;
	gkGameObjectList m_lstChilds;
	gkGameObjectLayerList m_lstLayers;

	IGameObjectRenderLayer* m_pRenderLayer;
	IGameObjectPhysicLayer*	m_pPhysicLayer;


	bool m_physicEnable;

private:
	static Quat			ms_IdentityOrientation;
	static Vec3			ms_IdentityPos;
	static Vec3			ms_IdentityScale;
	static Matrix44		ms_IdentityMatrix;

	
};


#endif