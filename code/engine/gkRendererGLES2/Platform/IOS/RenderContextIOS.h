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
// Name:   	gkRenderContextAndroid.h
// Desc:		
// 	
// Author:  Kaiming
// Date:	2012/6/2
// Modify:	
// 
//////////////////////////////////////////////////////////////////////////
#ifndef _RenderContextIOS_h_
#define _RenderContextIOS_h_

#include "../../Platform.h"
#include "../../DeviceRenderContext.h"

//@class EAGLView;

class DeviceRenderContext : public IDeviceRenderContext
{
public:
	DeviceRenderContext() {}
	virtual ~DeviceRenderContext() {}
    
	virtual bool initDevice(HWND hWnd);
	virtual bool destroyDevice();
    
	virtual void swapBuffer();
    
	virtual UINT32 get_device_width() {return m_device_width;}
	virtual UINT32 get_device_height() {return m_device_height;}

public:
	
    UINT32								m_device_width;
	UINT32								m_device_height;
    
    // apple egl
    void*                               m_eglView;
};

#endif