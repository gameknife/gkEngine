// PaneStyles.cpp : implementation file
//

#include "stdafx.h"
#include "ribbonsample.h"
#include "gkPaneConsole.h"
#include "RibbonSample.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CPaneStyles dialog


CPaneConsole::CPaneConsole(CWnd* pParent /*=NULL*/)
: IPaneBase(CPaneConsole::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPaneStyles)
	//}}AFX_DATA_INIT

	m_pCallback = new SConsolePaneLogCallback(this);
}

//-----------------------------------------------------------------------
CPaneConsole::~CPaneConsole()
{
	SAFE_DELETE( m_pCallback );
}



void CPaneConsole::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPaneStyles)
	//DDX_Control(pDX, IDC_LIST_CONSOLE, m_edtConsoleBox);
	DDX_Control(pDX, IDC_BUTTON_CVAR, m_btnCVar);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_RICHEDIT_CONSOLELIST, m_richEdit);
}


BEGIN_MESSAGE_MAP(CPaneConsole, IPaneBase)
	//{{AFX_MSG_MAP(CPaneStyles)
	//ON_WM_CTLCOLOR()

	ON_BN_CLICKED( IDC_BUTTON_CVAR , OnCVar )

	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPaneStyles message handlers

void CPaneConsole::OnCancel()
{
}

void CPaneConsole::OnOK()
{
}

void CPaneConsole::OnCVar()
{
	((CMainFrame*)AfxGetMainWnd())->OnSwitchPanes( ID_PANES_CVAR);
}



HBRUSH CPaneConsole::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	return m_brush;
}

BOOL CPaneConsole::OnInitDialog() 
{
	CXTResizeDialog::OnInitDialog();

	//SetResize(IDC_LIST_CONSOLE,       SZ_TOP_LEFT, SZ_BOTTOM_RIGHT);

 	SetResize(IDC_BUTTON_CVAR,		  SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDC_RICHEDIT_CONSOLELIST,       SZ_TOP_LEFT, SZ_BOTTOM_RIGHT);
	
	CXTPImageManagerIconHandle ih;
	CXTPImageManagerIconHandle ihh;
	// 	ih = CXTPImageManagerIcon::LoadBitmapFromResource(MAKEINTRESOURCE(IDC_BUTTON_NEWSTYLE), NULL);
	// 	m_btnNewStyle.SetIcon(CSize(16, 16), ih, ihh);

	RefreshMetrics(TRUE);

	gEnv->pLog->AddCallback(m_pCallback);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CPaneConsole::RefreshMetrics(BOOL bOffice2007)
{
	m_brush.DeleteObject();
}


static CString CopyAndRemoveColorCode( const TCHAR* sText,int &iColorCode )
{
	CString ret=sText;		// alloc and copy

	TCHAR *s,*d;

	s=ret.GetBuffer();d=s;

	// remove color code in place
	while(*s!=0)
	{
		if(*s==_T('$') && isdigit(*(s+1)))
		{
			if (iColorCode == 0)
				iColorCode = *(s+1) - _T('0');
			s+=2;
			continue;
		}
		if (*s == _T('\r') || *s == _T('\n'))
		{
			s++;
			continue;
		}

		*d++=*s++;
	}

	ret.ReleaseBuffer(d-ret.GetBuffer());

	return ret;
}

//-----------------------------------------------------------------------
void CPaneConsole::AddAMessageLine( const TCHAR *sText, bool bNewLine )
{
	//gkAutoLock<gkCritcalSectionLock> autolock( eLGID_global, eLID_consolelog );

	//return;
	//m_edtConsoleBox.AddString( sText );
	//m_edtConsoleBox.SendMessage(WM_VSCROLL,SB_PAGEDOWN,0); 


	// richedit add [2/4/2012 Kaiming]

	HWND hWndRichEdit = m_richEdit.GetSafeHwnd();
	if (!hWndRichEdit)
		return;

	uint32 m_vColorTable[10];

	m_vColorTable[0]=XTPColorManager()->GetColor(COLOR_BTNTEXT);//RGB(0,0,0);
	m_vColorTable[1]=XTPColorManager()->GetColor(COLOR_WINDOW);
	m_vColorTable[2]=RGB(0,0,200);// blue
	m_vColorTable[3]=RGB(0,200,0);// green
	m_vColorTable[4]=RGB(200,0,0);// red
	m_vColorTable[5]=RGB(0,200,200);// cyan
	m_vColorTable[6]=RGB(200,180,0);// yellow
	m_vColorTable[7]=RGB(200,0,200);// red+blue
	m_vColorTable[8]=0x000080ff;
	m_vColorTable[9]=0x008f8f8f;

	int iColor = 0;

	//wchar_t buf[MAX_PATH];
	//MultiByteToWideChar(CP_ACP, 0, sText, -1, buf, MAX_PATH);

	CString tempStr = CopyAndRemoveColorCode(sText,iColor);
	if (iColor < 0 || iColor > 9)
		iColor = 0;

	//tempStr = sText;

	// remember selection and the top row
	int len = ::GetWindowTextLength( hWndRichEdit );
	int top, from, to;
	::SendMessage( hWndRichEdit, EM_GETSEL, (WPARAM)&from, (LPARAM)&to );
	bool keepPos = false;
	bool locking = false;

	if ( from!=len || to!=len )
	{
		keepPos = ::GetFocus() == hWndRichEdit;
		if ( keepPos )
		{
			top = ::SendMessage( hWndRichEdit, EM_GETFIRSTVISIBLELINE, 0, 0 );
			locking = bNewLine && ::IsWindowVisible( hWndRichEdit );
			if ( locking )
				m_richEdit.LockWindowUpdate();
		}
		::SendMessage( hWndRichEdit, EM_SETSEL, len, len );
	}
	if ( bNewLine )
	{
		tempStr.TrimRight();
		tempStr = CString(_T("\r\n")) + tempStr;
	}

	CHARFORMAT2 chrfmt;
	// set text color
	m_richEdit.GetSelectionCharFormat( chrfmt );
	chrfmt.cbSize = sizeof(chrfmt);
	chrfmt.dwMask = CFM_COLOR|CFM_BOLD|CFM_FACE;
	chrfmt.dwEffects = CFE_BOLD;
	chrfmt.crTextColor = m_vColorTable[iColor];
	//chrfmt.yHeight = 500;
	_tcscpy(chrfmt.szFaceName,_T("微软雅黑"));//设置字体
// 	if (chrfmt.crTextColor != 0)
// 	{
// 		chrfmt.dwEffects |= (CFE_BOLD);
// 	}
	m_richEdit.SetSelectionCharFormat( chrfmt );

	const TCHAR *szStr = tempStr;
	::SendMessage( hWndRichEdit, EM_REPLACESEL, FALSE, (LPARAM)szStr );

	// restore selection and the top line
	if ( keepPos )
	{
		::SendMessage( hWndRichEdit, EM_SETSEL, from, to );
		top -= ::SendMessage( hWndRichEdit, EM_GETFIRSTVISIBLELINE, 0, 0 );
		::SendMessage( hWndRichEdit, EM_LINESCROLL, 0, (LPARAM)top );
	}
	else
	{
		// Scroll to bottom
		::SendMessage(hWndRichEdit,WM_VSCROLL, /*SB_LINEDOWN*/ SB_BOTTOM, (LPARAM)NULL);
	}

	if ( locking )
		m_richEdit.UnlockWindowUpdate();
}
//-----------------------------------------------------------------------
void CPaneConsole::RegConsoleCallback()
{
	
}

void CPaneConsole::OnSize( UINT nType, int cx, int cy )
{
	CXTResizeDialog::OnSize(nType, cx, cy);

// 	RECT rcEdit;
// 	// Get the size of the client window
// 	GetClientRect(&rcEdit);
// 
// 	int inputH = 18;
// 	int btnWidth = 30;
// 
// 	if (m_richEdit.GetSafeHwnd())
// 		m_richEdit.SetWindowPos(NULL, rcEdit.left, rcEdit.top + 2, rcEdit.right, rcEdit.bottom - 1 - inputH, SWP_NOZORDER);
}

//-----------------------------------------------------------------------
void SConsolePaneLogCallback::OnWriteToConsole( const TCHAR *sText,bool bNewLine )
{
	if (m_pOwner)
	{
		m_pOwner->AddAMessageLine(sText, bNewLine);
	}
}
