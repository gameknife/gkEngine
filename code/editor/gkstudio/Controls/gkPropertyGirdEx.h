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
// K&K Studio GameKnife ENGINE Source File
//
// Name:   	gkPropertyGirdEx.h
// Desc:		
// 	
// Author:  Kaiming
// Date:	2012/5/14
// Modify:	2012/5/14
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _gkPropertyGirdEx_h_
#define _gkPropertyGirdEx_h_

#include "PropertyGrid\XTPPropertyGrid.h"
#include "CustomItems.h"

class CCustomItemSlider;
class CCustomItemColor;
class CCustomItemFileBox;
class CCustomItemFilename;

//===========================================================================
// Summary:
//     CXTPPropertyGridVisualStudio2010Theme is a CXTPPropertyGridPaintManager derived
//     class.  This represents a Visual Studio 2010 style theme.
//===========================================================================
class CXTPPropertyGridVisualStudio2010ThemeCustom : public CXTPPropertyGridVisualStudio2010Theme
{
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPPropertyGridVisualStudio2010Theme object.
	// Parameters:
	//     pGrid - Points to a CXTPPropertyGrid object
	//-----------------------------------------------------------------------
	CXTPPropertyGridVisualStudio2010ThemeCustom(CXTPPropertyGrid* pGrid):CXTPPropertyGridVisualStudio2010Theme(pGrid) {}
	~CXTPPropertyGridVisualStudio2010ThemeCustom() {}
protected:
	//-------------------------------------------------------------------------
	// Summary:
	//     This method is called to refresh the visual metrics of the
	//     property grid.
	//-------------------------------------------------------------------------
	void RefreshMetrics();

};

class gkPropertyGirdEx : public CXTPPropertyGrid
{
	DECLARE_DYNAMIC(gkPropertyGirdEx)

public:

	gkPropertyGirdEx() {}
	virtual ~gkPropertyGirdEx() {}

	CCustomItemSlider* AddFloatVar( CXTPPropertyGridItem* cate, LPCTSTR lpszName, UINT id, float* val, float minval /*= 0*/, float maxval /*= 100*/, LPCTSTR lpszDescription /*= _T("描述")*/ );
	CCustomItemSlider* AddDoubleVar( CXTPPropertyGridItem* cate, LPCTSTR lpszName, UINT id, double* val, float minval /*= 0*/, float maxval /*= 100*/, LPCTSTR lpszDescription /*= _T("描述")*/ );
	CXTPPropertyGridItemNumber* AddIntegerVar( CXTPPropertyGridItem* cate, LPCTSTR lpszName, UINT id, int* val, LPCTSTR lpszDescription /*= _T("描述")*/ );
	CCustomItemColor* AddColorVar( CXTPPropertyGridItem* cate, LPCTSTR lpszName, UINT id, float* val, LPCTSTR lpszDescription /*= _T("描述")*/, IPSCPColorChangeCallback* callback = 0);
	CXTPPropertyGridItemBool* AddBoolVar( CXTPPropertyGridItem* cate, LPCTSTR lpszName, UINT id, bool* val, LPCTSTR lpszDescription /*= _T("描述")*/ );
	CCustomItemFilename* AddFileVar( CXTPPropertyGridItem* cate, LPCTSTR lpszName, LPCTSTR lpszValue, LPCTSTR lpszDescription /*= _T("描述")*/ );


	void AddStringVar( CXTPPropertyGridItem* cate, LPCTSTR lpszName, LPCTSTR lpszValue, LPCTSTR lpszDescription );


	virtual BOOL Create( const RECT& rect, CWnd* pParentWnd, UINT nID, DWORD dwListStyle = 0 );

	void RefreshMetrics(BOOL bOffice2007);

	void RefreshAllItem();

	void ExpandAll();
	void CollapseAll();

	DECLARE_MESSAGE_MAP()
	afx_msg void OnSysColorChange();
	



private:

};

#endif
