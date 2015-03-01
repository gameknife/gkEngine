#include "InputStableHeader.h"
#include "gkNuiInterface.h"



// Note: 
//  1. DirectX SDK must be installed before compiling. 
//  2. DirectX SDK include path should be added after the VC include
//     path, because strsafe.h in DirectX SDK may be older.
//  3. platform SDK lib path should be added before the VC lib
//     path, because uuid.lib in VC lib path may be older

#include <strsafe.h>
/************************************************************************
*                                                                       *
*   NuiImpl.cpp -- Implementation of gkNuiInterface methods dealing *
*                  with NUI processing                                  *
*                                                                       *
* Copyright (c) Microsoft Corp. All rights reserved.                    *
*                                                                       *
* This code is licensed under the terms of the                          *
* Microsoft Kinect for Windows SDK (Beta)                               *
* License Agreement: http://kinectforwindows.org/KinectSDK-ToU          *
*                                                                       *
************************************************************************/

#include <mmsystem.h>
#include <assert.h>

static const COLORREF g_JointColorTable[NUI_SKELETON_POSITION_COUNT] = 
{
	RGB(169, 176, 155), // NUI_SKELETON_POSITION_HIP_CENTER
	RGB(169, 176, 155), // NUI_SKELETON_POSITION_SPINE
	RGB(168, 230, 29), // NUI_SKELETON_POSITION_SHOULDER_CENTER
	RGB(200, 0,   0), // NUI_SKELETON_POSITION_HEAD
	RGB(79,  84,  33), // NUI_SKELETON_POSITION_SHOULDER_LEFT
	RGB(84,  33,  42), // NUI_SKELETON_POSITION_ELBOW_LEFT
	RGB(255, 126, 0), // NUI_SKELETON_POSITION_WRIST_LEFT
	RGB(215,  86, 0), // NUI_SKELETON_POSITION_HAND_LEFT
	RGB(33,  79,  84), // NUI_SKELETON_POSITION_SHOULDER_RIGHT
	RGB(33,  33,  84), // NUI_SKELETON_POSITION_ELBOW_RIGHT
	RGB(77,  109, 243), // NUI_SKELETON_POSITION_WRIST_RIGHT
	RGB(37,   69, 243), // NUI_SKELETON_POSITION_HAND_RIGHT
	RGB(77,  109, 243), // NUI_SKELETON_POSITION_HIP_LEFT
	RGB(69,  33,  84), // NUI_SKELETON_POSITION_KNEE_LEFT
	RGB(229, 170, 122), // NUI_SKELETON_POSITION_ANKLE_LEFT
	RGB(255, 126, 0), // NUI_SKELETON_POSITION_FOOT_LEFT
	RGB(181, 165, 213), // NUI_SKELETON_POSITION_HIP_RIGHT
	RGB(71, 222,  76), // NUI_SKELETON_POSITION_KNEE_RIGHT
	RGB(245, 228, 156), // NUI_SKELETON_POSITION_ANKLE_RIGHT
	RGB(77,  109, 243) // NUI_SKELETON_POSITION_FOOT_RIGHT
};




void gkNuiInterface::Nui_Zero()
{
	m_pNuiInstance = NULL;
	m_hNextDepthFrameEvent = NULL;
	m_hNextVideoFrameEvent = NULL;
	m_hNextSkeletonEvent = NULL;
	m_pDepthStreamHandle = NULL;
	m_pVideoStreamHandle = NULL;
	m_hThNuiProcess=NULL;
	m_hEvNuiProcessStop=NULL;
	ZeroMemory(m_Pen,sizeof(m_Pen));
	m_SkeletonDC = NULL;
	m_SkeletonBMP = NULL;
	m_SkeletonOldObj = NULL;
	m_PensTotal = 6;
	ZeroMemory(m_Points,sizeof(m_Points));
	m_LastSkeletonFoundTime = -1;
	m_bScreenBlanked = false;
	m_FramesTotal = 0;
	m_LastFPStime = -1;
	m_LastFramesTotal = 0;
}

void CALLBACK gkNuiInterface::Nui_StatusProc(const NuiStatusData *pStatusData)
{
	if (SUCCEEDED(pStatusData->hrStatus))
	{
		// Update UI
		::PostMessageW(m_hWnd, WM_USER_UPDATE_COMBO, 0, 0);

		if (m_instanceId && 0 == wcscmp(pStatusData->instanceName, m_instanceId))
		{
			Nui_Init(m_instanceId);
		}
		else if (!m_pNuiInstance)
		{
			Nui_Init(0);
		}

	}
	else
	{
		if (0 == wcscmp(pStatusData->instanceName, m_instanceId))
		{
			Nui_UnInit();
			Nui_Zero();
		}
	}
}

HRESULT gkNuiInterface::Nui_Init(OLECHAR *instanceName)
{
	HRESULT hr = MSR_NuiCreateInstanceByName(instanceName, &m_pNuiInstance);

	if (FAILED(hr))
	{
		return hr;
	}

	if (m_instanceId)
	{
		::SysFreeString(m_instanceId);
	}

	m_instanceId = m_pNuiInstance->NuiInstanceName();

	return Nui_Init();
}

HRESULT gkNuiInterface::Nui_Init(int index)
{
	HRESULT hr = MSR_NuiCreateInstanceByIndex(index, &m_pNuiInstance);

	if (FAILED(hr))
	{
		return hr;
	}

	if (m_instanceId)
	{
		::SysFreeString(m_instanceId);
	}

	m_instanceId = m_pNuiInstance->NuiInstanceName();

	return Nui_Init();
}

HRESULT gkNuiInterface::Nui_Init()
{
	HRESULT                hr;

	if (!m_pNuiInstance)
	{
		HRESULT hr = MSR_NuiCreateInstanceByIndex(0, &m_pNuiInstance);

		if (FAILED(hr))
		{
			return hr;
		}

		if (m_instanceId)
		{
			::SysFreeString(m_instanceId);
		}

		m_instanceId = m_pNuiInstance->NuiInstanceName();
	}

	m_hNextDepthFrameEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
	m_hNextVideoFrameEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
	m_hNextSkeletonEvent = CreateEvent( NULL, TRUE, FALSE, NULL );

// 	GetWindowRect(GetDlgItem( m_hWnd, IDC_SKELETALVIEW ), &rc );
// 	int width = rc.right - rc.left;
// 	int height = rc.bottom - rc.top;
// 	HDC hdc = GetDC(GetDlgItem( m_hWnd, IDC_SKELETALVIEW));
// 	m_SkeletonBMP = CreateCompatibleBitmap( hdc, width, height );
// 	m_SkeletonDC = CreateCompatibleDC( hdc );
// 	::ReleaseDC(GetDlgItem(m_hWnd,IDC_SKELETALVIEW), hdc );
// 	m_SkeletonOldObj = SelectObject( m_SkeletonDC, m_SkeletonBMP );
// 
// 	hr = m_DrawDepth.CreateDevice( GetDlgItem( m_hWnd, IDC_DEPTHVIEWER ) );
// 	if( FAILED( hr ) )
// 	{
// 		MessageBoxResource( m_hWnd,IDS_ERROR_D3DCREATE,MB_OK | MB_ICONHAND);
// 		return hr;
// 	}
// 
// 	hr = m_DrawDepth.SetVideoType( 320, 240, 320 * 4 );
// 	if( FAILED( hr ) )
// 	{
// 		MessageBoxResource( m_hWnd,IDS_ERROR_D3DVIDEOTYPE,MB_OK | MB_ICONHAND);
// 		return hr;
// 	}
// 
// 	hr = m_DrawVideo.CreateDevice( GetDlgItem( m_hWnd, IDC_VIDEOVIEW ) );
// 	if( FAILED( hr ) )
// 	{
// 		MessageBoxResource( m_hWnd,IDS_ERROR_D3DCREATE,MB_OK | MB_ICONHAND);
// 		return hr;
// 	}
// 
// 	hr = m_DrawVideo.SetVideoType( 640, 480, 640 * 4 );
// 	if( FAILED( hr ) )
// 	{
// 		MessageBoxResource( m_hWnd,IDS_ERROR_D3DVIDEOTYPE,MB_OK | MB_ICONHAND);
// 		return hr;
// 	}

	DWORD nuiFlags = NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX | NUI_INITIALIZE_FLAG_USES_SKELETON |  NUI_INITIALIZE_FLAG_USES_COLOR;
	hr = m_pNuiInstance->NuiInitialize(nuiFlags);
	if (E_NUI_SKELETAL_ENGINE_BUSY == hr)
	{
		nuiFlags = NUI_INITIALIZE_FLAG_USES_DEPTH |  NUI_INITIALIZE_FLAG_USES_COLOR;
		hr = m_pNuiInstance->NuiInitialize(nuiFlags);
	}

	if( FAILED( hr ) )
	{
		//MessageBoxResource(m_hWnd,IDS_ERROR_NUIINIT,MB_OK | MB_ICONHAND);
		return hr;
	}

	if (HasSkeletalEngine(m_pNuiInstance))
	{
		hr = m_pNuiInstance->NuiSkeletonTrackingEnable( m_hNextSkeletonEvent, 0 );
		if( FAILED( hr ) )
		{
			//MessageBoxResource(m_hWnd,IDS_ERROR_SKELETONTRACKING,MB_OK | MB_ICONHAND);
			return hr;
		}
	}

	hr = m_pNuiInstance->NuiImageStreamOpen(
		NUI_IMAGE_TYPE_COLOR,
		NUI_IMAGE_RESOLUTION_640x480,
		0,
		2,
		m_hNextVideoFrameEvent,
		&m_pVideoStreamHandle );
	if( FAILED( hr ) )
	{
		//MessageBoxResource(m_hWnd,IDS_ERROR_VIDEOSTREAM,MB_OK | MB_ICONHAND);
		return hr;
	}

	hr = m_pNuiInstance->NuiImageStreamOpen(
		HasSkeletalEngine(m_pNuiInstance) ? NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX : NUI_IMAGE_TYPE_DEPTH,
		NUI_IMAGE_RESOLUTION_320x240,
		0,
		2,
		m_hNextDepthFrameEvent,
		&m_pDepthStreamHandle );
	if( FAILED( hr ) )
	{
		//MessageBoxResource(m_hWnd,IDS_ERROR_DEPTHSTREAM,MB_OK | MB_ICONHAND);
		return hr;
	}

	// Start the Nui processing thread
	m_hEvNuiProcessStop=CreateEvent(NULL,FALSE,FALSE,NULL);
	m_hThNuiProcess=CreateThread(NULL,0,Nui_ProcessThread,this,0,NULL);

	return hr;
}



void gkNuiInterface::Nui_UnInit( )
{
	::SelectObject( m_SkeletonDC, m_SkeletonOldObj );
	DeleteDC( m_SkeletonDC );
	DeleteObject( m_SkeletonBMP );

	if( m_Pen[0] != NULL )
	{
		DeleteObject(m_Pen[0]);
		DeleteObject(m_Pen[1]);
		DeleteObject(m_Pen[2]);
		DeleteObject(m_Pen[3]);
		DeleteObject(m_Pen[4]);
		DeleteObject(m_Pen[5]);
		ZeroMemory(m_Pen,sizeof(m_Pen));
	}

	// Stop the Nui processing thread
	if(m_hEvNuiProcessStop!=NULL)
	{
		// Signal the thread
		SetEvent(m_hEvNuiProcessStop);

		// Wait for thread to stop
		if(m_hThNuiProcess!=NULL)
		{
			WaitForSingleObject(m_hThNuiProcess,INFINITE);
			CloseHandle(m_hThNuiProcess);
		}
		CloseHandle(m_hEvNuiProcessStop);
	}

	if (m_pNuiInstance)
	{
		m_pNuiInstance->NuiShutdown( );
	}
	if( m_hNextSkeletonEvent && ( m_hNextSkeletonEvent != INVALID_HANDLE_VALUE ) )
	{
		CloseHandle( m_hNextSkeletonEvent );
		m_hNextSkeletonEvent = NULL;
	}
	if( m_hNextDepthFrameEvent && ( m_hNextDepthFrameEvent != INVALID_HANDLE_VALUE ) )
	{
		CloseHandle( m_hNextDepthFrameEvent );
		m_hNextDepthFrameEvent = NULL;
	}
	if( m_hNextVideoFrameEvent && ( m_hNextVideoFrameEvent != INVALID_HANDLE_VALUE ) )
	{
		CloseHandle( m_hNextVideoFrameEvent );
		m_hNextVideoFrameEvent = NULL;
	}
	//m_DrawDepth.DestroyDevice( );
	//m_DrawVideo.DestroyDevice( );
}



DWORD WINAPI gkNuiInterface::Nui_ProcessThread(LPVOID pParam)
{
	gkNuiInterface *pthis=(gkNuiInterface *) pParam;
	return pthis->Nui_ProcessThread();
}

DWORD WINAPI gkNuiInterface::Nui_ProcessThread()
{
	HANDLE                hEvents[4];
	int                    nEventIdx,t,dt;

	// Configure events to be listened on
	hEvents[0]= m_hEvNuiProcessStop;
	hEvents[1]= m_hNextDepthFrameEvent;
	hEvents[2]= m_hNextVideoFrameEvent;
	hEvents[3]= m_hNextSkeletonEvent;

#pragma warning(push)
#pragma warning(disable: 4127) // conditional expression is constant

	// Main thread loop
	while(1)
	{
		// Wait for an event to be signalled
		nEventIdx=WaitForMultipleObjects(sizeof(hEvents)/sizeof(hEvents[0]),hEvents,FALSE,100);

		// If the stop event, stop looping and exit
		if(nEventIdx==0)
			break;            

		// Perform FPS processing
		t = timeGetTime( );
		if( m_LastFPStime == -1 )
		{
			m_LastFPStime = t;
			m_LastFramesTotal = m_FramesTotal;
		}
		dt = t - m_LastFPStime;
		if( dt > 1000 )
		{
			m_LastFPStime = t;
			int FrameDelta = m_FramesTotal - m_LastFramesTotal;
			m_LastFramesTotal = m_FramesTotal;
			//::PostMessageW(m_hWnd, WM_USER_UPDATE_FPS, IDC_FPS, FrameDelta);
		}

		// Perform skeletal panel blanking
		if( m_LastSkeletonFoundTime == -1 )
			m_LastSkeletonFoundTime = t;
		dt = t - m_LastSkeletonFoundTime;
		if( dt > 250 )
		{
			if( !m_bScreenBlanked )
			{
				//Nui_BlankSkeletonScreen( GetDlgItem( m_hWnd, IDC_SKELETALVIEW ) );
				m_bScreenBlanked = true;
			}
		}

		// Process signal events
		switch(nEventIdx)
		{
		case 1:
			Nui_GotDepthAlert();
			m_FramesTotal++;
			break;

		case 2:
			Nui_GotVideoAlert();
			break;

		case 3:
			Nui_GotSkeletonAlert( );
			break;
		}
	}
#pragma warning(pop)

	return (0);
}

void gkNuiInterface::Nui_GotVideoAlert( )
{
	const NUI_IMAGE_FRAME * pImageFrame = NULL;

	HRESULT hr = m_pNuiInstance->NuiImageStreamGetNextFrame(
		m_pVideoStreamHandle,
		0,
		&pImageFrame );
	if( FAILED( hr ) )
	{
		return;
	}

	INuiFrameTexture * pTexture = pImageFrame->pFrameTexture;
	NUI_LOCKED_RECT LockedRect;
	pTexture->LockRect( 0, &LockedRect, NULL, 0 );
	if( LockedRect.Pitch != 0 )
	{
		BYTE * pBuffer = (BYTE*) LockedRect.pBits;

		//m_DrawVideo.DrawFrame( (BYTE*) pBuffer );

	}
	else
	{
		OutputDebugString( _T("Buffer length of received texture is bogus\r\n") );
	}

	m_pNuiInstance->NuiImageStreamReleaseFrame( m_pVideoStreamHandle, pImageFrame );
}


void gkNuiInterface::Nui_GotDepthAlert( )
{
	const NUI_IMAGE_FRAME * pImageFrame = NULL;

	HRESULT hr = m_pNuiInstance->NuiImageStreamGetNextFrame(
		m_pDepthStreamHandle,
		0,
		&pImageFrame );

	if( FAILED( hr ) )
	{
		return;
	}

	INuiFrameTexture * pTexture = pImageFrame->pFrameTexture;
	NUI_LOCKED_RECT LockedRect;
	pTexture->LockRect( 0, &LockedRect, NULL, 0 );
	if( LockedRect.Pitch != 0 )
	{
		BYTE * pBuffer = (BYTE*) LockedRect.pBits;

		// draw the bits to the bitmap
		RGBQUAD * rgbrun = m_rgbWk;
		USHORT * pBufferRun = (USHORT*) pBuffer;
		for( int y = 0 ; y < 240 ; y++ )
		{
			for( int x = 0 ; x < 320 ; x++ )
			{
				RGBQUAD quad = Nui_ShortToQuad_Depth( *pBufferRun );
				pBufferRun++;
				*rgbrun = quad;
				rgbrun++;
			}
		}

		//m_DrawDepth.DrawFrame( (BYTE*) m_rgbWk );
	}
	else
	{
		OutputDebugString( _T("Buffer length of received texture is bogus\r\n") );
	}

	m_pNuiInstance->NuiImageStreamReleaseFrame( m_pDepthStreamHandle, pImageFrame );
}



void gkNuiInterface::Nui_BlankSkeletonScreen(HWND hWnd)
{
	HDC hdc = GetDC( hWnd );
	RECT rct;
	GetClientRect(hWnd, &rct);
	int width = rct.right;
	int height = rct.bottom;
	PatBlt( hdc, 0, 0, width, height, BLACKNESS );
	ReleaseDC( hWnd, hdc );
}

void gkNuiInterface::Nui_DrawSkeletonSegment( NUI_SKELETON_DATA * pSkel, int numJoints, ... )
{
	va_list vl;
	va_start(vl,numJoints);

	POINT segmentPositions[NUI_SKELETON_POSITION_COUNT];
	int segmentPositionsCount = 0;

	DWORD polylinePointCounts[NUI_SKELETON_POSITION_COUNT];
	int numPolylines = 0;
	int currentPointCount = 0;

	// Note the loop condition: We intentionally run one iteration beyond the
	// last element in the joint list, so we can properly end the final polyline.

	for (int iJoint = 0; iJoint <= numJoints; iJoint++)
	{
		if (iJoint < numJoints)
		{
			NUI_SKELETON_POSITION_INDEX jointIndex = va_arg(vl,NUI_SKELETON_POSITION_INDEX);

			if (pSkel->eSkeletonPositionTrackingState[jointIndex] != NUI_SKELETON_POSITION_NOT_TRACKED)
			{
				// This joint is tracked: add it to the array of segment positions.

				segmentPositions[segmentPositionsCount].x = m_Points[jointIndex].x;
				segmentPositions[segmentPositionsCount].y = m_Points[jointIndex].y;
				segmentPositionsCount++;
				currentPointCount++;

				// Fully processed the current joint; move on to the next one

				continue;
			}
		}

		// If we fall through to here, we're either beyond the last joint, or
		// the current joint is not tracked: end the current polyline here.

		if (currentPointCount > 1)
		{
			// Current polyline already has at least two points: save the count.

			polylinePointCounts[numPolylines++] = currentPointCount;
		}
		else if (currentPointCount == 1)
		{
			// Current polyline has only one point: ignore it.

			segmentPositionsCount--;
		}
		currentPointCount = 0;
	}

#ifdef _DEBUG
	// We should end up with no more points in segmentPositions than the
	// original number of joints.

	assert(segmentPositionsCount <= numJoints);

	int totalPointCount = 0;
	for (int i = 0; i < numPolylines; i++)
	{
		// Each polyline should contain at least two points.

		assert(polylinePointCounts[i] > 1);

		totalPointCount += polylinePointCounts[i];
	}

	// Total number of points in all polylines should be the same as number
	// of points in segmentPositions.

	assert(totalPointCount == segmentPositionsCount);
#endif

	if (numPolylines > 0)
	{
		PolyPolyline(m_SkeletonDC, segmentPositions, polylinePointCounts, numPolylines);
	}

	va_end(vl);
}

void gkNuiInterface::Nui_DrawSkeleton( bool bBlank, NUI_SKELETON_DATA * pSkel, HWND hWnd, int WhichSkeletonColor )
{
	HGDIOBJ hOldObj = SelectObject(m_SkeletonDC,m_Pen[WhichSkeletonColor % m_PensTotal]);

	RECT rct;
	GetClientRect(hWnd, &rct);
	int width = rct.right;
	int height = rct.bottom;

	if( m_Pen[0] == NULL )
	{
		m_Pen[0] = CreatePen( PS_SOLID, width / 80, RGB(255, 0, 0) );
		m_Pen[1] = CreatePen( PS_SOLID, width / 80, RGB( 0, 255, 0 ) );
		m_Pen[2] = CreatePen( PS_SOLID, width / 80, RGB( 64, 255, 255 ) );
		m_Pen[3] = CreatePen( PS_SOLID, width / 80, RGB(255, 255, 64 ) );
		m_Pen[4] = CreatePen( PS_SOLID, width / 80, RGB( 255, 64, 255 ) );
		m_Pen[5] = CreatePen( PS_SOLID, width / 80, RGB( 128, 128, 255 ) );
	}

	if( bBlank )
	{
		PatBlt( m_SkeletonDC, 0, 0, width, height, BLACKNESS );
	}

	int scaleX = width; //scaling up to image coordinates
	int scaleY = height;
	float fx=0,fy=0;
	int i;
	for (i = 0; i < NUI_SKELETON_POSITION_COUNT; i++)
	{
		NuiTransformSkeletonToDepthImageF( pSkel->SkeletonPositions[i], &fx, &fy );
		m_Points[i].x = (int) ( fx * scaleX + 0.5f );
		m_Points[i].y = (int) ( fy * scaleY + 0.5f );
	}

	SelectObject(m_SkeletonDC,m_Pen[WhichSkeletonColor%m_PensTotal]);

	Nui_DrawSkeletonSegment(pSkel,4,NUI_SKELETON_POSITION_HIP_CENTER, NUI_SKELETON_POSITION_SPINE, NUI_SKELETON_POSITION_SHOULDER_CENTER, NUI_SKELETON_POSITION_HEAD);
	Nui_DrawSkeletonSegment(pSkel,5,NUI_SKELETON_POSITION_SHOULDER_CENTER, NUI_SKELETON_POSITION_SHOULDER_LEFT, NUI_SKELETON_POSITION_ELBOW_LEFT, NUI_SKELETON_POSITION_WRIST_LEFT, NUI_SKELETON_POSITION_HAND_LEFT);
	Nui_DrawSkeletonSegment(pSkel,5,NUI_SKELETON_POSITION_SHOULDER_CENTER, NUI_SKELETON_POSITION_SHOULDER_RIGHT, NUI_SKELETON_POSITION_ELBOW_RIGHT, NUI_SKELETON_POSITION_WRIST_RIGHT, NUI_SKELETON_POSITION_HAND_RIGHT);
	Nui_DrawSkeletonSegment(pSkel,5,NUI_SKELETON_POSITION_HIP_CENTER, NUI_SKELETON_POSITION_HIP_LEFT, NUI_SKELETON_POSITION_KNEE_LEFT, NUI_SKELETON_POSITION_ANKLE_LEFT, NUI_SKELETON_POSITION_FOOT_LEFT);
	Nui_DrawSkeletonSegment(pSkel,5,NUI_SKELETON_POSITION_HIP_CENTER, NUI_SKELETON_POSITION_HIP_RIGHT, NUI_SKELETON_POSITION_KNEE_RIGHT, NUI_SKELETON_POSITION_ANKLE_RIGHT, NUI_SKELETON_POSITION_FOOT_RIGHT);

	// Draw the joints in a different color
	for (i = 0; i < NUI_SKELETON_POSITION_COUNT ; i++)
	{
		if (pSkel->eSkeletonPositionTrackingState[i] != NUI_SKELETON_POSITION_NOT_TRACKED)
		{
			HPEN hJointPen;

			hJointPen=CreatePen(PS_SOLID,9, g_JointColorTable[i]);
			hOldObj=SelectObject(m_SkeletonDC,hJointPen);

			MoveToEx( m_SkeletonDC, m_Points[i].x, m_Points[i].y, NULL );
			LineTo( m_SkeletonDC, m_Points[i].x, m_Points[i].y );

			SelectObject( m_SkeletonDC, hOldObj );
			DeleteObject(hJointPen);
		}
	}

	return;

}




void gkNuiInterface::Nui_DoDoubleBuffer(HWND hWnd,HDC hDC)
{
	RECT rct;
	GetClientRect(hWnd, &rct);
	int width = rct.right;
	int height = rct.bottom;

	HDC hdc = GetDC( hWnd );

	BitBlt( hdc, 0, 0, width, height, hDC, 0, 0, SRCCOPY );

	ReleaseDC( hWnd, hdc );

}

void gkNuiInterface::Nui_GotSkeletonAlert( )
{
	NUI_SKELETON_FRAME SkeletonFrame;

	bool bFoundSkeleton = false;

	if( SUCCEEDED(m_pNuiInstance->NuiSkeletonGetNextFrame( 0, &SkeletonFrame )) )
	{
		for( int i = 0 ; i < NUI_SKELETON_COUNT ; i++ )
		{
			if( SkeletonFrame.SkeletonData[i].eTrackingState == NUI_SKELETON_TRACKED )
			{
				bFoundSkeleton = true;
			}
		}
	}

	// no skeletons!
	//
	if( !bFoundSkeleton )
	{
		return;
	}

	// smooth out the skeleton data
	m_pNuiInstance->NuiTransformSmooth(&SkeletonFrame,NULL);

	// we found a skeleton, re-start the timer
	m_bScreenBlanked = false;
	m_LastSkeletonFoundTime = -1;

	// draw each skeleton color according to the slot within they are found.
	//
	bool bBlank = true;
	for( int i = 0 ; i < NUI_SKELETON_COUNT ; i++ )
	{
		// Show skeleton only if it is tracked, and the center-shoulder joint is at least inferred.

		if( SkeletonFrame.SkeletonData[i].eTrackingState == NUI_SKELETON_TRACKED &&
			SkeletonFrame.SkeletonData[i].eSkeletonPositionTrackingState[NUI_SKELETON_POSITION_SHOULDER_CENTER] != NUI_SKELETON_POSITION_NOT_TRACKED)
		{
			//Nui_DrawSkeleton( bBlank, &SkeletonFrame.SkeletonData[i], GetDlgItem( m_hWnd, IDC_SKELETALVIEW ), i );
			bBlank = false;
		}
	}

	//Nui_DoDoubleBuffer(GetDlgItem(m_hWnd,IDC_SKELETALVIEW), m_SkeletonDC);
}



RGBQUAD gkNuiInterface::Nui_ShortToQuad_Depth( USHORT s )
{
	bool hasPlayerData = HasSkeletalEngine(m_pNuiInstance);
	USHORT RealDepth = hasPlayerData ? (s & 0xfff8) >> 3 : s & 0xffff;
	USHORT Player = hasPlayerData ? s & 7 : 0;

	// transform 13-bit depth information into an 8-bit intensity appropriate
	// for display (we disregard information in most significant bit)
	BYTE l = 255 - (BYTE)(256*RealDepth/0x0fff);

	RGBQUAD q;
	q.rgbRed = q.rgbBlue = q.rgbGreen = 0;

	switch( Player )
	{
	case 0:
		q.rgbRed = l / 2;
		q.rgbBlue = l / 2;
		q.rgbGreen = l / 2;
		break;
	case 1:
		q.rgbRed = l;
		break;
	case 2:
		q.rgbGreen = l;
		break;
	case 3:
		q.rgbRed = l / 4;
		q.rgbGreen = l;
		q.rgbBlue = l;
		break;
	case 4:
		q.rgbRed = l;
		q.rgbGreen = l;
		q.rgbBlue = l / 4;
		break;
	case 5:
		q.rgbRed = l;
		q.rgbGreen = l / 4;
		q.rgbBlue = l;
		break;
	case 6:
		q.rgbRed = l / 2;
		q.rgbGreen = l / 2;
		q.rgbBlue = l;
		break;
	case 7:
		q.rgbRed = 255 - ( l / 2 );
		q.rgbGreen = 255 - ( l / 2 );
		q.rgbBlue = 255 - ( l / 2 );
	}

	return q;
}
//////////////////////////////////////////////////////////////////////////
gkNuiInterface::gkNuiInterface()
{
	::InitializeCriticalSection(&m_critSecUi);
	m_fUpdatingUi = false;
	Nui_Zero();
}
//////////////////////////////////////////////////////////////////////////
gkNuiInterface::~gkNuiInterface()
{
	::DeleteCriticalSection(&m_critSecUi);
	Nui_Zero();
	::SysFreeString(m_instanceId);
}
