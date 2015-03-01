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
// Name:   	gkPaneConsole.h
// Desc:	控制台面板	
// 
// Author:  Kaiming-Desktop
// Date:	2011 /8/17
// Modify:	2011 /8/17
// 
//////////////////////////////////////////////////////////////////////////

#ifndef GKPANECONSOLE_H_
#define GKPANECONSOLE_H_

#include "resource.h"
#include "afxcmn.h"
#include "Panels/gkPaneBase.h"

class CPaneConsole;

struct SConsolePaneLogCallback : public ILogCallback
{
	friend CPaneConsole;
	CPaneConsole* m_pOwner;

	SConsolePaneLogCallback(CPaneConsole* owner):m_pOwner(owner) {}

	virtual void OnWriteToConsole( const TCHAR *sText,bool bNewLine );
	virtual void OnWriteToFile( const TCHAR *sText,bool bNewLine ) {}
};

class CPaneConsole : public IPaneBase
{
	// Construction
public:
	CPaneConsole(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPaneConsole();

	// Dialog Data
	//{{AFX_DATA(CPaneStyles)
	enum { IDD = IDD_PANE_CONSOLE };

	CXTPButton	m_btnCVar;
	SConsolePaneLogCallback* m_pCallback;
	//}}AFX_DATA

	CBrush m_brush;

	void RefreshMetrics(BOOL bOffice2007);

	void AddAMessageLine( const TCHAR *sText, bool bNewLine );

	void RegConsoleCallback();

private:
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
	void OnCVar();
	afx_msg void OnSize(UINT nType, int cx, int cy);

	// Generated message map functions
	//{{AFX_MSG(CPaneStyles)
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	CRichEditCtrl m_richEdit;


public:
	virtual const TCHAR* getPanelName() {return _T("Console Panel");}
	virtual UINT getIID() {return IDD;}
};

#endif