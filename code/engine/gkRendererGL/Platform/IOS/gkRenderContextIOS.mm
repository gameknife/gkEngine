#include "StableHeader.h"
#include "gkRenderContextIOS.h"
#include "gkRendererGL330.h"

#import <UIKit/UIKit.h>
#import <OpenGLES/EAGL.h>
#import <OpenGLES/EAGLDrawable.h>

#import "EAGLView.h"
#import "TouchView.h"

bool gkDeviceRenderContext::destroyDevice()
{
	return true;
}

HWND gkDeviceRenderContext::initDevice(ISystemInitInfo& sii)
{
    float scale = 1.0;
    
    UIScreen* screen = [UIScreen mainScreen];
	if ([UIScreen instancesRespondToSelector:@selector(scale)])
	{
		scale = [screen scale];
	}
    
    //scale = 1.0;
    
    //[UIScreen ]
    
	CGRect	rect = [screen bounds];
    
    if( rect.size.width * rect.size.height > 700000 )
    {
        scale = 1.0;
    }
    
    gkTouchView* _touchView;
    _touchView = [[gkTouchView alloc] initWithFrame:rect];
    
    float offset = 0;
    
    
    //rect.size.width =
    
    if (false) {
        offset = rect.size.width;
        offset -= (rect.size.height * 9 / 16);
        offset *= 0.5f;
    }
    
    
    
    UIWindow* _window = NULL;
    //Create a full-screen window
	_window = [[UIWindow alloc] initWithFrame:rect];

	
    //[_window ]
    
	NSString *strColourFormat;
	int iDepthFormat, iStencilFormat;
    

	
    float tmp = rect.size.width;
    rect.size.width = rect.size.height;
    rect.size.height = tmp;
    
    EAGLView* _glView;
    
	for(;;)
	{
        
        // 16bit backbuffer
        //strColourFormat = kEAGLColorFormatRGB565;
        // 32bit bakcbuffer
        strColourFormat = kEAGLColorFormatRGBA8;
		
		int iMSAA = 0;
		
		
#ifdef BUILD_OGLES2
		//Create the OpenGL ES view and add it to the window
        // need stencil & depth
		iDepthFormat = iStencilFormat = GL_DEPTH24_STENCIL8_OES;
        // just stencil
		//iDepthFormat = 0;
		//iStencilFormat = GL_STENCIL_INDEX8;
        // just depth
        //iDepthFormat = GL_DEPTH_COMPONENT24_OES;
        //iStencilFormat = 0;
#else
		// OGLES 1.1
		//Create the OpenGL ES view and add it to the window
        iDepthFormat = iStencilFormat = GL_DEPTH24_STENCIL8_OES;
		// just stencil
		//iDepthFormat = 0;
		//iStencilFormat = GL_STENCIL_INDEX8_OES;
        // just depth
        //iDepthFormat = GL_DEPTH_COMPONENT24_OES;
        //iStencilFormat = 0;
        
#endif
        
        
        _glView = [[EAGLView alloc] initWithFrame:rect pixelFormat:strColourFormat depthFormat:iDepthFormat stencilFormat:iStencilFormat preserveBackbuffer:NO scale:scale msaaMaxSamples:2*iMSAA];
        // i.e. 1,2,4 for MSAA
        
        if(_glView!=NULL)
		{
            //NSString *string = @"A contrived string %@";
            //NSLog(string);
            //NSLog(@"device created \n");
			break;	// success!!!
		}
        
    }
    
    // set glview
    //_glView.re
    _glView.transform = CGAffineTransformTranslate(_glView.transform, -rect.size.width / 2.0, -rect.size.height / 2.0);
    _glView.transform = CGAffineTransformRotate(_glView.transform, -M_PI /2.0);
    _glView.transform = CGAffineTransformTranslate(_glView.transform, -rect.size.width / 2.0, rect.size.height / 2.0  - offset);
    
    //_glView.transform = CGAffineTransformTranslate(_glView.transform, -rect.size.width / 2.0, rect.size.height / 2.0  - offset);
    
    //_touchView.transform = CGAffineTransformTranslate(_touchView.transform, offset, 0);
    
    [_touchView setExclusiveTouch: TRUE];
    [_touchView reloadInputViews];
    
    //[_window addSubview:_touchView];
    [_window addSubview:_touchView];
    [_window addSubview:_glView];
    
    //[_glView setUserInteractionEnabled:NO];
    
    //[_window insertSubview:_touchView aboveSubview:_glView];
    
    //[_touchView setExclusiveTouch:YES];
    //_window.autoresizesSubviews = true;
    
	//Show the window
	[_window makeKeyAndVisible];
    
    m_eglView = _glView;
    
	// clear color & depth
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClearDepthf(0);
    
    sii.fWidth = rect.size.width * scale;
    sii.fHeight = rect.size.height * scale - offset * 2.0;
    
	glViewport(0,0,sii.fWidth, sii.fHeight);
    
    return (HWND)1;
}

void gkDeviceRenderContext::startRender()
{
    EAGLView* _glView = (EAGLView*)m_eglView;
    [_glView BeginRender];
}

void gkDeviceRenderContext::swapBuffer()
{
    EAGLView* _glView = (EAGLView*)m_eglView;
    
    
    
    //glClearColor(0.3, 0.3, 0.8, 1);
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    
    [_glView EndRender];
}

void gkDeviceRenderContext::makeThreadContext(bool close)
{
    if ( pthread_equal(gEnv->pSystemInfo->mainThreadId, pthread_self()) )
    {
        
    }
    else
    {
        EAGLView* _glView = (EAGLView*)m_eglView;
        
        if (close) {
            [_glView setCurrentContext];
        }
        else
        {
            [_glView setCurrentContextLT];
        }
        
    }
}


