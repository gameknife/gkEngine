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
// yikaiming (C) 2013
// gkENGINE Source File 
//
// Name:   	I3DEngine.h
// Desc:	
// 	
// 
// Author:  YiKaiming
// Date:	2011/8/5
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _I3DEngine_h_
#define _I3DEngine_h_

struct ICamera;
class ISceneNode;
struct IGameObject;
struct IGameObjectRenderLayer;
struct IGameObjectLightLayer;
struct IGameObjectParticleLayer;

struct ITimeOfDay;
struct Ray;
struct gkBaseMesh;

#include "gk_Math.h"
//#include "gk_Geo.h"
#include "gk_Color.h"

#define GSM_SHADOWCASCADE1_DELAY 17
#define GSM_SHADOWCASCADE2_DELAY 63

enum E3DRenderableType
{
	e3RT_StaticObj,
	e3RT_TerrianBlock,
	e3RT_Other,
};

class gkMeshPtr;
struct gkRenderable;
struct ITerrianSystem;
	/**
	 @brief 场景raycast的结果结构
	 @remark 
	*/
struct SRayhitResult
{
	std::vector<IGameObject*> pObjects;
	uint32 nNum;

	SRayhitResult()
	{
		nNum = 0;
		pObjects.clear();
	}
};
	/**
	 @ingroup CoreModuleAPI
	 @brief 3D世界驱动模块
	 @remark 
	*/
struct UNIQUE_IFACE I3DEngine 
{
public:
	virtual ~I3DEngine(void) {}

	/**
	 @brief 初始化
	 @return 
	 @remark 
	*/
	virtual void Init() =0;
	/**
	 @brief 销毁
	 @return 
	 @remark 
	*/
	virtual void Destroy() =0;

	//////////////////////////////////////////////////////////////////////////
	// 相机操作

	/**
	 @brief 创建一个相机
	 @return 
	 @param const gkStdString & wstrName
	 @remark 
	*/
	virtual ICamera* createCamera(const gkStdString& wstrName) =0;
	/**
	 @brief 取得一个相机
	 @return 
	 @param const gkStdString & wstrName
	 @remark 
	*/
	virtual ICamera* getCamera(const gkStdString& wstrName) const =0;
	/**
	 @brief 查找相机是否存在
	 @return 
	 @param const gkStdString & wstrName
	 @remark 
	*/
	virtual bool hasCamera(const gkStdString& wstrName) const =0;
	/**
	 @brief 销毁一个相机
	 @return 
	 @param const gkStdString & wstrName
	 @remark 
	*/
	virtual void destroyCamera(const gkStdString& wstrName) =0;
	/**
	 @brief 销毁所有相机
	 @return 
	 @param void
	 @remark 
	*/
	virtual void destroyAllCameras(void) =0;

	virtual void setMainCamera( ICamera* cam )=0;
	virtual ICamera* getMainCamera()=0;
	virtual ICamera* getRenderingCamera()=0;

	virtual void pushReplaceCamera( ICamera* cam ) =0;
	virtual void popReplaceCamera() =0;
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	// renderlayer操作

	/**
	 @brief 创建一个renderlayer
	 @return 
	 @param const gkStdString & gameobjectName layer名称
	 @param const Vec3 & pos 初始位置
	 @param const Quat & quat 初始朝向
	 @remark 
	 @sa IGameObjectRenderLayer
	*/
	virtual IGameObjectRenderLayer* createRenderLayer(const gkStdString& gameobjectName, const Vec3& pos, const Quat& quat) =0;
	/**
	 @brief 创建一个renderlayer
	 @return 
	 @remark 自动赋予名字,初始化在原点，正朝向
	 @sa IGameObjectRenderLayer
	*/
	virtual IGameObjectRenderLayer* createRenderLayer() =0;

	/**
	 @brief 删除renderlayer
	 @return 
	 @param const gkStdString & meshName
	 @remark 
	*/
	virtual void removeRenderLayer(const gkStdString& meshName) =0;
	/**
	 @brief 删除renderlayer
	 @return 
	 @param IGameObjectRenderLayer * entity
	 @remark 
	*/
	virtual void removeRenderLayer(IGameObjectRenderLayer* entity) =0;
	/**
	 @brief 取得renderlayer
	 @return 
	 @param const gkStdString & meshName
	 @remark 
	*/
	virtual IGameObjectRenderLayer* getRenderLayer(const gkStdString& meshName) =0;
		/**
	 @brief 清理所有renderalyer
	 @return 
	 @remark 
	*/
	virtual void destroyAllRenderLayer() =0;


	//////////////////////////////////////////////////////////////////////////
	// terrian操作

	/**
	 @brief 创建地形
	 @return 
	 @remark 地形目前是全局只能存在一个的
	*/
	virtual ITerrianSystem* createTerrian() =0;
	/**
	 @brief 删除地形
	 @return 
	 @param ITerrianSystem * target
	 @remark 
	*/
	virtual void destroyTerrian(ITerrianSystem* target) =0;
	/**
	 @brief 同步删除地形
	 @return 
	 @param ITerrianSystem * target
	 @remark 慎用，必须在线程保护区间使用
	*/
	virtual void destroyTerrianSync(ITerrianSystem* target) =0;
	/**
	 @brief 获取唯一的地形对象
	 @return 
	 @remark 
	*/
	virtual ITerrianSystem* getTerrian() =0;
	//////////////////////////////////////////////////////////////////////////




	//////////////////////////////////////////////////////////////////////////
	// lightlayer操作

	/**
	 @brief 创建lightlayer
	 @return 
	 @param IGameObject * parent
	 @param const gkStdString & lightName
	 @param Vec3 & lightpos
	 @param float lightRadius
	 @param ColorF & lightColor
	 @param bool isFakeShadow
	 @remark 
	*/
	virtual IGameObjectLightLayer* createLightLayer(IGameObject* parent, const gkStdString& lightName, Vec3& lightpos, float lightRadius, ColorF& lightColor, bool isFakeShadow = false) =0;
		/**
	 @brief 清理所有lightlayer
	 @return 
	 @remark 
	*/
	virtual void destroyAllLightLayer() =0;

	/**
	 @brief 创建particlelayer
	 @return 
	 @param IGameObject * parent
	 @param const gkStdString & particleName
	 @param const gkStdString & ptcFilename
	 @remark 
	*/
	virtual IGameObjectParticleLayer* createParticleLayer(IGameObject* parent, const gkStdString& particleName, const gkStdString& ptcFilename ) =0;

	// renderable Creation
	/**
	 @brief 根据renderable type创建renderable
	 @return 
	 @param gkMeshPtr & meshptr 创建renderable的mesh
	 @param E3DRenderableType type
	 @param uint32 subsetID
	 @remark 
	 @sa gkRenderable
	*/
	virtual gkRenderable* createRenderable(gkMeshPtr& meshptr, E3DRenderableType type, uint32 subsetID ) =0;

 	/**
 	 @brief 创建全局太阳
 	 @return 
 	 @remark 
 	*/
 	virtual void createSun() =0;

	/**
	 @brief 取得TIMEOFDAY接口
	 @return 
	 @remark 
	*/
	virtual ITimeOfDay* getTimeOfDay() =0;
	/**
	 @brief 设置TIMEOFDAY的当前时间
	 @return 
	 @param float time
	 @remark 
	*/
	virtual void setTimeOfDay(float time) =0;

	//////////////////////////////////////////////////////////////////////////
	// 临时测试函数
	virtual void setSnowAmount(float amount) =0;
	virtual float getSnowAmount() =0;



	//////////////////////////////////////////////////////////////////////////
	// scene query


	/**
	 @brief 场景内射线检查，返回第一个碰到的gameobject
	 @return 
	 @param Ray & ray
	 @param uint32 index
	 @remark 
	*/
	virtual IGameObject* getRayHitEntity(Ray& ray, uint32 index = 0) =0;
	/**
	 @brief 场景内射线检查，返回所有平道的gameobject
	 @return 
	 @param Ray & ray
	 @param uint32 ignoreFirstNum
	 @remark 
	*/
	virtual SRayhitResult getRayHitEntitys(Ray& ray, uint32 ignoreFirstNum = 0) =0;
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	// 更新内部接口


	/**
	 @brief 线程同步更新
	 @return 
	 @remark 内部模块调用，一般不需理会
	 \n内部实现负责场景图的更新，渲染队列的交换等
	*/
	virtual bool syncupdate() =0;
	/**
	 @brief 线程异步更新
	 @return 
	 @param float fElapsedTime
	 @remark 内部模块调用，一般不需理会
	 \n内部实现负责场景管理，渲染队列的填充等
	*/
	virtual void update(float fElapsedTime) =0;
};

#endif