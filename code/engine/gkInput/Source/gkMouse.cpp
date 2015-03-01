#include "InputStableHeader.h"
#include "gkMouse.h"
#include "ITimer.h"
#include "IRenderer.h"

#if 1
SInputSymbol*	gkMouse::Symbol[MAX_MOUSE_SYMBOLS] = {0};

// define a few internal ids
#define _DIMOFS_WHEEL_UP		(DIMOFS_BUTTON7 + 1)
#define _DIMOFS_WHEEL_DOWN	(DIMOFS_BUTTON7 + 2)

gkMouse::gkMouse(gkInputManager& input):
gkInputDevice(input, _T("mouse"), GUID_SysMouse)
{
	m_deviceId = eDI_Mouse;
}

///////////////////////////////////////////
bool gkMouse::Init()
{
	if (!CreateDirectInputDevice(&c_dfDIMouse2, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND, 4096))
		return false;
	
	// Set mouse axis mode.
	DIPROPDWORD dipdw = {{sizeof(DIPROPDWORD), sizeof(DIPROPHEADER), 0, DIPH_DEVICE}, DIPROPAXISMODE_REL};

	HRESULT hr = GetDirectInputDevice()->SetProperty( DIPROP_AXISMODE,&dipdw.diph );
	if  (FAILED(hr))
	{
		gkLogError( _T("CMouse::Init SetProperty DIPROP_AXISMODE failed.") );
		return false;
	}

	gkLogMessage( _T("InputDevice [ Mouse ] Initialized.\n") );
	

	m_deltas.zero();
	m_oldDeltas.zero();
	m_deltasInertia.zero();
	m_mouseWheel = 0.0f;
	
	Acquire();

	// key mapping
	Symbol[eKI_Mouse1-KI_MOUSE_BASE] = MapSymbol(DIMOFS_BUTTON0, eKI_Mouse1);
	Symbol[eKI_Mouse2-KI_MOUSE_BASE] = MapSymbol(DIMOFS_BUTTON1, eKI_Mouse2);
	Symbol[eKI_Mouse3-KI_MOUSE_BASE] = MapSymbol(DIMOFS_BUTTON2, eKI_Mouse3);
	Symbol[eKI_Mouse4-KI_MOUSE_BASE] = MapSymbol(DIMOFS_BUTTON3, eKI_Mouse4);
	Symbol[eKI_Mouse5-KI_MOUSE_BASE] = MapSymbol(DIMOFS_BUTTON4, eKI_Mouse5);
	Symbol[eKI_Mouse6-KI_MOUSE_BASE] = MapSymbol(DIMOFS_BUTTON5, eKI_Mouse6);
	Symbol[eKI_Mouse7-KI_MOUSE_BASE] = MapSymbol(DIMOFS_BUTTON6, eKI_Mouse7);
	Symbol[eKI_Mouse8-KI_MOUSE_BASE] = MapSymbol(DIMOFS_BUTTON7, eKI_Mouse8);

	Symbol[eKI_MouseWheelUp-KI_MOUSE_BASE] = MapSymbol(_DIMOFS_WHEEL_UP, eKI_MouseWheelUp);
	Symbol[eKI_MouseWheelDown-KI_MOUSE_BASE] = MapSymbol(_DIMOFS_WHEEL_DOWN, eKI_MouseWheelDown);
	Symbol[eKI_MouseX-KI_MOUSE_BASE] = MapSymbol(DIMOFS_X, eKI_MouseX, SInputSymbol::RawAxis);
	Symbol[eKI_MouseY-KI_MOUSE_BASE] = MapSymbol(DIMOFS_Y, eKI_MouseY, SInputSymbol::RawAxis);
	Symbol[eKI_MouseZ-KI_MOUSE_BASE] = MapSymbol(DIMOFS_Z, eKI_MouseZ, SInputSymbol::RawAxis);

	return true;
}

///////////////////////////////////////////
void gkMouse::Update(bool bFocus)
{
	HRESULT hr;
	
	if (!GetDirectInputDevice()) 
		return;

	m_deltas.zero();
	m_mouseWheel = 0;

	SInputSymbol* pSymbol = 0;
	EInputState newState;

	if (g_pInputCVars->i_mouse_buffered)
	{
		//memcpy(m_oldEvents, m_Events, sizeof(m_Events));
		//memset(m_Events,0,sizeof(m_Events));

		DIDEVICEOBJECTDATA Events[200];
		// // Buffer mouse input.
		DWORD nElements = 200;
		while (nElements > 0)
		{
			hr = GetDirectInputDevice()->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), Events, &nElements, 0);

			if (FAILED(hr) && Acquire())
			{
				// try again
				hr = GetDirectInputDevice()->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), Events, &nElements, 0);
			}
			
			if (FAILED(hr))
				return;

			if (nElements > 0)
			{
				for (int i = 0; i < (int)nElements; i++)
				{
					pSymbol = DevSpecIdToSymbol(Events[i].dwOfs);
					if (pSymbol)
					{
						// marcok: should actually process axis events here as` well,
						// but I'm not sure if the game can handle multiple move events

						if (pSymbol->type == SInputSymbol::Button)
						{
							if (Events[i].dwData&0x80)
							{
								pSymbol->state = eIS_Pressed;
								pSymbol->value = 1.0;
							}
							else
							{
								pSymbol->state = eIS_Released;
								pSymbol->value = 0.0;
							}
							PostEvent(pSymbol);
						}
					}
					switch (Events[i].dwOfs) 
					{
					case DIMOFS_X: 
						m_deltas.x += float((int)Events[i].dwData);
						break;
					case DIMOFS_Y: 
						m_deltas.y += float((int)Events[i].dwData);
						break; 
					case DIMOFS_Z:
						m_mouseWheel += float((int)Events[i].dwData);
						break;
					}
				}
			}
		}
	}
 	else
 	{
 		// Not buffered.
 		DIMOUSESTATE2 dims;
 		memset (&dims, 0, sizeof(dims));
 
 		hr = GetDirectInputDevice()->GetDeviceState(sizeof(DIMOUSESTATE2), &dims);	
 
 		if (FAILED(hr) && Acquire())
 		{
 			// try again
 			hr = GetDirectInputDevice()->GetDeviceState(sizeof(DIMOUSESTATE2), &dims);
 		}
 
 		if (SUCCEEDED(hr))
 		{
 			m_deltas.set((float)dims.lX, (float)dims.lY);
 			m_mouseWheel = (float)dims.lZ;
 		
			// mouse1 - mouse8
 			for (int i=0; i<8; ++i)
 			{
 				newState = (dims.rgbButtons[i]&0x80) ? eIS_Pressed : eIS_Released;
 				PostOnlyIfChanged(Symbol[i], newState);
 			}
 		}		
 	}

	//marcok: here the raw input already gets cooked a bit ... should be moved
	float mouseaccel = g_pInputCVars->i_mouse_accel;

	if (mouseaccel>0.0f)
	{
		m_deltas.x = m_deltas.x * (float)fabs(m_deltas.x * mouseaccel);
		m_deltas.y = m_deltas.y * (float)fabs(m_deltas.y * mouseaccel);

		CapDeltas(g_pInputCVars->i_mouse_accel_max);
	}

	SmoothDeltas(g_pInputCVars->i_mouse_smooth);

	//mouse wheel
	newState = (m_mouseWheel > 0.0f) ? eIS_Pressed : eIS_Released;
	PostOnlyIfChanged(Symbol[eKI_MouseWheelUp-KI_MOUSE_BASE], newState);

	newState = (m_mouseWheel < 0.0f) ? eIS_Pressed : eIS_Released;
	PostOnlyIfChanged(Symbol[eKI_MouseWheelDown-KI_MOUSE_BASE], newState);

	// mouse movements
	if (m_deltas.GetLength2()>0.0f || m_mouseWheel)
	{
		POINT point;
		GetCursorPos( &point );
		ScreenToClient( gEnv->pRenderer->GetWindowHwnd(), &point );

		Vec2i offset = gEnv->pRenderer->GetWindowOffset();


		pSymbol = Symbol[eKI_MouseX-KI_MOUSE_BASE];
		pSymbol->state = eIS_Changed;
		pSymbol->value = m_deltas.x;
		pSymbol->value2 = point.x - offset.x;
		PostEvent(pSymbol);

		pSymbol = Symbol[eKI_MouseY-KI_MOUSE_BASE];
		pSymbol->state = eIS_Changed;
		pSymbol->value = m_deltas.y;
		pSymbol->value2 = point.y - offset.y;
		PostEvent(pSymbol);

		pSymbol = Symbol[eKI_MouseZ-KI_MOUSE_BASE];
		pSymbol->state = eIS_Changed;
		pSymbol->value = m_mouseWheel;
		PostEvent(pSymbol);
	}

	//////////////////////////////////////////////////////////////////////////
	float inertia = g_pInputCVars->i_mouse_inertia;

	// mouse inertia
	if (inertia>0) 
	{
		float dt = gEnv->pTimer->GetFrameTime();
		if (dt>0.1f) dt=0.1f;
		m_deltas = (m_deltasInertia += (m_deltas-m_deltasInertia)*inertia*dt);
	}
}

///////////////////////////////////////////
bool gkMouse::SetExclusiveMode(bool value)
{
	if (!GetDirectInputDevice()) 
		return false;
	
	HRESULT hr;
	
	Unacquire();

	if (value)
	{
		hr = GetDirectInputDevice()->SetCooperativeLevel(GetInputManager().getHwnd(), DISCL_EXCLUSIVE | DISCL_FOREGROUND);
	}
	else
	{
		hr = GetDirectInputDevice()->SetCooperativeLevel(GetInputManager().getHwnd(), DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	}

	if (FAILED(hr))
	{
		gkLogError(_T("Mouse:SetExclusiveMode(%s) failed\n"), value?"true":"false");
		return false;
	}				
	
	return Acquire(); 
}

//////////////////////////////////////////////////////////////////////////
void gkMouse::PostEvent(SInputSymbol* pSymbol)
{
	// Post Input events.
	SInputEvent event;
	pSymbol->AssignTo(event, GetInputManager().GetModifiers());
	GetInputManager().PostInputEvent(event);
}

void gkMouse::PostOnlyIfChanged(SInputSymbol* pSymbol, EInputState newState)
{
	if (pSymbol->state != eIS_Released && newState == eIS_Released )
	{
		pSymbol->state = newState;
		pSymbol->value = 0.0f;
	}
	else if (pSymbol->state == eIS_Released && newState == eIS_Pressed)
	{
		pSymbol->state = newState;
		pSymbol->value = 1.0f;
	}
	else
	{
		return;
	}

	if (pSymbol->keyId >= eKI_Mouse1 && pSymbol->keyId <= eKI_Mouse8)
	{
		POINT point;
		GetCursorPos( &point );
		ScreenToClient( gEnv->pRenderer->GetWindowHwnd(), &point );
		Vec2i offset = gEnv->pRenderer->GetWindowOffset();

		pSymbol->value = point.x - offset.x;
		pSymbol->value2 =  point.y - offset.y;
	}


	PostEvent(pSymbol);
}

void gkMouse::SmoothDeltas(float accel,float decel)
{
	if (accel<0.0001f)
	{
		//do nothing ,just like it was before.
		return;
	}
	else if (accel<0.9999f)//mouse smooth, average the old and the actual delta by the delta ammount, less delta = more smooth speed.
	{
		Vec2 delta = m_deltas - m_oldDeltas;

		float len = delta.GetLength();

		float amt = 1.0f - (min(10.0f,len)/10.0f*min(accel,0.9f));

		m_deltas = m_oldDeltas + delta*amt;
	}
	else if (accel<1.0001f)//mouse smooth, just average the old and the actual delta.
	{
		m_deltas = (m_deltas + m_oldDeltas) * 0.5f;
	}
	else//mouse smooth with acceleration
	{
		float dt = min(gEnv->pTimer->GetFrameTime(),0.1f);

		Vec2 delta;

		float amt = 0.0;

		//if the input want to stop use twice of the acceleration.
		if (m_deltas.GetLength2()<0.0001f)
			if (decel>0.0001f)//there is a custom deceleration value? use it.
				amt = min(1.0f,dt*decel);
			else//otherwise acceleration * 2 is the default.
				amt = min(1.0f,dt*accel*2.0f);
		else
			amt = min(1.0f,dt*accel);

		delta = m_deltas - m_oldDeltas;
		m_deltas = m_oldDeltas + delta*amt;
	}

	m_oldDeltas = m_deltas;
}

void gkMouse::CapDeltas(float cap)
{

	//just cap deltas at certain value, used for mouse acceleration
	float temp;

	temp = fabs(m_deltas[0])/cap;
	if (temp>1.0f) m_deltas[0] /= temp;

	temp = fabs(m_deltas[1])/cap;
	if (temp>1.0f) m_deltas[1] /= temp;
}

#endif
