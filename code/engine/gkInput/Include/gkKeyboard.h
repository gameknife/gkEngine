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
// Name:   	gkKeyboard.h
// Desc:		
// 	
// Author:  Kaiming
// Date:	2012/2/9
// Modify:	2012/2/9
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _GKKEYBOARD_H_
#define _GKKEYBOARD_H_
#include "gkInputDeviceBase.h"

class gkKeyboard : public gkInputDevice
{
	struct SScanCode
	{
		char		lc;	// lowercase
		char		uc;	// uppercase
		char		ac;	// alt gr
		char		cl;	// caps lock (differs slightly from uppercase)
	};
public:
	gkKeyboard(gkInputManager& input);

	// IInputDevice overrides
	virtual bool Init();
	virtual void Update(bool bFocus);
	virtual bool SetExclusiveMode(bool value);
	virtual void ClearKeyState();
	// ~IInputDevice
	virtual const char* GetKeyName( const SInputEvent& event, bool bGUI/*=0*/ );

public:	
	unsigned char Event2ASCII(const SInputEvent& event);
	unsigned char ToAscii(unsigned int vKeyCode, unsigned int k, unsigned char sKState[256]) const;
	TCHAR ToUnicode(unsigned int vKeyCode, unsigned int k, unsigned char sKState[256]) const;

protected:
	void	SetupKeyNames();
	void	ProcessKey(uint32 devSpecId, bool pressed);
	
private:
	static SScanCode			m_scanCodes[256];
	static SInputSymbol*		Symbol[256];

};

#endif