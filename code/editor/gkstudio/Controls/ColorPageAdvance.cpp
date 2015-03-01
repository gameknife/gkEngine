#include "stdafx.h"

#include "Common/XTPDrawHelpers.h"
#include "Common/XTPColorManager.h"

#include "../Resource.h"
// #include "../Defines.h"
//#include "XTPColorDialog.h"
#include "ColorPageAdvance.h"
#include "PSCPColorDialog.h"

#define DEFAULT_LUMINANCE 0.50f

/////////////////////////////////////////////////////////////////////////////
// CXTPColorBase

CPSCPColorBase::CPSCPColorBase()
{
	m_nLum = 0.0;
	m_nSat = 0.0;
	m_nHue = 0.0;
	m_ptMousePos = CPoint(0, 0);
	m_bPreSubclassInit = true;
}

CPSCPColorBase::~CPSCPColorBase()
{
}

CPSCPColorBase::FocusedControl CPSCPColorBase::m_eHasFocus = focusNone;

BEGIN_MESSAGE_MAP(CPSCPColorBase, CStatic)
	//{{AFX_MSG_MAP(CXTPColorBase)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXTPColorBase message handlers

bool CPSCPColorBase::Init()
{
	if ( ::IsWindow(m_hWnd) )
	{
		// Set the style to SS_NOTIFY.
		DWORD dwStyle = ::GetWindowLong(m_hWnd, GWL_STYLE);
		::SetWindowLong(m_hWnd, GWL_STYLE, dwStyle | SS_NOTIFY);

		return true;
	}

	return false;
}

void CPSCPColorBase::PreSubclassWindow()
{
	CWnd::PreSubclassWindow();

	if (m_bPreSubclassInit)
	{
		// Initialize the control.
		Init();
	}
}

int CPSCPColorBase::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Initialize the control.
	Init();

	return 0;
}

BOOL CPSCPColorBase::PreCreateWindow(CREATESTRUCT& cs)
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	// When creating controls dynamically Init() must
	// be called from OnCreate() and not from
	// PreSubclassWindow().

	m_bPreSubclassInit = false;

	return TRUE;
}

void CPSCPColorBase::OnLButtonDown(UINT nFlags, CPoint point)
{
	CWnd::OnLButtonDown(nFlags, point);
	SetCapture();
	UpdateCursorPos(point);

	if (GetFocus() != this)
	{
		SetFocus();
	}
}

void CPSCPColorBase::OnLButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();
	CStatic::OnLButtonUp(nFlags, point);
}

void CPSCPColorBase::OnMouseMove(UINT nFlags, CPoint point)
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

void CPSCPColorBase::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	UpdateCursorPos(point);

	CWnd* pWnd = GetOwner();
	if (pWnd)
	{
		pWnd->SendMessage(WM_XTP_COLORDBLCLK,
			(WPARAM)0, (LPARAM)MAKELPARAM(LOWORD(point.x),HIWORD(point.y)));
	}

	CStatic::OnLButtonDblClk(nFlags, point);
}

void CPSCPColorBase::UpdateCursorPos(CPoint /*point*/)
{

}

COLORREF CPSCPColorBase::HLStoRGB(double h, double v, double s)
{
	//return CXTPDrawHelpers::HSLtoRGB(h, s, v);
	
	float r, g, b;
	if (v <= 0.00001 )
	{
		r = 0.0f; g = 0.0f; b = 0.0f;
	}
	else if (s < 0.00001)
	{
		r = v; g = v; b = v;
	}
	else
	{
		const float hi = h * 6.0f;
		const int i = (int)::floor(hi);
		const float f = hi - i;

		const float v0 = v * (1.0f - s);
		const float v1 = v * (1.0f - s * f);
		const float v2 = v * (1.0f - s * (1.0f - f));

		switch (i)
		{
		case 0: r = v; g = v2; b = v0; break;
		case 1: r = v1; g = v; b = v0; break;
		case 2: r = v0; g = v; b = v2; break;
		case 3: r = v0; g = v1; b = v; break;
		case 4: r = v2; g = v0; b = v; break;
		case 5: r = v; g = v0; b = v1; break;

		case 6: r = v; g = v2; b = v0; break;
		case -1: r = v; g = v0; b = v1; break;
		default: r = 0.0f; g = 0.0f; b = 0.0f; break;
		}
	}

	return RGB(r * 255, g * 255, b * 255);
}

void CPSCPColorBase::RGBtoHSL(COLORREF color, double& h, double& s, double& v)
{
	double r, g, b;
	r = GetRValue(color) / 255.0;
	g = GetGValue(color) / 255.0;
	b = GetBValue(color) / 255.0;

	if ((b > g) && (b > r))
	{
		if ((v = b) > 0.0001)
		{
			const double min = r < g ? r : g;
			const double delta = v - min;
			if ((delta) > 0.0001)
			{
				s = delta / v;
				h = (240.0f/360.0f) + (r - g) / delta * (60.0f/360.0f);
			}
			else
			{
				s = 0.0f;
				h = (240.0f/360.0f) + (r - g) * (60.0f/360.0f);
			}
			if (h < 0.0f) h += 1.0f;
		}
		else
		{
			s = 0.0f;
			h = 0.0f;
		}
	}
	else if (g > r)
	{
		if ((v = g) > 0.0001)
		{
			const double min = r < b ? r : b;
			const double delta = v - min;
			if ((delta) > 0.0001)
			{
				s = delta / v;
				h = (120.0f/360.0f) + (b - r) / delta * (60.0f/360.0f);
			}
			else
			{
				s = 0.0f;
				h = (120.0f/360.0f) + (b - r) * (60.0f/360.0f);
			}
			if (h < 0.0f) h += 1.0f;
		}
		else
		{
			s = 0.0f;
			h = 0.0f;
		}
	}
	else
	{
		if ((v = r) > 0.0001)
		{
			const double min = g < b ? g : b;
			const double delta = v - min;
			if ((delta) > 0.0001)
			{
				s = delta / v;
				h = (g - b) / delta * (60.0f/360.0f);
			}
			else
			{
				s = 0.0f;
				h = (g - b) * (60.0f/360.0f);
			}
			if (h < 0.0f) h += 1.0f;
		}
		else
		{
			s = 0.0f;
			h = 0.0f;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CXTPColorWnd

CPSCPColorWnd::CPSCPColorWnd()
{
	m_nRed = 255;
	m_nGreen = 0;
	m_nBlue = 0;
	m_needUpdate = true;

	m_mode = MODE_HUE;
}

CPSCPColorWnd::~CPSCPColorWnd()
{
}

BEGIN_MESSAGE_MAP(CPSCPColorWnd, CPSCPColorBase)
	//{{AFX_MSG_MAP(CXTPColorWnd)
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXTPColorWnd message handlers

void CPSCPColorWnd::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CRect rc;
	GetClientRect(&rc);
	CXTPBufferDC memDC(dc);

	int cy = rc.Height();
	int cx = rc.Width();

	if (m_bmpPicker.GetSafeHandle() == NULL)
	{
		// create bitmap
		m_bmpPicker.CreateCompatibleBitmap(&memDC, cx, cy);
	}


	if (m_needUpdate)
	{
		// create picker DC
		//CXTPCompatibleDC dcPicker(&memDC, m_bmpPicker);

		DWORD* pixels = new DWORD[cx * cy];	// 32位颜色点阵数组

		int rForRow, gForRow, bForRow, rForColumn, gForColumn, bForColumn;

		for(int y = 0; y < cy; y++)
		{
			int normal_y = y * 255 / cy;
			
			for(int x = 0; x < cx; x++)
			{
				int normal_x = x * 255 / cx;

				COLORREF clr;

				clr = TRANSCODE_RGB(normal_x, normal_y);

				int r = GetRValue(clr);
				int g = GetGValue(clr);
				int b = GetBValue(clr);

				pixels[y * cx + x] = RGB(b, g, r);
			}
		}

		m_bmpPicker.SetBitmapBits(cx * cy * 4, pixels);
		delete[] pixels;

		m_needUpdate = false;
	}

	memDC.DrawState(CPoint(0, 0), CSize(0, 0), &m_bmpPicker, DSS_NORMAL, NULL);
	DrawCrossHair(&memDC);
}

BOOL CPSCPColorWnd::OnEraseBkgnd(CDC* /*pDC*/)
{
	return TRUE;
}

COLORREF CPSCPColorWnd::TRANSCODE_RGB( int x, int y )
{
	int rForColumn, gForColumn, bForColumn;
	COLORREF clr;

	switch( m_mode )
	{
	case MODE_HUE:
		clr = HLStoRGB(m_nHue, (1.0 - y / 255.0), x / 255.0 );
		break;
	case MODE_SAT:
		clr = HLStoRGB(x / 255.0, (1.0 - y / 255.0), m_nSat  );
		break;
	case MODE_BRIGHTNESS:
		clr = HLStoRGB(x / 255.0, m_nLum, (1.0 - y / 255.0) );
		break;
	case MODE_RED:
		rForColumn = m_nRed;
		gForColumn = x;
		bForColumn = y;
		clr = RGB(rForColumn, gForColumn, bForColumn);
		break;
	case MODE_GREEN:
		rForColumn = x;
		gForColumn = m_nGreen;
		bForColumn = y;
		clr = RGB(rForColumn, gForColumn, bForColumn);
		break;
	case MODE_BLUE:
		rForColumn = x;
		gForColumn = y;
		bForColumn = m_nBlue;
		clr = RGB(rForColumn, gForColumn, bForColumn);
		break;
	}
	
	return clr;
}

void CPSCPColorWnd::UpdateCursorPos(CPoint point)
{
	CRect rcClient;
	GetClientRect(rcClient);

	if (point.y < 0) point.y = 0;
	if (point.x < 0) point.x = 0;
	if (point.y > rcClient.bottom) point.y = rcClient.bottom;
	if (point.x > rcClient.right) point.x = rcClient.right;
	m_ptMousePos = point;

	//////////////////////////////////////////////////////////////////////////
	// calc new RGB from this
	
	int m_axis_x = (double)point.x / (double)rcClient.Width() * 255;
	int m_axis_y = (double)point.y / (double)rcClient.Height() * 255;

	CWnd* pWnd = GetOwner();
	if (pWnd)
	{
		pWnd->SendMessage(WM_XTP_UPDATECOLOR,
			(WPARAM)TRANSCODE_RGB( m_axis_x, m_axis_y ), (LPARAM)m_hWnd);
	}

	Invalidate(FALSE);
}

BOOL CPSCPColorWnd::PreTranslateMessage(MSG* pMsg)
{
	if (m_eHasFocus == focusColorWheel)
	{
		switch (pMsg->message)
		{
		case WM_KEYDOWN:
			{
				CRect rect;
				GetClientRect(&rect);

				TCHAR vkey = (TCHAR)pMsg->wParam;
				switch (vkey)
				{
				case VK_LEFT:
					m_ptMousePos.x--;
					UpdateCursorPos(m_ptMousePos);
					return TRUE;

				case VK_UP:
					m_ptMousePos.y--;
					UpdateCursorPos(m_ptMousePos);
					return TRUE;

				case VK_RIGHT:
					m_ptMousePos.x++;
					UpdateCursorPos(m_ptMousePos);
					return TRUE;

				case VK_DOWN:
					m_ptMousePos.y++;
					UpdateCursorPos(m_ptMousePos);
					return TRUE;
				}
			}
			break;
		}
	}

	return CStatic::PreTranslateMessage(pMsg);
}

void CPSCPColorWnd::DrawCrossHair(CDC* pDC)
{
	int x = m_ptMousePos.x;
	int y = m_ptMousePos.y;

	CPen pen1(PS_SOLID, 2, RGB(0xff, 0xff, 0xff));
	CPen pen2(PS_SOLID, 2, RGB(0x00, 0x00, 0x00));

	CPen* pOldPen = pDC->SelectObject((m_eHasFocus == focusColorWheel) ? &pen1 : &pen2);
	
	pDC->MoveTo(x-5, y);
	pDC->LineTo(x-10, y);

	pDC->MoveTo(x + 5, y);
 	pDC->LineTo(x + 10, y);

	pDC->MoveTo(x, y-5);
	pDC->LineTo(x, y-10);

	pDC->MoveTo(x, y + 5);
	pDC->LineTo(x, y + 10);

	pDC->SelectObject(pOldPen);
}

void CPSCPColorWnd::SetColor( COLORREF color )
{
	double h,s,l;
	RGBtoHSL( color, h, s, l );

	m_nHue = h;
	m_nSat = s;
	m_nLum = l;

	m_nRed = GetRValue( color );
	m_nBlue = GetBValue( color );
	m_nGreen = GetGValue( color );

	double r,g,b;

	r = m_nRed / 255.0;
	g = m_nGreen / 255.0;
	b = m_nBlue / 255.0;

	m_needUpdate = true;

	CRect rc;
	GetClientRect(&rc);

	switch ( m_mode )
	{
	case MODE_HUE:
		m_ptMousePos.x = (long)((double)rc.Width() * s);
		m_ptMousePos.y = rc.Height()-(long)((double)rc.Height() * l);	
		break;


	case MODE_SAT:
		m_ptMousePos.x = (long)((double)rc.Width() * h);
		m_ptMousePos.y = rc.Height()-(long)((double)rc.Height() * l);
		break;

	case MODE_BRIGHTNESS:
		m_ptMousePos.x = (long)((double)rc.Width() * h);
		m_ptMousePos.y = rc.Height()-(long)((double)rc.Height() * s);
		break;

	case MODE_RED:
		m_ptMousePos.x = (long)((double)rc.Width() * g);
		m_ptMousePos.y = (long)((double)rc.Height() * b);
		break;

	case MODE_GREEN:
		m_ptMousePos.x = (long)((double)rc.Width() * r);
		m_ptMousePos.y = (long)((double)rc.Height() * b);
		break;

	case MODE_BLUE:
		m_ptMousePos.x = (long)((double)rc.Width() * r);
		m_ptMousePos.y = (long)((double)rc.Height() * g);
		break;
	}


	Invalidate(FALSE);
}

void CPSCPColorWnd::OnSetFocus(CWnd* pOldWnd)
{
	CPSCPColorBase::OnSetFocus(pOldWnd);
	m_eHasFocus = focusColorWheel;

	Invalidate(FALSE);
}

void CPSCPColorWnd::OnKillFocus(CWnd* pNewWnd)
{
	CPSCPColorBase::OnKillFocus(pNewWnd);
	m_eHasFocus = focusNone;

	Invalidate(FALSE);
}

COLORREF CPSCPColorWnd::GET_RGB_BY_CURSOR()
{
	CRect rcClient;
	GetClientRect(rcClient);

	int m_axis_x = (double)m_ptMousePos.x / (double)rcClient.Width() * 255;
	int m_axis_y = (double)m_ptMousePos.y / (double)rcClient.Height() * 255;
	return TRANSCODE_RGB(m_axis_x, m_axis_y);
}

/////////////////////////////////////////////////////////////////////////////
// CXTPColorLum

CPSCPColorLum::CPSCPColorLum()
{
	m_ptMousePos.y = 0;
}

CPSCPColorLum::~CPSCPColorLum()
{
}

BEGIN_MESSAGE_MAP(CPSCPColorLum, CPSCPColorBase)
	//{{AFX_MSG_MAP(CXTPColorLum)
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXTPColorLum message handlers

void CPSCPColorLum::OnPaint()
{
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


	DrawLuminanceBar(&memDC);
	DrawSliderArrow(&memDC);
}

BOOL CPSCPColorLum::OnEraseBkgnd(CDC* /*pDC*/)
{
	return TRUE;
}

#define COLOR_BAND_WIDTH 20

void CPSCPColorLum::GetLumBarRect(CRect& rect)
{
	GetClientRect(&rect);
	rect.DeflateRect(0, 4);
	rect.right = rect.left + COLOR_BAND_WIDTH;
}

void CPSCPColorLum::SetColor( COLORREF color )
{

	double h,s,l;
	RGBtoHSL( color, h, s, l );

	m_nHue = h;
	m_nSat = s;
	m_nLum = l;

	m_nRed = GetRValue( color );
	m_nBlue = GetBValue( color );
	m_nGreen = GetGValue( color );

	double r,g,b;

	r = m_nRed / 255.0;
	g = m_nGreen / 255.0;
	b = m_nBlue / 255.0;

	CRect rcBar;
	GetLumBarRect(rcBar);

	switch ( m_mode )
	{
	case MODE_HUE:
		m_ptMousePos.y = rcBar.top + (long)((double)rcBar.Height() * h);
		break;

	case MODE_SAT:
		m_ptMousePos.y = rcBar.top + (long)((double)rcBar.Height() * (1.0 - s));
		break;

	case MODE_BRIGHTNESS:
		m_ptMousePos.y = rcBar.top + (long)((double)rcBar.Height() * (1.0 - l));
		break;

	case MODE_RED:
		m_ptMousePos.y = rcBar.top + (long)((double)rcBar.Height() * r);
		break;

	case MODE_GREEN:
		m_ptMousePos.y = rcBar.top + (long)((double)rcBar.Height() * g);
		break;

	case MODE_BLUE:
		m_ptMousePos.y = rcBar.top + (long)((double)rcBar.Height() * b);
		break;

	default:


		break;
	}


	Invalidate(FALSE);
}

COLORREF CPSCPColorLum::GetColorFromX( double digit )
{
	COLORREF clr;

	switch(m_mode)
	{
	case MODE_HUE:
		clr = HLStoRGB( digit, 1, 1 );
		break;
	case MODE_SAT:
		clr = HLStoRGB( m_nHue, m_nLum, 1.0 - digit );
		break;
	case MODE_BRIGHTNESS:
		clr = HLStoRGB( m_nHue, 1.0 - digit, m_nSat );
		break;
	case MODE_RED:
		clr = RGB( digit * 255, m_nGreen, m_nBlue);
		break;
	case MODE_BLUE:
		clr = RGB( m_nRed, m_nGreen, digit * 255);
		break;
	case MODE_GREEN:
		clr = RGB( m_nRed, digit * 255, m_nBlue);
		break;
	}
	
	return clr;
}

void CPSCPColorLum::DrawLuminanceBar(CDC* pDC)
{
	CRect rcBar;
	GetLumBarRect(rcBar);

	int  cy = rcBar.Height();
	int  cx = rcBar.Width();

	// fill color picker bitmap
	for (int y = 0; y < cy; y++)
	{
		COLORREF clr;

		clr = GetColorFromX( (double)y / (double)cy );
		pDC->FillSolidRect(rcBar.left + 6, y + rcBar.top, cx, 1, clr);
	}
}

void CPSCPColorLum::DrawSliderArrow(CDC* pDC)
{
	CPen pen1(PS_SOLID, 1, RGB(0x00, 0x00, 0x00));
	CPen pen2(PS_SOLID, 1, GetXtremeColor(COLOR_3DSHADOW));

	CPen* pOldPen = pDC->SelectObject((m_eHasFocus == focusLumination) ? &pen1 : &pen2);

	{
		CRect rc;
		GetClientRect(&rc);
		rc.left += COLOR_BAND_WIDTH + 7;

		int x = rc.left;
		int y = m_ptMousePos.y;

		CRect rcBar;
		GetLumBarRect(rcBar);

		y = max(rcBar.top, y);
		y = min(rcBar.bottom, y);

		pDC->MoveTo(x, y);
		pDC->LineTo(x + 5, y - 3);
		pDC->LineTo(x + 5, y + 3);
		pDC->LineTo(x, y);

	}

	{
		CRect rc;
		GetClientRect(&rc);
		rc.left += 5;

		int x = rc.left;
		int y = m_ptMousePos.y;

		CRect rcBar;
		GetLumBarRect(rcBar);

		pDC->MoveTo(x, y);
		pDC->LineTo(x - 5, y - 3);
		pDC->LineTo(x - 5, y + 3);
		pDC->LineTo(x, y);
	}


	if (m_eHasFocus != focusLumination)
	{

	}

	pDC->SelectObject(pOldPen);
}

void CPSCPColorLum::UpdateCursorPos(CPoint point)
{
	point.x = 0;

	CRect rcBar;
	GetLumBarRect(rcBar);

	point.y = max(rcBar.top, point.y);
	point.y = min(rcBar.bottom, point.y);
	double cy = (double)rcBar.Height();

	switch(m_mode)
	{
	case MODE_HUE:
		m_nHue = point.y / (double)cy;
		break;
	case MODE_SAT:
		m_nSat = 1.0 - point.y / (double)cy;
		break;
	case MODE_BRIGHTNESS:
		m_nLum = 1.0 - point.y / (double)cy;
		break;
	case MODE_RED:
		m_nRed = point.y / (double)cy * 255;
		break;
	case MODE_GREEN:
		m_nGreen = point.y / (double)cy * 255;
		break;
	case MODE_BLUE:
		m_nBlue = point.y / (double)cy * 255;
		break;
	}


	CWnd* pWnd = GetOwner();
	if (pWnd)
	{
		pWnd->SendMessage(WM_XTP_UPDATECOLOR,
			(WPARAM)m_clr, (LPARAM)m_hWnd);
	}

	m_ptMousePos = point;

	Invalidate(FALSE);
}

BOOL CPSCPColorLum::PreTranslateMessage(MSG* pMsg)
{
	if (m_eHasFocus == focusLumination)
	{
		CRect rcBar;
		GetLumBarRect(rcBar);

		switch (pMsg->message)
		{
		case WM_KEYDOWN:
			{
				TCHAR vkey = (TCHAR)pMsg->wParam;
				switch (vkey)
				{
				case VK_UP:
					m_ptMousePos.y--;
					UpdateCursorPos(m_ptMousePos);
					return TRUE;

				case VK_DOWN:
					m_ptMousePos.y++;
					UpdateCursorPos(m_ptMousePos);
					return TRUE;
				}
			}
			break;
		}
	}

	return CStatic::PreTranslateMessage(pMsg);
}

void CPSCPColorLum::OnSetFocus(CWnd* pOldWnd)
{
	CPSCPColorBase::OnSetFocus(pOldWnd);
	m_eHasFocus = focusLumination;

	Invalidate(FALSE);
}

void CPSCPColorLum::OnKillFocus(CWnd* pNewWnd)
{
	CPSCPColorBase::OnKillFocus(pNewWnd);
	m_eHasFocus = focusNone;

	Invalidate(FALSE);
}

/////////////////////////////////////////////////////////////////////////////
// CXTPColorPageCustom

BEGIN_MESSAGE_MAP(CPSCPColorPageCustom, CPropertyPage)
	//{{AFX_MSG_MAP(CXTPColorPageCustom)

	ON_EN_CHANGE(IDC_PSCP_RED, OnChangeEdit)
	ON_EN_CHANGE(IDC_PSCP_GREEN, OnChangeEdit)
	ON_EN_CHANGE(IDC_PSCP_BLUE, OnChangeEdit)


	ON_EN_CHANGE(IDC_PSCP_HUE, OnChangeEditHLS)
	ON_EN_CHANGE(IDC_PSCP_SAT, OnChangeEditHLS)
	ON_EN_CHANGE(IDC_PSCP_LUM, OnChangeEditHLS)

	ON_BN_CLICKED(IDC_PSCP_RCHECKER, OnChangeModeR)
	ON_BN_CLICKED(IDC_PSCP_GCHECKER, OnChangeModeG)
	ON_BN_CLICKED(IDC_PSCP_BCHECKER, OnChangeModeB)
	ON_BN_CLICKED(IDC_PSCP_HCHECKER, OnChangeModeH)
	ON_BN_CLICKED(IDC_PSCP_SCHECKER, OnChangeModeS)
	ON_BN_CLICKED(IDC_PSCP_LCHECKER, OnChangeModeL)
	
// 	ON_EN_CHANGE(XTP_IDC_EDIT_LUM, OnChangeEditLum)
// 	ON_EN_CHANGE(XTP_IDC_EDIT_HUE, OnChangeEditHue)
// 	ON_EN_CHANGE(XTP_IDC_EDIT_SAT, OnChangeEditSat)

	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_XTP_UPDATECOLOR, OnUpdateColor)
	ON_MESSAGE(WM_XTP_COLORDBLCLK, OnColorDblClick)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXTPColorPageCustom construction/destruction

CPSCPColorPageCustom::CPSCPColorPageCustom(CPSCPColorDialog* pParentSheet)
	: CPropertyPage(CPSCPColorPageCustom::IDD)
{
	m_psp.dwFlags &= ~PSP_HASHELP;
	m_pParentSheet = pParentSheet;

	//{{AFX_DATA_INIT(CXTPColorPageCustom)
	m_nR = 0;
	m_nB = 0;
	m_nG = 0;
	m_nH = 0;
	m_nL = 0;
	m_nS = 0;

	//ChangeMode(MODE_HUE);
	//}}AFX_DATA_INIT
}

CPSCPColorPageCustom::~CPSCPColorPageCustom()
{
}

void CPSCPColorPageCustom::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CXTPColorPageCustom)
	DDX_Control(pDX, IDC_PSCP_COLOR_AREA1, m_colorWnd);
 	DDX_Control(pDX, IDC_PSCP_COLOR_AREA2, m_colorLum);

	DDX_Control(pDX, IDC_PSCP_HUE, m_editHue);
	DDX_Control(pDX, IDC_PSCP_SAT, m_editSat);
	DDX_Control(pDX, IDC_PSCP_LUM, m_editLum);

 	DDX_Control(pDX, IDC_PSCP_GREEN, m_editGreen);
 	DDX_Control(pDX, IDC_PSCP_BLUE, m_editBlue);
 	DDX_Control(pDX, IDC_PSCP_RED, m_editRed);

	DDX_Control(pDX, IDC_PSCP_COLORREF, m_editColorRef);


	DDX_Text(pDX, IDC_PSCP_RED, m_nR);
	DDV_MinMaxInt(pDX, m_nR, 0, 255);
	DDX_Text(pDX, IDC_PSCP_BLUE, m_nB);
	DDV_MinMaxInt(pDX, m_nB, 0, 255);
	DDX_Text(pDX, IDC_PSCP_GREEN, m_nG);
	DDV_MinMaxInt(pDX, m_nG, 0, 255);

	DDX_Text(pDX, IDC_PSCP_HUE, m_nH);
	DDV_MinMaxInt(pDX, m_nH, 0, 100);
	DDX_Text(pDX, IDC_PSCP_SAT, m_nS);
	DDV_MinMaxInt(pDX, m_nS, 0, 100);
	DDX_Text(pDX, IDC_PSCP_LUM, m_nL);
	DDV_MinMaxInt(pDX, m_nL, 0, 100);

	DDX_Text(pDX, IDC_PSCP_COLORREF, m_colorref);
}

BOOL CPSCPColorPageCustom::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	((CButton *)GetDlgItem(IDC_PSCP_HCHECKER))->SetCheck(TRUE);

	return TRUE;
}

BOOL CPSCPColorPageCustom::OnSetActive()
{
	COLORREF clr = m_pParentSheet->GetColor();
	RGBtoHSL(clr, m_nH, m_nS, m_nL);

	UpdateRGB(clr);


	//////////////////////////////////////////////////////////////////////////
	// reposition the color band and cursor
	OnUpdateColor( clr, (LPARAM)m_hWnd );

	ChangeMode(MODE_HUE);

	return CPropertyPage::OnSetActive();
}

void CPSCPColorPageCustom::OnChangeEdit()
{
	UpdateData();
	COLORREF clr = (COLORREF)RGB(m_nR, m_nG, m_nB);

	OnUpdateColor((WPARAM)clr, 0);
}

void CPSCPColorPageCustom::OnChangeEditHLS()
{
	UpdateData();
	double dh, dl, ds;
	dh = m_nH / 100.0;
	dl = m_nL / 100.0;
	ds = m_nS / 100.0;

	COLORREF clr = m_colorLum.HLStoRGB(dh, dl, ds);

	OnUpdateColor((WPARAM)clr, 0);
}

void CPSCPColorPageCustom::UpdateRGB(COLORREF clr)
{
	m_nR = GetRValue(clr);
	m_nG = GetGValue(clr);
	m_nB = GetBValue(clr);
	RGBtoHSL(clr, m_nH, m_nS, m_nL);

	m_colorref.Format(_T("%2X%2X%2X"), GetRValue(clr), GetGValue(clr), GetBValue(clr));
	m_colorref.Replace(_T(" "), _T("0"));

	UpdateData(FALSE);
}

LRESULT CPSCPColorPageCustom::OnUpdateColor(WPARAM wParam, LPARAM lParam)
{
	COLORREF clr = (COLORREF)wParam;

	if ((HWND)lParam == m_colorWnd.m_hWnd)
	{
		double h,s,l;
		RGBtoHSL(clr, h, s, l);
		m_colorLum.m_nHue = h;
		m_colorLum.m_nSat = s;
		m_colorLum.m_nLum = l;

		m_colorLum.m_nRed = GetRValue(clr);
		m_colorLum.m_nGreen = GetGValue(clr);
		m_colorLum.m_nBlue = GetBValue(clr);

		m_colorLum.Invalidate(FALSE);
	}
	else if ((HWND)lParam == m_colorLum.m_hWnd)
	{
		m_colorWnd.m_nHue = m_colorLum.m_nHue;
		m_colorWnd.m_nSat = m_colorLum.m_nSat;
		m_colorWnd.m_nLum = m_colorLum.m_nLum;

 		m_colorWnd.m_nRed = m_colorLum.m_nRed;
 		m_colorWnd.m_nGreen = m_colorLum.m_nGreen;
 		m_colorWnd.m_nBlue = m_colorLum.m_nBlue;
		m_colorWnd.m_needUpdate = true;

		clr = m_colorWnd.GET_RGB_BY_CURSOR();

		m_colorWnd.Invalidate(FALSE);
	}
	else
	{
		// reposition the cursor and grid
		m_colorWnd.SetColor( clr );
		m_colorLum.SetColor( clr );

		// get a handle to the window at the cursor location.
		POINT point;
		::GetCursorPos(&point);
		HWND hWnd = ::WindowFromPoint(point);

		if (::IsWindow(hWnd))
		{
			if ((hWnd == m_colorLum.m_hWnd) ||
				(hWnd == m_colorWnd.m_hWnd))
			{
				::SetFocus(hWnd);
			}
		}
	}


	

	if ((HWND)lParam != m_pParentSheet->GetSafeHwnd())
	{
		if (clr != m_pParentSheet->GetColor())
			m_pParentSheet->SetNewColor(clr, FALSE);
	}

	UpdateRGB(clr);
	

	CPSCPColorDialog* parent = (CPSCPColorDialog*)m_pParentSheet;
	IPSCPColorChangeCallbacks& callback =  parent->getListener();
	IPSCPColorChangeCallbacks::iterator it = callback.begin();
	for (; it != callback.end(); ++it)
	{
		if( *it )
		{
			(*it)->OnColorChange( clr );
		}
	}

	return 0;
}

LRESULT CPSCPColorPageCustom::OnColorDblClick(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	m_pParentSheet->EndDialog(IDOK);

	return 0;
}

void CPSCPColorPageCustom::RGBtoHSL( COLORREF color, double& h, double& s, double& v )
{
	double r, g, b;
	r = GetRValue(color) / 255.0;
	g = GetGValue(color) / 255.0;
	b = GetBValue(color) / 255.0;

	if ((b > g) && (b > r))
	{
		if ((v = b) > 0.0001)
		{
			const double min = r < g ? r : g;
			const double delta = v - min;
			if ((delta) > 0.0001)
			{
				s = delta / v;
				h = (240.0f/360.0f) + (r - g) / delta * (60.0f/360.0f);
			}
			else
			{
				s = 0.0f;
				h = (240.0f/360.0f) + (r - g) * (60.0f/360.0f);
			}
			if (h < 0.0f) h += 1.0f;
		}
		else
		{
			s = 0.0f;
			h = 0.0f;
		}
	}
	else if (g > r)
	{
		if ((v = g) > 0.0001)
		{
			const double min = r < b ? r : b;
			const double delta = v - min;
			if ((delta) > 0.0001)
			{
				s = delta / v;
				h = (120.0f/360.0f) + (b - r) / delta * (60.0f/360.0f);
			}
			else
			{
				s = 0.0f;
				h = (120.0f/360.0f) + (b - r) * (60.0f/360.0f);
			}
			if (h < 0.0f) h += 1.0f;
		}
		else
		{
			s = 0.0f;
			h = 0.0f;
		}
	}
	else
	{
		if ((v = r) > 0.0001)
		{
			const double min = g < b ? g : b;
			const double delta = v - min;
			if ((delta) > 0.0001)
			{
				s = delta / v;
				h = (g - b) / delta * (60.0f/360.0f);
			}
			else
			{
				s = 0.0f;
				h = (g - b) * (60.0f/360.0f);
			}
			if (h < 0.0f) h += 1.0f;
		}
		else
		{
			s = 0.0f;
			h = 0.0f;
		}
	}
}

void CPSCPColorPageCustom::RGBtoHSL( COLORREF color, int& h, int& s, int& l )
{
	double dh, ds, dl;
	RGBtoHSL(color, dh, ds, dl);
	h = dh * 100;
	s = ds * 100;
	l = dl * 100;
}

void CPSCPColorPageCustom::ChangeMode( DWORD mode )
{
	m_mode = mode;

	m_colorWnd.m_mode = m_mode;
	m_colorWnd.m_needUpdate = true;
	m_colorLum.m_mode = m_mode;

	OnUpdateColor( RGB(m_nR, m_nG, m_nB) , (LPARAM)m_hWnd );
}

