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

#ifndef _IGAMEOBJECTLAYER_H_
#define _IGAMEOBJECTLAYER_H_

/** 
	@defgroup GOSYSAPI 游戏物件系统
	@brief 游戏驱动的核心系统，直接和用户层打交道的最顶层模块
	@remark 
*/


#include "IGameObject.h"
#include "gk_Color.h"

class gkMeshPtr;
struct gkRenderable;
class IMaterial;
struct AABB;
class gkMaterialPtr;


	/**
	 @ingroup GOSYSAPI
	 @brief 游戏物件层-基类
	 \n \b IGameObjectRenderLayer渲染层： 控制组合gameobject的渲染
	 \n \b IGameObjectAnimLayer动画层： 控制组合gameobject的动画控制
	 \n \b IGameObjectPhysicLayer物理层： 控制组合gameobject的物理状态
	 \n \b IGameObjectLightLayer灯光层： 控制组合gameobject的光照属性
	 \n \b IGameObjectParticleLayer粒子层： 控制组合gameobject的粒子控制
	 \n \b IGameObjectScriptLayer脚本层： 控制组合gameobject的脚本驱动


	 @remark 定义 \b游戏物件层 的基本接口
	*/
struct IGameObjectLayer
{
	IGameObjectLayer() {m_pParent = NULL;}
	virtual ~IGameObjectLayer(void) {}

	/**
	 @brief 取得GameObjectLayer的类型
	 @return 
	 @remark 
	 @sa EGameObjectLayerType
	*/
	virtual EGameObjectLayerType getType() const=0;
	/**
	 @brief 取得绑定的gameobject
	 @return IGameObject* 绑定的gameobject
	 @remark 
	*/
	virtual IGameObject* getParentGameObject() {return m_pParent;}

	/**
	 @brief 初始化
	 @return 
	 @remark 
	*/
	virtual void Init() {}
	/**
	 @brief 预更新调用
	 @return 
	 @param float fElapsedTime
	 @remark 非纯虚，可不实现。需要在其他层的upate前调用了任务实现在这里
	*/
	virtual void PreUpdateLayer( float fElapsedTime) {}
	/**
	 @brief 更新调用
	 @return 
	 @param float fElpasedTime
	 @remark 非纯虚，可不实现。
	*/
	virtual void UpdateLayer(float fElpasedTime) {}
	/**
	 @brief 销毁层
	 @return 
	 @remark 
	*/
	virtual void Destroy() {}
	/**
	 @brief 将Layer克隆
	 @return 
	 @param const gkStdString & newName
	 @remark 
	*/
	virtual IGameObjectLayer* clone( const gkStdString& newName) const =0;
	/**
	 @brief 将layer绑定到gameobject上
	 @return 
	 @param IGameObject * pParent
	 @remark 内部调用，外部一般慎用
	*/
	virtual void _setParentGameObject(IGameObject* pParent) {m_pParent = pParent;} 

	IGameObject* m_pParent;
};

	/**
	 @ingroup GOSYSAPI
	 @brief 渲染层
	 @remark 所有需要渲染的GameObject均需要绑定RenderLayer。
	*/
struct IGameObjectRenderLayer : public IGameObjectLayer
{
	virtual ~IGameObjectRenderLayer(void) {}
	virtual EGameObjectLayerType getType() const {return eGL_RenderLayer;}

	/**
	 @brief 设置材质名
	 @return 
	 @param const gkStdString & name
	 @param BYTE index
	 @remark 内部主动读取材质名指定的材质路径，如果载入成功，成功应用，如果不成功，使用默认材质
	*/
	virtual void setMaterialName(const gkStdString& name, BYTE index = 0) =0;
	/**
	 @brief 获取材质名
	 @return 
	 @param BYTE index
	 @remark 
	*/
	virtual const gkStdString& getMaterialName() const =0;
	/**
	 @brief 取得材质
	 @return 
	 @remark 
	*/
	virtual IMaterial* getMaterial() =0;
	/**
	 @brief 
	 @return 设置子渲染体
	 @param gkRenderable * newRenderable
	 @param BYTE yIdx
	 @remark 将从申请到的renderable绑定在renderlayer上，以便被加入渲染队列和设置正确的材质被渲染出来
	*/
	virtual	void modifySubRenderable( gkRenderable* newRenderable,  BYTE yIdx ) =0;
	/**
	 @brief 取得子渲染体的数量
	 @return 
	 @remark 
	*/
	virtual	int getSubRenderableCount() =0;
	/**
	 @brief 取得renderlayer所用的mesh
	 @return 
	 @param uint8 uIdx
	 @remark 
	*/
	virtual const gkMeshPtr& getMesh(uint8 uIdx = 0) const =0;
	/**
	 @brief 取得第idx个子渲染体
	 @return 
	 @param uint8 uIdx
	 @remark 
	*/
	virtual gkRenderable* getSubRenderable(uint8 uIdx) =0;
	/**
	 @brief 设置渲染管道
	 @return 
	 @param uint8 layer
	 @remark 
	*/
	virtual void setRenderPipe(uint8 layer) =0;
	/**
	 @brief 显示renderlayer的包围盒
	 @return 
	 @param bool bShow
	 @remark 
	*/
	virtual void showBBox(bool bShow) =0;
	/**
	 @brief 取得包围盒
	 @return 
	 @param void
	 @remark 
	*/
	virtual const AABB& getAABB(void) const =0;

	virtual const AABB& getWorldAABB(void) const =0;
	/**
	 @brief 设置renderlayer的可见性
	 @return 
	 @param bool visible
	 @remark 
	*/
	virtual void setVisible(bool visible)=0;
	/**
	 @brief 设置一个mask颜色
	 @return 
	 @param float maskColor
	 @remark 用于特殊显示的颜色(OutGlow等)
	*/
	virtual void setMaskColor(float maskColor) =0;
	/**
	 @brief 取得mask颜色
	 @return 
	 @remark 
	*/
	virtual float getMaskColor() =0;
	/**
	 @brief 是否产生阴影
	 @return 
	 @param bool enable
	 @remark 设置是否产生阴影，默认值：产生
	*/
	virtual void enableCastShadow(bool enable) =0;

	//////////////////////////////////////////////////////////////////////////
	// 变换信息


	/**
	 @brief 设置朝向
	 @return 
	 @param const Quat & q
	 @remark 
	*/
	virtual void setOrientation( const Quat& q ) =0;
	/**
	 @brief 设置朝向
	 @return 
	 @param float w
	 @param float x
	 @param float y
	 @param float z
	 @remark 
	*/
	virtual void setOrientation( float w, float x, float y, float z) =0;

	/**
	 @brief 获取朝向
	 @return 
	 @remark 
	*/
	virtual const Quat & getOrientation() const =0;
	/**
	 @brief 获取世界空间绝对朝向
	 @return 
	 @remark 
	*/
	virtual const Quat & getWorldOrientation() const =0;

	// position
	/**
	 @brief 设置位置
	 @return 
	 @param const Vec3 & pos
	 @remark 
	*/
	virtual void setPosition(const Vec3& pos) =0;
	/**
	 @brief 设置位置
	 @return 
	 @param float x
	 @param float y
	 @param float z
	 @remark 
	*/
	virtual void setPosition(float x, float y, float z) =0;

	/**
	 @brief 设置世界空间绝对位置
	 @return 
	 @param const Vec3 & pos
	 @remark 
	*/
	virtual void setWorldPosition(const Vec3& pos) =0;
	/**
	 @brief 设置世界空间绝对位置
	 @return 
	 @param float x
	 @param float y
	 @param float z
	 @remark 
	*/
	virtual void setWorldPosition(float x, float y, float z) =0;

	/**
	 @brief 取得位置
	 @return 
	 @param void
	 @remark 
	*/
	virtual const Vec3 &	getPosition(void) const =0;
	/**
	 @brief 取得世界空间绝对位置
	 @return 
	 @param void
	 @remark 
	*/
	virtual const Vec3&		getWorldPosition(void) const =0;

	// scaling
	/**
	 @brief 设置缩放
	 @return 
	 @param const Vec3 & scale
	 @remark 
	*/
	virtual void setScale(const Vec3& scale) =0;
	/**
	 @brief 设置缩放
	 @return 
	 @param float x
	 @param float y
	 @param float z
	 @remark 
	*/
	virtual void setScale(float x, float y, float z) =0;
	virtual const Vec3 & getScale(void) const =0;

	// get world matrix
	/**
	 @brief 取得世界矩阵
	 @return 
	 @remark 
	*/
	virtual const Matrix44& GetWorldMatrix() const =0;



	//////////////////////////////////////////////////////////////////////////
	// 相对变换

	// translate
	/**
	 @brief 位移，TRANSFORMSPACE决定空间
	 @return 
	 @param const Vec3 & d
	 @param gkTransformSpace relativeTo
	 @remark 
	*/
	virtual void translate(const Vec3& d, gkTransformSpace relativeTo = TS_PARENT) =0;
	/**
	 @brief 位移，TRANSFORMSPACE决定空间
	 @return 
	 @param float x
	 @param float y
	 @param float z
	 @param gkTransformSpace relativeTo
	 @remark 
	*/
	virtual void translate(float x, float y, float z, gkTransformSpace relativeTo = TS_PARENT) =0;
	/**
	 @brief 旋转，TRANSFORMSPACE决定空间
	 @return 
	 @param const Vec3 & axis
	 @param float angle
	 @param gkTransformSpace relativeTo
	 @remark 
	*/
	virtual void rotate(const Vec3& axis, float angle, gkTransformSpace relativeTo = TS_LOCAL) =0;
	/**
	 @brief 旋转，TRANSFORMSPACE决定空间
	 @return 
	 @param const Quat & q
	 @param gkTransformSpace relativeTo
	 @remark 
	*/
	virtual void rotate(const Quat& q, gkTransformSpace relativeTo = TS_LOCAL) =0;

	/**
	 @brief 头冲z轴上方的指向
	 @return 
	 @param const Vec3 & point
	 @param bool bHeadup
	 @remark 
	*/
	virtual void lookat(const Vec3& point, bool bHeadup = true) =0;
	/**
	 @brief 带缓动的指向
	 @return 
	 @param const Vec3 & point
	 @param float fDamping
	 @param bool bHeadup
	 @remark 
	*/
	virtual void smoothLookat(const Vec3& point, float fDamping, bool bHeadup = true) =0;

	/**
	 @brief 克隆renderlayer
	 @return 
	 @param const gkStdString & newName
	 @remark 
	*/
	virtual IGameObjectLayer* clone( const gkStdString& newName) const =0;


	/**
	 @brief 添加子gameobject
	 @return 
	 @param IGameObject * ent
	 @remark 
	*/
	virtual void attachEntity(IGameObject* ent) =0;
	/**
	 @brief 移除子gameobject
	 @return 
	 @param IGameObject * ent
	 @remark 
	*/
	virtual	void detachChildEntity(IGameObject* ent) =0;
};




	/**
	 @ingroup GOSYSAPI
	 @brief 物理层
	 @remark 需要物理属性和物理控制的gameobject，需要绑定物理层。创建对应的物理代理，来实现不同的物理性质
	*/
struct IGameObjectPhysicLayer : public IGameObjectLayer
{
	/**
	 @brief 物理层类型
	 @remark 
	*/
	enum EPhysicLayerType
	{
		ePLT_Static,			// 静态碰撞体
		ePLT_Dynamic,			// 动态物理体
		ePLT_Character,			// 角色物理体
	};
	/**
	 @brief 动态物理体类型
	 @remark 
	*/
	enum EPhysicDynamicType
	{
		ePDT_Sphere,			// 球形模拟
		ePDT_Box,				// 盒形模拟
		ePDT_Cylinder,			// 圆柱模拟
		ePDT_Convex,			// 凸形模拟
	};

	virtual ~IGameObjectPhysicLayer(void) {}
	virtual EGameObjectLayerType getType() const {return eGL_PhysicLayer;}
	virtual IGameObjectLayer* clone( const gkStdString& newName) const {return NULL;}


	/**
	 @brief 
	 @return 
	 @param Vec3 worldpos
	 @remark 
	*/
	virtual bool teleport(Vec3 worldpos) =0;

	// create static physics based on mesh
	virtual bool createStatic() =0;


	virtual bool createDynamic(EPhysicDynamicType type) {return false;}

	virtual bool createHeightMap(const gkStdString& heightMapName, float maxHeight) =0;

	// create a cc based on charamesh
	virtual bool createCharacterController() =0;

	virtual EPhysicLayerType getPhysicType() =0;
};

struct IGameObjectScirptLayer : public IGameObjectLayer
{
	virtual ~IGameObjectScirptLayer(void) {}
	virtual EGameObjectLayerType getType() const {return eGL_ScirptLayer;}
	virtual IGameObjectLayer* clone( const gkStdString& newName) const {return NULL;}
};

/**
 *@brief 粒子层
 */
struct IGameObjectParticleLayer : public IGameObjectLayer
{
	virtual ~IGameObjectParticleLayer(void) {}
	virtual EGameObjectLayerType getType() const {return eGL_ParticleLayer;}
	virtual IGameObjectLayer* clone( const gkStdString& newName) const {return NULL;}

	/**
	*@brief 读取粒子文件，直接建立粒子
	*/
	virtual void loadPtcFile(const TCHAR* filename) =0;

};

/**
 *@brief 动作层
 */
struct IGameObjectAnimLayer : public IGameObjectLayer
{
	virtual ~IGameObjectAnimLayer(void) {}
	virtual EGameObjectLayerType getType() const {return eGL_AnimLayer;}
	virtual IGameObjectLayer* clone( const gkStdString& newName) const {return NULL;}

	/**
	*@brief 读取角色文件，直接建立角色
	*/
	virtual bool loadChrFile(const TCHAR* filename) =0;

	// animation control
	virtual void playAnimation(const TCHAR* animName, float blendTime) =0;
	virtual void stopAnimation(const TCHAR* animName, float blendTime) =0;
	virtual void setAnimationWeight(const TCHAR* animName, float weight) =0;
	virtual void stopAllAnimation(float blendTime) =0;
	virtual void setAnimationSpeed(const TCHAR* animName, float speed) =0;
	virtual void setAnimationLocalTime(const TCHAR* animName, float localTime  ) =0;

	virtual int getAnimationCount() =0;
	virtual const TCHAR* getAnimationName(int index) =0;
};

struct IGameObjectLightLayer : public IGameObjectLayer
{
	virtual ~IGameObjectLightLayer(void) {}
	virtual EGameObjectLayerType getType() const {return eGL_LightLayer;}
	virtual IGameObjectLayer* clone( const gkStdString& newName) const {return NULL;}

	// final position get
	virtual const Vec3& getDerivedPosition(bool cameraRelativeIfSet = true) const =0;
	virtual const Vec3& getDerivedDirection(bool cameraRelativeIfSet = true) const =0;


	//! 光源强度属性
	virtual void setLightIntensity(float inten) =0;
	virtual float getLightIntensity() const =0;
	virtual float* getLightIntensityPtr_Editor() =0;
	virtual float* getLightIntensitySpecPtr_Editor() =0;
	

	//! 光源颜色属性
	virtual void setDiffuseColor(float red, float green, float blue) =0;
	virtual const ColorF& getDiffuseColor(void) const =0;
	/// light value ptr for editor
	virtual float* getDiffuseColorPtr_Editor() =0;

	//! 半径属性
	virtual void setRadius(float radius) =0;
	virtual float getRadius() =0;
	/// light value ptr for editor
	virtual float* getRadiusPtr_Editor() =0;

	//! 假阴影属性
	virtual bool getFakeShadow() =0;
	virtual void setFakeShadow(bool isFakeShadow) =0;
	/// light value ptr for editor
	virtual bool* getFakeShadowPtr_Editor() =0;

	//! 全局阴影属性
	virtual bool getGlobalShadow() =0;
	virtual void setGlobalShadow(bool enable) =0;
	virtual bool* getGlobalShadowPtr_Editor() =0;
};



#endif