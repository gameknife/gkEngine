#include "StdAfx.h"
#include "gkSwissKnifeBar.h"
#include "..\resource.h"

static UINT tabs[] =
{
	IDS_SWISSKNIFEBAR_CREATION,
	IDS_SWISSKNIFEBAR_MODIFY,
	IDS_SWISSKNIFEBAR_DISPLAY,
};

//////////////////////////////////////////////////////////////////////////
void gkSwissKnifeBar::Create( CWnd* parent, CWnd* owner, UINT id )
{
	m_wndShortcutBar.SetTheme(xtpShortcutThemeResource);

	m_wndShortcutBar.Create(WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS, CRect(0, 0, 0, 0), 
		parent,  id);

	m_wndShortcutBar.SetOwner(owner);
	m_wndShortcutBar.GetToolTipContext()->SetStyle(xtpToolTipOffice);

	BOOL bUseAlphaIcons = XTPImageManager()->IsAlphaIconsSupported();

 	XTPImageManager()->SetIcons(bUseAlphaIcons ? IDB_SWISSBAR_SMALLA : IDB_SWISSKNIFE_SMALL , 
 		tabs, sizeof(tabs)/sizeof(UINT), CSize(16, 16));
 
 	XTPImageManager()->SetIcons(bUseAlphaIcons ? IDB_SWISSBAR_BIGA : IDB_SWISSKNIFE_BIG , 
 		tabs, sizeof(tabs)/sizeof(UINT), CSize(24, 24));

 	VERIFY(m_paneCreation.Create(_T("Creation"), &m_wndShortcutBar));
 	VERIFY(m_paneModify.Create(_T("Modification"), &m_wndShortcutBar));
// 	VERIFY(m_paneFolders.Create(_T("Folders"), &m_wndShortcutBar));
// 	VERIFY(m_paneContacts.Create(_T("Contacts"), &m_wndShortcutBar));

	m_itemCreation = m_wndShortcutBar.AddItem(IDS_SWISSKNIFEBAR_CREATION, &m_paneCreation);
	m_itemModify = m_wndShortcutBar.AddItem(IDS_SWISSKNIFEBAR_MODIFY, &m_paneModify);
	m_wndShortcutBar.AddItem(IDS_SWISSKNIFEBAR_DISPLAY);

	m_wndShortcutBar.AllowMinimize(FALSE);
	m_wndShortcutBar.ShowMinimizeButton(FALSE);
	//m_wndShortcutBar.ShowGripper(TRUE);

	m_wndShortcutBar.SelectItem(m_itemCreation);
	m_wndShortcutBar.LoadState(_T("SwissKnifeBar"));
}
//////////////////////////////////////////////////////////////////////////
void gkSwissKnifeBar::Destroy()
{
	m_wndShortcutBar.SaveState(_T("SwissKnifeBar"));
}
//////////////////////////////////////////////////////////////////////////
void gkSwissKnifeBar::RefreshObj( IGameObject* pTarget )
{
	if (pTarget)
	{
		m_wndShortcutBar.SelectItem(m_itemModify);
	}
	

	m_paneModify.RefreshProperties(pTarget);
}
