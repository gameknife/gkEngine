// BackstagePageRecent.cpp : implementation file
//

#include "stdafx.h"
#include "ribbonsample.h"
#include "BackstagePageRecent.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBackstagePageRecent property page

IMPLEMENT_DYNCREATE(CBackstagePageRecent, CXTPRibbonBackstagePage)

CBackstagePageRecent::CBackstagePageRecent() : CXTPRibbonBackstagePage(CBackstagePageRecent::IDD)
{
	//{{AFX_DATA_INIT(CBackstagePageRecent)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CBackstagePageRecent::~CBackstagePageRecent()
{
}

void CBackstagePageRecent::DoDataExchange(CDataExchange* pDX)
{
	CXTPRibbonBackstagePage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBackstagePageRecent)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP

	DDX_Control(pDX, IDC_SEPARATOR_1, m_lblSeparator1);
	DDX_Control(pDX, IDC_LABEL_CAPTION, m_lblRecent);

	DDX_Control(pDX, IDC_RECENTFILELIST, m_wndList);

}


BEGIN_MESSAGE_MAP(CBackstagePageRecent, CXTPRibbonBackstagePage)
	//{{AFX_MSG_MAP(CBackstagePageRecent)
	//}}AFX_MSG_MAP

	ON_BN_CLICKED(IDC_RECENTFILELIST, OnRecentFileListClick)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBackstagePageRecent message handlers

BOOL CBackstagePageRecent::OnInitDialog() 
{
	CXTPRibbonBackstagePage::OnInitDialog();
	
	ModifyStyleEx(0, WS_EX_CONTROLPARENT);
	
	
	m_lblRecent.SetFont(&m_fntCaption);
	m_lblRecent.SetTextColor(RGB(94, 94, 94));
	

	SetResize(IDC_SEPARATOR_1, XTP_ANCHOR_TOPLEFT, XTP_ANCHOR_TOPRIGHT);
	SetResize(IDC_RECENTFILELIST, XTP_ANCHOR_TOPLEFT, XTP_ANCHOR_BOTTOMRIGHT);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


BOOL CBackstagePageRecent::OnSetActive()
{
	m_wndList.BuildItems();


	return TRUE;
}

void CBackstagePageRecent::OnRecentFileListClick()
{
	CXTPRibbonBackstageView* pView = (CXTPRibbonBackstageView*)GetParent();
	
	pView->OnCancel();


	CXTPControlGalleryItem* pItem = m_wndList.GetGallery()->GetItem(m_wndList.GetGallery()->GetSelectedItem());
	
	if (pItem)
	{
		AfxGetMainWnd()->SendMessage(WM_COMMAND, pItem->GetID());		
	}
}
