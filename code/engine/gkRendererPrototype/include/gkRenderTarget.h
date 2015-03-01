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
// Name:   	gkRenderTarget.h
// Desc:	RenderTarget, 描述一个渲染窗口，普通游戏就只需一个rendertarget即可
//			RenderTarget内包含一套完整的渲染流程，渲染到一个D3DSURFACE上
// 描述:	
// 
// Author:  Kaiming-Laptop
// Date:	2010/9/10
// 
//////////////////////////////////////////////////////////////////////////

#ifndef gkRenderTarget_h_1196d89a_eb15_4e40_85eb_57ac93f486a9
#define gkRenderTarget_h_1196d89a_eb15_4e40_85eb_57ac93f486a9

#include "Prerequisites.h"
#include "IRenderTarget.h"

class gkRenderTarget : public IRenderTarget
{
protected:
	gkStdString		m_wstrName;				// RT名字，方便索引
	ICamera*			m_pCamera;				// 当前RT渲染视野


// 	// D3D相关结构
// 	LPDIRECT3DDEVICE9	m_pD3DDevice9;			// 设备指针，方便以后调用
// 
// 	// 渲染表面, 目前显卡一般具备4个SimultaneousRT, 这里设置的四个表面供渲染
// 	// 一般来说只用RT0
// 	LPDIRECT3DSURFACE9*	m_ppD3DSurf9_RT0;		
// 	LPDIRECT3DSURFACE9*	m_ppD3DSurf9_RT1;		
// 	LPDIRECT3DSURFACE9*	m_ppD3DSurf9_RT2;		
// 	LPDIRECT3DSURFACE9*	m_ppD3DSurf9_RT3;		
												
	// 一些辅助参数
	bool				m_bActive;				// 当前RT是否激活
	bool				m_bAutoUpdate;			// 当前RT是否每一帧自动更新
	
	float				m_fUpdateInterval;		// 如果非自动更新，设置更新间隔

	bool				m_bIsCreated;

	//////////////////////////////////////
	// Name:  gkRenderTarget::updateImpl
	// Desc:  实际的更新函数，便于派生类调用
	// Returns:   void
	//////////////////////////////////////
	virtual void updateImpl();
public:
	gkRenderTarget();
	gkRenderTarget(gkStdString name);
	virtual ~gkRenderTarget();
	//////////////////////////////////////
	// Name:  gkRenderTarget::setCamera
	// Desc:	设置RT使用的摄像机
	// Returns:   void
	// Parameter: gkCamera * cam
	//////////////////////////////////////
	virtual void setCamera(ICamera* cam);

	virtual void _beginUpdate();
	virtual void _endUpdate();
	//////////////////////////////////////
	// Name:  gkRenderTarget::_update
	// Desc:	更新RT，其实就是每一个RT的渲染入口！
	// Returns:   void
	//////////////////////////////////////
	virtual void _update();

	// D3D的设备相关函数
	// RT涉及到D3D底层SURFACE的使用，因此需要在设备丢失时作处理
// 	virtual void onCreate(LPDIRECT3DDEVICE9 pDevice);
// 	virtual void onReset();
// 	virtual void onLost();
// 	virtual void onDestroy();

	// 重新绑定Surface
//	virtual void _setRenderSurface(LPDIRECT3DSURFACE9* ppSurf0, LPDIRECT3DSURFACE9* ppSurf1 = NULL, LPDIRECT3DSURFACE9* ppSurf2 = NULL, LPDIRECT3DSURFACE9* ppSurf3 = NULL);
	// 取得Surface的引用，因为此Surface是存在其他位置的
	// param: 取用第几个RT，默认0
//	LPDIRECT3DSURFACE9* _getRenderSurface(BYTE rtIndex = 0);
};

#endif // gkRenderTarget_h_1196d89a_eb15_4e40_85eb_57ac93f486a9
