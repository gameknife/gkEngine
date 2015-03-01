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
#ifndef _gkTouchView_H_
#define _gkTouchView_H_

#import "EAGLView.h"

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

@class gkTouchView;

//PROTOCOLS:

//@protocol EAGLViewDelegate <NSObject>
//- (void) didResizeEAGLSurfaceForView:(EAGLView*)view; //Called whenever the EAGL surface has been resized
//@end

//CLASS INTERFACE:

/*
 This class wraps the CAEAGLLayer from CoreAnimation into a convenient UIView subclass.
 The view content is basically an EAGL surface you render your OpenGL scene into.
 Note that setting the view non-opaque will only work if the EAGL surface has an alpha channel.
 */
@interface gkTouchView : UIView
{
    
@private
	float					_scale;
	float                   _offset;
	CGPoint					m_TouchStart;
    CGRect                  m_Frame;
    
    bool                    m_gesture_processing;
}
- (id) initWithFrame:(CGRect)frame; //These also set the current context

- (void) setCurrentContext;
- (BOOL) isCurrentContext;
- (void) clearCurrentContext;

@end

#endif // _EAGLVIEW_H_

