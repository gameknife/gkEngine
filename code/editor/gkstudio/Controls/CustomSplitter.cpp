//////////////////////////////////////////////////////////////////////////
//
// K&K Studio GameKnife ENGINE Source File
//
// Name:   	CustomSplitter.cpp
// Desc:		
// 	
// Author:  Kaiming
// Date:	2012/5/13
// Modify:	2012/5/13
// 
//////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "CustomSplitter.h"


// CSplitterWndEx

IMPLEMENT_DYNAMIC(CSplitterWndEx, CSplitterWnd)
	CSplitterWndEx::CSplitterWndEx()
{
	m_cxSplitter = m_cySplitter = 3 + 1 + 1;
	m_cxBorderShare = m_cyBorderShare = 0;
	m_cxSplitterGap = m_cySplitterGap = 3 + 1 + 1;
	m_cxBorder = m_cyBorder = 1;
	m_bTrackable = true;
}

CSplitterWndEx::~CSplitterWndEx()
{
}


BEGIN_MESSAGE_MAP(CSplitterWndEx, CSplitterWnd)
END_MESSAGE_MAP()


//////////////////////////////////////////////////////////////////////////
void CSplitterWndEx::SetNoBorders()
{
	m_cxSplitter = m_cySplitter = 0;
	m_cxBorderShare = m_cyBorderShare = 0;
	m_cxSplitterGap = m_cySplitterGap = 1;
	m_cxBorder = m_cyBorder = 0;
}

//////////////////////////////////////////////////////////////////////////
void CSplitterWndEx::SetTrackable( bool bTrackable )
{
	m_bTrackable = bTrackable;
}

// CSplitterWndEx message handlers

void CSplitterWndEx::SetPane( int row,int col,CWnd *pWnd,SIZE sizeInit )
{
	assert( pWnd != NULL );

	// set the initial size for that pane
	m_pColInfo[col].nIdealSize = sizeInit.cx;
	m_pRowInfo[row].nIdealSize = sizeInit.cy;

	pWnd->ModifyStyle( 0,WS_BORDER,WS_CHILD|WS_VISIBLE );
	pWnd->SetParent(this);

	CRect rect(CPoint(0,0), sizeInit);
	pWnd->MoveWindow( 0,0,sizeInit.cx,sizeInit.cy,FALSE );
	pWnd->SetDlgCtrlID( IdFromRowCol(row,col) );

	ASSERT((int)::GetDlgCtrlID(pWnd->m_hWnd) == IdFromRowCol(row, col));
}

void CSplitterWndEx::OnDrawSplitter(CDC* pDC, ESplitType nType, const CRect& rectArg)
{
	// Let CSplitterWnd handle everything but the border-drawing
	if((nType != splitBorder) || (pDC == NULL))
	{
		CSplitterWnd::OnDrawSplitter(pDC, nType, rectArg);
		return;
	}

	ASSERT_VALID(pDC);

	// Draw border
	pDC->Draw3dRect(rectArg, GetSysColor(COLOR_BTNSHADOW), GetSysColor(COLOR_BTNHIGHLIGHT));
}

//////////////////////////////////////////////////////////////////////////
void CSplitterWndEx::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_bTrackable)
		__super::OnLButtonDown(nFlags, point);
	else
		CWnd::OnLButtonDown(nFlags, point);
}

//////////////////////////////////////////////////////////////////////////
CWnd* CSplitterWndEx::GetActivePane(int* pRow, int* pCol )
{
	return GetFocus();
}

/////////////////////////////////////////////////////////////////////////////
// CSplitterWnd command routing

BOOL CSplitterWndEx::OnCommand(WPARAM wParam, LPARAM lParam)
{
	if (CWnd::OnCommand(wParam, lParam))
		return TRUE;

	// route commands to the splitter to the parent window
	return !GetParent()->SendMessage(WM_COMMAND, wParam, lParam);
}

BOOL CSplitterWndEx::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	if (CWnd::OnNotify(wParam, lParam, pResult))
		return TRUE;

	// route commands to the splitter to the parent window
	*pResult = GetParent()->SendMessage(WM_NOTIFY, wParam, lParam);
	return TRUE;
}
