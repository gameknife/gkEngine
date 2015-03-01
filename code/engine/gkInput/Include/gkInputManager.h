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
// Name:   	gkInputManager.h
// Desc:	输入管理器，gkInput和外部打交道的管理器	
// 	
// Author:  Kaiming
// Date:	2010/11/15
// Modify:	2012/2/9
// 
//////////////////////////////////////////////////////////////////////////
#pragma once
//#define STRICT
//#define DIRECTINPUT_VERSION 0x0800

#include "InputPrerequisites.h"
#include "IInputManager.h"

#ifdef OS_WIN32
#include <d3dx9.h>
#endif

struct gkInputCVars
{
public:
	int		i_debug;
	int		i_forcefeedback;

	int		i_mouse_buffered;
	float	i_mouse_accel;
	float	i_mouse_accel_max;
	float	i_mouse_smooth;
	float	i_mouse_inertia;

	int		i_bufferedkeys;

	int		i_xinput;
	int		i_xinput_poll_time;

	float	i_holdtime_medium;
	float	i_holdtime_long;

	gkInputCVars();
	~gkInputCVars();
};

extern gkInputCVars* g_pInputCVars;

struct SScanCode
{
	char		lc;	// lowercase
	char		uc;	// uppercase
	char		ac;	// alt gr
	char		cl;	// caps lock (differs slightly from uppercase)
};

class gkInputManager: public IInputManager
{
public:

	gkInputManager(void);
	~gkInputManager(void);

	// DirectX Input
	virtual void Destroy();
	virtual HRESULT Init( HWND hDlg );

	void Update(float fElapsedTime);

	void setHWND(HWND hWnd);

	virtual bool isMultiKeyPressedSinceLastFrame() {return (m_multiKeyPressed > 1);}
	virtual int getLastPressedKey() {return m_nlastKey;}

	virtual void addInputEventListener(IInputEventListener* listener);
	virtual void removeEventListener( IInputEventListener* listener );

	virtual void setExclusiveListener( IInputEventListener *pListener ) {m_exclusiveListener = pListener;}
	virtual IInputEventListener *getExclusiveListener() {return m_exclusiveListener;}

	virtual void PostInputEvent( const SInputEvent &event, bool bForce = false);
	int		GetModifiers() const	{ return m_globalModifier;	}
	void	SetModifiers(int modifiers)	{		m_globalModifier = modifiers;	}
	bool AddInputDevice(IInputDevice* pDevice);
	SInputSymbol* LookupSymbol( EDeviceId deviceId, EKeyId keyId );
	void ClearKeyState();

	virtual void SetExclusiveMode(EDeviceId deviceId, bool exclusive, void *pUser);
#ifdef OS_ANDROID	
	virtual Android_InputHandler getAndroidHandler();
#endif

public:
#ifdef OS_WIN32
	LPDIRECTINPUT8 getDirectInput() {return m_pDI;}
	HWND getHwnd() {return m_hWnd;}
#endif
protected:
	void processKeyboard();
	void processKey(EKeyId keycode);
	void processModifier();
	void RetriggerKeyState();
	void PostHoldEvents();
	const char* GetKeyName(const SInputEvent& event, bool bGUI = 0);
protected:

#ifdef OS_WIN32
	LPDIRECTINPUT8          m_pDI; // DirectInput interface       
	LPDIRECTINPUTDEVICE8    m_pMouse; // Device interface
#endif
	float m_fFramesToSmoothMouseData;
	HWND m_hWnd;

	int m_multiKeyPressed;
	int m_nlastKey;
	bool m_retriggering;
	typedef std::list<IInputEventListener*> TInputEventListeners;
	TInputEventListeners m_listeners;
	IInputEventListener* m_exclusiveListener;

	typedef std::vector<SInputSymbol*> TInputSymbols;
	TInputSymbols					m_holdSymbols;

	// input device management
	typedef std::vector<IInputDevice*>	TInputDevices;
	TInputDevices					m_inputDevices;

	int m_globalModifier;
	SScanCode			m_scanCodes[256];

	gkInputCVars* m_pInputCVars;
};
