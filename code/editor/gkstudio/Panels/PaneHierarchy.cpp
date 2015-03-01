#include "StdAfx.h"
#include "PaneHierarchy.h"
#include "EditorUtils.h"
#include "gkObjectManager.h"


//////////////////////////////////////////////////////////////////////////
CPaneHierarchy::CPaneHierarchy(CWnd* pParent)
	: gkToolBarPaneBase(CPaneHierarchy::IDD, pParent)

{
	//{{AFX_DATA_INIT(CPaneStyles)
	//}}AFX_DATA_INIT
}

BEGIN_MESSAGE_MAP(CPaneHierarchy, gkToolBarPaneBase)
	ON_COMMAND_RANGE(ID_HRC_REFRESH, ID_HRC_FOCUS, OnEmptyCommandRange)
	ON_NOTIFY(NM_CLICK, 38985, &CPaneHierarchy::OnTreeClick)
	ON_NOTIFY(NM_DBLCLK, 38985, &CPaneHierarchy::OnTreeDbClick)
END_MESSAGE_MAP()


//////////////////////////////////////////////////////////////////////////
BOOL CPaneHierarchy::OnInitDialog()
{
	gkToolBarPaneBase::OnInitDialog();


	VERIFY(m_wndTreeHierarchy.Create(WS_VISIBLE|TVS_HASBUTTONS|TVS_LINESATROOT, CXTPEmptyRect(), this, 38985));
	m_wndTreeHierarchy.SetItemHeight(18);
	m_wndTreeHierarchy.SetOwner(this);
	// TreeCtrl must be already created.
	CMFCUtils::LoadTrueColorImageList( m_imageList,IDB_SKB_ICONLIST,16,RGB(255,255,255) );
	m_wndTreeHierarchy.SetImageList(&m_imageList,TVSIL_NORMAL);



	SetResize(IDC_TODSLIDER,       SZ_TOP_LEFT, SZ_TOP_RIGHT);

	//RefreshMetrics(TRUE);



 	RegisterPane(&m_wndTreeHierarchy, _T("Tree"), xtpPaneDockBottom);
 	OpenPane(_T("Tree"));
// 
// 	RegisterPane(&m_wndPropertyGrid, _T("Time Of Day Properties"), xtpPaneDockBottom);
// 	OpenPane(_T("Time Of Day Properties"));

	InitToolBar(_T("ToolBar_Hierarchy"), IDR_TOOLBAR_HIERARCHY);

	return TRUE;
}
//////////////////////////////////////////////////////////////////////////
void CPaneHierarchy::RefreshMetrics( BOOL bOffice2007 )
{

}
//////////////////////////////////////////////////////////////////////////
void CPaneHierarchy::OnEmptyCommandRange( UINT ID)
{
	switch (ID)
	{
	case ID_HRC_REFRESH:
		RefreshHierarchy();
		return;
	case ID_HRC_DELETE:
		DeleteSelection();
		return;
	case ID_HRC_FOCUS:
		FocusOnSelection();

		
		return;
	}
}
//////////////////////////////////////////////////////////////////////////
void CPaneHierarchy::RefreshHierarchy()
{
	m_wndTreeHierarchy.DeleteAllItems();

	uint32 count = gEnv->pGameObjSystem->getGameObjectCount();
	uint32 realcount = 0;
	uint32 fullcount = 0;
	for( uint32 i=0; i < count; ++i)
	{
		IGameObject* go = gEnv->pGameObjSystem->GetGameObjectById(i);
		if (go && go->getParent() == NULL && go->getGameObjectSuperClass() != eGOClass_SYSTEM)
		{
			//HTREEITEM hItem = m_wndTreeHierarchy.InsertItem( go->getName().c_str() );
			RefreshChild(go, NULL);
		}
	}
}
//////////////////////////////////////////////////////////////////////////
void CPaneHierarchy::RefreshChild( IGameObject* parent, HTREEITEM hItem )
{
	HTREEITEM hItemSub = m_wndTreeHierarchy.InsertItem( parent->getName().c_str(), 0, 1, hItem );

	// child situation
	uint32 childcount = parent->getChildCount();
	for(uint32 i=0; i < childcount; ++i)
	{
		if (parent->getChild(i) && parent->getChild(i)->getGameObjectSuperClass() != eGOClass_SYSTEM)
		{

			RefreshChild(parent->getChild(i), hItemSub);
		}
	}
}
//////////////////////////////////////////////////////////////////////////
void CPaneHierarchy::OnTreeClick( NMHDR *pNMHDR, LRESULT *pResult )
{
	*pResult = FALSE;

	// Show helper menu.
	CPoint point;
	// Find node under mouse.
	GetCursorPos( &point );
	m_wndTreeHierarchy.ScreenToClient( &point );
	// Select the item that is at the point myPoint.
	UINT uFlags;
	HTREEITEM hItem = m_wndTreeHierarchy.HitTest(point,&uFlags);
	if (hItem )
	{
		CString text = m_wndTreeHierarchy.GetItemText(hItem);
		
		IGameObject* pTarget = gEnv->pGameObjSystem->GetGameObjectByName(gkStdString(text));
		if (pTarget)
		{
			GetIEditor()->getObjectMng()->setCurrSelected( pTarget );
		}		
	}

	*pResult = 0;
}

void CPaneHierarchy::OnTreeDbClick( NMHDR *pNMHDR, LRESULT *pResult )
{
	FocusOnSelection();
}

void CPaneHierarchy::FocusOnSelection()
{
	GetIEditor()->focusGameObject( GetIEditor()->getObjectMng()->getCurrSelection() );
}

void CPaneHierarchy::DeleteSelection()
{
	IGameObject* pSelection = GetIEditor()->getObjectMng()->getCurrSelection();

	gEnv->pGameObjSystem->DestoryGameObject(pSelection);

	GetIEditor()->getObjectMng()->setCurrSelected(NULL);
}
