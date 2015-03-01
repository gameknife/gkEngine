#include "InputStableHeader.h"

#ifdef OS_IOS
#include "gkIOSInput.h"
#include "gkInputManager.h"



static SIOSInputDelegate g_IOSInputDelegate;
SIOSInputDelegate* global_getIOSInputDelegate()
{
    return &g_IOSInputDelegate;
}

gkIOSInput::gkIOSInput(gkInputManager& input)
{
	m_pManager = &input;
	m_deviceId = eDI_IOS;
}

bool gkIOSInput::Init()
{
	return true;
}


void gkIOSInput::Update( bool bFocus )
{
	std::vector<SIOSInputEvent>::iterator it = g_IOSInputDelegate.m_actionQueue.begin();
    
	// Post Input events.
	SInputEvent event;
	event.deviceId = m_deviceId;
	event.deviceIndex = 0;
    
    
    Vec2 m_posThisFrame[GK_IOS_MAX_TOUCHES];
    memset( &m_posThisFrame, 0, sizeof(Vec2) * GK_IOS_MAX_TOUCHES );
    
	for (; it != g_IOSInputDelegate.m_actionQueue.end(); ++it)
	{
		event.deviceIndex = it->deviceIdx;
        
        switch( it->motionState )
        {
            case eIES_IOSstart:
                event.state = eIS_Pressed;
                break;
            case eIES_IOSchange:
                event.state = eIS_Changed;
                break;
            case eIES_IOSend:
                event.state = eIS_Released;
                break;
        }
        
        // 处理touch view队列中的信息
        
		if ( it->motionType == eIET_finger )
		{
			event.keyId = eKI_Android_Touch;
            
			event.value = it->asyncPos.x;
			event.value2 = it->asyncPos.y;
			
            if(it->motionState == eIES_IOSstart)
            {
                
                
            }
            
            switch( it->motionState )
            {
                case eIES_IOSstart:
                    m_posPrevious[it->deviceIdx] = it->asyncPos;
                case eIES_IOSend:
                    m_pManager->PostInputEvent(event);
                    break;
                case eIES_IOSchange:
                    m_posThisFrame[it->deviceIdx].x = it->asyncPos.x;
                    m_posThisFrame[it->deviceIdx].y = it->asyncPos.y;
                    break;
            }
            
		}
        else if( it->motionType == eIET_ges_zoom )
        {
            event.keyId = eKI_Gesture_Zoom;
			event.value = it->asyncPos.x; // value mean delta
            
			m_pManager->PostInputEvent(event);
        }

	}
    
    
	for (int i=0; i < GK_IOS_MAX_TOUCHES; ++i)
	{
		m_posDelta[i].set(0,0);
		if ( !(m_posThisFrame[i].IsZero(0.1f) ))
		{
			m_posDelta[i] = m_posThisFrame[i] - m_posPrevious[i];
			m_posPrevious[i] = m_posThisFrame[i];
            
			SInputEvent eventX;
			eventX.deviceId = m_deviceId;
			eventX.deviceIndex = i;
			eventX.keyId = eKI_Android_DragX;
			eventX.state = eIS_Changed;
			eventX.value = m_posDelta[i].x; // value mean delta
			eventX.value2 = m_posPrevious[i].x; // value2 mean actual pos
            
			m_pManager->PostInputEvent(eventX);
            
			SInputEvent eventY;
			eventY.deviceId = m_deviceId;
			eventY.deviceIndex = i;
			eventY.keyId = eKI_Android_DragY;
			eventY.state = eIS_Changed;
			eventY.value = m_posDelta[i].y;
			eventY.value2 = m_posPrevious[i].y; // value2 mean actual pos
            
			m_pManager->PostInputEvent(eventY);
		}
	}
    
	g_IOSInputDelegate.clearEvent();
}

bool gkIOSInput::SetExclusiveMode( bool value )
{
	return true;
}

void gkIOSInput::ClearKeyState()
{

}

const char* gkIOSInput::GetKeyName( const SInputEvent& event, bool bGUI/*=0*/ )
{
	return 0;
}

const TCHAR* gkIOSInput::GetDeviceName() const
{
	return _T("IOS");
}

EDeviceId gkIOSInput::GetDeviceId() const
{
	return m_deviceId;
}

void gkIOSInput::PostInit()
{
	
}

void gkIOSInput::ClearAnalogKeyState()
{
	
}

SInputSymbol* gkIOSInput::LookupSymbol( EKeyId id ) const
{
	return NULL;
}

void gkIOSInput::Enable( bool enable )
{
	
}

bool gkIOSInput::IsEnabled() const
{
	return true;
}

#endif