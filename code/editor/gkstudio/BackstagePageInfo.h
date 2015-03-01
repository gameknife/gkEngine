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



#if !defined(AFX_BACKSTAGEPAGEINFO_H__96962F77_2D2D_4C65_BE2B_DF2425152C6C__INCLUDED_)
#define AFX_BACKSTAGEPAGEINFO_H__96962F77_2D2D_4C65_BE2B_DF2425152C6C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BackstagePageInfo.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBackstagePageInfo dialog

class CBackstagePageInfo : public CXTPRibbonBackstagePage
{
// Construction
public:
	CBackstagePageInfo();   // standard constructor

// Dialog Data
	//{{AFX_DATA(CBackstagePageInfo)
	enum { IDD = IDD_BACKSTAGEPAGE_INFO };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBackstagePageInfo)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	CXTPRibbonBackstageLabel m_lblInformation;

	CXTPRibbonBackstageLabel m_lblPermissions;
	CXTPRibbonBackstageLabel m_lblPrepareForSharing;
	CXTPRibbonBackstageLabel m_lblVersions;
	
	CXTPRibbonBackstageSeparator m_lblSeparator1;
	CXTPRibbonBackstageSeparator m_lblSeparator2;
	CXTPRibbonBackstageSeparator m_lblSeparator3;
	CXTPRibbonBackstageSeparator m_lblSeparator4;

	CXTPRibbonBackstageButton m_btnProtectDocument;
	CXTPRibbonBackstageButton m_btnCheckForIssues;
	CXTPRibbonBackstageButton m_btnManageVersions;

	void InitButton(UINT nID, BOOL bPopup);

	// Generated message map functions
	//{{AFX_MSG(CBackstagePageInfo)
	virtual BOOL OnInitDialog();
	afx_msg void OnStaticPreview();
	//}}AFX_MSG
	void OnProtectDocumentDropDown();
	void OnCheckForIssuesDropDown();
	
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BACKSTAGEPAGEINFO_H__96962F77_2D2D_4C65_BE2B_DF2425152C6C__INCLUDED_)
