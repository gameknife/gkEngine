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
// Name:   	gkEditor.h
// Desc:	Editor总类实现	
// 
// Author:  Kaiming-Desktop
// Date:	2011 /8/23
// Modify:	2011 /8/23
// 
//////////////////////////////////////////////////////////////////////////

#ifndef GKEDITOR_H_
#define GKEDITOR_H_

#include "IEditor.h"
#include "ISystem.h"
#include "IInputManager.h"

class CMainFrame;
class CWorkspaceView;
class gkSwissKnifeBar;
class gkCameraPosManager;

class CSceneLoadCallback : public IProgressCallback
{
	virtual void OnUpdateProgress( int nProgress );

	virtual void OnInitProgress( const TCHAR* progressText );

};

class gkEditor : public IEditor, public IInputEventListener
{
public:
	gkEditor();
	virtual ~gkEditor();


	virtual gkObjectManager* getObjectMng() {return m_pObjMng;}
	virtual CWorkspaceView* getMainViewport() {return m_pMainVP;}
	virtual CMainFrame* getMainFrame() {return m_pMainFrame;}
	virtual gkCameraPosManager* getCameraPosManager() {return m_pCamPosMng;}	
	virtual gkSwissKnifeBar* getSwissKnifeBar();
	
	virtual void Init(CMainFrame* pMainFrame);
	virtual void Update();

	virtual void Destroy();

	virtual void SetObjectSelMode(uint8 selmode);

	virtual void addUpdateListener(IEditorUpdateListener* listener);
	virtual void removeUpdateListener( IEditorUpdateListener* listener );

	virtual void loadScene(LPCTSTR lpszPathName);
	virtual void markSceneLoaded(int nProgress);
	virtual void saveScene(LPCTSTR lpszPathName);

	virtual void moveCameraTo( const Vec3& pos, const Quat& rot, float Damping = 0.1f );
	virtual void focusGameObject( IGameObject* pTarget, float Damping = 0.1f );

	// implement for IInputEventListener [2/11/2012 Kaiming]
	virtual bool OnInputEvent( const SInputEvent &event );
	virtual bool OnInputEventUI( const SInputEvent &event ) {	return false;	}

	virtual struct IFtFont* getDefaultFont() {return m_defaultFont;}

	virtual Vec2 getMouseDelta() { return m_mouseDelta; }

protected:
	CMainFrame* m_pMainFrame;
	gkObjectManager* m_pObjMng;
	CWorkspaceView* m_pMainVP;
	CSceneLoadCallback m_pLoadCallback;
	gkCameraPosManager* m_pCamPosMng;

	bool	m_bIsSceneEmpty;
	bool	m_bGameMode;

	Quat		m_camTargetQuat;
	Vec3		m_camTargetPos;
	Vec2		m_mouseDelta;
	float		m_camDamping;

	struct IFtFont*	m_defaultFont;

	typedef std::list<IEditorUpdateListener*> TEditorUpdateListener;
	TEditorUpdateListener m_listeners;

protected:
	TCHAR backupSceneFile[MAX_PATH];
};

#endif
