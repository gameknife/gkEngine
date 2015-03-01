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
// Name:   	gkGameFramework.h
// Desc:		
// 	
// Author:  Kaiming
// Date:	2012/4/22
// Modify:	2012/4/22
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _gkGameFramework_h_
#define _gkGameFramework_h_
#include "IGameFramework.h"
#include "IInputManager.h"

struct IGame;
struct SSystemGlobalEnvironment;

// non static lib, dynamic choose gamedll
#ifndef _STATIC_LIB
typedef IGame* (*GET_GAMEDLL)(SSystemGlobalEnvironment*);
typedef void (*DESTROY_GAMEDLL)(void);
#endif


class gkGameFramework : public IGameFramework, public IInputEventListener
{

public:
	gkGameFramework(void);
	virtual ~gkGameFramework(void) {}

	// System Creation And Destroy
	// init
	virtual bool Init( ISystemInitInfo& sii );
	virtual bool PostInit( HWND hWnd, ISystemInitInfo& sii );
	// run
	virtual void Run();
	// destroy
	virtual bool Destroy();
	// updating
	virtual bool Update();

	// GameDll Initializing & Destroying
	virtual bool InitGame(const TCHAR* dllname);
	virtual bool DestroyGame(bool deferred);
	virtual bool DestroyGameImmediate();



	// LevelLoading
	virtual void LoadLevel(const TCHAR* filename);
	virtual void OnLevelLoaded();

	// implement for IInputEventListener [2/11/2012 Kaiming]
	virtual bool OnInputEvent( const SInputEvent &event );

	bool AndroidFreeModeControl( const SInputEvent &event );
    bool AndroidMoveUpdate(float fElapsedTime);

	virtual bool OnInputEventUI( const SInputEvent &event ) {	return false;	}

	virtual SSystemGlobalEnvironment* getENV() {return gEnv;}

    virtual void markClose() {m_closeNextTime = true;}
    
private:
	#ifndef _STATIC_LIB
	GET_GAMEDLL			m_pFuncGameStart;
	DESTROY_GAMEDLL		m_pFuncGameEnd;
	#endif

	HINSTANCE				m_hHandleGame;

	IGame*				m_pGame;


	int				m_deferredDestroy;

	uint32			m_uDeviceRot;
	uint32			m_uDeviceMove;
	Vec2			m_vecMoveStartPos;
    
    Vec2            m_vecMoveDir;
    
    bool m_closeNextTime;
};

#endif
