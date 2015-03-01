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



#include "IInputManager.h"
//////////////////////////////////////////////////////////////////////////
//
// K&K Studio GameKnife ENGINE Source File
//
// Name:   	gkMainPlayer.h
// Desc:		
// 	
// Author:  Kaiming
// Date:	2012/4/23
// Modify:	2012/4/23
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _gkMainPlayer_h_
#define _gkMainPlayer_h_

#include "IInputManager.h"

enum EMainPlayer_Status
{
	eMPS_Idle,
	eMPS_Walk,
	eMPS_Run,
};


class gkMainPlayer : public IInputEventListener
{
public:
	gkMainPlayer();
	virtual ~gkMainPlayer() {}

	bool Init(const Quat& camrot = Quat::CreateIdentity());
	bool Update( float fElapsedTime, bool uiMode );
	bool Destroy();

	bool TeleportPlayer(const Vec3& worldpos, const Quat& rotation = Quat::CreateIdentity());

	// implement for IInputEventListener [2/11/2012 Kaiming]
	virtual bool OnInputEvent( const SInputEvent &event );
	virtual bool OnInputEventUI( const SInputEvent &event ) {	return false;	}

private:
	void OnActionForward();
	void OnActionBackward();
	void OnActionTurn();
	void OnActionTurnCam();

	void UpdateCam();


	void OnPlayerMoving(float fElapsedTime, int Direction);
	void AdjustFacing();
	void LastReleaseJudge(Vec3& lastDirection);
private:
	IGameObject* m_pPlayer;
	IGameObjectAnimLayer* m_pPlayerAnim;
	IGameObjectPhysicLayer* m_pPlayerPhysic;

	EMainPlayer_Status			m_nCurrPlayerStatus;
	EMainPlayer_Status			m_nPrevPlayerStatus;
	float						m_fSpeed;
	float						m_fMaxSpeed;
	float						m_fAccelaration;
	float						m_fCamDistance;
	int							m_nDirection;
	Vec3						m_vecDirection;
	Vec3						m_vecFinalDirection;
	Quat						m_quatDesireCam;
	bool						m_bUiMode;
	bool						m_bInitialized;

	Quat						m_quatDesirePlayerOrien;

};

#endif
