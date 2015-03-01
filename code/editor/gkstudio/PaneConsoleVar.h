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



#pragma once
#include "..\Resource.h"
#include "afxwin.h"
#include "Panels/gkPaneBase.h"
#include "Controls/gkPropertyGirdEx.h"
// CPaneConsoleVar dialog
class CCustomItemSlider;

class CPaneConsoleVar : public IPaneBase
{
	DECLARE_DYNAMIC(CPaneConsoleVar)

public:
	CPaneConsoleVar(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPaneConsoleVar();

	void RefreshMetrics(BOOL bOffice2007);
	virtual BOOL OnInitDialog();

// Dialog Data
	enum { IDD = IDD_PANE_CONSOLEVAR };

protected:
	BOOL InitPropertyTable();
	BOOL RefreshPropertyMap(LPCTSTR filter = NULL);

protected:
	LRESULT OnGridNotify(WPARAM wParam, LPARAM lParam);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	
	

	DECLARE_MESSAGE_MAP()
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);

	
public:
	CStatic m_wndPlaceHolder;
	gkPropertyGirdEx m_wndPropertyGrid;	
	CXTPEdit m_edtSearch;

	
	afx_msg void OnEnChangeEditCvsearch();
	CStatic m_lblSearch;

public:
	virtual const TCHAR* getPanelName() {return _T("ConsoleVar Panel");}
	virtual UINT getIID() {return IDD;}
};
