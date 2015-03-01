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
// Name:   	gkXboxController.h
// Desc:		
// 	
// Author:  Kaiming
// Date:	2012/2/10
// Modify:	2012/2/10
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _GKXBOXCONTROLLER_H_
#define _GKXBOXCONTROLLER_H_

#include "gkInputDeviceBase.h"
#include <xinput.h>
#pragma comment(lib, "xinput.lib")

struct ICVar;

/*
Device: 'XBOX 360 For Windows (Controller)' - 'XBOX 360 For Windows (Controller)'
Product GUID:  {028E045E-0000-0000-0000-504944564944}
Instance GUID: {65429170-3725-11DA-8001-444553540000}
*/

class gkXboxInputDevice : public gkInputDevice
{
public:
	gkXboxInputDevice(gkInputManager& pInput, int deviceNo);
	virtual ~gkXboxInputDevice();

	// IInputDevice overrides
	virtual bool Init();
	virtual void PostInit();
	virtual void Update(bool bFocus);
	virtual void ClearKeyState();
	virtual void ClearAnalogKeyState();
	// ~IInputDevice overrides

private:
	void UpdateConnectedState(bool isConnected);
	//void AddInputItem(const IInput::InputItem& item);

	//triggers the speed of the vibration motors -> leftMotor is for low frequencies, the right one is for high frequencies
	//bool SetVibration(USHORT leftMotor = 0, USHORT rightMotor = 0, float timing = 0, EFFEffectId effectId = eFF_Rumble_Basic);
	void ProcessAnalogStick(SInputSymbol* pSymbol, SHORT prev, SHORT current, SHORT threshold);

	ILINE float GetClampedLeftMotorAccumulatedVibration() const { return clamp(m_basicLeftMotorRumble + m_frameLeftMotorRumble, 0.0f, 65535.0f); }
	ILINE float GetClampedRightMotorAccumulatedVibration() const { return clamp(m_basicRightMotorRumble + m_frameRightMotorRumble, 0.0f, 65535.0f); }

	int							m_deviceNo; //!< device number (from 0 to 3) for this XInput device
	bool						m_connected;
	XINPUT_STATE				m_state;

	float						m_basicLeftMotorRumble, m_basicRightMotorRumble;
	float						m_frameLeftMotorRumble, m_frameRightMotorRumble;
	float						m_fVibrationTimer;
	float						m_fDeadZone;
	//std::vector<IInput::InputItem>	mInputQueue;	//!< queued inputs

public:
	static GUID		ProductGUID;
};

#endif