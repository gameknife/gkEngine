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

#ifndef _HwTextureLoader_h_
#define _HwTextureLoader_h_

#include "Prerequisites.h"
#include "pvrtex_def.h"

#define ATC_SIGNATURE        0xCCC40002

#define ATC_RGB              0x00000001
#define ATC_RGBA             0x00000002
#define ATC_TILED            0X00000004

enum EHwTexError
{
	PVR_SUCCESS = 0,
	PVR_FAIL = 1,
	PVR_OVERFLOW = 2
};

EHwTexError LoadPVR(const void* pointer,
	GLuint *const texName,
	const unsigned int nLoadFromLevel);

struct HWT_Loader
{
	virtual EHwTexError Load_Data(const void* pointer ) =0;
	virtual EHwTexError Load_Bind(GLuint *const texName, const unsigned int nLoadFromLevel) =0;
	virtual bool Load_IsCube(const void* pointer ) =0;

	virtual uint32 getWidth() =0;
	virtual uint32 getHeight() =0;
};

class PVR_Loader : public HWT_Loader
{
public:
	virtual EHwTexError Load_Data(const void* pointer );

	virtual EHwTexError Load_Bind(GLuint *const texName,
		const unsigned int nLoadFromLevel);

	virtual bool Load_IsCube(const void* pointer );

	virtual uint32 getWidth() {return sTextureHeader.u32Width;}
	virtual uint32 getHeight() {return sTextureHeader.u32Height;}

	bool bIsCompressedFormat;
	GLenum eTextureFormat;
	GLenum eTextureInternalFormat;
	GLenum eTextureType;

	PVRTextureHeaderV3 sTextureHeader;
	uint8* pTextureData;
};


#endif