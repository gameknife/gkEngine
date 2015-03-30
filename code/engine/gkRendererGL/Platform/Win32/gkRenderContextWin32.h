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
// Name:   	gkRenderContextWin32.h
// Desc:		
// 	
// Author:  Kaiming
// Date:	2012/6/2
// Modify:	
// 
//////////////////////////////////////////////////////////////////////////
#ifndef _gkRenderContextWin32_h_
#define _gkRenderContextWin32_h_

#include "Prerequisites.h"
#include "DeviceRenderContext.h"


/* Entry points */
typedef int (APIENTRY * PFNWGLSWAPINTERVALEXTPROC) (int);

/* Entry points */
typedef const char *(APIENTRY * PFNWGLGETEXTENSIONSSTRINGARBPROC)( HDC );

/* Entry points */
typedef const char *(APIENTRY * PFNWGLGETEXTENSIONSSTRINGEXTPROC)( void );

/* Entry points */
typedef BOOL (WINAPI * PFNWGLGETPIXELFORMATATTRIBIVARBPROC) (HDC, int, int, UINT, const int *, int *);

/* Entry points */
typedef HGLRC (WINAPI * PFNWGLCREATECONTEXTATTRIBSARBPROC) (HDC, HGLRC, const int *);
typedef const GLubyte * (APIENTRY *PFNGLGETSTRINGIPROC) (GLenum, GLuint);

#define WGL_CONTEXT_MAJOR_VERSION_ARB          0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB          0x2092
#define WGL_CONTEXT_LAYER_PLANE_ARB            0x2093
#define WGL_CONTEXT_FLAGS_ARB                  0x2094
#define WGL_CONTEXT_PROFILE_MASK_ARB           0x9126

/* Bits for WGL_CONTEXT_FLAGS_ARB */
#define WGL_CONTEXT_DEBUG_BIT_ARB              0x0001
#define WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB 0x0002

/* Bits for WGL_CONTEXT_PROFILE_MASK_ARB */
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB          0x00000001
#define WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB 0x00000002

class gkDeviceRenderContext : public IDeviceRenderContext
{
public:
	gkDeviceRenderContext();
	virtual ~gkDeviceRenderContext();

	virtual HWND initDevice(ISystemInitInfo& sii);
	virtual bool destroyDevice();

	virtual void swapBuffer();

	virtual void makeThreadContext(bool close) {}

	virtual void resizeBackBuffer(int width, int height);
	
private:
	bool SetWindow(int width, int height, bool fullscreen, HWND hWnd);
// 	EGLSurface createSurfaceFromWindow(::EGLDisplay display, NativeWindowType win);
// 	EGLConfig SelectEGLConfiguration();
// 	// win32 stuff
	HWND								m_hWnd;
	DWORD							m_dwWindowStyle;
	HINSTANCE							m_hInst;

	HGLRC							m_mainContext;
	HDC								m_dc;
	// egl stuff
// 	NativeWindowType m_NativeWindow;
// 	NativeDisplayType m_NativeDisplay;
// 
// 	// egl backbuffer surface
// 	EGLDisplay m_eglDisplay;
// 	EGLSurface m_eglSurface;
// 	EGLContext m_eglContext;
// 	EGLContext m_eglContextLoading[8];
// 
// 	// egl config
// 	EGLConfig		m_eglConfig;
// 	EGLint		m_iConfig;
// 
// 	ThreadContexts m_eglContexts;

	PFNWGLSWAPINTERVALEXTPROC SwapIntervalEXT;
};


#endif


