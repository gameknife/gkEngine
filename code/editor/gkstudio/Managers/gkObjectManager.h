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
// Name:   	gkObjectManager.h
// Desc:	物体管理器	
// 
// Author:  Kaiming-Desktop
// Date:	2011 /8/23
// Modify:	2011 /8/23
// 
//////////////////////////////////////////////////////////////////////////

#ifndef GKOBJECTMANAGER_H_
#define GKOBJECTMANAGER_H_

#include "IEditor.h"
#include "IInputManager.h"

struct IGameObject;

class gkObjectManager : IInputEventListener
{
public:
	gkObjectManager(void);
	virtual ~gkObjectManager(void);

	virtual void setCurrSelected(IGameObject* pEntity);
	virtual void setCurrHover(IGameObject* pEntity);
	virtual void Update();
	virtual void Destroy();
	virtual bool UpdateGizmoHover(uint8 selmode);
	virtual void UpdateGizmoDraging(uint8 selmode);
	void OnLButtonPressed();
	IGameObject* getCurrSelection();

	// implement for IInputEventListener [2/11/2012 Kaiming]
	virtual bool OnInputEvent( const SInputEvent &event );
	virtual bool OnInputEventUI( const SInputEvent &event ) {	return false;	}

	static uint8 ms_objselmode;

private:
	bool checkSelected( uint8 type, f32 size, bool draging = false );

private:
	static IGameObject* ms_pCurrentPick;
	static IGameObject* ms_pCurrentHover;
	static uint8 ms_selectedAxis;
	static int8 ms_dragInvertX;
	static int8 ms_dragInvertY;
	
	bool m_cloneMode;

	bool m_isDarging;
	bool m_bIsTargetDataReady;
	Vec3 m_vTargetPos;
	Quat m_qTargetRot;

	Vec3 m_vStartPos;

	Vec2i m_dragOffset;

	gkTexturePtr m_icon_light;

};


#endif

