#include "StableHeader.h"
#include "gkRenderContextOSX.h"
#include "gkRendererGL330.h"

#include "IInputManager.h"
#include "IGameFramework.h"

int lastx = 0;
int lasty = 0;

SInputSymbol* g_mouseSymbol[3];
SInputSymbol* g_keyboardSymbol[512];

static void cursor_position_callback(int x, int y)
{
    static bool first = true;
    
    if (first) {
        first = false;
        lastx = x;
        lasty = y;
    }

    
    if(gEnv->pInputManager)
    {
        SInputEvent event;
        event.deviceId = eDI_Mouse;
        event.state = eIS_Changed;
        
        event.keyId = eKI_MouseX;
        event.value = x - lastx;
        lastx = x;
        event.value2 = x;
        
        gEnv->pInputManager->PostInputEvent(event);
        
        event.keyId = eKI_MouseY;
        event.value = y - lasty;
        lasty = y;
        event.value2 = y;
        
        gEnv->pInputManager->PostInputEvent(event);
        
    }
}

static void mouse_button_callback(int Button, int Action)
{
    if(!(gEnv->pInputManager))
    {
        return;
    }
    SInputEvent event;
    event.deviceId = eDI_Mouse;
    
            switch(Button)
            {
                case GLFW_MOUSE_BUTTON_LEFT:
                {
                    event.pSymbol = g_mouseSymbol[0];
                    if (Action == GLFW_PRESS)
                    {
                        event.pSymbol->state = eIS_Pressed;
                    }
                    else
                    {
                        event.pSymbol->state = eIS_Released;
                    }
                    event.pSymbol->value = lastx;
                    event.pSymbol->value2 = lasty;
                    
                    event.pSymbol->AssignTo(event);
                    
                    gEnv->pInputManager->PostInputEvent(event);
                }
					break;
                case GLFW_MOUSE_BUTTON_MIDDLE:
                {
                    event.pSymbol = g_mouseSymbol[2];
                    if (Action == GLFW_PRESS)
                    {
                        event.pSymbol->state = eIS_Pressed;
                    }
                    else
                    {
                        event.pSymbol->state = eIS_Released;
                    }
                    event.pSymbol->value = lastx;
                    event.pSymbol->value2 = lasty;
                    
                    event.pSymbol->AssignTo(event);
                    
                    gEnv->pInputManager->PostInputEvent(event);
                }
					break;
                case GLFW_MOUSE_BUTTON_RIGHT:
                {
                    event.pSymbol = g_mouseSymbol[1];
                    if (Action == GLFW_PRESS)
                    {
                        event.pSymbol->state = eIS_Pressed;
                    }
                    else
                    {
                        event.pSymbol->state = eIS_Released;
                    }
                    event.pSymbol->value = lastx;
                    event.pSymbol->value2 = lasty;
                    
                    event.pSymbol->AssignTo(event);
                    
                    gEnv->pInputManager->PostInputEvent(event);
                }
					break;
            }
}

static void GLFWCALL key_callback(int key, int action)
{
    SInputEvent event;
    
//    switch(action)
//    {
//        case GLFW_PRESS:
//        {
    
            event.pSymbol = g_keyboardSymbol[key];
            if ( event.pSymbol )
            {
                if (action == GLFW_PRESS)
                {
                    event.pSymbol->state = eIS_Pressed;
                }
                else
                {
                    event.pSymbol->state = eIS_Released;
                }
                event.pSymbol->value = 1.0f;
                
                event.pSymbol->AssignTo(event);
                
                gEnv->pInputManager->PostInputEvent(event);
            }

    
    
//        break;
//        }
//        case GLFW_RELEASE:
//        {
//
//            break;
//        }
//    }
}

static int GLFWCALL close_callback(void)
{
    gEnv->pGameFramework->markClose();
}

gkDeviceRenderContext::gkDeviceRenderContext()
{

}

SInputSymbol* MapKeyboardSymbol( uint32 GLFW_KEY, EKeyId gkENGINE_Key )
{
    SInputSymbol* ret = new SInputSymbol(GLFW_KEY, gkENGINE_Key, SInputSymbol::Button);
    ret->deviceId = eDI_Keyboard;
    
    return ret;
}

#define MAP_KEY( X, Y ) g_keyboardSymbol[X] = MapKeyboardSymbol(X, eKI_##Y);  

HWND gkDeviceRenderContext::initDevice(ISystemInitInfo& sii)
{
    g_mouseSymbol[0] = new SInputSymbol(GLFW_MOUSE_BUTTON_LEFT, eKI_Mouse1, SInputSymbol::Button);
    g_mouseSymbol[1] = new SInputSymbol(GLFW_MOUSE_BUTTON_LEFT, eKI_Mouse2, SInputSymbol::Button);
    g_mouseSymbol[2] = new SInputSymbol(GLFW_MOUSE_BUTTON_LEFT, eKI_Mouse3, SInputSymbol::Button);
    
    g_mouseSymbol[0]->deviceId = eDI_Mouse;
    g_mouseSymbol[1]->deviceId = eDI_Mouse;
    g_mouseSymbol[2]->deviceId = eDI_Mouse;
    
    MAP_KEY( '1', 1 );
    MAP_KEY( '2', 2 );
    MAP_KEY( '3', 3 );
    MAP_KEY( '4', 4 );
    MAP_KEY( '5', 5 );
    MAP_KEY( '6', 6 );
    MAP_KEY( '7', 7 );
    MAP_KEY( '8', 8 );
    MAP_KEY( '9', 9 );
    MAP_KEY( '0', 0 );
    
    MAP_KEY( 'Q', Q );
    MAP_KEY( 'W', W );
    MAP_KEY( 'E', E );
    MAP_KEY( 'R', R );
    MAP_KEY( 'T', T );
    MAP_KEY( 'Y', Y );
    MAP_KEY( 'U', U );
    MAP_KEY( 'I', I );
    MAP_KEY( 'O', O );
    MAP_KEY( 'P', P );
    
    MAP_KEY( 'A', A );
    MAP_KEY( 'S', S );
    MAP_KEY( 'D', D );
    MAP_KEY( 'F', F );
    MAP_KEY( 'G', G );
    MAP_KEY( 'H', H );
    MAP_KEY( 'J', J );
    MAP_KEY( 'K', K );
    MAP_KEY( 'L', L );
    
    MAP_KEY( 'Z', Z );
    MAP_KEY( 'X', X );
    MAP_KEY( 'C', C );
    MAP_KEY( 'V', V );
    MAP_KEY( 'B', B );
    MAP_KEY( 'N', N );
    MAP_KEY( 'M', M );
    MAP_KEY( 'K', K );
    
    //eKI_LShift
    
    MAP_KEY(  GLFW_KEY_UP  , Up);
    MAP_KEY(  GLFW_KEY_DOWN  , Down);
    MAP_KEY(  GLFW_KEY_LEFT   ,      Left);
    MAP_KEY(  GLFW_KEY_RIGHT        , Right);
    MAP_KEY(  GLFW_KEY_LSHIFT       , LShift);
    MAP_KEY(  GLFW_KEY_RSHIFT  , RShift);
    MAP_KEY(  GLFW_KEY_LCTRL      , LCtrl);
    MAP_KEY(  GLFW_KEY_RCTRL      , RCtrl);
    MAP_KEY(  GLFW_KEY_LALT , LAlt);
    MAP_KEY(  GLFW_KEY_RALT, RAlt);
    MAP_KEY(  GLFW_KEY_TAB, Tab);
    MAP_KEY(  GLFW_KEY_ENTER, Enter);
    MAP_KEY(  GLFW_KEY_BACKSPACE, Backspace);
    MAP_KEY(  GLFW_KEY_INSERT, Insert);
    MAP_KEY(  GLFW_KEY_DEL, Delete);

    MAP_KEY(  GLFW_KEY_PAGEUP, PgUp);
    MAP_KEY(  GLFW_KEY_PAGEDOWN, PgDn);
    MAP_KEY(  GLFW_KEY_HOME, Home);
    MAP_KEY(  GLFW_KEY_END, End);
    
    MAP_KEY(  GLFW_KEY_SPACE, Space);

    MAP_KEY(  GLFW_KEY_ESC, Escape);
    MAP_KEY(  GLFW_KEY_F1, F1);
    MAP_KEY(  GLFW_KEY_F2, F2);
    MAP_KEY(  GLFW_KEY_F3, F3);
    MAP_KEY(  GLFW_KEY_F4, F4);
    MAP_KEY(  GLFW_KEY_F5, F5);
    MAP_KEY(  GLFW_KEY_F6, F6);
    MAP_KEY(  GLFW_KEY_F7, F7);
    MAP_KEY(  GLFW_KEY_F8, F8);
    MAP_KEY(  GLFW_KEY_F9, F9);
    MAP_KEY(  GLFW_KEY_F10, F10);
    MAP_KEY(  GLFW_KEY_F11, F11);
    MAP_KEY(  GLFW_KEY_F12, F12);

    glfwInit();
    
    glfwEnable(GLFW_AUTO_POLL_EVENTS); /* No explicit call to glfwPollEvents() */
    
    glfwOpenWindowHint(GLFW_WINDOW_NO_RESIZE, GL_FALSE);
    //if(version(Major, Minor) >= version(3, 2))
    {
        glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
        glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 2);
        
        glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        
#			if defined(NDEBUG)
        glfwOpenWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_FALSE);
#			else
        glfwOpenWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_FALSE);
#			endif
    }
    GLboolean Result = glfwOpenWindow(sii.fWidth, sii.fHeight, 0, 0, 0, 0, 24, 8, GLFW_WINDOW);
    assert(Result == GL_TRUE);
    
//    glfwSetWindowTitle(argv[0]);
    glfwSetMouseButtonCallback(mouse_button_callback);
    glfwSetMousePosCallback(cursor_position_callback);
    glfwSetWindowCloseCallback(close_callback);
    glfwSetKeyCallback(key_callback);

    glfwSwapInterval(0);
    
    return (HWND)1;
}

bool gkDeviceRenderContext::destroyDevice()
{
    glfwTerminate();
}

void gkDeviceRenderContext::swapBuffer()
{
    glfwSwapBuffers();
}

gkDeviceRenderContext::~gkDeviceRenderContext()
{

}
