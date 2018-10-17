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
// Name:   	gkLight.h
// Desc:	GameKnife的灯光抽象，渲染中用来向shader提供灯光参数
//			向shadowmap提供矩阵。目前只考虑点光源和平行光
// 描述:	
// 
// Author:  Kaiming-Desktop	 
// Date:	12/30/2010 	
// 
//////////////////////////////////////////////////////////////////////////
#pragma once
#include "gkPrerequisites.h"
#include "gkMovableObject.h"
#include "IGameObjectLayer.h"

class gkLight : public gkMovableObject, public IGameObjectLightLayer
{
public:
	/// Defines the type of light
	enum gkLightTypes
	{
		/// Point light sources give off light equally in all directions, so require only position not direction
		LT_POINT = 0,
		/// Directional lights simulate parallel light beams from a distant source, hence have direction but no position
		LT_DIRECTIONAL = 1,
		/// Spotlights simulate a cone of light from a source so require position and direction, plus extra values for falloff
		LT_SPOTLIGHT = 2
	};

public:
	gkLight(void);
	gkLight(gkStdString name);
	virtual ~gkLight(void);

	/// 灯光类型设置
	void setLightType(gkLightTypes type);
	/// 灯光类型设置
	gkLightTypes getLightType(void) const;

	virtual void setLightIntensity(float inten) {m_fIntensity = inten;}
	virtual float getLightIntensity() const {return m_fIntensity;}
	virtual float* getLightIntensityPtr_Editor() {return &m_fIntensity;}
	virtual float* getLightIntensitySpecPtr_Editor() {return &m_fIntensitySpec;}

	

	/// 灯光颜色
	virtual void setDiffuseColor(float red, float green, float blue);
	virtual const ColorF& getDiffuseColor(void) const;
	virtual float* getDiffuseColorPtr_Editor() {return &(m_vDiffuse.r);}

	virtual float getRadius() {return m_fRadius;}
	virtual float* getRadiusPtr_Editor() {return &m_fRadius;}
	virtual void setRadius(float radius){m_fRadius = radius;}

	virtual bool getFakeShadow() {return m_bFakeShadow;}
	virtual void setFakeShadow(bool isFakeShadow) {m_bFakeShadow = isFakeShadow;} 
	/// light value ptr for editor
	virtual bool* getFakeShadowPtr_Editor() { return &m_bFakeShadow; }

	//! 全局阴影属性
	virtual bool getGlobalShadow() {return m_bGlobalShadow;}
	virtual void setGlobalShadow(bool enable) {m_bGlobalShadow = enable;}
	virtual bool* getGlobalShadowPtr_Editor() {return &m_bGlobalShadow;}

	/// 天光颜色
	void setAmbientColor(float red, float green, float blue);
	/// 取得天光颜色
	const ColorF& getAmbientColor(void) const;
	

	/// 设置光源位置，针对点光源
	void setPosition(float x, float y, float z);
	/// 
	void setPosition(const Vec3& vec);

	/// 取得光源位置
	const Vec3& getPosition(void) const;

	/// 设置光源方向，针对平行光
	void setDirection(float x, float y, float z);
	///
	void setDirection(const Vec3& vec);
	/// 取得光源方向
	const Vec3& getDirection(void) const;

	/// 设置光强
	void setLightPower(float fLightPower)
	{
		m_fLightPower = fLightPower;
	}
	/// 取得光强
	float getLightPower()
	{
		return m_fLightPower;
	}



	// override
	// 覆盖movable接口
	virtual void _updateRenderSequence(IRenderSequence* queue);
	// 覆盖movable接口
	virtual const gkStdString& getMovableType() const;
	// 覆盖movable接口
	virtual const AABB& getAABB(void) const;

	// 取得绝对位置
	virtual const Vec3& getDerivedPosition(bool cameraRelativeIfSet = true) const;

	// 取得绝对朝向 
	virtual const Vec3& getDerivedDirection(bool cameraRelativeIfSet = true) const;


	// 设置阴影投射近平面，针对平行光
	void setShadowNearClipDistance(float nearClip) { m_fShadowNearClipDist = nearClip; }
	// 设置阴影投射近平面，正对平行光
	float getShadowNearClipDistance() const { return m_fShadowNearClipDist; }

	// 从目标摄像机取得进平面
	float _deriveShadowNearClipDistance(const gkCamera* maincam) const;

	void setShadowFarClipDistance(float farClip) { m_fShadowFarClipDist = farClip; }
	float getShadowFarClipDistance() const { return m_fShadowFarClipDist; }
	float _deriveShadowFarClipDistance(const gkCamera* maincam) const;

	/// 设置一个伴随camera, 设置后位置就由camera来获得了
	void _setCameraRelative(gkCamera* cam);

public:
	// IGameObjectLayer Override
	virtual void Destroy();
	virtual void UpdateLayer(float fElpasedTime);


protected:
	/// internal method for synchronising with parent node (if any)
	virtual void update(void) const;

	gkLightTypes m_yLightType;
	Vec3 m_vPosition;
	ColorF m_vDiffuse;
	ColorF m_vAnimatedDiffuse;
	ColorF m_vAmbient;
	ColorF m_vSpecular;

	float m_fIntensity;
	float m_fIntensitySpec;
	float m_fRadius;

	bool m_bFakeShadow;
	bool m_bGlobalShadow;

	Vec3 m_vDirection;
	float m_fLightPower;

	size_t mIndexInFrame;

	float m_fShadowNearClipDist;
	float m_fShadowFarClipDist;


	mutable Vec3 m_vDerivedPosition;
	mutable Vec3 m_vDerivedDirection;
	// 如果挂载了 cam，则取得cam的pos和dir作为dir
	mutable Vec3 m_vDerivedCamRelativePosition;
	mutable Vec3 m_vDerivedCamRelativeDirection;
	mutable bool m_bDerivedCamRelativeDirty;
	gkCamera* m_pCameraToBeRelativeTo;

	/// Shared class-level name for Movable type
	static gkStdString m_wstrMovableType;

	/// Is the derived transform dirty?
	mutable bool m_bDerivedTransformDirty;
};

/** Factory object for creating Light instances */
class GAMEKNIFECORE_API gkLightFactory : public gkMovableObjectFactory
{
protected:
	gkMovableObject* createInstanceImpl( const gkStdString& name, const NameValuePairList* params, void* userdata);
public:
	gkLightFactory() {}
	~gkLightFactory() {}

	static gkStdString FACTORY_TYPE_NAME;

	const gkStdString& getType(void) const;
	void destroyInstance( gkMovableObject* obj);  

};