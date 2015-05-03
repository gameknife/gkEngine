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
// Name:   	IEditor.h
// Desc:	Editor接口，这里存放editor使用的所有类	
// 
// Author:  Kaiming-Desktop
// Date:	2011 /8/23
// Modify:	2011 /8/23
// 
//////////////////////////////////////////////////////////////////////////

#ifndef IEDITOR_H_
#define IEDITOR_H_

class gkObjectManager;
class CWorkspaceView;
class CMainFrame;
class gkSwissKnifeBar;
struct IGameObject;
class gkCameraPosManager;


enum GKSTUDIO_AXIS
{
	GKSTUDIO_AXIS_X = 1,
	GKSTUDIO_AXIS_Y,
	GKSTUDIO_AXIS_Z,

	GKSTUDIO_AXIS_XY,
	GKSTUDIO_AXIS_YZ,
	GKSTUDIO_AXIS_XZ,
};

enum GKSTUDIO_OBJMODE
{
	GKSTUDIO_OBJMODE_SEL = 0,
	GKSTUDIO_OBJMODE_MOV,
	GKSTUDIO_OBJMODE_ROT,
	GKSTUDIO_OBJMODE_SCL,
};

struct IEditorUpdateListener
{
	// Summary:
	//	 Auto call the panes or controls who need to accept update
	// Return Value:
	//	 If it returns True then the broadcasting of this event should be aborted and the rest of input 
	//	 listeners should not receive this event.
	virtual void OnUpdate( float fElpasedTime ) = 0;
};

struct UNIQUE_IFACE IEditor
{
	virtual ~IEditor() {}
	virtual gkObjectManager* getObjectMng() =0;
	virtual CWorkspaceView* getMainViewport() =0;
	virtual CMainFrame* getMainFrame() =0;
	virtual gkSwissKnifeBar* getSwissKnifeBar() =0;
	virtual gkCameraPosManager* getCameraPosManager() =0;

	virtual void Init(CMainFrame* pMainFrame) =0;
	virtual void Update() =0;
	virtual void Destroy() =0;

	virtual void SetObjectSelMode(uint8 selmode) =0;

	virtual void loadScene(LPCTSTR lpszPathName) =0;
	virtual void markSceneLoaded(int nProgress) =0;
	virtual void saveScene(LPCTSTR lpszPathName) =0;

	virtual void moveCameraTo( const Vec3& pos, const Quat& rot, float Damping = 0.1f ) =0;
	virtual void focusGameObject( IGameObject* pTarget, float Damping = 0.1f ) =0;

	virtual void addUpdateListener(IEditorUpdateListener* listener) =0;
	virtual void removeUpdateListener( IEditorUpdateListener* listener ) = 0;

	virtual Vec2 getMouseDelta() =0;

	virtual struct IFtFont* getDefaultFont() =0;
};

// Use only inside editor executable.
extern IEditor* GetIEditor();


#endif