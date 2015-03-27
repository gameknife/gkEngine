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
// Name:   	gkStaticLibraryLoader.h
// Desc:		
// 	
// Author:  Kaiming
// Date:	2012/6/10
// Modify:	
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _gkStaticLibraryLoader_h_
#define _gkStaticLibraryLoader_h_

// 3dengine
#include "..\gkcore\include\gkRoot.h"

// gkRendererGLES2
#include "..\gkRendererGLES2\include\gkRendererGLES2.h"

// gkGameObjectSystem
#include "..\gkGameObjectSystem\include\gkGameObjectSystem.h"

#ifdef OS_WIN32
#include "..\gkHavok\include\dllmain.h"
#include "..\gkInput\include\gkInputManager.h"
//#include "gkNetwork.h"
#endif

struct gkStaticLibraryLoader
{
	gk3DEngine* m_p3DEngine;
	gkRendererGL* m_pRendererGLES2;
	gkGameObjectSystem* m_pGameObjectSystem;

#ifdef OS_WIN32
	//gkHavok* m_pHavok;
	gkInputManager* m_pInput;
#endif

	gkStaticLibraryLoader() {}
	~gkStaticLibraryLoader() {}


	void load()
	{
		m_p3DEngine = new gk3DEngine;
		m_pRendererGLES2 = new gkRendererGL;

		m_pGameObjectSystem = new gkGameObjectSystem;

#ifdef OS_WIN32
		//m_pHavok = new gkHavok;
		loadStaticHavok(gEnv);
		m_pInput = new gkInputManager;
#endif

		gEnv->p3DEngine = m_p3DEngine;
		gEnv->pRenderer = m_pRendererGLES2;
		gEnv->pGameObjSystem  = m_pGameObjectSystem;

#ifdef OS_WIN32
		//gEnv->pHavok = m_pHavok;
		gEnv->pInputManager = m_pInput;
#endif


	}

	void unload()
	{

		SAFE_DELETE( m_p3DEngine );
		SAFE_DELETE( m_pRendererGLES2 );
		SAFE_DELETE( m_pGameObjectSystem );

#ifdef OS_WIN32
		//SAFE_DELETE( m_pHavok );
		freeStaticHavok();
		SAFE_DELETE( m_pInput );
#endif

	}
};

#endif


