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
// Name:   	IRenderer.h
// Desc:	
// 	
// 
// Author:  YiKaiming
// Date:	2011/7/23
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _IRenderer_h_
#define _IRenderer_h_

#include "gkPlatform.h"
#include "MathLib/gk_Math.h"
#include "MathLib/gk_Color.h"
#include "MathLib/gk_Geo.h"
#include "IShader.h"
class IResourceManager;
struct IAuxRenderer;
struct IRenderSequence;
struct IParticleProxy;

#define PROFILE_LABEL_PUSH(X) { wchar_t buf[128]; size_t outCount=0; mbstowcs_s(&outCount, buf, X, _countof(buf)-1); D3DPERF_BeginEvent(0xff00ff00, buf); }
#define PROFILE_LABEL_POP(X) D3DPERF_EndEvent();

	/**
	 @brief 渲染灯光
	 @remark 
	 @sa IRenderer
	*/
struct gkRenderLight
{
	Vec3 m_vPos;
	Vec3 m_vDir;
	ColorF m_vDiffuse;
	ColorF m_vSpecular;
	float m_fHDRPower;
	float m_fRadius;

	// impl futrue
	bool m_bProjected;
	bool m_bFakeShadow;
	bool m_bGlobalShadow;

	gkRenderLight()
	{
		m_vPos = Vec3(0,0,0);
		m_vDir = Vec3(0,0,1);
		m_vDiffuse = ColorF(1,1,1,1);
		m_vSpecular = ColorF(1,1,1,1);
		m_fHDRPower = 1;
		m_fRadius = 5;
		m_bProjected = false;
		m_bFakeShadow = false;
		m_bGlobalShadow = false;
	}
};

typedef std::list<gkRenderLight> gkRenderLightList;
	/**
	 @brief 渲染API枚举
	 @remark 
	*/
enum ERendererAPI
{
	ERdAPI_D3D9,
	ERdAPI_D3D11,
	ERdAPI_OPENGL,
	ERdAPI_OPENGLES,
};

	/**
	 @ingroup CoreModuleAPI
	 @brief 渲染器模块接口
	 @remark 渲染器是gkENGINE的最核心模块
	 \n该接口定义了渲染器面向其他模块和用户层的操作接口
	 \n渲染器接口不包含任何设备相关的操作，只提供抽象的操作
	 \n在渲染器的实际实现内实现和各设备相关的开发，同时在渲染器内实现各种设备资源
	*/
struct IRenderer
{
public:
	virtual ~IRenderer(void) {}
	// 取得设备
	virtual IResourceManager*	getResourceManager(BYTE type) =0;

	//////////////////////////////////////////////////////////////////////////
	// Initialize & Destroying
	/**
	 @brief 初始化渲染器
	 @return 
	 @param struct ISystemInitInfo & sii
	 @remark 初始化渲染器时，各渲染器内部负责渲染窗口的创建（各平台实现），负责资源管理器的构建和与system绑定。
	*/
	virtual HWND Init(struct ISystemInitInfo& sii) =0;
	/**
	 @brief 摧毁渲染器
	 @return 
	 @remark 
	*/
	virtual void Destroy() =0;

	// GPU粒子更新ROUTINE接口 [4/12/2013 YiKaiming]
	/**
	 @brief 创建一个GPU粒子的渲染器代理
	 @return 
	 @remark 渲染器代理负责管理GPU例子所需的设备资源
	*/
	virtual IParticleProxy* createGPUParticleProxy() =0;
	/**
	 @brief 摧毁GPU例子渲染器代理
	 @return 
	 @param IParticleProxy *
	 @remark 
	*/
	virtual void destroyGPUParticelProxy(IParticleProxy*) =0;

	


	//////////////////////////////////////////////////////////////////////////
	// RenderThread Interfaces

	/**
	 @brief 交换渲染队列
	 @return 
	 @remark 本函数在每帧的线程保护段调用
	*/
	virtual IRenderSequence* RT_SwapRenderSequence() =0;

	/**
	 @brief 取得更新队列
	 @return 
	 @remark 需要在Swap之后调用
	*/
	virtual IRenderSequence* RT_GetRenderSequence() =0;
	/**
	 @brief 清空渲染队列
	 @return 
	 @remark 一般的调用情形：
	 \n材质重载，影响渲染，清理并跳过一帧
	*/
	virtual void RT_CleanRenderSequence() =0;
	/**
	 @brief 渲染器一帧的启动函数
	 @return 
	 @remark 在单线程渲染中，负责渲染器的帧初始化，渲染API调用等
	 \n在多线程渲染中，负责渲染线程的启动调度
	*/
	virtual bool RT_StartRender() =0;
	/**
	 @brief 渲染器一帧的结束函数
	 @return 
	 @remark 在单线程渲染中，负责渲染器的present，帧结束工作
	 \n在多线程渲染中，负责等待渲染线程的返回，同步渲染线程和更新线程。
	*/
	virtual bool RT_EndRender() =0;
	/**
	 @brief 跳过一帧的渲染
	 @return 
	 @param int framecount
	 @remark 
	*/
	virtual void RT_SkipOneFrame(int framecount = 1) =0;

	//////////////////////////////////////////////////////////////////////////







	//////////////////////////////////////////////////////////////////////////
	// FX Interfaces : need deprcated

	virtual void FX_ColorGradingTo( gkTexturePtr& pCch, float fAmount ) =0;

	//////////////////////////////////////////////////////////////////////////







	//////////////////////////////////////////////////////////////////////////
	// Render
	/**
	 @brief 设置主光的方向
	 @return 
	 @param const Vec3 & lightdir
	 @remark 
	*/
	virtual void RC_SetSunDir(const Vec3& lightdir)=0;

	//////////////////////////////////////////////////////////////////////////


	/**
	 @brief 获取渲染窗口的高
	 @return 
	 @remark 
	*/
	virtual uint32 GetScreenHeight(bool forceOrigin = false) =0;
	/**
	 @brief 获取渲染窗口的宽
	 @return 
	 @remark 
	*/
	virtual uint32 GetScreenWidth(bool forceOrigin = false) =0;
	/**
	 @brief 取得渲染窗口的句柄
	 @return 
	 @remark 
	*/
	virtual HWND GetWindowHwnd() =0;
	/**
	 @brief 取得窗口的偏移值
	 @return 
	 @remark 
	*/
	virtual Vec2i GetWindowOffset() =0;

	/**
	 @brief 设置窗口的present属性（编辑器用）
	 @return 
	 @param HWND hWnd
	 @param uint32 posx
	 @param uint32 posy
	 @param uint32 width
	 @param uint32 height
	 @remark 
	*/
	virtual void SetCurrContent(HWND hWnd, uint32 posx, uint32 posy, uint32 width, uint32 height, bool fullscreen = false) =0;

	/**
	 @brief 取得aux帮助物体渲染器
	 @return 
	 @remark 
	*/
	virtual IAuxRenderer* getAuxRenderer() =0;

	/**
	 @brief 获知当前渲染器的API类型
	 @return 
	 @remark 
	*/
	virtual ERendererAPI GetRendererAPI() =0;

	/**
	 @brief 通过屏幕坐标获取世界空间的射线
	 @return 
	 @param uint32 nX
	 @param uint32 nY
	 @remark 
	*/
	virtual Ray GetRayFromScreen( uint32 nX, uint32 nY ) =0;
	/**
	 @brief 通过世界空间一个点投影到屏幕坐标
	 @return 
	 @param const Vec3 & worldpos
	 @remark 
	*/
	virtual Vec3 ProjectScreenPos( const Vec3& worldpos ) =0;


	virtual void SetOverrideSize(int width, int height, bool set) {}
	virtual void GetOverrideSize(int& width, int& height) {}

	virtual void SetPixelReSize(float scale) {}
	virtual float GetPixelReSize() {return 1.0f;}

    virtual Vec3 ScreenPosToViewportPos(Vec3 screenPos) =0;

	virtual bool FullScreenMode() {return true;}

	virtual void SavePositionCubeMap(Vec3 position, const TCHAR* texturename) {}
};

#endif