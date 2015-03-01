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
// Name:   	gkIOSSpecific.h
// Desc:	platform specific for ios	
// 	
// Author:  Kaiming
// Date:	2012/6/13
// Modify:	
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _gkIOSSpecific_h_
#define _gkIOSSpecific_h_

#include "gkPlatform.h"
#include <mach/mach_time.h>

inline unsigned long timeGetTime()
{
	uint64_t time = mach_absolute_time();

	mach_timebase_info_data_t m_sTimeBaseInfo;
	mach_timebase_info(&m_sTimeBaseInfo);
	uint64_t millis = (time * (m_sTimeBaseInfo.numer/m_sTimeBaseInfo.denom))/1000000.0;
	return static_cast<unsigned long>(millis);
	return 0;

}

enum EISOInputEventType
{
    eIET_finger,
    eIET_ges_zoom,
    eIET_ges_pan,
    eIET_ges_rotate,
    
    
//    eIET_IOSdown,
//    eIET_IOSup,
//    eIET_IOSmove,
//    
//    eIET_IOSzoom_start,
//    eIET_IOSzoom_change,
//    eIET_IOSzoom_end,
//    
//    eIET_IOSpan_start,
//    eIET_IOSpan_change,
//    eIET_IOSpan_end,
};

enum EISOInputEventState
{
    eIES_IOSstart,
    eIES_IOSend,
    eIES_IOSchange,
};

struct SIOSInputEvent
{
	uint8	motionType;
    uint8   motionState;
	uint8	deviceIdx;
	Vec2	asyncPos;
};


struct SIOSInputDelegate
{
    void pushEvent(SIOSInputEvent& event)
    {
        std::map<int,int>::iterator it = m_indexMap.find(event.deviceIdx);
        if (it == m_indexMap.end()) {
            // new touch
            if(m_touchRegCount < 255)
            {
                m_indexMap[event.deviceIdx] = m_touchRegCount++;
                event.deviceIdx = m_touchRegCount - 1;
            }
            else {
                // reach max, this shouldnot exist
                event.deviceIdx = -1;
            }
            
        }
        else {
            event.deviceIdx = it->second;
        }
        
        if (event.deviceIdx != 255) {
            m_actionQueue.push_back(event);
        }
        
    }
    
    void clearEvent()
    {
        m_actionQueue.clear();
    }
    
    std::map<int,int> m_indexMap;
    std::vector<SIOSInputEvent> m_actionQueue;
    int m_touchRegCount;
    
    SIOSInputDelegate()
    {
        m_indexMap.clear();
        m_actionQueue.clear();
        m_touchRegCount = 0;
    }
};

#endif


