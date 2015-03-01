/******************************************************************************

 @File         PVRShellOS.cpp

 @Title        Android/PVRShellOS

 @Version      

 @Copyright    Copyright (c) Imagination Technologies Limited.

 @Platform     Non-windowed support for any Linux

 @Description  Makes programming for 3D APIs easier by wrapping window creation
               and other functions for use by a demo.

******************************************************************************/
// #include "PVRShell.h"
// #include "PVRShellAPI.h"
// #include "PVRShellOS.h" 
// #include "PVRShellImpl.h"
#include "..\android_native_app_glue.h"

#include <stdio.h>
#include <stdarg.h> 
#include <unistd.h>
#include <string.h>   

#include <android/sensor.h>  
#include <android/log.h> 
#include <android/window.h> 
   
#include "ISystem.h"   
#include "IGameFramework.h"  
#include "gkPlatform_impl.h"  
#include "IInputManager.h"   

IGameFramework* gGameFramework = 0;
typedef IGameFramework* (*STARTFUNC)(void);
void* gHandle = 0;
ISystemInitInfo gSii;
bool gIntialized = false;

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
				gGameFramework->Init(gSii);
				gGameFramework->PostInit( 0, gSii);
				gGameFramework->InitGame( 0 );
				OsDisplayDebugString( "windowinit..." );

				app->onInputEvent = gGameFramework->getENV()->pInputManager->getAndroidHandler();
				OsDisplayDebugString( "input device binded..." );
				gIntialized = true;
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



/*!***************************************************************************
@function		android_main
@input			state	the android app state
@description	Main function of the program
*****************************************************************************/
void android_main(struct android_app* state)
{
	 // Make sure glue isn't stripped.
    app_dummy();

	OsDisplayDebugString( "coming..." );

	// check the tf card dog.
	//system(  );

	

    // Initialise the demo, process the command line, create the OS initialiser.

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

	OsDisplayDebugString( "system initialized..." );
	// Setup our android state
	//state->userData = &init;

	state->onAppCmd = handle_cmd;
	state->onInputEvent = handle_input;
	
	//init.m_pAndroidState = state;
	//g_AssetManager = state->activity->assetManager;

	// Handle our events until we have a valid window or destroy has been requested
	int ident;
	int events;
    struct android_poll_source* source;

	//	Initialise our window/run/shutdown
	for(;;)
	{
		while ((ident = ALooper_pollAll(/*(init.m_eState == ePVRShellRender && init.m_bRendering) ? 0 : -1*/ 0, NULL, &events, (void**)&source)) >= 0)
		{
			// Process this event.
			if (source != NULL)
			{
				source->process(state, source);
			}

			// Check if we are exiting.
			if (state->destroyRequested != 0)
			{
				return;
			}
		}

		// Render our scene
// 		if(!init.Run())
// 		{
// 			ANativeActivity_finish(state->activity);
// 			break;
// 		}
		if (gIntialized)
		{
			gGameFramework->Update();
		}
		
// 		if( gGameFramework->Update() )
// 		{
// 			gGameFramework->PostUpdate();
// 		}
		
	}
}


/*****************************************************************************
 End of file (PVRShellOS.cpp)
*****************************************************************************/


