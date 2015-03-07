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
// Name:   	gkGameDemo.h
// Desc:		
// 	
// Author:  Kaiming
// Date:	2012/4/22
// Modify:	2012/4/22
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _gkGameDemo_h_
#define _gkGameDemo_h_

#include "IGame.h"
#include "IInputManager.h"


class gkGameDemo : public IGame, public IInputEventListener
{

public:
	gkGameDemo(void);
	virtual ~gkGameDemo(void) {}

	// init
	virtual bool OnInit();
	// destroy
	virtual bool OnDestroy();
	// updating
	virtual bool OnUpdate();

	// level loaded
	virtual bool OnLevelLoaded();


	// implement for IInputEventListener [2/11/2012 Kaiming]
	virtual bool OnInputEvent( const SInputEvent &event );

	void start_player();

	virtual bool OnInputEventUI( const SInputEvent &event ) {	return false;	}

private:

	class gkMainPlayer* m_pMainPlayer;
	bool				m_bUIMode;
	IFtFont*			m_subtitleFont;
};

#endif
