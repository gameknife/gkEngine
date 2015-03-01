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



// RibbonSample.h : main header file for the RIBBONSAMPLE application
//

#if !defined(AFX_RIBBONSAMPLE_H__74489F48_E565_4B03_B8D5_A9D066133B66__INCLUDED_)
#define AFX_RIBBONSAMPLE_H__74489F48_E565_4B03_B8D5_A9D066133B66__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CRibbonSampleApp:
// See RibbonSample.cpp for the implementation of this class
//
struct ISystem;
class CMainFrame;
struct IEditor;
struct IGameFramework;


typedef IGameFramework* (*GET_SYSTEM)(void);
typedef void (*DESTROY_END)(void);
static CMainFrame* GetMainFrame();
class CRibbonSampleApp : public CWinApp
{
public:
	CRibbonSampleApp();
	virtual ~CRibbonSampleApp();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRibbonSampleApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual BOOL OnIdle(LONG lCount);
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CRibbonSampleApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG

	BOOL FirstInstance();

// gkSystem
	IGameFramework*	m_pGameFramework;
	HINSTANCE	m_hSystemHandle;
	GET_SYSTEM	m_FuncStart;
	DESTROY_END m_FuncEnd;

// Ieditor
	IEditor*    m_pEditor;

	HINSTANCE m_richeditInst;

	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RIBBONSAMPLE_H__74489F48_E565_4B03_B8D5_A9D066133B66__INCLUDED_)
