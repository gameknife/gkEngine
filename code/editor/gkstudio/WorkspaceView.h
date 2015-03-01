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



#if !defined(AFX_WORKSPACEVIEW_H__8B9758DA_BD65_4E68_BC3C_8C98B2419838__INCLUDED_)
#define AFX_WORKSPACEVIEW_H__8B9758DA_BD65_4E68_BC3C_8C98B2419838__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WorkspaceView.h : header file
//

#include "WorkspaceTitle.h"

class CRibbonSampleView;
class IgkEntity;
/////////////////////////////////////////////////////////////////////////////
// CWorkspaceView view

class CWorkspaceView : public CView
{
protected:
	CWorkspaceView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CWorkspaceView)

// Attributes
public:

// Operations
public:
	void SetLayoutRTL(BOOL bRTLLayout);
	void Reposition(CSize sz);

// gkOperations
	void Update();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWorkspaceView)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CWorkspaceView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:
	//CXTPCommandBarScrollBarCtrl m_wndScrollBar[2];
	CRect GetViewRect(CRect rc);
	Vec2 getCursorOnClientScreen();
	bool isCursorInView();
	Ray getRayFronScreen();
	Ray getRayFronScreenOffset(const Vec2i& pt);
	bool getScreenPosFromScene(const Vec3& point, Vec2& out );
	POINT getPtClientToScreen(POINT pt);

protected:
	CRibbonSampleView* m_pView;

	static bool ms_mousestate;

	CWorkspaceTitle m_titleBar;


	// Generated message map functions
protected:
	//{{AFX_MSG(CWorkspaceView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);


	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WORKSPACEVIEW_H__8B9758DA_BD65_4E68_BC3C_8C98B2419838__INCLUDED_)
