// PaneConsoleVar.cpp : implementation file
//

#include "stdafx.h"
#include "PaneConsoleVar.h"
#include "Controls\CustomItems.h"
#include <string>
#include <cctype>
#include <algorithm>


// CPaneConsoleVar dialog

IMPLEMENT_DYNAMIC(CPaneConsoleVar, CXTResizeDialog)

CPaneConsoleVar::CPaneConsoleVar(CWnd* pParent /*=NULL*/)
	: IPaneBase(CPaneConsoleVar::IDD, pParent)
{

}

CPaneConsoleVar::~CPaneConsoleVar()
{
}

void CPaneConsoleVar::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PANE_CONSOLEVAR_PLACEHOLDER, m_wndPlaceHolder);
	DDX_Control(pDX, IDC_EDIT_CVSEARCH, m_edtSearch);
}




BEGIN_MESSAGE_MAP(CPaneConsoleVar, IPaneBase)

	ON_MESSAGE(XTPWM_PROPERTYGRID_NOTIFY, OnGridNotify)
	ON_EN_CHANGE(IDC_EDIT_CVSEARCH, &CPaneConsoleVar::OnEnChangeEditCvsearch)
	ON_WM_SHOWWINDOW()

END_MESSAGE_MAP()


void CPaneConsoleVar::RefreshMetrics(BOOL bOffice2007)
{
	//m_brush.DeleteObject();

	if (bOffice2007)
	{
		COLORREF clr = XTPResourceImages()->GetImageColor(_T("DockingPane"), _T("WordPaneBackground"));
// 		m_brush.CreateSolidBrush(clr);
// 
// 		m_edtConsoleBox.SetListStyle(xtListBoxOffice2007);
// 		m_btnCVar.SetTheme(xtpButtonThemeOffice2007);
	}
	else
	{
// 		m_brush.CreateSolidBrush(GetSysColor(COLOR_3DFACE));
// 
// 		m_edtConsoleBox.SetListStyle(xtListBoxOfficeXP);
// 		m_btnCVar.SetTheme(xtpButtonThemeOfficeXP);
	}


}

BOOL CPaneConsoleVar::InitPropertyTable()
{
	CRect rc;
	m_wndPlaceHolder.GetWindowRect( &rc );
	ScreenToClient( &rc );

	// create the property grid.
	if ( m_wndPropertyGrid.Create( rc, this, IDC_PANECONSOLEVAR ) )
	{
		m_wndPropertyGrid.SetVariableItemsHeight(TRUE);

		LOGFONT lf;
		GetFont()->GetLogFont( &lf );


		CXTPPropertyGridItem* pItem = NULL;

		// create global settings category.
		CXTPPropertyGridItem* pGlobals      = m_wndPropertyGrid.AddCategory(_T("控制台参数"));
	}

	SetResize(IDC_PANECONSOLEVAR, SZ_TOP_LEFT, SZ_BOTTOM_RIGHT);

	return TRUE;
}

BOOL CPaneConsoleVar::RefreshPropertyMap( LPCTSTR filter /*= NULL*/ )
{
	CVarNameMap& namemap = gEnv->pCVManager->getCVarMap();

	m_wndPropertyGrid.ResetContent();
	CXTPPropertyGridItem* pGlobals      = m_wndPropertyGrid.AddCategory(_T("控制台参数"));
	
	CVarNameMap::iterator it = namemap.begin();
	int idCount = 0;
	for (; it != namemap.end(); ++it)
	{
		if (filter)
		{
			TCHAR upperFilter[MAX_PATH];
			_tcscpy_s(upperFilter, filter);
			_tcsupr_s(upperFilter);
			gkStdString uppername = it->first;
			std::transform(uppername.begin(), uppername.end(), uppername.begin(), std::toupper);
			if ( gkStdString::npos == uppername.find(upperFilter))
				continue;
		}


		if( it->second.m_type == 0 )
		{
			// float
			m_wndPropertyGrid.AddDoubleVar( pGlobals, it->first.c_str(), idCount, (double*)(it->second.m_ptr), it->second.m_dMin, it->second.m_dMax, it->second.m_wstrDesc.c_str() );
			idCount++;
		}
		else if (it->second.m_type == 1)
		{
			m_wndPropertyGrid.AddIntegerVar(  pGlobals, it->first.c_str(), idCount, (int*)(it->second.m_ptr), it->second.m_wstrDesc.c_str() );
			idCount++;
			
		}
	}

	pGlobals->Expand();

	return TRUE;
}

BOOL CPaneConsoleVar::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();

	SetResize(IDC_EDIT_CVSEARCH,     SZ_BOTTOM_LEFT, SZ_BOTTOM_RIGHT);
	SetResize(IDC_STATIC_CVSEARCH,     SZ_BOTTOM_LEFT, SZ_BOTTOM_CENTER);

	RefreshMetrics(TRUE);
	InitPropertyTable();
	//RefreshPropertyMap();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CPaneConsoleVar::OnGridNotify( WPARAM wParam, LPARAM lParam )
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
		TRACE(_T("Value Changed. Caption = %s, ID = %i, Value = %s\n"), pItem->GetCaption(), pItem->GetID(), pItem->GetValue());

		if (DYNAMIC_DOWNCAST(CXTPPropertyGridItemEnum, pItem))
		{
			if (pItem->GetMetrics(TRUE, FALSE))
			{
				pItem->GetMetrics(TRUE, FALSE)->m_nImage = ((CXTPPropertyGridItemEnum*)pItem)->GetEnum();
			}
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




// CPaneConsoleVar message handlers

void CPaneConsoleVar::OnEnChangeEditCvsearch()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CXTResizeDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	CString text;
	m_edtSearch.GetWindowText(text);
	if (text == _T(""))
		RefreshPropertyMap();
	else
		RefreshPropertyMap(text);

}

// void CPaneConsoleVar::OnShowWindow( BOOL bShow, UINT nStatus )
// {
// 	CXTResizeDialog::OnShowWindow(bShow, nStatus);
// 
// 	if (bShow)
// 	{
// 		RefreshPropertyMap();
// 	}
// }

void CPaneConsoleVar::OnShowWindow( BOOL bShow, UINT nStatus )
{
	if (bShow)
	{
		gkLogMessage(_T("cvar opened."));
		RefreshPropertyMap();
	}
	
}
