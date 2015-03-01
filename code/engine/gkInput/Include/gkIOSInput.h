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
// Name:   	gkIOSInput.h
// Desc:		
// 	
// Author:  Kaiming
// Date:	2012/6/17
// Modify:	
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _gkIOSInput_h_
#define _gkIOSInput_h_

#include "IInputManager.h"

#define GK_IOS_MAX_TOUCHES  255

class gkInputManager;

class gkIOSInput : public IInputDevice
{
public:
	gkIOSInput(gkInputManager& input);

	// IInputDevice overrides
	virtual bool Init();
	virtual void Update(bool bFocus);
	virtual bool SetExclusiveMode(bool value);
	virtual void ClearKeyState();


	// ~IInputDevice
	virtual const char* GetKeyName( const SInputEvent& event, bool bGUI=0 );

	virtual const TCHAR* GetDeviceName() const;

	virtual EDeviceId GetDeviceId() const;

	virtual void PostInit();

	virtual void ClearAnalogKeyState();

	virtual SInputSymbol* LookupSymbol( EKeyId id ) const;

	virtual void Enable( bool enable );

	virtual bool IsEnabled() const;

private:
	gkInputManager* m_pManager;
	EDeviceId m_deviceId;
    
    Vec2 m_posPrevious[GK_IOS_MAX_TOUCHES];
    Vec2 m_posDelta[GK_IOS_MAX_TOUCHES];
};

#endif


