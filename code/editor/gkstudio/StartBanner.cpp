// StartBanner.cpp : implementation file
//

#include "stdafx.h"
#include "StartBanner.h"
#include "afxdialogex.h"

CStartBanner *CStartBanner::s_pLogoWindow = 0;
// StartBanner dialog

IMPLEMENT_DYNAMIC(CStartBanner, CDialog)

CStartBanner::CStartBanner(CWnd* pParent /*=NULL*/)
	: CDialog(CStartBanner::IDD, pParent)
{

}

CStartBanner::~CStartBanner()
{
}

void CStartBanner::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STARTTEXT, m_lblStartText);
}

BEGIN_MESSAGE_MAP(CStartBanner, CDialog)
END_MESSAGE_MAP()


// StartBanner message handlers


BOOL CStartBanner::OnInitDialog() 
{
	s_pLogoWindow = this;

	CDialog::OnInitDialog();

	CRect rcBmp, rcDlg, rcTxt;

	GetWindowRect(rcDlg);
	GetDlgItem(IDC_STARTTEXT)->GetWindowRect(rcTxt);
	GetDlgItem(IDC_STATIC)->GetWindowRect(rcBmp);
	rcTxt.OffsetRect(rcBmp.Width()+2 - rcDlg.Width(), 0);
	ScreenToClient(rcTxt);
	SetWindowPos(NULL, 0, 0, rcBmp.Width(), rcBmp.Height() + 20, SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOOWNERZORDER|SWP_NOZORDER);
	GetDlgItem(IDC_STARTTEXT)->SetWindowPos(NULL, rcBmp.left + 20, rcTxt.top, 0, 0, SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOOWNERZORDER|SWP_NOZORDER);

	SetWindowText(_T("Starting gkEngine Studio...") );

	SetInfo(_T("Starting gkENGINE Studio..."));

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
//////////////////////////////////////////////////////////////////////////
void CStartBanner::SetInfo( const TCHAR *text )
{
	m_lblStartText.SetWindowText(text);
}
//////////////////////////////////////////////////////////////////////////
void CStartBanner::SetText( const TCHAR *text )
{
	if (s_pLogoWindow)
	{
		s_pLogoWindow->SetInfo(text);
	}
}
