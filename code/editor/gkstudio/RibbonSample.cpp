// RibbonSample.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "RibbonSample.h"

#include "MainFrm.h"
#include "RibbonSampleDoc.h"
#include "RibbonSampleView.h"
#include "WorkspaceView.h"

#include "gkPlatform.h"
#include "gkEditor.h"

#include "gkStudioWndMsg.h"
#include "StartBanner.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRibbonSampleApp

BEGIN_MESSAGE_MAP(CRibbonSampleApp, CWinApp)
	//{{AFX_MSG_MAP(CRibbonSampleApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()


//////////////////////////////////////////////////////////////////////////
static CMainFrame* GetMainFrame()
{
	CWnd *pWnd = AfxGetMainWnd();
	if (!pWnd)
		return 0;
	if (!pWnd->IsKindOf(RUNTIME_CLASS(CMainFrame)))
		return 0;
	return (CMainFrame*)AfxGetMainWnd();
}


/////////////////////////////////////////////////////////////////////////////
// CRibbonSampleApp construction

CRibbonSampleApp::CRibbonSampleApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	 m_richeditInst = LoadLibrary(_T("msftedit.dll"));
}


CRibbonSampleApp::~CRibbonSampleApp()
{
	FreeLibrary(m_richeditInst);
}


/////////////////////////////////////////////////////////////////////////////
// The one and only CRibbonSampleApp object

CRibbonSampleApp theApp;

LRESULT CALLBACK EditorWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(hWnd, msg, wParam, lParam);
}


BOOL CRibbonSampleApp::FirstInstance()
{                                       
	CWnd* pwndFirst = CWnd::FindWindow( _T("gkStudioClass"),NULL);
	if (pwndFirst)
	{
		// another instance is already running - activate it
		CWnd* pwndPopup = pwndFirst->GetLastActivePopup();								   
		pwndFirst->SetForegroundWindow();
		if (pwndFirst->IsIconic())
			pwndFirst->ShowWindow(SW_SHOWNORMAL);
		if (pwndFirst != pwndPopup)
			pwndPopup->SetForegroundWindow(); 

// 		if (m_bPreviewMode)
// 		{
// 			// IF in preview mode send this window copy data message to load new preview file.
// 			COPYDATASTRUCT cd;
// 			ZeroMemory(&cd, sizeof(COPYDATASTRUCT));
// 			cd.dwData = 100;
// 			cd.cbData = strlen(m_sPreviewFile);
// 			cd.lpData = m_sPreviewFile;
// 			pwndFirst->SendMessage( WM_COPYDATA,0,(LPARAM)&cd );
// 		}
		return FALSE;			
	}
	else
	{   
		// this is the first instance
		// Register your unique class name that you wish to use
		WNDCLASS wndcls;
		ZeroMemory(&wndcls, sizeof(WNDCLASS));
		wndcls.style = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
		wndcls.lpfnWndProc = EditorWndProc;
		wndcls.hInstance = AfxGetInstanceHandle();
		wndcls.hIcon = LoadIcon(IDR_MAINFRAME); // or load a different icon.
		wndcls.hCursor = LoadCursor(IDC_ARROW);
		wndcls.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wndcls.lpszMenuName = NULL;

		// Specify your own class name for using FindWindow later
		wndcls.lpszClassName = _T("gkStudioClass");

		// Register the new class and exit if it fails
		if(!AfxRegisterClass(&wndcls))
		{
			TRACE("Class Registration Failed\n");
			return FALSE;
		}
		//		bClassRegistered = TRUE;

		return TRUE;
	}
}	

/////////////////////////////////////////////////////////////////////////////
// CRibbonSampleApp initialization

BOOL CRibbonSampleApp::InitInstance()
{
//TODO: call AfxInitRichEdit2() to initialize richedit2 library.

// 	LCID lcidNew = MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT);
// 	SetThreadLocale(lcidNew);

	FirstInstance();

	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxInitRichEdit2();

	AfxEnableControlContainer();
	CXTPWinDwmWrapper().SetProcessDPIAware();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#if _MSC_VER <= 1200 //MFC 6.0 or earlier
#ifdef _AFXDLL
	Enable3dControls();         // Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();   // Call this when linking to MFC statically
#endif
#endif
	// Change the registry key under which our settings are stored.
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("gkStudio"));
 

	LoadStdProfileSettings(0);  // Load standard INI file options (including MRU)

	m_pRecentFileList = new CXTPPinableRecentFileList(0, _T("Recent File List"), _T("File%d"), 16);
	m_pRecentFileList->ReadList();

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	// load gkSystem dll
	HINSTANCE m_hSystemHandle = LoadLibraryEx(_T("gkGameFramework.dll"), NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
	m_FuncStart = (GET_SYSTEM)GetProcAddress(m_hSystemHandle, "gkModuleInitialize");
	m_FuncEnd = (DESTROY_END)GetProcAddress(m_hSystemHandle, "gkModuleUnload");
	m_pGameFramework = m_FuncStart();
	gEnv = m_pGameFramework->getENV();



	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CRibbonSampleDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CWorkspaceView));
	pDocTemplate->SetContainerInfo(IDR_CNTR_INPLACE);
	AddDocTemplate(pDocTemplate);


	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	CStartBanner logo;
	// Do not create logo screen for model viewer.
	logo.Create( CStartBanner::IDD );

	// initialize gkENGINE
	ISystemInitInfo sii;
	CSceneLoadCallback pCallback;
	sii.hInstance = AfxGetInstanceHandle();
	sii.pProgressCallBack = &pCallback;
	m_pGameFramework->Init( sii );
	
	m_pEditor = new gkEditor();

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The one and only window has been initialized, so show and update it.
// 	m_pMainWnd->ShowWindow(SW_SHOW);
// 	m_pMainWnd->UpdateWindow();

	// initialize editor
	


	m_pGameFramework->PostInit( AfxGetMainWnd()->GetSafeHwnd() , sii);
	m_pEditor->Init(GetMainFrame());

	
	Sleep(500);
	CStartBanner::SetText(_T("Finishing Initialize..."));
	Sleep(100);
	logo.DestroyWindow();

	m_pMainWnd->ShowWindow(SW_SHOW);



	

	m_pMainWnd->UpdateWindow();


	m_pMainWnd->DragAcceptFiles();

	return TRUE;
}

// App command to run the dialog
void CRibbonSampleApp::OnAppAbout()
{
	//CAboutDlg aboutDlg;
	//aboutDlg.DoModal();
}
//-----------------------------------------------------------------------
int CRibbonSampleApp::ExitInstance()
{
	m_pEditor->Destroy();
	m_pGameFramework->Destroy();

	// free
	if(m_FuncEnd)
	{
		m_FuncEnd();
		if (m_hSystemHandle)
			FreeLibrary(m_hSystemHandle);
	}

	SAFE_DELETE( m_pEditor );


	return CWinApp::ExitInstance();
}


//-----------------------------------------------------------------------
BOOL CRibbonSampleApp::OnIdle( LONG lCount )
{
	if ( m_pMainWnd->GetFocus() == NULL )
	{
		Sleep(33);
	}
	else
	{
		m_pEditor->Update();
		m_pGameFramework->Update();
	}

	CWinApp::OnIdle(lCount);

	return true;
}

/////////////////////////////////////////////////////////////////////////////
// CRibbonSampleApp message handlers

