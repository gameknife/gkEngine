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
// Name:   	gkSwissKnifeBar.h
// Desc:		
// 	
// Author:  Kaiming
// Date:	2012/5/18
// Modify:	2012/5/18
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _gkSwissKnifeBar_h_
#define _gkSwissKnifeBar_h_
#include "SwissKnifePaneCreation.h"
#include "SwissKnifePaneModify.h"

class gkSwissKnifeBar
{
public:
	gkSwissKnifeBar() {}
	virtual ~gkSwissKnifeBar() {}

	void Create( CWnd* parent, CWnd* owner, UINT id );
	void Destroy();
	CWnd* getBar() {return &m_wndShortcutBar;}

	void RefreshObj(IGameObject* pTarget);

private:

	CXTPShortcutBar m_wndShortcutBar;


	// subpage
	CSwissKnifePaneCreation m_paneCreation;
	CSwissKnifePaneModify m_paneModify;

	CXTPShortcutBarItem* m_itemCreation;
	CXTPShortcutBarItem* m_itemModify;
};

#endif

