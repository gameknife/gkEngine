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

 @File         OpenGL_Internal.h

 @Title        

 @Version      

 @Copyright    Copyright (c) Imagination Technologies Limited.

 @Platform     

 @Description  

******************************************************************************/
#ifndef _OPENGL_INTERNAL_H_
#define _OPENGL_INTERNAL_H_

/* Degrees / Radians conversion */
#define DEGREES_TO_RADIANS(__ANGLE__) ((__ANGLE__) / 180.0 * M_PI)
#define RADIANS_TO_DEGREES(__ANGLE__) ((__ANGLE__) / M_PI * 180.0)

/* Generic error reporting */
#define REPORT_ERROR(__FORMAT__, ...) printf("%s: %s\n", __FUNCTION__, [[NSString stringWithFormat:__FORMAT__, __VA_ARGS__] UTF8String])

/* EAGL and GL functions calling wrappers that log on error */
#define CALL_EAGL_FUNCTION(__FUNC__, ...) ({ EAGLError __error = __FUNC__( __VA_ARGS__ ); if(__error != kEAGLErrorSuccess) printf("%s() called from %s returned error %i\n", #__FUNC__, __FUNCTION__, __error); (__error ? NO : YES); })
#define CHECK_GL_ERROR() ({ GLenum __error = glGetError(); if(__error) printf("OpenGL error 0x%04X in %s\n", __error, __FUNCTION__); (__error ? NO : YES); })

/* Optional delegate methods support */
#ifndef __DELEGATE_IVAR__
#define __DELEGATE_IVAR__ _delegate
#endif
#ifndef __DELEGATE_METHODS_IVAR__
#define __DELEGATE_METHODS_IVAR__ _delegateMethods
#endif
#define TEST_DELEGATE_METHOD_BIT(__BIT__) (self->__DELEGATE_METHODS_IVAR__ & (1 << __BIT__))
#define SET_DELEGATE_METHOD_BIT(__BIT__, __NAME__) { if([self->__DELEGATE_IVAR__ respondsToSelector:@selector(__NAME__)]) self->__DELEGATE_METHODS_IVAR__ |= (1 << __BIT__); else self->__DELEGATE_METHODS_IVAR__ &= ~(1 << __BIT__); }

#endif // _OPENGL_INTERNAL_H_

