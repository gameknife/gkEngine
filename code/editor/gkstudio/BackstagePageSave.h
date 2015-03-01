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



#if !defined(AFX_CBackstagePageSave_H__0789D950_3894_46E8_B34F_996A19A138E3__INCLUDED_)
#define AFX_CBackstagePageSave_H__0789D950_3894_46E8_B34F_996A19A138E3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CBackstagePageSave.h : header file
//
#include "BackstagePageSaveEmail.h"
#include "BackstagePageSaveWeb.h"

/////////////////////////////////////////////////////////////////////////////
// CBackstagePageSave dialog

class CBackstagePageSave : public CXTPRibbonBackstagePage
{
// Construction
public:
	CBackstagePageSave();
	~CBackstagePageSave();

// Dialog Data
	//{{AFX_DATA(CBackstagePageSave)
	enum { IDD = IDD_BACKSTAGEPAGE_SAVE };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CBackstagePageSave)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	BOOL OnInitDialog();
// Implementation

	CBackstagePageSaveEmail m_pageSaveEmail;
	CBackstagePageSaveWeb m_pageSaveWeb;


	CXTPRibbonBackstageSeparator m_lblSeparator1;
	CXTPRibbonBackstageSeparator m_lblSeparator2;
	CXTPRibbonBackstageSeparator m_lblSeparator4;

	CXTPRibbonBackstageLabel m_lblSaveAndSend;
	CXTPRibbonBackstageLabel m_lblFileTypes;

	CXTPRibbonBackstageButton m_btnSendEmail;
	CXTPRibbonBackstageButton m_btnSaveToWeb;
	CXTPRibbonBackstageButton m_btnSaveToSharePoint;
	CXTPRibbonBackstageButton m_btnPublish;
	CXTPRibbonBackstageButton m_btnChangeFileType;
	CXTPRibbonBackstageButton m_btnCreatePDF;

	CXTPRibbonBackstageButton* m_pSelectedButton;
	
	void InitButton(CXTPRibbonBackstageButton& btn);
	
	CXTPRibbonBackstagePage* GetButtonPage(CXTPRibbonBackstageButton* pButton);
	CXTPImageManager m_imagelist;


protected:
	// Generated message map functions
	//{{AFX_MSG(CBackstagePageSave)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	void OnButtonSelected();
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CBackstagePageSave_H__0789D950_3894_46E8_B34F_996A19A138E3__INCLUDED_)
