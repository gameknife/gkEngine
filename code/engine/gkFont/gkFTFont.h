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
// Name:   	gkFTFont.h
// Desc:	
// 	
// 
// Author:  YiKaiming
// Date:	2013/6/30
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _gkFTFont_h_
#define _gkFTFont_h_

#include "gkPlatform.h"
#include "gkFont.h"
#include "ITexture.h"
#include "gkFontRenderable.h"

typedef std::vector<gkMaterialPtr> gkMaterialPtrs;

typedef std::vector<gkFontRenderable> gkFontRenderables;

class gkFTFont : public IFtFont
{
public:
	gkFTFont(void);
	~gkFTFont(void);
	
	void init( const gkFTFontKey& key, int height, int weight );
	void destroy();

	virtual int getHeight() {return m_height;}
	virtual int getWeight() {return m_weight;}

	gkFTCharInfo& getCharacterInfo( wchar_t character );
	void drawString( const TCHAR* strings, const Vec2& pos, const ColorB& color, uint32 alignment, uint32 depth_order = 0 );
	Vec2i measureString( const char* strings );
	Vec2i measureString( const wchar_t* strings );

	int StepByCharacter( wchar_t thisChar, Vec3i &tmpPos, FT_UInt& glyphPrev );

	void addTexture();
	
	void flush();

	void makeSureFaceSize();

	bool equal( gkFTFontKey& other ); 

private:
	gkFTFontKey m_key;

	int m_height;
	int m_weight;
	int m_spaceWidth;

	int m_ascend;
	int m_lineGap;

	int m_useKernering;
	

	FT_Face m_face;
	FT_Face m_face2;

	gkFTCharMap m_charMap;

	Vec3i m_textRenderBufferi;
	int m_textRenderIndex;

	gkFontRenderables m_renderables;
	gkFontRenderable* m_current_renderable;

	float m_pixelOffset;
};


#endif

