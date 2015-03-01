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
// Name:   	IFont.h
// Desc:	
// 	
// 
// Author:  YiKaiming
// Date:	2013/6/30
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _IFont_h_
#define _IFont_h_

#include "gkPlatform.h"
#include "MathLib/gk_Color.h"

enum EFontAlignment
{
	eFA_Left = 1<<0,
	eFA_Right = 1<<1,
	eFA_HCenter = 1<<2,

	eFA_Top = 1<<3,
	eFA_Bottom = 1<<4,
	eFA_VCenter = 1<<5,
};

#define GKFONT_OUTLINE_0PX 0
#define GKFONT_OUTLINE_1PX 1
#define GKFONT_OUTLINE_2PX 2
#define GKFONT_OUTLINE_3PX 3

struct IFtFont
{
	virtual int getHeight() =0;
	virtual int getWeight() =0;
};

struct IFontModule
{
	virtual ~IFontModule() {}

	virtual void Init() =0;
	virtual void Destroy() =0;

	virtual IFtFont* CreateFont( const TCHAR* filename, int height, int weight, const TCHAR* secondname = NULL) = 0;

	virtual void DrawString( const TCHAR* strings, const IFtFont* font, const Vec2& screenpos, const ColorB& color, uint32 alignment = (eFA_Left | eFA_Top) ) =0;
	virtual void Flush() =0;
};


#endif