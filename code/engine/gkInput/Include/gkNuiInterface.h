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



//////////////////////////////////////////////////////////////////////////
//
// K&K Studio GameKnife ENGINE Source File
//
// Name:   	gkNuiInterface.h
// Desc:		
// 	
// Author:  Kaiming
// Date:	2012/5/8
// Modify:	2012/5/8
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _gkNuiInterface_h_
#define _gkNuiInterface_h_

#include "MSR_NuiApi.h"
//#include "DrawDevice.h"

#define SZ_APPDLG_WINDOW_CLASS          _T("SkeletalViewerAppDlgWndClass")
#define WM_USER_UPDATE_FPS              WM_USER
#define WM_USER_UPDATE_COMBO            WM_USER+1

class gkNuiInterface
{
public:
	gkNuiInterface();
	~gkNuiInterface();
	HRESULT                 Nui_Init();
	HRESULT                 Nui_Init(int index);
	HRESULT                 Nui_Init(OLECHAR *instanceName);
	void                    Nui_UnInit( );
	void                    Nui_GotDepthAlert( );
	void                    Nui_GotVideoAlert( );
	void                    Nui_GotSkeletonAlert( );
	void                    Nui_Zero();
	void                    Nui_BlankSkeletonScreen( HWND hWnd );
	void                    Nui_DoDoubleBuffer(HWND hWnd,HDC hDC);
	void                    Nui_DrawSkeleton( bool bBlank, NUI_SKELETON_DATA * pSkel, HWND hWnd, int WhichSkeletonColor );
	void                    Nui_DrawSkeletonSegment( NUI_SKELETON_DATA * pSkel, int numJoints, ... );

	RGBQUAD                 Nui_ShortToQuad_Depth( USHORT s );

	static LRESULT CALLBACK MessageRouter(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK        WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static void CALLBACK    Nui_StatusProcThunk(const NuiStatusData *pStatusData);
	void CALLBACK           Nui_StatusProc(const NuiStatusData *pStatusData);

	HWND m_hWnd;

private:
	bool m_fUpdatingUi;
	void UpdateComboBox();

	CRITICAL_SECTION        m_critSecUi; // Gate UI operations on the background thread.
	static DWORD WINAPI     Nui_ProcessThread(LPVOID pParam);
	DWORD WINAPI            Nui_ProcessThread();
	INuiInstance*           m_pNuiInstance;
	BSTR                    m_instanceId;

	// NUI and draw stuff
	//DrawDevice              m_DrawDepth;
	//DrawDevice              m_DrawVideo;

	// thread handling
	HANDLE        m_hThNuiProcess;
	HANDLE        m_hEvNuiProcessStop;

	HANDLE        m_hNextDepthFrameEvent;
	HANDLE        m_hNextVideoFrameEvent;
	HANDLE        m_hNextSkeletonEvent;
	HANDLE        m_pDepthStreamHandle;
	HANDLE        m_pVideoStreamHandle;
	HFONT         m_hFontFPS;
	HPEN          m_Pen[6];
	HDC           m_SkeletonDC;
	HBITMAP       m_SkeletonBMP;
	HGDIOBJ       m_SkeletonOldObj;
	int           m_PensTotal;
	POINT         m_Points[NUI_SKELETON_POSITION_COUNT];
	RGBQUAD       m_rgbWk[640*480];
	int           m_LastSkeletonFoundTime;
	bool          m_bScreenBlanked;
	int           m_FramesTotal;
	int           m_LastFPStime;
	int           m_LastFramesTotal;
};

#endif
