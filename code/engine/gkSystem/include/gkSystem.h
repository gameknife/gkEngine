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
// Name:   	gkSystem.h
// Desc:		
// 
// Author:  Kaiming-Desktop
// Date:	2011 /7/30
// Modify:	2011 /7/30
// 
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "gkSystemPrerequisites.h"
#include "ISystem.h"
#include "gkTimer.h"
#include "gkConsoleVariablesManager.h"
#include "gkSystemProfiler.h"
#include "gkResFileManager.h"
//#include "gkStaticLibraryLoader.h"

class gkInGUI;

struct SModuleHandles
{
	HINSTANCE hInputLayer;
	HINSTANCE hNetworkLayer;
	HINSTANCE hAnimation;
	HINSTANCE hPhysics;
	HINSTANCE hRenderer;
	HINSTANCE h3DEngine;
	HINSTANCE hGameObjectSystem;
	HINSTANCE hFont;
	HINSTANCE hTrackBus;

	HINSTANCE hSound;
	HINSTANCE hVideo;
	HINSTANCE hStereoDevice;
};

struct gkSystemCVars;
class FileChangeMonitor;
class gkSystem : public ISystem
{
public:
	gkSystem();
	virtual ~gkSystem(void);

	// System Management
	virtual bool Init(ISystemInitInfo& sii);
	virtual bool PostInit(HWND hWnd,  ISystemInitInfo& sii);

	virtual bool Destroy();
	virtual bool Update();
	virtual bool PostUpdate();

	virtual SSystemGlobalEnvironment* getENV() {return gEnv;}

	virtual void SetEditorHWND( HWND drawHwnd, HWND mainHwnd);
	virtual bool IsEditor() {return m_bEditor;}

	virtual bool LoadScene(const TCHAR* filename);
	virtual bool LoadScene_s(const TCHAR* filename, IProgressCallback* pCallback);

	virtual bool SaveScene(const TCHAR* filename);

	virtual bool IsSceneEmtpy() {return m_bIsSceneEmpty;}

	virtual bool reloadResource_s(uint8 type, const gkStdString& name);

	virtual void cleanGarbage();

	virtual HWND getHWnd() {return m_hWnd;}

	// Resource Management
	// 资源管理//-
	virtual IResourceManager* getTextureMngPtr() {return m_pTextureManager;}
	virtual IResourceManager* getMaterialMngPtr() {return m_pMaterialManager;}
	virtual IResourceManager* getMeshMngPtr() {return m_pMeshManager;}
	virtual IResourceManager* getShaderMngPtr() {return m_pShaderManager;}
	
	virtual void updateProgress(int progress);

	virtual IMeshLoader* getOBJMeshLoader();
	virtual IMeshLoader* getHKXMeshLoader();
	virtual void ReturnMeshLoader(IMeshLoader* target);


private:
	SModuleHandles		m_moduleHandles;
	HWND				m_hWnd;
	gkTimer				m_Timer;
	gkConsoleVariablesManager	m_CVManager;
	gkSystemProfiler	m_Profiler;
	ILog*				m_pLog;
	gkResFileManager	m_ResFileManager;
	gkInGUI*			m_ingui;
	
	// resource management
	IResourceManager*	m_pTextureManager;
	IResourceManager*	m_pMeshManager;
	IResourceManager*	m_pMaterialManager;
	IResourceManager*	m_pShaderManager;

	bool				m_bEditor;
	bool				m_bIsSceneEmpty;

	gkStdString		m_wstrCurSceneFile;

	// thread safe stuffs [9/27/2011 Kaiming]
	gkStdString		m_wstrThreadCacheSceneFile;

	bool				m_threadLoadAquire;

	std::vector<gkStdString> m_vecMeshReloadTasks;

	gkSystemCVars*		m_pSystemCVars;

	IProgressCallback*	m_pLoadCallback;
	// when android device, update may call before init, so add this to ensure [6/7/2012 Kaiming]
	bool				m_bInitialized;

#ifdef OS_WIN32
	FileChangeMonitor*	m_fileMonitor;
#endif
	
	bool				m_bNeedGarbage;
};
