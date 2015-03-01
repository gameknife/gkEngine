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
// Name:   	gkMouse.h
// Desc:		
// 	
// Author:  Kaiming
// Date:	2012/2/9
// Modify:	2012/2/9
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _GKMOUSE_H_
#define _GKMOUSE_H_

#include "gkInputDeviceBase.h"

class gkMouse : public gkInputDevice
{
public:
	gkMouse(gkInputManager& input);

	// IInputDevice overrides
	virtual bool Init();
	virtual void Update(bool bFocus);
	virtual bool SetExclusiveMode(bool value);
	// ~IInputDevice

private:
	void PostEvent(SInputSymbol* pSymbol);
	void PostOnlyIfChanged(SInputSymbol* pSymbol, EInputState newState);

	//smooth movement & mouse accel
	void CapDeltas(float cap);
	void SmoothDeltas(float accel,float decel=0.0f);

	Vec2						m_deltas;
	Vec2						m_oldDeltas;
	Vec2						m_deltasInertia;
	float						m_mouseWheel;

	const static int MAX_MOUSE_SYMBOLS = eKI_MouseLast-KI_MOUSE_BASE;
	static SInputSymbol*	Symbol[MAX_MOUSE_SYMBOLS];
};



#endif