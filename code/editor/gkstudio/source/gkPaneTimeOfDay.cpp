// F:\_PersonalProjects\_gkEngine\trunk\Projects\gkStudio\source\gkPaneTimeOfDay.cpp : implementation file
//

#include "stdafx.h"
#include "gkPaneTimeOfDay.h"
#include "..\Controls\CustomItems.h"
#include "ITimeOfDay.h"
#include "I3DEngine.h"


// gkPaneTimeOfDay dialog

//IMPLEMENT_DYNAMIC(gkPaneTimeOfDay, CDialog)

gkPaneTimeOfDay::gkPaneTimeOfDay(CWnd* pParent /*=NULL*/)
: gkToolBarPaneBase(gkPaneTimeOfDay::IDD, pParent)
, m_edtTodTime(0)
, m_bPlaying(false)
{
	//{{AFX_DATA_INIT(CPaneStyles)
	//}}AFX_DATA_INIT
}

//-----------------------------------------------------------------------
gkPaneTimeOfDay::~gkPaneTimeOfDay()
{
\
}

void gkPaneTimeOfDay::DoDataExchange(CDataExchange* pDX)
{
	gkToolBarPaneBase::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_PROPPLACEHOLDER, m_wndPlaceHolder);
	DDX_Control(pDX, IDC_TODSLIDER, m_ctrlSlider);
}


BEGIN_MESSAGE_MAP(gkPaneTimeOfDay, gkToolBarPaneBase)


	//{{AFX_MSG_MAP(CPaneStyles)
	ON_WM_CTLCOLOR()
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()

	ON_MESSAGE(XTPWM_PROPERTYGRID_NOTIFY, OnGridNotify)

	//ON_BN_CLICKED( IDC_BUTTON_CVAR , OnCVar )

	//}}AFX_MSG_MAP
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_TODSLIDER, &gkPaneTimeOfDay::OnNMCustomdrawTodslider)

	ON_COMMAND_RANGE(ID_BUTTON_TODEDT_PLAY, ID_BUTTON_TODEDT_PSTKEY, OnCommandRange)

END_MESSAGE_MAP()


void gkPaneTimeOfDay::OnSysCommand(UINT nID, LPARAM lParam)
{
	gkToolBarPaneBase::OnSysCommand(nID, lParam);
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void gkPaneTimeOfDay::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;
	}
	else
	{
		gkToolBarPaneBase::OnPaint();
	}
}

BOOL gkPaneTimeOfDay::OnInitDialog()
{
	gkToolBarPaneBase::OnInitDialog();

	SetResize(IDC_TODSLIDER,       SZ_TOP_LEFT, SZ_TOP_RIGHT);
	

	RefreshMetrics(TRUE);

	m_ctrlSlider.SetRange(0,240);
	m_ctrlSlider.SetPos(90);  


	// add listener
	GetIEditor()->addUpdateListener(this);

	// init the property table [10/27/2011 Kaiming]
	InitPropertyTable();

	RegisterPane(&m_ctrlSlider, _T("Timeline"), xtpPaneDockTop, CSize(500, 50), xtpPaneNoCaption);
	OpenPane(_T("Timeline"));

	RegisterPane(&m_wndPropertyGrid, _T("Time Of Day Properties"), xtpPaneDockBottom, CSize(2048, 1024), xtpPaneNoCaption);
	OpenPane(_T("Time Of Day Properties"));

	InitToolBar(_T("ToolBar_TodEdt"), IDR_TOOLBAR_TODEDITOR);

	return TRUE;
}

	/////////////////////////////////////////////////////////////////////////////
	// CPaneStyles message handlers


void gkPaneTimeOfDay::RefreshMetrics( BOOL bOffice2007 )
{
	m_brush.DeleteObject();

	if (bOffice2007)
	{
		COLORREF clr = XTPResourceImages()->GetImageColor(_T("DockingPane"), _T("WordPaneBackground"));
		m_brush.CreateSolidBrush(clr);
	}
	else
	{
		m_brush.CreateSolidBrush(GetSysColor(COLOR_3DFACE));
	}
}

HBRUSH gkPaneTimeOfDay::OnCtlColor( CDC* pDC, CWnd* pWnd, UINT nCtlColor )
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	return m_brush;
}

BOOL gkPaneTimeOfDay::InitPropertyTable()
{
	CRect rc;
	m_wndPlaceHolder.GetWindowRect( &rc );
	ScreenToClient( &rc );

// create the property grid.
if ( m_wndPropertyGrid.Create( rc, this, IDC_TODPROPERTY ) )
{
	m_wndPropertyGrid.SetVariableItemsHeight(TRUE);
	m_wndPropertyGrid.SetOwner(this);

	LOGFONT lf;
	GetFont()->GetLogFont( &lf );


	CXTPPropertyGridItem* pItem = NULL;

	// create global settings category.
	CXTPPropertyGridItem* pGlobals      = m_wndPropertyGrid.AddCategory(_T("全局设定"));

	// add child items to category.
	m_wndPropertyGrid.AddFloatVar(pGlobals, _T("HDR增益"), HDRBRIGHTNESS, &(m_curKey.fHDRBrightness), 0, 10, _T("控制HDR的整体强度"));
	m_wndPropertyGrid.AddFloatVar(pGlobals, _T("HDR对比度"), HDRCONSTACT, &(m_curKey.fHDRConstract), 0, 3, _T("控制HDR的整体对比"));
	m_wndPropertyGrid.AddFloatVar(pGlobals, _T("SSAO增益"), SSAOINTENSITY, &(m_curKey.fSSAOIntensity), 0, 2, _T("控制全局遮蔽效果强度"));

	m_wndPropertyGrid.AddColorVar(pGlobals, _T("天光颜色") , CLSKYLIGHT, &(m_curKey.clSkyLight.r), _T("模拟阴天整体天光，影响整体色彩氛围"), this );
	m_wndPropertyGrid.AddColorVar(pGlobals, _T("地面氛围颜色") , CLGROUNDAMB, &(m_curKey.clGroundAmb.r),  _T("地面初始颜色，和天光配合模拟环境遮罩"  ));


	m_wndPropertyGrid.AddFloatVar(pGlobals, _T("环境遮罩最低高度"), GROUNDAMBMIN, &(m_curKey.fGroundAmbMin), 0, 100, _T("控制环境遮罩的起始高度"));
	m_wndPropertyGrid.AddFloatVar(pGlobals, _T("环境遮罩最高高度"), GROUNDAMBMAX, &(m_curKey.fGroundAmbMax), 0, 500, _T("控制环境遮罩的最高高度"));

	m_wndPropertyGrid.AddFloatVar(pGlobals, _T("LDR亮度"), LDRFAKEBRIGHTNESS, &(m_curKey.fLdrFakeBrightness), 0, 100, _T("控制LDR时的场景亮度"));

	//////////////////////////////////////////////////////////////////////////
	// 太阳
	CXTPPropertyGridItem* pCategorySun = m_wndPropertyGrid.AddCategory(_T("阳光调节"));

	m_wndPropertyGrid.AddColorVar(pCategorySun, _T("阳光颜色") , CLSUNLIGHT, &(m_curKey.clSunLight.r),  _T("太阳主光颜色"  ));
	m_wndPropertyGrid.AddFloatVar(pCategorySun, _T("阳光增益"), SUNINTENSITY, &(m_curKey.fSunIntensity), 0, 15, _T("控制阳光的强度"));
	m_wndPropertyGrid.AddFloatVar(pCategorySun, _T("阳光高光增益"), SUNSPECINTENSITY, &(m_curKey.fSunSpecIntensity), 0, 15, _T("控制高光的强度"));
	m_wndPropertyGrid.AddFloatVar(pCategorySun, _T("光线强度"), SUNSHAFTVISIBILITY, &(m_curKey.fSunShaftVisibility), 0, 100, _T("控制上帝之光的强度"));
	m_wndPropertyGrid.AddFloatVar(pCategorySun, _T("光线衰减"), SUNSHAFTATTEN, &(m_curKey.fSunShaftAtten), 0, 10, _T("控制上帝之光的衰减度"));
	m_wndPropertyGrid.AddFloatVar(pCategorySun, _T("光线长度"), SUNSHAFTLENGTH, &(m_curKey.fSunShaftLength), 0, 2, _T("控制上帝之光的长度"));
	//AddColorVar(pGlobals, _T("阳光泽色") , CLGROUNDAMB, _T("地面初始颜色，和天光配合模拟环境遮罩"  ));
	//////////////////////////////////////////////////////////////////////////
	// 天空盒
	CXTPPropertyGridItem* pCategorySky = m_wndPropertyGrid.AddCategory(_T("天空调节"));
	m_wndPropertyGrid.AddColorVar(pCategorySky, _T("天际线颜色") , CLZENITHBOTTOM,&(m_curKey.clZenithBottom.r),  _T("天际线色"  ));
	m_wndPropertyGrid.AddColorVar(pCategorySky, _T("天空顶颜色") , CLZENITHTOP,&(m_curKey.clZenithTop.r),  _T("天空穹顶色"  ));
	m_wndPropertyGrid.AddFloatVar(pCategorySky, _T("天际/天空系数"), ZENITHSHIFT, &(m_curKey.fZenithShift), 0, 1, _T("控制阳光的强度"));

	//////////////////////////////////////////////////////////////////////////
	// 雾效
	CXTPPropertyGridItem* pCategoryFog = m_wndPropertyGrid.AddCategory(_T("雾调节"));
	m_wndPropertyGrid.AddColorVar(pCategoryFog, _T("雾颜色") , CLFOGCOLOR,&(m_curKey.clFogColor.r),  _T("雾气颜色"  ) , this );
	m_wndPropertyGrid.AddFloatVar(pCategoryFog, _T("雾色倍增"), CLFOGCOLOR3, &(m_curKey.clFogColor.a), 0, 15, _T("雾颜色的强度"));
	m_wndPropertyGrid.AddFloatVar(pCategoryFog, _T("雾气浓度"), FOGDENSITY, &(m_curKey.clFogDensity), 0, 0.1f, _T("雾颜色的强度"));
	m_wndPropertyGrid.AddFloatVar(pCategoryFog, _T("雾气开始距离"), FOGSTART, &(m_curKey.clFogStart), 0, 0.2f, _T("雾颜色的强度"));

	//////////////////////////////////////////////////////////////////////////
	// HDR
	CXTPPropertyGridItem* pCategoryHDR = m_wndPropertyGrid.AddCategory(_T("HDR调节"));

	m_wndPropertyGrid.AddFloatVar(pCategoryHDR, _T("辉光强度"), BLOOMINTENSITY, &(m_curKey.fBloomIntensity), 0, 5, _T("控制HDR亮部辉光逸色强度"));
	m_wndPropertyGrid.AddColorVar(pCategoryHDR, _T("辉光颜色") , CLBLOOMCOLOR,&(m_curKey.clBloomColor.r),  _T("控制HDR亮部辉光逸色偏移"  ));

	m_wndPropertyGrid.AddFloatVar(pCategoryHDR, _T("光晕强度"), STREAKINTENSITY, &(m_curKey.fStreakIntensity), 0, 5, _T("控制HDR亮部光晕强度"));
	m_wndPropertyGrid.AddColorVar(pCategoryHDR, _T("光晕颜色") , CLSTREAKCOLOR,&(m_curKey.clStreakColor.r),  _T("控制HDR亮部光晕色彩"  ));

	//////////////////////////////////////////////////////////////////////////
	// 色彩校正相关
	CXTPPropertyGridItem* pCategoryColorCorection = m_wndPropertyGrid.AddCategory(_T("色彩校正"));
	m_wndPropertyGrid.AddFloatVar(pCategoryColorCorection, _T("饱和度"), SATURATE, &(m_curKey.fSaturate), 0, 2, _T("控制后期调校的饱和度"));
	m_wndPropertyGrid.AddFloatVar(pCategoryColorCorection, _T("对比度"), CONSTRACT, &(m_curKey.fConstract), 0, 2, _T("控制后期调校的对比度"));
	m_wndPropertyGrid.AddFloatVar(pCategoryColorCorection, _T("亮度"), BRIGHTNESS, &(m_curKey.fBrightness), 0, 5, _T("控制后期调校的亮度"));

	m_wndPropertyGrid.AddFloatVar(pCategoryColorCorection, _T("色阶暗部"), LEVELINPUTMIN, &(m_curKey.fLevelInputMin), 0, 255, _T("控制后期调校的色阶"));
	m_wndPropertyGrid.AddFloatVar(pCategoryColorCorection, _T("色阶灰部"), LEVEL, &(m_curKey.fLevel), 0, 10, _T("控制后期调校的色阶"));
	m_wndPropertyGrid.AddFloatVar(pCategoryColorCorection, _T("色阶亮部"), LEVELINPUTMAX, &(m_curKey.fLeveInputMax), 0, 255, _T("控制后期调校的色阶"));

	//////////////////////////////////////////////////////////////////////////
	// 景深相关
	CXTPPropertyGridItem* pCategoryColorDOF = m_wndPropertyGrid.AddCategory(_T("景深"));
	m_wndPropertyGrid.AddFloatVar(pCategoryColorDOF, _T("景深模糊"), DOFBLURINTENSITY, &(m_curKey.fDofBlurIntensity), 0, 1, _T("控制景深的模糊程度"));
	m_wndPropertyGrid.AddFloatVar(pCategoryColorDOF, _T("景深距离-远"), DOFDISTANCE, &(m_curKey.fDofDistance), 0, 1000, _T("控制景深的产生距离"));
	m_wndPropertyGrid.AddFloatVar(pCategoryColorDOF, _T("景深距离-近"), DOFDISTANCENEAR, &(m_curKey.fDofDistanceNear), 0, 1000, _T("控制景深的产生距离"));
	m_wndPropertyGrid.AddFloatVar(pCategoryColorDOF, _T("焦距"), DOFFOCUSPLANE, &(m_curKey.fDofFocusPlane), 0, 1000, _T("控制景深的产生距离"));
	m_wndPropertyGrid.AddFloatVar(pCategoryColorDOF, _T("焦半径"), DOFFOCUSDISTANCE, &(m_curKey.fDofFocusDistance), 0, 1000, _T("控制景深的产生距离"));
	
	
	// create custom items category.
// 	CXTPPropertyGridItem* pCustom   = m_wndPropertyGrid.AddCategory(_T("Custom Items"));
// 
// 	// add child items to category.
// 	pCustom->AddChildItem(new CCustomItemColor(_T("CustomCombolist"), RGB(0xFF, 0x80, 0x40)));
// 
// 	pCustom->AddChildItem(new CCustomItemFileBox(_T("File Box")));
// 
// 	CXTPPropertyGridItem* pItemMaskEdit = pCustom->AddChildItem(new CXTPPropertyGridItem(_T("Mask Edit"), _T("Phone No: (816) 220-0000")));
// 	pItemMaskEdit->SetMask(_T("Phone No: (000) 000-0000"), _T("Phone No: (___) ___-____"));
// 
// 
// 	CXTPPropertyGridItem* pItemPassword = pCustom->AddChildItem(new CXTPPropertyGridItem(_T("Password"), _T("Text")));
// 	pItemPassword->SetPasswordMask();
// 
// 	// add multi level tree node.
// 	CXTPPropertyGridItem* pCategoryOne    = pCustom->AddChildItem(new CXTPPropertyGridItemCategory(_T("First Sub Category")));
// 	CXTPPropertyGridItem* pCategoryTwo    = pCategoryOne->AddChildItem(new CXTPPropertyGridItemCategory(_T("Second Sub Category 1")));
// 	pCategoryTwo->AddChildItem(new CXTPPropertyGridItem(_T("Third Level 1"), _T("")));
// 	pCategoryTwo->AddChildItem(new CXTPPropertyGridItem(_T("Third Level 2"), _T("")));
// 	CXTPPropertyGridItem* pCategoryTwo2   = pCategoryOne->AddChildItem(new CXTPPropertyGridItemCategory(_T("Second Sub Category 2")));
// 	pCategoryTwo2->AddChildItem(new CXTPPropertyGridItem(_T("Third Level 1"), _T("")));
// 
// 	CXTPPropertyGridItem* pItemOne    = pCustom->AddChildItem(new CXTPPropertyGridItem(_T("First Leve_T("), _T(")")));
// 	CXTPPropertyGridItem* pItemTwo    = pItemOne->AddChildItem(new CXTPPropertyGridItem(_T("Second Leve_T("), _T(")")));
// 	CXTPPropertyGridItem* pItemThird     = pItemTwo->AddChildItem(new CXTPPropertyGridItem(_T("Third Leve_T("), _T(")")));
// 	pItemThird->AddChildItem(new CXTPPropertyGridItem(_T("Fourth Level 1"), _T("")));
// 	pItemThird->AddChildItem(new CXTPPropertyGridItem(_T("Fourth Level 2"), _T("")));
// 
// 
// 	// create custom items category.
// 	pCustom   = m_wndPropertyGrid.AddCategory(_T("Custom Buttons"));
// 
// 	CXTPPropertyGridItem* pItemSpin = pCustom->AddChildItem(new CCustomItemSpin(_T("SpinButton")));
// 	pItemSpin->SetDescription(_T("This sample shows how to add new button type"));
// 
// 	pCustom->AddChildItem(new CCustomItemSlider(_T("Slider")));
// 
// 	CCustomItemCheckBox* pItemCheckBox = (CCustomItemCheckBox*)pCustom->AddChildItem(new CCustomItemCheckBox(_T("Check Box")));
// 	pItemCheckBox->SetValue(_T("agree with conditions"));
// 	pItemCheckBox->SetBool(TRUE);
// 
// 	pCustom->AddChildItem(new CCustomItemButton(_T("Left Origin"), FALSE, TRUE));
// 	pCustom->AddChildItem(new CCustomItemButton(_T("Right Origin"), FALSE, TRUE));
// 	pCustom->AddChildItem(new CCustomItemButton(_T("Pointer"), TRUE, TRUE));
// 	pCustom->AddChildItem(new CCustomItemButton(_T("Gradient"), TRUE, FALSE));


}

SetResize(IDC_TODPROPERTY, SZ_TOP_LEFT, SZ_BOTTOM_RIGHT);

return TRUE;
}

LRESULT gkPaneTimeOfDay::OnGridNotify(WPARAM wParam, LPARAM lParam)
{
	if (wParam == XTP_PGN_INPLACEBUTTONDOWN)
	{
		CXTPPropertyGridInplaceButton* pButton = (CXTPPropertyGridInplaceButton*)lParam;
		if (pButton->GetItem()->GetID() == 510 && pButton->GetID() == XTP_ID_PROPERTYGRID_EXPANDBUTTON) // 2 Buttons
		{
			AfxMessageBox(_T("Expand button"));
			return TRUE;
		}
	}

	if (wParam == XTP_PGN_SORTORDER_CHANGED)
	{
		//m_nSort = m_wndPropertyGrid.GetPropertySort();
		UpdateData(FALSE);

		TRACE(_T("SortOrder Changed.\n"));
	}
	if (wParam == XTP_PGN_ITEMVALUE_CHANGED)
	{
		CXTPPropertyGridItem* pItem = (CXTPPropertyGridItem*)lParam;
		//TRACE(_T("Value Changed. Caption = %s, ID = %i, Value = %s\n"), pItem->GetCaption(), pItem->GetID(), pItem->GetValue());

		if (DYNAMIC_DOWNCAST(CXTPPropertyGridItemEnum, pItem))
		{
			if (pItem->GetMetrics(TRUE, FALSE))
			{
				pItem->GetMetrics(TRUE, FALSE)->m_nImage = ((CXTPPropertyGridItemEnum*)pItem)->GetEnum();
			}
		}

		if( pItem->GetID() >= HDRBRIGHTNESS && pItem->GetID() < VAR_END )
		{
			//gEnv->p3DEngine->getTimeOfDay()->setPreviewTODKey(m_curKey);
			UpdateTODKeyToEngine();
		}
	}
	if (wParam == XTP_PGN_SELECTION_CHANGED)
	{
		CXTPPropertyGridItem* pItem = (CXTPPropertyGridItem*)lParam;
		if (pItem)
		{
			if (pItem->IsKindOf(RUNTIME_CLASS(CXTPPropertyGridItemColor)))
			{
				TRACE(_T("Color Item. Selection Changed. Item = %s\n"), pItem->GetCaption());
			}
		}
	}
	if (wParam == XTP_PGN_EDIT_CHANGED)
	{
		CXTPPropertyGridInplaceEdit* pEdit = DYNAMIC_DOWNCAST(CXTPPropertyGridInplaceEdit, (CWnd*)lParam);
		if (pEdit && pEdit->GetItem())
		{
			// Custom Validation
			// Custom Validation
			if (pEdit->GetItem()->GetCaption() == _T("ItemsInMRUList"))
			{
				CString str;
				pEdit->CEdit::GetWindowText(str);

				int i = _ttoi(str);
				if (i > 20)
				{
					MessageBeep((UINT)-1);
					pEdit->SetSel(0, -1);
					pEdit->ReplaceSel(_T("20"));
				}
			}
		}
	}
	return 0;
}


// gkPaneTimeOfDay message handlers

void gkPaneTimeOfDay::OnNMCustomdrawTodslider(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

	static int nPrevPos;
	int nPos = m_ctrlSlider.GetPos();

	if (nPrevPos != nPos)
	{
		gEnv->p3DEngine->getTimeOfDay()->setTime( (float)nPos / 10.0f);
		UpdateTODKeyFromEngine();
	}

	nPrevPos = nPos;

}

void gkPaneTimeOfDay::UpdateTODKeyFromEngine()
{
	m_curKey = gEnv->p3DEngine->getTimeOfDay()->getCurrentTODKeyPure();
	m_wndPropertyGrid.RefreshAllItem();
}

void gkPaneTimeOfDay::UpdateTODKeyToEngine()
{
	gEnv->p3DEngine->getTimeOfDay()->setPreviewTODKey(m_curKey);
}

void gkPaneTimeOfDay::UpdateTodSlider()
{
	STimeOfDayKey tempKey = gEnv->p3DEngine->getTimeOfDay()->getCurrentTODKey();
	m_ctrlSlider.SetPos((int)(tempKey.fKeyTime * 10.0f));

	m_ctrlSlider.ClearTics();
	TODKeySequence& seq = gEnv->p3DEngine->getTimeOfDay()->getTODKeys();
	TODKeySequence::iterator it = seq.begin();
	for(; it != seq.end(); ++it)
	{
		m_ctrlSlider.SetTic( (int)(it->fKeyTime * 10.0) );
	}
}

void gkPaneTimeOfDay::OnBnClickedTodPrevkey()
{
	// TODO: Add your control notification handler code here
	m_curKey = gEnv->p3DEngine->getTimeOfDay()->getPrevTODKey();
	gEnv->p3DEngine->getTimeOfDay()->setTime( m_curKey.fKeyTime );
	UpdateTodSlider();
	//UpdateTODKeyFromEngine();

}

void gkPaneTimeOfDay::OnBnClickedTodNextkey()
{
	// TODO: Add your control notification handler code here
	m_curKey = gEnv->p3DEngine->getTimeOfDay()->getNextTODKey();
	gEnv->p3DEngine->getTimeOfDay()->setTime( m_curKey.fKeyTime );
	UpdateTodSlider();
	//UpdateTODKeyFromEngine();
}

void gkPaneTimeOfDay::OnBnClickedTodPlantkey()
{
	// TODO: Add your control notification handler code here
	// ensure heres not exist a key

	gEnv->p3DEngine->getTimeOfDay()->removeTODKey(m_curKey.fKeyTime);
	// plant
	gEnv->p3DEngine->getTimeOfDay()->setTODKey(m_curKey);
	UpdateTodSlider();
}

void gkPaneTimeOfDay::OnBnClickedTodDeletekey()
{
	// TODO: Add your control notification handler code here
	gEnv->p3DEngine->getTimeOfDay()->removeTODKey(m_curKey.fKeyTime);
	UpdateTodSlider();
}

void gkPaneTimeOfDay::OnCommandRange( UINT ID)
{
	switch( ID)
	{
	case ID_BUTTON_TODEDT_PLAY:
			gEnv->p3DEngine->getTimeOfDay()->setSpeed(0.6f);
			m_bPlaying = true;
		break;
	case ID_BUTTON_TODEDT_FORWARD:
			gEnv->p3DEngine->getTimeOfDay()->setSpeed(3.0f);
			m_bPlaying = true;
		break;
	case ID_BUTTON_TODEDT_STOP:
		gEnv->p3DEngine->getTimeOfDay()->setSpeed(0.0f);
		m_bPlaying = false;
		break;
	case ID_BUTTON_TODEDT_NEXTKEY:
		OnBnClickedTodNextkey();
		break;
	case ID_BUTTON_TODEDT_PREVKEY:
		OnBnClickedTodPrevkey();
		break;
	case ID_BUTTON_TODEDT_PLANTKEY:
		OnBnClickedTodPlantkey();
		break;
	case ID_BUTTON_TODEDT_DELKEY:
		OnBnClickedTodDeletekey();
		break;
	case ID_BUTTON_TODEDT_CPYKEY:
		TodCopyKey();
		break;
	case ID_BUTTON_TODEDT_PSTKEY:
		TodPasteKey();
		break;
	}
}

void gkPaneTimeOfDay::OnUpdate( float fElapsedTime )
{
	if (m_bPlaying)
	{
		UpdateTodSlider();
	}
	
}

void gkPaneTimeOfDay::TodCopyKey()
{
	m_copyKey = m_curKey;
}

void gkPaneTimeOfDay::TodPasteKey()
{
	m_copyKey.fKeyTime = m_curKey.fKeyTime;
	gEnv->p3DEngine->getTimeOfDay()->removeTODKey(m_copyKey.fKeyTime);
	// plant
	gEnv->p3DEngine->getTimeOfDay()->setTODKey(m_copyKey);
	UpdateTodSlider();
}

void gkPaneTimeOfDay::OnColorChange( COLORREF clr )
{
	UpdateTODKeyToEngine();
	gEnv->pSystem->Update();
	gEnv->pSystem->PostUpdate();
}
