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
// Name:   	gmpMaxViewport.h
// Desc:		
// 	
// Author:  Kaiming
// Date:	2012/8/19
// Modify:	
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _gmpMaxViewport_h_
#define _gmpMaxViewport_h_

#include "maxapi.h"

class NullView1 : public View
{
public:
	Point2 ViewToScreen(Point3 p)
	{ return Point2(p.x,p.y); }
	NullView1() {
		worldToView.IdentityMatrix();
		screenW=640.0f; screenH = 480.0f;
	}
};

class gkPortWindow : public ViewWindow, public ViewportDisplayCallback
{
public:
	//返回的名字，就是在Views/Extended中看到的名字
	MCHAR *GetName(){return "gkMaxPort";}

	//在这里创建你自己的渲染窗口，一般使用WS_CHILD创建，如果你希望包含自己的菜单，可以使用对话框模版实现，这是惟一可以包含菜单又可以作为子窗口的方式
	//然后响应父窗口的WM_SIZE和WM_MOVE来调整它；另外，如果使用OpenGL，请注意使用独立线程；防止3dsmax 使用opengl窗口时发生混乱； 
	HWND CreateViewWindow(HWND hParent, int x, int y, int w, int h);
	//切换视图时会导致Destroy 发生，传入的hWnd就是你自己创建的那个窗口的句柄
	void DestroyViewWindow(HWND hWnd);
	HWND m_hWnd;

// override VDC
public:
	virtual void	Display (TimeValue t, ViewExp *vpt, int flags);
	virtual void	GetViewportRect (TimeValue t, ViewExp *vpt, Rect *rect);
	virtual BOOL	Foreground () {return TRUE;}
};




#endif