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



#ifndef _IMAGE_H_
#define _IMAGE_H_

#pragma once

#include "gkPlatform.h"

// These defines are used to tell us about the type of TARGA file it is
#define TGA_RGB		 2		// This tells us it's a normal RGB (really BGR) file
#define TGA_A		 3		// This tells us it's a ALPHA file
#define TGA_RLE		10		// This tells us that the targa is Run-Length Encoded (RLE)

#define GL_RGB                            0x1907
#define GL_RGBA                           0x1908

#pragma pack( push, 1 )			//禁止字节自动对齐
typedef struct
{
	uint8		descLen;
	uint8		cmapType;
	uint8		imageType;
	int16		cmapStart;
	uint16	cmapEntries;
	uint8		cmapBits;
	uint16	xOffset;
	uint16	yOffset;
}Header;

typedef struct 
{
	Header		head;
	uint16	width;
	uint16	height;
	uint8		bpp;
	uint8		attrib;
}TGAHeader;


typedef struct _tTexImage
{
	uint8*		imageData;
	uint32		width;
	uint32		height;
	uint32		bpp;				//Image color depth in bits per pixel
	uint32		texID;
	uint32		imageType;
	bool		bCompressed;		//Compressed or Uncompressed

	_tTexImage():imageData(NULL){}
}TexImage;

#pragma pack( pop )

class Image
{
public:
	Image(void);
	~Image(void);

	
public:
	/** 加载TGA图片 **/
	bool	loadTGA( TexImage* texture, LPCSTR filename );
	bool	release( TexImage* texture );
	
protected:
	bool    loadUncompressedTGA( TexImage* texture, FILE* file );
	bool	loadCompressedTGA( TexImage* texture, FILE* file );
};

#endif