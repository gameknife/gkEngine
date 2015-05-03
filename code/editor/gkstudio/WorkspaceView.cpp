// WorkspaceView.cpp : implementation file
//

#include "stdafx.h"
#include "ribbonsample.h"
#include "WorkspaceView.h"

#include "RibbonSampleView.h"
#include "MainFrm.h"

#include "gk_Math.h"
#include "gk_Geo.h"

#include "I3DEngine.h"
#include "ICamera.h"
#include "IAuxRenderer.h"
#include "IInputManager.h"
#include "Managers\gkObjectManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

bool	  CWorkspaceView::ms_mousestate = false;

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceView

IMPLEMENT_DYNCREATE(CWorkspaceView, CView)

CWorkspaceView::CWorkspaceView()
{
	m_pView = 0;
}

CWorkspaceView::~CWorkspaceView()
{
}


BEGIN_MESSAGE_MAP(CWorkspaceView, CView)
	//{{AFX_MSG_MAP(CWorkspaceView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_VSCROLL()
	ON_WM_MOUSEACTIVATE()
	//}}AFX_MSG_MAP
	
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceView drawing

void CWorkspaceView::OnDraw(CDC* /*pDC*/)
{

}

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceView diagnostics

#ifdef _DEBUG
void CWorkspaceView::AssertValid() const
{
	CView::AssertValid();
}

void CWorkspaceView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceView message handlers

int CWorkspaceView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	ModifyStyleEx(WS_EX_CLIENTEDGE, 0);
	
	CCreateContext contextT;
	contextT.m_pCurrentDoc     = GetDocument();
	contextT.m_pNewViewClass   = RUNTIME_CLASS(CRibbonSampleView);
	contextT.m_pNewDocTemplate = GetDocument()->GetDocTemplate();

	TRY
	{
		m_pView = (CRibbonSampleView*)contextT.m_pNewViewClass->CreateObject();
		if (m_pView == NULL)
		{
			AfxThrowMemoryException();
		}
	}
	CATCH_ALL(e)
	{
		TRACE0( "Out of memory creating a view.\n" );
		// Note: DELETE_EXCEPTION(e) not required
		return FALSE;
	}
	END_CATCH_ALL

	DWORD dwStyle = AFX_WS_DEFAULT_VIEW;
	dwStyle &= ~WS_BORDER;

	// Create with the right size (wrong position)
	CRect rect(0,0,0,0);
	if (!m_pView->Create(NULL, NULL, dwStyle,
		rect, this, AFX_IDW_PANE_FIRST, &contextT))
	{
		TRACE0( "Warning: couldn't create client tab for view.\n" );
		// pWnd will be cleaned up by PostNcDestroy
		return NULL;
	}

	m_pView->ModifyStyleEx(WS_EX_CLIENTEDGE, 0, SWP_FRAMECHANGED);
	m_pView->ModifyStyle(0, WS_BORDER, SWP_FRAMECHANGED);

	m_pView->SetOwner(this);

	GetParentFrame()->SetActiveView(m_pView);

	m_titleBar.Create(CWorkspaceTitle::IDD, m_pView);
	m_titleBar.ShowWindow(TRUE);
	m_titleBar.UpdateWindow();

	return 0;
}

void CWorkspaceView::Reposition(CSize sz)
{
	if (m_pView)
	{
		CRect rc(GetViewRect(CRect(0, 0, sz.cx, sz.cy)));
		m_pView->MoveWindow(rc);

		m_titleBar.SetWindowPos(NULL, -1, -1, sz.cx, 24, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
		m_titleBar.UpdateWindow();	
	}


}

void CWorkspaceView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);

	Reposition(CSize(cx, cy));
}

CRect CWorkspaceView::GetViewRect(CRect rc)
{
	m_pView->CalcWindowRect(&rc, 0);
	
	return rc;
}


BOOL CWorkspaceView::OnEraseBkgnd(CDC* /*pDC*/) 
{
	return TRUE;
}

void CWorkspaceView::OnPaint() 
{
	CPaintDC dc(this);

	CXTPClientRect rc(this);
	
	CXTPWindowRect rcClient(GetDlgItem(AFX_IDW_PANE_FIRST));
	ScreenToClient(&rcClient);
	
	CXTPCommandBars* pCommandBars = ((CMainFrame*)GetParentFrame())->GetCommandBars();
	((CXTPOffice2007Theme*)(pCommandBars->GetPaintManager()))->FillWorkspace(&dc, rc, rcClient);

	gEnv->pSystem->SetEditorHWND(GetSafeHwnd(), GetSafeHwnd());
	gEnv->pRenderer->SetCurrContent(GetSafeHwnd(), 0, 24, rc.Width(), rc.Height() - 24);
}

void CWorkspaceView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	//m_pView->OnVScroll(nSBCode, nPos, pScrollBar);
}

BOOL CWorkspaceView::PreCreateWindow(CREATESTRUCT& cs) 
{
	return CView::PreCreateWindow(cs);
}

void CWorkspaceView::SetLayoutRTL(BOOL bRTLLayout)
{
	ModifyStyleEx(bRTLLayout ? 0 : WS_EX_LAYOUTRTL, !bRTLLayout ? 0 : WS_EX_LAYOUTRTL);

	Reposition(CXTPClientRect(this).Size());
	Invalidate(FALSE);
}

int CWorkspaceView::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message) 
{
	// Don't call CView::OnMouseActivate.
	return CView::OnMouseActivate(pDesktopWnd, nHitTest, message);
}
//-----------------------------------------------------------------------
void CWorkspaceView::OnLButtonDown( UINT nFlags, CPoint point )
{
	CView::OnLButtonDown(nFlags, point);
}
//-----------------------------------------------------------------------
void CWorkspaceView::Update()
{

}
//-----------------------------------------------------------------------
Vec2 CWorkspaceView::getCursorOnClientScreen()
{
	POINT point;
	GetCursorPos( &point );
	ScreenToClient( &point );	

	CXTPClientRect rc(this);
	Vec2 v(point.x, point.y - 24);

	return v;
}

POINT CWorkspaceView::getPtClientToScreen(POINT pt)
{
	ClientToScreen(&pt);
	pt.y += 18;
	return pt;
}
//-----------------------------------------------------------------------
Ray CWorkspaceView::getRayFronScreen()
{
	POINT point;
	GetCursorPos( &point );
	ScreenToClient( &point );
	return gEnv->pRenderer->GetRayFromScreen( point.x, point.y - 24 );
}


Ray CWorkspaceView::getRayFronScreenOffset( const Vec2i& pt )
{
	POINT point;
	GetCursorPos( &point );
	ScreenToClient( &point );

	point.x += pt.x;
	point.y += pt.y;
	point.y -= 24;

// 	clamp_tpl( point.x, 600l, point.x );
// 	clamp_tpl( point.y, 600l, point.y );

	return gEnv->pRenderer->GetRayFromScreen( point.x, point.y );
}


bool CWorkspaceView::isCursorInView()
{
	POINT point;
	GetCursorPos( &point );
	ScreenToClient( &point );

	RECT rc;
	GetClientRect(&rc);

	if (point.x > rc.left && point.x < rc.right && point.y > rc.top && point.y < rc.bottom)
		return true;

	return false;
}
//////////////////////////////////////////////////////////////////////////
bool CWorkspaceView::getScreenPosFromScene( const Vec3& point, Vec2& out )
{
	Vec3 pos = gEnv->pRenderer->ProjectScreenPos( point );
	out = Vec2(pos.x, pos.y);
	return pos.z > 0;
}


