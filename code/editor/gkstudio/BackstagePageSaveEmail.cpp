// BackstagePageSaveEmail.cpp : implementation file
//

#include "stdafx.h"
#include "ribbonsample.h"
#include "BackstagePageSaveEmail.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBackstagePageSaveEmail property page

IMPLEMENT_DYNCREATE(CBackstagePageSaveEmail, CXTPRibbonBackstagePage)

CBackstagePageSaveEmail::CBackstagePageSaveEmail() : CXTPRibbonBackstagePage(CBackstagePageSaveEmail::IDD)
{
	//{{AFX_DATA_INIT(CBackstagePageSaveEmail)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CBackstagePageSaveEmail::~CBackstagePageSaveEmail()
{
}

void CBackstagePageSaveEmail::DoDataExchange(CDataExchange* pDX)
{
	CXTPRibbonBackstagePage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBackstagePageSaveEmail)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP

	DDX_Control(pDX, IDC_SEPARATOR_1, m_lblSeparator);
	DDX_Control(pDX, IDC_LABEL_CAPTION, m_lblCaption);
	DDX_Control(pDX, IDC_BUTTON_SEND, m_btnSend);

}


BEGIN_MESSAGE_MAP(CBackstagePageSaveEmail, CXTPRibbonBackstagePage)
	//{{AFX_MSG_MAP(CBackstagePageSaveEmail)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBackstagePageSaveEmail message handlers

BOOL CBackstagePageSaveEmail::OnInitDialog() 
{
	CXTPRibbonBackstagePage::OnInitDialog();

	ModifyStyleEx(0, WS_EX_CONTROLPARENT);
	
	m_lblCaption.SetFont(&m_fntCaption);
	m_lblCaption.SetTextColor(RGB(94, 94, 94));

	m_btnSend.ShowShadow(TRUE);

	SetResize(IDC_SEPARATOR_1, XTP_ANCHOR_TOPLEFT, XTP_ANCHOR_TOPRIGHT);	
	
	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
