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
// Name:   	IRenderTarget.h
// Desc:	RT的接口	
// 
// Author:  Kaiming-Desktop
// Date:	2011 /8/9
// Modify:	2011 /8/9
// 
//////////////////////////////////////////////////////////////////////////

#ifndef IRENDERTARGET_H_
#define IRENDERTARGET_H_

class IRenderTarget {
public:
	enum GK_RTT_TYPE
	{
		GK_RTT_TYPE_SM_SMALL,			// SHADOWMAP小型
		GK_RTT_TYPE_SM_NORMAL,			// SHADOWMAP中型
		GK_RTT_TYPE_SM_LARGE,			// SHADOWMAP大型
		GK_RTT_TYPE_CM_SMALL,			// COLORMAP小型
		GK_RTT_TYPE_CM_NORMAL,			// COLORMAP中型
		GK_RTT_TYPE_CM_LARGE,			// COLORMAP大型
		GK_RTT_TYPE_CM_CUSTOM,
	};
public:
	virtual ~IRenderTarget() {}
	//////////////////////////////////////
	// Name:  gkRenderTarget::setCamera
	// Desc:	设置RT使用的摄像机
	// Returns:   void
	// Parameter: gkCamera * cam
	//////////////////////////////////////
	virtual void setCamera(ICamera* cam) =0;

// 	// 重新绑定Surface
// 	virtual void _setRenderSurface(LPDIRECT3DSURFACE9* ppSurf0, LPDIRECT3DSURFACE9* ppSurf1 = NULL, LPDIRECT3DSURFACE9* ppSurf2 = NULL, LPDIRECT3DSURFACE9* ppSurf3 = NULL) =0;
// 	// 取得Surface的引用，因为此Surface是存在其他位置的
// 	// param: 取用第几个RT，默认0
// 	LPDIRECT3DSURFACE9* _getRenderSurface(BYTE rtIndex = 0) =0;
};

#endif