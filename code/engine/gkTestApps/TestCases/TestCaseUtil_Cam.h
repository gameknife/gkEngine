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
// Name:   	TestCaseUtil_Cam.h
// Desc:	
// 	
// 
// Author:  Kaiming
// Date:	2013/3/29	
// 
//////////////////////////////////////////////////////////////////////////

#ifndef TestCaseUtil_Cam_h__
#define TestCaseUtil_Cam_h__

#include "ICamera.h"
#include "I3DEngine.h"
#include "IAuxRenderer.h"

#define ANDROID_MOVETUMBE_STALLPIXEL 5

inline bool TouchDeviceFreeCamHandle( const SInputEvent &event )
{
    static uint8 m_uDeviceMove;
    static Vec2 m_vecMoveDir = Vec2(0,0);
    static uint8 m_uDeviceRot;
    static Vec2 m_vecMoveStartPos = Vec2(0,0);
    
    float fElapsedTime = gEnv->pTimer->GetFrameTime();
    
    static bool moveNext = false;
    
    if(moveNext)
    {
        gEnv->p3DEngine->getMainCamera()->moveLocal( Vec3(fElapsedTime * m_vecMoveDir.x * 0.1f, fElapsedTime * m_vecMoveDir.y * 0.1f, 0) );
        moveNext = false;
    }
    
    int32 halfWidth = gEnv->pRenderer->GetScreenWidth() / 2;
    //int32 halfHeight = gEnv->pRenderer->GetScreenHeight() / 2;
    //float fFrameTime = gEnv->pTimer->GetFrameTime();
    
    if (event.keyId == eKI_Android_Touch && event.state == eIS_Pressed)
    {
        // judge left or right
        if ( event.value < halfWidth )
        {
            // left
            m_uDeviceMove = event.deviceIndex;
            m_vecMoveStartPos = Vec2( event.value, event.value2 );
        }
        else if ( event.value >= halfWidth )
        {
            m_uDeviceRot = event.deviceIndex;
            
        }
        
        //gkLogMessage(_T("Touch: %d | pos: %.1f, %.1f"), event.deviceIndex, event.value, event.value2);
        
        return false;
    }
    if (event.keyId == eKI_Android_Touch && event.state == eIS_Released)
    {
        if ( m_uDeviceMove == event.deviceIndex )
        {
            m_uDeviceMove = -1;
            m_vecMoveDir.set(0,0);
        }
        else if ( m_uDeviceRot == event.deviceIndex  )
        {
            m_uDeviceRot = -1;
        }
        
        //gkLogMessage(_T("Release: %d"), event.deviceIndex);
        return false;
    }
    if (event.keyId == eKI_Android_DragX && event.state == eIS_Changed)
    {
        //gkLogMessage(_T("moving: %d"), event.deviceIndex);
        if (event.deviceIndex == m_uDeviceRot)
        {
            //gkLogMessage(_T("Rotating: %d"), event.deviceIndex);
            Quat qBefore = gEnv->p3DEngine->getMainCamera()->getDerivedOrientation();
            Ang3 aRot(qBefore);
            
            aRot.z -= event.value * 0.002f;
            //aRot.x -= vMouseDelta.y * 0.002f;
            
            Quat qRot = Quat::CreateRotationXYZ(aRot);
            qRot.Normalize();
            gEnv->p3DEngine->getMainCamera()->setOrientation(qRot);
        }
        else if (event.deviceIndex == m_uDeviceMove)
        {
            //gkLogMessage(_T("MoveX: %d | %.1f, %.1f"), event.deviceIndex, event.value, event.value2);
            if ( event.value2 < (m_vecMoveStartPos.x - ANDROID_MOVETUMBE_STALLPIXEL) )
            {
                //moveleft
                m_vecMoveDir.x = event.value2 - (m_vecMoveStartPos.x - ANDROID_MOVETUMBE_STALLPIXEL);
            }
            else if (  event.value2 > (m_vecMoveStartPos.x + ANDROID_MOVETUMBE_STALLPIXEL) )
            {
                // moveright
                m_vecMoveDir.x = event.value2 - (m_vecMoveStartPos.x - ANDROID_MOVETUMBE_STALLPIXEL);
            }
            else {
                m_vecMoveDir.x = 0;
            }
            moveNext = true;
        }
        
        return false;
    }
    if (event.keyId == eKI_Android_DragY && event.state == eIS_Changed)
    {
        if (event.deviceIndex == m_uDeviceRot)
        {
            Quat qBefore = gEnv->p3DEngine->getMainCamera()->getDerivedOrientation();
            Ang3 aRot(qBefore);
            
            aRot.x -= event.value * 0.002f;
            //aRot.x -= vMouseDelta.y * 0.002f;
            
            Quat qRot = Quat::CreateRotationXYZ(aRot);
            qRot.Normalize();
            gEnv->p3DEngine->getMainCamera()->setOrientation(qRot);
        }
        else if (event.deviceIndex == m_uDeviceMove)
        {
            //gkLogMessage(_T("MoveY: %d | %.1f, %.1f"), event.deviceIndex, event.value, event.value2);
            if ( event.value2 < (m_vecMoveStartPos.y - ANDROID_MOVETUMBE_STALLPIXEL) )
            {
                //moveback
                m_vecMoveDir.y = (m_vecMoveStartPos.y - ANDROID_MOVETUMBE_STALLPIXEL) - event.value2;
                
            }
            else if (  event.value2 > (m_vecMoveStartPos.y + ANDROID_MOVETUMBE_STALLPIXEL) )
            {
                // moveforward
                m_vecMoveDir.y = (m_vecMoveStartPos.y - ANDROID_MOVETUMBE_STALLPIXEL) - event.value2;
            }
            else {
                m_vecMoveDir.y = 0;
            }
            moveNext = true;
            
        }
        return false;
    }
    
    return false;
}

inline bool HandleFreeCam( const SInputEvent &input_event )
{
	float fFrameTime = gEnv->pTimer->GetFrameTime();
	static float speed = 1.0f;

	switch ( input_event.deviceId )
	{
	case eDI_Mouse:
		{
			static bool holdingR = false;
			static bool holdingL = false;
			static bool holdingM = false;
			if (input_event.keyId == eKI_Mouse2)
			{
				if (input_event.state == eIS_Down)
				{
					holdingR = true;
				}
				else
				{
					holdingR = false;
				}
			}
			else if (input_event.keyId == eKI_Mouse1)
			{
				if (input_event.state == eIS_Down)
				{
					holdingL = true;
				}
				else
				{
					holdingL = false;
				}
			}
			else if (input_event.keyId == eKI_Mouse3)
			{
				if (input_event.state == eIS_Down)
				{
					holdingM = true;
				}
				else
				{
					holdingM = false;
				}
			}
			else if (input_event.keyId == eKI_MouseX && holdingR)
			{
				Quat qBefore = gEnv->p3DEngine->getMainCamera()->getDerivedOrientation();
				Ang3 aRot(qBefore);
				aRot.z -= input_event.value * 0.005f;
				Quat qRot = Quat::CreateRotationXYZ(aRot);
				qRot.Normalize();
				gEnv->p3DEngine->getMainCamera()->setOrientation(qRot);				
			}
			else if (input_event.keyId == eKI_MouseY && holdingR)
			{
				Quat qBefore = gEnv->p3DEngine->getMainCamera()->getDerivedOrientation();
				Ang3 aRot(qBefore);
				aRot.x -= input_event.value * 0.005f;
				Quat qRot = Quat::CreateRotationXYZ(aRot);
				qRot.Normalize();
				gEnv->p3DEngine->getMainCamera()->setOrientation(qRot);				
			}
			else if (input_event.keyId == eKI_MouseY && (holdingM))
			{
				//m_camHeight -= input_event.value * 0.005f;			
			}
			else if (input_event.keyId == eKI_MouseWheelUp)
			{
				//m_camDist -= 0.2f;
			}
			else if (input_event.keyId == eKI_MouseWheelDown)
			{
				//m_camDist += 0.2f;
			}
			break;
		}
	case eDI_Keyboard:
		{
			if (input_event.keyId == eKI_LShift && input_event.state == eIS_Down)
			{
				speed = 20.0f;
			}
			else if (input_event.keyId == eKI_LShift && input_event.state == eIS_Released)
			{
				speed = 0.5f;
			}
			else if (input_event.keyId == eKI_W)
			{
				gEnv->p3DEngine->getMainCamera()->moveLocal( Vec3(0, fFrameTime * speed, 0) );
			}
			else if (input_event.keyId == eKI_S)
			{
				gEnv->p3DEngine->getMainCamera()->moveLocal( Vec3(0, -fFrameTime * speed, 0 ) );
			}
			else if (input_event.keyId == eKI_A)
			{
				gEnv->p3DEngine->getMainCamera()->moveLocal( Vec3(-fFrameTime * speed, 0, 0) );
			}
			else if (input_event.keyId == eKI_D)
			{
				gEnv->p3DEngine->getMainCamera()->moveLocal( Vec3(fFrameTime * speed, 0, 0 ) );
			}
			break;
		}
	case eDI_XBox:
		{
			if (input_event.keyId == eKI_Xbox_TriggerL && input_event.state == eIS_Changed)
			{
				speed = (input_event.value) * 45.0 + 15;
				return false;
			}

			if (input_event.keyId == eKI_Xbox_ThumbLY && input_event.state == eIS_Changed)
			{
				float thumbspeed = (input_event.value) * speed;
				gEnv->p3DEngine->getMainCamera()->moveLocal( Vec3(0, fFrameTime * thumbspeed, 0) );
				return false;
			}

			if (input_event.keyId == eKI_Xbox_ThumbLX && input_event.state == eIS_Changed)
			{
				float thumbspeed = (input_event.value) * speed;
				gEnv->p3DEngine->getMainCamera()->moveLocal( Vec3(fFrameTime * thumbspeed, 0, 0) );
				return false;
			}

			if (input_event.keyId == eKI_Xbox_ThumbRX && input_event.state == eIS_Changed)
			{
				Quat qBefore = gEnv->p3DEngine->getMainCamera()->getDerivedOrientation();
				Ang3 aRot(qBefore);

				aRot.z -= input_event.value * 0.015f;
				//aRot.x -= vMouseDelta.y * 0.002f;

				Quat qRot = Quat::CreateRotationXYZ(aRot);
				qRot.Normalize();
				gEnv->p3DEngine->getMainCamera()->setOrientation(qRot);			
				return false;
			}

			if (input_event.keyId == eKI_Xbox_ThumbRY && input_event.state == eIS_Changed)
			{
				Quat qBefore = gEnv->p3DEngine->getMainCamera()->getDerivedOrientation();
				Ang3 aRot(qBefore);

				aRot.x += input_event.value * 0.015f;
				//aRot.x -= vMouseDelta.y * 0.002f;

				Quat qRot = Quat::CreateRotationXYZ(aRot);
				qRot.Normalize();
				gEnv->p3DEngine->getMainCamera()->setOrientation(qRot);		
				return false;
			}
			break;
		}
	case eDI_Android:
	case eDI_IOS:
		{
            TouchDeviceFreeCamHandle(input_event);
			break;
		}
	}
	return false;
}

inline bool HandleModelViewCam( const SInputEvent &input_event, float speed, float& camDist, IGameObject* obj = NULL )
{
	float fFrameTime = gEnv->pTimer->GetFrameTime();
	

	static float height = 0;
	static float width = 0;
	switch ( input_event.deviceId )
	{
	case eDI_Mouse:
		{
			static bool holdingR = false;
			static bool holdingL = false;
			static bool holdingM = false;

			if (holdingM || holdingR)
			{
				if (obj)
				{
					//gEnv->pRenderer->getAuxRenderer()->AuxRender3DBoxSolid( obj->getWorldAABB().GetCenter(), Vec3(0.5,0.5,0.5), ColorF(1,0,0,1), true );
				}
				else
				{
					//gEnv->pRenderer->getAuxRenderer()->AuxRender3DBoxSolid( Vec3(0,0,0), Vec3(1,1,1), ColorF(1,0,0,1), true );
				}
			}


			if (input_event.keyId == eKI_Mouse2)
			{
				if (input_event.state == eIS_Down)
				{
					holdingR = true;
				}
				else
				{
					holdingR = false;
				}
			}
			else if (input_event.keyId == eKI_Mouse1)
			{
				if (input_event.state == eIS_Down)
				{
					holdingL = true;
				}
				else
				{
					holdingL = false;
				}
			}
			else if (input_event.keyId == eKI_Mouse3)
			{
				if (input_event.state == eIS_Down)
				{
					holdingM = true;
				}
				else
				{
					holdingM = false;

					// change the target [10/16/2014 gameKnife]
				}
			}
			else if (input_event.keyId == eKI_MouseX && holdingR)
			{
				Quat qBefore = gEnv->p3DEngine->getMainCamera()->getDerivedOrientation();
				Ang3 aRot(qBefore);
				aRot.z -= input_event.value * speed;
				Quat qRot = Quat::CreateRotationXYZ(aRot);
				qRot.Normalize();
				gEnv->p3DEngine->getMainCamera()->setOrientation(qRot);				
			}
			else if (input_event.keyId == eKI_MouseY && holdingR)
			{
				Quat qBefore = gEnv->p3DEngine->getMainCamera()->getDerivedOrientation();
				Ang3 aRot(qBefore);
				aRot.x -= input_event.value * speed;
				Quat qRot = Quat::CreateRotationXYZ(aRot);
				qRot.Normalize();
				gEnv->p3DEngine->getMainCamera()->setOrientation(qRot);				
			}
			else if (input_event.keyId == eKI_MouseY && (holdingM))
			{
				height += input_event.value * 0.01f;			
			}
			else if (input_event.keyId == eKI_MouseX && (holdingM))
			{
				width -= input_event.value * 0.01f;			
			}
			else if (input_event.keyId == eKI_MouseWheelUp)
			{
				camDist -= 0.1f;
			}
			else if (input_event.keyId == eKI_MouseWheelDown)
			{
				camDist += 0.1f;
			}
			break;
		}
	case eDI_Keyboard:
		{

		}
	case eDI_XBox:
		{
			if (input_event.keyId == eKI_Xbox_ThumbRX && input_event.state == eIS_Changed)
			{
				Quat qBefore = gEnv->p3DEngine->getMainCamera()->getDerivedOrientation();
				Ang3 aRot(qBefore);

				aRot.z -= input_event.value * speed * 3;
				//aRot.x -= vMouseDelta.y * 0.002f;

				Quat qRot = Quat::CreateRotationXYZ(aRot);
				qRot.Normalize();
				gEnv->p3DEngine->getMainCamera()->setOrientation(qRot);			
			}

			if (input_event.keyId == eKI_Xbox_ThumbRY && input_event.state == eIS_Changed)
			{
				Quat qBefore = gEnv->p3DEngine->getMainCamera()->getDerivedOrientation();
				Ang3 aRot(qBefore);

				aRot.x += input_event.value * speed * 3;
				//aRot.x -= vMouseDelta.y * 0.002f;

				Quat qRot = Quat::CreateRotationXYZ(aRot);
				qRot.Normalize();
				gEnv->p3DEngine->getMainCamera()->setOrientation(qRot);		
			}
			break;
		}
	case eDI_Android:
	case eDI_IOS:
		{
            if (input_event.keyId == eKI_Android_DragX && input_event.state == eIS_Changed)
            {
                    //gkLogMessage(_T("Rotating: %d"), event.deviceIndex);
                    Quat qBefore = gEnv->p3DEngine->getMainCamera()->getDerivedOrientation();
                    Ang3 aRot(qBefore);
                    
                    aRot.z -= input_event.value * 0.002f;
                    //aRot.x -= vMouseDelta.y * 0.002f;
                    
                    Quat qRot = Quat::CreateRotationXYZ(aRot);
                    qRot.Normalize();
                    gEnv->p3DEngine->getMainCamera()->setOrientation(qRot);
                
                //return false;
            }
            if (input_event.keyId == eKI_Android_DragY && input_event.state == eIS_Changed)
            {
                    Quat qBefore = gEnv->p3DEngine->getMainCamera()->getDerivedOrientation();
                    Ang3 aRot(qBefore);
                    
                    aRot.x -= input_event.value * 0.002f;
                    //aRot.x -= vMouseDelta.y * 0.002f;
                    
                    Quat qRot = Quat::CreateRotationXYZ(aRot);
                    qRot.Normalize();
                    gEnv->p3DEngine->getMainCamera()->setOrientation(qRot);

                //return false;
            }
            
            //static float startCamHeight = camDist;
            static float startScale = 1.0;
            static float startDist = 1.0;
            
            if (input_event.keyId == eKI_Gesture_Zoom)
            {
                if( input_event.state == eIS_Pressed )
                {
                    startScale = input_event.value;
                    startDist = camDist;
                }
                if( input_event.state == eIS_Changed )
                {
                    float delta = startScale - input_event.value;
                    camDist = startDist + delta * 5.f;
                
                    //TCHAR dist[MAX_PATH];
                    //_stprintf(dist, _T("start dist: %f | cam dist: %f"), startDist, camDist);
                    
                    //IFtFont* m_consoleFont = gEnv->pFont->CreateFont( _T("engine/fonts/msyh.ttf"), 14, GKFONT_OUTLINE_0PX );
                    //gEnv->pRenderer->getAuxRenderer()->AuxRenderText( dist, 50, 150, m_consoleFont );
                }
                
                //return false;
            }
			break;
		}
	}

	// handle cam pos
	if (obj)
	{
		gEnv->p3DEngine->getMainCamera()->setPosition( obj->getWorldAABB().GetCenter() );
	}
	else
	{
		gEnv->p3DEngine->getMainCamera()->setPosition( Vec3(0,0,0) );
	}

	gEnv->p3DEngine->getMainCamera()->moveLocal( Vec3(width, -camDist, height));


	return false;
}
#endif // TestCaseUtil_Cam_h__