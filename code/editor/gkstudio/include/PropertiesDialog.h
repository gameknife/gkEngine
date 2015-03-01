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



#if !defined(AFX_PROPERTIESDIALOG_H__CBBFC362_FD42_4574_9E6A_AA5F3F171128__INCLUDED_)
#define AFX_PROPERTIESDIALOG_H__CBBFC362_FD42_4574_9E6A_AA5F3F171128__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropertiesDialog.h : header file
//

#include <XTToolkitPro.h>
#include "Controls\PropertyCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CPropertiesDialog dialog

class CPropertiesDialog : public CXTResizeDialog
{
// Construction
public:
	CPropertiesDialog( const CString &title,XmlNodeRef &node,CWnd* pParent = NULL, bool bShowSearchBar=false);   // standard constructor
	typedef Functor1<IVariable*> UpdateVarCallback;

// Dialog Data
	//{{AFX_DATA(CPropertiesDialog)
	enum { IDD = IDD_PROPERTIES };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	void SetUpdateCallback( UpdateVarCallback cb ) { m_varCallback = cb; };
	CPropertyCtrl* GetPropertyCtrl() { return &m_wndProps; };

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPropertiesDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual void OnOK() {};
	virtual void OnCancel();

	void OnPropertyChange( IVariable *pVar );
	void ConfigureLayout();

	// Generated message map functions
	//{{AFX_MSG(CPropertiesDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnClose();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	CPropertyCtrl m_wndProps;
	XmlNodeRef m_node;
	CString m_title;
	UpdateVarCallback m_varCallback;

	bool m_bShowSearchBar; 
	CEdit m_input;
	CStatic m_searchLabel;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERTIESDIALOG_H__CBBFC362_FD42_4574_9E6A_AA5F3F171128__INCLUDED_)
