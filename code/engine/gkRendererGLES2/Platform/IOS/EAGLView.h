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



/******************************************************************************

 @File         EAGLView.h

 @Title        

 @Version      

 @Copyright    Copyright (c) Imagination Technologies Limited.

 @Platform     

 @Description  

******************************************************************************/
#ifndef _EAGLVIEW_H_
#define _EAGLVIEW_H_


#import <UIKit/UIKit.h>
#import <OpenGLES/EAGL.h>
#import <OpenGLES/EAGLDrawable.h>


#ifdef BUILD_OGLES		
#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>
#elif BUILD_OGLES2
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>
#endif

#define MSAAx4 1

//CONSTANTS
//const double c_TouchDistanceThreshold = 20.0;

//CLASSES:

@class EAGLView;

//PROTOCOLS:

@protocol EAGLViewDelegate <NSObject>
- (void) didResizeEAGLSurfaceForView:(EAGLView*)view; //Called whenever the EAGL surface has been resized
@end

//CLASS INTERFACE:

/*
This class wraps the CAEAGLLayer from CoreAnimation into a convenient UIView subclass.
The view content is basically an EAGL surface you render your OpenGL scene into.
Note that setting the view non-opaque will only work if the EAGL surface has an alpha channel.
*/
@interface EAGLView : UIView
{

@private
	NSString*				_format;
	GLuint					_depthFormat;
	GLuint					_stencilFormat;
	BOOL					_autoresize;
	EAGLContext				*_context;
    EAGLContext             *_loading_context;
    
    
	GLuint					_framebuffer;
	GLuint					_renderbuffer;
	GLuint					_depthBuffer;
	int						_enableMSAA;
	int						_enableFramebufferDiscard;
	GLuint					_msaaMaxSamples;
	GLuint					_msaaFrameBuffer;
	GLuint					_msaaColourBuffer;
	GLuint					_msaaDepthBuffer;
	GLuint					_stencilBuffer;
	CGSize					_size;
	BOOL					_hasBeenCurrent;
	id<EAGLViewDelegate>	_delegate;
	float					_scale;
	
	CGPoint					m_TouchStart;
}
- (id) initWithFrame:(CGRect)frame; //These also set the current context
- (id) initWithFrame:(CGRect)frame pixelFormat:(NSString*)format;
- (id) initWithFrame:(CGRect)frame pixelFormat:(NSString*)format depthFormat:(GLuint)depth stencilFormat:(GLuint)stencil preserveBackbuffer:(BOOL)retained scale:(float)fscale msaaMaxSamples:(GLuint)maxSamples;

@property(readonly) GLuint framebuffer;
@property(readonly) NSString* pixelFormat;
@property(readonly) GLuint depthFormat;
@property(readonly) EAGLContext *context;
@property(readonly) EAGLContext *loading_context;

@property BOOL autoresizesSurface; //NO by default - Set to YES to have the EAGL surface automatically resized when the view bounds change, otherwise the EAGL surface contents is rendered scaled
@property(readonly, nonatomic) CGSize surfaceSize;

@property(assign) id<EAGLViewDelegate> delegate;

- (void) setCurrentContext;
- (BOOL) isCurrentContext;
- (void) clearCurrentContext;

- (void) setCurrentContextLT;

- (void) BeginRender;
- (void) EndRender;
- (void) swapBuffers; //This also checks the current OpenGL error and logs an error if needed

- (CGPoint) convertPointFromViewToSurface:(CGPoint)point;
- (CGRect) convertRectFromViewToSurface:(CGRect)rect;

//- (void) setPVRShellInit: (PVRShellInit*) pPVRShellInit;

@end

#endif // _EAGLVIEW_H_

