#include "InputStableHeader.h"

#ifdef OS_ANDROID
#include "gkAndroidInput.h"
#include "gkInputManager.h"


struct SAndroidInputEvent
{
	uint8	motionType;
	uint8	deviceIdx;
	Vec2	asyncPos;

	SAndroidInputEvent()
	{
		motionType = AMOTION_EVENT_ACTION_OUTSIDE;
		deviceIdx = -1;
		asyncPos.set(0,0);
	}
};
std::vector<SAndroidInputEvent> g_actionQueue;

SAndroidInputEvent g_devicePos[ANDROID_MAX_MULTITOUCH];

/*****************************************************************************
 Global code
*****************************************************************************/
static int32_t androidhandle_input(struct android_app* app, AInputEvent* event)
{
	//////////////////////////////////////////////////////////////////////////
	// full version support multi-touch

	//////////////////////////////////////////////////////////////////////////
	// input source
	//		int nSourceId		= AInputEvent_getSource( event );
	// 		if( nSourceId == AINPUT_SOURCE_TOUCHPAD )
	// 			touchstate = engine->touchstate_pad;
	// 		else
	// 			touchstate = engine->touchstate_screen;


	//////////////////////////////////////////////////////////////////////////
	// action info
	int actionEvent		= AMotionEvent_getAction( event );
	int nPointerId	= AMotionEvent_getPointerId( event, 0 );
	int nAction	 = AMOTION_EVENT_ACTION_MASK & actionEvent;

	//////////////////////////////////////////////////////////////////////////
	// 1. step one, handle down & up event

	SAndroidInputEvent aiEvent;

	// if only on finger, index:0
	int nPointerIndex = 0;
	aiEvent.deviceIdx = nPointerId;

	// if multi-finger touch, reset PointerIndex and deviceIdx
  	if( nAction == AMOTION_EVENT_ACTION_POINTER_DOWN || nAction == AMOTION_EVENT_ACTION_POINTER_UP )
  	{
 		nPointerIndex = (actionEvent & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
 		nPointerId = AMotionEvent_getPointerId( event, nPointerIndex );
 		aiEvent.deviceIdx = nPointerId;
  	}

	if (aiEvent.deviceIdx >= ANDROID_MAX_MULTITOUCH)
	{
		return 1;
	}

	// action judge
	bool flag = 1;
	if( nAction == AMOTION_EVENT_ACTION_DOWN || nAction == AMOTION_EVENT_ACTION_POINTER_DOWN )
	{
		aiEvent.motionType = AMOTION_EVENT_ACTION_DOWN;
		g_devicePos[aiEvent.deviceIdx].motionType = AMOTION_EVENT_ACTION_DOWN;
		aiEvent.asyncPos.x = AMotionEvent_getX( event, nPointerIndex );
		aiEvent.asyncPos.y = AMotionEvent_getY( event, nPointerIndex );

		// reset device pos
		g_devicePos[aiEvent.deviceIdx].asyncPos = aiEvent.asyncPos;
	}
	else if( nAction == AMOTION_EVENT_ACTION_UP || nAction == AMOTION_EVENT_ACTION_POINTER_UP || nAction == AMOTION_EVENT_ACTION_CANCEL )
	{
		aiEvent.motionType = AMOTION_EVENT_ACTION_UP;
		g_devicePos[aiEvent.deviceIdx].motionType = AMOTION_EVENT_ACTION_UP;
		aiEvent.asyncPos.x = AMotionEvent_getX( event, nPointerIndex );
		aiEvent.asyncPos.y = AMotionEvent_getY( event, nPointerIndex );
	}
	else
	{
		// unknown action
		flag = 0;
	}

	//////////////////////////////////////////////////////////////////////////
	// 2. handle move event
	int nPointerCount	= AMotionEvent_getPointerCount( event );

	for ( int i=0; i < nPointerCount; ++i)
	{
		int nPointerId	= AMotionEvent_getPointerId( event, i );
		if ( g_devicePos[nPointerId].motionType == AMOTION_EVENT_ACTION_DOWN && nAction == AMOTION_EVENT_ACTION_MOVE  )
		{
			//aiEvent.motionType = AMOTION_EVENT_ACTION_MOVE;
			g_devicePos[nPointerId].asyncPos.x = AMotionEvent_getX( event, i );
			g_devicePos[nPointerId].asyncPos.y = AMotionEvent_getY( event, i );
		}
	}


		
	if (flag)
	{
		g_actionQueue.push_back(aiEvent);
	}
		
	
	return 1;

}




gkAndroidInput::gkAndroidInput(gkInputManager& input)
{
	m_pManager = &input;
	m_deviceId = eDI_Android;

	memset( &( m_posPrevious[0] ), 0, sizeof(Vec2) * ANDROID_MAX_MULTITOUCH );
	memset( &( m_posDelta[0] ), 0, sizeof(Vec2) * ANDROID_MAX_MULTITOUCH );
}

bool gkAndroidInput::Init()
{
	return true;
}


void gkAndroidInput::Update( bool bFocus )
{
	//gkLogMessage(_T("android input updating..."));
	std::vector<SAndroidInputEvent>::iterator it = g_actionQueue.begin();

	// Post Input events.
	SInputEvent event;
	event.deviceId = m_deviceId;
	event.deviceIndex = 0;

	for (; it != g_actionQueue.end(); ++it)
	{
		event.deviceIndex = it->deviceIdx;

		if ( it->motionType == AMOTION_EVENT_ACTION_DOWN )
		{
			event.keyId = eKI_Android_Touch;
			event.state = eIS_Pressed;

			event.value = it->asyncPos.x;
			event.value2 = it->asyncPos.y;
			
			m_pManager->PostInputEvent(event);

			m_posPrevious[it->deviceIdx] = it->asyncPos;
		}
		else if ( it->motionType == AMOTION_EVENT_ACTION_UP )
		{
			event.keyId = eKI_Android_Touch;
			event.state = eIS_Released;

			event.value = it->asyncPos.x;
			event.value2 = it->asyncPos.y;

			m_pManager->PostInputEvent(event);
		}
		else if ( it->motionType == AMOTION_EVENT_ACTION_MOVE )
		{

		}
	}


	for (int i=0; i < ANDROID_MAX_MULTITOUCH; ++i)
	{
		m_posDelta[i].set(0,0);
		if ( g_devicePos[i].motionType == AMOTION_EVENT_ACTION_DOWN )
		{
			m_posDelta[i] = g_devicePos[i].asyncPos - m_posPrevious[i];
			m_posPrevious[i] = g_devicePos[i].asyncPos;

			SInputEvent eventX;
			eventX.deviceId = m_deviceId;
			eventX.deviceIndex = i;
			eventX.keyId = eKI_Android_DragX;
			eventX.state = eIS_Changed;
			eventX.value = m_posDelta[i].x; // value mean delta
			eventX.value2 = g_devicePos[i].asyncPos.x; // value2 mean actual pos

			m_pManager->PostInputEvent(eventX);

			SInputEvent eventY;
			eventY.deviceId = m_deviceId;
			eventY.deviceIndex = i;
			eventY.keyId = eKI_Android_DragY;
			eventY.state = eIS_Changed;
			eventY.value = m_posDelta[i].y;
			eventY.value2 = g_devicePos[i].asyncPos.y; // value2 mean actual pos

			m_pManager->PostInputEvent(eventY);
		}
	}


	g_actionQueue.clear();
}

bool gkAndroidInput::SetExclusiveMode( bool value )
{
	return true;
}

void gkAndroidInput::ClearKeyState()
{

}

const char* gkAndroidInput::GetKeyName( const SInputEvent& event, bool bGUI/*=0*/ )
{
	
}

Android_InputHandler gkAndroidInput::getAndroidHandler()
{
	return androidhandle_input;
}

const TCHAR* gkAndroidInput::GetDeviceName() const
{
	return _T("android");
}

EDeviceId gkAndroidInput::GetDeviceId() const
{
	return m_deviceId;
}

void gkAndroidInput::PostInit()
{
	
}

void gkAndroidInput::ClearAnalogKeyState()
{
	
}

SInputSymbol* gkAndroidInput::LookupSymbol( EKeyId id ) const
{
	return NULL;
}

void gkAndroidInput::Enable( bool enable )
{
	
}

bool gkAndroidInput::IsEnabled() const
{
	return true;
}
#endif
