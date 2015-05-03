// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "RibbonSample.h"

#include "MainFrm.h"
#include "Galleryitems.h"
#include "WorkspaceView.h"

#include "gkStudioWndMsg.h"
#include "IResourceManager.h"
#include "Panels\PaneHierarchy.h"
#include "Managers\gkCameraPosManager.h"
#include "Managers\gkObjectManager.h"
#include "I3DEngine.h"
#include "ITerrianSystem.h"
#include "gkTrackBusEditor.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CXTPFrameWnd)


BEGIN_MESSAGE_MAP(CMainFrame, CXTPFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_COMMAND(ID_OPTIONS_ANIMATION, OnOptionsAnimation)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_ANIMATION, OnUpdateOptionsAnimation)
	ON_COMMAND(ID_OPTIONS_RIGHTTOLEFT, OnOptionsRighttoleft)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_RIGHTTOLEFT, OnUpdateOptionsRighttoleft)
	
	ON_COMMAND_RANGE(ID_OPTIONS_GKWHITE, ID_OPTIONS_GKBLACK, OnOptionsStyle)
	ON_UPDATE_COMMAND_UI_RANGE(ID_OPTIONS_GKWHITE, ID_OPTIONS_GKBLACK, OnUpdateOptionsStyle)

	ON_UPDATE_COMMAND_UI_RANGE(ID_OPTIONS_FONT_SYSTEM, ID_OPTIONS_FONT_EXTRALARGE, OnUpdateOptionsFont)
	ON_COMMAND_RANGE(ID_OPTIONS_FONT_SYSTEM, ID_OPTIONS_FONT_EXTRALARGE, OnOptionsFont)

	ON_COMMAND(ID_OPTIONS_FONT_AUTORESIZEICONS, OnAutoResizeIcons)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_FONT_AUTORESIZEICONS, OnUpdateAutoResizeIcons)
	
	ON_COMMAND(ID_OPTIONS_FRAMETHEME, OnFrameTheme)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_FRAMETHEME, OnUpdateFrameTheme)
	
	ON_COMMAND(ID_TOGGLE_GROUPS, OnToggleGroups)
	ON_COMMAND(XTP_ID_RIBBONCUSTOMIZE_MINIMIZE, OnToggleGroups)
	//}}AFX_MSG_MAP

	// make gameknife movement call [8/28/2011 Kaiming-Desktop]
	ON_COMMAND(ID_THEMES_THEMES, OnSwitchMovementModeSelect)
	ON_UPDATE_COMMAND_UI(ID_THEMES_THEMES, OnUpdateSwitchMovementMode)
	ON_COMMAND(ID_PAGE_MOVEMODE_MOVE, OnSwitchMovementModeMove)
	ON_UPDATE_COMMAND_UI(ID_PAGE_MOVEMODE_MOVE, OnUpdateSwitchMovementMode)
	ON_COMMAND(ID_PAGE_MOVEMODE_ORIENTATION, OnSwitchMovementModeRotate)
	ON_UPDATE_COMMAND_UI(ID_PAGE_MOVEMODE_ORIENTATION, OnUpdateSwitchMovementMode)
	ON_COMMAND(ID_PAGE_MOVEMODE_SCALE, OnSwitchMovementModeScale)
	ON_UPDATE_COMMAND_UI(ID_PAGE_MOVEMODE_SCALE, OnUpdateSwitchMovementMode)

	ON_COMMAND_RANGE(ID_PANES_SWISSKNIFEBAR, ID_PANES_CONSOLE+1, OnSwitchPanes)

	ON_COMMAND_RANGE(ID_THEMES_FONTS, ID_ARRANGE_TEXTWRAPPING, OnEmptyCommandRange)
	ON_COMMAND(ID_FILE_SEND_MAIL, OnEmptyCommand)
	ON_COMMAND(ID_FILE_SEND_INTERNETFAX, OnEmptyCommand)
	ON_COMMAND(ID_STYLES_CHANGE, OnEmptyCommand)

	ON_COMMAND(ID_SWITCH_FULLSCREEN, OnFullScreen)

	ON_WM_KEYDOWN()

	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_RULER, ID_VIEW_THUMBNAILS, OnUpdateViewShowHide)
	ON_COMMAND_RANGE(ID_VIEW_RULER, ID_VIEW_THUMBNAILS, OnViewShowHide)

	ON_COMMAND_EX(ID_VIEW_MESSAGE_BAR, CFrameWnd::OnBarCheck)
	ON_UPDATE_COMMAND_UI(ID_VIEW_MESSAGE_BAR, CFrameWnd::OnUpdateControlBarMenu)

	ON_NOTIFY(TCN_SELCHANGE, XTP_ID_RIBBONCONTROLTAB, OnRibbonTabChanged)
	ON_NOTIFY(TCN_SELCHANGING, XTP_ID_RIBBONCONTROLTAB, OnRibbonTabChanging)

	ON_COMMAND(XTP_ID_CUSTOMIZE, OnCustomize)
	ON_COMMAND(ID_APP_OPTIONS, OnCustomize)
	ON_COMMAND(XTP_ID_RIBBONCUSTOMIZE, OnCustomizeQuickAccess)

	ON_XTP_EXECUTE(ID_GALLERY_STYLES, OnGalleryStyles)
	ON_UPDATE_COMMAND_UI(ID_GALLERY_STYLES, OnUpdateGalleryStyles)

	ON_XTP_EXECUTE(ID_GALLERY_COLUMNS, OnGalleryColumns)
	ON_UPDATE_COMMAND_UI(ID_GALLERY_COLUMNS, OnUpdateGalleryColumns)


	ON_XTP_EXECUTE(ID_GALLERY_SHAPES, OnGalleryShapes)
	ON_UPDATE_COMMAND_UI(ID_GALLERY_SHAPES, OnUpdateGalleryShapes)

	ON_XTP_EXECUTE(ID_GALLERY_STYLESET, OnGalleryStyleSet)
	ON_UPDATE_COMMAND_UI(ID_GALLERY_STYLESET, OnUpdateGalleryStyleSet)	

	ON_XTP_EXECUTE(ID_GALLERY_COLORS, OnGalleryThemeColors)
	ON_UPDATE_COMMAND_UI(ID_GALLERY_COLORS, OnUpdateGalleryThemeColors)	

	ON_XTP_EXECUTE(ID_GALLERY_FONTS, OnGalleryThemeFonts)
	ON_UPDATE_COMMAND_UI(ID_GALLERY_FONTS, OnUpdateGalleryThemeFonts)	

	ON_XTP_CREATECONTROL()

	ON_COMMAND_RANGE(ID_SWITCH_PRINTLAYOUT, ID_SWITCH_DRAFT, OnStatusBarSwitchView)
	
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_CAPSLOCK, OnUpdateKeyIndicator)

	ON_WM_COPYDATA()

	ON_COMMAND_RANGE(ID_SWITCH_CAM1, ID_SWITCH_CAM9, OnSwitchCam)
	ON_COMMAND_RANGE(ID_SAVE_CAM1, ID_SAVE_CAM9, OnSaveCam)
	//ON_COMMAND(ID_SWITCH_CAM9, &CMainFrame::OnSwitchCam9)
	ON_COMMAND(ID_GLOBAL_FOCUSOBJ, &CMainFrame::OnGlobalFocusobj)

END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

//#define _SAVE_TO_XML


CMainFrame::CMainFrame()
{
	TCHAR szStylesPath[_MAX_PATH];

	VERIFY(::GetModuleFileName(
		AfxGetApp()->m_hInstance, szStylesPath, _MAX_PATH));		
	 
	CString csStylesPath(szStylesPath);
	int nIndex  = csStylesPath.ReverseFind(_T('\\'));
	if (nIndex > 0) {
		csStylesPath = csStylesPath.Left(nIndex);
	}
	else {
		csStylesPath.Empty();
	}
	m_csStylesPath += csStylesPath + _T("\\Styles\\");

	m_bChecked = FALSE;
	m_bAnimation = TRUE;
	m_bLayoutRTL = FALSE;

	ZeroMemory(&m_bOptions, sizeof(m_bOptions));


	// get path of executable	
	LPTSTR lpszExt = _tcsrchr(szStylesPath, '.');
	lstrcpy(lpszExt, _T(".xml"));
	
	m_strIniFileName = szStylesPath;

	m_nColumns = 0;

	m_pItemsShapes = 0;
	m_pItemsStyles = 0;
	m_pItemsStyleSet = 0;
	m_pItemsThemeColors = 0;
	m_pItemsThemeFonts = 0;
	m_pItemsFontTextColor = 0;
	m_pItemsFontBackColor = 0;
	m_pItemsFontFace = 0;
	m_pItemsFontSize = 0;
	m_pItemsColumns = 0;

	m_nRibbonStyle = ID_OPTIONS_STYLEWHITE;

	m_pGroupMovement = NULL;
	m_uChecked = 0;
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTPFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!CreateStatusBar())
		return -1;

	if (!InitCommandBars())
		return -1;

	CXTPCommandBars* pCommandBars = GetCommandBars();
	m_wndStatusBar.SetCommandBars(pCommandBars);

	// To Set theme for individual CommandBars
	pCommandBars->SetTheme(xtpThemeRibbon);

	
	//////////////////////////////////////////////////////////////////////////	
	// IMPORTANT:
	// Ribbon bitmaps resources by default located in application executable. 
	//
	// You also can specify handle to find ribbon resources:
	//((CXTPOffice2007Theme*)XTPPaintManager())->SetImageHandle(hHandle);

	//HMODULE hModule = LoadLibrary(m_csStylesPath + _T("Office2007Blue.dll"));
	//((CXTPOffice2007Theme*)pCommandBars->GetPaintManager())->SetImageHandle(hModule);

	CXTPToolTipContext* pToolTipContext = GetCommandBars()->GetToolTipContext();
	pToolTipContext->SetStyle(xtpToolTipOffice2007);
	pToolTipContext->ShowTitleAndDescription();
	pToolTipContext->ShowImage(TRUE, 0);
	pToolTipContext->SetMargin(CRect(2, 2, 2, 2));
	pToolTipContext->SetMaxTipWidth(180);
	pToolTipContext->SetFont(pCommandBars->GetPaintManager()->GetIconFont());
	pToolTipContext->SetDelayTime(TTDT_INITIAL, 900);


	LoadIcons();

	pCommandBars->GetPaintManager()->m_bAutoResizeIcons = TRUE;

	pCommandBars->GetCommandBarsOptions()->bToolBarAccelTips = TRUE;

	//pCommandBars->GetShortcutManager()->SetAccelerators(IDR_MAINFRAME);

	//CreateGalleries();

	if (!CreateRibbonBar())
	{
		TRACE0("Failed to create ribbon\n");
		return -1;
	}

	OnOptionsStyle(ID_OPTIONS_GKBLACK);

	//CreateMiniToolBar();

// 	if (!CreateMessageBar())
// 		return -1;


#ifndef _SAVE_TO_XML
	LoadCommandBars(_T("CommandBars3"));
#else
	CXTPPropExchangeXMLNode px(TRUE, 0, _T("Settings"));

	if (px.LoadFromFile(m_strIniFileName))
	{
		CXTPPropExchangeSection pxCommandBars(px.GetSection(_T("CommandBars")));
		XTP_COMMANDBARS_PROPEXCHANGE_PARAM param; 
		param.bSerializeControls = TRUE;
		param.bSerializeOptions = TRUE;
		GetCommandBars()->DoPropExchange(&pxCommandBars, &param);
	}
#endif

	pCommandBars->GetCommandBarsOptions()->bShowKeyboardTips = TRUE;

	CreateDockingPane();


	//SetThemeAppProperties();


	return 0;
}

void CMainFrame::CreateDockingPane()
{
	

	CXTPDockingPaneMiniWnd::m_bShowPinButton = TRUE;

	m_paneManager.RegisterPanelDialog( new gkMaterialEditor(this) );
	m_paneManager.RegisterPanelDialog( new CPaneConsole(this) );
	m_paneManager.RegisterPanelDialog( new gkPaneTimeOfDay(this) );
	m_paneManager.RegisterPanelDialog( new CPaneObject(this) );
	m_paneManager.RegisterPanelDialog( new CPaneConsoleVar(this) );
	m_paneManager.RegisterPanelDialog( new CPaneHierarchy(this) );
	m_paneManager.RegisterPanelDialog( new gkTrackBusEditor(this) );
	
	m_barSwissKnife.Create( this, this, 37588 );
	m_paneManager.RegisterPanelManmual( m_barSwissKnife.getBar(), _T("SwissKnifeBar"));

	m_paneManager.Init(this);
	m_paneManager.OpenPanel( _T("Console Panel") );
	m_paneManager.OpenPanel( _T("SwissKnifeBar") );
	//m_paneManager.OpenPanel( _T("Hierarchy") );

}


BOOL CMainFrame::CreateMessageBar()
{
	if (!m_wndMessageBar.Create(GetCommandBars()))
		return FALSE;

	m_wndMessageBar.AddButton(SC_CLOSE, NULL, _T("Close Message Bar"));
	m_wndMessageBar.AddButton(ID_PAGE_TIMEOFDAY, _T("Open TOD"), _T("Show more options"));

	XTPImageManager()->SetIcon(IDC_MESSAGEBAR_WARNING, IDC_MESSAGEBAR_WARNING);

// 	m_wndMessageBar.EnableMarkup();
// 	m_wndMessageBar.SetMessage(_T("You can open tod."));

 	m_wndMessageBar.EnableMarkup();
 	m_wndMessageBar.SetMessage(
 		_T("<StackPanel Orientation='Horizontal'>")
 		_T("        <Image Source='22988'/>")
 		_T("		<TextBlock Padding='3, 0, 0, 0' VerticalAlignment='Center'><Bold>Security Warning</Bold></TextBlock>")
 		_T("		<TextBlock Padding='10, 0, 0, 0' VerticalAlignment='Center'>Certain content has been disabled.</TextBlock>")
 		_T("</StackPanel>"));

	return TRUE;
}

BOOL CMainFrame::CreateStatusBar()
{
	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("Failed to create status bar\n");
		return FALSE;      // fail to create
	}

	CXTPStatusBarPane* pPane;
	
	pPane = m_wndStatusBar.AddIndicator(ID_INDICATOR_PAGENUMBER);
	pPane->SetButton();
	pPane->SetPadding(8, 0, 8, 0);
	pPane->SetBeginGroup(TRUE);
	pPane->SetCaption(_T("&Select"));
	pPane->SetValue(_T("None"));
	pPane->SetText(_T("None"));
	pPane->SetTooltip(_T("CurrSelectObj"));

// 	UINT nID = ID_INDICATOR_MACRORECORDING;
// 	m_wndStatusBar.GetImageManager()->SetIcons(ID_INDICATOR_MACRORECORDING, &nID, 1, 0);


	CXTPStatusBarProgressPane* pProgressPane = (CXTPStatusBarProgressPane*)m_wndStatusBar.AddIndicator(new CXTPStatusBarProgressPane(), ID_INDICATOR_PROGRESS);
	pProgressPane->SetBeginGroup(FALSE);
	pProgressPane->SetCaption(_T("&ProgressBar"));
	pProgressPane->SetWidth(100);
	pProgressPane->SetPos(0);
	pProgressPane->SetPadding(3, 0, 3, 0);
	pProgressPane->SetVisible(FALSE);


	pPane = m_wndStatusBar.AddIndicator(0);
	pPane->SetWidth(1);
	pPane->SetStyle(SBPS_STRETCH | SBPS_NOBORDERS);
	pPane->SetBeginGroup(TRUE);

 	pPane = m_wndStatusBar.AddIndicator(ID_INDICATOR_CAPSLOCK);
 	pPane->SetPadding(8, 0, 8, 0);
 	//pPane->SetBeginGroup(TRUE);
 	//pPane->ShowBorders(FALSE);
 	pPane->SetCaption(_T("&Caps Lock"));
 	pPane->SetValue(_T("Off"));
 	pPane->SetText(_T("CAP"));
 
 	pPane = m_wndStatusBar.AddIndicator(ID_INDICATOR_NUM);
 	pPane->SetPadding(8, 0, 8, 0);
 	//pPane->SetBeginGroup(TRUE);
 	pPane->SetCaption(_T("&Num Lock"));
 	pPane->SetValue(_T("Off"));
 	pPane->SetText(_T("NUM"));
 
 	pPane = m_wndStatusBar.AddIndicator(ID_INDICATOR_SCRL);
 	pPane->SetPadding(8, 0, 8, 0);
 	//pPane->SetBeginGroup(TRUE);
 	pPane->SetCaption(_T("&Scrl Lock"));
 	pPane->SetValue(_T("Off"));
 	pPane->SetText(_T("SCRL"));
	
	m_wndStatusBar.SetDrawDisabledText(TRUE);
//	m_wndStatusBar.SetRibbonDividerIndex(pSwitchPane->GetIndex() - 1);
	m_wndStatusBar.EnableCustomization();

	CXTPToolTipContext* pToolTipContext = m_wndStatusBar.GetToolTipContext();
	pToolTipContext->SetStyle(xtpToolTipOffice2007);
	pToolTipContext->SetFont(m_wndStatusBar.GetPaintManager()->GetIconFont());

	return TRUE;
}

void CMainFrame::LoadIcons()
{
	CXTPCommandBars* pCommandBars = GetCommandBars();

	pCommandBars->GetImageManager()->SetIcons(IDR_SMALLICONS);
	pCommandBars->GetImageManager()->SetIcons(IDR_LARGEICONS);
	pCommandBars->GetImageManager()->SetIcons(IDR_LAYOUTTABLARGE);
	pCommandBars->GetImageManager()->SetIcons(IDR_LAYOUTTABSMALL);

	UINT uiGroups[] = {ID_GROUP_CLIPBOARD, ID_GROUP_FONT, ID_GROUP_PARAGRAPH, ID_GROUP_FIND}; 
	pCommandBars->GetImageManager()->SetIcons(IDB_BITMAPS_GROUPS, uiGroups, _countof(uiGroups), CSize(16, 16));

	UINT uiSystemMenu[] = {ID_FILE_NEW, ID_FILE_OPEN, ID_FILE_SAVE, ID_FILE_SAVE_AS, ID_FILE_PRINT, ID_FILE_PRINT_SETUP, 
	ID_FILE_PRINT_PREVIEW, ID_FILE_PREPARE, ID_FILE_SEND, ID_APP_CLOSE, ID_FILE_SEND_MAIL, ID_FILE_SEND_INTERNETFAX}; 
	pCommandBars->GetImageManager()->SetIcons(IDB_SYSTEMMENULARGE, uiSystemMenu, _countof(uiSystemMenu), CSize(32, 32));

	UINT uiGroupParagraph[] = {ID_INSERT_BULLET, ID_INSERT_NUMBERING, ID_INSERT_LIST, 
		ID_PARA_DECREASEINDENT, ID_PARA_INCREASEINDENT, ID_PARA_SORT, ID_PARA_SHOWMARKS, ID_PARA_LEFT, 
		ID_PARA_CENTER, ID_PARA_RIGHT, ID_PARA_JUSTIFY, ID_PARA_LINESPACING, ID_PARA_SHADING, ID_BORDERS_NOBORDER};
	pCommandBars->GetImageManager()->SetIcons(ID_GROUP_PARAGRAPH, uiGroupParagraph, _countof(uiGroupParagraph), CSize(16, 16));

	UINT uiGroupClipboard[] = {ID_EDIT_PASTE, ID_EDIT_CUT, ID_EDIT_COPY, ID_FORMAT_PAINTER};
	pCommandBars->GetImageManager()->SetIcons(ID_GROUP_CLIPBOARD, uiGroupClipboard, _countof(uiGroupClipboard), CSize(16, 16));

	UINT uiGroupFont[] = {ID_FONT_GROW, ID_FONT_SHRINK, ID_FONT_CLEAR, ID_CHAR_BOLD, ID_CHAR_ITALIC, ID_CHAR_UNDERLINE,
		ID_CHAR_STRIKETHROUGH, ID_TEXT_SUBSCRIPT,ID_TEXT_SUPERSCRIPT, ID_TEXT_CHANGECASE, ID_TEXT_COLOR, ID_TEXT_HIGHLIGHTCOLOR};
	pCommandBars->GetImageManager()->SetIcons(ID_GROUP_FONT, uiGroupFont, _countof(uiGroupFont), CSize(16, 16));

	UINT uiGroupStyles[] = {ID_GALLERY_STYLES, ID_STYLES_CHANGE};
	pCommandBars->GetImageManager()->SetIcons(ID_GROUP_STYLES, uiGroupStyles, _countof(uiGroupStyles), CSize(32, 32));

	UINT uiGroupFind[] = {ID_EDIT_FIND, ID_EDIT_REPLACE, ID_EDIT_GOTO, ID_EDIT_SELECT};
	pCommandBars->GetImageManager()->SetIcons(ID_GROUP_FIND, uiGroupFind, _countof(uiGroupFind), CSize(16, 16));


	UINT uiInsertTabIcons[] = {ID_PAGES_NEW, ID_PAGES_COVER, ID_PAGES_BREAK, ID_INSERT_HEADER, ID_INSERT_FOOTER, ID_INSERT_PAGENUMBER, 
		ID_TEXT_PARTS, ID_TEXT_WORDART, ID_TEXT_DROPCAP, 
		ID_ILLUSTRATION_GRAPHIC, ID_ILLUSTRATION_CLIPART, ID_ILLUSTRATION_CLIPART, ID_ILLUSTRATION_CHART, 
		ID_INSERT_HYPERLINK, ID_INSERT_BOOKMARK, ID_INSERT_CROSS_REFERENCE, ID_TEXT_SIGNATURE, ID_TEXT_DATETIME, ID_TEXT_INSERTOBJECT};
	pCommandBars->GetImageManager()->SetIcons(IDB_INSERTTAB, uiInsertTabIcons, _countof(uiInsertTabIcons), CSize(16, 16));
	
	pCommandBars->GetImageManager()->SetIcon(ID_GROUP_BUTTONPOPUP, ID_GROUP_BUTTONPOPUP);

	UINT uiTabReferences[] = {ID_CONTENTS_TABLE, ID_FOOTNOTES_INSERT, 1, ID_CAPTIONS_INSERT, ID_INDEX_MARK };
	pCommandBars->GetImageManager()->SetIcons(ID_TAB_REFERENCES, uiTabReferences, _countof(uiTabReferences), CSize(32, 32));

	UINT uiView[] = {ID_VIEW_PRINTLAYOUT, ID_VIEW_FULLSCREENREADING, ID_VIEW_WEBLAYOUT, ID_VIEW_OUTLINE, ID_VIEW_DRAFT,
		ID_ZOOM_ZOOM, ID_WINDOW_NEW, ID_WINDOW_ARRANGE, ID_WINDOW_SPLIT, ID_WINDOW_SWITCH};
	pCommandBars->GetImageManager()->SetIcons(ID_TAB_VIEW, uiView, _countof(uiView), CSize(32, 32));

	UINT uiPageParagraph[] = {ID_PARAGRAPH_INDENTLEFT, ID_PARAGRAPH_INDENTRIGHT, ID_PARAGRAPH_SPACINGBEFORE, ID_PARAGRAPH_SPACINGAFTER};
	pCommandBars->GetImageManager()->SetIcons(	ID_GROUP_PAGEPARAGRAPH, uiPageParagraph, _countof(uiPageParagraph), CSize(16, 16));

	UINT uiTabPageLayout[] = {ID_ARRANGE_POSITION, ID_ARRANGE_FRONT, ID_ARRANGE_BACK, ID_ARRANGE_TEXTWRAPPING,
		ID_ARRANGE_ALIGN, ID_ARRANGE_GROUP, ID_ARRANGE_ROTATE};
	pCommandBars->GetImageManager()->SetIcons(ID_TAB_PAGELAYOUT, uiTabPageLayout, _countof(uiTabPageLayout), CSize(32, 32));

	UINT uiGroupsTip[] = {ID_GROUP_CLIPBOARD_OPTION, ID_GROUP_FONT_OPTION, ID_GROUP_PARAGRAPH_OPTION, ID_GROUP_STYLES_OPTION, IDB_GEAR};
	pCommandBars->GetImageManager()->SetIcons(IDB_GROUPOPTIONTOOLTIP, uiGroupsTip, _countof(uiGroupsTip), CSize(100, 130));

 	UINT uiSubEditorIcon[] = {ID_PANES_TIMEOFDAYEDITOR, ID_PANES_MATERIALEDITOR, ID_PANES_HIERARCHYEDITOR, ID_PANES_TERRIANEDITOR, ID_PANES_TRACKBUSEDITOR, ID_PANES_CONSOLE, ID_PANES_SWISSKNIFEBAR, ID_SWITCH_FULLSCREEN};
 	pCommandBars->GetImageManager()->SetIcons(IDR_PANES, uiSubEditorIcon, _countof(uiSubEditorIcon), CSize(32, 32));
}


void CMainFrame::CreateGalleries()
{
	m_pItemsShapes = CXTPControlGalleryItems::CreateItems(GetCommandBars(), ID_GALLERY_SHAPES);
	
	m_pItemsShapes->GetImageManager()->SetIcons(ID_GALLERY_SHAPES, 0, 0, CSize(20, 20));

	CXTPControlGalleryItem* pItem = m_pItemsShapes->AddLabel(0);
	pItem->SetCaption(_T("Lines"));

	int nShape;
	for (nShape = 0; nShape < 12; nShape++)
		m_pItemsShapes->AddItem(nShape, nShape);

	pItem = m_pItemsShapes->AddLabel(0);
	pItem->SetCaption(_T("Basic Shapes"));

	for (; nShape < 12 + 32; nShape++)
		m_pItemsShapes->AddItem(nShape, nShape);

	pItem = m_pItemsShapes->AddLabel(0);
	pItem->SetCaption(_T("Block Arrows"));

	for (; nShape < 12 + 32 + 27; nShape++)
		m_pItemsShapes->AddItem(nShape, nShape);

	pItem = m_pItemsShapes->AddLabel(0);
	pItem->SetCaption(_T("Flowchart"));

	for (; nShape < 12 + 32 + 27 + 28; nShape++)
		m_pItemsShapes->AddItem(nShape, nShape);

	pItem = m_pItemsShapes->AddLabel(0);
	pItem->SetCaption(_T("Callouts"));

	for (; nShape < 12 + 32 + 27 + 28 + 20; nShape++)
		m_pItemsShapes->AddItem(nShape, nShape);

	pItem = m_pItemsShapes->AddLabel(0);
	pItem->SetCaption(_T("Stars and Banners"));

	for (; nShape < 12 + 32 + 27 + 28 + 20 + 16; nShape++)
		m_pItemsShapes->AddItem(nShape, nShape);

	m_nShape = -1;
	m_nStyle = 0;


	m_pItemsStyles = CXTPControlGalleryItems::CreateItems(GetCommandBars(), ID_GALLERY_STYLES);

	m_pItemsStyles->GetImageManager()->SetMaskColor(0xFF00FF);
	m_pItemsStyles->GetImageManager()->SetIcons(ID_GALLERY_STYLES, 0, 0, CSize(64, 48));
	m_pItemsStyles->GetImageManager()->SetMaskColor(COLORREF_NULL); // Default TopLeft Pixel.

	m_pItemsStyles->SetItemSize(CSize(72, 56));

	for (int nStyle = 0; nStyle < 16; nStyle++)
		m_pItemsStyles->AddItem(nStyle, nStyle);


	m_nStyleSet = 0;

	m_pItemsStyleSet = CXTPControlGalleryItems::CreateItems(GetCommandBars(), ID_GALLERY_STYLESET);
	m_pItemsStyleSet->SetItemSize(CSize(0, 22));
	m_pItemsStyleSet->AddItem(new CGalleryItemStyleSet(0, _T("Classic")));
	m_pItemsStyleSet->AddItem(new CGalleryItemStyleSet(1, _T("Default")));
	m_pItemsStyleSet->AddItem(new CGalleryItemStyleSet(2, _T("Distinctive")));
	m_pItemsStyleSet->AddItem(new CGalleryItemStyleSet(3, _T("Elegant")));
	m_pItemsStyleSet->AddItem(new CGalleryItemStyleSet(4, _T("Fancy")));
	m_pItemsStyleSet->AddItem(new CGalleryItemStyleSet(5, _T("Formal")));
	m_pItemsStyleSet->AddItem(new CGalleryItemStyleSet(6, _T("Manuscript")));
	m_pItemsStyleSet->AddItem(new CGalleryItemStyleSet(7, _T("Modern")));
	m_pItemsStyleSet->AddItem(new CGalleryItemStyleSet(8, _T("Simple")));
	m_pItemsStyleSet->AddItem(new CGalleryItemStyleSet(9, _T("Traditional")));

	m_nThemeColors = 0;
	m_pItemsThemeColors = CXTPControlGalleryItems::CreateItems(GetCommandBars(), ID_GALLERY_COLORS);
	m_pItemsThemeColors->SetItemSize(CSize(0, 20));

	pItem = m_pItemsThemeColors->AddLabel(0);
	pItem->SetCaption(_T("Built-In"));
	m_pItemsThemeColors->AddItem(new CGalleryItemThemeColors(0, _T("Office")));
	m_pItemsThemeColors->AddItem(new CGalleryItemThemeColors(1, _T("Grayscale")));
	m_pItemsThemeColors->AddItem(new CGalleryItemThemeColors(2, _T("Apex")));
	m_pItemsThemeColors->AddItem(new CGalleryItemThemeColors(3, _T("Aspect")));
	m_pItemsThemeColors->AddItem(new CGalleryItemThemeColors(4, _T("Civic")));
	m_pItemsThemeColors->AddItem(new CGalleryItemThemeColors(5, _T("Concourse")));
	m_pItemsThemeColors->AddItem(new CGalleryItemThemeColors(6, _T("Currency")));
	m_pItemsThemeColors->AddItem(new CGalleryItemThemeColors(7, _T("Deluxe")));

	m_nThemeFonts = 0;
	m_pItemsThemeFonts = CXTPControlGalleryItems::CreateItems(GetCommandBars(), ID_GALLERY_FONTS);
	m_pItemsThemeFonts->SetItemSize(CSize(0, 60));

	pItem = m_pItemsThemeFonts->AddLabel(0);
	pItem->SetCaption(_T("Built-In"));
	m_pItemsThemeFonts->AddItem(new CGalleryItemThemeFonts(0, _T("Office")));
	m_pItemsThemeFonts->AddItem(new CGalleryItemThemeFonts(1, _T("Apex")));
	m_pItemsThemeFonts->AddItem(new CGalleryItemThemeFonts(2, _T("Aspect")));
	m_pItemsThemeFonts->AddItem(new CGalleryItemThemeFonts(3, _T("Civic")));
	m_pItemsThemeFonts->AddItem(new CGalleryItemThemeFonts(4, _T("Concourse")));
	m_pItemsThemeFonts->AddItem(new CGalleryItemThemeFonts(5, _T("Currency")));
	m_pItemsThemeFonts->AddItem(new CGalleryItemThemeFonts(6, _T("Deluxe")));
	m_pItemsThemeFonts->AddItem(new CGalleryItemThemeFonts(7, _T("Equity")));

	

	m_pItemsFontTextColor = CXTPControlGalleryItems::CreateItems(GetCommandBars(), ID_GALLERY_FONTTEXTCOLOR);
	m_pItemsFontTextColor->SetItemSize(CSize(17, 17));

	pItem = m_pItemsFontTextColor->AddLabel(0);
	pItem->SetCaption(_T("Theme Colors"));
	CGalleryItemFontColor::AddThemeColors(m_pItemsFontTextColor, 0);

	pItem = m_pItemsFontTextColor->AddLabel(0);
	pItem->SetCaption(_T("Standard Colors"));
	CGalleryItemFontColor::AddStandardColors(m_pItemsFontTextColor);


	m_pItemsFontBackColor = CXTPControlGalleryItems::CreateItems(GetCommandBars(), ID_GALLERY_FONTBACKCOLOR);
	m_pItemsFontBackColor->SetItemSize(CSize(30, 30));

	CGalleryItemFontColor::AddStandardBackColors(m_pItemsFontBackColor);



	m_pItemsFontFace = CXTPControlGalleryItems::CreateItems(GetCommandBars(), ID_GALLERY_FONTFACE);
	m_pItemsFontFace->SetItemSize(CSize(0, 26));
	CGalleryItemFontFace::AddFontItems(m_pItemsFontFace);

	m_pItemsFontSize = CXTPControlGalleryItems::CreateItems(GetCommandBars(), ID_GALLERY_FONTSIZE);
	m_pItemsFontSize->SetItemSize(CSize(0, 17));
	m_pItemsFontSize->AddItem(_T("8"));
	m_pItemsFontSize->AddItem(_T("9"));
	m_pItemsFontSize->AddItem(_T("10"));
	m_pItemsFontSize->AddItem(_T("11"));
	m_pItemsFontSize->AddItem(_T("12"));
	m_pItemsFontSize->AddItem(_T("14"));
	m_pItemsFontSize->AddItem(_T("16"));
	m_pItemsFontSize->AddItem(_T("18"));
	m_pItemsFontSize->AddItem(_T("20"));
	m_pItemsFontSize->AddItem(_T("22"));
	m_pItemsFontSize->AddItem(_T("24"));
	m_pItemsFontSize->AddItem(_T("26"));
	m_pItemsFontSize->AddItem(_T("28"));
	m_pItemsFontSize->AddItem(_T("36"));
	m_pItemsFontSize->AddItem(_T("48"));
	m_pItemsFontSize->AddItem(_T("72"));

	m_pItemsUndo = CXTPControlGalleryItems::CreateItems(GetCommandBars(), ID_EDIT_UNDO);
	m_pItemsUndo->SetItemSize(CSize(0, 21));
	m_pItemsUndo->ClipItems(FALSE);

	m_pItemsColumns = CXTPControlGalleryItems::CreateItems(GetCommandBars(), ID_GALLERY_COLUMNS);
	m_pItemsColumns->GetImageManager()->SetIcons(ID_GALLERY_COLUMNS, 0, 0, CSize(32, 32));
	m_pItemsColumns->AddItem(_T("One"), 0, 0);
	m_pItemsColumns->AddItem(_T("Two"), 1, 1);
	m_pItemsColumns->AddItem(_T("Three"), 2, 2);
	m_pItemsColumns->AddItem(_T("Left"), 3, 3);
	m_pItemsColumns->AddItem(_T("Right"), 4, 4);
	m_pItemsColumns->SetItemSize(CSize(0, 44));

}

BOOL CMainFrame::CreateMiniToolBar()
{
	CXTPCommandBars* pCommandBars = GetCommandBars();

	CXTPMiniToolBar* pMiniToolBar = CXTPMiniToolBar::CreateMiniToolBar(pCommandBars);
	pMiniToolBar->SetBarID(IDR_MENU_MINITOOLBAR);
	pMiniToolBar->SetTitle(_T("MiniBar"));


	CXTPControlComboBox* pComboFont = new CXTPControlComboBox();
	pMiniToolBar->GetControls()->Add(pComboFont, ID_FONT_FACE);

	pComboFont->SetDropDownListStyle();
	pComboFont->EnableAutoComplete();
	pComboFont->SetWidth(64);
	
	CXTPPopupBar* pPopupBarGallery = CXTPControlComboBoxGalleryPopupBar::CreateComboBoxGalleryPopupBar(pCommandBars);
	
	pComboFont->SetCommandBar(pPopupBarGallery);
	
	CXTPControlGallery* pControlGallery = new CXTPControlGallery();
	pControlGallery->SetControlSize(CSize(290, 508));
	pControlGallery->SetResizable(FALSE, TRUE);
	pControlGallery->ShowLabels(TRUE);
	pControlGallery->SetItems(m_pItemsFontFace);
	
	pPopupBarGallery->GetControls()->Add(pControlGallery, ID_GALLERY_FONTFACE);
	
	pPopupBarGallery->InternalRelease();



	pComboFont = new CXTPControlComboBox();
	pMiniToolBar->GetControls()->Add(pComboFont, ID_FONT_SIZE);

	pComboFont->SetDropDownListStyle();
	pComboFont->SetWidth(45);
	
	pPopupBarGallery = CXTPControlComboBoxGalleryPopupBar::CreateComboBoxGalleryPopupBar(pCommandBars);
	
	pComboFont->SetCommandBar(pPopupBarGallery);
	
	pControlGallery = new CXTPControlGallery();
	pControlGallery->SetControlSize(CSize(42, 16 * 17));
	pControlGallery->SetResizable(FALSE, TRUE);
	pControlGallery->SetItems(m_pItemsFontSize);
	
	pPopupBarGallery->GetControls()->Add(pControlGallery, ID_GALLERY_FONTSIZE);
	
	pPopupBarGallery->InternalRelease();
	

	pMiniToolBar->GetControls()->Add(xtpControlButton, ID_FONT_GROW);
	pMiniToolBar->GetControls()->Add(xtpControlButton, ID_FONT_SHRINK);
	pMiniToolBar->GetControls()->Add(xtpControlButtonPopup, ID_STYLES_CHANGE);

	pMiniToolBar->GetControls()->Add(xtpControlButton, ID_CHAR_BOLD);
	pMiniToolBar->GetControls()->Add(xtpControlButton, ID_CHAR_ITALIC);

	pMiniToolBar->GetControls()->Add(xtpControlButton, ID_PARA_LEFT);
	pMiniToolBar->GetControls()->Add(xtpControlButton, ID_PARA_CENTER);
	pMiniToolBar->GetControls()->Add(xtpControlButton, ID_PARA_RIGHT);

	pMiniToolBar->GetControls()->Add(xtpControlButton, ID_PARA_INCREASEINDENT);
	pMiniToolBar->GetControls()->Add(xtpControlButton, ID_PARA_DECREASEINDENT);
	pMiniToolBar->GetControls()->Add(xtpControlButton, ID_INSERT_BULLET);


	
	pCommandBars->GetContextMenus()->Add(pMiniToolBar);
	pMiniToolBar->SetWidth(22 * 8 + 4);

	// Add Simple context menu
	CMenu mnu;
	mnu.LoadMenu(IDR_MENU_CONTEXT);

	pCommandBars->GetContextMenus()->Add(IDR_MENU_CONTEXT, _T("Context Menu"), mnu.GetSubMenu(0));

	return TRUE;
}

BOOL CMainFrame::CreateRibbonBar()
{
	CXTPCommandBars* pCommandBars = GetCommandBars();

	CMenu menu;
	menu.Attach(::GetMenu(m_hWnd));
	SetMenu(NULL);

	CXTPRibbonBar* pRibbonBar = (CXTPRibbonBar*)pCommandBars->Add(_T("The Ribbon"), xtpBarTop, RUNTIME_CLASS(CXTPRibbonBar));
	if (!pRibbonBar)
	{
		return FALSE;
	}
	pRibbonBar->EnableDocking(0);
	//pRibbonBar->GetTabPaintManager()->m_bSelectOnButtonDown = FALSE;


	// create the file menu button and set the icon [8/27/2011 Kaiming-Desktop]
	CXTPControlPopup* pControlFile = (CXTPControlPopup*)pRibbonBar->AddSystemButton(ID_MENU_FILE);
	pControlFile->SetCommandBar(menu.GetSubMenu(0));
	pControlFile->GetCommandBar()->SetIconSize(CSize(32, 32));

 	pControlFile->SetIconId(IDB_GEAR);
 	UINT uCommand = {IDB_GEAR};
 	pCommandBars->GetImageManager()->SetIcons(IDB_GEAR, &uCommand, 1, CSize(0, 0), xtpImageNormal);
	

	//CreateBackstage();


	//////////////////////////////////////////////////////////////////////////
	// TAB HOME

	CXTPRibbonTab* pTabHome = pRibbonBar->AddTab(ID_TAB_HOME);

	CXTPRibbonGroup* pGroupClipborad = pTabHome->AddGroup(ID_GROUP_CLIPBOARD);
	pGroupClipborad->ShowOptionButton();
	pGroupClipborad->SetIconId(ID_GROUP_BUTTONPOPUP);
	pGroupClipborad->GetControlGroupOption()->SetID(ID_GROUP_CLIPBOARD_OPTION);
	pGroupClipborad->SetControlsCentering();
	
	CXTPControlPopup* pControlPaste = (CXTPControlPopup*)pGroupClipborad->Add(xtpControlSplitButtonPopup, ID_EDIT_PASTE);
	pControlPaste->GetCommandBar()->GetControls()->Add(xtpControlButton, ID_EDIT_PASTE);
	pControlPaste->GetCommandBar()->GetControls()->Add(xtpControlButton, ID_EDIT_PASTE_SPECIAL);
	CXTPControl* pControlCut = pGroupClipborad->Add(xtpControlButton, ID_EDIT_CUT);
	pControlCut->SetKeyboardTip(_T("X"));

	pGroupClipborad->Add(xtpControlButton, ID_EDIT_COPY);	
	pGroupClipborad->Add(xtpControlButton, ID_FORMAT_PAINTER);
	pControlPaste->SetKeyboardTip(_T("V"));

	// movement control group [8/28/2011 Kaiming-Desktop]
	m_pGroupMovement = pTabHome->AddGroup(ID_GROUP_OBJSEL);
	m_pGroupMovement->Add(xtpControlButton, ID_THEMES_THEMES);
	m_pGroupMovement->Add(xtpControlButton, ID_PAGE_MOVEMODE_MOVE);
	m_pGroupMovement->Add(xtpControlButton, ID_PAGE_MOVEMODE_ORIENTATION);
	m_pGroupMovement->Add(xtpControlButton, ID_PAGE_MOVEMODE_SCALE);


	CXTPRibbonTab* pTabEditors = pRibbonBar->AddTab(ID_TAB_SUBEDITOR);
	CXTPRibbonGroup* pGroupEditor = pTabEditors->AddGroup(ID_GROUP_EDITORS);

	pGroupEditor->Add(xtpControlButton, ID_PANES_TIMEOFDAYEDITOR);
	pGroupEditor->Add(xtpControlButton, ID_PANES_MATERIALEDITOR);
	pGroupEditor->Add(xtpControlButton, ID_PANES_HIERARCHYEDITOR);
	pGroupEditor->Add(xtpControlButton, ID_PANES_TERRIANEDITOR);
	pGroupEditor->Add(xtpControlButton, ID_PANES_TRACKBUSEDITOR);


	CXTPRibbonGroup* pGroupBasicPane = pTabEditors->AddGroup(ID_GROUP_BASICPANE);
	pGroupBasicPane->Add(xtpControlButton, ID_PANES_CONSOLE);
	pGroupBasicPane->Add(xtpControlButton, ID_PANES_SWISSKNIFEBAR);
	pGroupBasicPane->Add(xtpControlButton, ID_SWITCH_FULLSCREEN);

#if 0
	CXTPRibbonGroup* pGroupFont = pTabHome->AddGroup(ID_GROUP_FONT);
	pGroupFont->SetControlsGrouping();
	pGroupFont->ShowOptionButton();
	pGroupFont->GetControlGroupOption()->SetID(ID_GROUP_FONT_OPTION);

	CXTPControlComboBox* pComboFont = new CXTPControlComboBox();
	pGroupFont->Add(pComboFont, ID_FONT_FACE);

	pComboFont->SetDropDownListStyle();
	pComboFont->EnableAutoComplete();
	pComboFont->SetWidth(130);
	
	CXTPPopupBar* pPopupBarGallery = CXTPControlComboBoxGalleryPopupBar::CreateComboBoxGalleryPopupBar(pCommandBars);
	
	pComboFont->SetCommandBar(pPopupBarGallery);
	
	CXTPControlGallery* pControlGallery = new CXTPControlGallery();
	pControlGallery->SetControlSize(CSize(290, 508));
	pControlGallery->SetResizable(FALSE, TRUE);
	pControlGallery->ShowLabels(TRUE);
	pControlGallery->SetItems(m_pItemsFontFace);
	
	pPopupBarGallery->GetControls()->Add(pControlGallery, ID_GALLERY_FONTFACE);
	
	pPopupBarGallery->InternalRelease();



	pComboFont = new CXTPControlComboBox();
	pGroupFont->Add(pComboFont, ID_FONT_SIZE);

	pComboFont->SetDropDownListStyle();
	pComboFont->SetWidth(45);
	
	pPopupBarGallery = CXTPControlComboBoxGalleryPopupBar::CreateComboBoxGalleryPopupBar(pCommandBars);
	
	pComboFont->SetCommandBar(pPopupBarGallery);
	
	pControlGallery = new CXTPControlGallery();
	pControlGallery->SetControlSize(CSize(42, 16 * 17));
	pControlGallery->SetResizable(FALSE, TRUE);
	pControlGallery->SetItems(m_pItemsFontSize);
	
	pPopupBarGallery->GetControls()->Add(pControlGallery, ID_GALLERY_FONTSIZE);
	
	pPopupBarGallery->InternalRelease();
	

	CXTPControl* pControl = pGroupFont->Add(xtpControlButton, ID_FONT_GROW);
	pControl->SetBeginGroup(TRUE);
	pGroupFont->Add(xtpControlButton, ID_FONT_SHRINK);

	pControl = pGroupFont->Add(xtpControlButton, ID_FONT_CLEAR);
	pControl->SetBeginGroup(TRUE);


	pGroupFont->Add(xtpControlButton, ID_CHAR_BOLD)->SetBeginGroup(TRUE);
	pGroupFont->Add(xtpControlButton, ID_CHAR_ITALIC);
	pGroupFont->Add(xtpControlSplitButtonPopup, ID_CHAR_UNDERLINE);	

	pGroupFont->Add(xtpControlButton, ID_CHAR_STRIKETHROUGH);
	pGroupFont->Add(xtpControlButton, ID_TEXT_SUBSCRIPT)->SetBeginGroup(TRUE);
	pGroupFont->Add(xtpControlButton, ID_TEXT_SUPERSCRIPT);	
	pControl = pGroupFont->Add(xtpControlPopup, ID_TEXT_CHANGECASE);
	pControl->SetStyle(xtpButtonIcon);


	CXTPControlPopupColor* pPopupColor = (CXTPControlPopupColor*)pGroupFont->Add(new CXTPControlPopupColor(), ID_TEXT_HIGHLIGHTCOLOR);
	pPopupColor->SetBeginGroup(TRUE);
	CXTPPopupBar* pColorBar = CXTPPopupBar::CreatePopupBar(GetCommandBars());
	CMenu menuColor;
	menuColor.LoadMenu(ID_TEXT_HIGHLIGHTCOLOR);
	pColorBar->LoadMenu(menuColor.GetSubMenu(0));
	pPopupColor->SetCommandBar(pColorBar);
	pColorBar->InternalRelease();


	pPopupColor = (CXTPControlPopupColor*)pGroupFont->Add(new CXTPControlPopupColor, ID_TEXT_COLOR);
	
	pColorBar = CXTPPopupBar::CreatePopupBar(GetCommandBars());
	
	pColorBar->GetControls()->Add(new CXTPControlButton(), XTP_IDS_AUTOMATIC);

	pControlGallery = new CXTPControlGallery();
	pControlGallery->SetControlSize(CSize(171, 145));
	pControlGallery->ShowLabels(TRUE);
	pControlGallery->ShowScrollBar(FALSE);
	pControlGallery->SetItems(m_pItemsFontTextColor);
	pColorBar->GetControls()->Add(pControlGallery, ID_GALLERY_FONTTEXTCOLOR);

	pColorBar->GetControls()->Add(new CXTPControlButton(), XTP_IDS_MORE_COLORS);
	
	pPopupColor->SetCommandBar(pColorBar);
	pColorBar->EnableCustomization(FALSE);
	pColorBar->InternalRelease();


	CXTPRibbonGroup* pGroupParagraph = pTabHome->AddGroup(ID_GROUP_PARAGRAPH);
	pGroupParagraph->SetControlsGrouping();
	pGroupParagraph->ShowOptionButton();
	pGroupParagraph->GetControlGroupOption()->SetID(ID_GROUP_PARAGRAPH_OPTION);
	
	pGroupParagraph->Add(xtpControlSplitButtonPopup, ID_INSERT_BULLET);
	pGroupParagraph->Add(xtpControlSplitButtonPopup, ID_INSERT_NUMBERING);
	pGroupParagraph->Add(xtpControlSplitButtonPopup, ID_INSERT_LIST);
	pGroupParagraph->Add(xtpControlButton, ID_PARA_DECREASEINDENT)->SetBeginGroup(TRUE);
	pGroupParagraph->Add(xtpControlButton, ID_PARA_INCREASEINDENT);

	pGroupParagraph->Add(xtpControlButton, ID_PARA_SORT)->SetBeginGroup(TRUE);	
	pGroupParagraph->Add(xtpControlButton, ID_PARA_SHOWMARKS)->SetBeginGroup(TRUE);
	
	pGroupParagraph->Add(xtpControlButton, ID_PARA_LEFT)->SetBeginGroup(TRUE);
	pGroupParagraph->Add(xtpControlButton, ID_PARA_CENTER);
	pGroupParagraph->Add(xtpControlButton, ID_PARA_RIGHT);
	pGroupParagraph->Add(xtpControlButton, ID_PARA_JUSTIFY);

	pGroupParagraph->Add(xtpControlButtonPopup, ID_PARA_LINESPACING)->SetBeginGroup(TRUE);
	pGroupParagraph->Add(xtpControlSplitButtonPopup, ID_PARA_SHADING)->SetBeginGroup(TRUE);
	pGroupParagraph->Add(xtpControlSplitButtonPopup, ID_BORDERS_NOBORDER)->SetBeginGroup(TRUE);

	CXTPRibbonGroup* pGroupStyles = pTabHome->AddGroup(ID_GROUP_STYLES);
	pGroupStyles->SetControlsCentering(TRUE);
	
	pControlGallery = (CXTPControlGallery*)pGroupStyles->Add(new CXTPControlGallery(), ID_GALLERY_STYLES);	

	pControlGallery->SetControlSize(CSize(234, 60));
	pControlGallery->SetResizable();
	pControlGallery->SetItemsMargin(0, 1, 0, 1);
	pControlGallery->ShowLabels(FALSE);
	pControlGallery->ShowBorders(TRUE);
	pControlGallery->SetItems(m_pItemsStyles);
	
	CMenu menuQuickStyles;
	menuQuickStyles.LoadMenu(ID_STYLES_QUICKSTYLES);
	
	CXTPPopupBar* pPopupBar = CXTPPopupBar::CreatePopupBar(GetCommandBars());
	pPopupBar->LoadMenu(menuQuickStyles.GetSubMenu(0));
	
	pControlGallery->SetCommandBar(pPopupBar);
	pPopupBar->EnableAnimation();
	pPopupBar->InternalRelease();
	
	pGroupStyles->Add(xtpControlButtonPopup, ID_STYLES_CHANGE);

	CXTPRibbonGroup* pGroupFind = pTabHome->AddGroup(ID_GROUP_FIND);
	
	CXTPControlPopup* pControlFind = (CXTPControlPopup*)pGroupFind->Add(xtpControlSplitButtonPopup, ID_EDIT_FIND);
	pControlFind->GetCommandBar()->GetControls()->Add(xtpControlButton, ID_EDIT_FIND);
	pControlFind->GetCommandBar()->GetControls()->Add(xtpControlButton, ID_EDIT_GOTO);
	pGroupFind->Add(xtpControlButton, ID_EDIT_REPLACE);
	
	CXTPControlPopup* pControlSelect = (CXTPControlPopup*)pGroupFind->Add(xtpControlPopup, ID_EDIT_SELECT);
	pGroupFind->SetIconId(ID_GROUP_BUTTONPOPUP);
	pControlSelect->GetCommandBar()->GetControls()->Add(xtpControlButton, ID_EDIT_SELECT_ALL);
	pControlSelect->GetCommandBar()->GetControls()->Add(xtpControlButton, ID_EDIT_SELECT_OBJECTS);
	pControlSelect->GetCommandBar()->GetControls()->Add(xtpControlButton, ID_EDIT_SELECT_MULTIPLE_OBJECTS);
	pControlSelect->SetFlags(xtpFlagManualUpdate);



	CXTPRibbonTab* pTabInsert = pRibbonBar->AddTab(ID_TAB_INSERT);

	CXTPRibbonGroup* pGroupShapes = pTabInsert->AddGroup(ID_GROUP_SHAPES);

	pControlGallery = (CXTPControlGallery*)pGroupShapes->Add(
		new CXTPControlGallery(), ID_GALLERY_SHAPES);	
	pControlGallery->SetControlSize(CSize(138, 60));
	pControlGallery->SetResizable();
	pControlGallery->ShowBorders(TRUE);
	pControlGallery->SetItemsMargin(0, -1, 0, -1);
	pControlGallery->ShowLabels(FALSE);
	pControlGallery->SetItems(m_pItemsShapes);

	CMenu menuShapes;
	menuShapes.LoadMenu(ID_INSERT_SHAPES);
	
	pPopupBar = CXTPPopupBar::CreatePopupBar(GetCommandBars());
	pPopupBar->LoadMenu(menuShapes.GetSubMenu(0));
	
	pControlGallery->SetCommandBar(pPopupBar);
	pPopupBar->EnableAnimation();
	pPopupBar->InternalRelease();


	CXTPRibbonGroup* pGroupPages = pTabInsert->AddGroup(ID_GROUP_PAGES);
	pGroupPages->SetIconId(ID_GROUP_BUTTONPOPUP);
	pGroupPages->Add(xtpControlSplitButtonPopup, ID_PAGES_NEW);
	pGroupPages->Add(xtpControlSplitButtonPopup, ID_PAGES_COVER);
	pGroupPages->Add(xtpControlButton, ID_PAGES_BREAK);
	pGroupPages->ShowOptionButton();
	
	CXTPRibbonGroup* pGroupTable = pTabInsert->AddGroup(ID_GROUP_TABLE);
	pGroupTable->Add(xtpControlButtonPopup, ID_TABLE_NEW);

	CXTPRibbonGroup* pGroupIllustrations = pTabInsert->AddGroup(ID_GROUP_ILLUSTRATIONS);
	pGroupIllustrations->Add(xtpControlSplitButtonPopup, ID_ILLUSTRATION_PICTURE);
	pGroupIllustrations->Add(xtpControlButton, ID_ILLUSTRATION_GRAPHIC);
	pGroupIllustrations->Add(xtpControlButton, ID_ILLUSTRATION_CHART);

	CXTPRibbonGroup* pGroupLinks = pTabInsert->AddGroup(ID_GROUP_LINKS);
	pGroupLinks->Add(xtpControlButton, ID_INSERT_HYPERLINK);
	pGroupLinks->Add(xtpControlButton, ID_INSERT_BOOKMARK);
	pGroupLinks->Add(xtpControlButton, ID_INSERT_CROSS_REFERENCE);

	CXTPRibbonGroup* pGroupHeader = pTabInsert->AddGroup(ID_GROUP_HEADERFOOTERS);
	pGroupHeader->Add(xtpControlButtonPopup, ID_INSERT_HEADER);
	pGroupHeader->Add(xtpControlButtonPopup, ID_INSERT_FOOTER);
	pGroupHeader->Add(xtpControlButtonPopup, ID_INSERT_PAGENUMBER);

	CXTPRibbonGroup* pGroupText = pTabInsert->AddGroup(ID_GROUP_TEXT);
	pGroupText->Add(xtpControlButtonPopup, ID_TEXT_TEXTBOX);
	pGroupText->Add(xtpControlSplitButtonPopup, ID_TEXT_PARTS);
	pGroupText->Add(xtpControlButtonPopup, ID_TEXT_WORDART);
	pGroupText->Add(xtpControlButtonPopup, ID_TEXT_DROPCAP);
	pGroupText->Add(xtpControlSplitButtonPopup, ID_TEXT_SIGNATURE);
	pGroupText->Add(xtpControlButton, ID_TEXT_DATETIME);
	pGroupText->Add(xtpControlSplitButtonPopup, ID_TEXT_INSERTOBJECT);

	CXTPRibbonGroup* pGroupSymbols = pTabInsert->AddGroup(ID_GROUP_SYMBOLS);
	pGroupSymbols->Add(xtpControlButtonPopup, ID_SYMBOL_EQUATIONS);
	pGroupSymbols->Add(xtpControlButtonPopup, ID_SYMBOL_SYMBOL);

	pGroupSymbols->GetControlGroupPopup()->SetIconId(ID_SYMBOL_SYMBOL);


	CXTPRibbonTab* pTabPageLayout = pRibbonBar->AddTab(ID_TAB_PAGELAYOUT);
	
	CXTPRibbonGroup* pGroupThemes = pTabPageLayout->AddGroup(ID_GROUP_THEMES);
	pGroupThemes->SetIconId(ID_GROUP_BUTTONPOPUP);
	pGroupThemes->Add(xtpControlSplitButtonPopup, ID_THEMES_THEMES);
	pGroupThemes->Add(xtpControlPopup, ID_THEMES_COLORS);
	pGroupThemes->Add(xtpControlPopup, ID_THEMES_FONTS);
	CXTPControlPopup* pControlPopup = (CXTPControlPopup*)pGroupThemes->Add(xtpControlPopup, ID_THEMES_EFFECTS);
	pControlPopup->GetCommandBar()->GetControls()->Add(new CXTPControlLabel(), ID_THEMES_BUILTIN);
	pControlPopup->GetCommandBar()->GetControls()->Add(xtpControlButton, ID_THEMES_OFFICE);

	CXTPRibbonGroup* pGroupPageSetup = pTabPageLayout->AddGroup(ID_GROUP_PAGESETUP);
	pGroupThemes->SetIconId(ID_GROUP_BUTTONPOPUP);
	pGroupPageSetup->Add(xtpControlPopup, ID_PAGE_MOVEMODE_MOVE);
	pGroupPageSetup->Add(xtpControlPopup, ID_PAGE_MOVEMODE_ORIENTATION);
	pGroupPageSetup->Add(xtpControlPopup, ID_PAGE_MOVEMODE_SCALE);
	pControlPopup = (CXTPControlPopup*)pGroupPageSetup->Add(xtpControlPopup, ID_PAGE_COLUMNS);

	pControlGallery = (CXTPControlGallery*)pControlPopup->GetCommandBar()->GetControls()->Add(new CXTPControlGallery(), ID_GALLERY_COLUMNS);	

	pControlGallery->SetControlSize(CSize(140, 44 * 5));
	pControlGallery->ShowScrollBar(FALSE);
	pControlGallery->SetItems(m_pItemsColumns);
	pControlGallery->SetStyle(xtpButtonIconAndCaption);

	pGroupPageSetup->Add(xtpControlPopup, ID_PAGE_BREAKS);
	pGroupPageSetup->Add(xtpControlPopup, ID_PAGE_LINENUMBERS);
	pGroupPageSetup->Add(xtpControlPopup, ID_PAGE_HYPHENATATION);

	CXTPRibbonGroup* pGroupPageBackground = pTabPageLayout->AddGroup(ID_GROUP_PAGEBACKGROUND);
	pGroupPageBackground->Add(xtpControlPopup, ID_PAGE_TIMEOFDAY);
	pGroupPageBackground->Add(xtpControlPopup, ID_PAGE_MATERIALEDITOR);
	pGroupPageBackground->Add(xtpControlButton, ID_PAGE_BORDERS);

	CXTPRibbonGroup* pGroupPageParagraph = pTabPageLayout->AddGroup(ID_GROUP_PAGEPARAGRAPH);
	pControl = pGroupPageParagraph->Add(xtpControlLabel, ID_PARAGRAPH_INDENT);
	pControl->SetID(0);
	pControl->SetStyle(xtpButtonCaption);

	CXTPControlEdit* pControlEdit = (CXTPControlEdit*)pGroupPageParagraph->Add(xtpControlEdit, ID_PARAGRAPH_INDENTLEFT);
	pControlEdit->SetWidth(133);
	pControlEdit->ShowSpinButtons();

	pControlEdit = (CXTPControlEdit*)pGroupPageParagraph->Add(xtpControlEdit, ID_PARAGRAPH_INDENTRIGHT);
	pControlEdit->SetWidth(133);
	pControlEdit->ShowSpinButtons();

	pControl = pGroupPageParagraph->Add(xtpControlLabel, ID_PARAGRAPH_SPACING);
	pControl->SetBeginGroup(TRUE);
	pControl->SetID(0);
	pControl->SetStyle(xtpButtonCaption);

	pControlEdit = (CXTPControlEdit*)pGroupPageParagraph->Add(xtpControlEdit, ID_PARAGRAPH_SPACINGBEFORE);
	pControlEdit->SetWidth(133);
	pControlEdit->ShowSpinButtons();

	pControlEdit = (CXTPControlEdit*)pGroupPageParagraph->Add(xtpControlEdit, ID_PARAGRAPH_SPACINGAFTER);
	pControlEdit->SetWidth(133);
	pControlEdit->ShowSpinButtons();

	CXTPRibbonGroup* pGroupArrange = pTabPageLayout->AddGroup(ID_GROUP_ARRANGE);
	pGroupArrange->Add(xtpControlPopup, ID_ARRANGE_POSITION);
	pGroupArrange->Add(xtpControlPopup, ID_ARRANGE_FRONT);
	pGroupArrange->Add(xtpControlPopup, ID_ARRANGE_BACK);
	pGroupArrange->Add(xtpControlPopup, ID_ARRANGE_TEXTWRAPPING);

	pGroupArrange->Add(xtpControlPopup, ID_ARRANGE_ALIGN);
	pControlPopup = (CXTPControlPopup*)pGroupArrange->Add(xtpControlPopup, ID_ARRANGE_GROUP);
	pControlPopup->GetCommandBar()->GetControls()->Add(xtpControlButton, ID_ARRANGE_GROUP);
	pControlPopup->GetCommandBar()->GetControls()->Add(xtpControlButton, ID_ARRANGE_UNGROUP);

	pGroupArrange->Add(xtpControlPopup, ID_ARRANGE_ROTATE);
#endif

// 	CXTPRibbonTab* pTabReferences = pRibbonBar->AddTab(ID_TAB_REFERENCES);
// 
// 	pTabReferences->AddGroup(ID_GROUP_TABLEOFCONTENTS);
// 	pTabReferences->AddGroup(ID_GROUP_FOOTNOTES);
// 	pTabReferences->AddGroup(ID_GROUP_CAPTIONS);
// 	pTabReferences->AddGroup(ID_GROUP_INDEX);


	//CXTPRibbonTab* pTab;

// mutex last two tab

// 	pTab = pRibbonBar->AddTab(ID_TAB_MAILINGS);
// 
// 	pTab = pRibbonBar->AddTab(ID_TAB_REVIEW);
// 	pTab->SetContextTab(xtpRibbonTabContextColorPurple);
// 	
// 
// 	CXTPRibbonTab* pTabView = pRibbonBar->AddTab(ID_TAB_VIEW);
// 
// 	pTabView->AddGroup(ID_GROUP_DOCUMENTVIEWS);
// 
// 
// 	CXTPRibbonGroup* pGroupShowHide = pTabView->AddGroup(ID_GROUP_SHOWHIDE);
// 	pGroupShowHide->Add(xtpControlCheckBox, ID_VIEW_RULER);
// 	pGroupShowHide->Add(xtpControlCheckBox, ID_VIEW_GRIDLINES);
// 	pGroupShowHide->Add(xtpControlCheckBox, ID_VIEW_PROPERTIES);
// 	pGroupShowHide->Add(xtpControlCheckBox, ID_VIEW_DOCUMENTMAP);
// 	pGroupShowHide->Add(xtpControlCheckBox, ID_VIEW_STATUS_BAR);
// 	pGroupShowHide->Add(xtpControlCheckBox, ID_VIEW_MESSAGE_BAR);
// 
// 	pTabView->AddGroup(ID_GROUP_ZOOM);
// 	pTabView->AddGroup(ID_GROUP_WINDOW);


// mutex the last two tab [8/27/2011 Kaiming-Desktop]

// 	pTab = pRibbonBar->AddTab(ID_TAB_TABLEDESIGN);
// 	pTab->SetContextTab(xtpRibbonTabContextColorYellow, _T("Table Tools"));
// 	pTab = pRibbonBar->AddTab(ID_TAB_TABLELAYOUT);
// 	pTab->SetContextTab(xtpRibbonTabContextColorYellow, _T("Table Tools"));
// 	
// 	pTab = pRibbonBar->AddTab(ID_TAB_CHARTDESIGN);
// 	pTab->SetContextTab(xtpRibbonTabContextColorGreen, ID_TAB_CONTEXTCHART);
// 	pTab = pRibbonBar->AddTab(ID_TAB_CHARTLAYOUT);
// 	pTab->SetContextTab(xtpRibbonTabContextColorGreen, ID_TAB_CONTEXTCHART);
// 	pTab = pRibbonBar->AddTab(ID_TAB_CHARTFORMAT);
// 	pTab->SetContextTab(xtpRibbonTabContextColorGreen, ID_TAB_CONTEXTCHART);
// 
// 	pTab = pRibbonBar->AddTab(ID_TAB_PICTUREFORMAT);
// 	pTab->SetContextTab(xtpRibbonTabContextColorRed, _T("Picture Tools"));


	
	CXTPControlPopup* pControlOptions = (CXTPControlPopup*)pRibbonBar->GetControls()->Add(xtpControlPopup, -1);
	pControlOptions->SetFlags(xtpFlagRightAlign);
	CMenu mnuOptions;
	mnuOptions.LoadMenu(IDR_MENU_OPTIONS);
	pControlOptions->SetCommandBar(mnuOptions.GetSubMenu(0));
	pControlOptions->SetCaption(_T("Options"));
// 	
// 
// 	CXTPControl* pControlAbout = pRibbonBar->GetControls()->Add(xtpControlButton, ID_APP_ABOUT);
// 	pControlAbout->SetFlags(xtpFlagRightAlign);


	// add controls to quick access [8/27/2011 Kaiming-Desktop]
	pRibbonBar->GetQuickAccessControls()->Add(xtpControlButton, ID_FILE_NEW)->SetHideFlag(xtpHideCustomize, TRUE);
	pRibbonBar->GetQuickAccessControls()->Add(xtpControlButton, ID_FILE_OPEN)->SetHideFlag(xtpHideCustomize, TRUE);
	pRibbonBar->GetQuickAccessControls()->Add(xtpControlButton, ID_FILE_SAVE);
	pRibbonBar->GetQuickAccessControls()->Add(xtpControlButton, ID_SWITCH_CAM1);
	CXTPControlPopup* pControlUndo = (CXTPControlPopup*)pRibbonBar->GetQuickAccessControls()->Add(xtpControlSplitButtonPopup, ID_EDIT_UNDO);


	// popupbar, what for? mutex test [8/27/2011 Kaiming-Desktop]
// 	pPopupBar = CXTPPopupBar::CreatePopupBar(GetCommandBars());
// 	pPopupBar->SetShowGripper(FALSE);
// 	
// 	pControlUndo->SetCommandBar(pPopupBar);
// 	
// 	pControlGallery = new CControlGalleryUndo();
// 	pControlGallery->SetItems(m_pItemsUndo);
// 	pControlGallery->SetWidth(120);
// 	pControlGallery->ShowScrollBar(FALSE);
// 	pPopupBar->GetControls()->Add(pControlGallery, ID_EDIT_UNDO);
// 	
// 	CXTPControlStatic* pControlListBoxInfo = (CXTPControlStatic*)pPopupBar->GetControls()->Add(new CXTPControlStatic(), ID_EDIT_UNDO);
// 	pControlListBoxInfo->SetWidth(120);
// 	pControlListBoxInfo->SetFlags(xtpFlagSkipFocus);
// 	
// 	pPopupBar->InternalRelease();



	pRibbonBar->GetQuickAccessControls()->Add(xtpControlButton, ID_FILE_PRINT);
	pRibbonBar->GetQuickAccessControls()->CreateOriginalControls();


	pRibbonBar->SetCloseable(FALSE);

	pRibbonBar->EnableFrameTheme();

	return TRUE;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CXTPFrameWnd::PreCreateWindow(cs) )
		return FALSE;

	cs.lpszClass = _T("gkStudioClass");
	CXTPDrawHelpers::RegisterWndClass(AfxGetInstanceHandle(), cs.lpszClass, 
		CS_DBLCLKS, AfxGetApp()->LoadIcon(IDR_MAINFRAME));

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CXTPFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CXTPFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers



void CMainFrame::OnEmptyCommandRange(UINT)
{

}

void CMainFrame::OnEmptyCommand()
{

}


void CMainFrame::OnSwitchCommand()
{
	m_bChecked = !m_bChecked;
}

void CMainFrame::OnUpdateSwitchCommand(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_bChecked);
}


void CMainFrame::OnViewShowHide(UINT nID)
{
	m_bOptions[nID - ID_VIEW_RULER] ^= 1;
}

void CMainFrame::OnUpdateViewShowHide(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_bOptions[pCmdUI->m_nID - ID_VIEW_RULER] ? TRUE : FALSE);

}

void CMainFrame::OnRibbonTabChanged(NMHDR* pNMHDR, LRESULT* pRes) 
{
	NMXTPTABCHANGE* pNMTabChanged = (NMXTPTABCHANGE*)pNMHDR;

	if (pNMTabChanged->pTab)
	{
		TRACE(_T("Tab Changed, Caption = %s\n"), (LPCTSTR)pNMTabChanged->pTab->GetCaption());
	}
	
	*pRes = 0;
}

void CMainFrame::OnRibbonTabChanging(NMHDR* /*pNMHDR*/, LRESULT* pRes) 
{

	*pRes = 0;	
}


void CMainFrame::ShowCustomizeDialog(int nSelectedPage)
{
	CXTPCustomizeSheet cs(GetCommandBars());

	CXTPRibbonCustomizeQuickAccessPage pageQuickAccess(&cs);
	cs.AddPage(&pageQuickAccess);
	pageQuickAccess.AddCategories(IDR_MAINFRAME);

	CXTPCustomizeKeyboardPage pageKeyboard(&cs);
	cs.AddPage(&pageKeyboard);
	pageKeyboard.AddCategories(IDR_MAINFRAME);

	CXTPCustomizeMenusPage pageMenus(&cs);
	cs.AddPage(&pageMenus);

	CXTPCustomizeOptionsPage pageOptions(&cs);
	cs.AddPage(&pageOptions);

	CXTPCustomizeCommandsPage* pCommands = cs.GetCommandsPage();
	pCommands->AddCategories(IDR_MAINFRAME);

	cs.SetActivePage(nSelectedPage);

	cs.DoModal();
}

void CMainFrame::OnCustomize()
{
	ShowCustomizeDialog(0);
}

void CMainFrame::OnCustomizeQuickAccess()
{
	ShowCustomizeDialog(2);
}


void CMainFrame::OnClose() 
{
#ifndef _SAVE_TO_XML
	SaveCommandBars(_T("CommandBars3"));
#else

	CXTPPropExchangeXMLNode px(FALSE, 0, _T("Settings"));

	if (px.OnBeforeExchange()) 
	{
		CXTPPropExchangeSection pxCommandBars(px.GetSection(_T("CommandBars")));
		XTP_COMMANDBARS_PROPEXCHANGE_PARAM param; 
		param.bSerializeControls = TRUE; 
		param.bSaveOriginalControls = FALSE;
		param.bSerializeOptions = TRUE;
		GetCommandBars()->DoPropExchange(&pxCommandBars, &param);

		px.SaveToFile(m_strIniFileName);
	}
#endif

	m_paneManager.Destroy();
	m_barSwissKnife.Destroy();
	
	CXTPFrameWnd::OnClose();
}

int CMainFrame::OnCreateControl(LPCREATECONTROLSTRUCT lpCreateControl)
{
	if (lpCreateControl->nID == ID_GALLERY_STYLESET)
	{
		CXTPControlGallery* pControlGallery = new CXTPControlGallery();
		pControlGallery->SetControlSize(CSize(200, 22 * 7 + 2));
		pControlGallery->SetResizable();
		pControlGallery->ShowLabels(TRUE);
		pControlGallery->SetItems(m_pItemsStyleSet);

		lpCreateControl->pControl = pControlGallery;
		return TRUE;
	}

	if (lpCreateControl->nID == ID_GALLERY_COLORS)
	{
		CXTPControlGallery* pControlGallery = new CXTPControlGallery();
		pControlGallery->SetControlSize(CSize(200, 20 * 9 + 2));
		pControlGallery->ShowLabels(TRUE);
		pControlGallery->SetItems(m_pItemsThemeColors);

		lpCreateControl->pControl = pControlGallery;
		return TRUE;
	}

	if (lpCreateControl->nID == ID_GALLERY_FONTBACKCOLOR)
	{
		CXTPControlGallery* pControlGallery = new CXTPControlGallery();
		pControlGallery->SetControlSize(CSize(30 * 5, 30 * 3 + 2));
		pControlGallery->ShowScrollBar(FALSE);
		pControlGallery->SetItems(m_pItemsFontBackColor);

		lpCreateControl->pControl = pControlGallery;
		return TRUE;
	}
	if (lpCreateControl->nID == ID_GALLERY_FONTS)
	{
		CXTPControlGallery* pControlGallery = new CXTPControlGallery();
		pControlGallery->SetControlSize(CSize(250, 60 * 5 + 2 + 20));
		pControlGallery->ShowLabels(TRUE);
		pControlGallery->SetItems(m_pItemsThemeFonts);

		lpCreateControl->pCommandBar->EnableAnimation();

		lpCreateControl->pControl = pControlGallery;
		return TRUE;
	}

	if (lpCreateControl->nID == ID_GALLERY_STYLES)
	{
		CXTPControlGallery* pControlGallery = new CXTPControlGallery();
		pControlGallery->SetControlSize(CSize(235 + 72, 56 * 3 + 2));
		pControlGallery->SetResizable();
		pControlGallery->SetItems(m_pItemsStyles);

		lpCreateControl->pControl = pControlGallery;
		return TRUE;
	}

	if (lpCreateControl->nID == ID_GALLERY_SHAPES)
	{
		CXTPControlGallery* pControlGallery = new CXTPControlGallery();
		pControlGallery->SetControlSize(CSize(200, 200));
		pControlGallery->SetResizable();
		pControlGallery->ShowLabels(TRUE);
		pControlGallery->SetItems(m_pItemsShapes);
		
		lpCreateControl->pControl = pControlGallery;
		return TRUE;
	}

	if (lpCreateControl->nID == ID_CONTENTS_TABLE)
	{		
		lpCreateControl->controlType = xtpControlButtonPopup;
		return TRUE;
	}

	if (lpCreateControl->nID >= ID_OPTIONS_STYLEBLUE && lpCreateControl->nID <= ID_OPTIONS_STYLESYSTEM)
	{		
		lpCreateControl->controlType = xtpControlRadioButton;
		return TRUE;
	}

	if (lpCreateControl->nID >= ID_OPTIONS_FONT_SYSTEM && lpCreateControl->nID <= ID_OPTIONS_FONT_EXTRALARGE)
	{		
		lpCreateControl->controlType = xtpControlRadioButton;
		return TRUE;
	}

	if (lpCreateControl->nID == ID_FOOTNOTES_NEXT)
	{		
		lpCreateControl->controlType = xtpControlSplitButtonPopup;
		return TRUE;
	}

	if (lpCreateControl->nID == ID_FILE_RECENTDOCUMENTS)
	{		
		lpCreateControl->pControl = new CXTPRibbonControlSystemRecentFileList();
		return TRUE;
	}

	if (lpCreateControl->nID == ID_APP_EXIT || lpCreateControl->nID == ID_APP_OPTIONS)
	{		
		lpCreateControl->pControl = new CXTPRibbonControlSystemPopupBarButton();

		return TRUE;
	}
	if (lpCreateControl->nID == ID_FILE_PRINT && lpCreateControl->nIndex == 5)
	{		
		CXTPControlPopup* pControl = CXTPControlPopup::CreateControlPopup(xtpControlSplitButtonPopup);

		CMenu menu;
		menu.LoadMenu(IDR_MENU_PRINT);

		CXTPRibbonSystemPopupBarPage* pPopupBar = new CXTPRibbonSystemPopupBarPage();
		pPopupBar->SetCommandBars(GetCommandBars());

		pPopupBar->LoadMenu(menu.GetSubMenu(0));

		CXTPControl* pControlLabel = pPopupBar->GetControls()->Add(xtpControlLabel, ID_FILE_PRINT_LABEL, NULL, 0);
		pControlLabel->SetWidth(296);
		pControlLabel->SetItemDefault(TRUE);
		pControlLabel->SetStyle(xtpButtonCaption);

		pPopupBar->SetShowGripper(FALSE);
		pPopupBar->SetDefaultButtonStyle(xtpButtonCaptionAndDescription);
		pPopupBar->SetIconSize(CSize(32, 32));

		pControl->SetCommandBar(pPopupBar);

		pPopupBar->InternalRelease();

		lpCreateControl->pControl = pControl;
		return TRUE;
	}
	if (lpCreateControl->nID == ID_FILE_SEND)
	{		
		CXTPControlPopup* pControl = CXTPControlPopup::CreateControlPopup(xtpControlButtonPopup);

		CMenu menu;
		menu.LoadMenu(IDR_MENU_SEND);

		CXTPRibbonSystemPopupBarPage* pPopupBar = new CXTPRibbonSystemPopupBarPage();
		pPopupBar->SetCommandBars(GetCommandBars());

		pPopupBar->LoadMenu(menu.GetSubMenu(0));

		CXTPControl* pControlLabel = pPopupBar->GetControls()->Add(xtpControlLabel, ID_FILE_SEND_LABEL, NULL, 0);
		pControlLabel->SetWidth(296);
		pControlLabel->SetItemDefault(TRUE);
		pControlLabel->SetStyle(xtpButtonCaption);

		pPopupBar->SetShowGripper(FALSE);
		pPopupBar->SetDefaultButtonStyle(xtpButtonCaptionAndDescription);
		pPopupBar->SetIconSize(CSize(32, 32));

		pControl->SetCommandBar(pPopupBar);

		pPopupBar->InternalRelease();

		pControl->SetFlags(xtpFlagManualUpdate);

		lpCreateControl->pControl = pControl;
		return TRUE;
	}
	return FALSE;

}
void CMainFrame::OnGalleryShapes(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMXTPCONTROL* tagNMCONTROL = (NMXTPCONTROL*)pNMHDR;

	CXTPControlGallery* pGallery = DYNAMIC_DOWNCAST(CXTPControlGallery, tagNMCONTROL->pControl);
	
	if (pGallery)
	{
		CXTPControlGalleryItem* pItem = pGallery->GetItem(pGallery->GetSelectedItem());
		if (pItem)
		{
			m_nShape = pItem->GetID();
		}

		*pResult = TRUE; // Handled
	}
}

void CMainFrame::OnUpdateGalleryShapes(CCmdUI* pCmdUI)
{
	CXTPControlGallery* pGallery = 	DYNAMIC_DOWNCAST(CXTPControlGallery, CXTPControl::FromUI(pCmdUI));
	
	if (pGallery)
	{
		pGallery->SetCheckedItem(m_nShape);
	}
	pCmdUI->Enable(TRUE);
}



void CMainFrame::OnGalleryStyles(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMXTPCONTROL* tagNMCONTROL = (NMXTPCONTROL*)pNMHDR;

	CXTPControlGallery* pGallery = DYNAMIC_DOWNCAST(CXTPControlGallery, tagNMCONTROL->pControl);
	
	if (pGallery)
	{
		CXTPControlGalleryItem* pItem = pGallery->GetItem(pGallery->GetSelectedItem());
		if (pItem)
		{
			m_nStyle = pItem->GetID();
		}

		*pResult = TRUE; // Handled
	}
}

void CMainFrame::OnUpdateGalleryStyles(CCmdUI* pCmdUI)
{
	CXTPControlGallery* pGallery = 	DYNAMIC_DOWNCAST(CXTPControlGallery, CXTPControl::FromUI(pCmdUI));
	
	if (pGallery)
	{
		if (pGallery->GetCheckedItem() != m_nStyle)
		{
			pGallery->SetCheckedItem(m_nStyle);
			pGallery->EnsureVisible(m_nStyle);
		}
	}
	pCmdUI->Enable(TRUE);
}

void CMainFrame::OnGalleryColumns(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMXTPCONTROL* tagNMCONTROL = (NMXTPCONTROL*)pNMHDR;

	CXTPControlGallery* pGallery = DYNAMIC_DOWNCAST(CXTPControlGallery, tagNMCONTROL->pControl);
	
	if (pGallery)
	{
		CXTPControlGalleryItem* pItem = pGallery->GetItem(pGallery->GetSelectedItem());
		if (pItem)
		{
			m_nColumns = pItem->GetID();
		}

		*pResult = TRUE; // Handled
	}
}

void CMainFrame::OnUpdateGalleryColumns(CCmdUI* pCmdUI)
{
	CXTPControlGallery* pGallery = 	DYNAMIC_DOWNCAST(CXTPControlGallery, CXTPControl::FromUI(pCmdUI));
	
	if (pGallery)
	{
		pGallery->SetCheckedItem(m_nColumns);
	}
	pCmdUI->Enable(TRUE);
}




void CMainFrame::OnGalleryStyleSet(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMXTPCONTROL* tagNMCONTROL = (NMXTPCONTROL*)pNMHDR;

	CXTPControlGallery* pGallery = DYNAMIC_DOWNCAST(CXTPControlGallery, tagNMCONTROL->pControl);
	
	if (pGallery)
	{
		CXTPControlGalleryItem* pItem = pGallery->GetItem(pGallery->GetSelectedItem());
		if (pItem)
		{
			m_nStyleSet = pItem->GetID();
		}

		*pResult = TRUE; // Handled
	}
}

void CMainFrame::OnUpdateGalleryStyleSet(CCmdUI* pCmdUI)
{
	CXTPControlGallery* pGallery = 	DYNAMIC_DOWNCAST(CXTPControlGallery, CXTPControl::FromUI(pCmdUI));
	
	if (pGallery)
	{
		pGallery->SetCheckedItem(m_nStyleSet);
	}
	pCmdUI->Enable(TRUE);
}


void CMainFrame::OnGalleryThemeColors(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMXTPCONTROL* tagNMCONTROL = (NMXTPCONTROL*)pNMHDR;

	CXTPControlGallery* pGallery = DYNAMIC_DOWNCAST(CXTPControlGallery, tagNMCONTROL->pControl);
	
	if (pGallery)
	{
		CXTPControlGalleryItem* pItem = pGallery->GetItem(pGallery->GetSelectedItem());
		if (pItem)
		{
			m_nThemeColors = pItem->GetID();
			
			m_pItemsFontTextColor->RemoveAll();
			pItem = m_pItemsFontTextColor->AddLabel(0);
			pItem->SetCaption(_T("Theme Colors"));
			CGalleryItemFontColor::AddThemeColors(m_pItemsFontTextColor, m_nThemeColors);
			
			pItem = m_pItemsFontTextColor->AddLabel(0);
			pItem->SetCaption(_T("Standard Colors"));
			CGalleryItemFontColor::AddStandardColors(m_pItemsFontTextColor);

		}

		*pResult = TRUE; // Handled
	}
}

void CMainFrame::OnUpdateGalleryThemeColors(CCmdUI* pCmdUI)
{
	CXTPControlGallery* pGallery = 	DYNAMIC_DOWNCAST(CXTPControlGallery, CXTPControl::FromUI(pCmdUI));
	
	if (pGallery)
	{
		pGallery->SetCheckedItem(m_nThemeColors);
	}
	pCmdUI->Enable(TRUE);
}


void CMainFrame::OnGalleryThemeFonts(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMXTPCONTROL* tagNMCONTROL = (NMXTPCONTROL*)pNMHDR;

	CXTPControlGallery* pGallery = DYNAMIC_DOWNCAST(CXTPControlGallery, tagNMCONTROL->pControl);
	
	if (pGallery)
	{
		CXTPControlGalleryItem* pItem = pGallery->GetItem(pGallery->GetSelectedItem());
		
		if (pItem)
		{
			m_nThemeFonts = pItem->GetID();
		}

		*pResult = TRUE; // Handled
	}
}

void CMainFrame::OnUpdateGalleryThemeFonts(CCmdUI* pCmdUI)
{
	CXTPControlGallery* pGallery = 	DYNAMIC_DOWNCAST(CXTPControlGallery, CXTPControl::FromUI(pCmdUI));
	
	if (pGallery)
	{
		pGallery->SetCheckedItem(m_nThemeFonts);
	}
	pCmdUI->Enable(TRUE);
}


void CMainFrame::OnOptionsAnimation() 
{
	m_bAnimation ^= 1;
	GetCommandBars()->GetMenuBar()->EnableAnimation(m_bAnimation);
	
}

void CMainFrame::OnUpdateOptionsAnimation(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bAnimation);	
}



void CMainFrame::OnOptionsRighttoleft() 
{
	if (GetExStyle() & WS_EX_LAYOUTRTL)
	{
		GetCommandBars()->SetLayoutRTL(FALSE);
		ModifyStyleEx(WS_EX_LAYOUTRTL, 0);

		((CWorkspaceView*)GetDlgItem(AFX_IDW_PANE_FIRST))->SetLayoutRTL(FALSE);
	}
	else
	{
		GetCommandBars()->SetLayoutRTL(TRUE);
		ModifyStyleEx(0, WS_EX_LAYOUTRTL);

		((CWorkspaceView*)GetDlgItem(AFX_IDW_PANE_FIRST))->SetLayoutRTL(TRUE);
	}	

	m_wndMessageBar.ModifyStyleEx(WS_EX_LAYOUTRTL, GetExStyle() & WS_EX_LAYOUTRTL);

	RecalcLayout(FALSE);
}

void CMainFrame::OnUpdateOptionsRighttoleft(CCmdUI* pCmdUI) 
{
	if (XTPSystemVersion()->IsLayoutRTLSupported())
	{
		pCmdUI->SetCheck(GetExStyle() & WS_EX_LAYOUTRTL ? TRUE : FALSE);	
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}

void CMainFrame::OnUpdateOptionsStyle(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(pCmdUI->m_nID == m_nRibbonStyle ? 1 : 0);

}
void CMainFrame::OnOptionsStyle(UINT nStyle)
{
	CXTPRibbonBar* pRibbonBar = (CXTPRibbonBar*)GetCommandBars()->GetMenuBar();
	pRibbonBar->EnableFrameTheme(FALSE);
	//XTPSkinManager()->ApplyWindow(this->GetSafeHwnd());

	GetCommandBars()->SetTheme(xtpThemeRibbon);

	HMODULE hModule = AfxGetInstanceHandle();

	LPCTSTR lpszIniFile = 0;
	switch (nStyle)
	{
		case ID_OPTIONS_GKBLACK: 
			XTPSkinManager()->LoadSkin(m_csStylesPath + _T("gkBlack.cjstyles"));
			XTPSkinManager()->ApplyWindow(this->GetSafeHwnd());
			hModule = LoadLibrary(m_csStylesPath + _T("Office2010.dll"));
			lpszIniFile = _T("OFFICE2010BLACK.INI"); break;
		case ID_OPTIONS_GKWHITE: 
			XTPSkinManager()->LoadSkin(m_csStylesPath + _T("gkWhite.cjstyles"));
			XTPSkinManager()->ApplyWindow(this->GetSafeHwnd());
			hModule = LoadLibrary(m_csStylesPath + _T("Office2010.dll"));
			lpszIniFile = _T("OFFICE2010SILVER.INI"); break;
	}


	if (hModule != 0)
	{
		((CXTPOffice2007Theme*)GetCommandBars()->GetPaintManager())->SetImageHandle(hModule, lpszIniFile);
	}

	m_paneManager.RefreshMetrics(TRUE);

	

// 	if (nStyle == ID_OPTIONS_STYLEWHITE || nStyle == ID_OPTIONS_STYLESCENIC)
// 		pRibbonBar->GetSystemButton()->SetStyle(xtpButtonCaption);
// 	else
	pRibbonBar->GetSystemButton()->SetStyle(xtpButtonAutomatic);


	GetCommandBars()->GetImageManager()->RefreshAll();
	GetCommandBars()->RedrawCommandBars();
	SendMessage(WM_NCPAINT);

	RedrawWindow(0, 0, RDW_ALLCHILDREN|RDW_INVALIDATE);



	m_nRibbonStyle = nStyle;

	pRibbonBar->EnableFrameTheme();
}

void CMainFrame::OnToggleGroups() 
{
	CXTPRibbonBar* pRibbonBar = (CXTPRibbonBar*)GetCommandBars()->GetAt(0);
	
	pRibbonBar->SetRibbonMinimized(!pRibbonBar->IsRibbonMinimized());
}

void CMainFrame::OnAutoResizeIcons()
{
	GetCommandBars()->GetPaintManager()->m_bAutoResizeIcons ^= 1;
	GetCommandBars()->RedrawCommandBars();
}

void CMainFrame::OnUpdateAutoResizeIcons(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(GetCommandBars()->GetPaintManager()->m_bAutoResizeIcons);
}

int nFonts[] = {0, 11, 13, 16};

void CMainFrame::OnOptionsFont(UINT nID)
{
	int nFontHeight = nFonts[nID - ID_OPTIONS_FONT_SYSTEM];

	CXTPRibbonBar* pRibbonBar = (CXTPRibbonBar*)GetCommandBars()->GetAt(0);

	pRibbonBar->SetFontHeight(nFontHeight);

}

void CMainFrame::OnUpdateOptionsFont(CCmdUI* pCmdUI)
{
	int nFontHeight = nFonts[pCmdUI->m_nID - ID_OPTIONS_FONT_SYSTEM];

	CXTPRibbonBar* pRibbonBar = (CXTPRibbonBar*)GetCommandBars()->GetAt(0);

	pCmdUI->SetCheck(pRibbonBar->GetFontHeight() == nFontHeight ? TRUE : FALSE);

}

void CMainFrame::OnFrameTheme()
{
	ShowWindow(SW_NORMAL);
	CXTPRibbonBar* pRibbonBar = (CXTPRibbonBar*)GetCommandBars()->GetAt(0);

	CXTPWindowRect rc(this);
	rc.top += (pRibbonBar->IsFrameThemeEnabled() ? -1 : +1) * GetSystemMetrics(SM_CYCAPTION);
	MoveWindow(rc);
	
	pRibbonBar->EnableFrameTheme(!pRibbonBar->IsFrameThemeEnabled());

}

void CMainFrame::OnUpdateFrameTheme(CCmdUI* pCmdUI)
{
	CXTPRibbonBar* pRibbonBar = (CXTPRibbonBar*)GetCommandBars()->GetAt(0);

	pCmdUI->SetCheck(pRibbonBar->IsFrameThemeEnabled() ? TRUE : FALSE);
}

void CMainFrame::OnStatusBarSwitchView(UINT nID)
{
	CXTPStatusBarSwitchPane* pPane = (CXTPStatusBarSwitchPane*)m_wndStatusBar.FindPane(ID_INDICATOR_VIEWSHORTCUTS);

	pPane->SetChecked(nID);
}

void CMainFrame::OnUpdateKeyIndicator(CCmdUI* pCmdUI)
{
	CXTPStatusBar* pStatusBar = DYNAMIC_DOWNCAST(CXTPStatusBar, pCmdUI->m_pOther);
	if (!pStatusBar)
		return;

	CXTPStatusBarPane* pPane = pStatusBar->GetPane(pCmdUI->m_nIndex);

	BOOL bVisible = ::GetKeyState(VK_CAPITAL) & 0x0001;
	pPane->SetVisible(bVisible);
	pPane->SetValue(bVisible ? _T("On") : _T("Off"));
}

void CMainFrame::SetProgressBar(int progress)
{
	CXTPStatusBarProgressPane* pPane = DYNAMIC_DOWNCAST(CXTPStatusBarProgressPane, m_wndStatusBar.FindPane(ID_INDICATOR_PROGRESS));
	if (!pPane)
		return;

	if (progress < 100)
	{
		pPane->SetVisible(TRUE);
	}
	else
	{
		pPane->SetVisible(FALSE);
	}


	CRect rect;
	m_wndStatusBar.GetClientRect(&rect);
	pPane->SetWidth(rect.Width() - 400);


	pPane->SetPos(progress);
	m_wndStatusBar.UpdateWindow();
}
//-----------------------------------------------------------------------
void CMainFrame::OnUpdateSwitchMovementMode( CCmdUI* pCmdUI )
{
	for (uint8 i=0; i < m_pGroupMovement->GetCount(); ++i)
	{
		if (i != m_uChecked)
			m_pGroupMovement->GetAt(i)->SetChecked(FALSE);
	}
	m_pGroupMovement->GetAt(m_uChecked)->SetChecked(TRUE);
	
	//pCmdUI->SetCheck(TRUE);

}
//-----------------------------------------------------------------------
void CMainFrame::OnSwitchMovementModeSelect()
{
	m_uChecked = GKSTUDIO_OBJMODE_SEL;
	GetIEditor()->SetObjectSelMode(GKSTUDIO_OBJMODE_SEL);
}
//-----------------------------------------------------------------------
void CMainFrame::OnSwitchMovementModeMove()
{
	m_uChecked = GKSTUDIO_OBJMODE_MOV;
	GetIEditor()->SetObjectSelMode(GKSTUDIO_OBJMODE_MOV);
}
//-----------------------------------------------------------------------
void CMainFrame::OnSwitchMovementModeRotate()
{
	m_uChecked = GKSTUDIO_OBJMODE_ROT;
	GetIEditor()->SetObjectSelMode(GKSTUDIO_OBJMODE_ROT);
}
//-----------------------------------------------------------------------
void CMainFrame::OnSwitchMovementModeScale()
{
	m_uChecked = GKSTUDIO_OBJMODE_SCL;
	GetIEditor()->SetObjectSelMode(GKSTUDIO_OBJMODE_SCL);
}

//-----------------------------------------------------------------------
BOOL CMainFrame::OnCopyData( CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct )
{
	switch (pCopyDataStruct->dwData)
	{
		case CPDATA_UPDATE_MESH:
			{
// 				char str[1024];
// 				memcpy( str,pCopyDataStruct->lpData,pCopyDataStruct->cbData );
// 				str[pCopyDataStruct->cbData] = 0;
// 
// 				TCHAR wszFileName[MAX_PATH];
// 				MultiByteToWideChar( CP_ACP, 0, str, -1, wszFileName, MAX_PATH );
// 
// 
// 				gEnv->pSystem->getMeshMngPtr()->reload(wszFileName);

				break;
			}
	

		case CPDATA_SYNC_SCENE:
			{
// 				char str[1024];
// 				memcpy( str,pCopyDataStruct->lpData,pCopyDataStruct->cbData );
// 				str[pCopyDataStruct->cbData] = 0;
// 
// 				TCHAR wszFileName[MAX_PATH];
// 				MultiByteToWideChar( CP_ACP, 0, str, -1, wszFileName, MAX_PATH );
// 
// 
// 				gEnv->pSystem->LoadScene_s(wszFileName, NULL);

				break;
			}

	}

	return __super::OnCopyData(pWnd, pCopyDataStruct);
}




void CMainFrame::OnSwitchCam(UINT ID)
{
	uint8 camid = 0;
	switch (ID)
	{
	case ID_SWITCH_CAM1:
		camid = 1;
		break;
	case ID_SWITCH_CAM2:
		camid = 2;
		break;
	case ID_SWITCH_CAM3:
		camid = 3;
		break;
	case ID_SWITCH_CAM4:
		camid = 4;
		break;
	case ID_SWITCH_CAM5:
		camid = 5;
		break;
	case ID_SWITCH_CAM6:
		camid = 6;
		break;
	case ID_SWITCH_CAM7:
		camid = 7;
		break;
	case ID_SWITCH_CAM8:
		camid = 8;
		break;
	case ID_SWITCH_CAM9:
		camid = 9;
		break;

	}
	// TODO: Add your command handler code here
	gkLogMessage( _T("Switch to Cam%d."), camid );
	GetIEditor()->getCameraPosManager()->SwitchToCam(camid);
}


void CMainFrame::OnSaveCam(UINT ID)
{
	uint8 camid = 0;
	switch (ID)
	{
	case ID_SAVE_CAM1:
		camid = 1;
		break;
	case ID_SAVE_CAM2:
		camid = 2;
		break;
	case ID_SAVE_CAM3:
		camid = 3;
		break;
	case ID_SAVE_CAM4:
		camid = 4;
		break;
	case ID_SAVE_CAM5:
		camid = 5;
		break;
	case ID_SAVE_CAM6:
		camid = 6;
		break;
	case ID_SAVE_CAM7:
		camid = 7;
		break;
	case ID_SAVE_CAM8:
		camid = 8;
		break;
	case ID_SAVE_CAM9:
		camid = 9;
		break;

	}
	// TODO: Add your command handler code here
	gkLogMessage( _T("Save to Cam%d."), camid );
	GetIEditor()->getCameraPosManager()->SaveCamPose(camid);
}


void CMainFrame::OnGlobalFocusobj()
{
	// TODO: Add your command handler code here
	GetIEditor()->focusGameObject( GetIEditor()->getObjectMng()->getCurrSelection() );
}

void CMainFrame::OnLanguage( UINT id)
{
// 	WORD fisrt;
// 	WORD second;
// 	switch( id )
// 	{
// 	case ID_OPTIONS_CHINESE:
// 		fisrt = LANG_CHINESE;
// 		second = SUBLANG_CHINESE_SIMPLIFIED;
// 		break;
// 	case ID_OPTIONS_ENGLISH:
// 		fisrt = LANG_ENGLISH;
// 		second = SUBLANG_ENGLISH_US;
// 
// 		break;
// 	}
// 
// 	LCID lcidNew = GetThreadLocale();
// 
// 	if (LANG_ENGLISH == PRIMARYLANGID(LANGIDFROMLCID(lcidNew)))
// 		lcidNew = MAKELCID(MAKELANGID(fisrt, second), SORT_DEFAULT);
}

void CMainFrame::OnSwitchPanes( UINT id )
{
	switch(id)
	{
	case ID_PANES_CONSOLE:
		m_paneManager.SwitchPanel(_T("Console Panel"));
		break;
	case ID_PANES_CVAR:
		m_paneManager.SwitchPanel(_T("ConsoleVar Panel"));
		break;
	case ID_PANES_TIMEOFDAYEDITOR:
		m_paneManager.SwitchPanel(_T("TimeOfDay Editor"));
		break;
	case ID_PANES_MATERIALEDITOR:
		m_paneManager.SwitchPanel(_T("Material Editor"));
		break;
	case ID_PANES_SWISSKNIFEBAR:
		m_paneManager.SwitchPanel(_T("SwissKnifeBar"));
		break;
	case ID_PANES_HIERARCHYEDITOR:
		m_paneManager.SwitchPanel(_T("Hierarchy"));
		break;
	case ID_PANES_TRACKBUSEDITOR:
		m_paneManager.SwitchPanel(_T("TrackBus Editor"));
		break;
	case ID_PANES_TERRIANEDITOR:
		{
			ITerrianSystem* terrian = gEnv->p3DEngine->createTerrian();
			if (terrian)
			{
				terrian->ModifyVegetationDensityMap(_T("/terrian/basic_terrian/vegs.raw"));
				terrian->Create(_T("/terrian/basic_terrian/default.raw"), 8, 512);

				terrian->ModifyColorMap(_T("/terrian/basic_terrian/color.tga"));
				terrian->ModifyAreaMap(_T("/terrian/basic_terrian/divide.tga"));
				terrian->ModifyDetailMap(_T("/terrian/basic_terrian/detail.tga"), 0);
				terrian->ModifyDetailMap(_T("/terrian/basic_terrian/detail_ddn.tga"), 1);
				terrian->ModifyDetailMap(_T("/terrian/basic_terrian/detail_s.tga"), 2);
			}
		}
		break;
	}
}

void CMainFrame::OnFullScreen()
{
	//bool toggle = !gEnv->pRenderer->FullScreenMode();
	gEnv->pRenderer->SetCurrContent( NULL, 0, 0, 1280, 720, true );
}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	switch( pMsg->message )
	{
	case WM_KEYDOWN:
		{
			switch(pMsg->wParam)
			{
			case VK_F8:
				OnFullScreen();
				break;
			}
		}
		break;
	}

	return CXTPFrameWnd::PreTranslateMessage(pMsg);
}
