#include "StableHeader.h"

#ifdef OS_ANDROID
#include "gkRenderContextAndroid.h"
#include "gkRendererGLES2.h"

bool gkDeviceRenderContext::destroyDevice()
{
	return true;
}

HWND gkDeviceRenderContext::initDevice(ISystemInitInfo& sii)
{
	//////////////////////////////////////////////////////////////////////////
	// native create ogles2 device

	int					bDone;
	m_NDT = 0;//(EGLNativeDisplayType)OsGetNativeDisplayType();
	m_NPT = 0;//(EGLNativePixmapType) OsGetNativePixmapType();
	m_NWT = (EGLNativeWindowType)(sii.nativeWindowPTR);//(EGLNativeWindowType) OsGetNativeWindowType();

	m_EGLContext = 0;
	do
	{
		bDone = true;

		m_EGLDisplay = eglGetDisplay(m_NDT);

		if(m_EGLDisplay == EGL_NO_DISPLAY)
		{
#if defined(BUILD_OGLES2) || defined(BUILD_OVG) || defined(BUILD_OGLES3)
			m_EGLDisplay = eglGetDisplay((EGLNativeDisplayType)EGL_DEFAULT_DISPLAY);
#else
			m_EGLDisplay = eglGetDisplay((NativeDisplayType)EGL_DEFAULT_DISPLAY);
#endif
		}

		if(!eglInitialize(m_EGLDisplay, &m_MajorVersion, &m_MinorVersion))
		{
			gkLogError(_T("RendererGLES2::eglInitialize failed."));
			return 0;
		}
		gkLogMessage(_T("RendererGLES2::eglInitialize success."));
	

		// Check Extension avaliablility after EGL initialization
		if (m_MajorVersion > 1 || (m_MajorVersion == 1 && m_MinorVersion >= 1))
		{
			m_bPowerManagementSupported = true;
		}
		else
		{
			m_bPowerManagementSupported = false;
		}

		do
		{
			{
//#if defined EGL_VERSION_1_3 && defined GL_ES_VERSION_2_0
				if(!eglBindAPI(EGL_OPENGL_ES_API))
				{
					gkLogError(_T("RendererGLES2::Failed to bind OpenGL ES API\n"));
					return 0;
				}
//#endif
			}
			gkLogMessage(_T("RendererGLES2::eglBindAPI success."));

			// Find an EGL config
			m_EGLConfig = SelectEGLConfiguration();
			eglGetConfigAttrib(m_EGLDisplay, m_EGLConfig, EGL_CONFIG_ID, &m_iConfig);

			// Destroy the context if we already created one
			if (m_EGLContext)
			{
				eglDestroyContext(m_EGLDisplay, m_EGLContext);
			}

			// Attempt to create a context
			EGLint ai32ContextAttribs[32];
			int	i = 0;

//#if defined(EGL_VERSION_1_3) && defined(GL_ES_VERSION_2_0)
			ai32ContextAttribs[i++] = EGL_CONTEXT_CLIENT_VERSION;
			ai32ContextAttribs[i++] = 2;
//#endif

// #if defined(BUILD_OGLES2) || defined(BUILD_OGLES) || defined(BUILD_OGLES3)
// 			if(PVRShellIsExtensionSupported(m_EGLDisplay,"EGL_IMG_context_priority"))
// 			{
// 				ai32ContextAttribs[i++] = EGL_CONTEXT_PRIORITY_LEVEL_IMG;
// 				switch(m_pShell->PVRShellGet(prefPriority))
// 				{
// 					case 0: ai32ContextAttribs[i++] = EGL_CONTEXT_PRIORITY_LOW_IMG; break;
// 					case 1: ai32ContextAttribs[i++] = EGL_CONTEXT_PRIORITY_MEDIUM_IMG; break;
// 					default:ai32ContextAttribs[i++] = EGL_CONTEXT_PRIORITY_HIGH_IMG;
// 				}
// 			}
// #endif
			ai32ContextAttribs[i] = EGL_NONE;

			m_EGLContext = eglCreateContext(m_EGLDisplay, m_EGLConfig, NULL, ai32ContextAttribs);

			if(m_EGLContext == EGL_NO_CONTEXT)
			{
				if(m_iRequestedConfig > 0)
				{
					// We failed to create a context
					gkLogError(_T("RendererGLES2::eglCreateContext failed."));
					return 0;
				}
			}
			gkLogMessage(_T("RendererGLES2::eglCreateContext retry."));
		} while(m_EGLContext == EGL_NO_CONTEXT);

		EGLint		attrib_list[16];
		int	i = 0;

// #if defined(EGL_VERSION_1_2)
// 		if(m_pShell->m_pShellData->bNeedAlphaFormatPre) // The default is EGL_ALPHA_FORMAT_NONPRE
// 		{
// 			attrib_list[i++] = EGL_ALPHA_FORMAT;
// 			attrib_list[i++] = EGL_ALPHA_FORMAT_PRE;
// 		}
// #endif

		// Terminate the attribute list with EGL_NONE
		attrib_list[i] = EGL_NONE;

// 		if(m_pShell->m_pShellData->bNeedPixmap)
// 		{
// 			m_pShell->PVRShellOutputDebug("InitAPI() Using pixmaps, about to create egl surface\n");
// 			m_EGLWindow = eglCreatePixmapSurface(m_EGLDisplay, m_EGLConfig, m_NPT, attrib_list);
// 		}
// 		else
		{
//#if defined(ANDROID)
			EGLint visualID;
		    eglGetConfigAttrib(m_EGLDisplay, m_EGLConfig, EGL_NATIVE_VISUAL_ID, &visualID);
			gkLogMessage(_T("RendererGLES2::eglGetConfigAttrib success."));
		    // Change the format of our window to match our config
    		ANativeWindow_setBuffersGeometry(m_NWT, 0, 0, visualID);
			gkLogMessage(_T("RendererGLES2::ANativeWindow_setBuffersGeometry success."));
//#endif
			m_EGLWindow = eglCreateWindowSurface(m_EGLDisplay, m_EGLConfig, m_NWT, attrib_list);

            // If we have failed to create a surface then try using Null
			if(m_EGLWindow == EGL_NO_SURFACE)
			{
				m_EGLWindow = eglCreateWindowSurface(m_EGLDisplay, m_EGLConfig, NULL, attrib_list);
			}
		}

		if (m_EGLWindow == EGL_NO_SURFACE)
		{
			gkLogError(_T("RendererGLES2::eglCreateWindowSurface failed."));
			return false;
		}

		if (!eglMakeCurrent(m_EGLDisplay, m_EGLWindow, m_EGLWindow, m_EGLContext))
		{
#ifdef EGL_VERSION_1_3
			if((eglGetError() == EGL_CONTEXT_LOST))
#else
			if((eglGetError() == EGL_CONTEXT_LOST_IMG) && m_bPowerManagementSupported)
#endif
			{
				bDone = false;
			}
			else
			{
				gkLogError(_T("RendererGLES2::eglMakeCurrent failed."));
				return false;
			}
		}
	} while(!bDone);

	/*
	 	Get correct screen width and height and
		save them into
		m_pShell->m_pShellData->nShellDimX and
		m_pShell->m_pShellData->nShellDimY
	*/
	EGLint screenWidth;
	EGLint screenHeight;


	eglQuerySurface(m_EGLDisplay, m_EGLWindow,	EGL_WIDTH,  &screenWidth);
	eglQuerySurface(m_EGLDisplay, m_EGLWindow,	EGL_HEIGHT, &screenHeight);

	getRenderer()->SetCurrContent(0,0,0,screenWidth, screenHeight);

	glViewport(0,0,screenWidth, screenHeight);

	gkLogMessage(_T("RendererGLES2::CreateDevice Success! %d x %d"), screenWidth, screenHeight);

	eglSwapInterval(m_EGLDisplay, 0);

	sii.fWidth = screenWidth;
	sii.fHeight = screenHeight;

	/*
		Done - activate requested features
	*/
	//ApiActivatePreferences();

	//glViewport(0,0,sii.fWidth, sii.fHeight);
	return 0;
}

void gkDeviceRenderContext::swapBuffer()
{
	eglSwapBuffers(m_EGLDisplay, m_EGLWindow);
}


/*******************************************************************************
 * Function Name  : SelectEGLConfiguration
 * Inputs		  : pData
 * Returns        : EGLConfig
 * Description    : Find the config to use for EGL initialisation
 *******************************************************************************/
EGLConfig gkDeviceRenderContext::SelectEGLConfiguration()
{
    EGLint		num_config;
    EGLint		conflist[32];
	EGLConfig	conf = (EGLConfig) 0;
    int			i = 0;

	// Select default configuration
	if( 0 ) // 32bpp)
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

	// depth buffer, 16bit
	if(1)
	{
		conflist[i++] = EGL_DEPTH_SIZE;
		conflist[i++] = 24;
	}
	
	// stencil buffer, 8bit
	if(0)
	{
		conflist[i++] = EGL_STENCIL_SIZE;
		conflist[i++] = 8;
	}

	// 
	conflist[i++] = EGL_SURFACE_TYPE;
	conflist[i] = EGL_WINDOW_BIT;

	// pbuffer, noneed
	if(0)
	{
		conflist[i] |= EGL_PBUFFER_BIT;
	}

	// pixmap, noneed
	if(0)
	{
		conflist[i] |= EGL_PIXMAP_BIT;
	}

	++i;

	// ogles specific
	conflist[i++] = EGL_RENDERABLE_TYPE;
	conflist[i++] = EGL_OPENGL_ES2_BIT;

	
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

	// choosecofig
	{
		// Terminate the list with EGL_NONE
		conflist[i++] = EGL_NONE;

		// Return null config if config is not found
		if(!eglChooseConfig(m_EGLDisplay, conflist, &conf, 1, &num_config) || num_config != 1)
		{
			gkLogError(_T("RendererGLES2::eglChooseConfig failed."));
			return 0;
		}
	}

	// Return config index
	return conf;
}


/*******************************************************************************
 * Function Name  : StringFrom_eglGetError
 * Returns        : A string
 * Description    : Returns a string representation of an egl error
 *******************************************************************************/
const char *gkDeviceRenderContext::StringFrom_eglGetError() const
{
	EGLint nErr = eglGetError();

	switch(nErr)
	{
		case EGL_SUCCESS:
			return "EGL_SUCCESS";
		case EGL_BAD_DISPLAY:
			return "EGL_BAD_DISPLAY";
		case EGL_NOT_INITIALIZED:
			return "EGL_NOT_INITIALIZED";
		case EGL_BAD_ACCESS:
			return "EGL_BAD_ACCESS";
		case EGL_BAD_ALLOC:
			return "EGL_BAD_ALLOC";
		case EGL_BAD_ATTRIBUTE:
			return "EGL_BAD_ATTRIBUTE";
		case EGL_BAD_CONFIG:
			return "EGL_BAD_CONFIG";
		case EGL_BAD_CONTEXT:
			return "EGL_BAD_CONTEXT";
		case EGL_BAD_CURRENT_SURFACE:
			return "EGL_BAD_CURRENT_SURFACE";
		case EGL_BAD_MATCH:
			return "EGL_BAD_MATCH";
		case EGL_BAD_NATIVE_PIXMAP:
			return "EGL_BAD_NATIVE_PIXMAP";
		case EGL_BAD_NATIVE_WINDOW:
			return "EGL_BAD_NATIVE_WINDOW";
		case EGL_BAD_PARAMETER:
			return "EGL_BAD_PARAMETER";
		case EGL_BAD_SURFACE:
			return "EGL_BAD_SURFACE";
		default:
			return "unknown";
	}
}
#endif
