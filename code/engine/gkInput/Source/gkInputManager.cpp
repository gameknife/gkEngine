#include "InputStableHeader.h"
#include "gkInputManager.h"
#include "ISystem.h"

#ifdef OS_WIN32
#include "gkKeyboard.h"
#include "gkMouse.h"
#include "gkXboxController.h"
#elif defined ( OS_ANDROID )
#include "gkAndroidInput.h"
#elif defined (OS_IOS)
#include "gkIOSInput.h"
#endif

#include "gkMemoryLeakDetecter.h"



gkInputCVars* g_pInputCVars=0;

gkInputCVars::gkInputCVars()
{
	REGISTER_CVARINT_DESC(i_debug, 
		_T("Toggles input event debugging.\n")
		_T("Usage: i_debug [0/1]\n")
		_T("Default is 0 (off). Set to 1 to spam console with key events (only press and release)."), 0);
	REGISTER_CVARINT_DESC(i_forcefeedback, _T("Enable/Disable force feedback output."), 0);

	// mouse
	REGISTER_CVARINT_DESC(i_mouse_buffered, 
		_T("Toggles mouse input buffering.\n")
		_T("Usage: i_mouse_buffered [0/1]\n")
		_T("Default is 0 (off). Set to 1 to process buffered mouse input."),
		0);
	REGISTER_CVARFLOAT_DESC(i_mouse_accel, 
		_T("Set mouse acceleration, 0.0 means no acceleration.\n")
		_T("Usage: i_mouse_accel [float number] (usually a small number, 0.1 is a good one)\n")
		_T("Default is 0.0 (off)"), 0.0, 1.0, 0.0);
	REGISTER_CVARFLOAT_DESC(i_mouse_accel_max, 
		_T("Set mouse max mouse delta when using acceleration.\n")
		_T("Usage: i_mouse_accel_max [float number]\n")
		_T("Default is 100.0"), 0.0, 100.0, 100.0);	
	REGISTER_CVARFLOAT_DESC(i_mouse_smooth,
		_T("Set mouse smoothing value, also if 0 (disabled) there will be a simple average\n")
		_T("between the old and the actual input.\n")
		_T("Usage: i_mouse_smooth [float number]\n")
		_T("(1.0 = very very smooth, 30 = almost instant)\n")
		_T("Default is 0.0"), 0.0, 10.0, 0.0);	
	REGISTER_CVARFLOAT_DESC(i_mouse_inertia, 
		_T("Set mouse inertia. It is disabled (0.0) by default.\n")
		_T("Usage: i_mouse_inertia [float number]\n")
		_T("Default is 0.0"), 0.0, 1.0, 0.0);

	// keyboard
	REGISTER_CVARINT_DESC(i_bufferedkeys,
		_T("Toggles key buffering.\n")
		_T("Usage: i_bufferedkeys [0/1]\n")
		_T("Default is 0 (off). Set to 1 to process buffered key strokes."), 0);
}

gkInputCVars::~gkInputCVars()
{

}

//-----------------------------------------------------------------------
gkInputManager::gkInputManager( void ) 
{
#ifdef OS_WIN32
	m_pDI = NULL;
	m_pMouse = NULL;
#endif
	m_retriggering = false;
	m_exclusiveListener = NULL;
	m_globalModifier = eMM_None;

	m_hWnd = 0;
	m_pInputCVars = NULL;

	m_holdSymbols.reserve(64);
}

//-----------------------------------------------------------------------
gkInputManager::~gkInputManager( void )
{

}

//-----------------------------------------------------------------------
void gkInputManager::Update( float fElapsedTime)
{
	//gkLogMessage(_T("input mng updating..."));

 	if (gEnv->pSystem->IsEditor())
 	{
// 		POINT pos;
// 		GetCursorPos(&pos);
// 		if (pos.x < 0 || pos.y < 0x0132)
// 			return;

	}
	else
	{
#ifdef OS_WIN32
		//如果不是焦点，不交互
		 if(GetFocus() !=  m_hWnd)
			 return;
#endif
	}

	PostHoldEvents();

	//gkLogMessage(_T("input mng updating..."));
	for (TInputDevices::iterator i = m_inputDevices.begin(); i != m_inputDevices.end(); ++i)
	{
		if ((*i)->IsEnabled())
			(*i)->Update(false);
	}


	//////////////////////////////////////////////////////////////////////////

}

//-----------------------------------------------------------------------------
// Name: InitDirectInput()
// Desc: Initialize the DirectInput variables.
//-----------------------------------------------------------------------------
HRESULT gkInputManager::Init( HWND hDlg )
{
	HRESULT hr;

	m_pInputCVars = new gkInputCVars;
	g_pInputCVars = m_pInputCVars;

	m_hWnd = hDlg;

#ifdef OS_WIN32
	// Register with the DirectInput subsystem and get a pointer
	// to a IDirectInput interface we can use.
	if( FAILED( hr = DirectInput8Create( GetModuleHandle( NULL ), DIRECTINPUT_VERSION,
		IID_IDirectInput8, ( VOID** )&m_pDI, NULL ) ) )
		return hr;

	// add keyboard and mouse
	if (!AddInputDevice(new gkKeyboard(*this))) return S_FALSE;
	if (!AddInputDevice(new gkMouse(*this))) return S_FALSE;


	// add xinput controllers devices
	for (int i= 0; i < 4; ++i)
	{
		if (!AddInputDevice(new gkXboxInputDevice(*this, i))) return S_FALSE;
	}

	// add kinect [5/8/2012 Kaiming]
	// add kinect controller

	//if (!AddInputDevice(new gkKinect(*this))) return false;
#elif defined ( OS_ANDROID )

	if (!AddInputDevice(new gkAndroidInput(*this))) return S_FALSE;

#elif defined ( OS_IOS )
    
    if (!AddInputDevice(new gkIOSInput(*this))) return S_FALSE;
    
#endif
	ClearKeyState();

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: FreeDirectInput()
// Desc: Initialize the DirectInput variables.
//-----------------------------------------------------------------------------
void gkInputManager::Destroy()
{
	//gkLogMessage(_T("input mng updating..."));
	for (TInputDevices::iterator i = m_inputDevices.begin(); i != m_inputDevices.end(); ++i)
	{
		delete (*i);
	}

#ifdef OS_WIN32
	// Unacquire the device one last time just in case 
	// the app tried to exit while the device is still acquired.
	if ( m_pDI )
	{
		uint32 ref = m_pDI->Release();
		if (ref > 0)
		{
			gkLogError( _T("DirectInput REFCOUNT not ZERO!") );
		}
	}
#endif

	delete m_pInputCVars;
}


void gkInputManager::ClearKeyState()
{
// 	if (g_pInputCVars->i_debug)
// 	{
// 		gEnv->pLog->Log("InputDebug: ClearKeyState");
// 	}
	m_globalModifier = 0;
	for (TInputDevices::iterator i = m_inputDevices.begin(); i != m_inputDevices.end(); ++i)
	{
		(*i)->ClearKeyState();
	}
	m_retriggering = false;

	m_holdSymbols.clear();
// 	if (g_pInputCVars->i_debug)
// 	{
// 		gEnv->pLog->Log("InputDebug: ~ClearKeyState");
// 	}
}

SInputSymbol* gkInputManager::LookupSymbol( EDeviceId deviceId, EKeyId keyId )
{
	for (TInputDevices::iterator i = m_inputDevices.begin(); i != m_inputDevices.end(); ++i)
	{
		IInputDevice *pDevice = *i;
		if (pDevice->GetDeviceId() == deviceId)
			return pDevice->LookupSymbol(keyId);
	}

	// if no symbol found try finding it in any device
	for (TInputDevices::iterator i = m_inputDevices.begin(); i != m_inputDevices.end(); ++i)
	{
		IInputDevice *pDevice = *i;
		SInputSymbol *pSym = pDevice->LookupSymbol(keyId);
		if (pSym)
			return pSym;
	}

	return NULL;
}

bool gkInputManager::AddInputDevice(IInputDevice* pDevice)
{
	if (pDevice)
	{
		if (pDevice->Init())
		{
			m_inputDevices.push_back(pDevice);
			return true;
		}
		delete pDevice;
	}
	return false;
}

void gkInputManager::addInputEventListener( IInputEventListener* listener )
{
	m_listeners.push_back(listener);
}

void gkInputManager::removeEventListener( IInputEventListener* listener )
{
	TInputEventListeners::iterator it = std::find(m_listeners.begin(),m_listeners.end(),listener);
	if (it != m_listeners.end()) m_listeners.erase( it );
}

void gkInputManager::PostHoldEvents()
{
	SInputEvent event;

	const int count = (int)m_holdSymbols.size();
	for (int i=0; i < count; ++i)
	{
		m_holdSymbols[i]->AssignTo(event, GetModifiers());
		event.deviceIndex = m_holdSymbols[i]->deviceIndex;
		PostInputEvent(event);
	}
}


void gkInputManager::RetriggerKeyState()
{
	if (g_pInputCVars->i_debug)
	{
		//gEnv->pLog->Log("InputDebug: RetriggerKeyState");
	}

	m_retriggering = true;
	SInputEvent event;

	const int count = (int)m_holdSymbols.size();
	for (int i=0; i < count; ++i)
	{
		EInputState oldState = m_holdSymbols[i]->state;
		m_holdSymbols[i]->state = eIS_Pressed;
		m_holdSymbols[i]->AssignTo(event, GetModifiers());
		event.deviceIndex = m_holdSymbols[i]->deviceIndex;
		PostInputEvent(event);
		m_holdSymbols[i]->state = oldState;
	}
	m_retriggering = false;
	if (g_pInputCVars->i_debug)
	{
		//gEnv->pLog->Log("InputDebug: ~RetriggerKeyState");
	}
}

void gkInputManager::PostInputEvent( const SInputEvent &event, bool bForce )
{
// 	if(!bForce && !m_enableEventPosting) 
// 	{
// 		return;
// 	}

	if(event.keyId == eKI_Unknown && event.state != eIS_UI)
	{
		return;
	}

// 	if (g_pInputCVars->i_debug)
// 	{
// 		// log out key press and release events
// 		if (event.state == eIS_Pressed || event.state == eIS_Released)
// 			gEnv->pLog->Log("InputDebug: '%s' - %s", event.keyName.c_str(), event.state == eIS_Pressed?"pressed":"released");
// 	}

	// console listeners get to process the event first
// 	for (TInputEventListeners::const_iterator it = m_consoleListeners.begin(); it != m_consoleListeners.end(); ++it)
// 	{
// 		bool ret = false;
// 		if (event.state != eIS_UI)
// 			ret = (*it)->OnInputEvent(event);
// 		else
// 			ret = (*it)->OnInputEventUI(event);
// 
// 		if (ret)
// 			return;
// 	}

	// exclusive listener can filter out the event if he wants to and cause this call to return
	// before any of the regular listeners get to process it
	if (m_exclusiveListener)
	{ 
		bool ret = false;
		if (event.state != eIS_UI)
			ret = m_exclusiveListener->OnInputEvent(event);
		else
			ret = m_exclusiveListener->OnInputEventUI(event);

		if (ret)
			return;
	}

	// Send this event to all listeners.
	for (TInputEventListeners::const_iterator it = m_listeners.begin(); it != m_listeners.end(); ++it)
	{
		assert(*it);
		if (*it == NULL)
			continue;

		bool ret = false;
		if (event.state != eIS_UI)
			ret = (*it)->OnInputEvent(event);
		else
			ret = (*it)->OnInputEventUI(event);

		if (ret) break;
	}


	// if it's a keypress, then add it to the "hold" list
	if (!m_retriggering)
	{
		if (event.pSymbol && event.pSymbol->state == eIS_Pressed)
		{
			event.pSymbol->state = eIS_Down;
			//cache device index in the hold symbol
			event.pSymbol->deviceIndex = event.deviceIndex;
			m_holdSymbols.push_back(event.pSymbol);
		}
		else if (event.pSymbol && event.pSymbol->state == eIS_Released && !m_holdSymbols.empty())
		{
			// remove hold key
			int slot = -1;
			int last = m_holdSymbols.size()-1;

			for (int i=last; i>=0; --i)
			{
				if (m_holdSymbols[i] == event.pSymbol)
				{
					slot = i;
					break;
				}
			}
			if (slot != -1)
			{
				// swap last and found symbol
				m_holdSymbols[slot] = m_holdSymbols[last];
				// pop last ... which is now the one we want to get rid of
				m_holdSymbols.pop_back();
			}
		}
	}

}

void gkInputManager::SetExclusiveMode(EDeviceId deviceId, bool exclusive, void *pUser)
{
// 	if (g_pInputCVars->i_debug)
// 	{
// 		gEnv->pLog->Log("InputDebug: SetExclusiveMode(%d, %s)", deviceId, exclusive?"true":"false");
// 	}

	if(deviceId < (int)m_inputDevices.size())
	{
		m_inputDevices[deviceId]->ClearKeyState();
		m_inputDevices[deviceId]->SetExclusiveMode(exclusive);

		// try to flush the device ... perform a dry update
		//EnableEventPosting(false);
		m_inputDevices[deviceId]->Update(false);
		//EnableEventPosting(true);
	}
}

const char* gkInputManager::GetKeyName(const SInputEvent& event, bool bGUI)
{
	for (TInputDevices::iterator i = m_inputDevices.begin(); i != m_inputDevices.end(); ++i)
	{
		const char* ret = (*i)->GetKeyName(event, bGUI);
		if (ret) return ret;
	}

	return "";
}

void gkInputManager::setHWND(HWND hWnd)
{
	m_hWnd = hWnd;

	for (TInputDevices::iterator i = m_inputDevices.begin(); i != m_inputDevices.end(); ++i)
	{
		(*i)->SetHwnd( hWnd );
	}
}

#ifdef OS_ANDROID	
Android_InputHandler gkInputManager::getAndroidHandler()
{
	for (TInputDevices::iterator i = m_inputDevices.begin(); i != m_inputDevices.end(); ++i)
	{
		if( (*i)->GetDeviceId() == eDI_Android )
			return (*i)->getAndroidHandler();
	}
}



#endif
