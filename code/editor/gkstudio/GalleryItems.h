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



// GalleryItems.h: interface for the CGalleryItemStyleSet class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GALLERYITEMS_H__04E890F7_BFE8_48DF_9E4A_AE16DBA3BD6A__INCLUDED_)
#define AFX_GALLERYITEMS_H__04E890F7_BFE8_48DF_9E4A_AE16DBA3BD6A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CGalleryItemStyleSet : public CXTPControlGalleryItem
{
public:
	CGalleryItemStyleSet(int nId, LPCTSTR lpszCaption);
	virtual ~CGalleryItemStyleSet();

	virtual void Draw(CDC* pDC, CXTPControlGallery* pGallery, CRect rcItem, BOOL bEnabled, BOOL bSelected, BOOL bPressed, BOOL bChecked);
};

class CGalleryItemThemeColors : public CXTPControlGalleryItem
{
public:
	CGalleryItemThemeColors(int nId, LPCTSTR lpszCaption);

	virtual void Draw(CDC* pDC, CXTPControlGallery* pGallery, CRect rcItem, BOOL bEnabled, BOOL bSelected, BOOL bPressed, BOOL bChecked);
};

class CGalleryItemThemeFonts : public CXTPControlGalleryItem
{
public:
	CGalleryItemThemeFonts(int nId, LPCTSTR lpszCaption);

	virtual void Draw(CDC* pDC, CXTPControlGallery* pGallery, CRect rcItem, BOOL bEnabled, BOOL bSelected, BOOL bPressed, BOOL bChecked);
};

class CGalleryItemTable : public CXTPControlGalleryItem
{
public:
	CGalleryItemTable(int nId);

	virtual void Draw(CDC* pDC, CXTPControlGallery* pGallery, CRect rcItem, BOOL bEnabled, BOOL bSelected, BOOL bPressed, BOOL bChecked);
};

class CGalleryItemTableLabel : public CXTPControlGalleryItem
{
public:
	CGalleryItemTableLabel();

	virtual void Draw(CDC* pDC, CXTPControlGallery* pGallery, CRect rcItem, BOOL bEnabled, BOOL bSelected, BOOL bPressed, BOOL bChecked);
};

class CGalleryItemFontColor : public CXTPControlGalleryItem
{
public:
	CGalleryItemFontColor(COLORREF clr, LPCTSTR lpszCaption, BOOL bTopMargin = TRUE, BOOL bBotttomMargin = TRUE);
	CGalleryItemFontColor(COLORREF clr, int nToolTip);

public:
	static void AddStandardColors(CXTPControlGalleryItems* pItems);
	static void AddStandardBackColors(CXTPControlGalleryItems* pItems);
	static void AddThemeColors(CXTPControlGalleryItems* pItems, int nTheme);

	virtual void Draw(CDC* pDC, CXTPControlGallery* pGallery, CRect rcItem, BOOL bEnabled, BOOL bSelected, BOOL bPressed, BOOL bChecked);

protected:
	BOOL m_bTopMargin;
	BOOL m_bBottomMargin;

};

class CGalleryItemFontFace : public CXTPControlGalleryItem
{
public:
	CGalleryItemFontFace(LPCTSTR lpszCaption);

public:
	static void AddFontItems(CXTPControlGalleryItems* pItems);

public:
	virtual void Draw(CDC* pDC, CXTPControlGallery* pGallery, CRect rcItem, BOOL bEnabled, BOOL bSelected, BOOL bPressed, BOOL bChecked);
};




class CGalleryItemUndo : public CXTPControlGalleryItem
{
public:
	CGalleryItemUndo(LPCTSTR lpszCaption);

public:
	virtual void Draw(CDC* pDC, CXTPControlGallery* pGallery, CRect rcItem, BOOL bEnabled, BOOL bSelected, BOOL bPressed, BOOL bChecked);
};


class CControlGalleryUndo : public CXTPControlGallery
{
	DECLARE_XTP_CONTROL(CControlGalleryUndo);

public:
	CControlGalleryUndo();

	CXTPControlStatic* FindInfoControl();
	void OnSelectedItemChanged();
};


#endif // !defined(AFX_GALLERYITEMS_H__04E890F7_BFE8_48DF_9E4A_AE16DBA3BD6A__INCLUDED_)
