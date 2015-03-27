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
// Name:   	HwTextureLoader.h
// Desc:	
// 	
// 
// Author:  YiKaiming
// Date:	2013/7/9
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _DDsTextureLoader_h_
#define _DDsTextureLoader_h_

#include "Prerequisites.h"
#include "ITextureLoader.h"


class DDS_Loader : public HWT_Loader
{
public:
	virtual EHwTexError Load_Data(const void* pointer, uint32 size );

	virtual EHwTexError Load_Bind(GLuint *const texName,
		const unsigned int nLoadFromLevel);

	virtual bool Load_IsCube(const void* pointer );

	virtual uint32 getWidth() {return iWidth;}
	virtual uint32 getHeight() {return iHeight;}

	uint32 iWidth;
	uint32 iHeight;
	uint32 iMipCount;
	uint32 iArraySize;

	uint32 fmt;
	const BYTE* pSrcBits;
	GLuint* m_hw_texptr;
};


#endif