// GalleryItems.cpp: implementation of the CGalleryItemStyleSet class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GalleryItems.h"

#ifdef _DEBUG
#undef THIS_FILE 
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// CGalleryItemStyleSet

CGalleryItemStyleSet::CGalleryItemStyleSet(int nId, LPCTSTR lpszCaption)
{
	m_strCaption = lpszCaption;	
	m_nId = nId;
}

CGalleryItemStyleSet::~CGalleryItemStyleSet()
{

}

void CGalleryItemStyleSet::Draw(CDC* pDC, CXTPControlGallery* pGallery, CRect rcItem, BOOL bEnabled, BOOL bSelected, BOOL bPressed, BOOL bChecked)
{
	CXTPPaintManager* pPaintManager = pGallery->GetPaintManager();

	COLORREF clrTextColor = pPaintManager->GetRectangleTextColor(bSelected, bPressed, bEnabled, FALSE, FALSE, xtpBarTypePopup, xtpBarPopup);

	pDC->SetTextColor(clrTextColor);
	CXTPFontDC font(pDC, pPaintManager->GetRegularFont());

	int cx = pPaintManager->GetPopupBarGripperWidth(pGallery->GetParent()) - 1;
	pPaintManager->DrawPopupBarGripper(pDC, rcItem.left - 1, rcItem.top, cx + 1, rcItem.Height());

	pPaintManager->DrawRectangle(pDC, rcItem, bSelected, bPressed, bEnabled, FALSE, FALSE, xtpBarTypePopup, xtpBarPopup);

	if (bChecked && bEnabled)
	{
		CRect rcCheck(rcItem.left + 1, rcItem.top + 1, rcItem.left + cx - 2, rcItem.bottom - 1);
		pPaintManager->DrawRectangle(pDC, rcCheck, bSelected, bPressed, bEnabled, TRUE, FALSE, xtpBarTypePopup, xtpBarPopup);
		
		pPaintManager->DrawCheckMark(pDC, rcCheck);
	}

	CRect rcText(rcItem);
	rcText.left += pPaintManager->m_nPopupBarTextPadding + cx;

	pDC->DrawText(m_strCaption, rcText, DT_END_ELLIPSIS|DT_SINGLELINE|DT_VCENTER);
}

//////////////////////////////////////////////////////////////////////////
// CGalleryItemThemeColors


CGalleryItemThemeColors::CGalleryItemThemeColors(int nId, LPCTSTR lpszCaption)
{
	m_strToolTip = m_strCaption = lpszCaption;	
	m_nId = nId;
}

const struct COLORPREVIEWREC
{
	COLORREF clr;
	LPCTSTR lpszCaption;
};

const struct COLORPREVIEW
{
	COLORPREVIEWREC rec[10];
} 
themeColors[] = 
{
	{//Office
		RGB(255, 255, 255), _T("White"), RGB(0, 0, 0), _T("Black"),
		RGB(238, 236, 225), _T("Tan"), RGB(31, 73, 125), _T("Dark Blue"), RGB(79, 129, 189), _T("Blue"), RGB(192, 80, 77), _T("Red"),
		RGB(155, 187, 89), _T("Olive Green"), RGB(128, 100, 162), _T("Purple"), RGB(75, 172, 198), _T("Aqua"), RGB(247, 150, 70), _T("Orange")
	},
	{//Grayscale
		RGB(255, 255, 255), _T("White"), RGB(0, 0, 0), _T("Black"),
		RGB(248, 248, 248), NULL, RGB(0, 0, 0), NULL, RGB(221, 221, 221), NULL, RGB(178, 178, 178), NULL, 
		RGB(150, 150, 150), NULL,  RGB(128, 128, 128), NULL, RGB(95, 95, 95), NULL,  RGB(77, 77, 77), NULL
	},
	{//Apex
		RGB(255, 255, 255), _T("White"), RGB(0, 0, 0), _T("Black"),
		RGB(201, 194, 209), NULL, RGB(105, 103, 109), NULL, RGB(206, 185, 102), NULL, RGB(156, 176, 132), NULL, 
		RGB(107, 177, 201), NULL, RGB(101, 133, 207),NULL,  RGB(126, 107, 201), NULL, RGB(163, 121, 187), NULL
	},
	{//Aspect
		RGB(255, 255, 255), _T("White"), RGB(0, 0, 0), _T("Black"),
		RGB(227, 222, 209), NULL, RGB(50, 50, 50), NULL, RGB(240, 127, 9), NULL, RGB(159, 41, 54), NULL,
		RGB(27, 88, 124), NULL, RGB(78, 133, 66), NULL, RGB(96, 72, 120), NULL, RGB(193, 152, 89), NULL
	},
	{//Civic
		RGB(255, 255, 255), _T("White"), RGB(0, 0, 0), _T("Black"),
		RGB(197, 209, 215), NULL, RGB(100, 107, 134), NULL, RGB(209, 99, 73), NULL, RGB(204, 180, 0), NULL,
		RGB(140, 173, 174), NULL, RGB(140, 123, 112), NULL, RGB(143, 176, 140), NULL, RGB(209, 144, 73)
	},
	{//Concourse
		RGB(255, 255, 255), _T("White"), RGB(0, 0, 0), _T("Black"),
		RGB(222, 245, 250), NULL, RGB(70, 70, 70), NULL, RGB(045, 162, 191), NULL, RGB(218, 31, 40), NULL,
		RGB(235, 100, 27), NULL, RGB(57, 99, 157), NULL, RGB(71, 75, 120), NULL, RGB(125, 60, 74)
	},
	{//Currency
		RGB(255, 255, 255), _T("White"), RGB(0, 0, 0), _T("Black"),
		RGB(209, 225, 227), NULL, RGB(74, 96, 110), NULL, RGB(121, 181, 176), NULL, RGB(180, 188, 76), NULL,
		RGB(183, 120, 81), NULL, RGB(119, 106, 91), NULL, RGB(182, 173, 118), NULL, RGB(149, 174, 177)
	},
	{//Deluxe
		RGB(255, 255, 255), _T("White"), RGB(0, 0, 0), _T("Black"),
		RGB(255, 249, 229), NULL, RGB(48, 53, 110), NULL, RGB(204, 71, 87), NULL, RGB(255, 111, 97), NULL,
		RGB(255, 149, 62), NULL, RGB(248, 189, 82), NULL, RGB(70, 166, 189), NULL, RGB(84, 136, 188)
	}

};

void CGalleryItemThemeColors::Draw(CDC* pDC, CXTPControlGallery* pGallery, CRect rcItem, BOOL bEnabled, BOOL bSelected, BOOL bPressed, BOOL bChecked)
{
	CXTPPaintManager* pPaintManager = pGallery->GetPaintManager();

	COLORREF clrTextColor = pPaintManager->GetRectangleTextColor(bSelected, bPressed, bEnabled, FALSE, FALSE, xtpBarTypePopup, xtpBarPopup);

	pDC->SetTextColor(clrTextColor);
	CXTPFontDC font(pDC, pPaintManager->GetRegularFont());

	pPaintManager->DrawRectangle(pDC, rcItem, bSelected, bPressed, bEnabled, FALSE, FALSE, xtpBarTypePopup, xtpBarPopup);

	CRect rcPreview(rcItem.left + 2, rcItem.top + 2, rcItem.left + 2 + 108, rcItem.bottom - 2);

	if (bChecked && bEnabled)
	{
		pPaintManager->DrawRectangle(pDC, rcPreview, bSelected, bPressed, bEnabled, TRUE, FALSE, xtpBarTypeNormal, xtpBarPopup);
	}

	CRect rcColor(rcPreview);
	rcColor.DeflateRect(2, 2);
	rcColor.right = rcColor.left + rcColor.Height();

	const COLORPREVIEW& clrPreview = themeColors[GetID()];
	int nOrder[] = {3, 2, 4, 5, 6, 7, 8, 9};

	for (int i = 0; i < _countof(nOrder); i++)
	{
		pDC->FillSolidRect(rcColor, clrPreview.rec[nOrder[i]].clr);
		pDC->Draw3dRect(rcColor, RGB(134, 134, 134), RGB(134, 134, 134));
		
		rcColor.OffsetRect(rcColor.Width() + 1, 0);
	}

	CRect rcText(rcItem);
	rcText.left += 120;
	pDC->DrawText(m_strCaption, rcText, DT_END_ELLIPSIS|DT_SINGLELINE|DT_VCENTER);
}

//////////////////////////////////////////////////////////////////////////
// CGalleryItemThemeFonts


const struct FONTPREVIEW
{
	LPCTSTR lpszBody;
	LPCTSTR lpszHeadings;
} 
themeFonts[] = 
{
	{//Office
		_T("Cambria"), _T("Calibri")
	},
	{//Apex
		_T("Aria_T("), _T(")Times New Roman")
	},
	{//Aspect
		_T("Verdana"), _T("Verdana")
	},
	{//Civic
		_T("Georgia"), _T("Georgia")
	},
	{//Concourse
		_T("Eras Medium ITC"), _T("Eras Medium ITC")
	},
	{//Currency
		_T("Constantia"), _T("Constantia")
	},
	{//Deluxe
		_T("Corbe_T("), _T(")Corbel")
	},
	{//Equity
		_T("Franklin Gothic Book"), _T("Perpetua")
	}

};

CGalleryItemThemeFonts::CGalleryItemThemeFonts(int nId, LPCTSTR lpszCaption)
{
	m_strToolTip = m_strCaption = lpszCaption;	
	m_nId = nId;
}


void CGalleryItemThemeFonts::Draw(CDC* pDC, CXTPControlGallery* pGallery, CRect rcItem, BOOL bEnabled, BOOL bSelected, BOOL bPressed, BOOL bChecked)
{
	CXTPPaintManager* pPaintManager = pGallery->GetPaintManager();
	pPaintManager->DrawRectangle(pDC, rcItem, bSelected, bPressed, bEnabled, bChecked, FALSE, xtpBarTypeNormal, xtpBarPopup);

	CRect rcPreview(rcItem.left + 6, rcItem.top + 6, rcItem.left + 6 + 43, rcItem.bottom - 6);
	pDC->Draw3dRect(rcPreview, RGB(97, 108, 129), RGB(97, 108, 129));
	rcPreview.DeflateRect(1, 1);
	pDC->Draw3dRect(rcPreview, RGB(255, 255, 255), RGB(255, 255, 255));
	rcPreview.DeflateRect(1, 1);
	pDC->Draw3dRect(rcPreview, RGB(195, 200, 213), RGB(195, 200, 213));
	rcPreview.DeflateRect(1, 1);
	pDC->FillSolidRect(rcPreview, RGB(243, 243, 244));
	
	pDC->SetTextColor(0);

	const FONTPREVIEW& fontPreview = themeFonts[GetID()];

	CFont fntBody;
	fntBody.CreatePointFont(130, fontPreview.lpszBody);
	
	CFont fntHeadings;
	fntHeadings.CreatePointFont(120, fontPreview.lpszHeadings);

	CRect rcFontPreviewBody(rcPreview.left + 5, rcPreview.top, rcPreview.right, rcPreview.bottom - 5);
	CXTPFontDC fnt(pDC, &fntBody);
	pDC->DrawText(_T("A"), rcFontPreviewBody, DT_BOTTOM | DT_SINGLELINE);

	CRect rcFontPreviewBodyText(rcItem.left + 70, rcItem.top + 17, rcItem.right, rcItem.bottom);
	pDC->DrawText(fontPreview.lpszBody, rcFontPreviewBodyText, DT_TOP | DT_SINGLELINE);

	CRect rcFontPreviewHeadings(rcPreview.left + 20, rcPreview.top, rcPreview.right, rcPreview.bottom - 5);
	fnt.SetFont(&fntHeadings);
	pDC->DrawText(_T("a"), rcFontPreviewHeadings, DT_BOTTOM | DT_SINGLELINE);

	CRect rcFontPreviewHeadingsText(rcItem.left + 70, rcItem.top + 37, rcItem.right, rcItem.bottom);
	pDC->DrawText(fontPreview.lpszHeadings, rcFontPreviewHeadingsText, DT_TOP | DT_SINGLELINE);

}

//////////////////////////////////////////////////////////////////////////
// CGalleryItemTable


CGalleryItemTable::CGalleryItemTable(int nId)
{
	m_nId = nId;
}


void CGalleryItemTable::Draw(CDC* pDC, CXTPControlGallery* pGallery, CRect rcItem, BOOL /*bEnabled*/, BOOL bSelected, BOOL /*bPressed*/, BOOL /*bChecked*/)
{
	int nItem = pGallery->IsItemSelected() ? pGallery->GetSelectedItem() : -1;
	CRect rcItems = pGallery->GetItemsRect();
	CSize szItems = pGallery->GetItems()->GetItemSize();
	int nItemsPerRow = rcItems.Width() / szItems.cx;

	int nColumns = nItem < 1 ? 0 : (nItem - 1) % nItemsPerRow + 1;
	int nRows = nItem < 1 ? 0 : (nItem - 1) / nItemsPerRow + 1;


	int nColumnItem = (GetIndex() - 1) % nItemsPerRow + 1;
	int nRowItem = (GetIndex() - 1) / nItemsPerRow + 1;

	bSelected = nRowItem <= nRows && nColumnItem <= nColumns;
	


	rcItem.DeflateRect(0, 0, 1, 1);

	if (bSelected)
	{
		pDC->Draw3dRect(rcItem, RGB(239, 72, 16), RGB(239, 72, 16));
		rcItem.DeflateRect(1, 1);
		pDC->Draw3dRect(rcItem, RGB(255, 226, 148), RGB(255, 226, 148));
	}
	else
	{
		rcItem.DeflateRect(1, 1);
		pDC->Draw3dRect(rcItem, 0, 0);
	}
	rcItem.DeflateRect(1, 1);
	pDC->FillSolidRect(rcItem, GetXtremeColor(COLOR_WINDOW));

}

CGalleryItemTableLabel::CGalleryItemTableLabel()
{
	m_bLabel = TRUE;
}


void CGalleryItemTableLabel::Draw(CDC* pDC, CXTPControlGallery* pGallery, CRect rcItem, BOOL bEnabled, BOOL bSelected, BOOL bPressed, BOOL bChecked)
{
	int nItem = pGallery->IsItemSelected() ? pGallery->GetSelectedItem() : -1;
	CRect rcItems = pGallery->GetItemsRect();
	CSize szItems = pGallery->GetItems()->GetItemSize();
	int nItemsPerRow = rcItems.Width() / szItems.cx;

	int nColumns = nItem < 1 ? 0 : (nItem - 1) % nItemsPerRow + 1;
	int nRows = nItem < 1 ? 0 : (nItem - 1) / nItemsPerRow + 1;


	if (nItem < 1)
		m_strCaption = _T("Insert Table");
	else
		m_strCaption.Format(_T("%ix%i Table"), nRows, nColumns);

	CXTPControlGalleryItem::Draw(pDC, pGallery, rcItem, bEnabled, bSelected, bPressed, bChecked);
}

//////////////////////////////////////////////////////////////////////////
//

CGalleryItemFontColor::CGalleryItemFontColor(COLORREF clr, int nToolTip)
{
	m_nId = clr;
	m_strToolTip.LoadString(nToolTip);

	m_bTopMargin = m_bBottomMargin = TRUE;
}

CGalleryItemFontColor::CGalleryItemFontColor(COLORREF clr, LPCTSTR lpszCaption, BOOL bTopMargin /*= TRUE*/, BOOL bBotttomMargin /*= TRUE*/)
{
	m_nId = clr;
	m_strCaption = m_strToolTip = lpszCaption;

	m_bTopMargin = bTopMargin;
	m_bBottomMargin = bBotttomMargin;

	SetSize(CSize(17, 13 + (bTopMargin ? 2 : 0) + (bBotttomMargin ? 2 : 0)));
}


void CGalleryItemFontColor::Draw(CDC* pDC, CXTPControlGallery* /*pGallery*/, CRect rcItem, BOOL /*bEnabled*/, BOOL bSelected, BOOL /*bPressed*/, BOOL bChecked)
{
	rcItem.DeflateRect(2, 0);
	if (m_bTopMargin)
		rcItem.DeflateRect(0, 2, 0, 0);
	
	if (m_bBottomMargin)
		rcItem.DeflateRect(0, 0, 0, 2);

	pDC->FillSolidRect(rcItem, (COLORREF)m_nId);

	if (bSelected || bChecked)
	{
		if (bSelected)
			pDC->Draw3dRect(rcItem, RGB(242, 148, 54), RGB(242, 148, 54));
		else
			pDC->Draw3dRect(rcItem, RGB(239, 72, 16), RGB(239, 72, 16));
		
		rcItem.DeflateRect(1, 1);
		pDC->Draw3dRect(rcItem, RGB(255, 226, 148), RGB(255, 226, 148));
	}
	else
	{
		if (m_bTopMargin) pDC->FillSolidRect(rcItem.left, rcItem.top, rcItem.Width(), 1, RGB(197, 197, 197));
		pDC->FillSolidRect(rcItem.left, rcItem.top, 1, rcItem.Height(), RGB(197, 197, 197));
		pDC->FillSolidRect(rcItem.right - 1, rcItem.top, 1, rcItem.Height(), RGB(197, 197, 197));
		if (m_bBottomMargin) pDC->FillSolidRect(rcItem.left, rcItem.bottom - 1, rcItem.Width(), 1, RGB(197, 197, 197));
	}
}

void CGalleryItemFontColor::AddStandardBackColors(CXTPControlGalleryItems* pItems)
{
	pItems->AddItem(new CGalleryItemFontColor(RGB(0xFF, 0xFF, 0x00), XTP_IDS_CLR_YELLOW));
	pItems->AddItem(new CGalleryItemFontColor(RGB(0x00, 0xFF, 0x00), XTP_IDS_CLR_BRIGHT_GREEN));
	pItems->AddItem(new CGalleryItemFontColor(RGB(0x00, 0xFF, 0xFF), XTP_IDS_CLR_TURQUOISE));
	pItems->AddItem(new CGalleryItemFontColor(RGB(0xFF, 0x00, 0xFF), XTP_IDS_CLR_PINK));
	pItems->AddItem(new CGalleryItemFontColor(RGB(0x00, 0x00, 0xFF), XTP_IDS_CLR_BLUE));
	pItems->AddItem(new CGalleryItemFontColor(RGB(0xFF, 0x00, 0x00), XTP_IDS_CLR_RED));
	pItems->AddItem(new CGalleryItemFontColor(RGB(0x00, 0x00, 0x80), XTP_IDS_CLR_DARK_BLUE));
	pItems->AddItem(new CGalleryItemFontColor(RGB(0x00, 0x80, 0x80), XTP_IDS_CLR_TEAL));
	pItems->AddItem(new CGalleryItemFontColor(RGB(0x00, 0x80, 0x00), XTP_IDS_CLR_GREEN));
	pItems->AddItem(new CGalleryItemFontColor(RGB(0x80, 0x00, 0x80), XTP_IDS_CLR_VIOLET));
	pItems->AddItem(new CGalleryItemFontColor(RGB(0x80, 0x00, 0x00), XTP_IDS_CLR_DARK_RED));
	pItems->AddItem(new CGalleryItemFontColor(RGB(0x80, 0x80, 0x00), XTP_IDS_CLR_DARK_YELLOW));
	pItems->AddItem(new CGalleryItemFontColor(RGB(0x80, 0x80, 0x80), XTP_IDS_CLR_GRAY50));
	pItems->AddItem(new CGalleryItemFontColor(RGB(0xC0, 0xC0, 0xC0), XTP_IDS_CLR_GRAY25));
	pItems->AddItem(new CGalleryItemFontColor(RGB(0x00, 0x00, 0x00), XTP_IDS_CLR_BLACK));
}

void CGalleryItemFontColor::AddStandardColors(CXTPControlGalleryItems* pItems)
{
	pItems->AddItem(new CGalleryItemFontColor(RGB(192, 0, 0), _T("Dark Red")));
	pItems->AddItem(new CGalleryItemFontColor(RGB(255, 0, 0), _T("Red")));
	pItems->AddItem(new CGalleryItemFontColor(RGB(255, 192, 0), _T("Orange")));
	pItems->AddItem(new CGalleryItemFontColor(RGB(255, 255, 0), _T("Yellow")));
	pItems->AddItem(new CGalleryItemFontColor(RGB(146, 208, 80), _T("Light Green")));
	pItems->AddItem(new CGalleryItemFontColor(RGB(0, 176, 80), _T("Green")));
	pItems->AddItem(new CGalleryItemFontColor(RGB(0, 176, 240), _T("Light Blue")));
	pItems->AddItem(new CGalleryItemFontColor(RGB(0, 112, 192), _T("Blue")));
	pItems->AddItem(new CGalleryItemFontColor(RGB(0, 32, 96), _T("Dark Blue")));
	pItems->AddItem(new CGalleryItemFontColor(RGB(112, 48, 160), _T("Purple")));
}

COLORREF LightColor(COLORREF clr, int nTint)
{
	double h, s, l;

	CXTPDrawHelpers::RGBtoHSL(clr, h, s, l);
	clr = CXTPDrawHelpers::HSLtoRGB(h, s, l + (1.0 - l) * (double)nTint / 100.0);

	return clr;
}

COLORREF DarkColor(COLORREF clr, int nShade)
{
	double h, s, l;

	CXTPDrawHelpers::RGBtoHSL(clr, h, s, l);
	clr = CXTPDrawHelpers::HSLtoRGB(h, s, l * (1 + (double)nShade / 100.0));

	return clr;
}

int nMatrix[6][10] = 
{
	{  0,   0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ -5, +50, -10, +80, +80, +80, +80, +80, +80, +80},
	{ -15, +35, -25, +60, +60, +60, +60, +60, +60, +60},
	{ -25, +25, -50, +40, +40, +40, +40, +40, +40, +40},
	{ -35, +15, -75, -25, -25, -25, -25, -25, -25, -25},
	{ -50, +5, -90, -50, -50, -50, -50, -50, -50, -50},
};

LPCTSTR strMatrixTip[10] = 
{
	_T("Background 1"), _T("Text 1"), _T("Background 2"), _T("Text 2"), 
	_T("Accent 1"), _T("Accent 2"), _T("Accent 3"), _T("Accent 4"), _T("Accent 5"), _T("Accent 6")
};

COLORREF TranslateColor(COLORREF clr, int nTransalte)
{
	return nTransalte == 0 ? clr : nTransalte > 0 ? LightColor(clr, nTransalte) : DarkColor(clr, nTransalte);
}

CString GetColorTip(LPCTSTR lpszToolTip, LPCTSTR lpszAccent, int nTransalte)
{
	CString strTranslate;
	strTranslate.Format(_T(", %s %i%%"), nTransalte > 0 ? _T("Lighter") : _T("Darker"), abs(nTransalte));

	return (lpszToolTip ? CString(lpszToolTip) + _T(", ") : CString(_T(""))) + CString(lpszAccent) + (nTransalte != 0 ? strTranslate : _T(""));
}

void CGalleryItemFontColor::AddThemeColors(CXTPControlGalleryItems* pItems, int nTheme)
{
	const COLORPREVIEW& clrPreview = themeColors[nTheme];

	for (int j = 0; j < 6; j++)
	{
		BOOL bTopMargin = j < 2;
		BOOL bBottomMargin = j == 0 || j == 5;
		
		for (int i = 0; i < 10; i++)
		{	
			int nTranslate = nMatrix[j][i];
			COLORREF clr = TranslateColor(clrPreview.rec[i].clr, nTranslate);
			CString strToolTip = GetColorTip(clrPreview.rec[i].lpszCaption, strMatrixTip[i], nTranslate);

			pItems->AddItem(new CGalleryItemFontColor(clr, strToolTip, bTopMargin, bBottomMargin));
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// CGalleryItemFontFace

CGalleryItemFontFace::CGalleryItemFontFace(LPCTSTR lpszCaption)
{
	m_strCaption = lpszCaption;
}

void CGalleryItemFontFace::Draw(CDC* pDC, CXTPControlGallery* pGallery, CRect rcItem, BOOL bEnabled, BOOL bSelected, BOOL /*bPressed*/, BOOL bChecked)
{
	CXTPPaintManager* pPaintManager = pGallery->GetPaintManager();

	pPaintManager->DrawRectangle(pDC, rcItem, bSelected, FALSE, bEnabled, bChecked, FALSE, xtpBarTypePopup, xtpBarPopup);

	COLORREF clrTextColor = pPaintManager->GetRectangleTextColor(bSelected, FALSE, bEnabled, FALSE, FALSE, xtpBarTypePopup, xtpBarPopup);
	pDC->SetTextColor(clrTextColor);

	CFont fnt;
	fnt.CreatePointFont(150, m_strCaption);

	CXTPFontDC font(pDC, &fnt);

	CRect rcText(rcItem);
	rcText.left += 4;

	pDC->DrawText(m_strCaption, rcText, DT_END_ELLIPSIS|DT_SINGLELINE|DT_VCENTER);
}

void CGalleryItemFontFace::AddFontItems(CXTPControlGalleryItems* pItems)
{
	pItems->AddLabel(_T("Recently Used Fonts"));

	pItems->AddLabel(_T("All Fonts"));

	static CXTPControlFontComboBoxList::CFontDescHolder s_fontHolder;
	s_fontHolder.EnumFontFamilies();

		// now walk through the fonts and remove (charset) from fonts with only one
	CArray<CXTPControlFontComboBoxList::CFontDesc*, CXTPControlFontComboBoxList::CFontDesc*>& arrFontDesc = 
		s_fontHolder.m_arrayFontDesc;

	int nCount = (int)arrFontDesc.GetSize();
	int i;
	for (i = 0; i < nCount; i++)
	{
		CXTPControlFontComboBoxList::CFontDesc* pDesc = arrFontDesc[i];
		CString str = pDesc->m_strName;

		if (str == _T("Aria_T(") || str == _T(")Tahoma"))
		{
			CXTPControlGalleryItem* pItem = pItems->InsertItem(1, new CGalleryItemFontFace(str));
			pItem->SetData((DWORD_PTR)pDesc);
		}

		CXTPControlGalleryItem* pItem = pItems->AddItem(new CGalleryItemFontFace(str));
		pItem->SetData((DWORD_PTR)pDesc);
	}
}


//////////////////////////////////////////////////////////////////////////
// CControlGalleryUndo

IMPLEMENT_XTP_CONTROL(CControlGalleryUndo,  CXTPControlGallery)

CControlGalleryUndo::CControlGalleryUndo()
{
}

void CControlGalleryUndo::OnSelectedItemChanged()
{
	CXTPControlStatic* pInfo = FindInfoControl();
	if (pInfo)
	{
		CString str;
		if (!IsItemSelected())
		{
			str = _T("Cancel");
		}
		else
		{
			str.Format(_T("Undo %i Actions"), GetSelectedItem() + 1);
		}
		pInfo->SetCaption(str);
		pInfo->RedrawParent(FALSE);
	}

}

CXTPControlStatic* CControlGalleryUndo::FindInfoControl()
{
	CXTPCommandBar* pCommandBar = GetParent();

	for (int i = 0; i < pCommandBar->GetControls()->GetCount(); i++)
	{
		CXTPControlStatic* pControlStatic = DYNAMIC_DOWNCAST(CXTPControlStatic, pCommandBar->GetControl(i));
		if (pControlStatic && pControlStatic->GetID() == GetID())
		{
			return pControlStatic;
		}

	}
	return NULL;
}


CGalleryItemUndo::CGalleryItemUndo(LPCTSTR lpszCaption)
{
	m_strCaption = lpszCaption;
}

void CGalleryItemUndo::Draw(CDC* pDC, CXTPControlGallery* pGallery, CRect rcItem, BOOL bEnabled, BOOL bSelected, BOOL bPressed, BOOL bChecked)
{
	int nItem = pGallery->IsItemSelected() ? pGallery->GetSelectedItem() : -1;
	bSelected = GetID() <= nItem;

	CXTPControlGalleryItem::Draw(pDC, pGallery, rcItem, bEnabled, bSelected, bPressed, bChecked);
}
