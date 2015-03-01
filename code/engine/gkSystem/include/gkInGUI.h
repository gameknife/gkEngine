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



/**
  @file gkInGUI.h
  
  @author Kaiming

  更改日志 history
  ver:1.0
   
 */

#ifndef _gkInGUI_H_
#define _gkInGUI_H_
#include "gkSystemPrerequisites.h"
#include "IInputManager.h"

struct IFtFont;

struct gkPopupMsgStruct
{
	gkStdString m_msg;
	float m_time;
	float m_totalTime;

	gkPopupMsgStruct(const gkStdString& msg, float time):m_msg(msg), m_time(time), m_totalTime(time)
	{

	}
};

class gkInGUI : public IInputEventListener, public IInGUI
{
	enum EMouseState
	{
		Pressing,
		Hold,
		Releasing,
		Released,
	};

public:
	gkInGUI(void);
	~gkInGUI(void);

	void Init();
	void Shutdown();
	void Update(float fElapsedTime);

	uint32 gkGUITab(const TCHAR* title, const Vec2& pos, int width, int height, const ColorB& textColor, const ColorB& bgColor, int selection = 0, IFtFont* font = NULL);
	bool gkGUIButton(const TCHAR* title, const Vec2& pos, int width, int height, const ColorB& textColor, const ColorB& bgColor, IFtFont* font = NULL);
	void gkPopupMsg(const TCHAR* title);

	virtual bool OnInputEvent( const SInputEvent &event );

	EMouseState m_LMBState;
	Vec2 m_lastPoint;
	IFtFont* m_defaultFont;

	std::vector<gkPopupMsgStruct> m_popupMsgs;
};

#endif




