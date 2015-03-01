//////////////////////////////////////////////////////////////////////////
//
// K&K Studio GameKnife ENGINE Source File
//
// Name:   	gkPropertyGirdEx.cpp
// Desc:		
// 	
// Author:  Kaiming
// Date:	2012/5/14
// Modify:	2012/5/14
// 
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "gkPropertyGirdEx.h"
#include "CustomItems.h"
#include "..\resource.h"

#define PROPERTY_ICON_INT 0
#define PROPERTY_ICON_COLOR 1
#define PROPERTY_ICON_BOOL 2
#define PROPERTY_ICON_STRING 3
#define PROPERTY_ICON_MATERIAL 4
#define PROPERTY_ICON_FLOAT 5
#define PROPERTY_ICON_SOUND 6
#define PROPERTY_ICON_FILE 7

typedef void (CXTPPropertyGridItem::*ITEMFUNCTIONPTR)();

void _DoCollapseExpand(CXTPPropertyGridItems* pItems, ITEMFUNCTIONPTR pFunction)
{
	for (int i = 0; i < pItems->GetCount(); i++)
	{
		CXTPPropertyGridItem* pItem = pItems->GetAt(i);
		if (pItem->HasChilds())
		{
			(pItem->*pFunction)();
			_DoCollapseExpand(pItem->GetChilds(), pFunction);
		}
	}
}

IMPLEMENT_DYNAMIC(gkPropertyGirdEx, CXTPPropertyGrid)

	BEGIN_MESSAGE_MAP(gkPropertyGirdEx, CXTPPropertyGrid)
		ON_WM_SYSCOLORCHANGE()
	END_MESSAGE_MAP()
	

//////////////////////////////////////////////////////////////////////////
CCustomItemSlider* gkPropertyGirdEx::AddFloatVar( CXTPPropertyGridItem* cate, LPCTSTR lpszName, UINT id, float* val, float minval /*= 0*/, float maxval /*= 100*/, LPCTSTR lpszDescription /*= _T("描述")*/ )
{
	if (!cate)
	{
		cate = AddCategory(_T("Default"));
	}
	CCustomItemSlider* pItem = (CCustomItemSlider*)(cate->AddChildItem(new CCustomItemSlider(lpszName, minval, maxval)));
	
	pItem->SetID(id);
	pItem->SetDescription(lpszDescription);
	pItem->SetDouble(*val);
	pItem->BindToFloat(val);
	pItem->GetCaptionMetrics()->m_nImage = PROPERTY_ICON_FLOAT;

	return pItem;
}
//////////////////////////////////////////////////////////////////////////
CCustomItemSlider* gkPropertyGirdEx::AddDoubleVar( CXTPPropertyGridItem* cate, LPCTSTR lpszName, UINT id, double* val, float minval /*= 0*/, float maxval /*= 100*/, LPCTSTR lpszDescription /*= _T("描述")*/ )
{
	if (!cate)
	{
		cate = AddCategory(_T("Default"));
	}
	CCustomItemSlider* pItem = (CCustomItemSlider*)(cate->AddChildItem(new CCustomItemSlider(lpszName, minval, maxval)));

	pItem->SetID(id);
	pItem->SetDescription(lpszDescription);
	pItem->SetDouble(*val);
	pItem->BindToDouble(val);
	pItem->GetCaptionMetrics()->m_nImage = PROPERTY_ICON_FLOAT;

	return pItem;
}

//////////////////////////////////////////////////////////////////////////
CXTPPropertyGridItemBool* gkPropertyGirdEx::AddBoolVar( CXTPPropertyGridItem* cate, LPCTSTR lpszName, UINT id, bool* val, LPCTSTR lpszDescription /*= _T("描述")*/ )
{
	if (!cate)
	{
		cate = AddCategory(_T("Default"));
	}
	CXTPPropertyGridItemBool* pItem = (CXTPPropertyGridItemBool*)(cate->AddChildItem(new CXTPPropertyGridItemBool(lpszName)));
	pItem->SetID(id);
	pItem->SetDescription(lpszDescription);
	//pItem->AddSliderControl();
	pItem->SetBool(*val);
	pItem->BindToBool((BOOL*)val);
	pItem->GetCaptionMetrics()->m_nImage = PROPERTY_ICON_BOOL;

	return pItem;
}


//////////////////////////////////////////////////////////////////////////
CXTPPropertyGridItemNumber* gkPropertyGirdEx::AddIntegerVar( CXTPPropertyGridItem* cate, LPCTSTR lpszName, UINT id, int* val, LPCTSTR lpszDescription /*= _T("描述")*/ )
{
	if (!cate)
	{
		cate = AddCategory(_T("Default"));
	}
	CXTPPropertyGridItemNumber* pItem = (CXTPPropertyGridItemNumber*)(cate->AddChildItem(new CXTPPropertyGridItemNumber(lpszName)));
	pItem->SetID(id);
	pItem->SetDescription(lpszDescription);
	pItem->AddSliderControl();
	pItem->SetNumber(*val);
	pItem->BindToNumber((long*)val);
	pItem->GetCaptionMetrics()->m_nImage = PROPERTY_ICON_INT;

	return pItem;
}
//////////////////////////////////////////////////////////////////////////
CCustomItemColor* gkPropertyGirdEx::AddColorVar( CXTPPropertyGridItem* cate, LPCTSTR lpszName, UINT id, float* val, LPCTSTR lpszDescription /*= _T("描述")*/, IPSCPColorChangeCallback* callback )
{
	if (!cate)
	{
		cate = AddCategory(_T("Default"));
	}
	CCustomItemColor* pColor = (CCustomItemColor*)(cate->AddChildItem(new CCustomItemColor(lpszName, callback, RGB(127, 127, 127))));
	pColor->SetDescription(lpszDescription);
	pColor->SetID(id);
	pColor->SetColor(val);
	pColor->BindToColorF(val);
	pColor->GetCaptionMetrics()->m_nImage = PROPERTY_ICON_COLOR;

	return pColor;
}

CCustomItemFilename* gkPropertyGirdEx::AddFileVar( CXTPPropertyGridItem* cate, LPCTSTR lpszName, LPCTSTR lpszValue,  LPCTSTR lpszDescription /*= _T("描述")*/ )
{
	if (!cate)
	{
		cate = AddCategory(_T("Default"));
	}
	CCustomItemFilename* pItem = (CCustomItemFilename*)(cate->AddChildItem(new CCustomItemFilename(lpszName)));
	//pItem->SetID(id);
	pItem->SetDescription(lpszDescription);
	pItem->SetValue(lpszValue);
	//pItem->AddSliderControl();
	//pItem->SetNumber(*val);
	//pItem->BindToNumber((long*)val);
	pItem->GetCaptionMetrics()->m_nImage = PROPERTY_ICON_FILE;

	return pItem;
}

//////////////////////////////////////////////////////////////////////////
void gkPropertyGirdEx::AddStringVar( CXTPPropertyGridItem* cate, LPCTSTR lpszName, LPCTSTR lpszValue, LPCTSTR lpszDescription )
{
	if (!cate)
	{
		cate = AddCategory(_T("Default"));
	}
	CXTPPropertyGridItem* pItem = cate->AddChildItem(new CXTPPropertyGridItem(lpszName));
	//pItem->SetID(id);
	pItem->SetDescription(lpszDescription);
	pItem->SetValue(lpszValue);
	//pItem->AddSliderControl();
	//pItem->SetNumber(*val);
	//pItem->BindToNumber((long*)val);
	pItem->GetCaptionMetrics()->m_nImage = PROPERTY_ICON_STRING;

	//return pItem;
}
//////////////////////////////////////////////////////////////////////////
BOOL gkPropertyGirdEx::Create( const RECT& rect, CWnd* pParentWnd, UINT nID, DWORD dwListStyle /*= 0 */ )
{
	CXTPPropertyGrid::Create(rect, pParentWnd, nID, dwListStyle);

	GetImageManager()->SetMaskColor(0xC0C0C0);
	GetImageManager()->SetIcons(IDB_GRIDPROPERTY, 0, 9, CSize(14, 14));

	// modify the theme
	//SetTheme(xtpGridThemeDefault);

	SetCustomTheme(new CXTPPropertyGridVisualStudio2010ThemeCustom(this));

	//SetCustomColors( RGB(255,255,255), RGB(30,72,108), RGB(190,199,210), RGB(255,255,255), RGB(0,0,0), RGB(30,72,108) );

	SetCustomColors( XTPColorManager()->GetColor(COLOR_WINDOW), 
		XTPColorManager()->GetColor(COLOR_BTNTEXT), 
		XTPColorManager()->GetColor(COLOR_BTNFACE), 
		XTPColorManager()->GetColor(COLOR_WINDOW), 
		XTPColorManager()->GetColor(COLOR_BTNTEXT), 
		XTPColorManager()->GetColor(COLOR_BTNTEXT)  );

	SetViewDivider( 0.4 );
	//SetViewDividerPos( 0.3 );

	//SetVariableSplitterPos( 0 );
	//SetVariableSplitterPos(  )

	//GetXtremeColor( x)

	//SetCustomColors( GetSysColor( ) )

	return TRUE;
}
//////////////////////////////////////////////////////////////////////////
void gkPropertyGirdEx::RefreshAllItem()
{
	RefeshBindings();
}

//////////////////////////////////////////////////////////////////////////
void gkPropertyGirdEx::ExpandAll()
{
	_DoCollapseExpand(GetCategories(), &CXTPPropertyGridItem::Expand);
}
//////////////////////////////////////////////////////////////////////////
void gkPropertyGirdEx::CollapseAll()
{
	_DoCollapseExpand(GetCategories(), &CXTPPropertyGridItem::Collapse);
}

void gkPropertyGirdEx::OnSysColorChange()
{
	CXTPPropertyGrid::OnSysColorChange();

	SetCustomColors( XTPColorManager()->GetColor(COLOR_WINDOW), 
		XTPColorManager()->GetColor(COLOR_BTNTEXT), 
		XTPColorManager()->GetColor(COLOR_BTNFACE), 
		XTPColorManager()->GetColor(COLOR_WINDOW), 
		XTPColorManager()->GetColor(COLOR_BTNTEXT), 
		XTPColorManager()->GetColor(COLOR_BTNTEXT)  );
}


void CXTPPropertyGridVisualStudio2010ThemeCustom::RefreshMetrics()
{
	CXTPPropertyGridPaintManager::RefreshMetrics();

	m_pMetrics->m_clrLine.SetStandardValue(RGB(240, 240, 240));
	m_pMetrics->m_clrCategoryFore.SetStandardValue(GetSysColor(COLOR_3DSHADOW));
	m_clrShadow.SetStandardValue(GetSysColor(COLOR_3DSHADOW));
	m_clrCategorySeparator.SetStandardValue(GetSysColor(COLOR_3DSHADOW));
	m_clrFace.SetStandardValue(RGB(222, 225, 231));
	m_pMetrics->m_clrHelpBack.SetStandardValue(RGB(222, 225, 231));
	m_pMetrics->m_clrHelpFore.SetStandardValue(0);
}
