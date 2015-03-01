//////////////////////////////////////////////////////////////////////////
//
// K&K Studio GameKnife ENGINE Source File
//
// Name:   	gkMaxPort.cpp
// Desc:		
// 
// Author:  Kaiming
// Date:	2011/9/20 
// Modify:	2011/9/20
// 
//////////////////////////////////////////////////////////////////////////
#include "../system/gkMaxPort.H"
#include "gmpMaxViewport.h"
#include "resource1.h"
#include "IRenderer.h"
#include "ISystem.h"
#include "decomp.h"
//#include "IGameExporter.h"

// #include "gkMemoryLeakDetecter.h"
// #include "gkStudioWndMsg.h"
// #include "gkObject.h"
// #include "IResourceManager.h"

struct CreateStruct
{
	DWORD dwExStyle;
	DWORD style;
	int x;
	int y;
	int w;
	int h;
	HWND hParent;
	HINSTANCE hInst;
	HWND hWnd;
	volatile int lock;
};

LRESULT WINAPI __DefWindowProc(HWND hWnd,UINT Msg,WPARAM wParam,LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	RECT area;
	GraphicsWindow* gw;
	HMENU hMenu,HMenuView;
	HINSTANCE hBase;

	switch( Msg )
	{
	case WM_LBUTTONDOWN:
		{
			HWND hwnd1 = GetCOREInterface()->GetActiveViewport()->GetHWnd();
			HWND hwnd2 = ::GetParent( hWnd );
			if(hwnd1!=hWnd)
			{ 
				GetCOREInterface()->MakeExtendedViewportActive( hWnd ); 
			} 
		}
		break;
	case WM_RBUTTONUP:
		{
			hBase = ::GetModuleHandle( 0 );
			hMenu = ::LoadMenu( gUtilEnv->hInst, MAKEINTRESOURCE(IDR_MENU1));
			HMenuView = ::GetSubMenu( hMenu,0);
			DWORD fwKeys = wParam; 
			POINT pt;
			pt.x= LOWORD(lParam); 
			pt.y = HIWORD(lParam);

			//RECT rect;
			//GetClientRect(hWnd, &rect);
			//GetCOREInterface()->PutUpViewMenu( hWnd,pt);
			::TrackPopupMenu( HMenuView,0,pt.x,pt.y,0,hWnd,&area);
		}
		break;
	case WM_COMMAND:
		{
			DWORD type = HIWORD(wParam);
			DWORD id = LOWORD(wParam);
			if(  type == 0 )
			{
				switch(id)
				{
				case ID_GKMAXPORTMENU_LOADSCENE:
					gEnv->pSystem->LoadScene_s(_T("EditorNew.GKS"), NULL);
					break;

				case ID_GKMAXPORTMENU_CHANGERENDERMODE:
					{
						RECT rect;
						GetClientRect(hWnd, &rect);	
						gEnv->pRenderer->SetCurrContent( hWnd, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
					}

					break;
				}
			}
			break;
		}
	case WM_PAINT:  
		break;
	case WM_SIZE:
		{
			int cx = LOWORD( lParam );
			int cy = HIWORD( lParam );

			//RECT rect;
			//GetClientRect(hWnd, &rect);
			if (gEnv && gEnv->pRenderer)
			{
				gEnv->pRenderer->SetCurrContent( hWnd, 0, 0, cx, cy);
			}

		}
		break;
	}
	return ::DefWindowProc( hWnd,Msg,wParam,lParam);
}

typedef ISystem* (*GET_SYSTEM)(void);
typedef void (*DESTROY_END)(void);

HINSTANCE hHandle;
GET_SYSTEM pFuncStart;
DESTROY_END pFuncEnd;

HWND gkPortWindow::CreateViewWindow( HWND hParent, int x, int y, int w, int h )
{
 	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, __DefWindowProc, 0L, 0L, 
 		gUtilEnv->hInst, NULL, NULL, NULL, NULL,
 		"gkMaxPortWindow", NULL };
 	::RegisterClassEx( &wc ); 

	hParent = CreateWindow("gkMaxPortWindow", "gkMaxPort", WS_VISIBLE | WS_CHILD, x, y, w, h, hParent, NULL,gUtilEnv->hInst, NULL);

	gUtilEnv->m_bUpdate = true;

		// init gkSystem here
	// load gkSystem dll

	//::AttachThreadInput( ::GetCurrentThreadId(), *threadid,TRUE);

	hHandle = LoadLibraryEx("gkSystem.dll", NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
	pFuncStart = (GET_SYSTEM)GetProcAddress(hHandle, "gkModuleInitialize");
	pFuncEnd = (DESTROY_END)GetProcAddress(hHandle, "gkModuleUnload");
	gUtilEnv->pSystem = pFuncStart();

	gEnv = gUtilEnv->pSystem->getENV();

	// run
	ISystemInitInfo sii;
	sii.pProgressCallBack = 0;
	sii.hInstance = 0;
	sii.fWidth = 128;
	sii.fHeight = 128;

	gUtilEnv->pSystem->Init( sii );


	GetCOREInterface()->RegisterViewportDisplayCallback(FALSE, this);


	return hParent;
}


void gkPortWindow::DestroyViewWindow( HWND hWnd )
{
	GetCOREInterface()->UnRegisterViewportDisplayCallback(FALSE, this);
	gUtilEnv->pSystem->Destroy();

	// free
	pFuncEnd();
	FreeLibrary(hHandle);


	::DestroyWindow( hWnd );

	
	gUtilEnv->m_bUpdate = false;
}

void gkPortWindow::Display( TimeValue t, ViewExp *vpt, int flags )
{
		//Sleep(50);
	static int lastTime = t;
// 	if (t - lastTime < 480)
// 	{
// 		lastTime = t;
// 		return;
// 	}

	lastTime = t;

		// here hack the campos [9/26/2011 Kaiming]
	if (GetCOREInterface()->GetActiveViewport()->IsPerspView())
	{
		AffineParts ap;
		AffineParts apI;
		Matrix3 viewTM;
		Matrix3 viewTMI;
		GetCOREInterface()->GetActiveViewport()->GetAffineTM(viewTM);

		// change to export the front dir of cam
		//GetCOREInterface()->GetActiveViewport()->get

		viewTMI = viewTM;
		viewTM.RotateX(PI * 0.5f);
		//Point3 viewDir = Point3(0,-1,0);
		//viewDir = viewTM.VectorTransform(viewDir);

		float yaw;
		float roll;
		float pitch;
		viewTM.GetYawPitchRoll(&yaw, &pitch, &roll);
		//Point4 viewDir = viewTM.GetRow(2);
		viewTMI.Invert();

		decomp_affine(viewTM, &ap);
		decomp_affine(viewTMI, &apI);

		Point3 pos = apI.t;
		Quat quat = ap.q;
		float fov = GetCOREInterface()->GetActiveViewport()->GetFOV();
		//gUtilEnv->pSystem->setOverrideCam(pos.x, pos.y, pos.z, -pitch, yaw, -roll , quat.w, fov);
		//gUtilEnv->pSystem->setOverrideCam(pos.x, pos.y, pos.z, -(pitch - 26.1f * PI / 180.0f), yaw, -roll , quat.w, fov);
	}

	gUtilEnv->pSystem->Update();
	
}

void gkPortWindow::GetViewportRect( TimeValue t, ViewExp *vpt, Rect *rect )
{

}
