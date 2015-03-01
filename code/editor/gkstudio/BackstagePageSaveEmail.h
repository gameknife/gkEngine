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



#if !defined(AFX_BACKSTAGEPAGESAVEEMAIL_H__2D794247_017D_43DD_9E00_F8759BCA7BE1__INCLUDED_)
#define AFX_BACKSTAGEPAGESAVEEMAIL_H__2D794247_017D_43DD_9E00_F8759BCA7BE1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BackstagePageSaveEmail.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBackstagePageSaveEmail dialog

class CBackstagePageSaveEmail : public CXTPRibbonBackstagePage
{
	DECLARE_DYNCREATE(CBackstagePageSaveEmail)

// Construction
public:
	CBackstagePageSaveEmail();
	~CBackstagePageSaveEmail();

// Dialog Data
	//{{AFX_DATA(CBackstagePageSaveEmail)
	enum { IDD = IDD_BACKSTAGEPAGE_SAVE_EMAIL };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CBackstagePageSaveEmail)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	CXTPRibbonBackstageLabel m_lblCaption;
	CXTPRibbonBackstageSeparator m_lblSeparator;
	CXTPRibbonBackstageButton m_btnSend;

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CBackstagePageSaveEmail)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BACKSTAGEPAGESAVEEMAIL_H__2D794247_017D_43DD_9E00_F8759BCA7BE1__INCLUDED_)
