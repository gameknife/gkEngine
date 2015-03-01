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



#if !defined(AFX_BACKSTAGEPAGESAVEWEB_H__025EFF2E_691E_469B_A5B5_95A48731C7D8__INCLUDED_)
#define AFX_BACKSTAGEPAGESAVEWEB_H__025EFF2E_691E_469B_A5B5_95A48731C7D8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BackstagePageSaveWeb.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBackstagePageSaveWeb dialog

class CBackstagePageSaveWeb : public CXTPRibbonBackstagePage
{
	DECLARE_DYNCREATE(CBackstagePageSaveWeb)

// Construction
public:
	CBackstagePageSaveWeb();
	~CBackstagePageSaveWeb();

// Dialog Data
	//{{AFX_DATA(CBackstagePageSaveWeb)
	enum { IDD = IDD_BACKSTAGEPAGE_SAVE_WEB };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CBackstagePageSaveWeb)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
	CXTPRibbonBackstageLabel m_lblCaption;
	CXTPRibbonBackstageSeparator m_lblSeparator;
	CXTPRibbonBackstageButton m_btnSignIn;

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CBackstagePageSaveWeb)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BACKSTAGEPAGESAVEWEB_H__025EFF2E_691E_469B_A5B5_95A48731C7D8__INCLUDED_)
