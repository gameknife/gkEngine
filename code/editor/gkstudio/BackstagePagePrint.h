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



#if !defined(AFX_BACKSTAGEPAGEPRINT_H__A1221CB4_7CFC_47DB_8765_D3812497F8A4__INCLUDED_)
#define AFX_BACKSTAGEPAGEPRINT_H__A1221CB4_7CFC_47DB_8765_D3812497F8A4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BackstagePagePrint.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBackstagePagePrint dialog

class CBackstagePagePrint : public CXTPRibbonBackstagePage
{
	DECLARE_DYNCREATE(CBackstagePagePrint)

// Construction
public:
	CBackstagePagePrint();
	~CBackstagePagePrint();

// Dialog Data
	//{{AFX_DATA(CBackstagePagePrint)
	enum { IDD = IDD_BACKSTAGEPAGE_PRINT };
	int		m_nCopies;
	//}}AFX_DATA

	CXTPRibbonBackstageButton m_btnPrint;
	CXTPRibbonBackstageSeparator m_lblSeparator4;
	CXTPRibbonBackstageSeparator m_lblSeparator1;
	void InitButton(UINT nID);

	CXTPRibbonBackstageLabel m_lblPrint;

	BOOL OnSetActive();
	BOOL OnKillActive();

	class CBackstagePreviewView : CXTPPreviewView
	{

		friend class CBackstagePagePrint;
	};
	
	class CBackstagePrintView : CView
	{
		
		friend class CBackstagePagePrint;
	};

	CBackstagePreviewView* m_pPreviewView;
	CFrameWnd* m_pFrameWnd;
	CBackstagePrintView* m_pView;

	void UpdateCopies(BOOL bSaveAndValidate);
	
	BOOL CreatePrintPreview();

	BOOL OnInitDialog();
// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CBackstagePagePrint)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CBackstagePagePrint)
	afx_msg void OnButtonPrint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BACKSTAGEPAGEPRINT_H__A1221CB4_7CFC_47DB_8765_D3812497F8A4__INCLUDED_)
