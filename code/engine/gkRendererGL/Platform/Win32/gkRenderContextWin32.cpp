#include "StableHeader.h"
#include "gkRenderContextWin32.h"
#include "gkRendererGL330.h"


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		{
			CREATESTRUCT	*pCreate = (CREATESTRUCT*)lParam;
			SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pCreate->lpCreateParams);
			break;
		}
	case WM_CLOSE:
		{
			HMENU hMenu;
			hMenu = GetMenu( hWnd );
			if( hMenu != NULL )
				DestroyMenu( hMenu );
			DestroyWindow( hWnd );
			UnregisterClass( _T("EglWindowClass"), NULL );
			return 0;
		}

	case WM_DESTROY:
		PostQuitMessage( 0 );
		break;
	case WM_SIZE:
		{
			RECT rc;
			GetClientRect( hWnd, &rc );
			gEnv->pRenderer->SetCurrContent( hWnd, 0, 0, rc.right - rc.left, rc.bottom - rc.top );
		}
	default:
		break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}


gkDeviceRenderContext::gkDeviceRenderContext()
{
	m_hWnd = 0;
	m_dwWindowStyle = 0;
	m_hInst = 0;
}

HWND gkDeviceRenderContext::initDevice(ISystemInitInfo& sii)
{
	//////////////////////////////////////////////////////////////////////////
	// create a window
	SetWindow( sii.fWidth, sii.fHeight, false, 0 );
	getRenderer()->SetCurrContent(0,0,0,sii.fWidth, sii.fHeight);

	//////////////////////////////////////////////////////////////////////////
	// create a tmp context
	m_dc = GetDC( m_hWnd );
	PIXELFORMATDESCRIPTOR pfd=
	{
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW|PFD_SUPPORT_OPENGL|
		PFD_DOUBLEBUFFER|PFD_GENERIC_ACCELERATED|PFD_GENERIC_FORMAT,
		PFD_TYPE_RGBA,
		24,
		0,0,0,0,0,0,
		0,
		0,
		0,
		0,0,0,0,
		0,
		0,
		0,
		PFD_MAIN_PLANE,
		0,
		0,0,0
	};

	int pixelFormat = ChoosePixelFormat( m_dc, &pfd );

	if (pixelFormat == 0)
	{
		pixelFormat == 1;
	}

	DescribePixelFormat(m_dc, pixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd );
	SetPixelFormat(m_dc, pixelFormat, &pfd);

	m_mainContext = wglCreateContext( m_dc );
	if (!m_mainContext)
	{
		gkLogError( _T("Win32 Create GL Context Failed.") );
	}
	wglMakeCurrent( m_dc, m_mainContext );

	//////////////////////////////////////////////////////////////////////////
	// get eglextension
	PFNWGLGETEXTENSIONSSTRINGEXTPROC GetExtensionsStringEXT = (PFNWGLGETEXTENSIONSSTRINGEXTPROC)
		wglGetProcAddress( "wglGetExtensionsStringEXT" );

	PFNWGLGETEXTENSIONSSTRINGARBPROC GetExtensionsStringARB = (PFNWGLGETEXTENSIONSSTRINGARBPROC)
		wglGetProcAddress( "wglGetExtensionsStringARB" );

	PFNWGLCREATECONTEXTATTRIBSARBPROC CreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)
			wglGetProcAddress( "wglCreateContextAttribsARB" );

	SwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)
			wglGetProcAddress( "wglSwapIntervalEXT" );

	PFNWGLGETPIXELFORMATATTRIBIVARBPROC GetPixelFormatAttribivARB = (PFNWGLGETPIXELFORMATATTRIBIVARBPROC)
			wglGetProcAddress( "wglGetPixelFormatAttribivARB" );

	if (!CreateContextAttribsARB || !GetExtensionsStringEXT || !GetExtensionsStringARB || !SwapIntervalEXT || !GetPixelFormatAttribivARB)
	{
		gkLogError(_T("Can not Create GL330 device, driver may not support."));
		return NULL;
	}

	// delete tmp context

	wglMakeCurrent( NULL, NULL );
	wglDeleteContext( m_mainContext );
	//////////////////////////////////////////////////////////////////////////
	
	
	
	//////////////////////////////////////////////////////////////////////////
	// config

	 int flags, i = 0, attribs[40];

	 // choose api version
	 attribs[i++] = WGL_CONTEXT_MAJOR_VERSION_ARB;
	 attribs[i++] = 3;
	 attribs[i++] = WGL_CONTEXT_MINOR_VERSION_ARB;
	 attribs[i++] = 2;

	 flags = 0;
	 flags |= WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB;
	 //flags |= WGL_CONTEXT_DEBUG_BIT_ARB;

	 attribs[i++] = WGL_CONTEXT_FLAGS_ARB;
	 attribs[i++] = flags;

	 // core profile or compatibility
	 flags = WGL_CONTEXT_CORE_PROFILE_BIT_ARB;
	 //flags = WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB;

	 attribs[i++] = WGL_CONTEXT_PROFILE_MASK_ARB;
	 attribs[i++] = flags;

	 attribs[i++] = 0;

	 m_dc = GetDC( m_hWnd );
	 m_mainContext = CreateContextAttribsARB( m_dc, NULL, attribs );

	if (!m_mainContext)
	{
		gkLogError( _T("Win32 Create GL Context Failed.") );
	}

	wglMakeCurrent( m_dc, m_mainContext );

	SwapIntervalEXT( 0 );
	SwapBuffers( m_dc );

	return m_hWnd;
}

bool gkDeviceRenderContext::destroyDevice()
{

	wglMakeCurrent( NULL, NULL);
	wglDeleteContext( m_mainContext );

	if (m_hWnd) DestroyWindow(m_hWnd);
	return true;
}


bool gkDeviceRenderContext::SetWindow(int width, int height, bool fullscreen, HWND hWnd)
{
	if (!hWnd)
	{
		HINSTANCE hInstance = NULL;
		if( hInstance == NULL )
			hInstance = ( HINSTANCE )GetModuleHandle( NULL );

		HICON hIcon = NULL;

		// Register the windows class
		WNDCLASS wndClass;
		wndClass.style = CS_DBLCLKS; // CS_HREDRAW | CS_VREDRAW | CS_OWNDC;//
		wndClass.lpfnWndProc = (WNDPROC)WndProc;
		wndClass.cbClsExtra = 0;
		wndClass.cbWndExtra = 0;
		wndClass.hInstance = hInstance;
		wndClass.hIcon = hIcon;
		wndClass.hCursor = LoadCursor( NULL, IDC_ARROW );
		wndClass.hbrBackground = ( HBRUSH )GetStockObject( BLACK_BRUSH );
		wndClass.lpszMenuName = NULL;
		wndClass.lpszClassName = _T("EglWindowClass");

		if (!RegisterClass(&wndClass))
		{
			DWORD dwError = GetLastError();
			if( dwError != ERROR_CLASS_ALREADY_EXISTS )
				OutputDebugString( _T("Cannot Register Window Class.") );
			return false;
		}

		// Create the render window
		m_hWnd = CreateWindow( _T("EglWindowClass"), _T("gkENGINE"), WS_OVERLAPPEDWINDOW,
			20, 20, width, height, 0,
			NULL, hInstance, 0 );
	}
	else
		m_hWnd = (HWND)hWnd;

	RECT realRect;
	GetClientRect(m_hWnd, &realRect);

	int realwidth = realRect.right - realRect.left;
	int realheight = realRect.bottom - realRect.top;
	width = width * 2 - realwidth;
	height = height * 2 - realheight;

	MoveWindow(m_hWnd, GetSystemMetrics(SM_CXSCREEN) / 2 - width / 2, GetSystemMetrics(SM_CYSCREEN) / 2 - height / 2, width, height, FALSE);

	ShowWindow(m_hWnd, SW_SHOWNORMAL);
	UpdateWindow(m_hWnd);
	SetFocus(m_hWnd);
	SetForegroundWindow(m_hWnd);

	return true;
}

void gkDeviceRenderContext::swapBuffer()
{
	SwapBuffers( m_dc );
}

gkDeviceRenderContext::~gkDeviceRenderContext()
{
	//////////////////////////////////////////////////////////////////////////
	// get eglextension
	PFNWGLGETEXTENSIONSSTRINGEXTPROC GetExtensionsStringEXT = (PFNWGLGETEXTENSIONSSTRINGEXTPROC)
		wglGetProcAddress( "wglGetExtensionsStringEXT" );

	PFNWGLGETEXTENSIONSSTRINGARBPROC GetExtensionsStringARB = (PFNWGLGETEXTENSIONSSTRINGARBPROC)
		wglGetProcAddress( "wglGetExtensionsStringARB" );

	PFNWGLCREATECONTEXTATTRIBSARBPROC CreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)
		wglGetProcAddress( "wglCreateContextAttribsARB" );

	SwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)
		wglGetProcAddress( "wglSwapIntervalEXT" );

	PFNWGLGETPIXELFORMATATTRIBIVARBPROC GetPixelFormatAttribivARB = (PFNWGLGETPIXELFORMATATTRIBIVARBPROC)
		wglGetProcAddress( "wglGetPixelFormatAttribivARB" );

	// delete tmp context

	wglMakeCurrent( NULL, NULL );
	wglDeleteContext( m_mainContext );

	return;

	int flags, i = 0, attribs[40];

	// choose api version
	attribs[i++] = WGL_CONTEXT_MAJOR_VERSION_ARB;
	attribs[i++] = 3;
	attribs[i++] = WGL_CONTEXT_MINOR_VERSION_ARB;
	attribs[i++] = 2;

	flags = 0;
	flags |= WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB;
	//flags |= WGL_CONTEXT_DEBUG_BIT_ARB;

	attribs[i++] = WGL_CONTEXT_FLAGS_ARB;
	attribs[i++] = flags;

	// core profile or compatibility
	flags = WGL_CONTEXT_CORE_PROFILE_BIT_ARB;
	//flags = WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB;

	attribs[i++] = WGL_CONTEXT_PROFILE_MASK_ARB;
	attribs[i++] = flags;

	attribs[i++] = 0;

	m_dc = GetDC( m_hWnd );
	m_mainContext = CreateContextAttribsARB( m_dc, NULL, attribs );

	if (!m_mainContext)
	{
		gkLogError( _T("Win32 Create GL Context Failed.") );
	}

	wglMakeCurrent( m_dc, m_mainContext );

	SwapIntervalEXT( 0 );
	SwapBuffers( m_dc );
}

void gkDeviceRenderContext::resizeBackBuffer(int width, int height)
{
// 	eglDestroySurface( m_eglDisplay, m_eglSurface );
// 
// 	m_eglSurface = eglCreateWindowSurface(m_eglDisplay, m_eglConfig, m_NativeWindow, NULL);
// 	if(m_eglSurface == EGL_NO_SURFACE)
// 	{
// 		m_eglSurface = eglCreateWindowSurface(m_eglDisplay, m_eglConfig, NULL, NULL);
// 	}

	
}