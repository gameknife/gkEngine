
#include "stdafx.h"
#include "gkToolBarPaneBase.h"
//IMPLEMENT_DYNAMIC(gkMaterialEditor, CXTResizeDialog)

gkToolBarPaneBase::gkToolBarPaneBase(UINT diagID, CWnd* pParent)
: IPaneBase(diagID, pParent)
{
	m_rcBorders.SetRectEmpty();
	m_bInRepositionControls = FALSE;
}

gkToolBarPaneBase::~gkToolBarPaneBase()
{
}

void gkToolBarPaneBase::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(gkToolBarPaneBase, IPaneBase)
	ON_MESSAGE(WM_KICKIDLE, OnKickIdle)
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
END_MESSAGE_MAP()


//////////////////////////////////////////////////////////////////////////
BOOL gkToolBarPaneBase::OnInitDialog()
{
	IPaneBase::OnInitDialog();

	VERIFY(m_paneManager.InstallDockingPanes(this));
	m_paneManager.SetOwner(this);

	m_paneManager.SetAlphaDockingContext(TRUE);
	m_paneManager.SetShowDockingContextStickers(TRUE);
	m_paneManager.SetThemedFloatingFrames(TRUE);
	m_paneManager.SetShowContentsWhileDragging(TRUE);

	m_paneManager.HideClient( true );

	m_paneIDCounter = 0;

	return TRUE;
}

void gkToolBarPaneBase::RefreshMetrics( BOOL bOffice2007 )
{
	m_brush.DeleteObject();

	if (bOffice2007)
	{
		COLORREF clr = XTPResourceImages()->GetImageColor(_T("DockingPane"), _T("WordPaneBackground"));
		m_brush.CreateSolidBrush(clr);
	}
	else
	{
		m_brush.CreateSolidBrush(GetSysColor(COLOR_3DFACE));
	}
	gkLogMessage(_T("Material Editor Refresh."));
}

void gkToolBarPaneBase::InitToolBar( const TCHAR* toolbarname, DWORD nId )
{
	//////////////////////////////////////////////////////////////////////////
	// ToolBar Initilizing
	VERIFY(InitCommandBars());

	CXTPCommandBars* pCommandBars = GetCommandBars();
	pCommandBars->GetImageManager()->SetIcons(nId);
	pCommandBars->SetTheme(xtpThemeWhidbey);
	//pCommandBars->SetQuickCustomizeMode(FALSE);

	//TCHAR toolbarname = 
	CXTPToolBar* pToolBar = pCommandBars->Add(toolbarname, xtpBarTop);
	pToolBar->SetDefaultButtonStyle(xtpButtonIcon);
	pToolBar->LoadToolBar(nId);

 	pToolBar->SetCloseable(FALSE);
 	pToolBar->EnableCustomization();

	pToolBar->GetControls()->CreateOriginalControls();
	pCommandBars->GetCommandBarsOptions()->ShowKeyboardCues(xtpKeyboardCuesShowWindowsDefault);
	pCommandBars->GetToolTipContext()->SetStyle(xtpToolTipOffice2007);

	RecalcLayout();
	//////////////////////////////////////////////////////////////////////////
}

void gkToolBarPaneBase::RecalcLayout()
{
	////////////////////////////////////////////////////////////////////////
	// Place the toolbars and move the controls in the dialog to make space
	// for them
	////////////////////////////////////////////////////////////////////////

	CRect rcClientStart;
	CRect rcClientNow;
	CRect rcChild;					
	CRect rcWindow;
	CWnd *pwndChild = GetWindow(GW_CHILD);
	if (pwndChild == NULL)
		return;

	CRect clientRect;
	GetClientRect(clientRect);
	// We need to resize the dialog to make room for control bars.
	// First, figure out how big the control bars are.
	GetClientRect(rcClientStart);
	RepositionBarsInternal(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 
				   0, reposQuery, rcClientNow);

	// Now move all the controls so they are in the same relative
	// position within the remaining client area as they would be
	// with no control bars.
	CPoint ptOffset(rcClientNow.left - rcClientStart.left,
					rcClientNow.top - rcClientStart.top); 

	//CRect rcClient(0, 0, cx, cy);
	RepositionBars(0, 0xffff, AFX_IDW_PANE_FIRST, 0, 0, &rcClientNow);

	// And position the control bars
	RepositionBarsInternal(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);
}

void gkToolBarPaneBase::RepositionBarsInternal(UINT nIDFirst, UINT nIDLast, UINT nIDLeftOver,
													UINT nFlags, LPRECT lpRectParam, LPCRECT lpRectClient, BOOL bStretch)
{
	ASSERT(nFlags == 0 || (nFlags & ~reposNoPosLeftOver) == reposQuery || 
		(nFlags & ~reposNoPosLeftOver) == reposExtra);

	// walk kids in order, control bars get the resize notification
	//   which allow them to shrink the client area
	// remaining size goes to the 'nIDLeftOver' pane
	// NOTE: nIDFirst->nIDLast are usually 0->0xffff

	AFX_SIZEPARENTPARAMS layout;
	HWND hWndLeftOver = NULL;

	layout.bStretch = bStretch;
	layout.sizeTotal.cx = layout.sizeTotal.cy = 0;
	if (lpRectClient != NULL)
		layout.rect = *lpRectClient;    // starting rect comes from parameter
	else
		GetClientRect(&layout.rect);    // starting rect comes from client rect

	if ((nFlags & ~reposNoPosLeftOver) != reposQuery)
		layout.hDWP = ::BeginDeferWindowPos(8); // reasonable guess
	else
		layout.hDWP = NULL; // not actually doing layout

	for (HWND hWndChild = ::GetTopWindow(m_hWnd); hWndChild != NULL;
		hWndChild = ::GetNextWindow(hWndChild, GW_HWNDNEXT))
	{
		UINT_PTR nIDC = ((UINT)(WORD)::GetDlgCtrlID(hWndChild));
		CWnd* pWnd = CWnd::FromHandlePermanent(hWndChild);
		if (nIDC == nIDLeftOver)
			hWndLeftOver = hWndChild;
		else if (nIDC >= nIDFirst && nIDC <= nIDLast && pWnd != NULL)
			::SendMessage(hWndChild, WM_SIZEPARENT, 0, (LPARAM)&layout);
	}

	// if just getting the available rectangle, return it now...
	if ((nFlags & ~reposNoPosLeftOver) == reposQuery)
	{
		ASSERT(lpRectParam != NULL);
		if (bStretch)
			::CopyRect(lpRectParam, &layout.rect);
		else
		{
			lpRectParam->left = lpRectParam->top = 0;
			lpRectParam->right = layout.sizeTotal.cx;
			lpRectParam->bottom = layout.sizeTotal.cy;
		}
		return;
	}

	// the rest is the client size of the left-over pane
	if (nIDLeftOver != 0 && hWndLeftOver != NULL)
	{
		CWnd* pLeftOver = CWnd::FromHandle(hWndLeftOver);
		// allow extra space as specified by lpRectBorder
		if ((nFlags & ~reposNoPosLeftOver) == reposExtra)
		{
			ASSERT(lpRectParam != NULL);
			layout.rect.left += lpRectParam->left;
			layout.rect.top += lpRectParam->top;
			layout.rect.right -= lpRectParam->right;
			layout.rect.bottom -= lpRectParam->bottom;
		}
		// reposition the window
		if ((nFlags & reposNoPosLeftOver) != reposNoPosLeftOver)
		{
			pLeftOver->CalcWindowRect(&layout.rect);
			RepositionWindowInternal(&layout, hWndLeftOver, &layout.rect);
		}
	}

	// move and resize all the windows at once!
	if (layout.hDWP == NULL || !::EndDeferWindowPos(layout.hDWP))
		TRACE(traceAppMsg, 0, "Warning: DeferWindowPos failed - low system resources.\n");
}

//////////////////////////////////////////////////////////////////////////
void gkToolBarPaneBase::RepositionWindowInternal(AFX_SIZEPARENTPARAMS* lpLayout,HWND hWnd, LPCRECT lpRect)
{
	ASSERT(hWnd != NULL);
	ASSERT(lpRect != NULL);
	HWND hWndParent = ::GetParent(hWnd);
	ASSERT(hWndParent != NULL);

	if (lpLayout != NULL && lpLayout->hDWP == NULL)
		return;

	// first check if the new rectangle is the same as the current
	CRect rectOld;
	::GetWindowRect(hWnd, rectOld);
	::ScreenToClient(hWndParent, &rectOld.TopLeft());
	::ScreenToClient(hWndParent, &rectOld.BottomRight());
	if (::EqualRect(rectOld, lpRect))
		return;     // nothing to do

	// try to use DeferWindowPos for speed, otherwise use SetWindowPos
	if (lpLayout != NULL)
	{
		lpLayout->hDWP = ::DeferWindowPos(lpLayout->hDWP, hWnd, NULL,
			lpRect->left, lpRect->top,  lpRect->right - lpRect->left,
			lpRect->bottom - lpRect->top, SWP_NOACTIVATE|SWP_NOZORDER);
	}
	else
	{
		::SetWindowPos(hWnd, NULL, lpRect->left, lpRect->top,
			lpRect->right - lpRect->left, lpRect->bottom - lpRect->top,
			SWP_NOACTIVATE|SWP_NOZORDER);
	}
}
LRESULT gkToolBarPaneBase::OnKickIdle(WPARAM, LPARAM)
{
	m_paneManager.UpdatePanes();

	if (GetCommandBars()) GetCommandBars()->UpdateCommandBars();
	return 0;
}
//////////////////////////////////////////////////////////////////////////
BOOL gkToolBarPaneBase::OnEraseBkgnd( CDC* pDC )
{
	BOOL ret = IPaneBase::OnEraseBkgnd(pDC);
	// TODO: Add your message handler code here and/or call default
//  	CRect rc;
//  	GetClientRect(&rc);
//  	pDC->FillSolidRect(&rc,RGB(25,99,161));
	return ret;
}

void gkToolBarPaneBase::OnSize(UINT nType, int cx, int cy) 
{
	IPaneBase::OnSize(nType, cx, cy);
	RecalcLayout();
}
//////////////////////////////////////////////////////////////////////////
void gkToolBarPaneBase::RegisterPane( CWnd* pCtrl, const TCHAR* name, XTPDockingPaneDirection dockdir, CSize constrain, int dockoption )
{
 	NameIDMap::iterator it = m_paneMap.find(name);
 	if (it == m_paneMap.end())
 	{
 		CXTPDockingPane* pane = m_paneManager.CreatePane( ++m_paneIDCounter, CRect(0, 0, 100, 120), dockdir );
 		pane->SetTitle(name);
 		pane->SetOptions(dockoption);
 		pane->Attach( pCtrl );
 		m_paneManager.ShowPane(pane);

		//pane->SetMinTrackSize(constrain);
		pane->SetMaxTrackSize(constrain);
 
 		m_paneMap[name] = m_paneIDCounter;
 	}
}
//////////////////////////////////////////////////////////////////////////
void gkToolBarPaneBase::OpenPane( const TCHAR* name )
{

}
//////////////////////////////////////////////////////////////////////////
void gkToolBarPaneBase::FloatPane( const TCHAR* name )
{

}
//////////////////////////////////////////////////////////////////////////
void gkToolBarPaneBase::ClosePane( const TCHAR* name )
{

}


