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
#ifndef _gkRenderContextAndroid_h_
#define _gkRenderContextAndroid_h_
#include "Prerequisites.h"
#include "DeviceRenderContext.h"

class gkDeviceRenderContext : public IDeviceRenderContext
{
public:
	gkDeviceRenderContext() {}
	virtual ~gkDeviceRenderContext() {}

	virtual HWND initDevice(ISystemInitInfo& sii);
	virtual bool destroyDevice();

	virtual void swapBuffer();
	virtual void makeThreadContext(bool close = false) {}
public:
	EGLDisplay	m_EGLDisplay;
	EGLSurface	m_EGLWindow;
	EGLContext	m_EGLContext;
	EGLConfig	m_EGLConfig;
	EGLint		m_MajorVersion, m_MinorVersion;
	bool		m_bPowerManagementSupported;
	EGLint		m_iRequestedConfig;
	EGLint		m_iConfig;

	EGLNativeDisplayType m_NDT;
	EGLNativePixmapType  m_NPT;
	EGLNativeWindowType  m_NWT;

public:
	EGLConfig SelectEGLConfiguration();
	const char *StringFrom_eglGetError() const;
};

#endif