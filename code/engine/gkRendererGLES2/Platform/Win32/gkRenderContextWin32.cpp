#include "StableHeader.h"
#include "gkRenderContextWin32.h"
#include "gkRendererGLES2.h"


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

		break;
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

	// egl stuff
	m_NativeWindow = 0;
	m_NativeDisplay = 0;

	// egl backbuffer surface
	m_eglDisplay = 0;
	m_eglSurface = 0;
	m_eglContext = 0;

	// egl config
	m_eglConfig = 0;
	m_iConfig = 0;
}

HWND gkDeviceRenderContext::initDevice(ISystemInitInfo& sii)
{
	// create a window
	SetWindow( sii.fWidth, sii.fHeight, false, 0 );
	getRenderer()->SetCurrContent(0,0,0,sii.fWidth, sii.fHeight);
	//////////////////////////////////////////////////////////////////////////
	// bind hWnd and hDc
	m_NativeWindow = m_hWnd;
	m_NativeDisplay = GetDC(m_hWnd);
	m_eglDisplay = eglGetDisplay(m_NativeDisplay);

	if (m_eglDisplay == EGL_NO_DISPLAY)
	{
		m_eglDisplay = eglGetDisplay( EGL_DEFAULT_DISPLAY );
	}

	//////////////////////////////////////////////////////////////////////////
	// egl system initialize
	if( !eglInitialize(m_eglDisplay, NULL, NULL) )
	{
		return 0;
	}

	if( !eglBindAPI(EGL_OPENGL_ES_API) )
	{
		return 0;
	}


	//////////////////////////////////////////////////////////////////////////
	// egl config
	m_eglConfig = SelectEGLConfiguration();
	eglGetConfigAttrib(m_eglDisplay, m_eglConfig, EGL_CONFIG_ID, &m_iConfig);

	//////////////////////////////////////////////////////////////////////////
	// egl windowsurface creation
	m_eglSurface = eglCreateWindowSurface(m_eglDisplay, m_eglConfig, m_NativeWindow, NULL);
	if(m_eglSurface == EGL_NO_SURFACE)
	{
		m_eglSurface = eglCreateWindowSurface(m_eglDisplay, m_eglConfig, NULL, NULL);
	}

	//////////////////////////////////////////////////////////////////////////
	// egl context creation
	EGLint ai32ContextAttribs[] = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE};
	m_eglContext = eglCreateContext(m_eglDisplay, m_eglConfig, NULL, ai32ContextAttribs);
	for (uint32 i=0; i < 8; ++i)
	{
		m_eglContextLoading[i] = eglCreateContext(m_eglDisplay, m_eglConfig, m_eglContext, ai32ContextAttribs);
		m_context_ptr = 0;
	}

	
	//////////////////////////////////////////////////////////////////////////
	// bind display, surface, context
	if (!eglMakeCurrent(m_eglDisplay, m_eglSurface, m_eglSurface, m_eglContext))
	{
		return 0;
	}

	// set interval
	eglSwapInterval(m_eglDisplay, 0);

	// clear color & depth
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glClearDepthf(1.0f);

	return m_hWnd;
}

bool gkDeviceRenderContext::destroyDevice()
{
	eglMakeCurrent(m_eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
	eglTerminate(m_eglDisplay);

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
		//GetDXUTState().SetHInstance( hInstance );

		TCHAR szExePath[MAX_PATH];
		GetModuleFileName( NULL, szExePath, MAX_PATH );

		HICON hIcon = NULL;

		if( hIcon == NULL ) // If the icon is NULL, then use the first one found in the exe
			hIcon = ExtractIcon( hInstance, szExePath, 0 );

		// Register the windows class
		WNDCLASS wndClass;
		wndClass.style = CS_DBLCLKS;
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


EGLConfig gkDeviceRenderContext::SelectEGLConfiguration()
{
	EGLint		num_config;
	EGLint		conflist[32];
	EGLConfig	conf = (EGLConfig) 0;
	int			i = 0;

	// Select default configuration
#if defined(ANDROID)
	if(pData->nColorBPP == 32)
	{
		conflist[i++] = EGL_RED_SIZE;
		conflist[i++] = 8;
		conflist[i++] = EGL_GREEN_SIZE;
		conflist[i++] = 8;
		conflist[i++] = EGL_BLUE_SIZE;
		conflist[i++] = 8;
		conflist[i++] = EGL_ALPHA_SIZE;
		conflist[i++] = 8;
	}
	else
	{
		conflist[i++] = EGL_RED_SIZE;
		conflist[i++] = 5;
		conflist[i++] = EGL_GREEN_SIZE;
		conflist[i++] = 6;
		conflist[i++] = EGL_BLUE_SIZE;
		conflist[i++] = 5;
		conflist[i++] = EGL_ALPHA_SIZE;
		conflist[i++] = 0;
	}
#else
	conflist[i++] = EGL_BUFFER_SIZE;
	conflist[i++] = 0;
#endif

	//if(pData->bNeedZbuffer || pData->nDepthBPP > 0)
	// depth buffer 16bit, or 24bit?
	{
		conflist[i++] = EGL_DEPTH_SIZE;
		conflist[i++] = 16;
	}

	//if(pData->bNeedStencilBuffer)
	if (0)
	{
		conflist[i++] = EGL_STENCIL_SIZE;
		conflist[i++] = 8;
	}

	conflist[i++] = EGL_SURFACE_TYPE;
	conflist[i] = EGL_WINDOW_BIT;

	//if(pData->bNeedPbuffer)
	// 	{
	// 		conflist[i] |= EGL_PBUFFER_BIT;
	// 	}
	// 
	// 	//if(pData->bNeedPixmap)
	// 	{
	// 		conflist[i] |= EGL_PIXMAP_BIT;
	// 	}

	++i;

	// OGLES 2
	conflist[i++] = EGL_RENDERABLE_TYPE;
	conflist[i++] = EGL_OPENGL_ES2_BIT;

	// FSAA
	// Append number of number buffers depending on FSAA mode selected
	switch(0)
	{
	case 1:
		conflist[i++] = EGL_SAMPLE_BUFFERS;
		conflist[i++] = 1;
		conflist[i++] = EGL_SAMPLES;
		conflist[i++] = 2;
		break;

	case 2:
		conflist[i++] = EGL_SAMPLE_BUFFERS;
		conflist[i++] = 1;
		conflist[i++] = EGL_SAMPLES;
		conflist[i++] = 4;
		break;

	default:
		conflist[i++] = EGL_SAMPLE_BUFFERS;
		conflist[i++] = 0;
	}

	{
		// Terminate the list with EGL_NONE
		conflist[i++] = EGL_NONE;

		// Return null config if config is not found
		if(!eglChooseConfig(m_eglDisplay, conflist, &conf, 1, &num_config) || num_config != 1)
		{
			return 0;
		}
	}

	// Return config index
	return conf;
}

EGLSurface gkDeviceRenderContext::createSurfaceFromWindow(::EGLDisplay display,	NativeWindowType win)
{
	EGLSurface surface;

	surface = eglCreateWindowSurface(display, m_eglConfig, win, NULL);

	if (surface == EGL_NO_SURFACE)
	{
		return (EGLSurface)0;
	}
	return surface;

}

void gkDeviceRenderContext::swapBuffer()
{
	eglSwapBuffers(m_eglDisplay, m_eglSurface);
}

gkDeviceRenderContext::~gkDeviceRenderContext()
{

}

void gkDeviceRenderContext::makeThreadContext(bool close)
{
	if ( gEnv->pSystemInfo->mainThreadId == gkGetCurrentThread() )
	{
		gkLogWarning( _T("loading from main thread.") );
	}
	else
	{
		if (close)
		{
			eglMakeCurrent( m_eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, m_eglContext );
		}
		else
		{
			//eglMakeCurrent( m_eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, m_eglContextLoading );
			

			ThreadContexts::iterator it = m_eglContexts.find( gkGetCurrentThread() );
			if (it != m_eglContexts.end())
			{
				eglMakeCurrent( m_eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, it->second );
			}
			else
			{
				EGLContext newContent = m_eglContextLoading[m_context_ptr++];

				eglMakeCurrent( m_eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, newContent );

				m_eglContexts.insert( ThreadContexts::value_type( gkGetCurrentThread(), newContent ) );
			}
		}

		
	}
	
}

void gkDeviceRenderContext::resizeBackBuffer(int width, int height)
{
	eglDestroySurface( m_eglDisplay, m_eglSurface );

	m_eglSurface = eglCreateWindowSurface(m_eglDisplay, m_eglConfig, m_NativeWindow, NULL);
	if(m_eglSurface == EGL_NO_SURFACE)
	{
		m_eglSurface = eglCreateWindowSurface(m_eglDisplay, m_eglConfig, NULL, NULL);
	}
}
