// BackstagePageHelp.cpp : implementation file
//

#include "stdafx.h"
#include "ribbonsample.h"
#include "BackstagePageHelp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBackstagePageHelp property page

CBackstagePageHelp::CBackstagePageHelp() : CXTPRibbonBackstagePage(CBackstagePageHelp::IDD)
{
	//{{AFX_DATA_INIT(CBackstagePageHelp)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT


	UINT nIDs[] = {IDC_BUTTON_HELP, IDC_BUTTON_GETTING_STARTED, IDC_BUTTON_CONTACT_US, IDC_BUTTON_OPTIONS, IDC_BUTTON_CHECK_FOR_UPDATES};
	m_imagelist.SetIcons(IDD_BACKSTAGEPAGE_HELP, nIDs, 5, CSize(64, 64), xtpImageNormal);

}

CBackstagePageHelp::~CBackstagePageHelp()
{
}

void CBackstagePageHelp::DoDataExchange(CDataExchange* pDX)
{
	CXTPRibbonBackstagePage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBackstagePageHelp)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP

	DDX_Control(pDX, IDC_SEPARATOR_1, m_lblSeparator1);
	DDX_Control(pDX, IDC_SEPARATOR_2, m_lblSeparator2);
	DDX_Control(pDX, IDC_SEPARATOR_3, m_lblSeparator3);
	DDX_Control(pDX, IDC_SEPARATOR_4, m_lblSeparator4);

	DDX_Control(pDX, IDC_BUTTON_HELP, m_btnHelp);
	DDX_Control(pDX, IDC_BUTTON_GETTING_STARTED, m_btnGettingStarted);
	DDX_Control(pDX, IDC_BUTTON_CONTACT_US, m_btnContactUs);
	DDX_Control(pDX, IDC_BUTTON_OPTIONS, m_btnOptions);
	DDX_Control(pDX, IDC_BUTTON_CHECK_FOR_UPDATES, m_btnCheckForUpdates);

	DDX_Control(pDX, IDC_LABEL_CAPTION, m_lblSupport);
	DDX_Control(pDX, IDC_LABEL_TOOLS, m_lblTools);
	DDX_Control(pDX, IDC_LABEL_ABOUT, m_lblAbout);
}


BEGIN_MESSAGE_MAP(CBackstagePageHelp, CXTPRibbonBackstagePage)
	//{{AFX_MSG_MAP(CBackstagePageHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBackstagePageHelp message handlers

void CBackstagePageHelp::InitButton(CXTPRibbonBackstageButton& btn)
{

	UINT nIDIcon = btn.GetDlgCtrlID();

	CXTPImageManagerIcon* pIcon = m_imagelist.GetImage(nIDIcon);
	ASSERT(pIcon);

	btn.SetIcon(pIcon->GetExtent(), pIcon);
	pIcon->InternalAddRef();

	btn.SetTextAlignment(BS_LEFT | BS_VCENTER);
	btn.SetImageAlignment(BS_LEFT | BS_VCENTER);

	btn.SetFlatStyle(TRUE);
	btn.ShowShadow(FALSE);


#if 1 // Optionally Format color and Margins with Markup

	btn.EnableMarkup(TRUE);

	CString strWindowText;
	btn.GetWindowText(strWindowText);


	CString strTitle, strDescription;
	AfxExtractSubString(strTitle, strWindowText, 0);
	AfxExtractSubString(strDescription, strWindowText, 1);

	strWindowText.Format(_T("<StackPanel Margin='5, 3, 5, 3'><TextBlock>%s</TextBlock><TextBlock TextWrapping='Wrap' Foreground='#6f6f6f' Margin='0, 3, 0, 0'>%s</TextBlock></StackPanel>"), strTitle, strDescription);
	btn.SetWindowText(strWindowText);

#endif 
}


BOOL CBackstagePageHelp::OnInitDialog() 
{
	CXTPRibbonBackstagePage::OnInitDialog();

	ModifyStyleEx(0, WS_EX_CONTROLPARENT);

	InitButton(m_btnHelp);
	InitButton(m_btnGettingStarted);
	InitButton(m_btnContactUs);
	InitButton(m_btnOptions);
	InitButton(m_btnCheckForUpdates);

	
	m_lblTools.SetFont(&m_fntCaption);
	m_lblTools.SetTextColor(RGB(94, 94, 94));

	m_lblSupport.SetFont(&m_fntCaption);
	m_lblSupport.SetTextColor(RGB(94, 94, 94));

	m_lblAbout.SetFont(&m_fntCaption);
	m_lblAbout.SetTextColor(RGB(94, 94, 94));

	m_lblSeparator4.SetVerticalStyle(TRUE);

	CString strVersion;
	strVersion.Format(_T("Version: %d.%d.%d\n(c) 1998-%d Codejock Software, All Rights Reserved"), 
		_XTPLIB_VERSION_MAJOR, _XTPLIB_VERSION_MINOR, _XTPLIB_VERSION_REVISION, COleDateTime::GetCurrentTime().GetYear());

	SetDlgItemText(IDC_TEXT1, strVersion);

	SetResize(IDC_SEPARATOR_4, XTP_ANCHOR_TOPLEFT, XTP_ANCHOR_BOTTOMLEFT);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE

}