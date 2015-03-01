//////////////////////////////////////////////////////////////////////////
/*
Copyright (c) 2011-2015 Kaiming Yi
	
	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:
	
	The above copyright notice and this permission notice shall be included in
	all copies or substantial portions of the Software.
	
	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
	THE SOFTWARE.
	
*/
//////////////////////////////////////////////////////////////////////////



// AboutDlg.h : header file
//
// This file is a part of the XTREME TOOLKIT PRO MFC class library.
// (c)1998-2009 Codejock Software, All Rights Reserved.
//
// THIS SOURCE FILE IS THE PROPERTY OF CODEJOCK SOFTWARE AND IS NOT TO BE
// RE-DISTRIBUTED BY ANY MEANS WHATSOEVER WITHOUT THE EXPRESSED WRITTEN
// CONSENT OF CODEJOCK SOFTWARE.
//
// THIS SOURCE CODE CAN ONLY BE USED UNDER THE TERMS AND CONDITIONS OUTLINED
// IN THE XTREME TOOLKIT PRO LICENSE AGREEMENT. CODEJOCK SOFTWARE GRANTS TO
// YOU (ONE SOFTWARE DEVELOPER) THE LIMITED RIGHT TO USE THIS SOFTWARE ON A
// SINGLE COMPUTER.
//
// CONTACT INFORMATION:
// support@codejock.com
// http://www.codejock.com
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(__ABOUTDLG_H__)
#define __ABOUTDLG_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if (_MSC_VER > 1310) // VS2005
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog
#include "Resource.h"

class CAboutDlg : public CDialog
{
// Construction
public:
	CAboutDlg(CWnd* pParent = NULL)
		: CDialog(IDD_ABOUTBOX, pParent)
	{
		NONCLIENTMETRICS ncm;
		::ZeroMemory(&ncm, sizeof(NONCLIENTMETRICS));
		ncm.cbSize = sizeof(NONCLIENTMETRICS);
		
		VERIFY(::SystemParametersInfo(SPI_GETNONCLIENTMETRICS,
			sizeof(NONCLIENTMETRICS), &ncm, 0));

		ncm.lfMenuFont.lfWeight = FW_BOLD;
		m_fontBold.CreateFontIndirect(&ncm.lfMenuFont);
	}

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	//enum { IDD = IDD_ABOUTBOX };
	CStatic m_txtPackageVersion;
	CStatic m_txtCopyrightInfo;
	CStatic m_txtAppName;
	CXTButton m_btnOk;
	CXTHyperLink m_txtURL;
	CXTHyperLink m_txtEmail;
	//}}AFX_DATA

	CFont m_fontBold;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:

	virtual void DoDataExchange(CDataExchange* pDX)
	{
		CDialog::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(CAboutDlg)
		DDX_Control(pDX, IDC_TXT_TITLE, m_txtPackageVersion);
		DDX_Control(pDX, IDC_TXT_COPYRIGHT, m_txtCopyrightInfo);
		DDX_Control(pDX, IDC_TXT_APPNAME, m_txtAppName);
		DDX_Control(pDX, IDOK, m_btnOk);
		DDX_Control(pDX, IDC_TXT_URL, m_txtURL);
		DDX_Control(pDX, IDC_TXT_EMAIL, m_txtEmail);
		//}}AFX_DATA_MAP
	}

	virtual BOOL OnInitDialog()
	{
		CDialog::OnInitDialog();

		// get a pointer to CWinApp.
		CWinApp* pApp = AfxGetApp( );
		ASSERT( pApp != NULL );

		// set the sample title.
		CString csSampleTitle;
		csSampleTitle.Format(_T("%s Application"), pApp->m_pszAppName);
		m_txtAppName.SetWindowText( csSampleTitle );

		// set the package version.
		CString csPackageVersion;
		csPackageVersion.Format( _T( "%s v%d.%d.%d" ),
			_XTP_PACKAGE_NAME, _XTPLIB_VERSION_MAJOR, _XTPLIB_VERSION_MINOR, _XTPLIB_VERSION_REVISION);
		m_txtPackageVersion.SetWindowText( csPackageVersion );

		// set the about caption.
		CString csAboutCaption;
		csAboutCaption.Format( _T("About %s" ), pApp->m_pszAppName );
		SetWindowText( csAboutCaption );

		// set the copyright info.
		CString csCopyrightInfo;
		csCopyrightInfo.Format( _T( "(c)1998-%d Codejock Software, All Rights Reserved" ),
			COleDateTime::GetCurrentTime().GetYear( ) );
		m_txtCopyrightInfo.SetWindowText( csCopyrightInfo );

		// set the title text to bold font.
		m_txtPackageVersion.SetFont( &m_fontBold );


		// define the url for our hyperlinks.
		m_txtURL.SetURL( _T( "http://www.codejock.com" ) );
		m_txtURL.SetUnderline( false );

		m_txtEmail.SetURL( _T( "mailto:sales@codejock.com" ) );
		m_txtEmail.SetUnderline( false );

		// set OK button style.
		m_btnOk.SetXButtonStyle( BS_XT_SEMIFLAT | BS_XT_HILITEPRESSED | BS_XT_WINXP_COMPAT );

		return TRUE;  // return TRUE unless you set the focus to a control
		              // EXCEPTION: OCX Property Pages should return FALSE
	}
};

#ifdef ENABLE_MANIFESTEDITOR


#if !defined(_UNICODE) && (_MSC_VER < 1400)

#if _MSC_VER > 1200 //MFC 7.0
#include <..\src\mfc\afximpl.h> // MFC Global data
#else
#include <..\src\afximpl.h>     // MFC Global data
#endif

#endif

AFX_INLINE void EnableManifestEditor()
{
#if !defined(_UNICODE) && (_MSC_VER < 1400)
	OSVERSIONINFO ovi = {sizeof(OSVERSIONINFO)};
	::GetVersionEx(&ovi);

	if ((ovi.dwPlatformId >= VER_PLATFORM_WIN32_NT) && (ovi.dwMajorVersion >= 5))
	{
#if (_MSC_VER >= 1200)
		afxData.bWin95 = TRUE;
#else
		afxData.bWin32s = TRUE;
#endif
	}
#endif
}

#else

AFX_INLINE void EnableManifestEditor()
{
}

#endif


AFX_INLINE void ShowSampleHelpPopup(CWnd* pParentWnd, UINT nIDResource)
{
	CXTPPopupControl* pPopup = new CXTPPopupControl();
	pPopup->SetTransparency(200);
	pPopup->SetTheme(xtpPopupThemeOffice2003);
	pPopup->AllowMove(TRUE);
	pPopup->SetAnimateDelay(500);
	pPopup->SetPopupAnimation();
	pPopup->SetShowDelay(5000);
	pPopup->SetAutoDelete(TRUE);
	
	CXTPPopupItem* pItemText = (CXTPPopupItem*)pPopup->AddItem(new CXTPPopupItem(CRect(8, 12, 500, 130)));
	pItemText->SetRTFText(nIDResource);
	pItemText->FitToContent();
	CSize sz(pItemText->GetRect().Size());

	// close icon.
	CXTPPopupItem* pItemIcon = (CXTPPopupItem*)pPopup->AddItem(
		new CXTPPopupItem(CRect(sz.cx + 2, 10, sz.cx + 2 + 16, 10 + 16)));
	
	pPopup->GetImageManager()->SetIcon(IDI_POPUP_CLOSE, IDI_POPUP_CLOSE);
	pItemIcon->SetIconIndex(IDI_POPUP_CLOSE);

	pItemIcon->SetButton(TRUE);
	pItemIcon->SetID(XTP_ID_POPUP_CLOSE);

	pPopup->SetPopupSize(CSize(sz.cx + 20, sz.cy + 20));
	pPopup->Show(pParentWnd);
}


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(__ABOUTDLG_H__)
