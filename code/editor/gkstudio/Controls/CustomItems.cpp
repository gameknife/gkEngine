// Custom Items.cpp : implementation file
//
// This file is a part of the XTREME TOOLKIT PRO MFC class library.
// (c)1998-2009 Codejock Software, All Rights Reserved.
//
// THIS SOURCE FILE IS THE PROPERTY OF CODEJOCK SOFTWARE AND IS NOT TO BE
// RE-DISTRIBUTED BY ANY MEANS WHATSOEVER WITHOUT THE EXPRESSED WRITTEN
// CONSENT OF CODEJOCK SOFTWARE.
//
// THIS SOURCE CODE CAN ONLY BE USED UNDER THE TERMS AND CONDITIONS OUTLINED
// IN THE XTREME TOOLKIT PRO LICENSE AGREEMENT. CODEJOCK SOFTWARE GRANTS TO
// YOU (ONE SOFTWARE DEVELOPER) THE LIMITED RIGHT TO USE THIS SOFTWARE ON A
// SINGLE COMPUTER.
//
// CONTACT INFORMATION:
// support@codejock.com
// http://www.codejock.com
//
/////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "CustomItems.h"
#include "PSCPColorDialog.h"


////////////////////////////////////////////////////////////////////////////////////////////////

CCustomItemIcon::CCustomItemIcon(CString strCaption, HICON hIcon)
	: CXTPPropertyGridItem(strCaption)
{
	m_hIcon = hIcon? CopyIcon(hIcon): 0;
	m_nFlags = xtpGridItemHasExpandButton;
}

CCustomItemIcon::~CCustomItemIcon(void)
{
	if (m_hIcon)
		DestroyIcon(m_hIcon);
}

BOOL CCustomItemIcon::OnDrawItemValue(CDC& dc, CRect rcValue)
{
	if (m_hIcon)
	{
		COLORREF clr = dc.GetTextColor();
		CRect rcSample(rcValue.left - 2, rcValue.top + 1, rcValue.left + 18, rcValue.bottom - 1);
		DrawIconEx(dc, rcSample.left, rcSample.top, m_hIcon, rcSample.Width(), rcSample.Height(), 0, 0, DI_NORMAL);
		dc.Draw3dRect(rcSample, clr, clr);
	}

	CRect rcText(rcValue);
	rcText.left += 25;

	dc.DrawText( _T("(Icon)"), rcText,  DT_SINGLELINE|DT_VCENTER);

	return TRUE;
}

void CCustomItemIcon::OnInplaceButtonDown(CXTPPropertyGridInplaceButton* /*pButton*/)
{
	const TCHAR szFilters[]=
		_T("Icon files (*.ico)|*.ico|All Files (*.*)|*.*||");

	CFileDialog dlg(TRUE, _T("ico"), _T("*.ico"), OFN_FILEMUSTEXIST| OFN_HIDEREADONLY, szFilters);

	if (dlg.DoModal() == IDOK)
	{
		if (m_hIcon)
			DestroyIcon(m_hIcon);
		m_hIcon = (HICON)LoadImage(NULL, dlg.GetPathName(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE|LR_DEFAULTSIZE );


		OnValueChanged(_T(""));
		((CWnd*)m_pGrid)->Invalidate(FALSE);
	}
}



////////////////////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CCustomItemSpinInplaceButton, CSpinButtonCtrl)
	ON_NOTIFY_REFLECT(UDN_DELTAPOS, OnDeltapos)
END_MESSAGE_MAP()


void CCustomItemSpinInplaceButton::OnDeltapos(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

	m_pItem->OnValidateEdit();
	long nValue = m_pItem->GetNumber() + pNMUpDown->iDelta;

	int nLower, nUpper;
	GetRange(nLower, nUpper);
	nValue = max(nLower, min(nUpper, nValue));

	CString str;
	str.Format(_T("%i"), nValue);
	m_pItem->OnValueChanged(str);

	*pResult = 1;
}

CCustomItemSpin::CCustomItemSpin(CString strCaption)
	: CXTPPropertyGridItemNumber(strCaption)
{
	m_wndSpin.m_pItem = this;
}
void CCustomItemSpin::OnDeselect()
{
	CXTPPropertyGridItemNumber::OnDeselect();

	if (m_wndSpin.m_hWnd) m_wndSpin.ShowWindow(SW_HIDE);
}

void CCustomItemSpin::OnSelect()
{
	CXTPPropertyGridItem::OnSelect();

	if (!m_bReadOnly)
	{
		CRect rc = GetItemRect();
		rc.left = rc.right - 15;
		if (!m_wndSpin.m_hWnd)
		{
			m_wndSpin.Create(UDS_ARROWKEYS|WS_CHILD, rc, (CWnd*)m_pGrid, 0);
			m_wndSpin.SetRange(0, 100);
		}
		m_wndSpin.MoveWindow(rc);
		m_wndSpin.ShowWindow(SW_SHOW);
	}
}
CRect CCustomItemSpin::GetValueRect()
{
	CRect rcValue(CXTPPropertyGridItem::GetValueRect());
	rcValue.right -= 17;
	return rcValue;
}


////////////////////////////////////////////////////////////////////////////////////////////////

class CCustomItemChilds::CCustomItemChildsAll : public CXTPPropertyGridItemNumber
{
public:
	CCustomItemChildsAll(CString strCaption) : CXTPPropertyGridItemNumber(strCaption) {}
	virtual void OnValueChanged(CString strValue)
	{
		SetValue(strValue);

		CCustomItemChilds* pParent = ((CCustomItemChilds*)m_pParent);
		CRect& rc = pParent->m_rcValue;
		rc.left = rc.right = rc.top = rc.bottom = GetNumber();
		pParent->OnValueChanged(pParent->RectToString(rc));
	}
};

class CCustomItemChilds::CCustomItemChildsPad : public CXTPPropertyGridItemNumber
{
public:
	CCustomItemChildsPad(CString strCaption, LONG& nPad) : CXTPPropertyGridItemNumber(strCaption), m_nPad(nPad) {}
	virtual void OnValueChanged(CString strValue)
	{
		SetValue(strValue);

		CCustomItemChilds* pParent = ((CCustomItemChilds*)m_pParent);
		m_nPad = GetNumber();
		pParent->m_itemAll->SetNumber(0);
		pParent->OnValueChanged(pParent->RectToString(pParent->m_rcValue));

	}
	LONG& m_nPad;
};

CCustomItemChilds::CCustomItemChilds(CString strCaption, CRect rcValue)
	: CXTPPropertyGridItem(strCaption)
{
	m_rcValue = rcValue;
	m_strValue = RectToString(rcValue);
	m_nFlags = 0;
}

void CCustomItemChilds::OnAddChildItem()
{
	m_itemAll = (CCustomItemChildsAll*)AddChildItem(new CCustomItemChildsAll(_T("All")));
	m_itemLeft = (CCustomItemChildsPad*)AddChildItem(new CCustomItemChildsPad(_T("Left"), m_rcValue.left));
	m_itemTop = (CCustomItemChildsPad*)AddChildItem(new CCustomItemChildsPad(_T("Top"), m_rcValue.top));
	m_itemRight = (CCustomItemChildsPad*)AddChildItem(new CCustomItemChildsPad(_T("Right"), m_rcValue.right));
	m_itemBottom = (CCustomItemChildsPad*)AddChildItem(new CCustomItemChildsPad(_T("Bottom"), m_rcValue.bottom));

	UpdateChilds();
}

void CCustomItemChilds::UpdateChilds()
{
	m_itemLeft->SetNumber(m_rcValue.left);
	m_itemRight->SetNumber(m_rcValue.right);
	m_itemTop->SetNumber(m_rcValue.top);
	m_itemBottom->SetNumber(m_rcValue.bottom);
}

void CCustomItemChilds::SetValue(CString strValue)
{
	CXTPPropertyGridItem::SetValue(strValue);
	UpdateChilds();
}

CString CCustomItemChilds::RectToString(CRect rc)
{
	CString str;
	str.Format(_T("%i; %i; %i; %i"), rc.left, rc.top, rc.right, rc.bottom);
	return str;
}


///////////////////////////////////////////////////////////////////////////////


class CCustomItemColorPopup: public CXTColorPopup
{
	friend class CCustomItemColor;
public:
	CCustomItemColorPopup() : CXTColorPopup(TRUE) {}
private:
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnSelEndOK(WPARAM wParam, LPARAM lParam);

	CCustomItemColor* m_pItem;
};

BEGIN_MESSAGE_MAP(CCustomItemColorPopup, CXTColorPopup)
	ON_MESSAGE(CPN_XT_SELENDOK, OnSelEndOK)
END_MESSAGE_MAP()


LRESULT CCustomItemColorPopup::OnSelEndOK(WPARAM wParam, LPARAM /*lParam*/)
{
	m_pItem->OnValueChanged(m_pItem->RGBToString((COLORREF)wParam));
	return 0;
}


CCustomItemColor::CCustomItemColor(CString strCaption, IPSCPColorChangeCallback* listener, COLORREF clr)
	: CXTPPropertyGridItemColor(strCaption, clr)
	, m_colorf(0)
	, m_listener(listener)
{
	m_nFlags = xtpGridItemHasComboButton|xtpGridItemHasEdit;
	SetColor(clr);
	m_strDefaultValue = m_strValue;
}
//////////////////////////////////////////////////////////////////////////
void CCustomItemColor::OnBeforeInsert()
{
	if (m_pBindColor && *m_pBindColor != m_clrValue)
	{
		SetColor(*m_pBindColor);
	}

	if (m_colorf)
	{
		SetColor(m_colorf);
	}

	
}

void CCustomItemColor::OnInplaceButtonDown( CXTPPropertyGridInplaceButton* pButton )
{
	CPSCPColorDialog dlg(m_clrValue, m_clrValue, CPS_XTP_SHOW3DSELECTION|CPS_XTP_SHOWEYEDROPPER, m_pGrid);
	dlg.SetTitle(_T("拾色器"));

	dlg.addListener( m_listener );
	dlg.addListener( this );

	COLORREF backup = m_clrValue;

	if (dlg.DoModal() == IDOK)
	{
		CString strValue = RGBToString(dlg.GetColor());

		if (OnAfterEdit(strValue))
		{
			OnValueChanged(strValue);
			//SAFE_INVALIDATE(pGrid);
		}
	}
	else
	{
		OnCancelEdit();

		CString strValue = RGBToString(backup);

		OnValueChanged(strValue);
	}
}

void CCustomItemColor::OnColorChange( COLORREF clr )
{
	CString strValue = RGBToString(clr);
	SetColor(clr);
	OnValueChanged(strValue);
}


// void CCustomItemColor::OnInplaceButtonDown(CXTPPropertyGridInplaceButton* /*pButton*/)
// {
// 	CCustomItemColorPopup *pColorPopup = new CCustomItemColorPopup();
// 	pColorPopup->SetTheme(xtpControlThemeOffice2003);
// 
// 	CRect rcItem= GetItemRect();
// 	m_pGrid->ClientToScreen(&rcItem);
// 
// 	pColorPopup->Create(rcItem, m_pGrid, CPS_XTP_RIGHTALIGN|CPS_XTP_USERCOLORS|CPS_XTP_EXTENDED|CPS_XTP_MORECOLORS|CPS_XTP_SHOW3DSELECTION|CPS_XTP_SHOWHEXVALUE, GetColor(), GetColor());
// 	pColorPopup->SetOwner(m_pGrid);
// 	pColorPopup->SetFocus();
// 	pColorPopup->AddListener(pColorPopup->GetSafeHwnd());
// 	pColorPopup->m_pItem = this;
// 
// 
// }


///////////////////////////////////////////////////////////////////////////////

CCustomItemFileBox::CCustomItemFileBox(CString strCaption)
	: CXTPPropertyGridItem(strCaption)
{
	m_nFlags = xtpGridItemHasExpandButton|xtpGridItemHasEdit;
}
void CCustomItemFileBox::OnInplaceButtonDown(CXTPPropertyGridInplaceButton* /*pButton*/)
{
	CFileDialog dlg( TRUE, NULL,  GetValue(), OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, NULL, m_pGrid);
	if ( dlg.DoModal( ) == IDOK )
	{
		CString filepath = dlg.GetPathName();

		// chop to engine path
		//gkStdString relpath = gkGetGameRelativePath( filepath.GetBuffer() );
		gkStdString relpath = gkGetExecRelativePath( filepath.GetBuffer() );

		if( !gkIsEnginePath( relpath ) )
		{
			relpath = gkGetGameRelativePath( filepath.GetBuffer() );
		}

		OnValueChanged( relpath.c_str() );
		m_pGrid->Invalidate( FALSE );
	}
};

///////////////////////////////////////////////////////////////////////////////

CCustomItemFilename::CCustomItemFilename(CString strCaption)
	: CXTPPropertyGridItem(strCaption)
{
	m_nFlags = xtpGridItemHasExpandButton|xtpGridItemHasEdit;
}
void CCustomItemFilename::OnInplaceButtonDown(CXTPPropertyGridInplaceButton* /*pButton*/)
{
	CFileDialog dlg( TRUE, NULL,  GetValue(), OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, NULL, m_pGrid);
	if ( dlg.DoModal( ) == IDOK )
	{
		gkStdString fullname( dlg.GetPathName() );

		fullname = gkGetGameRelativePath( fullname );
		gkNormalizeResName( fullname );

		OnValueChanged( fullname.c_str() );

		// do something


		m_pGrid->Invalidate( FALSE );
	}
};
///////////////////////////////////////////////////////////////////////////////


BEGIN_MESSAGE_MAP(CInplaceCheckBox, CButton)
	ON_MESSAGE(BM_SETCHECK, OnCheck)
	ON_WM_CTLCOLOR_REFLECT()
END_MESSAGE_MAP()

HBRUSH CInplaceCheckBox::CtlColor(CDC* pDC, UINT /*nCtlColor*/)
{
	class CGridView : public CXTPPropertyGridView
	{
		friend class CInplaceCheckBox;
	};

	CGridView* pGrid = (CGridView*)m_pItem->m_pGrid;

	COLORREF clr = pGrid->GetPaintManager()->GetItemMetrics()->m_clrBack;

	if (clr != m_clrBack || !m_brBack.GetSafeHandle())
	{
		m_brBack.DeleteObject();
		m_brBack.CreateSolidBrush(clr);
		m_clrBack = clr;
	}

	pDC->SetBkColor(m_clrBack);
	return m_brBack;
}

LRESULT CInplaceCheckBox::OnCheck(WPARAM wParam, LPARAM lParam)
{
	m_pItem->m_bValue = (wParam == BST_CHECKED);
	m_pItem->OnValueChanged(m_pItem->GetValue());

	return CButton::DefWindowProc(BM_SETCHECK, wParam, lParam);
}

CCustomItemCheckBox::CCustomItemCheckBox(CString strCaption)
	: CXTPPropertyGridItem(strCaption)
{
	m_wndCheckBox.m_pItem = this;
	m_nFlags = 0;
	m_bValue = FALSE;
}

void CCustomItemCheckBox::OnDeselect()
{
	CXTPPropertyGridItem::OnDeselect();

	if (m_wndCheckBox.m_hWnd) m_wndCheckBox.DestroyWindow();
}

void CCustomItemCheckBox::OnSelect()
{
	CXTPPropertyGridItem::OnSelect();

	if (!m_bReadOnly)
	{
		CRect rc = GetValueRect();
		rc.left -= 15;
		rc.right = rc.left + 15;

		if (!m_wndCheckBox.m_hWnd)
		{
			m_wndCheckBox.Create(NULL, WS_CHILD|BS_AUTOCHECKBOX|BS_FLAT, rc, (CWnd*)m_pGrid, 0);

		}
		if (m_wndCheckBox.GetCheck() != m_bValue) m_wndCheckBox.SetCheck(m_bValue);
		m_wndCheckBox.MoveWindow(rc);
		m_wndCheckBox.ShowWindow(SW_SHOW);
	}
}

CRect CCustomItemCheckBox::GetValueRect()
{
	CRect rcValue(CXTPPropertyGridItem::GetValueRect());
	rcValue.left += 17;
	return rcValue;
}

BOOL CCustomItemCheckBox::OnDrawItemValue(CDC& dc, CRect rcValue)
{
	CRect rcText(rcValue);

	if (m_wndCheckBox.GetSafeHwnd() == 0 && m_bValue)
	{
		CRect rcCheck(rcText.left , rcText.top, rcText.left + 13, rcText.bottom -1);
		dc.DrawFrameControl(rcCheck, DFC_MENU, DFCS_MENUCHECK);
	}

	rcText.left += 17;
	dc.DrawText( GetValue(), rcText,  DT_SINGLELINE|DT_VCENTER);
	return TRUE;
}


BOOL CCustomItemCheckBox::GetBool()
{
	return m_bValue;
}
void CCustomItemCheckBox::SetBool(BOOL bValue)
{
	m_bValue = bValue;

	if (m_wndCheckBox.GetSafeHwnd())
		m_wndCheckBox.SetCheck(bValue);
}

BOOL CCustomItemCheckBox::IsValueChanged()
{
	return !m_bValue;
}



IMPLEMENT_DYNAMIC(CInplaceUpperCase, CXTPPropertyGridInplaceEdit)

BEGIN_MESSAGE_MAP(CInplaceUpperCase, CXTPPropertyGridInplaceEdit)
	//{{AFX_MSG_MAP(CXTPPropertyGridInplaceEdit)
	//}}AFX_MSG_MAP
	ON_WM_CHAR()
END_MESSAGE_MAP()

void CInplaceUpperCase::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CString strChar((TCHAR)nChar), strUpper((TCHAR)nChar);
	strUpper.MakeUpper();

	if (strChar != strUpper) ReplaceSel(strUpper, TRUE);
	else CXTPPropertyGridInplaceEdit::OnChar(nChar, nRepCnt, nFlags);
}


////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////


BEGIN_MESSAGE_MAP(CInplaceSlider, CSliderCtrl)
	ON_WM_CTLCOLOR_REFLECT()
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

HBRUSH CInplaceSlider::CtlColor(CDC* pDC, UINT /*nCtlColor*/)
{
	class CGridView : public CXTPPropertyGridView
	{
		friend class CInplaceSlider;
	};

	CGridView* pGrid = (CGridView*)m_pItem->m_pGrid;

	COLORREF clr = pGrid->GetPaintManager()->GetItemMetrics()->m_clrBack;

	if (clr != m_clrBack || !m_brBack.GetSafeHandle())
	{
		m_brBack.DeleteObject();
		m_brBack.CreateSolidBrush(clr);
		m_clrBack = clr;
	}

	m_clrFore = XTPColorManager()->GetColor(COLOR_HIGHLIGHT );
	m_clrGrey = XTPColorManager()->GetColor(COLOR_BTNTEXT );
	
	pDC->SetBkColor(m_clrBack);
	return m_brBack;
}

void CInplaceSlider::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMCUSTOMDRAW lpCustDraw = (LPNMCUSTOMDRAW)pNMHDR;
// 	if(lpCustDraw->dwDrawStage == CDDS_PREPAINT)
// 	{
// 		int nValue = GetPos();
// 		if (nValue != m_nValue)
// 		{
// 			m_nValue = nValue;
// 
// 			// maybe we should set as double [2011/10/30 Kaiming-Laptop]
// 			float realValue = (float)nValue / m_floatScale;
// 			if( fabsf(m_pItem->GetDouble() - realValue) > 0.001f )
// 			{
// 				m_pItem->SetDouble(realValue);
// 				m_pItem->OnValueChanged(m_pItem->GetValue());
// 				m_pItem->GetGrid()->Invalidate(FALSE);
// 			}
// 
// 		}
// 	}

	*pResult = CDRF_DODEFAULT;
}

BOOL CInplaceSlider::OnEraseBkgnd(CDC* /*pDC*/)
{
	return TRUE;
}

void CInplaceSlider::OnPaint()
{
	//CWnd::OnPaint();

	CPaintDC dc(this); // device context for painting

	CRect rc;
	GetClientRect(&rc);
	CXTPBufferDC memDC(dc);

	HBRUSH hBrush = (HBRUSH)GetParent()->SendMessage(WM_CTLCOLORSTATIC, (WPARAM)memDC.GetSafeHdc(), (LPARAM)m_hWnd);
	if (hBrush)
	{
		::FillRect(memDC.GetSafeHdc(), rc, hBrush);
	}
	else
	{
		memDC.FillSolidRect(rc, GetXtremeColor(COLOR_3DFACE));
	}

	DrawSliderBar(&memDC);
}


void CInplaceSlider::OnLButtonDown( UINT nFlags, CPoint point )
{
	CWnd::OnLButtonDown(nFlags, point);

	UpdateCursorPos(point);
}

void CInplaceSlider::OnMouseMove(UINT nFlags, CPoint point)
{
	CWnd::OnMouseMove(nFlags, point);

	if ((nFlags & MK_LBUTTON) && (GetCapture() == this))
	{
		UpdateCursorPos(point);

		if (GetFocus() != this)
		{
			SetFocus();
		}
	}
}

void CInplaceSlider::UpdateCursorPos(CPoint point)
{
	CRect rcClient;
	GetClientRect(rcClient);

	if (point.y < 0) point.y = 0;
	if (point.x < 0) point.x = 0;
	if (point.y > rcClient.bottom) point.y = rcClient.bottom;
	if (point.x > rcClient.right) point.x = rcClient.right;
	m_ptMousePos = point;

	float value = m_ptMousePos.x / (float)rcClient.Width() * (m_max - m_min) + m_min;
	SetValue( value );
}

void CInplaceSlider::DrawSliderBar(CDC* pDC)
{
	CRect rect;
	GetClientRect(&rect);
	//rect.DeflateRect(0, 4);
	//rect.right = rect.left + COLOR_BAND_WIDTH;

	if (m_enable)
	{
		pDC->FillSolidRect(rect.left, rect.top + 1,m_ptMousePos.x, rect.bottom - 1, m_clrFore);
		pDC->FillSolidRect(rect.left + m_ptMousePos.x - 1, rect.top + 1, 1, rect.bottom - 1, m_clrGrey);
	}
	else
	{
		pDC->FillSolidRect(rect.left, rect.top + 1, m_ptMousePos.x, rect.bottom - 1, m_clrGrey);
		pDC->FillSolidRect(rect.left + m_ptMousePos.x - 1, rect.top + 1, 1, rect.bottom - 1, m_clrFore);
	}
	
}

void CInplaceSlider::SetValue( float value )
{
	m_value = value;

	CRect rect;
	GetClientRect(&rect);

	m_ptMousePos.x = (m_value - m_min) / (m_max - m_min) * rect.Width();

	m_pItem->SetDouble(m_value);
	m_pItem->OnValueChanged(m_pItem->GetValue());
	m_pItem->GetGrid()->Invalidate(FALSE);

	Invalidate(FALSE);
	UpdateWindow();
}

void CInplaceSlider::SetRange( float min, float max )
{
	m_min = min;
	m_max = max;
}

void CInplaceSlider::SetHighlight(bool enable)
{
	m_enable = enable;
	Invalidate(FALSE);
}



CCustomItemSlider::CCustomItemSlider(CString strCaption, float fMin, float fMax)
	: CXTPPropertyGridItemDouble(strCaption, 0, _T("%0.3f"))
{
	m_wndSlider.m_pItem = this;
	//m_nFlags = 0;
	m_floatScale = 1000.0f / (fMax - fMin); 

	

	m_wndSlider.m_floatScale = m_floatScale;
	m_wndSlider.SetRange( fMin, fMax);
	//m_bReadOnly = false;
	m_pBindFloat = 0;
}

void CCustomItemSlider::OnDeselect()
{
	CXTPPropertyGridItemDouble::OnDeselect();
	m_wndSlider.SetHighlight(false);
	if (m_wndSlider.m_hWnd) m_wndSlider.DestroyWindow();
}

void CCustomItemSlider::OnSelect()
{
	CXTPPropertyGridItem::OnSelect();

	CRect rc = GetValueRect();
	CRect rco = rc;
	
 	if (!m_bReadOnly)
 	{
 
 		CWindowDC dc(m_pGrid);
 		CXTPFontDC font (&dc, GetGrid()->GetFont());
 		m_nWidth = dc.GetTextExtent(_T("XXXXX")).cx;
 
 		rc.left += m_nWidth + 2;
 
 		if (rc.left >= rc.right)
 			return;

 		if (!m_wndSlider.m_hWnd)
 		{
 			m_wndSlider.Create(WS_CHILD|TBS_HORZ, rc, (CWnd*)m_pGrid, 0);
			//m_wndSlider.CreateEx(WS_EX_TOPMOST, WS_CHILD|TBS_HORZ, rc, (CWnd*)m_pGrid, 0);
 		}

		m_wndSlider.SetValue(GetDouble());
 		m_wndSlider.EnableWindow();
 		m_wndSlider.MoveWindow(rc);
 		m_wndSlider.ShowWindow(SW_SHOW);
 	}

	GetInplaceEdit().SetWindowPos(0, rco.left, rco.top, m_nWidth, rco.Height(), SWP_NOZORDER | SWP_SHOWWINDOW);

	m_wndSlider.SetHighlight(true);
}
//////////////////////////////////////////////////////////////////////////
void CCustomItemSlider::SetDouble( double fValue )
{
	m_fValue = fValue;

	if (m_pBindDouble)
	{
		*m_pBindDouble = m_fValue;
	}

	if (m_pBindFloat)
	{
		*m_pBindFloat = m_fValue;
	}

	CXTPPropertyGridItem::SetValue(DoubleToString(fValue));
}
//////////////////////////////////////////////////////////////////////////
void CCustomItemSlider::OnBeforeInsert()
{
	if (m_pBindDouble && *m_pBindDouble != m_fValue)
	{
		SetDouble(*m_pBindDouble);
	}

	if (m_pBindFloat && *m_pBindFloat != m_fValue)
	{
		SetDouble(*m_pBindFloat);
	}
}

//////////////////////////////////////////////////////////////////////////
// CCustomItemButton



BEGIN_MESSAGE_MAP(CInplaceButton, CXTButton)
	ON_CONTROL_REFLECT(BN_CLICKED, OnClicked)
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()

void CInplaceButton::OnClicked()
{
	m_pItem->m_bValue = !m_pItem->m_bValue;
	m_pItem->OnValueChanged(m_pItem->GetValue());
	
	SetChecked(m_pItem->m_bValue);
}

BOOL CInplaceButton::OnSetCursor(CWnd* /*pWnd*/, UINT /*nHitTest*/, UINT /*message*/)
{
	::SetCursor(AfxGetApp()->LoadStandardCursor(MAKEINTRESOURCE(32649)));
	return TRUE;
}

CCustomItemButton::CCustomItemButton(CString strCaption, BOOL bFullRowButton, BOOL bValue)
	: CXTPPropertyGridItem(bFullRowButton? _T(""): strCaption)
{
	m_wndButton.m_pItem = this;
	m_nFlags = 0;
	m_bValue = bValue;
	m_strButtonText = strCaption;
	m_bFullRowButton = bFullRowButton;

	m_wndFont.CreateFont(12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, _T("Tahoma"));
}

BOOL CCustomItemButton::GetBool()
{
	return m_bValue;
}
void CCustomItemButton::SetBool(BOOL bValue)
{
	m_bValue = bValue;

	if (m_wndButton.GetSafeHwnd())
		m_wndButton.SetCheck(bValue);
}

BOOL CCustomItemButton::IsValueChanged()
{
	return !m_bValue;
}

void CCustomItemButton::CreateButton()
{
	if (IsVisible())
	{	
		CRect rc;
		if (m_bFullRowButton)
		{		
			rc = GetItemRect();
			rc.DeflateRect( m_nIndent * 14, 0, 0, 1);
		} else
		{
			rc = GetValueRect();
		}

		
		if (!m_wndButton.m_hWnd)
		{
			m_wndButton.Create(m_strButtonText, WS_CHILD|BS_FLAT|BS_NOTIFY|WS_VISIBLE|BS_OWNERDRAW, rc, (CWnd*)m_pGrid, 100);
			m_wndButton.SetFont(&m_wndFont);
			m_wndButton.SetTheme(new CXTButtonThemeOfficeXP(TRUE));
		}
		if (m_wndButton.GetChecked() != m_bValue) m_wndButton.SetChecked(m_bValue);
		m_wndButton.MoveWindow(rc);
		m_wndButton.Invalidate(FALSE);
	}
	else
	{
		m_wndButton.DestroyWindow();
	}
}


void CCustomItemButton::SetVisible(BOOL bVisible)
{
	CXTPPropertyGridItem::SetVisible(bVisible);
	CreateButton();
}

void CCustomItemButton::OnIndexChanged()
{
	CreateButton();	
}

BOOL CCustomItemButton::OnDrawItemValue(CDC& /*dc*/, CRect /*rcValue*/)
{
	CreateButton();
	return FALSE;
}



//////////////////////////////////////////////////////////////////////////
// CCustomItemMenu

void CCustomItemMenu::OnInplaceButtonDown(CXTPPropertyGridInplaceButton* pButton)
{
	CMenu menu;
	menu.CreatePopupMenu();

	menu.InsertMenu(0, MF_BYPOSITION | MF_STRING, 1, _T("Choose 1"));
	menu.InsertMenu(1, MF_BYPOSITION | MF_STRING, 2, _T("Choose 2"));
	menu.InsertMenu(2, MF_BYPOSITION | MF_STRING, 3, _T("Choose 3"));

	CRect rc = pButton->GetRect();
	pButton->GetGrid()->ClientToScreen(&rc);

	XTPPaintManager()->SetTheme(xtpThemeWhidbey);
	UINT nCmd = CXTPCommandBars::TrackPopupMenu(&menu, TPM_RETURNCMD|TPM_NONOTIFY, 
		rc.right, rc.top, pButton->GetGrid(), 0);
	
	if (nCmd >0)
	{
		CString str;
		menu.GetMenuString(nCmd, str, MF_BYCOMMAND);

		OnValueChanged(str);
	}
}
