// BackstagePagePrint.cpp : implementation file
//

#include "stdafx.h"
#include "ribbonsample.h"
#include "BackstagePagePrint.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBackstagePagePrint property page

IMPLEMENT_DYNCREATE(CBackstagePagePrint, CXTPRibbonBackstagePage)

CBackstagePagePrint::CBackstagePagePrint() : CXTPRibbonBackstagePage(CBackstagePagePrint::IDD)
{
	//{{AFX_DATA_INIT(CBackstagePagePrint)
	m_nCopies = 1;
	//}}AFX_DATA_INIT

	m_pPreviewView = NULL;
	m_pFrameWnd = NULL;
	m_pView = NULL;
}

CBackstagePagePrint::~CBackstagePagePrint()
{
}

void CBackstagePagePrint::DoDataExchange(CDataExchange* pDX)
{
	CXTPRibbonBackstagePage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBackstagePagePrint)
	DDX_Text(pDX, IDC_EDIT_COPIES, m_nCopies);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_SEPARATOR_4, m_lblSeparator4);
	DDX_Control(pDX, IDC_SEPARATOR_1, m_lblSeparator1);
	DDX_Control(pDX, IDC_BUTTON_PRINT, m_btnPrint);

	DDX_Control(pDX, IDC_LABEL_CAPTION, m_lblPrint);

}


BEGIN_MESSAGE_MAP(CBackstagePagePrint, CXTPRibbonBackstagePage)
	//{{AFX_MSG_MAP(CBackstagePagePrint)
	ON_BN_CLICKED(IDC_BUTTON_PRINT, OnButtonPrint)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBackstagePagePrint message handlers



void CBackstagePagePrint::InitButton(UINT nID)
{
	CXTPImageManagerIconHandle hIcon, hIconHot;
	
	hIcon = CXTPImageManagerIcon::LoadBitmapFromResource(MAKEINTRESOURCE(nID), NULL);

	CXTPRibbonBackstageButton* pButton = (CXTPRibbonBackstageButton*)GetDlgItem(nID);
	
	pButton->SetIcon(CSize(32, 32), hIcon, hIconHot);
	pButton->SetTextImageRelation(xtpButtonImageAboveText);
	pButton->ShowShadow(TRUE);
}


BOOL CBackstagePagePrint::OnInitDialog() 
{
	CXTPRibbonBackstagePage::OnInitDialog();
	
	ModifyStyleEx(0, WS_EX_CONTROLPARENT);
	
	
	InitButton(IDC_BUTTON_PRINT);
	m_lblSeparator4.SetVerticalStyle(TRUE);
	
	m_lblPrint.SetFont(&m_fntCaption);
	m_lblPrint.SetTextColor(RGB(94, 94, 94));
	
	SetResize(IDC_SEPARATOR_4, XTP_ANCHOR_TOPLEFT, XTP_ANCHOR_BOTTOMLEFT);


	m_pFrameWnd = new CFrameWnd();
	if (!m_pFrameWnd->Create(NULL, NULL, WS_VISIBLE | WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
		CRect(0, 0, 0, 0), this, NULL, 0, NULL))
	{
		delete m_pFrameWnd;
		return FALSE;
	}
	
	m_pFrameWnd->ModifyStyleEx(WS_EX_CLIENTEDGE, 0);
	
	CXTPWindowRect rc(GetDlgItem(IDC_STATIC_PAGE));
	ScreenToClient(rc);
	
	m_pFrameWnd->SetWindowPos(0, rc.left, rc.top, rc.Width(), rc.Height(), SWP_NOZORDER | SWP_SHOWWINDOW);
	SetResize(0, m_pFrameWnd->GetSafeHwnd(), XTP_ANCHOR_TOPLEFT, XTP_ANCHOR_BOTTOMRIGHT);

	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
	
}

void CBackstagePagePrint::UpdateCopies(BOOL bSaveAndValidate)
{
	HGLOBAL hDevMode = NULL, hDevNames = NULL;
	
	if (!XTPGetPrinterDeviceDefaults(hDevMode, hDevNames) || !hDevMode)
	{
		return;
	}
	
	LPDEVMODE pDevMode = (LPDEVMODE )::GlobalLock(hDevMode);
	ASSERT(pDevMode);
	if (!pDevMode)
		return;
	
	if (bSaveAndValidate)
	{
		UpdateData(bSaveAndValidate);

		pDevMode->dmCopies = m_nCopies;
	}
	else
	{
		m_nCopies = pDevMode->dmCopies;
		UpdateData(FALSE);
	}
	
	::GlobalUnlock(hDevMode);
}

void CBackstagePagePrint::OnButtonPrint() 
{
	UpdateCopies(TRUE);

	CXTPRibbonBackstageView* pView = (CXTPRibbonBackstageView*)GetParent();
	
	pView->OnCancel();

	AfxGetMainWnd()->SendMessage(WM_COMMAND, ID_FILE_PRINT);
	
}


BOOL CBackstagePagePrint::OnSetActive()
{
	if (!CXTPRibbonBackstagePage::OnSetActive())
		return FALSE;

	CXTPRibbonBackstageView* pBackstageView = (CXTPRibbonBackstageView*)GetParent();
	CFrameWnd* pSite = DYNAMIC_DOWNCAST(CFrameWnd, pBackstageView->GetCommandBars()->GetSite());
	
	m_pView = (CBackstagePrintView*)pSite->GetActiveFrame()->GetActiveView();
	if (!m_pView)
		return FALSE;

	if (!CreatePrintPreview())
		return FALSE;

	UpdateCopies(FALSE);

	return TRUE;
}

BOOL CBackstagePagePrint::CreatePrintPreview()
{
	CFrameWnd* pParent = m_pFrameWnd;
	CPrintPreviewState* pState = new CPrintPreviewState;
	
	CCreateContext context;
	context.m_pCurrentFrame = m_pFrameWnd;
	context.m_pCurrentDoc = NULL;
	context.m_pLastView = m_pView;
	
	
	CBackstagePreviewView* pView = new CBackstagePreviewView();
	if (pView == NULL)
	{
		TRACE0("Error: Failed to create preview view.\n");
		return FALSE;
	}
	
	ASSERT_KINDOF(CPreviewView, pView);
	pView->m_pPreviewState = pState;        // save pointer
	
	pView->m_pToolBar = new CDialogBar;
	if (!pView->m_pToolBar->Create(m_pFrameWnd, MAKEINTRESOURCE(XTP_IDD_PREVIEW_DIALOGBAR),
		CBRS_TOP, AFX_IDW_PREVIEW_BAR))
	{
		TRACE0("Error: Preview could not create toolbar dialog.\n");
		delete pView->m_pToolBar;       // not autodestruct yet
		pView->m_pToolBar = NULL;
		pView->m_pPreviewState = NULL;  // do not delete state structure
		delete pView;
		return NULL;
	}
	
	pView->m_pToolBar->ModifyStyle(0, WS_CLIPCHILDREN);
	pView->m_pToolBar->m_bAutoDelete = TRUE;    // automatic cleanup
	
	
	if (!pView->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,
		CRect(0,0,0,0), pParent, AFX_IDW_PANE_FIRST, &context))
	{
		TRACE0("Error: couldn't create preview view for frame.\n");
		
		pView->m_pPreviewState = NULL;  // do not delete state structure
		delete pView;
		
		return NULL;
	}
	
	
	pView->SetPrintView(m_pView);
	
	
	pParent->SetActiveView(pView);  // set active view - even for MDI
	
	m_pPreviewView = pView;
	
	
	pView->m_pCommandBar->SetPaintManager(new CXTPRibbonTheme());
	pView->m_pCommandBar->GetControls()->FindControl(XTP_ID_PREVIEW_PRINT)->SetVisible(FALSE);
	pView->m_pCommandBar->GetControls()->FindControl(XTP_ID_PREVIEW_CLOSE)->SetVisible(FALSE);
	pView->m_clrBackColor = RGB(255, 255, 255);
	pView->ModifyStyleEx(WS_EX_CLIENTEDGE, 0);
	pView->RecalLayout();
	
	
	pParent->RecalcLayout();            // position and size everything
	pParent->UpdateWindow();
	
	m_pPreviewView->SetZoomState(0, 0, CPoint(0, 0));
	
	return TRUE;
}

BOOL CBackstagePagePrint::OnKillActive()
{
	if (!CXTPRibbonBackstagePage::OnKillActive())
		return FALSE;

	if (m_pView && m_pPreviewView)
	{
		m_pPreviewView->m_pToolBar->DestroyWindow();
		m_pPreviewView->m_pToolBar = NULL;

		m_pView->OnEndPrinting(m_pPreviewView->m_pPreviewDC, m_pPreviewView->m_pPreviewInfo);
		m_pView = NULL;
	}

	if (m_pPreviewView)
	{
		m_pPreviewView->DestroyWindow();

		m_pFrameWnd->SetActiveView(NULL);

		m_pPreviewView = NULL;
	}


	return TRUE;
}