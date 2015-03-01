/**********************************************************************
 *<
   FILE: prim.cpp

   DESCRIPTION:   DLL implementation of primitives

   CREATED BY: Dan Silva

   HISTORY: created 12 December 1994

 *>   Copyright (c) 1994, All Rights Reserved.
 **********************************************************************/

#include "gkMaxPort.h"

#include <process.h> 
#include "IGame.h"
//#include "IConversionManager.h"

#include "viewport/gmpMaxViewport.h"
#include "exporter/gmpExporter.h"
//#include "gmpObject.h"


#include "gkPlatform.h"
#include "gkPlatform_impl.h"
#include "ISystem.h"
#include "gkStudioWndMsg.h"

#include "IResourceManager.h"

#include "gkMemoryLeakDetecter.h"


HINSTANCE hInstance;

class IGameExporter;
IGameExporter* gExporter = NULL;

gkUtilityGlobalEnv* gUtilEnv = NULL;

static gkPortWindow gvw;



/** public functions **/
BOOL WINAPI DllMain(HINSTANCE hinstDLL,ULONG fdwReason,LPVOID lpvReserved) {
   switch(fdwReason) 
   {
   case DLL_PROCESS_ATTACH:
	   {
		   hInstance = hinstDLL;
		   DisableThreadLibraryCalls(hInstance);

		   gUtilEnv = new gkUtilityGlobalEnv;
		   gUtilEnv->hInst = hinstDLL;
		   gUtilEnv->m_bUpdate = false;

		   if(MessageBox(NULL, "Enable gkMaxPort? That allow u view Engine Effect in 3ds Max.", "gkMaxPort", MB_YESNO) == IDNO)
				break;

		   gUtilEnv->m_bMaxPorted = true;

		   uint32 threadID = 0;
		   uint32 threadAddress = 0;
		   // register gkMaxPort Here [9/20/2011 Kaiming]
		   //gvw = NEW gkPortWindow();
		   GetCOREInterface()->RegisterViewWindow(&gvw);
		   //GetCOREInterface()->RegisterViewportDisplayCallback()

		   
		   //threadID = ::GetCurrentThreadId();
		   //::_beginthreadex(0, 0, gkMaxPortThread, (void*)&threadID, 0, &threadAddress);

		   gUtilEnv->Init();

	   }




	   break;
   case DLL_THREAD_ATTACH:
	   break;
   case DLL_THREAD_DETACH:
	   break;
   case DLL_PROCESS_DETACH:
	   gUtilEnv->Destroy();
	   delete gUtilEnv;
	   break;
   }
   return(TRUE);

   return(TRUE);
}

//------------------------------------------------------
// This is the interface to Jaguar:
//------------------------------------------------------

__declspec( dllexport ) const TCHAR *
LibDescription() { return GetString(IDS_LIBDESCRIPTION); }

/// MUST CHANGE THIS NUMBER WHEN ADD NEW CLASS
__declspec( dllexport ) int LibNumberClasses()
{
#ifdef NO_OBJECT_STANDARD_PRIMITIVES
   return 0;
#else
   return 2;
#endif
}

__declspec( dllexport ) ClassDesc*
LibClassDesc(int i) {
#ifdef NO_OBJECT_STANDARD_PRIMITIVES
   return 0;
#else
   switch(i) {
   case 0: return GetgkObjectInMaxDesc();
   case 1: return GetIGameExporterDesc();
   default: return 0;
   }
#endif
}

// Return version so can detect obsolete DLLs
__declspec( dllexport ) ULONG 
LibVersion() { return VERSION_3DSMAX; }

// Let the plug-in register itself for deferred loading
__declspec( dllexport ) ULONG CanAutoDefer()
{
   return 0;
}

TCHAR *GetString(int id)
   {
   static TCHAR buf[256];

   if (hInstance)
      return LoadString(hInstance, id, buf, sizeof(buf)) ? buf : NULL;
   return NULL;
   }

void getEngineRoot( TCHAR* path )
{
	if (path)
	{
		// load from engine path [9/26/2011 Kaiming]
		TCHAR cfgpath[MAX_PATH];
		GetModuleFileName( NULL, cfgpath, MAX_PATH );

		// 砍一层目录
		char* strLastSlash = NULL; 
		strLastSlash = strrchr( cfgpath, '\\' );
		if( strLastSlash )
			*strLastSlash = 0;

		strcat_s(cfgpath, MAX_PATH, "\\plugins\\config.txt");
		//strcat_s(cfgpath, MAX_PATH, "config.cfg");
		GetPrivateProfileStringA( "Engine", "EnginePath", "", path, MAX_PATH, cfgpath);
	}
}

void gkUtilityGlobalEnv::Init()
{
// 	IGameScene* pIgame = GetIGameInterface();
// 
// 	IGameConversionManager * cm = GetConversionManager();
// 	//	cm->SetUserCoordSystem(Whacky);
// 
// 	// set as 3dxmax anyway [12/3/2011 Kaiming]
// 	cm->SetCoordSystem(IGameConversionManager::IGAME_MAX);
// 	//cm->SetCoordSystem((IGameConversionManager::CoordSystem)cS);
// 	//	pIgame->SetPropertyFile(_T("hello world"));
// 
// 
// 	// IGAME initialized
// 	pIgame->InitialiseIGame(false);
// 	pIgame->SetStaticFrame(0);
}

void gkUtilityGlobalEnv::Destroy()
{
// 	IGameScene* pIgame = GetIGameInterface();
// 	pIgame->ReleaseIGame();
}
