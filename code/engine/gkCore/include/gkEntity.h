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
// Name:   	gkEntity.h
// Desc:	GameKnife的游戏实体
//			目前的设计是，对应于一个gkMesh和一个gkMaterial。
//			主要是方便对应max中的节点概念，节点本身来说是只包含一个mesh的
//			同时操作起来也更方便，需要多材质mesh组合的物体直接通过node层级结构构造即可
//			目前大致这样做吧。
//
// 描述:	
// 
// Author:  Kaiming-Desktop	 
// Date:	9/10/2010 
// Modify:	2010/12/21
// Modify:	2011/8/9
// 
//////////////////////////////////////////////////////////////////////////

#ifndef gkEntity_h_7f71e940_8d84_48c3_8d07_01857a065938
#define gkEntity_h_7f71e940_8d84_48c3_8d07_01857a065938

#include "gkPrerequisites.h"

#include <vector>

#include "IMesh.h"
#include "gkMovableObject.h"
#include "IGameObjectLayer.h"
#include "IMaterial.h"

class gkEntity : public gkMovableObject, public IGameObjectRenderLayer {
// 直接存取subentity
friend class gkStaticObjRenderable;

public:
	typedef std::vector< gkRenderable* > gkSubRenderableList;
	typedef std::list< gkEntity* > gkEntityList;
protected:
	bool m_bIsInitialized;

	gkSubRenderableList m_vecSubEntityList;

	gkEntity* m_parentEntity;

	mutable AABB m_FullAABB;
	mutable AABB m_FullWorldAABB;

	float m_fMaskColor;
	bool m_bIsGroup;

	uint8 m_uEntityClass;

	gkMaterialPtr m_pMaterial;

	mutable bool m_rebuildAABB;
	int m_nRealSubEntityCount;

	// 初始化
	virtual void _initialize(void);
	virtual	void _deinitialise(void);
	virtual void buildSubEntityList(gkMeshPtr& pMesh, gkSubRenderableList* pSubEntityList);
	virtual gkRenderable* getSubEntity(BYTE index = 0) const;

	// shadow casting
	virtual void enableCastShadow(bool enable);

public:
	gkEntity(void);
	gkEntity(const gkStdString& name);
	virtual ~gkEntity(void);

	// 一些直接操作renderable的方法
	virtual void setMaterialName(const gkStdString& name, BYTE index = 0);
	virtual const gkStdString& getMaterialName() const;
	virtual IMaterial* getMaterial() {return m_pMaterial.getPointer();}

	// 修改subEntity
	virtual	void modifySubRenderable( gkRenderable* newRenderable, BYTE yIdx );

	virtual	int getSubRenderableCount();

	virtual const gkMeshPtr& getMesh(uint8 uIdx = 0) const;

	IGameObjectLayer* clone( const gkStdString& newName) const;

	// override
	// 覆盖movable接口
	virtual void _updateRenderSequence(IRenderSequence* queue);
	// 覆盖movable接口
	virtual const gkStdString& getMovableType() const;
	// 覆盖movable接口
	virtual const AABB& getAABB(void) const;
	virtual const AABB& getWorldAABB(void) const;
	virtual void setRenderPipe(uint8 layer);



	virtual void setMaskColor(float maskColor) {m_fMaskColor = maskColor;}
	virtual float getMaskColor() {return m_fMaskColor;}

	virtual const gkStdString& getName() const {return gkMovableObject::getName();}

public:
	// IGameObjectLayer [3/29/2012 Kaiming]
	virtual void Destroy();
	virtual void UpdateLayer(float fElpasedTime) {}
	virtual void showBBox(bool bShow) {showAABB(bShow);}

	// override IEntity
	// 操作位置的方法
	/// Sets the orientation of this node via a quaternion.
	virtual void setOrientation( const Quat& q );
	/// Sets the orientation of this node via quaternion parameters.
	virtual void setOrientation( float w, float x, float y, float z);

	virtual const Quat & getOrientation() const;
	virtual const Quat & getWorldOrientation() const;

	/// 位置
	/// Sets the position of the node relative to it's parent.
	virtual void setPosition(const Vec3& pos);
	virtual void setPosition(float x, float y, float z);
	/// Gets the position of the node relative to it's parent.
	virtual const Vec3 & getPosition(void) const;
	virtual const Vec3& getWorldPosition(void) const;

	virtual void setWorldPosition(const Vec3& pos);
	virtual void setWorldPosition(float x, float y, float z);

	/// 缩放
	/// Sets the scaling factor applied to this node.
	virtual void setScale(const Vec3& scale);
	virtual void setScale(float x, float y, float z);
	/// Gets the scaling factor of this node.
	virtual const Vec3 & getScale(void) const;

	virtual const Matrix44& GetWorldMatrix() const;

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

	virtual void showAABB(bool canShow);

	virtual void attachEntity(IGameObject* ent);
	virtual	void detachChildEntity(IGameObject* ent);
	virtual gkEntity* _getParentEntity() {return m_parentEntity;}
	virtual void _setParentEntity(gkEntity* ent);

	virtual void setVisible(bool visible){gkMovableObject::setVisible(visible);}

	virtual gkRenderable* getSubRenderable( uint8 uIdx );

};

/** Factory object for creating Entity instances */
class GAMEKNIFECORE_API gkEntityFactory : public gkMovableObjectFactory
{
protected:
	gkMovableObject* createInstanceImpl( const gkStdString& name, const NameValuePairList* param, void* userdata);
public:
	gkEntityFactory() {}
	~gkEntityFactory() {}

	static gkStdString FACTORY_TYPE_NAME;

	const gkStdString& getType(void) const;
	void destroyInstance( gkMovableObject* obj);
};

#endif // gkEntity_h_7f71e940_8d84_48c3_8d07_01857a065938
