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
// Name:   	gkFont.h
// Desc:	
// 	
// 
// Author:  YiKaiming
// Date:	2013/6/30
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _gkFont_h_
#define _gkFont_h_

#include "IFont.h"

#include "./../../ThirdParty/freetype/include/ft2build.h"
#include "./../../ThirdParty/freetype/include/freetype/ftmodapi.h"
#include "./../../ThirdParty/freetype/include/freetype/freetype.h"
#include "./../../ThirdParty/freetype/include/freetype/ftoutln.h"
#include "./../../thirdparty/freetype/include/freetype/ftglyph.h"

class gkFTFont;

typedef std::vector<gkFTFont*> gkFTFonts;

struct gkFTFontKey
{
	gkStdString m_filename;
	gkStdString m_filename2;

	int32 m_height;
	int32 m_weight;

	bool operator== ( const gkFTFontKey& other )
	{
		if ( m_height != other.m_height || m_weight != other.m_weight || m_filename != other.m_filename || m_filename2 != other.m_filename2)
		{
			return false;
		}
		else
		{
			return true;
		}
	}
};

class gkFTFaceManager;

class gkFont : public IFontModule
{
public:
	gkFont(void);
	~gkFont(void);

	void Init();
	void Destroy();

	IFtFont* CreateFont( const TCHAR* filename, int height, int weight, const TCHAR* secondname);
	void DestroyFont(IFtFont* target);

	virtual void DrawString( const TCHAR* strings, const IFtFont* font, const Vec2& screenpos, const ColorB& color, uint32 alignment );
	virtual void Flush();

	FT_Library& getFTLib() {return glibrary;}

	gkFTFaceManager* getFaceManager() {return m_faceManager;}

private:
	FT_Library  glibrary;

	gkFTFonts m_fonts;

	gkFTFaceManager* m_faceManager;
};

extern gkFont* g_font;
inline gkFont* getFontMain() {return g_font;}

#endif


