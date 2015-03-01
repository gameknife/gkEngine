#include "StdAfx.h"
#include "PSCPColorDialog.h"
#include "ColorPageAdvance.h"
//#include "ResHandleMan.h"

//IMPLEMENT_DYNAMIC(CPSCPColorDialog, CPropertySheet)

CPSCPColorDialog::CPSCPColorDialog( COLORREF clrNew, COLORREF clrCurrent, DWORD dwFlags, CWnd* pWndParent ):CXTPColorDialog( clrNew, clrCurrent, dwFlags, pWndParent )
{
	//CResHandleMan man(g_instance);
	CPropertyPage* page = new CPSCPColorPageCustom(this);
	AddPage( page );
}


CPSCPColorDialog::~CPSCPColorDialog(void)
{
}

BEGIN_MESSAGE_MAP(CPSCPColorDialog, CXTPColorDialog)
	//{{AFX_MSG_MAP(CXTPColorDialog)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CPSCPColorDialog::OnInitDialog()
{
	MoveWindow(0,0,350,300);
	BOOL ret =  CXTPColorDialog::OnInitDialog();

	SetActivePage(2);

	return ret;
}

void CPSCPColorDialog::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(GetXtremeColor(COLOR_WINDOWTEXT));

	CXTPFontDC fontDC(&dc, GetFont());

	// get the Cancel button size.
	CXTPWindowRect rcBtnCancel(GetDlgItem(IDCANCEL));
	ScreenToClient(&rcBtnCancel);

	// construct the size for the the new / current color box.
	CXTPClientRect rect(this);
	rect.right = rcBtnCancel.right;
	rect.left = rcBtnCancel.left;
	rect.top = rcBtnCancel.bottom + (::IsWindow(m_wndHexEdit.m_hWnd) ? 66 : 43);
	rect.bottom = rect.top + 86;

	// draw the borders for the new / current color box.
// 	if (m_dwStyle & CPS_XTP_SHOW3DSELECTION)
// 	{
// 		dc.Draw3dRect(&rect, GetXtremeColor(COLOR_3DHILIGHT),
// 			GetXtremeColor(COLOR_3DDKSHADOW));
// 
// 		rect.DeflateRect(3, 3);
// 		dc.Draw3dRect(&rect, GetXtremeColor(COLOR_3DSHADOW),
// 			GetXtremeColor(COLOR_3DHILIGHT));
// 	}
// 	else
	{
		dc.Draw3dRect(&rect, GetXtremeColor(COLOR_WINDOWFRAME),
			GetXtremeColor(COLOR_WINDOWFRAME));
	}

	// draw the new text string.
	CRect rcText = rect;
	rcText = rect;
	rcText.top -= 22;
	rcText.bottom = rcText.top + 22;

	CString strText;
	CXTPResourceManager::AssertValid(XTPResourceManager()->LoadString(&strText, XTP_IDS_NEW));
	dc.DrawText(strText, rcText, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	// draw the current text string.
	rcText = rect;
	rcText.top = rcText.bottom;
	rcText.bottom = rcText.top + 22;

	CXTPResourceManager::AssertValid(XTPResourceManager()->LoadString(&strText, XTP_IDS_CURRENT));
	dc.DrawText(strText, rcText, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	rect.DeflateRect(1, 1);
	int cy = rect.Height()/2;

	// fill the new color box.
	m_rcNew = rect;
	m_rcNew.bottom = rect.top + cy;
	dc.FillSolidRect(&m_rcNew, m_clrNew);

	// fill the current color box.
	m_rcCurrent = rect;
	m_rcCurrent.top = rect.bottom - cy;
	dc.FillSolidRect(&m_rcCurrent, m_clrCurrent);
}

