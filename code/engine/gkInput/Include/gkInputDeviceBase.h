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
// Name:   	gkInputDeviceBase.h
// Desc:	gkENGINE 输入设备抽象类，实现输入设备的基本接口
// 	
// Author:  Kaiming
// Date:	2012/2/9
// Modify:	2012/2/9
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _GKINPUTDEVICEBASE_H_
#define _GKINPUTDEVICEBASE_H_

#include "InputPrerequisites.h"

class gkInputDevice : public IInputDevice
{
public:
public:
	gkInputDevice(gkInputManager& input, const TCHAR* deviceName, const GUID& guid);
	virtual ~gkInputDevice();

	//! return reference to the input interface which created the instance of this class
	gkInputManager& GetInputManager() const	{	return	m_input;	}

	//! return DirectInput device managed by this class
	LPDIRECTINPUTDEVICE8 GetDirectInputDevice() const;

	// override IInput
	virtual const TCHAR* GetDeviceName() const {return m_deviceName.c_str();}
	virtual EDeviceId GetDeviceId() const {return m_deviceId;}
	// Initialization.
	virtual bool	Init() {return true;}
	virtual void	PostInit() {}
	// Update.
	virtual void	Update(bool bFocus);

	virtual bool	SetExclusiveMode(bool value) {return true;}

	virtual void	ClearKeyState();

	virtual void	ClearAnalogKeyState();

	virtual SInputSymbol* LookupSymbol(EKeyId id) const;
	virtual void Enable(bool enable);
	virtual bool IsEnabled() const {return m_enabled;}

	virtual const char* GetKeyName(const SInputEvent& event, bool bGUI=0);

	virtual void SetHwnd(HWND hwnd);

	// own
protected:
	// device dependent id management
	virtual SInputSymbol*		IdToSymbol(EKeyId id) const;
	virtual SInputSymbol*		DevSpecIdToSymbol(uint32 devSpecId) const;
	virtual SInputSymbol*		MapSymbol(uint32 deviceSpecificId, EKeyId keyId, SInputSymbol::EType type = SInputSymbol::Button, uint32 user = 0);

	// device acquisition
	bool				Acquire();
	bool				Unacquire();
	bool CreateDirectInputDevice(LPCDIDATAFORMAT dataformat, DWORD coopLevel, DWORD bufSize);

protected:
	EDeviceId									m_deviceId;
	bool										m_enabled;
private:
	gkInputManager&								m_input;				// point to input system in use
	gkStdString								m_deviceName;		// name of the device (used for input binding)

	typedef std::map<EKeyId, SInputSymbol*>		TIdToSymbolMap;
	typedef std::map<uint32, SInputSymbol*>		TDevSpecIdToSymbolMap;


	TIdToSymbolMap								m_idToInfo;
	TDevSpecIdToSymbolMap						m_devSpecIdToSymbol;


	LPDIRECTINPUTDEVICE8						m_pDevice;			// pointer to the input device represented by this object (initialized to 0)
	const GUID&									m_guid;					// GUID of this device instance
	LPCDIDATAFORMAT								m_pDataFormat;	// this specifies the dinput specific data format in use
	DWORD										m_dwCoopLevel;	// this specifies the direct input cooperation level with other devices

	bool										m_bNeedsPoll;		// the device needs polling to fill the input buffers
};


#endif