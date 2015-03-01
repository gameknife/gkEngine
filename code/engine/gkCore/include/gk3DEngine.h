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
// Name:   	gkRoot.h
// Desc:	todo...
// 描述:	
// 
// Author:  Kaiming-Desktop	 
// Date:	9/20/2010 	
// 
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "gkPrerequisites.h"
#include "gkSingleton.h"
#include "I3DEngine.h"

class gkTimeOfDayManager;
struct IGameObjectRenderLayer;
class gkMeshPtr;
class gkTerrian;

class gk3DEngine : public Singleton<gk3DEngine> , public I3DEngine
{
	typedef std::stack<ICamera*> CameraStack;
protected:
	// Graphics Module
	// 图形模块
	gkSceneManager*		m_pSceneMng;
	gkTimeOfDayManager* m_pTODManager;

	typedef std::vector<ITerrianSystem*> Terrians;
	Terrians			m_terrians;
	Terrians			m_destroyterrians;

	typedef std::map<gkStdString, gkMovableObjectFactory*> MovableObjectFactoryMap;
	MovableObjectFactoryMap m_mapMovableObjectFactoryMap;

	// movable工厂
	gkMovableObjectFactory* m_pGameObjectFactory;					// GameObject工厂
	gkMovableObjectFactory* m_pLightFactory;						// Light工厂
	gkMovableObjectFactory* m_pParticleFactory;

	float m_fSnowAmount;	// temp value for fast implement [11/23/2011 Kaiming]
	float m_WhiteMouse;	// temp value for fast implement [11/23/2011 Kaiming]

	ICamera* m_mainCam;
	CameraStack m_camStack;
public:
	static gk3DEngine& getSingleton(void);
	static gk3DEngine* getSingletonPtr(void);

	gk3DEngine(void);
	virtual ~gk3DEngine(void);

	// I3DENGINE
	virtual void Init();
	virtual void Destroy();

	virtual ICamera* createCamera(const gkStdString& wstrName);
	virtual ICamera* getCamera(const gkStdString& wstrName) const;
	virtual bool hasCamera(const gkStdString& wstrName) const;
	virtual void destroyCamera(const gkStdString& wstrName);
	virtual void destroyAllCameras(void);

	virtual void setMainCamera( ICamera* cam );
	virtual ICamera* getMainCamera();
	virtual ICamera* getRenderingCamera();

	virtual void pushReplaceCamera( ICamera* cam ) { m_camStack.push(cam);}
	virtual void popReplaceCamera() {if (!m_camStack.empty()) m_camStack.pop(); }

	// gameobject相关函数
	virtual IGameObjectRenderLayer* createRenderLayer(const gkStdString& gameobjectName, const Vec3& pos, const Quat& quat );
	virtual IGameObjectRenderLayer* createRenderLayer();
	virtual ITerrianSystem* createTerrian();
	virtual void destroyTerrian(ITerrianSystem* target);
	virtual void destroyTerrianSync(ITerrianSystem* target);
	virtual void destroyTerrianImm(ITerrianSystem* target);
	virtual ITerrianSystem* getTerrian();

	virtual void removeRenderLayer(const gkStdString& gameobjectName);
	virtual void removeRenderLayer(IGameObjectRenderLayer* entity);
	virtual IGameObjectRenderLayer* getRenderLayer(const gkStdString& meshName);
	
	// renderable Creation
	virtual gkRenderable* createRenderable(gkMeshPtr& meshptr, E3DRenderableType type, uint32 subsetID);

	// gamelight相关函数
	virtual void createSun();
	virtual ITimeOfDay* getTimeOfDay();
	virtual void setTimeOfDay(float time);

	virtual const Vec3& getSunDir();
	virtual IGameObjectLightLayer* createLightLayer(IGameObject* parent, const gkStdString& lightName, Vec3& lightpos, float lightRadius, ColorF& lightColor, bool isFakeShadow = false);

	virtual IGameObjectParticleLayer* createParticleLayer(IGameObject* parent, const gkStdString& particleName, const gkStdString& ptcFilename );

	// 销毁
	virtual void destroyAllRenderLayer();
	virtual void destroyAllLightLayer();
	
	virtual bool syncupdate();
    virtual void update(float fElapsedTime);
	virtual void _SwapRenderSequence();
	
	virtual void setSnowAmount(float amount) {m_fSnowAmount = amount;}
	virtual float getSnowAmount() {return m_fSnowAmount;}

	
	// Factory Functions
	/// 加入工厂
	void addMovableObjectFactory(gkMovableObjectFactory* fact, bool overrideExisting);
	/// 移除工厂
	void removeMovableObjectFactory(gkMovableObjectFactory* fact);
	/// 查询是否存在工厂
	bool hasMovableObjectFactory(const gkStdString& typeName) const;
	/// 取得工厂
	gkMovableObjectFactory* getMovableObjectFactory(gkStdString typeName);

	virtual IGameObject* getRayHitEntity(Ray& ray, uint32 index = 0);
	virtual SRayhitResult getRayHitEntitys(Ray& ray, uint32 ignoreFirstNum = 0);

	// 取得Manager函数
	//////////////////////////////////////////////////////////////////////////
	// getPointer
	gkSceneManager* getSceneMngPtr()
	{
		return m_pSceneMng;
	}
};

//////////////////////////////////////////////////////////////////////////
// globle function getPointer
gkSceneManager* getSceneMngPtr();
gkVtxLayerManager* getVtxLayerMngPtr();
gk3DEngine* get3DEngine();


