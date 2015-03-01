// WorkspaceTitle.cpp : implementation file
//

#include "stdafx.h"
#include "WorkspaceTitle.h"
#include "afxdialogex.h"


// CWorkspaceTitle dialog

IMPLEMENT_DYNAMIC(CWorkspaceTitle, CXTResizeDialog)

CWorkspaceTitle::CWorkspaceTitle(CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(CWorkspaceTitle::IDD, pParent)
{

}

CWorkspaceTitle::~CWorkspaceTitle()
{
}

void CWorkspaceTitle::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_WORKSPACE_SHOWHELPER, m_btnShowHelper);
	DDX_Control(pDX, IDC_WORKSPACETITLE, m_lblTitle);
}


BEGIN_MESSAGE_MAP(CWorkspaceTitle, CXTResizeDialog)
	ON_BN_CLICKED(IDC_WORKSPACE_SHOWHELPER, &CWorkspaceTitle::OnBnClickedWorkspaceShowhelper)
//	ON_WM_CTLCOLOR()
ON_WM_CTLCOLOR()
ON_WM_ERASEBKGND()
END_MESSAGE_MAP()
//////////////////////////////////////////////////////////////////////////
BOOL CWorkspaceTitle::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();

	SetResize(IDC_WORKSPACE_SHOWHELPER, SZ_TOP_RIGHT, SZ_TOP_RIGHT);
	m_wndFont.CreateFont(18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, _T("Segoe UI"));

	m_lblTitle.SetFont(&m_wndFont);
	return TRUE;
}

void CWorkspaceTitle::OnBnClickedWorkspaceShowhelper()
{
	// TODO: Add your control notification handler code here
	bool bSwitch = (gEnv->pCVManager->getCVar(_T("r_aux"))->getInt() != 0);
	if (bSwitch)
	{
		gEnv->pCVManager->executeCommand( _T("r_aux 0"));
	}
	else
	{
		gEnv->pCVManager->executeCommand( _T("r_aux 1"));
	}

	bSwitch = !bSwitch;

}

HBRUSH CWorkspaceTitle::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CXTResizeDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  Change any attributes of the DC here
 	if (nCtlColor == CTLCOLOR_STATIC) {
		pDC->SetBkMode(TRANSPARENT);
//   		pDC->SetBkColor(PALETTERGB(38,91,37));  // yellow
//  		pDC->SetDCBrushColor(PALETTERGB(38,91,37));
//  		pDC->SetDCPenColor(PALETTERGB(38,91,37));
 		pDC->SetTextColor(RGB(255, 255, 255));
 	}

	return hbr;
}

BOOL CWorkspaceTitle::OnEraseBkgnd(CDC* pDC)
{
	BOOL ret = CXTResizeDialog::OnEraseBkgnd(pDC);
	// TODO: Add your message handler code here and/or call default
	CRect rc;
	GetClientRect(&rc);
	pDC->FillSolidRect(&rc,RGB(38,91,137));
	return ret;
}
