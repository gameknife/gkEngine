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
// Name:   	gkSceneManager.h
// Desc:	场景管理器，GameKnifeEngine的核心类，控制场景的
//			逻辑任务调度，渲染任务调度，资源调度等
// 描述:	
// 
// Author:  Kaiming-Desktop	 
// Date:	9/20/2010 	
// 
//////////////////////////////////////////////////////////////////////////

#ifndef gkSceneManager_h_916d8f63_9bbb_4d54_af01_dd1de7214666
#define gkSceneManager_h_916d8f63_9bbb_4d54_af01_dd1de7214666

#include "gkPrerequisites.h"
#include "gkrenderablelist.h"
#include "IShader.h"
#include "gkDecalsManager.h"
#include "gkIterator.h"
#include "gkSceneQuery.h"
#include "I3DEngine.h"

struct IGameObject;
class gkParticleInstance;
/** Default implementation of RaySceneQuery. */
class gkDefaultRaySceneQuery : public gkRaySceneQuery
{
public:

	gkDefaultRaySceneQuery(gkSceneManager* creator);
	~gkDefaultRaySceneQuery();

	/** See RayScenQuery. */
	gkRaySceneQueryResult& execute(void);
};

struct IRenderSequence;

class gkSceneManager {
protected:
	/// Camera工作中的
	//gkCamera*					m_pCameraInProgress;
	/// 递交给shader处理的数据源
	/// 渲染队列
	IRenderSequence*			m_pUpdatingRenderSequence;
	/// Root scene node
	gkSceneNode* m_pSceneRoot;
	/// shadowmap创建shader
	gkShaderPtr					m_pShadowMapShader;
	/// zprepass创建shader
	gkShaderPtr					m_pZprepassShader;
	/// zprepass创建shader
	gkShaderPtr					m_pReflMapShader;
	/// 光源向量（点：位置，平行：方向）
	Vec3						m_vCurrentLightVector;
	BYTE						m_yLightType;

	gkSun*						m_pSun;

	gkDecalsManager				m_DecalManager;

	/// 一些开关
	/// 是否进行裁剪算法
	bool m_bFindVisibleObjects;

	typedef std::map<gkStdString, gkCamera* > CameraList;
	CameraList m_pCameraList;

	typedef std::map<gkStdString, gkSceneNode*> SceneNodeList;
	SceneNodeList m_mapSceneNodes;

	typedef std::map<gkStdString, gkMovableObject*> MovableObjectMap;
	typedef std::map<gkStdString, MovableObjectMap*> MovableObjectCollectionMap;


	/// 名字生成器
	gkNameGenerator* m_MovableNameGenerator;
	

	virtual IRenderSequence* getRenderSequence(bool deferred = false);


	gkDefaultRaySceneQuery* m_pRayQueryer;
	gkRenderLightList	m_RenderLightList;

public:
	gkSceneManager(void);
	virtual ~gkSceneManager(void);

	virtual void setRenderSequence(IRenderSequence* seq) {m_pUpdatingRenderSequence = seq;}
	virtual void _updateSceneGraph();

	MovableObjectCollectionMap m_mapMovableObjectCollectionMap;
	virtual gkSceneManager::MovableObjectMap* getMovableObjectCollection(const gkStdString& typeName);
	virtual const gkSceneManager::MovableObjectMap* getMovableObjectCollection(const gkStdString& typeName) const;
	typedef MapIterator<MovableObjectMap> MovableObjectIterator;
	gkSceneManager::MovableObjectIterator getMovableObjectIterator(const gkStdString& typeName);

	virtual	void clearScene(void);

	void _sceneStart(gkCamera* pMainCamera);
	virtual void _prepareRenderSequences();

	virtual ICamera* createICamera(const gkStdString& wstrName);
	virtual gkCamera* createCamera(const gkStdString& wstrName);
	virtual gkCamera* getCamera(const gkStdString& wstrName) const;
	virtual bool hasCamera(const gkStdString& wstrName) const;
	virtual void destroyCamera(gkCamera *cam);
	virtual void destroyCamera(const gkStdString& wstrName);
	virtual void destroyAllCameras(void);

	virtual gkSceneNode* createSceneNodeImpl(void);
	virtual gkSceneNode* createSceneNodeImpl(const gkStdString& name);
	virtual gkSceneNode* getRootSceneNode(void);
	virtual gkSceneNode* createSceneNode(void);

	virtual gkSceneNode* createSceneNode(const gkStdString& name);

	virtual	gkSceneNode* getSceneNode(const gkStdString& name) const;
	virtual	bool hasSceneNode(const gkStdString& name) const;

	virtual void destroySceneNode(const gkStdString& name);

	virtual void destroySceneNode(gkSceneNode* sn);
	virtual void _findVisibleObjects( gkCamera* cam );

	// movable object

	/// 调用工厂创建MovableObject
	virtual gkMovableObject* createMovableObject(const gkStdString& name, 
		const  gkStdString& typeName, const NameValuePairList* params = 0);
	/// 调用工厂创建MovableObject
	virtual gkMovableObject* createMovableObject(const  gkStdString& typeName, const NameValuePairList* params = 0);

	/// 调用工厂销毁MovableObject
	virtual void destroyMovableObject(const  gkStdString& name, const  gkStdString& typeName);
	/// 调用工厂销毁MovableObject
	virtual void destroyMovableObject(gkMovableObject* m);
	/// 调用工厂销毁所有MovableObject
	virtual void destroyAllMovableObjectsByType(const  gkStdString& typeName);
	/// 调用工厂销毁所有MovableObject
	virtual void destroyAllMovableObjects(void);
	/// 按名字和类型取Movable
	virtual gkMovableObject* getMovableObject(const  gkStdString& name, const  gkStdString& typeName) const;
	/// 按名字和类型查找是否有Movable
	virtual bool hasMovableObject(const  gkStdString& name, const  gkStdString& typeName) const;

	// 迭代器相关，暂不实现 
// 	typedef MapIterator<gkMovableObjectMap> MovableObjectIterator;
// 	virtual MovableObjectIterator getMovableObjectIterator(const String& typeName);

	// gameobject相关函数
	virtual gkEntity* createEntity(const gkStdString& gameobjectName );
	virtual gkEntity* createEntity();
	virtual gkEntity* getGameObject(const gkStdString& gameobjectName);

	// gamelight相关函数
	virtual gkSun* createSun();
	virtual gkLight* createLight(const gkStdString& lightName);
	virtual gkLight* createLight();
	virtual gkLight* getLight(const gkStdString& lightName);
	virtual gkSun* getSun() const;

	virtual gkParticleInstance* createParticle(const gkStdString& lightName);
	virtual gkParticleInstance* createParticle();

	gkDecalsManager& getDecalManager(){return m_DecalManager;}

	// query相关 [8/22/2011 Kaiming-Desktop]
	virtual SRayhitResult getRayHitEntity(const Ray& ray, uint32 ignoreFirstNum = 0);
};



#endif // gkSceneManager_h_916d8f63_9bbb_4d54_af01_dd1de7214666
