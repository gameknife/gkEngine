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
// yikaiming (C) 2013
// gkENGINE Source File 
//
// Name:   	glExtension.h
// Desc:	
// 	
// 
// Author:  YiKaiming
// Date:	2013/7/9
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _glExtension_h_
#define _glExtension_h_

#include "Prerequisites.h"

// Type definitions for pointers to functions returned by eglGetProcAddress
typedef void (GL_APIENTRY *PFNGLMULTIDRAWELEMENTS) (GLenum mode, GLsizei *count, GLenum type, const GLvoid **indices, GLsizei primcount); // glvoid
typedef void* (GL_APIENTRY *PFNGLMAPBUFFEROESPROC)(GLenum target, GLenum access);
typedef GLboolean (GL_APIENTRY *PFNGLUNMAPBUFFEROESPROC)(GLenum target);
typedef void (GL_APIENTRY *PFNGLGETBUFFERPOINTERVOESPROC)(GLenum target, GLenum pname, void** params);
typedef void (GL_APIENTRY * PFNGLMULTIDRAWARRAYS) (GLenum mode, GLint *first, GLsizei *count, GLsizei primcount); // glvoid
typedef void (GL_APIENTRY * PFNGLDISCARDFRAMEBUFFEREXTPROC)(GLenum target, GLsizei numAttachments, const GLenum *attachments);

typedef void (GL_APIENTRYP PFNGLBINDVERTEXARRAYOESPROC) (GLuint vertexarray);
typedef void (GL_APIENTRYP PFNGLDELETEVERTEXARRAYSOESPROC) (GLsizei n, const GLuint *vertexarrays);
typedef void (GL_APIENTRYP PFNGLGENVERTEXARRAYSOESPROC) (GLsizei n, GLuint *vertexarrays);
typedef GLboolean (GL_APIENTRYP PFNGLISVERTEXARRAYOESPROC) (GLuint vertexarray);

typedef void (GL_APIENTRYP PFNGLRENDERBUFFERSTORAGEMULTISAMPLEIMG) (GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height);
typedef void (GL_APIENTRYP PFNGLFRAMEBUFFERTEXTURE2DMULTISAMPLEIMG) (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLsizei samples);

typedef void* (GL_APIENTRYP PFNGLMAPBUFFEROESPROC) (GLenum target, GLenum access);
typedef GLboolean (GL_APIENTRYP PFNGLUNMAPBUFFEROESPROC) (GLenum target);
typedef void (GL_APIENTRYP PFNGLGETBUFFERPOINTERVOESPROC) (GLenum target, GLenum pname, GLvoid** params);

struct gkGLExtension
{
	static void Init();
    


	// GL_EXT_multi_draw_arrays
// 	static PFNGLMULTIDRAWELEMENTS				glMultiDrawElementsEXT;
// 	static PFNGLMULTIDRAWARRAYS				glMultiDrawArraysEXT;

	// GL_EXT_multi_draw_arrays
	static PFNGLMAPBUFFEROESPROC                   glMapBufferOES;
	static PFNGLUNMAPBUFFEROESPROC                 glUnmapBufferOES;
	static PFNGLGETBUFFERPOINTERVOESPROC           glGetBufferPointervOES;

	// GL_EXT_discard_framebuffer
	static PFNGLDISCARDFRAMEBUFFEREXTPROC			glDiscardFramebufferEXT;

	// GL_OES_vertex_array_object
// #if !defined(GL_OES_vertex_array_object)
// #define GL_VERTEX_ARRAY_BINDING_OES 0x85B5
// #endif

	static bool EXT_VAO;
	static bool COMPRESS_VERTEX;
	static bool INDEX_32BIT;

	static PFNGLBINDVERTEXARRAYOESPROC glBindVertexArrayOES;
	static PFNGLDELETEVERTEXARRAYSOESPROC glDeleteVertexArraysOES;
	static PFNGLGENVERTEXARRAYSOESPROC glGenVertexArraysOES;
	static PFNGLISVERTEXARRAYOESPROC glIsVertexArrayOES;

	static bool IsGLExtensionSupported(const char * const extension);

	static const GLubyte * EXTENSIONS;
};


#endif

