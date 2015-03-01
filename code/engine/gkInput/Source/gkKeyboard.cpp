#include "InputStableHeader.h"

#include "gkKeyboard.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
gkKeyboard::SScanCode gkKeyboard::m_scanCodes[256];
SInputSymbol*	gkKeyboard::Symbol[256] = {0};

gkKeyboard::gkKeyboard(gkInputManager& input):
gkInputDevice(input, _T("keyboard"), GUID_SysKeyboard)
{
	m_deviceId = eDI_Keyboard;
}

//////////////////////////////////////////////////////////////////////////
bool gkKeyboard::Init()
{
	//gEnv->pLog->LogToFile("Initializing Keyboard\n");
	
	if (!CreateDirectInputDevice(&c_dfDIKeyboard, DISCL_NONEXCLUSIVE|DISCL_FOREGROUND|DISCL_NOWINKEY, 32))
	{
		gkLogMessage( _T("Initializing Keyboard Failed.\n") );
		return false;
	}
	
	gkLogMessage( _T("InputDevice [ Keyboard ] Initialized.\n") );
	Acquire();	
	SetupKeyNames();

	return true;
}

//////////////////////////////////////////////////////////////////////////
bool gkKeyboard::SetExclusiveMode(bool value)
{ 
	Unacquire();
 
	HRESULT hr;

	if (value)
	{		
		hr = GetDirectInputDevice()->SetCooperativeLevel(GetInputManager().getHwnd(), DISCL_FOREGROUND|DISCL_EXCLUSIVE|DISCL_NOWINKEY);

		if (FAILED(hr))
		{
			gkLogError(_T("Cannot Set Keyboard Exclusive Mode\n"));
			return false;
		}
	}
	else
	{
		hr = GetDirectInputDevice()->SetCooperativeLevel(GetInputManager().getHwnd(), DISCL_FOREGROUND|DISCL_NONEXCLUSIVE|DISCL_NOWINKEY);
		if(FAILED(hr))
		{
			gkLogError(_T("Cannot Set Keyboard Non-Exclusive Mode\n"));
			return false;
		}				
	}

	if (!Acquire()) 
		return false;

	return true;
}

//////////////////////////////////////////////////////////////////////////
unsigned char gkKeyboard::Event2ASCII(const SInputEvent& event)
{
	unsigned int dik = event.keyId + 1;

	// not found or out of range
	if (dik >= 256)
		return '\0';

	switch(dik)
	{
		case DIK_DECIMAL:		return '.';
		case DIK_DIVIDE:		return '/';
	};

	if (event.modifiers & eMM_NumLock)
	{
		switch(dik)
		{
			case DIK_NUMPAD0:	return '0';
			case DIK_NUMPAD1:	return '1';
			case DIK_NUMPAD2:	return '2';
			case DIK_NUMPAD3:	return '3';
			case DIK_NUMPAD4:	return '4';
			case DIK_NUMPAD5:	return '5';
			case DIK_NUMPAD6:	return '6';
			case DIK_NUMPAD7:	return '7';
			case DIK_NUMPAD8:	return '8';
			case DIK_NUMPAD9:	return '9';
		};
	}
	if ((event.modifiers & eMM_Ctrl) && (event.modifiers & eMM_Alt) || (event.modifiers & eMM_RAlt))
	{
		return m_scanCodes[dik].ac;
	}
	else if ((event.modifiers & eMM_CapsLock) != 0)
	{
		return m_scanCodes[dik].cl;
	}
	else if ((event.modifiers & eMM_Shift) != 0)
	{
		return m_scanCodes[dik].uc;
	}
	return m_scanCodes[dik].lc;
}

unsigned char gkKeyboard::ToAscii(unsigned int vKeyCode, unsigned int k, unsigned char sKState[256]) const
{
	unsigned short ascii[2] = {0};
	int nResult = ToAsciiEx( vKeyCode, k, sKState, ascii, 0, GetKeyboardLayout(0) );

	if (nResult == 2)
		return (char)(ascii[1] ? ascii[1] : (ascii[0] >> 8));
	else if (nResult == 1)
		return (char)ascii[0];
	else
		return 0;
}

TCHAR gkKeyboard::ToUnicode(unsigned int vKeyCode, unsigned int k, unsigned char sKState[256]) const
{
	wchar_t unicode[2] = {0};
	int nResult = ToUnicodeEx( vKeyCode, k, sKState, unicode, 2, 0, GetKeyboardLayout(0) );
	if (nResult == 1)
		return unicode[0];
	return 0;
}

//////////////////////////////////////////////////////////////////////////
void gkKeyboard::SetupKeyNames()
{
	////////////////////////////////////////////////////////////
	memset(m_scanCodes, 0, sizeof(m_scanCodes));
	
	unsigned char sKState[256] = {0};
	unsigned int vKeyCode;

	for (int k=0;k<256;k++)
	{
		vKeyCode = MapVirtualKeyEx( k, 1, GetKeyboardLayout(0) );

		// lower case
		m_scanCodes[k].lc = ToAscii(vKeyCode, k, sKState);

		// upper case
		sKState[VK_SHIFT] = 0x80;
		m_scanCodes[k].uc = ToAscii(vKeyCode, k, sKState);
		sKState[VK_SHIFT] = 0;

		// alternate
		sKState[VK_CONTROL] = 0x80;
		sKState[VK_MENU] = 0x80;
		sKState[VK_LCONTROL] = 0x80;
		sKState[VK_LMENU] = 0x80;
		m_scanCodes[k].ac = ToAscii(vKeyCode, k, sKState);
		sKState[VK_CONTROL] = 0x0;
		sKState[VK_MENU] = 0x0;
		sKState[VK_LCONTROL] = 0x0;
		sKState[VK_LMENU] = 0x0;

		// caps lock
		sKState[VK_CAPITAL] = 0x01;
		m_scanCodes[k].cl = ToAscii(vKeyCode, k, sKState);
		sKState[VK_CAPITAL] = 0;
	}

	// subtly different so, I will leave this here
	for (int k = 0; k < 256; k++)
	{
		vKeyCode = MapVirtualKeyEx( k, 1, GetKeyboardLayout(0) );

		sKState[k] = 0x81;

		if (m_scanCodes[k].lc == 0)
		{
			m_scanCodes[k].lc = ToAscii(vKeyCode, k, sKState);
		}

		if (m_scanCodes[k].uc == 0)
		{
			sKState[VK_SHIFT] = 0x80;
			m_scanCodes[k].uc = ToAscii(vKeyCode, k, sKState);
			sKState[VK_SHIFT] = 0;
		}

		if (m_scanCodes[k].cl == 0)
		{
			sKState[VK_CAPITAL] = 0x80;
			m_scanCodes[k].cl = ToAscii(vKeyCode, k, sKState);
			sKState[VK_CAPITAL] = 0;
		}

		sKState[k] = 0;
	}

	Symbol[DIK_ESCAPE] = Symbol[DIK_ESCAPE] = MapSymbol(DIK_ESCAPE, eKI_Escape);
	Symbol[DIK_1] = MapSymbol(DIK_1, eKI_1);
	Symbol[DIK_2] = MapSymbol(DIK_2, eKI_2);
	Symbol[DIK_3] = MapSymbol(DIK_3, eKI_3);
	Symbol[DIK_4] = MapSymbol(DIK_4, eKI_4);
	Symbol[DIK_5] = MapSymbol(DIK_5, eKI_5);
	Symbol[DIK_6] = MapSymbol(DIK_6, eKI_6);
	Symbol[DIK_7] = MapSymbol(DIK_7, eKI_7);
	Symbol[DIK_8] = MapSymbol(DIK_8, eKI_8);
	Symbol[DIK_9] = MapSymbol(DIK_9, eKI_9);
	Symbol[DIK_0] = MapSymbol(DIK_0, eKI_0);
	Symbol[DIK_MINUS] = MapSymbol(DIK_MINUS, eKI_Minus);
	Symbol[DIK_EQUALS] = MapSymbol(DIK_EQUALS, eKI_Equals);
	Symbol[DIK_BACK] = MapSymbol(DIK_BACK, eKI_Backspace);
	Symbol[DIK_TAB] = MapSymbol(DIK_TAB, eKI_Tab);
	Symbol[DIK_Q] = MapSymbol(DIK_Q, eKI_Q);
	Symbol[DIK_W] = MapSymbol(DIK_W, eKI_W);
	Symbol[DIK_E] = MapSymbol(DIK_E, eKI_E);
	Symbol[DIK_R] = MapSymbol(DIK_R, eKI_R);
	Symbol[DIK_T] = MapSymbol(DIK_T, eKI_T);
	Symbol[DIK_Y] = MapSymbol(DIK_Y, eKI_Y);
	Symbol[DIK_U] = MapSymbol(DIK_U, eKI_U);
	Symbol[DIK_I] = MapSymbol(DIK_I, eKI_I);
	Symbol[DIK_O] = MapSymbol(DIK_O, eKI_O);
	Symbol[DIK_P] = MapSymbol(DIK_P, eKI_P);
	Symbol[DIK_LBRACKET] = MapSymbol(DIK_LBRACKET, eKI_LBracket);
	Symbol[DIK_RBRACKET] = MapSymbol(DIK_RBRACKET, eKI_RBracket);
	Symbol[DIK_RETURN] = MapSymbol(DIK_RETURN, eKI_Enter);
	Symbol[DIK_LCONTROL] = MapSymbol(DIK_LCONTROL, eKI_LCtrl, SInputSymbol::Button, eMM_LCtrl);
	Symbol[DIK_A] = MapSymbol(DIK_A, eKI_A);
	Symbol[DIK_S] = MapSymbol(DIK_S, eKI_S);
	Symbol[DIK_D] = MapSymbol(DIK_D, eKI_D);
	Symbol[DIK_F] = MapSymbol(DIK_F, eKI_F);
	Symbol[DIK_G] = MapSymbol(DIK_G, eKI_G);
	Symbol[DIK_H] = MapSymbol(DIK_H, eKI_H);
	Symbol[DIK_J] = MapSymbol(DIK_J, eKI_J);
	Symbol[DIK_K] = MapSymbol(DIK_K, eKI_K);
	Symbol[DIK_L] = MapSymbol(DIK_L, eKI_L);
	Symbol[DIK_SEMICOLON] = MapSymbol(DIK_SEMICOLON, eKI_Semicolon);
	Symbol[DIK_APOSTROPHE] = MapSymbol(DIK_APOSTROPHE, eKI_Apostrophe);
	Symbol[DIK_GRAVE] = MapSymbol(DIK_GRAVE, eKI_Tilde);
	Symbol[DIK_LSHIFT] = MapSymbol(DIK_LSHIFT, eKI_LShift, SInputSymbol::Button, eMM_LShift);
	Symbol[DIK_BACKSLASH] = MapSymbol(DIK_BACKSLASH, eKI_Backslash);
	Symbol[DIK_Z] = MapSymbol(DIK_Z, eKI_Z);
	Symbol[DIK_X] = MapSymbol(DIK_X, eKI_X);
	Symbol[DIK_C] = MapSymbol(DIK_C, eKI_C);
	Symbol[DIK_V] = MapSymbol(DIK_V, eKI_V);
	Symbol[DIK_B] = MapSymbol(DIK_B, eKI_B);
	Symbol[DIK_N] = MapSymbol(DIK_N, eKI_N);
	Symbol[DIK_M] = MapSymbol(DIK_M, eKI_M);
	Symbol[DIK_COMMA] = MapSymbol(DIK_COMMA, eKI_Comma);
	Symbol[DIK_PERIOD] = MapSymbol(DIK_PERIOD, eKI_Period);
	Symbol[DIK_SLASH] = MapSymbol(DIK_SLASH, eKI_Slash);
	Symbol[DIK_RSHIFT] = MapSymbol(DIK_RSHIFT, eKI_RShift, SInputSymbol::Button, eMM_RShift);
	Symbol[DIK_MULTIPLY] = MapSymbol(DIK_MULTIPLY, eKI_NP_Multiply);
	Symbol[DIK_LALT] = MapSymbol(DIK_LALT, eKI_LAlt, SInputSymbol::Button, eMM_LAlt);
	Symbol[DIK_SPACE] = MapSymbol(DIK_SPACE, eKI_Space);
	Symbol[DIK_CAPSLOCK] = MapSymbol(DIK_CAPSLOCK, eKI_CapsLock, SInputSymbol::Toggle, eMM_CapsLock);
	Symbol[DIK_F1] = MapSymbol(DIK_F1, eKI_F1);
	Symbol[DIK_F2] = MapSymbol(DIK_F2, eKI_F2);
	Symbol[DIK_F3] = MapSymbol(DIK_F3, eKI_F3);
	Symbol[DIK_F4] = MapSymbol(DIK_F4, eKI_F4);
	Symbol[DIK_F5] = MapSymbol(DIK_F5, eKI_F5);
	Symbol[DIK_F6] = MapSymbol(DIK_F6, eKI_F6);
	Symbol[DIK_F7] = MapSymbol(DIK_F7, eKI_F7);
	Symbol[DIK_F8] = MapSymbol(DIK_F8, eKI_F8);
	Symbol[DIK_F9] = MapSymbol(DIK_F9, eKI_F9);
	Symbol[DIK_F10] = MapSymbol(DIK_F10, eKI_F10);
	Symbol[DIK_NUMLOCK] = MapSymbol(DIK_NUMLOCK, eKI_NumLock, SInputSymbol::Toggle, eMM_NumLock);
	Symbol[DIK_SCROLL] = MapSymbol(DIK_SCROLL, eKI_ScrollLock, SInputSymbol::Toggle, eMM_ScrollLock);
	Symbol[DIK_NUMPAD7] = MapSymbol(DIK_NUMPAD7, eKI_NP_7);
	Symbol[DIK_NUMPAD8] = MapSymbol(DIK_NUMPAD8, eKI_NP_8);
	Symbol[DIK_NUMPAD9] = MapSymbol(DIK_NUMPAD9, eKI_NP_9);
	Symbol[DIK_SUBTRACT] = MapSymbol(DIK_SUBTRACT, eKI_NP_Substract);
	Symbol[DIK_NUMPAD4] = MapSymbol(DIK_NUMPAD4, eKI_NP_4);
	Symbol[DIK_NUMPAD5] = MapSymbol(DIK_NUMPAD5, eKI_NP_5);
	Symbol[DIK_NUMPAD6] = MapSymbol(DIK_NUMPAD6, eKI_NP_6);
	Symbol[DIK_ADD] = MapSymbol(DIK_ADD, eKI_NP_Add);
	Symbol[DIK_NUMPAD1] = MapSymbol(DIK_NUMPAD1, eKI_NP_1);
	Symbol[DIK_NUMPAD2] = MapSymbol(DIK_NUMPAD2, eKI_NP_2);
	Symbol[DIK_NUMPAD3] = MapSymbol(DIK_NUMPAD3, eKI_NP_3);
	Symbol[DIK_NUMPAD0] = MapSymbol(DIK_NUMPAD0, eKI_NP_0);
	Symbol[DIK_DECIMAL] = MapSymbol(DIK_DECIMAL, eKI_NP_Period);
	Symbol[DIK_F11] = MapSymbol(DIK_F11, eKI_F11);
	Symbol[DIK_F12] = MapSymbol(DIK_F12, eKI_F12);
	Symbol[DIK_F13] = MapSymbol(DIK_F13, eKI_F13);
	Symbol[DIK_F14] = MapSymbol(DIK_F14, eKI_F14);
	Symbol[DIK_F15] = MapSymbol(DIK_F15, eKI_F15);
	Symbol[DIK_COLON] = MapSymbol(DIK_COLON, eKI_Colon);
	Symbol[DIK_UNDERLINE] = MapSymbol(DIK_UNDERLINE, eKI_Underline);
	Symbol[DIK_NUMPADENTER] = MapSymbol(DIK_NUMPADENTER, eKI_NP_Enter);
	Symbol[DIK_RCONTROL] = MapSymbol(DIK_RCONTROL,  eKI_RCtrl, SInputSymbol::Button, eMM_RCtrl);
	Symbol[DIK_DIVIDE] = MapSymbol(DIK_DIVIDE, eKI_NP_Divide);
	Symbol[DIK_SYSRQ] = MapSymbol(DIK_SYSRQ, eKI_Print);
	Symbol[DIK_RALT] = MapSymbol(DIK_RALT, eKI_RAlt, SInputSymbol::Button, eMM_RAlt);
	Symbol[DIK_PAUSE] = MapSymbol(DIK_PAUSE, eKI_Pause);
	Symbol[DIK_HOME] = MapSymbol(DIK_HOME, eKI_Home);
	Symbol[DIK_UP] = MapSymbol(DIK_UP, eKI_Up);
	Symbol[DIK_PGUP] = MapSymbol(DIK_PGUP, eKI_PgUp);
	Symbol[DIK_LEFT] = MapSymbol(DIK_LEFT, eKI_Left);
	Symbol[DIK_RIGHT] = MapSymbol(DIK_RIGHT, eKI_Right);
	Symbol[DIK_END] = MapSymbol(DIK_END, eKI_End);
	Symbol[DIK_DOWN] = MapSymbol(DIK_DOWN, eKI_Down);
	Symbol[DIK_PGDN] = MapSymbol(DIK_PGDN, eKI_PgDn);
	Symbol[DIK_INSERT] = MapSymbol(DIK_INSERT, eKI_Insert);
	Symbol[DIK_DELETE] = MapSymbol(DIK_DELETE, eKI_Delete);
	//Symbol[DIK_LWIN] = MapSymbol(DIK_LWIN, eKI_LWin,  "lwin");
	//Symbol[DIK_RWIN] = MapSymbol(DIK_RWIN, eKI_RWin,  "rwin");
	//Symbol[DIK_APPS] = MapSymbol(DIK_APPS, eKI_Apps,  "apps");
	Symbol[DIK_OEM_102] = MapSymbol(DIK_OEM_102, eKI_OEM_102);
}

//////////////////////////////////////////////////////////////////////////
void gkKeyboard::ProcessKey(uint32 devSpecId, bool pressed)
{
	SInputSymbol* pSymbol = Symbol[devSpecId];

	if (!pSymbol) return;

	EInputState newState;

	int modifiers = GetInputManager().GetModifiers();
	if (pressed)
	{
		if (pSymbol->type == SInputSymbol::Toggle)
		{
			if (modifiers & pSymbol->user)
				modifiers &= ~pSymbol->user;
			else
				modifiers |= pSymbol->user;
		}
		else if (pSymbol->user)
		{
			// must be a regular modifier key
			modifiers |= pSymbol->user;
		}

		newState = eIS_Pressed;
		pSymbol->value = 1.0f;
	}
	else
	{
		if (pSymbol->user && pSymbol->type == SInputSymbol::Button)
		{
			// must be a regular modifier key
			modifiers &= ~pSymbol->user;
		}

		newState = eIS_Released;
		pSymbol->value = 0.0f;
	}

	GetInputManager().SetModifiers(modifiers);

	// check if the state has really changed ... otherwise ignore it
	if (newState == pSymbol->state)
	{
		//gEnv->pLog->Log("Input: Identical key event discarded: '%s' - %s", pSymbol->name.c_str(), newState==eIS_Pressed?"pressed":"released");
		return;
	}
	pSymbol->state = newState;

	// Post input events
	SInputEvent event;
	pSymbol->AssignTo(event, modifiers);

	// if alt+tab was pressed
	// auto-release it, because we lost the focus on the press, so we don't get the up message
	if ((event.keyId == eKI_Tab) && (event.state == eIS_Pressed) && (modifiers & eMM_Alt))
	{
		// swallow "alt-tab"
		pSymbol->value = 0;
		pSymbol->state = eIS_Released;
	}
	else
	{
		GetInputManager().PostInputEvent(event);
	}
}

//////////////////////////////////////////////////////////////////////////
void gkKeyboard::Update(bool bFocus)
{
	HRESULT hr;    	
	DIDEVICEOBJECTDATA rgdod[256];	
	DWORD dwItems = 256;

	//gkLogMessage( _T("InputDevice Update.") );
	while (GetDirectInputDevice())
	{
		dwItems = 256;
		//if (g_pInputCVars->i_bufferedkeys)
		{
			// read buffered data and keep the previous values otherwise keys 
			hr = GetDirectInputDevice()->GetDeviceData(sizeof(DIDEVICEOBJECTDATA),rgdod,&dwItems,0 ); //0);
		}
		//else
		{	
			//memset(m_cTempKeys,0,256);
			//hr = GetDirectInputDevice()->GetDeviceState(sizeof(m_cTempKeys),m_cTempKeys);
		}

		if (SUCCEEDED(hr))
		{
			//if (g_pInputCVars->i_bufferedkeys)
			{
				// go through all buffered items
				for (unsigned int k=0; k < dwItems; k++)
				{
					int key = rgdod[k].dwOfs;
					bool pressed = ((rgdod[k].dwData & 0x80) != 0);

					ProcessKey(key, pressed);
				}
			}		
			break;
		}
		else 
		{
			if (hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED)
			{			
				
				if (FAILED(hr = GetDirectInputDevice()->Acquire()))	
				{
					break;
				}
			}
			else
				break;
		}
	} 		  
}

//////////////////////////////////////////////////////////////////////////
void gkKeyboard::ClearKeyState()
{
	// preserve toggle keys
	int modifiers = GetInputManager().GetModifiers();
	modifiers &= eMM_LockKeys;

	bool bScroll = GetKeyState(VK_SCROLL) & 0x1;
	bool bNumLock = GetKeyState(VK_NUMLOCK) & 0x1;
	bool bCapsLock = GetKeyState(VK_CAPITAL) & 0x1;

	if (bScroll)
		modifiers |= eMM_ScrollLock;
	if (bNumLock)
		modifiers |= eMM_NumLock;
	if (bCapsLock)
		modifiers |= eMM_CapsLock;

	GetInputManager().SetModifiers(modifiers);

	
	// release pressed or held keys
	gkInputDevice::ClearKeyState();
}

const char* gkKeyboard::GetKeyName( const SInputEvent& event, bool bGUI/*=0*/ )
{
	if (event.deviceId != eDI_Keyboard)
		return 0;

	if (bGUI)
	{
		static char szKeyName[2];
		szKeyName[0] = Event2ASCII(event);
		szKeyName[1] = 0;
		return szKeyName;
	}
	else
	{
		return "";
	}
}
