#include "StdAfx.h"
#include "gkPaneManager.h"
#include "gkPaneBase.h"



//////////////////////////////////////////////////////////////////////////
gkPaneManager::gkPaneManager()
{
	m_nPanelIDCounter = 0;
}
//////////////////////////////////////////////////////////////////////////
void gkPaneManager::RegisterPanelDialog( IPaneBase* panel )
{
	if (panel)
	{
		// find if it is in map
		PanelMap::iterator it = m_mapPanels.find( panel->getPanelName() );
		if (it == m_mapPanels.end())
		{
			// not find, insert
			PanelInfo paneinfo;
			paneinfo.m_paneID = ++m_nPanelIDCounter;
			paneinfo.m_paneType = ePT_PaneBase;
			paneinfo.m_paneTarget = panel;
			m_mapPanels.insert( PanelMap::value_type( panel->getPanelName(), paneinfo ) );
			panel->Create(panel->getIID(), GetParent());
		}
		else
		{
			// exist, delete it
			SAFE_DELETE( panel );
		}		
	}
}
//////////////////////////////////////////////////////////////////////////
void gkPaneManager::RegisterPanelManmual( CWnd* ctrl, const TCHAR* name )
{
	// find if it is in map
	PanelMap::iterator it = m_mapPanels.find( name );
	if (it == m_mapPanels.end())
	{
		// not find, insert
		PanelInfo paneinfo;
		paneinfo.m_paneID = ++m_nPanelIDCounter;
		paneinfo.m_paneType = ePT_Ctrl;
		paneinfo.m_paneTarget = ctrl;
		m_mapPanels.insert( PanelMap::value_type( name, paneinfo ) );
		//panel->Create(panel->getIID(), GetParent());
	}
}

//////////////////////////////////////////////////////////////////////////
void gkPaneManager::LinkPanel( const TCHAR* name )
{
	PanelMap::iterator it = m_mapPanels.find(name);

	if(it != m_mapPanels.end())
	{
		// find it, check if it attach to a panel
		PanelInfo& paneinfo = it->second;
		CXTPDockingPane* pane = FindPane(paneinfo.m_paneID);
		if (pane)
		{
			pane->SetTitle(name);
			pane->SetOptions(xtpPaneNoHideable | xtpPaneNoFloatableByTabDoubleClick);
			pane->Attach( paneinfo.m_paneTarget );
		}
	}
}


//////////////////////////////////////////////////////////////////////////
void gkPaneManager::OpenPanel( const TCHAR* name )
{
	PanelMap::iterator it = m_mapPanels.find(name);

	if(it != m_mapPanels.end())
	{
		// find it, check if it attach to a panel
		PanelInfo& paneinfo = it->second;
		CXTPDockingPane* pane = FindPane(paneinfo.m_paneID);
		if (pane)
		{
			pane->SetTitle(name);
			pane->SetOptions(xtpPaneNoHideable | xtpPaneNoFloatableByTabDoubleClick);
			pane->Attach( paneinfo.m_paneTarget );
			if( pane->IsClosed() )
			{
				ShowPane(pane);
			}
		}
		else
		{
			// all right, it is not attach to a docking panel, create and attach
			pane = CreatePane(	paneinfo.m_paneID, CRect(0, 0,380, 120), xtpPaneDockLeft);
			pane->SetTitle(name);
			pane->SetOptions(xtpPaneNoHideable | xtpPaneNoFloatableByTabDoubleClick);
			pane->Attach( paneinfo.m_paneTarget );


		}

	}
}
//////////////////////////////////////////////////////////////////////////
void gkPaneManager::ClosePanel( const TCHAR* name )
{
	PanelMap::iterator it = m_mapPanels.find(name);

	if(it != m_mapPanels.end())
	{
		// find it, check if it attach to a panel
		PanelInfo& paneinfo = it->second;
		CXTPDockingPane* pane = FindPane(paneinfo.m_paneID);
		if (pane)
		{
			if( !pane->IsClosed() )
			{
				ClosePane(pane);
			}
		}	
	}
}
//////////////////////////////////////////////////////////////////////////
void gkPaneManager::DestroyPanel( const TCHAR* name )
{

}
//////////////////////////////////////////////////////////////////////////
void gkPaneManager::Init(CWnd* parent)
{
	InstallDockingPanes(parent);

	//SetTheme(xtpPaneThemeVisualStudio2010);
	SetCustomTheme( new CXTPDockingPaneVisualStudio2010CustomTheme );
	
	SetThemedFloatingFrames(TRUE);
	SetShowContentsWhileDragging(TRUE);
	SetAlphaDockingContext(TRUE);
	SetShowDockingContextStickers(TRUE);
	SetThemedFloatingFrames(TRUE);
	SetShowContentsWhileDragging(TRUE);

	// Load the previous state for docking panes.
	CXTPDockingPaneLayout layoutNormal(this);
	if (layoutNormal.Load(_T("NormalLayout")))
	{
		SetLayout(&layoutNormal);
	}

	// Link the registered Panes
	PanelMap::iterator it = m_mapPanels.begin();
	for (; it != m_mapPanels.end(); ++it)
	{
		PanelInfo& paneinfo = it->second;
		if (paneinfo.m_paneType == ePT_PaneBase)
		{
			LinkPanel(it->first);
		}
	}

}

//////////////////////////////////////////////////////////////////////////
void gkPaneManager::Destroy()
{
	CXTPDockingPaneLayout layoutNormal(this);
	GetLayout(&layoutNormal);
	layoutNormal.Save(_T("NormalLayout"));

	PanelMap::iterator it = m_mapPanels.begin();
	for (; it != m_mapPanels.end(); ++it)
	{
		PanelInfo& paneinfo = it->second;
		if (paneinfo.m_paneType == ePT_PaneBase)
		{

			SAFE_DELETE( paneinfo.m_paneTarget ); 

		}
	}


}

//////////////////////////////////////////////////////////////////////////
void gkPaneManager::RefreshMetrics( BOOL bOffice2007 )
{
// 	if (bOffice2007)
// 	{
// 		SetTheme(xtpPaneThemeVisualStudio2010);
// 	}
// 	else
// 	{
// 		SetTheme(xtpPaneThemeVisualStudio2003);
// 	}

	GetPaintManager()->RefreshMetrics();

	PanelMap::iterator it = m_mapPanels.begin();
	for (; it != m_mapPanels.end(); ++it)
	{
		PanelInfo& paneinfo = it->second;
		if (paneinfo.m_paneType == ePT_PaneBase)
		{
			IPaneBase* panebase = static_cast<IPaneBase*>(paneinfo.m_paneTarget);
			panebase->RefreshMetrics(bOffice2007);
		}
		//it->second->RefreshMetrics(bOffice2007);
	}

	RedrawPanes();

	it = m_mapPanels.begin();
	for (; it != m_mapPanels.end(); ++it)
	{
		PanelInfo& paneinfo = it->second;
		if (paneinfo.m_paneType == ePT_PaneBase)
		{
			IPaneBase* panebase = static_cast<IPaneBase*>(paneinfo.m_paneTarget);
			panebase->Invalidate();
		}
		//it->second->Invalidate();
	}
}
//////////////////////////////////////////////////////////////////////////
bool gkPaneManager::IsPanelOpen( const TCHAR* name )
{
	PanelMap::iterator it = m_mapPanels.find(name);

	if(it != m_mapPanels.end())
	{
		// find it, check if it attach to a panel
		PanelInfo& paneinfo = it->second;
		CXTPDockingPane* pane = FindPane(paneinfo.m_paneID);
		if (pane)
		{
			return !(pane->IsClosed());
		}	
	}

	return false;
}
//////////////////////////////////////////////////////////////////////////
void gkPaneManager::SwitchPanel( const TCHAR* name )
{
	if(!(IsPanelOpen(name)))
	{
		OpenPanel(name);
	}
	else
	{
		ClosePanel(name);
	}
}

void CXTPDockingPaneVisualStudio2010CustomTheme::RefreshMetrics()
{
	CXTPDockingPaneVisualStudio2005Theme::RefreshMetrics();


	//xtpPaneThemeVisualStudio2010
	COLORREF clr = XTPColorManager()->GetColor(COLOR_HIGHLIGHT);
	COLORREF backClr = XTPColorManager()->GetColor(COLOR_MENU);
	COLORREF foreClr = XTPColorManager()->GetColor(COLOR_BTNFACE);
	COLORREF textClr = XTPColorManager()->GetColor(COLOR_BTNTEXT);

	m_clrNormalCaption.SetStandardValue(foreClr, foreClr);
	m_clrActiveCaption.SetStandardValue(clr, clr);

	m_clrCaptionBorder = clr;

// 	m_clrSplitter.SetStandardValue(RGB(41, 57, 85));
// 	m_clrSplitterGradient.SetStandardValue(RGB(41, 57, 85));

	m_clrSplitter.SetStandardValue(backClr);
	m_clrSplitterGradient.SetStandardValue(backClr);


	m_clrNormalCaptionText.SetStandardValue(textClr);
	m_clrActiveCaptionText.SetStandardValue(RGB(255, 255, 255));

	m_arrColor[XPCOLOR_3DFACE] = RGB(41, 57, 85);

	m_arrColor[XPCOLOR_HIGHLIGHT] = RGB(255, 255, 247);
	m_arrColor[XPCOLOR_HIGHLIGHT_BORDER] = RGB(231, 195, 99);
	m_arrColor[XPCOLOR_HIGHLIGHT_PUSHED] = RGB(255, 235, 165);
	m_arrColor[XPCOLOR_HIGHLIGHT_PUSHED_BORDER] = RGB(231, 195, 99);
	m_arrColor[XPCOLOR_PUSHED_TEXT] = RGB(0, 0, 0);
	m_arrColor[XPCOLOR_HIGHLIGHT_TEXT] = RGB(0, 0, 0);
}

COLORREF CXTPDockingPaneVisualStudio2010CustomTheme::FillCaptionPart( CDC* pDC, CXTPDockingPaneBase* pPane, CRect rcCaption, BOOL bActive, BOOL bVertical )
{
	CXTPPaintManagerColorGradient& clr = bActive ? m_clrActiveCaption : m_clrNormalCaption;
	XTPDrawHelpers()->GradientFill(pDC, rcCaption, clr, bVertical);

// 	if (!bVertical)
// 	{
// 		pDC->SetPixel(rcCaption.left, rcCaption.top, CXTPDrawHelpers::BlendColors(clr, m_clrCaptionBorder, (double)60 / 255));
// 		pDC->SetPixel(rcCaption.left + 1, rcCaption.top, CXTPDrawHelpers::BlendColors(clr, m_clrCaptionBorder, (double)120 / 255));
// 		pDC->SetPixel(rcCaption.left, rcCaption.top + 1, CXTPDrawHelpers::BlendColors(clr, m_clrCaptionBorder, (double)120 / 255));
// 
// 		pDC->SetPixel(rcCaption.right - 1, rcCaption.top, CXTPDrawHelpers::BlendColors(clr, m_clrCaptionBorder, (double)60 / 255));
// 		pDC->SetPixel(rcCaption.right - 2, rcCaption.top, CXTPDrawHelpers::BlendColors(clr, m_clrCaptionBorder, (double)120 / 255));
// 		pDC->SetPixel(rcCaption.right - 1, rcCaption.top + 1, CXTPDrawHelpers::BlendColors(clr, m_clrCaptionBorder, (double)120 / 255));
// 	}

	return bActive ? RGB(115, 97, 57) : RGB(206, 215, 222);
}

void CXTPDockingPaneVisualStudio2010CustomTheme::DrawFloatingFrame( CDC* pDC, CXTPDockingPaneMiniWnd* pPane, CRect rc )
{
	BOOL bActive = m_bHighlightActiveCaption && pPane->IsActive();

	COLORREF clr = bActive ? m_clrCaptionBorder : m_clrNormalCaption;

	pDC->Draw3dRect(rc, clr, clr);
	rc.DeflateRect(1, 1);
	pDC->Draw3dRect(rc, clr, clr);
	rc.DeflateRect(1, 1);
	pDC->Draw3dRect(rc, clr, clr);

	int nTitleHeight = m_nTitleHeight;
	CRect rcCaption(rc);
	rcCaption.DeflateRect(0, 0, 0, rc.Height() - nTitleHeight - 3);

	CXTPBufferDC dcCache(*pDC, rcCaption);
	dcCache.SetBkMode(TRANSPARENT);

	dcCache.FillSolidRect(rcCaption, clr);

	CString strTitle;
	pPane->GetWindowText(strTitle);

	rcCaption.DeflateRect(1, 2, 1, 0);

	DrawCaptionPart(&dcCache, pPane, rcCaption, strTitle, bActive, FALSE);
}
