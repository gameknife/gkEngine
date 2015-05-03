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



// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__E6D95F91_FCF6_4C13_A74C_E25D863697E5__INCLUDED_)
#define AFX_MAINFRM_H__E6D95F91_FCF6_4C13_A74C_E25D863697E5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Panels/PaneStyles.h"
#include "Panels/gkPaneConsole.h"
#include "Panels/gkPaneTimeOfDay.h"
#include "Panels/gkMaterialEditor.h"
#include "PaneConsoleVar.h"
#include "Panels\gkPaneManager.h"
#include "Panels\gkSwissKnifeBar.h"

// #include "BackstagePageInfo.h"
// #include "BackstagePageHelp.h"
// #include "BackstagePageSave.h"
// #include "BackstagePageRecent.h"
// #include "BackstagePagePrint.h"

class CMainFrame : public CXTPFrameWnd
{
	friend class gkEditor;
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:

// Operations
public:
	CXTPControlGalleryItems* m_pItemsShapes;
	CXTPControlGalleryItems* m_pItemsStyles;
	CXTPControlGalleryItems* m_pItemsStyleSet;
	CXTPControlGalleryItems* m_pItemsThemeColors;
	CXTPControlGalleryItems* m_pItemsThemeFonts;
	
	CXTPControlGalleryItems* m_pItemsFontTextColor;
	CXTPControlGalleryItems* m_pItemsFontBackColor;
	CXTPControlGalleryItems* m_pItemsFontFace;
	CXTPControlGalleryItems* m_pItemsFontSize;
	CXTPControlGalleryItems* m_pItemsUndo;
	CXTPControlGalleryItems* m_pItemsColumns;

	void CreateGalleries();
	void SetProgressBar(int progress);
	int m_nShape;
	int m_nStyle;
	int m_nColumns;

	int m_nStyleSet;
	int m_nThemeColors;
	int m_nThemeFonts;

	BOOL m_bAnimation;
	BOOL m_bLayoutRTL;

	CXTPMessageBar m_wndMessageBar;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CXTPStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;

	BOOL CreateRibbonBar();
	BOOL CreateMiniToolBar();
	BOOL CreateStatusBar();
	BOOL CreateMessageBar();

	void CreateBackstage();

	void OnGalleryColumns(NMHDR* pNMHDR, LRESULT* pResult);
	void OnUpdateGalleryColumns(CCmdUI* pCmdUI);


	void CreateDockingPane();
	

	gkPaneManager m_paneManager;
	CPaneObject m_paneStyles;
	CPaneConsole m_paneConsole;
	CPaneConsoleVar m_paneCVar;

	gkPaneTimeOfDay m_paneTimeOfDay;
	gkMaterialEditor m_paneMatEditor;

	CXTPDockingPane* m_pwndPaneTOD;
	CXTPDockingPane* m_pwndPaneMaterialEditor;
	CXTPDockingPane* m_pwndPaneCVar;

// 	CBackstagePageInfo m_pageInfo;
// 	CBackstagePageHelp m_pageHelp;
// 	CBackstagePageSave m_pageSave;
// 	CBackstagePageRecent m_pageRecent;
// 	CBackstagePagePrint m_pagePrint;


	CString m_csStylesPath;

	gkSwissKnifeBar m_barSwissKnife;

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	afx_msg void OnOptionsAnimation();
	afx_msg void OnUpdateOptionsAnimation(CCmdUI* pCmdUI);
	afx_msg void OnOptionsRighttoleft();
	afx_msg void OnUpdateOptionsRighttoleft(CCmdUI* pCmdUI);
	afx_msg void OnOptionsStyle(UINT);
	afx_msg void OnLanguage(UINT);
	afx_msg void OnUpdateOptionsStyle(CCmdUI* pCmdUI);
	afx_msg void OnToggleGroups();
	afx_msg void OnOptionsFont(UINT nID);
	afx_msg void OnUpdateOptionsFont(CCmdUI* pCmdUI);
	afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	void OnEmptyCommandRange(UINT);
	void OnEmptyCommand();

	void OnFullScreen();
	void OnSwitchCommand();
	void OnUpdateSwitchCommand(CCmdUI* pCmdUI);
	void OnUpdateViewShowHide(CCmdUI* pCmdUI);
	void OnViewShowHide(UINT nID);
	
	void OnRibbonTabChanged(NMHDR* /*pNMHDR*/, LRESULT* /*pRes*/);
	void OnRibbonTabChanging(NMHDR* /*pNMHDR*/, LRESULT* /*pRes*/);

	void OnCustomize();
	void OnCustomizeQuickAccess();
	void ShowCustomizeDialog(int nSelectedPage);
	void OnAutoResizeIcons();
	void OnUpdateAutoResizeIcons(CCmdUI* pCmdUI);
	void OnOptionsSystem();

	BOOL m_bChecked;
	BOOL m_bOptions[6];

	CString m_strIniFileName;
	UINT m_nRibbonStyle;

	void OnGalleryShapes(NMHDR* pNMHDR, LRESULT* pResult);
	void OnUpdateGalleryShapes(CCmdUI* pCmdUI);

	void OnGalleryStyles(NMHDR* pNMHDR, LRESULT* pResult);
	void OnUpdateGalleryStyles(CCmdUI* pCmdUI);

	void OnGalleryStyleSet(NMHDR* pNMHDR, LRESULT* pResult);
	void OnUpdateGalleryStyleSet(CCmdUI* pCmdUI);

	void OnGalleryThemeColors(NMHDR* pNMHDR, LRESULT* pResult);
	void OnUpdateGalleryThemeColors(CCmdUI* pCmdUI);

	void OnGalleryTable(NMHDR* pNMHDR, LRESULT* pResult);
	void OnUpdateGalleryTable(CCmdUI* pCmdUI);

	void OnGalleryThemeFonts(NMHDR* pNMHDR, LRESULT* pResult);
	void OnUpdateGalleryThemeFonts(CCmdUI* pCmdUI);

	int OnCreateControl(LPCREATECONTROLSTRUCT lpCreateControl);

	void OnFrameTheme();
	void OnUpdateFrameTheme(CCmdUI* pCmdUI);

	void OnStatusBarSwitchView(UINT nID);

	void LoadIcons();


	void OnUpdateKeyIndicator(CCmdUI* pCmdUI);

	// GameKnife Regist [8/28/2011 Kaiming-Desktop]
	void OnMovementModeTrans(NMHDR* pNMHDR, LRESULT* pResult);

	// GameKnifeMovement Control [8/28/2011 Kaiming-Desktop]
	CXTPRibbonGroup* m_pGroupMovement;
	uint8 m_uChecked;

	void OnSwitchMovementModeSelect();
	void OnSwitchMovementModeMove();
	void OnSwitchMovementModeRotate();
	void OnSwitchMovementModeScale();
	void OnUpdateSwitchMovementMode(CCmdUI* pCmdUI);

public:
	afx_msg void OnSwitchPanes(UINT id);

	gkPaneManager* getPaneManager() {return &m_paneManager;}

	afx_msg void OnSwitchCam(UINT ID);
	afx_msg void OnSaveCam(UINT ID);
	afx_msg void OnGlobalFocusobj();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__E6D95F91_FCF6_4C13_A74C_E25D863697E5__INCLUDED_)
