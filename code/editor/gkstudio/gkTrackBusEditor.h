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
// Name:   	gkTrackBusEditor.h
// Desc:	
// 	
// 
// Author:  YiKaiming
// Date:	2013/10/17
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _gkTrackBusEditor_h_
#define _gkTrackBusEditor_h_

#include "..\Resource.h"
#include "gkToolBarPaneBase.h"
#include "Panels/gkPaneBase.h"
#include "Controls\CustomSplitter.h"
#include "Controls\gkPropertyGirdEx.h"


class gkTrackBusEditor : public gkToolBarPaneBase
{
	//DECLARE_DYNAMIC(gkMaterialEditor)

public:
	gkTrackBusEditor(CWnd* pParent = NULL);   // standard constructor
	virtual ~gkTrackBusEditor();

	CBrush m_brush;
	CStatic m_wndPlaceHolder;
	gkPropertyGirdEx m_wndPropertyGrid;
	CXTTreeCtrl	m_treeMaterialList;
	CImageList m_imageList;

	CSplitterWndEx m_wndHSplitter;
	CSplitterWndEx m_wndVSplitter;

	typedef std::map<CString, HTREEITEM> NameTreeMap;
	NameTreeMap m_namemap;

// 	typedef std::vector<SMatItemInfo*> MatItemSet;
// 	MatItemSet m_items;

	void RefreshMetrics(BOOL bOffice2007);
	BOOL InitPropertyTable();

protected:
	//afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	void OnEmptyCommandRange(UINT);

	DECLARE_MESSAGE_MAP()

public:
	LRESULT OnGridNotify(WPARAM wParam, LPARAM lParam);

public:
	virtual const TCHAR* getPanelName() {return _T("TrackBus Editor");}
	virtual UINT getIID() {return 0;}

	afx_msg void OnTreeBrowserClick(NMHDR *pNMHDR, LRESULT *pResult);
};

#endif