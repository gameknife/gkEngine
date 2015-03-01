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
// Name:   	IGameObject.h
// Desc:		
// 	
// Author:  Kaiming
// Date:	2012/3/24
// Modify:	2012/3/24
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _IGAMEOBJECT_H_
#define _IGAMEOBJECT_H_

#include "gk_Color.h"
#include "gk_Geo.h"

#ifdef _3DSMAX
#define IGameObject _IGameObject
#endif

struct IGameObjectLayer;
struct IGameObjectRenderLayer;
struct IGameObjectPhysicLayer;
struct CRapidXmlParseNode;

enum EGameObjectLayerType
{
	eGL_RenderLayer				= 1 << 0,
	eGL_AnimLayer				= 1 << 1,
	eGL_PhysicLayer				= 1 << 2,
	eGL_ScirptLayer				= 1 << 3,
	eGL_LightLayer				= 1 << 4,
	eGL_ParticleLayer			= 1 << 5,


	//eGL_Count,
};

enum EGameObjectSuperClass
{
	eGOClass_VOID,
	eGOClass_STATICMESH,
	eGOClass_SKINNEDMESH,
	eGOClass_SYSTEM,
	eGOClass_GROUP,
	eGOClass_LIGHT,
	eGOClass_PARTICLE,
	eGOClass_CAMERA,
	eGOClass_OTHER,
};

struct IGameObject
{
	virtual ~IGameObject(void) {}

	//////////////////////////////////////////////////////////////////////////
	// basic infomation

	virtual uint32 getID() const =0;
	virtual const gkStdString& getName() const =0;

	virtual EGameObjectSuperClass getGameObjectSuperClass() const =0;
	virtual const gkStdString& getGameObjectClassName() const =0;

	virtual void setGameObjectSuperClass(EGameObjectSuperClass superclass) =0;
	virtual void setGameObjectClass(const gkStdString& classname) =0;

	//////////////////////////////////////////////////////////////////////////
	// clone
	virtual IGameObject* clone( const gkStdString& newName) const =0;

	//////////////////////////////////////////////////////////////////////////
	// layer set
	virtual void setGameObjectLayer(IGameObjectLayer* layer) =0;
	virtual IGameObjectRenderLayer* getRenderLayer() =0;
	virtual IGameObjectPhysicLayer* getPhysicLayer() =0;
	virtual IGameObjectLayer* getGameObjectLayer(EGameObjectLayerType layertype) =0;


	//////////////////////////////////////////////////////////////////////////
	// hiracachy control
	virtual void attachChild(IGameObject* ent) =0;
	virtual	void detachChild( IGameObject* ent ) =0;
	virtual void detachAllChild() =0;
	virtual void detachFromFather() =0;

	//////////////////////////////////////////////////////////////////////////
	// hirecachy access
	virtual uint32 getChildCount() =0;
	virtual IGameObject* getChild(uint32 index) =0;
	virtual IGameObject* getParent() =0;

	//////////////////////////////////////////////////////////////////////////
	// basic transformation

	// orientation
	virtual void setOrientation( const Quat& q ) =0;
	virtual void setOrientation( float w, float x, float y, float z) =0;

	virtual const Quat & getOrientation() const =0;
	virtual const Quat & getWorldOrientation() const =0;

	// position
	virtual void setPosition(const Vec3& pos) =0;
	virtual void setPosition(float x, float y, float z) =0;
	
	virtual void setWorldPosition(const Vec3& pos) =0;
	virtual void setWorldPosition(float x, float y, float z) =0;

	virtual const Vec3 &	getPosition(void) const =0;
	virtual const Vec3&		getWorldPosition(void) const =0;

	// scaling
	virtual void setScale(const Vec3& scale) =0;
	virtual void setScale(float x, float y, float z) =0;
	virtual const Vec3 & getScale(void) const =0;

	// get world matrix
	virtual const Matrix44& GetWorldMatrix() =0;

	// get bounding box
	virtual const AABB& getAABB() const =0;
	virtual const AABB& getWorldAABB() const =0;
	//////////////////////////////////////////////////////////////////////////
	// relative transformation

	// translate
	virtual void translate(const Vec3& d, gkTransformSpace relativeTo = TS_PARENT) =0;
	virtual void translate(float x, float y, float z, gkTransformSpace relativeTo = TS_PARENT) =0;

	// rotate
	virtual void rotate(const Vec3& axis, float angle, gkTransformSpace relativeTo = TS_LOCAL) =0;
	virtual void rotate(const Quat& q, gkTransformSpace relativeTo = TS_LOCAL) =0;

	/// lookat (head up)
	virtual void lookat(const Vec3& point, bool bHeadup = true) =0;
	virtual void smoothLookat(const Vec3& point, float fDamping, bool bHeadup = true) =0;

	//////////////////////////////////////////////////////////////////////////
	// fast RenderLayer call
	virtual void setVisible(bool bVisible) =0;
	virtual const gkStdString& getMaterialName() const =0;
	virtual void showBBox(bool bShow) =0;

	virtual bool* getPhysicEnable() =0;
};

typedef std::vector<IGameObject*> IGameObjectQueue;
typedef std::vector<IGameObject*> IGameObjectList;

	/**
	 @ingroup CoreModuleAPI
	 @brief 游戏物件系统
	 @remark 游戏物件系统是gkENGINE的系统驱动核心, GameObjectSystem提供GameObject的创建和删除接口，以及供system调用的更新接口

	 @sa IGameObject 游戏对象
	*/
struct IGameObjectSystem
{
	virtual ~IGameObjectSystem() {}

	/**
	 @brief 创建静态几何体类型的GameObject，引擎自动命名
	 @return IGameObject* 创建完成的GameObject< 创建不成功，返回NULL
	 @param const gkStdString & meshfile 几何体使用的mesh文件名
	 @param Vec3 & pos 初始位置
	 @param Quat & rot 初始朝向
	 @remark 内置的GameObject类型，在实现类中提供GameObject的组装。
	*/
	virtual IGameObject* CreateStaticGeoGameObject( const gkStdString& meshfile, Vec3& pos, Quat& rot)=0;
	/**
	 @brief 创建一个空的GameObject，引擎自动命名
	 @return IGameObject* 创建完成的GameObject< 创建不成功，返回NULL
	 @param Vec3 & pos 初始位置
	 @param Quat & rot 初始朝向
	 @remark 空的GameObject一般作为帮助物体，或组对象的配合管理其他gameobject
	*/
	virtual IGameObject* CreateVoidGameObject( Vec3& pos, Quat& rot)=0;
	/**
	 @brief 创建一个LightGameObject类型的GameObject，引擎自动命名
	 @return IGameObject* 创建完成的GameObject< 创建不成功，返回NULL
	 @param Vec3 & lightpos 灯光位置
	 @param float lightRadius 灯光半径
	 @param ColorF & lightColor 灯光颜色
	 @param bool isFakeShadow 是否打开假阴影
	 @remark 内置的GameObject类型，在实现类中提供GameObject的组装。
	*/
	virtual IGameObject* CreateLightGameObject( Vec3& lightpos, float lightRadius, ColorF& lightColor, bool isFakeShadow = false) =0;
	/**
	 @brief 创建一个Anim类型的GameObject，引擎自动命名
	 @return IGameObject* 创建完成的GameObject< 创建不成功，返回NULL
	 @param const gkStdString & chrfile 角色的描述文件
	 @param Vec3 & pos 初始位置
	 @param Quat & rot 初始朝向
	 @remark 内部实现调用IAnimation模块来创建AnimLayer并组装到GameObject上
	*/
	virtual IGameObject* CreateAnimGameObject( const gkStdString& chrfile, Vec3& pos, Quat& rot) =0;
	/**
	 @brief 创建一个Particle类型的GameObject，引擎自动命名
	 @return 
	 @param const gkStdString & ptcFilename 粒子描述文件
	 @param const Vec3 & pos
	 @param const Quat & rot
	 @remark 
	*/
	virtual IGameObject* CreateParticleGameObject( const gkStdString& ptcFilename, const Vec3& pos, const Quat& rot ) =0;


	virtual IGameObject* CreateStaticGeoGameObject(const gkStdString& name, const gkStdString& meshfile, Vec3& pos, Quat& rot)=0;
	virtual IGameObject* CreateVoidGameObject(const gkStdString& name, Vec3& pos, Quat& rot)=0;
	virtual IGameObject* CreateLightGameObject(const gkStdString& name, Vec3& lightpos, float lightRadius, ColorF& lightColor, bool isFakeShadow = false) =0;
	virtual IGameObject* CreateAnimGameObject(const gkStdString& name, const gkStdString& chrfile, Vec3& pos, Quat& rot) =0;
	virtual IGameObject* CreateParticleGameObject( const gkStdString& name, const gkStdString& ptcFilename, const Vec3& pos, const Quat& rot ) =0;

	/**
	 @brief 从gameobject xmlnode创建gameobject
	 @return 
	 @param CRapidXmlParseNode * node
	 @remark 
	*/
	virtual IGameObject* CreateGameObjectFromXml(CRapidXmlParseNode* node) =0;

	/**
	 @brief 删除gameobject
	 @return 
	 @param IGameObject * pTarget
	 @remark 
	*/
	virtual bool DestoryGameObject(IGameObject* pTarget) =0;
	/**
	 @brief 通过id删除gameobject
	 @return 
	 @param uint32 id
	 @remark 
	*/
	virtual bool DestoryGameObject(uint32 id) =0;

	/**
	 @brief 获取全局的gameobject数量
	 @return 
	 @remark 
	*/
	virtual uint32 getGameObjectCount() =0;

	/**
	 @brief 通过id获取gameobject
	 @return 
	 @param uint32 goID
	 @remark 
	*/
	virtual IGameObject* GetGameObjectById(uint32 goID) =0;
	/**
	 @brief 通过名字获取gameobject
	 @return 
	 @param const gkStdString & name
	 @remark 
	*/
	virtual IGameObject* GetGameObjectByName(const gkStdString& name) =0;

	virtual const IGameObjectQueue& GetGameObjects() const =0;

	/**
	 @brief 同步更新接口
	 @return 
	 @remark 
	*/
	virtual bool syncupdate() =0;
	/**
	 @brief 预更新
	 @return 
	 @param float fElpasedTime
	 @remark 
	*/
	virtual void preupdate( float fElpasedTime ) =0;
	/**
	 @brief 异步更新接口
	 @return 
	 @param float fElapsedTime
	 @remark 
	*/
	virtual void update(float fElapsedTime) =0;
	/**
	 @brief 摧毁gameobjectsystem
	 @return 
	 @remark 
	*/
	virtual void destroy() =0;
	
};


#endif
