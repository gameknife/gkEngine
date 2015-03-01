#include "InputStableHeader.h"
#include "gkXboxController.h"
#include "ITimer.h"


GUID	gkXboxInputDevice::ProductGUID = { 0x028e045E, 0x0000, 0x0000, { 0x00,0x00,0x50,0x49,0x44,0x56,0x49,0x44} };
#define INPUT_MAX (32767.0f)
#define INPUT_DEFAULT_DEADZONE  ( 0.24f )  // Default to 24% of the range. This is a reasonable default value but can be altered if needed.
#define INPUT_DEADZONE_MULTIPLIER  ( FLOAT(0x7FFF) )  // Multiply dead zone to cover the +/- 32767 range.  

const TCHAR* deviceNames[] =
{
	_T("xinput0"),
	_T("xinput1"),
	_T("xinput2"),
	_T("xinput3")
};

// a few internal ids
#define _XINPUT_GAMEPAD_LEFT_TRIGGER			(1 << 17)
#define _XINPUT_GAMEPAD_RIGHT_TRIGGER			(1 << 18)
#define _XINPUT_GAMEPAD_LEFT_THUMB_X			(1 << 19)
#define _XINPUT_GAMEPAD_LEFT_THUMB_Y			(1 << 20)
#define _XINPUT_GAMEPAD_RIGHT_THUMB_X			(1 << 21)
#define _XINPUT_GAMEPAD_RIGHT_THUMB_Y			(1 << 22)
#define _XINPUT_GAMEPAD_LEFT_TRIGGER_BTN	(1 << 23)	// left trigger usable as a press/release button
#define _XINPUT_GAMEPAD_RIGHT_TRIGGER_BTN	(1 << 24)	// right trigger usable as a press/release button
#define _XINPUT_GAMEPAD_LEFT_THUMB_UP			(1 << 25)
#define _XINPUT_GAMEPAD_LEFT_THUMB_DOWN		((1 << 25) + 1)
#define _XINPUT_GAMEPAD_LEFT_THUMB_LEFT		((1 << 25) + 2)
#define _XINPUT_GAMEPAD_LEFT_THUMB_RIGHT	((1 << 25) + 3)
#define _XINPUT_GAMEPAD_RIGHT_THUMB_UP		(1 << 26)
#define _XINPUT_GAMEPAD_RIGHT_THUMB_DOWN	((1 << 26) + 1)
#define _XINPUT_GAMEPAD_RIGHT_THUMB_LEFT	((1 << 26) + 2)
#define _XINPUT_GAMEPAD_RIGHT_THUMB_RIGHT	((1 << 26) + 3)

volatile bool g_bConnected[4] = { false };

//int g_numControllers = 0;

//////////////////////////////////////////////////////////////////////////
// This is a thread task responsible for periodically checking gamepad 
// connection status.
//////////////////////////////////////////////////////////////////////////
// class CInputConnectionThreadTask : public IThreadTask
// {
// 	IInput *m_pInput;
// 	volatile bool m_bQuit;
// 	SThreadTaskInfo m_TaskInfo;
// 
// public:
// 	CInputConnectionThreadTask( IInput *pInput )
// 	{
// 		m_bQuit = false;
// 		m_pInput = pInput;
// 	}
// 
// 	//////////////////////////////////////////////////////////////////////////
// 	virtual void Stop()
// 	{
// 		m_bQuit = true;
// 	}
// 
// 	virtual void OnUpdate()
// 	{
// 		IInput *pInput = m_pInput;
// 		XINPUT_CAPABILITIES caps;
// 
// 		while (!m_bQuit)
// 		{
// 			{
// 				for (DWORD i = 0; i < 4; ++i)
// 				{
// 					DWORD r = XInputGetCapabilities(i, XINPUT_FLAG_GAMEPAD, &caps);
// 					g_bConnected[i] = r == ERROR_SUCCESS;
// 				}
// 			}
// 			Sleep(1000);
// 		}
// 	}
// 	virtual SThreadTaskInfo* GetTaskInfo() { return &m_TaskInfo; }
// 
// };
// 
// //////////////////////////////////////////////////////////////////////////
// CInputConnectionThreadTask *g_pInputConnectionThreadTask = 0;

void CleanupVibrationAtExit()
{
	// Kill vibration on exit
	XINPUT_VIBRATION vibration;
	ZeroMemory( &vibration, sizeof(XINPUT_VIBRATION) );
	for (int i = 0; i < 4; i++)
		XInputSetState( i, &vibration );
}

gkXboxInputDevice::gkXboxInputDevice(gkInputManager& input, int deviceNo) : gkInputDevice(input, deviceNames[deviceNo], GUID()),
m_deviceNo(deviceNo), m_connected(false)
{
	memset( &m_state, 0, sizeof(XINPUT_STATE) );
	m_deviceId = eDI_XBox;
	//SetVibration(0, 0, 0);

	m_fDeadZone = INPUT_DEFAULT_DEADZONE * INPUT_DEADZONE_MULTIPLIER;

	atexit(CleanupVibrationAtExit);
}

gkXboxInputDevice::~gkXboxInputDevice()
{
	//SetVibration(0, 0, 0);

// 	if (g_pInputConnectionThreadTask)
// 	{
// 		gEnv->pSystem->GetIThreadTaskManager()->UnregisterTask( g_pInputConnectionThreadTask );
// 		delete g_pInputConnectionThreadTask;
// 		g_pInputConnectionThreadTask = 0;
// 	}
}

bool gkXboxInputDevice::Init()
{
	// setup input event names
	MapSymbol(XINPUT_GAMEPAD_DPAD_UP, eKI_Xbox_DPadUp);
	MapSymbol(XINPUT_GAMEPAD_DPAD_DOWN, eKI_Xbox_DPadDown);
	MapSymbol(XINPUT_GAMEPAD_DPAD_LEFT, eKI_Xbox_DPadLeft);
	MapSymbol(XINPUT_GAMEPAD_DPAD_RIGHT, eKI_Xbox_DPadRight);
	MapSymbol(XINPUT_GAMEPAD_START, eKI_Xbox_Start);
	MapSymbol(XINPUT_GAMEPAD_BACK, eKI_Xbox_Back);
	MapSymbol(XINPUT_GAMEPAD_LEFT_THUMB, eKI_Xbox_ThumbL);
	MapSymbol(XINPUT_GAMEPAD_RIGHT_THUMB, eKI_Xbox_ThumbR);
	MapSymbol(XINPUT_GAMEPAD_LEFT_SHOULDER, eKI_Xbox_ShoulderL);
	MapSymbol(XINPUT_GAMEPAD_RIGHT_SHOULDER, eKI_Xbox_ShoulderR);
	MapSymbol(XINPUT_GAMEPAD_A, eKI_Xbox_A);
	MapSymbol(XINPUT_GAMEPAD_B, eKI_Xbox_B);
	MapSymbol(XINPUT_GAMEPAD_X, eKI_Xbox_X);
	MapSymbol(XINPUT_GAMEPAD_Y, eKI_Xbox_Y);
	MapSymbol(_XINPUT_GAMEPAD_LEFT_TRIGGER, eKI_Xbox_TriggerL,	SInputSymbol::Trigger);
	MapSymbol(_XINPUT_GAMEPAD_RIGHT_TRIGGER, eKI_Xbox_TriggerR,	SInputSymbol::Trigger);
	MapSymbol(_XINPUT_GAMEPAD_LEFT_THUMB_X, eKI_Xbox_ThumbLX, 	SInputSymbol::Axis);
	MapSymbol(_XINPUT_GAMEPAD_LEFT_THUMB_Y, eKI_Xbox_ThumbLY,		SInputSymbol::Axis);
	MapSymbol(_XINPUT_GAMEPAD_LEFT_THUMB_UP, eKI_Xbox_ThumbLUp);
	MapSymbol(_XINPUT_GAMEPAD_LEFT_THUMB_DOWN, eKI_Xbox_ThumbLDown);
	MapSymbol(_XINPUT_GAMEPAD_LEFT_THUMB_LEFT, eKI_Xbox_ThumbLLeft);
	MapSymbol(_XINPUT_GAMEPAD_LEFT_THUMB_RIGHT, eKI_Xbox_ThumbLRight);
	MapSymbol(_XINPUT_GAMEPAD_RIGHT_THUMB_X, eKI_Xbox_ThumbRX, SInputSymbol::Axis);
	MapSymbol(_XINPUT_GAMEPAD_RIGHT_THUMB_Y, eKI_Xbox_ThumbRY, SInputSymbol::Axis);
	MapSymbol(_XINPUT_GAMEPAD_RIGHT_THUMB_UP, eKI_Xbox_ThumbRUp);
	MapSymbol(_XINPUT_GAMEPAD_RIGHT_THUMB_DOWN, eKI_Xbox_ThumbRDown);
	MapSymbol(_XINPUT_GAMEPAD_RIGHT_THUMB_LEFT, eKI_Xbox_ThumbRLeft);
	MapSymbol(_XINPUT_GAMEPAD_RIGHT_THUMB_RIGHT, eKI_Xbox_ThumbRRight);
	MapSymbol(_XINPUT_GAMEPAD_LEFT_TRIGGER_BTN, eKI_Xbox_TriggerLBtn);	// trigger usable as a button
	MapSymbol(_XINPUT_GAMEPAD_RIGHT_TRIGGER_BTN, eKI_Xbox_TriggerRBtn);	// trigger usable as a button


	//////////////////////////////////////////////////////////////////////////


	m_basicLeftMotorRumble = m_basicRightMotorRumble = 0.0f;
	m_frameLeftMotorRumble = m_frameRightMotorRumble = 0.0f;
	//RestoreDefaultDeadZone();

	return true;
}

void gkXboxInputDevice::PostInit()
{
	// Create input connection thread task
// 	if(g_pInputConnectionThreadTask == NULL)
// 	{
// 		SThreadTaskParams threadParams;
// 		threadParams.name = "InputWorker";
// 		threadParams.nFlags = THREAD_TASK_BLOCKING;
// #if defined (XENON)
// 		threadParams.nPreferedThread = 5;
// #endif
// 
// 		g_pInputConnectionThreadTask = new CInputConnectionThreadTask( &GetIInput() );
// 		gEnv->pSystem->GetIThreadTaskManager()->RegisterTask( g_pInputConnectionThreadTask,threadParams );
// 	}
 }

void gkXboxInputDevice::Update(bool bFocus)
{
	

	float now = gEnv->pTimer->GetCurrTime();
// 	if((m_fVibrationTimer && m_fVibrationTimer < now) || g_pInputCVars->i_forcefeedback == 0 || gEnv->pSystem->IsPaused())
// 	{
// 		m_fVibrationTimer = 0;
// 		//SetVibration(0, 0, 0.0f);
// 	}

	// interpret input
	if (true || bFocus)
	{
		XINPUT_STATE state;
		memset( &state, 0, sizeof(XINPUT_STATE) );
		if ( ERROR_SUCCESS != XInputGetState(m_deviceNo, &state) )
		{
			// disconnected.
			UpdateConnectedState(false);
			return;
		}
		else
		{
			// connected.
			UpdateConnectedState(true);
		}

		if (true || state.dwPacketNumber != m_state.dwPacketNumber)
		{
			SInputEvent event;
			SInputSymbol*	pSymbol = 0;

			event.deviceIndex = (uint8)m_deviceNo;

			const float INV_VALIDRANGE = (1.0f / (INPUT_MAX - m_fDeadZone));

			// make the inputs move smoothly out of the deadzone instead of snapping straight to m_fDeadZone
			float fraction=max(abs(state.Gamepad.sThumbLX) - m_fDeadZone, 0) * INV_VALIDRANGE; 
			float oldVal=state.Gamepad.sThumbLX;
			state.Gamepad.sThumbLX = fraction * INPUT_MAX * sgn(state.Gamepad.sThumbLX); 

			fraction = max(abs(state.Gamepad.sThumbLY) - m_fDeadZone, 0) * INV_VALIDRANGE; 
			oldVal = state.Gamepad.sThumbLY;
			state.Gamepad.sThumbLY = fraction * INPUT_MAX * sgn(state.Gamepad.sThumbLY);

			// make the inputs move smoothly out of the deadzone instead of snapping straight to m_fDeadZone
			fraction=max(abs(state.Gamepad.sThumbRX) - m_fDeadZone, 0) * INV_VALIDRANGE; 
			oldVal=state.Gamepad.sThumbRX;
			state.Gamepad.sThumbRX = fraction * INPUT_MAX * sgn(state.Gamepad.sThumbRX); 

			fraction = max(abs(state.Gamepad.sThumbRY) - m_fDeadZone, 0) * INV_VALIDRANGE; 
			oldVal = state.Gamepad.sThumbRY;
			state.Gamepad.sThumbRY = fraction * INPUT_MAX * sgn(state.Gamepad.sThumbRY);

			// compare new values against cached value and only send out changes as new input
			WORD buttonsChange = m_state.Gamepad.wButtons ^ state.Gamepad.wButtons;
			if (buttonsChange)
			{	
				for (int i = 0; i < 16; ++i)
				{
					uint32 id = (1 << i);
					if (buttonsChange & id && (pSymbol = DevSpecIdToSymbol(id)))
					{
						pSymbol->PressEvent((state.Gamepad.wButtons & id) != 0);
						pSymbol->AssignTo(event);
						event.deviceId = eDI_XBox;
						GetInputManager().PostInputEvent(event);
					}
				}
			}

			// now we have done the digital buttons ... let's do the analog stuff
			if (m_state.Gamepad.bLeftTrigger != state.Gamepad.bLeftTrigger)
			{
				pSymbol = DevSpecIdToSymbol(_XINPUT_GAMEPAD_LEFT_TRIGGER);
				pSymbol->ChangeEvent(state.Gamepad.bLeftTrigger / 255.0f);
				pSymbol->AssignTo(event);
				event.deviceId = eDI_XBox;
				GetInputManager().PostInputEvent(event);

				//--- Check previous and current trigger against threshold for digital press/release event
				bool bIsPressed=state.Gamepad.bLeftTrigger>XINPUT_GAMEPAD_TRIGGER_THRESHOLD ? true : false;
				bool bWasPressed=m_state.Gamepad.bLeftTrigger>XINPUT_GAMEPAD_TRIGGER_THRESHOLD ? true : false;
				if(bIsPressed!=bWasPressed)
				{
					pSymbol=DevSpecIdToSymbol(_XINPUT_GAMEPAD_LEFT_TRIGGER_BTN);
					pSymbol->PressEvent(bIsPressed);
					pSymbol->AssignTo(event);
					event.deviceId = eDI_XBox;
					GetInputManager().PostInputEvent(event);
				}
			}
			if (m_state.Gamepad.bRightTrigger != state.Gamepad.bRightTrigger)
			{
				pSymbol = DevSpecIdToSymbol(_XINPUT_GAMEPAD_RIGHT_TRIGGER);
				pSymbol->ChangeEvent(state.Gamepad.bRightTrigger / 255.0f);
				pSymbol->AssignTo(event);
				event.deviceId = eDI_XBox;
				GetInputManager().PostInputEvent(event);

				//--- Check previous and current trigger against threshold for digital press/release event
				bool bIsPressed=state.Gamepad.bRightTrigger>XINPUT_GAMEPAD_TRIGGER_THRESHOLD;
				bool bWasPressed=m_state.Gamepad.bRightTrigger>XINPUT_GAMEPAD_TRIGGER_THRESHOLD;
				if(bIsPressed!=bWasPressed)
				{
					pSymbol=DevSpecIdToSymbol(_XINPUT_GAMEPAD_RIGHT_TRIGGER_BTN);
					pSymbol->PressEvent(bIsPressed);
					pSymbol->AssignTo(event);
					event.deviceId = eDI_XBox;
					GetInputManager().PostInputEvent(event);
				}
			}
			if (1)
			{
				pSymbol = DevSpecIdToSymbol(_XINPUT_GAMEPAD_LEFT_THUMB_X);
				if(state.Gamepad.sThumbLX==0.f)
					pSymbol->ChangeEvent(0.f);
				else
					pSymbol->ChangeEvent((state.Gamepad.sThumbLX + 32768) / 32767.5f - 1.0f);
				pSymbol->AssignTo(event);
				event.deviceId = eDI_XBox;
				GetInputManager().PostInputEvent(event);
				//--- Check previous and current state to generate digital press/release event
				static SInputSymbol* pSymbolLeft = DevSpecIdToSymbol(_XINPUT_GAMEPAD_LEFT_THUMB_LEFT);
				ProcessAnalogStick(pSymbolLeft, m_state.Gamepad.sThumbLX, state.Gamepad.sThumbLX, -25000);
				static SInputSymbol* pSymbolRight = DevSpecIdToSymbol(_XINPUT_GAMEPAD_LEFT_THUMB_RIGHT);
				ProcessAnalogStick(pSymbolRight, m_state.Gamepad.sThumbLX, state.Gamepad.sThumbLX, 25000);
			}
			if (1)
			{
				pSymbol = DevSpecIdToSymbol(_XINPUT_GAMEPAD_LEFT_THUMB_Y);
				if(state.Gamepad.sThumbLY==0.f)
					pSymbol->ChangeEvent(0.f);
				else
					pSymbol->ChangeEvent((state.Gamepad.sThumbLY + 32768.0f) / 32767.5f - 1.0f);
				pSymbol->AssignTo(event);
				event.deviceId = eDI_XBox;
				GetInputManager().PostInputEvent(event);
				//--- Check previous and current state to generate digital press/release event
				static SInputSymbol* pSymbolUp = DevSpecIdToSymbol(_XINPUT_GAMEPAD_LEFT_THUMB_UP);
				ProcessAnalogStick(pSymbolUp, m_state.Gamepad.sThumbLY, state.Gamepad.sThumbLY, 25000);
				static SInputSymbol* pSymbolDown = DevSpecIdToSymbol(_XINPUT_GAMEPAD_LEFT_THUMB_DOWN);
				ProcessAnalogStick(pSymbolDown, m_state.Gamepad.sThumbLY, state.Gamepad.sThumbLY, -25000);
			}
			if (1)
			{
				pSymbol = DevSpecIdToSymbol(_XINPUT_GAMEPAD_RIGHT_THUMB_X);
				if(state.Gamepad.sThumbRX==0.f)
					pSymbol->ChangeEvent(0.f);
				else
					pSymbol->ChangeEvent((state.Gamepad.sThumbRX + 32768) / 32767.5f - 1.0f);
				pSymbol->AssignTo(event);
				event.deviceId = eDI_XBox;
				GetInputManager().PostInputEvent(event);
				//--- Check previous and current state to generate digital press/release event
				static SInputSymbol* pSymbolLeft = DevSpecIdToSymbol(_XINPUT_GAMEPAD_RIGHT_THUMB_LEFT);
				ProcessAnalogStick(pSymbolLeft, m_state.Gamepad.sThumbRX, state.Gamepad.sThumbRX, -25000);
				static SInputSymbol* pSymbolRight = DevSpecIdToSymbol(_XINPUT_GAMEPAD_RIGHT_THUMB_RIGHT);
				ProcessAnalogStick(pSymbolRight, m_state.Gamepad.sThumbRX, state.Gamepad.sThumbRX, 25000);
			}
			if (1)
			{
				pSymbol = DevSpecIdToSymbol(_XINPUT_GAMEPAD_RIGHT_THUMB_Y);
				if(state.Gamepad.sThumbRY==0.f)
					pSymbol->ChangeEvent(0.f);
				else
					pSymbol->ChangeEvent((state.Gamepad.sThumbRY + 32768) / 32767.5f - 1.0f);
				pSymbol->AssignTo(event);
				event.deviceId = eDI_XBox;
				GetInputManager().PostInputEvent(event);
				//--- Check previous and current state to generate digital press/release event
				static SInputSymbol* pSymbolUp = DevSpecIdToSymbol(_XINPUT_GAMEPAD_RIGHT_THUMB_UP);
				ProcessAnalogStick(pSymbolUp, m_state.Gamepad.sThumbRY, state.Gamepad.sThumbRY, 25000);
				static SInputSymbol* pSymbolDown = DevSpecIdToSymbol(_XINPUT_GAMEPAD_RIGHT_THUMB_DOWN);
				ProcessAnalogStick(pSymbolDown, m_state.Gamepad.sThumbRY, state.Gamepad.sThumbRY, -25000);
			}

			// update cache
			m_state = state;
		}
	}
}

void gkXboxInputDevice::ClearKeyState()
{
	m_fVibrationTimer = 0;
	//SetVibration(0, 0, 0.0f);

	gkInputDevice::ClearKeyState();
}

void gkXboxInputDevice::ClearAnalogKeyState()
{
	// Reset internal state and decrement packet number so updated again immediatly
	// This will force-send the current state of all analog keys on next update
	m_state.Gamepad.sThumbLX = 0;
	m_state.Gamepad.sThumbLY = 0;
	m_state.Gamepad.sThumbRX = 0;
	m_state.Gamepad.sThumbRY = 0;
	m_state.Gamepad.bLeftTrigger = 0;
	m_state.Gamepad.bRightTrigger = 0;
	m_state.dwPacketNumber--;
}

void gkXboxInputDevice::UpdateConnectedState(bool isConnected)
{
	if (m_connected != isConnected)
	{
		SInputEvent event;
		event.deviceIndex = (uint8)m_deviceNo;
		event.deviceId = eDI_XBox;
		event.state = eIS_Changed;

		// state has changed, emit event here
		if (isConnected)
		{
			// connect
			//printf("xinput%d connected\n", m_deviceNo);
			event.keyId = eKI_SYS_ConnectDevice;
			gkLogMessage(_T("Xbox Controller [#%d] connected."), m_deviceNo);
		}
		else
		{
			// disconnect
			//printf("xinput%d disconnected\n", m_deviceNo);
			event.keyId = eKI_SYS_DisconnectDevice;
			gkLogMessage(_T("Xbox Controller [#%d] disconnected."), m_deviceNo);
		}
		m_connected = isConnected;
		GetInputManager().PostInputEvent(event);

		// Send generalized keyId connect/disconnect 
		// eKI_Xbox_Connect & eKI_Xbox_Disconnect should be deprecated because all devices can be connected/disconnected
		event.keyId = (isConnected) ? eKI_SYS_ConnectDevice : eKI_SYS_DisconnectDevice;
		GetInputManager().PostInputEvent(event,true);
	}
}

void gkXboxInputDevice::ProcessAnalogStick(SInputSymbol* pSymbol, SHORT prev, SHORT current, SHORT threshold)
{
	bool send = false;

	if(threshold>0.0f)
	{
		if (prev >= threshold && current < threshold)
		{
			// release
			send = true;
			pSymbol->PressEvent(false);
		}
		else if (prev <= threshold && current > threshold)
		{
			// press
			send = true;
			pSymbol->PressEvent(true);
		}
	}
	else
	{
		if (prev <= threshold && current > threshold)
		{
			// release
			send = true;
			pSymbol->PressEvent(false);
		}
		else if (prev >= threshold && current < threshold)
		{
			// press
			send = true;
			pSymbol->PressEvent(true);
		}
	}


	if (send)
	{
		SInputEvent event;
		event.deviceIndex = (uint8)m_deviceNo;
		pSymbol->AssignTo(event);
		event.deviceId = eDI_XBox;
		GetInputManager().PostInputEvent(event);
	}
}
// 
// void gkXboxInputDevice::SetDeadZone(float fThreshold)
// {
// 	m_fDeadZone = clamp(fThreshold, 0.001f, 1.0f) * INPUT_DEADZONE_MULTIPLIER;
// }
// 
// void gkXboxInputDevice::RestoreDefaultDeadZone()
// {
// 	SetDeadZone(INPUT_DEFAULT_DEADZONE);
// }
