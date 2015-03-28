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
// yikaiming (C) 2014
// gkENGINE Source File 
//
// Name:   	gkVirtualAPI.h
// Desc:	
// 	
// 
// Author:  gameKnife
// Date:	2015/3/28
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _gkVirtualAPI_h_
#define _gkVirtualAPI_h_

struct gkVirtualAPI
{
	enum MapFlag
	{
		eVAPI_MapWrite,
		eVAPI_MapRead,
		eVAPI_MapRW,
	};

	static void gkVAPI_MapBuffer(GLvoid* *data, GLenum target, uint32 size, MapFlag flag);
	static void gkVAPI_UnMapBuffer(GLvoid* data, GLenum target, uint32 size);
	static void gen_or_bind_vao(GLuint& vao, GLuint& vbo);
	static void unbind_vao();
};


#endif

