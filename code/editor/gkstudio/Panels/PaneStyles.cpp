// PaneStyles.cpp : implementation file
//

#include "stdafx.h"
#include "ribbonsample.h"
#include "PaneStyles.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CPaneStyles dialog


CPaneObject::CPaneObject(CWnd* pParent /*=NULL*/)
	: IPaneBase(CPaneObject::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPaneStyles)
	//}}AFX_DATA_INIT
}


void CPaneObject::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPaneStyles)
	DDX_Control(pDX, IDC_LIST_STYLES, m_lstStyles);
	// 	DDX_Control(pDX, IDC_CHECK_LINKEDSTYLE, m_chkLinkedStyle);
	// 	DDX_Control(pDX, IDC_CHECK_PREVIEW, m_chkPreview);
	DDX_Control(pDX, IDC_BUTTON_STYLEINSPECTOR, m_btnStyleInspector);
	DDX_Control(pDX, IDC_BUTTON_NEWSTYLE, m_btnNewStyle);
	DDX_Control(pDX, IDC_BUTTON_MANAGESTYLES, m_btnManageStyles);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_PANEOBJ_PLACEHOLDER, m_wndPlaceholder);
}


BEGIN_MESSAGE_MAP(CPaneObject, IPaneBase)
	//{{AFX_MSG_MAP(CPaneStyles)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPaneStyles message handlers

void CPaneObject::OnCancel()
{
}

void CPaneObject::OnOK()
{
}

HBRUSH CPaneObject::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	return m_brush;
}

BOOL CPaneObject::OnInitDialog() 
{
	CXTResizeDialog::OnInitDialog();
	
	SetResize(IDC_LIST_STYLES,       SZ_TOP_LEFT, SZ_MIDDLE_RIGHT);

// 	SetResize(IDC_CHECK_PREVIEW,     SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
// 	SetResize(IDC_CHECK_LINKEDSTYLE,     SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
	SetResize(IDC_BUTTON_NEWSTYLE,     SZ_TOP_LEFT, SZ_TOP_CENTER);
	SetResize(IDC_BUTTON_STYLEINSPECTOR,     SZ_TOP_CENTER, SZ_TOP_RIGHT);
	SetResize(IDC_BUTTON_MANAGESTYLES,     SZ_TOP_LEFT, SZ_TOP_CENTER);
	SetResize(IDC_BUTTON_MANAGESTYLES2,     SZ_TOP_CENTER, SZ_TOP_RIGHT);

 	m_lstStyles.AddString(_T("点光源"));
 	m_lstStyles.AddString(_T("物理体"));
//  	m_lstStyles.AddString(_T("No Spacing"));
//  	m_lstStyles.AddString(_T("Heading 1"));
//  	m_lstStyles.AddString(_T("Heading 2"));
//  	m_lstStyles.AddString(_T("Heading 3"));
//  	m_lstStyles.AddString(_T("Title"));
//  	m_lstStyles.AddString(_T("Subtitle"));


	CXTPImageManagerIconHandle ih;
	CXTPImageManagerIconHandle ihh;
// 	
// 	ih = CXTPImageManagerIcon::LoadBitmapFromResource(MAKEINTRESOURCE(IDC_BUTTON_NEWSTYLE), NULL);
// 	m_btnNewStyle.SetIcon(CSize(16, 16), ih, ihh);
// 
// 	ih = CXTPImageManagerIcon::LoadBitmapFromResource(MAKEINTRESOURCE(IDC_BUTTON_MANAGESTYLES), NULL);
// 	m_btnManageStyles.SetIcon(CSize(16, 16), ih, ihh);
// 
// 	ih = CXTPImageManagerIcon::LoadBitmapFromResource(MAKEINTRESOURCE(IDC_BUTTON_STYLEINSPECTOR), NULL);
// 	m_btnStyleInspector.SetIcon(CSize(16, 16), ih, ihh);

	RefreshMetrics(TRUE);

	InitPropertyTable();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPaneObject::RefreshMetrics(BOOL bOffice2007)
{
	m_brush.DeleteObject();

	if (bOffice2007)
	{
		COLORREF clr = XTPResourceImages()->GetImageColor(_T("DockingPane"), _T("WordPaneBackground"));
		m_brush.CreateSolidBrush(clr);

		m_chkLinkedStyle.SetTheme(xtpButtonThemeOffice2007);
		m_chkPreview.SetTheme(xtpButtonThemeOffice2007);
		m_btnStyleInspector.SetTheme(xtpButtonThemeOffice2007);
		m_btnNewStyle.SetTheme(xtpButtonThemeOffice2007);
		m_btnManageStyles.SetTheme(xtpButtonThemeOffice2007);

		m_lstStyles.SetListStyle(xtListBoxOffice2007);

	}
	else
	{
		m_brush.CreateSolidBrush(GetSysColor(COLOR_3DFACE));

		m_chkLinkedStyle.SetTheme(xtpButtonThemeOfficeXP);
		m_chkPreview.SetTheme(xtpButtonThemeOfficeXP);
		m_btnStyleInspector.SetTheme(xtpButtonThemeOfficeXP);
		m_btnNewStyle.SetTheme(xtpButtonThemeOfficeXP);
		m_btnManageStyles.SetTheme(xtpButtonThemeOfficeXP);

		m_lstStyles.SetListStyle(xtListBoxOfficeXP);
	}
}



BOOL CPaneObject::InitPropertyTable()
{
	CRect rc;
	m_wndPlaceholder.GetWindowRect( &rc );
	ScreenToClient( &rc );

	// create the property grid.
	if ( m_wndPropertyGrid.Create( rc, this, IDC_PANEOBJPROPERTY ) )
	{
		m_wndPropertyGrid.SetVariableItemsHeight(TRUE);

		LOGFONT lf;
		GetFont()->GetLogFont( &lf );


		CXTPPropertyGridItem* pItem = NULL;

		// create global settings category.
		CXTPPropertyGridItem* pGlobals      = m_wndPropertyGrid.AddCategory(_T("公有属性"));
		CXTPPropertyGridItem* pSpecific      = m_wndPropertyGrid.AddCategory(_T("私有属性"));

	}

	SetResize(IDC_PANEOBJPROPERTY, SZ_MIDDLE_LEFT, SZ_BOTTOM_RIGHT);

	return TRUE;
}
