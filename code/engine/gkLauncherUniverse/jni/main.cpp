//----------------------------------------------------------------------------------
// File:        native_basic/jni/main.cpp
// SDK Version: v10.14 
// Email:       tegradev@nvidia.com
// Site:        http://developer.nvidia.com/
//
// Copyright (c) 2007-2012, NVIDIA CORPORATION.  All rights reserved.
//
// TO  THE MAXIMUM  EXTENT PERMITTED  BY APPLICABLE  LAW, THIS SOFTWARE  IS PROVIDED
// *AS IS*  AND NVIDIA AND  ITS SUPPLIERS DISCLAIM  ALL WARRANTIES,  EITHER  EXPRESS
// OR IMPLIED, INCLUDING, BUT NOT LIMITED  TO, IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL  NVIDIA OR ITS SUPPLIERS
// BE  LIABLE  FOR  ANY  SPECIAL,  INCIDENTAL,  INDIRECT,  OR  CONSEQUENTIAL DAMAGES
// WHATSOEVER (INCLUDING, WITHOUT LIMITATION,  DAMAGES FOR LOSS OF BUSINESS PROFITS,
// BUSINESS INTERRUPTION, LOSS OF BUSINESS INFORMATION, OR ANY OTHER PECUNIARY LOSS)
// ARISING OUT OF THE  USE OF OR INABILITY  TO USE THIS SOFTWARE, EVEN IF NVIDIA HAS
// BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
//
//
//----------------------------------------------------------------------------------
#include "gkPlatform.h"

#ifdef OS_WIN32

#include "IGameframework.h"
#include "ISystem.h"
#include "gkPlatform_impl.h"

IGameFramework* gkLoadStaticModule_gkGameFramework();
void gkFreeStaticModule_gkGameFramework();

#include "gkIniParser.h"

#include "gkMemoryLeakDetecter.h"

typedef IGameFramework* (*GET_SYSTEM)(const char*);
typedef void (*DESTROY_END)(void);

//--------------------------------------------------------------------------------------
// Initialize everything and go into a render loop
//--------------------------------------------------------------------------------------
INT WINAPI wWinMain( HINSTANCE, HINSTANCE, LPWSTR, int )
{
	// Enable run-time memory check for debug builds.
#if (defined(DEBUG) || defined(_DEBUG)) && defined( OS_WIN32 )
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

#ifdef _STATIC_LIB
	IGameFramework* game = gkLoadStaticModule_gkGameFramework();
#else


	int width = 1280;
	int height = 720;

	char strExePath[MAX_PATH] = { 0 };
	char* strLastSlash = NULL;
	GetModuleFileNameA(NULL, strExePath, MAX_PATH);
	strExePath[MAX_PATH - 1] = 0;

	strLastSlash = strrchr(strExePath, '\\');
	if (strLastSlash)
		*strLastSlash = 0;

	strLastSlash = strrchr(strExePath, '\\');
	if (strLastSlash)
		*strLastSlash = 0;

	strcat(strExePath, "\\");


	// load gkSystem dll
	HINSTANCE hHandle = 0;
	gkOpenModule(hHandle, _T("gkGameFramework"));
	GET_SYSTEM pFuncStart = (GET_SYSTEM)DLL_GETSYM(hHandle, "gkModuleInitialize");
	IGameFramework* game = pFuncStart(NULL);
#endif


	// init
	ISystemInitInfo sii;
	sii.fWidth = width;
	sii.fHeight = height;
	sii.rootDir = strExePath;

	if (!(game->Init(sii)))
	{
		return 0;
	}


	game->PostInit( NULL, sii );

	game->InitGame( NULL );

	// run
	game->Run();

	// destroy
	game->DestroyGame(false);
	game->Destroy();

	// free
#ifdef _STATIC_LIB
	gkFreeStaticModule_gkGameFramework();
#else
	gkFreeModule( hHandle );
#endif
	return 0;
}

#elif defined( OS_ANDROID )

#include "ISystem.h"   

#include <EGL/egl.h>
#include <EGL/eglplatform.h>
#include <GLES2/gl2.h>

#include <android/log.h>
#include "nv_and_util/nv_native_app_glue.h"
//#include "nv_and_util/nv_native_app_glue.c"


#include "IGameFramework.h"  
#include "gkPlatform_impl.h"  
#include "IInputManager.h"   

IGameFramework* gGameFramework = 0;
typedef IGameFramework* (*STARTFUNC)(void);
void* gHandle = 0;
ISystemInitInfo gSii;
bool gIntialized = false;
bool exit_req = false;

/*!***********************************************************************
 @Function		OsDisplayDebugString
 @Input			str		string to output
 @Description	Prints a debug string
*************************************************************************/
 void OsDisplayDebugString(char const * const str)
 {
 	__android_log_print(ANDROID_LOG_INFO, "gkENGINE", str);
 } 
   
/*!***********************************************************************
 @Function		OsGetTime 
 @Return		An incrementing time value measured in milliseconds
 @Description	Returns an incrementing time value measured in milliseconds
*************************************************************************/
// unsigned long PVRShellInit::OsGetTime()
// {
// 	timeval tv;
// 	gettimeofday(&tv,NULL);
// 
// 	if(tv.tv_sec < m_StartTime.tv_sec)
// 		m_StartTime.tv_sec = 0;
// 
// 	unsigned long sec = tv.tv_sec - m_StartTime.tv_sec;
// 	return (unsigned long)((sec*(unsigned long)1000) + (tv.tv_usec/1000.0));
// }

/*****************************************************************************
 Class: PVRShellInitOS
*****************************************************************************/

/*****************************************************************************
 Global code
*****************************************************************************/
static int32_t handle_input(struct android_app* app, AInputEvent* event)
{
	//PVRShellInit* init = (PVRShellInit*) app->userData;

// 	if(init)
// 	{
		switch(AInputEvent_getType(event))
		{
			case AINPUT_EVENT_TYPE_KEY: // Handle keyboard events
			{
				switch(AKeyEvent_getAction(event))
				{
					case AKEY_EVENT_ACTION_DOWN:
					{
						switch(AKeyEvent_getKeyCode(event))
						{
// 							case AKEYCODE_Q:			init->KeyPressed(PVRShellKeyNameQUIT);		break;
// 							case AKEYCODE_DPAD_CENTER:	init->KeyPressed(PVRShellKeyNameSELECT);	break;
// 							case AKEYCODE_SPACE: 		init->KeyPressed(PVRShellKeyNameACTION1); 	break;
// 							case AKEYCODE_SHIFT_LEFT: 	init->KeyPressed(PVRShellKeyNameACTION2); 	break;
// 							case AKEYCODE_DPAD_UP: 		init->KeyPressed(init->m_eKeyMapUP); 		break;
// 							case AKEYCODE_DPAD_DOWN: 	init->KeyPressed(init->m_eKeyMapDOWN); 		break;
// 							case AKEYCODE_DPAD_LEFT: 	init->KeyPressed(init->m_eKeyMapLEFT);  	break;
// 							case AKEYCODE_DPAD_RIGHT: 	init->KeyPressed(init->m_eKeyMapRIGHT); 	break;
// 							case AKEYCODE_S: 			init->KeyPressed(PVRShellKeyNameScreenshot);break;
							default:
								break;
						}
					}
					return 1;

					default:
						break;
				}
				return 1;
			}
			case AINPUT_EVENT_TYPE_MOTION: // Handle touch events
			{
				switch(AMotionEvent_getAction(event))
				{
					case AMOTION_EVENT_ACTION_DOWN:
//						init->m_bTouching = true;
					case AMOTION_EVENT_ACTION_MOVE:
					{
// 						PVRShell *pShell = init->m_pShell;
// 
// 						if(pShell)
// 						{
// 							init->m_vec2PointerLocation[0] = AMotionEvent_getX(event, 0) / pShell->PVRShellGet(prefWidth);
// 							init->m_vec2PointerLocation[1] = AMotionEvent_getY(event, 0) / pShell->PVRShellGet(prefHeight);
// 							return 1;
// 						}
						break;
					}
					case AMOTION_EVENT_ACTION_UP:
/*						init->m_bTouching = false;*/
						return 1;
				}
				return 1;
			}
		}
//	}

    return 1;
}

static void handle_cmd(struct android_app* app, int32_t cmd)
{
 //   PVRShellInit* init = (PVRShellInit*) app->userData;

    switch (cmd)
    {
		case APP_CMD_START:
			{

			}

			break;	
		case APP_CMD_RESUME:
			//init->m_bRendering = true;
			break;
		case APP_CMD_PAUSE:
			//init->m_bRendering = false;
			break;
        case APP_CMD_SAVE_STATE:
			// PVRShell doesn't support saving our state
			//init->m_bRendering = false;
            break;
        case APP_CMD_INIT_WINDOW:
			{
				// Call init view
				gSii.nativeWindowPTR = (void*)(app->window);
				if (!(gSii.nativeWindowPTR))
				{
					OsDisplayDebugString( "window not created!!!!" );
				}
				gSii.fWidth = 854;
				gSii.fHeight = 480;
				OsDisplayDebugString( "preinit..." );
				if (!(gGameFramework->Init(gSii)) )
				{
					exit_req = true;
				}
				else
				{
					gGameFramework->PostInit(0, gSii);
					gGameFramework->InitGame(0);
					OsDisplayDebugString("windowinit...");

					app->onInputEvent = gGameFramework->getENV()->pInputManager->getAndroidHandler();
					OsDisplayDebugString("input device binded...");
					gIntialized = true;
				}

			}

				
            break;

		case APP_CMD_WINDOW_RESIZED:
			// Unsupported by the shell
			break;
        case APP_CMD_TERM_WINDOW: // The window is being hidden or closed.
           	// Call release view
//            	if(init->m_eState <= ePVRShellReleaseView)
//            	{
// 				init->m_eState = ePVRShellReleaseView;
// 				init->Run();
// 			}
// 
// 			init->m_bRendering = false;
            break;
        case APP_CMD_STOP:
//		    init->Deinit();

			gGameFramework->Destroy();
            break;
    }
}

/**
 * This is the main entry point of a native application that is using
 * android_native_app_glue.  It runs in its own thread, with its own
 * event loop for receiving input events and doing other things.
 */
void android_main(struct android_app* app)
{
    // Make sure glue isn't stripped.
    app_dummy();

// 	NvEGLUtil* egl = NvEGLUtil::create();
//     if (!egl) 
//     {
//         // if we have a basic EGL failure, we need to exit immediately; nothing else we can do
//         nv_app_force_quit_no_cleanup(app);
//         return;
//     }

	void* gHandle;
	LOAD_MODULE_GLOBAL( gHandle, gkGameFramework );

	//= dlopen("/data/data/com.kkstudio.gklauncher/lib/libgkGameFrame.so", RTLD_LAZY |RTLD_GLOBAL );
	OsDisplayDebugString( "libloaded..." );
	if(gHandle)
	{ 
		STARTFUNC moduleInitialize = (STARTFUNC)(DLL_GETSYM(gHandle, "gkModuleInitialize"));
		if( moduleInitialize )
		{
			gGameFramework = moduleInitialize();
		}
		else
		{
			return;
		}
	}
	else
	{
		OsDisplayDebugString( "gkFramework not find..." );
	}

	OsDisplayDebugString( "system initialized..." );

    //Engine* engine = new Engine(*egl, app);

	//long lastTime = egl->getSystemTime();

    // loop waiting for stuff to do.

	OsDisplayDebugString( "system initialized..." );
	// Setup our android state
	//state->userData = &init;

	app->onAppCmd = handle_cmd;
	app->onInputEvent = handle_input;

	while (nv_app_status_running(app))
    {
        // Read all pending events.
        int ident;
        int events;
        struct android_poll_source* source;

        // If not rendering, we will block 250ms waiting for events.
        // If animating, we loop until all events are read, then continue
        // to draw the next frame of animation.
        while ((ident = ALooper_pollAll(((nv_app_status_focused(app)/* && engine->isGameplayMode()*/) ? 1 : 250),
        								NULL,
        								&events,
        								(void**)&source)) >= 0)
        {
            // Process this event. 
            if (source != NULL)
                source->process(app, source);

            // Check if we are exiting.  If so, dump out
            if (!nv_app_status_running(app))
				break;
        }

		if (gIntialized)
		{
			gGameFramework->Update();
		}

		if (exit_req)
		{
			break;
		}

// 		long currentTime = egl->getSystemTime();
// 
// 		// clamp time - it must not go backwards, and we don't
// 		// want it to be more than a half second to avoid huge
// 		// delays causing issues.  Note that we do not clamp to above
// 		// zero because some tools will give us zero delta
// 		long deltaTime = currentTime - lastTime;
// 		if (deltaTime < 0)
// 			deltaTime = 0;
// 		else if (deltaTime > 500)
// 			deltaTime = 500;
// 
// 		lastTime = currentTime;

		// Update the frame, which optionally updates time and animations
		// and renders
		//engine->updateFrame(nv_app_status_interactable(app), deltaTime);
    }

	//delete engine;
    //delete egl;
}

#else


#endif
