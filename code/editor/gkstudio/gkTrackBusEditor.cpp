#include "stdafx.h"
#include "gkTrackBusEditor.h"

gkTrackBusEditor::gkTrackBusEditor(CWnd* pParent /*=NULL*/)
	: gkToolBarPaneBase(0, pParent)
{
}

gkTrackBusEditor::~gkTrackBusEditor()
{
}

BEGIN_MESSAGE_MAP(gkTrackBusEditor, gkToolBarPaneBase)
	ON_MESSAGE(XTPWM_PROPERTYGRID_NOTIFY, OnGridNotify)
	ON_WM_SIZE()

	ON_COMMAND_RANGE(ID_BUTTON_MATEDIT_GET, ID_BUTTON_MATEDT_CATE, OnEmptyCommandRange)
	//ON_NOTIFY(NM_CLICK, AFX_IDW_PANE_FIRST, &gkMaterialEditor::OnTreeBrowserClick)
END_MESSAGE_MAP()

BOOL gkTrackBusEditor::InitPropertyTable()
{

	return TRUE;
}

BOOL gkTrackBusEditor::OnInitDialog()
{
	gkToolBarPaneBase::OnInitDialog();

	SetResize(IDC_MATEDTLIST,       SZ_TOP_LEFT, SZ_BOTTOM_CENTER);
	//SetResize(IDC_MATEDTGRID,       SZ_TOP_CENTER, SZ_BOTTOM_LEFT);
	RefreshMetrics(TRUE);



	// 	CRect rc;
	// 	GetClientRect(rc)
	// 
	// 	m_wndPlaceHolder.Create(_T(""), WS_CHILD | WS_VISIBLE, rc, this);


	m_treeMaterialList.Create( WS_VISIBLE|WS_CHILD|WS_TABSTOP|WS_BORDER|TVS_HASBUTTONS|TVS_SHOWSELALWAYS|TVS_LINESATROOT|TVS_HASLINES|
		TVS_FULLROWSELECT|TVS_INFOTIP,
		CRect(0,0,100,400), this, AFX_IDW_PANE_FIRST );

	m_treeMaterialList.SetItemHeight(18);
	// TreeCtrl must be already created.
	//CMFCUtils::LoadTrueColorImageList( m_imageList,IDB_MATEDTLIST,16,RGB(255,0,255) );
	//CMFCUtils::LoadTrueColorImageList( m_imageList,IDB_FILE_STATUS,20,RGB(255,0,255) );
	m_treeMaterialList.SetImageList(&m_imageList,TVSIL_NORMAL);

	InitPropertyTable();

	RegisterPane(&m_treeMaterialList, _T("ActorList"), xtpPaneDockLeft);
	OpenPane(_T("MaterialBrowser"));

	RegisterPane(&m_wndPropertyGrid, _T("TrackView"), xtpPaneDockRight);
	OpenPane(_T("MaterialProperty"));
	// 	RegisterPane(&m_treeMaterialList, _T("MaterialBrowser2"));
	// 	OpenPane(_T("MaterialBrowser2"));

	InitToolBar(_T("ToolBar_MatEdt"), IDR_TOOLBAR_MATERIALEDITOR);

	return TRUE;
}

void gkTrackBusEditor::OnEmptyCommandRange( UINT )
{

}

LRESULT gkTrackBusEditor::OnGridNotify( WPARAM wParam, LPARAM lParam )
{
	return 0;
}

void gkTrackBusEditor::OnTreeBrowserClick( NMHDR *pNMHDR, LRESULT *pResult )
{

}

void gkTrackBusEditor::RefreshMetrics( BOOL bOffice2007 )
{

}


