// BackstagePageSave.cpp : implementation file
//

#include "stdafx.h"
#include "ribbonsample.h"
#include "BackstagePageSave.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBackstagePageSave property page

CBackstagePageSave::CBackstagePageSave() : CXTPRibbonBackstagePage(CBackstagePageSave::IDD)
{
	//{{AFX_DATA_INIT(CBackstagePageSave)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	UINT nIDs[] = {IDC_BUTTON_SEND_EMAIL, IDC_BUTTON_SAVE_WEB, IDC_BUTTON_SAVE_SHAREPOINT, 
		IDC_BUTTON_PUBLISH, IDC_BUTTON_CHANGEFILETYPES, IDC_BUTTON_CREATE_PDF};
	m_imagelist.SetIcons(IDD_BACKSTAGEPAGE_SAVE, nIDs, 6, CSize(32, 32), xtpImageNormal);

	m_pSelectedButton = NULL;
}

CBackstagePageSave::~CBackstagePageSave()
{
}

void CBackstagePageSave::DoDataExchange(CDataExchange* pDX)
{
	CXTPRibbonBackstagePage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBackstagePageSave)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_SEPARATOR_1, m_lblSeparator1);
	DDX_Control(pDX, IDC_SEPARATOR_2, m_lblSeparator2);
	DDX_Control(pDX, IDC_SEPARATOR_4, m_lblSeparator4);

	DDX_Control(pDX, IDC_LABEL_CAPTION, m_lblSaveAndSend);
	DDX_Control(pDX, IDC_LABEL_FILETYPES, m_lblFileTypes);

	DDX_Control(pDX, IDC_BUTTON_SEND_EMAIL, m_btnSendEmail);
	DDX_Control(pDX, IDC_BUTTON_SAVE_WEB, m_btnSaveToWeb);
	DDX_Control(pDX, IDC_BUTTON_SAVE_SHAREPOINT, m_btnSaveToSharePoint);
	DDX_Control(pDX, IDC_BUTTON_PUBLISH, m_btnPublish);
	DDX_Control(pDX, IDC_BUTTON_CHANGEFILETYPES, m_btnChangeFileType);
	DDX_Control(pDX, IDC_BUTTON_CREATE_PDF, m_btnCreatePDF);
}


BEGIN_MESSAGE_MAP(CBackstagePageSave, CXTPRibbonBackstagePage)
	//{{AFX_MSG_MAP(CBackstagePageSave)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
	ON_BN_SETFOCUS(IDC_BUTTON_SEND_EMAIL, OnButtonSelected)
	ON_BN_SETFOCUS(IDC_BUTTON_SAVE_WEB, OnButtonSelected)
	ON_BN_SETFOCUS(IDC_BUTTON_SAVE_SHAREPOINT, OnButtonSelected)
	ON_BN_SETFOCUS(IDC_BUTTON_PUBLISH, OnButtonSelected)
	ON_BN_SETFOCUS(IDC_BUTTON_CHANGEFILETYPES, OnButtonSelected)
	ON_BN_SETFOCUS(IDC_BUTTON_CREATE_PDF, OnButtonSelected)
END_MESSAGE_MAP()

void CBackstagePageSave::InitButton(CXTPRibbonBackstageButton& btn)
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
	btn.SetTabStyle(TRUE);

}

/////////////////////////////////////////////////////////////////////////////
// CBackstagePageSave message handlers

BOOL CBackstagePageSave::OnInitDialog() 
{
	CXTPRibbonBackstagePage::OnInitDialog();
	
	ModifyStyleEx(0, WS_EX_CONTROLPARENT);
	
	
	m_lblSaveAndSend.SetFont(&m_fntCaption);
	m_lblSaveAndSend.SetTextColor(RGB(94, 94, 94));
	
	m_lblFileTypes.SetFont(&m_fntCaption);
	m_lblFileTypes.SetTextColor(RGB(94, 94, 94));

	m_lblSeparator4.SetVerticalStyle(TRUE);

	InitButton(m_btnSendEmail);
	InitButton(m_btnSaveToWeb);
	InitButton(m_btnSaveToSharePoint);
	InitButton(m_btnPublish);
	InitButton(m_btnChangeFileType);
	InitButton(m_btnCreatePDF);
	
	SetResize(IDC_SEPARATOR_4, XTP_ANCHOR_TOPLEFT, XTP_ANCHOR_BOTTOMLEFT);


	CXTPWindowRect rcPage(GetDlgItem(IDC_STATIC_PAGE));
	ScreenToClient(rcPage);
	
	m_pageSaveEmail.Create(CBackstagePageSaveEmail::IDD, this);
	m_pageSaveEmail.SetDlgCtrlID(CBackstagePageSaveEmail::IDD);

	m_pageSaveEmail.MoveWindow(rcPage);
	m_pageSaveEmail.ShowWindow(SW_SHOWNORMAL);
	
	m_pageSaveWeb.Create(CBackstagePageSaveWeb::IDD, this);
	m_pageSaveWeb.MoveWindow(rcPage);
	m_pageSaveWeb.SetDlgCtrlID(CBackstagePageSaveWeb::IDD);

	SetResize(CBackstagePageSaveEmail::IDD, XTP_ANCHOR_TOPLEFT, XTP_ANCHOR_BOTTOMRIGHT);
	SetResize(CBackstagePageSaveWeb::IDD, XTP_ANCHOR_TOPLEFT, XTP_ANCHOR_BOTTOMRIGHT);


	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

CXTPRibbonBackstagePage* CBackstagePageSave::GetButtonPage(CXTPRibbonBackstageButton* pButton)
{
	if (pButton == &m_btnSendEmail)
		return &m_pageSaveEmail;
	
	if (pButton == &m_btnSaveToWeb)
		return &m_pageSaveWeb;
	return NULL;
}

void CBackstagePageSave::OnButtonSelected()
{
	CXTPRibbonBackstageButton* pButton = DYNAMIC_DOWNCAST(	CXTPRibbonBackstageButton, GetFocus());
	if (!pButton)
		return;

	if (m_pSelectedButton == pButton)
		return;

	if (m_pSelectedButton)
	{
		m_pSelectedButton->SetChecked(FALSE);
	}

	pButton->SetChecked(TRUE);

	CXTPRibbonBackstagePage* pPage = GetButtonPage(m_pSelectedButton);
	if (pPage)
	{
		pPage->ShowWindow(SW_HIDE);
	}

	m_pSelectedButton = pButton;


	pPage = GetButtonPage(m_pSelectedButton);
	if (pPage)
	{
		pPage->ShowWindow(SW_SHOWNORMAL);
	}
}