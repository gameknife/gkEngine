#include "StableHeader.h"
#include "RenderContextIOS.h"
//#include "gkRendererGLES2.h"


#import <UIKit/UIKit.h>
#import <OpenGLES/EAGL.h>
#import <OpenGLES/EAGLDrawable.h>

#import "EAGLView.h"

bool DeviceRenderContext::destroyDevice()
{
	return true;
}

bool DeviceRenderContext::initDevice(HWND hWnd)
{
    float scale = 1.0;
    
    UIScreen* screen = [UIScreen mainScreen];
	if ([UIScreen instancesRespondToSelector:@selector(scale)])
	{
		scale = [screen scale];
	}
	CGRect	rect = [screen applicationFrame];
    
    
    UIWindow* _window = (UIWindow*)hWnd;
    
    //Create a full-screen window
	_window = [[UIWindow alloc] initWithFrame:rect];
	
	NSString *strColourFormat;
	int iDepthFormat, iStencilFormat;
	
    EAGLView* _glView;
    
	for(;;)
	{
        
        // 16bit backbuffer
        //strColourFormat = kEAGLColorFormatRGB565;
        // 32bit bakcbuffer
        strColourFormat = kEAGLColorFormatRGBA8;
		
		int iMSAA = 1;
		
		
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
    
        if(_glView!=NO)
		{
			break;	// success!!!
		}
        
    }
    
    // set glview
    [_window addSubview:_glView];
    
	//Show the window
	[_window makeKeyAndVisible];
    
    m_eglView = _glView;
    
	// clear color & depth
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClearDepthf(0);

	//glViewport(0,0,sii.fWidth, sii.fHeight);
	return true;
}

void DeviceRenderContext::swapBuffer()
{
    EAGLView* _glView = (EAGLView*)m_eglView;
    
    [_glView BeginRender];
    
    glClearColor(0.3, 0.3, 0.8, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    
    [_glView EndRender];
}


