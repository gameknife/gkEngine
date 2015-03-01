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
// Name:   	gkToolBarPaneBase.h
// Desc:		
// 	
// Author:  Kaiming
// Date:	2012/2/12
// Modify:	2012/2/12
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _GKTOOLBARPANEBAE_H_
#define _GKTOOLBARPANEBAE_H_

//////////////////////////////////////////////////////////////////////////
//
// K&K Studio GameKnife ENGINE Source File
//
// Name:   	gkMaterialEditor.h
// Desc:		
// 
// Author:  Kaiming
// Date:	2011/11/3 
// Modify:	2011/11/3
// 
//////////////////////////////////////////////////////////////////////////
#pragma once
#include "..\Resource.h"
#include "Panels\gkPaneBase.h"
#include "Panels\gkPaneManager.h"

class gkToolBarPaneBase : public IPaneBase
{
	//DECLARE_DYNAMIC(gkToolBarPaneBase)

public:
	gkToolBarPaneBase(UINT diagID, CWnd* pParent = NULL);   // standard constructor
	//gkToolBarPaneBase(CWnd* pParent = NULL);   // standard constructor
	virtual ~gkToolBarPaneBase();

	CBrush m_brush;

	CRect m_rcBorders;

	void InitToolBar(const TCHAR* toolbarname, DWORD nId);

	bool m_bInRepositionControls;

	gkPaneManager m_paneManager;
	uint32 m_paneIDCounter;

	typedef std::map<gkStdString, uint32> NameIDMap;
	NameIDMap m_paneMap;

	void RegisterPane(CWnd* pCtrl, const TCHAR* name, XTPDockingPaneDirection dockdir = xtpPaneDockLeft, CSize constrain = CSize(2048,1024), int dockoption = (xtpPaneNoHideable | xtpPaneNoCloseable | xtpPaneNoFloatableByTabDoubleClick));
	void OpenPane(const TCHAR* name);
	void FloatPane(const TCHAR* name);
	void ClosePane(const TCHAR* name);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	virtual BOOL OnInitDialog();
	
	
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	//}}AFX_MSG
	afx_msg LRESULT OnKickIdle(WPARAM wp, LPARAM lCount);

	DECLARE_MESSAGE_MAP()

	void RecalcLayout();
	void RepositionBarsInternal(UINT nIDFirst, UINT nIDLast, UINT nIDLeftOver,
		UINT nFlags=reposDefault, LPRECT lpRectParam=NULL, LPCRECT lpRectClient=NULL, BOOL bStretch=TRUE);
	void RepositionWindowInternal(AFX_SIZEPARENTPARAMS* lpLayout,HWND hWnd, LPCRECT lpRect);
	void RefreshMetrics( BOOL bOffice2007 );
};


#endif