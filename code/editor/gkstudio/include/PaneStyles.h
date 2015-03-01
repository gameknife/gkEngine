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



#if !defined(AFX_PANESTYLES_H__12778969_E726_421D_A3ED_D6C977E48A7A__INCLUDED_)
#define AFX_PANESTYLES_H__12778969_E726_421D_A3ED_D6C977E48A7A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PaneStyles.h : header file
//
#include "resource.h"
#include "afxwin.h"
#include "Panels/gkPaneBase.h"

/////////////////////////////////////////////////////////////////////////////
// CPaneStyles dialog

class CPaneObject : public IPaneBase
{
// Construction
public:
	CPaneObject(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPaneStyles)
	enum { IDD = IDD_PANE_STYLES };
	CXTListBox	m_lstStyles;
	CXTPButton	m_chkLinkedStyle;
	CXTPButton	m_chkPreview;
	CXTPButton	m_btnStyleInspector;
	CXTPButton	m_btnNewStyle;
	CXTPButton	m_btnManageStyles;
	//}}AFX_DATA

	CBrush m_brush;

	void RefreshMetrics(BOOL bOffice2007);


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPaneStyles)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void OnOK();
	void OnCancel();

	// Generated message map functions
	//{{AFX_MSG(CPaneStyles)
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	BOOL InitPropertyTable();
public:
	CStatic m_wndPlaceholder;
	CXTPPropertyGrid m_wndPropertyGrid;	

public:
	virtual const TCHAR* getPanelName() {return _T("RollBar");}
	virtual UINT getIID() {return IDD;}
};


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PANESTYLES_H__12778969_E726_421D_A3ED_D6C977E48A7A__INCLUDED_)
