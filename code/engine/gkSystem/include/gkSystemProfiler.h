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
// Name:   	gkSystemProfiler.h
// Desc:		
// 
// Author:  Kaiming-Desktop
// Date:	2011 /8/14
// Modify:	2011 /8/14
// 
//////////////////////////////////////////////////////////////////////////

#ifndef GKSYSTEMPROFILER_H_
#define GKSYSTEMPROFILER_H_

#pragma once
#include "gkSystemPrerequisites.h"
#include "ISystemProfiler.h"

struct gkProfileElement
{
	uint32 m_count;
	float m_elpasedTime;
	float m_startTime;

	gkProfileElement():m_count(0), m_elpasedTime(0), m_startTime(0)
	{

	}

	void Clear()
	{
		m_count = 0;
	}
};

class gkSystemProfiler : public ISystemProfiler
{
public:
	gkSystemProfiler(void);
	virtual ~gkSystemProfiler(void) {}

	virtual uint32 getCurrFrameDP() {return m_uDPCurrentFrame;}
	virtual uint32 getRecentAverageDP() {return m_uDPCurrentFrame;}

	virtual uint32 getCurrFrameShadowDP() {return m_uShadowDPCurrentFrame;}
	virtual uint32 getRecentAverageShadowDP() {return m_uShadowDPCurrentFrame;}

	virtual void setCurrFrameDP(uint32 frameDP);
	virtual void setCurrFrameShadowDP(uint32 frameShadowDP);
	

	virtual void setStartRendering();
	virtual void setEndRendering();

	virtual void setStartWaiting();
	virtual void setEndWaiting();

	virtual void setStartSceneManage();
	virtual void setEndSceneManage();

	virtual void setStartCommit();
	virtual void setEndCommit();

	virtual void setFrameBegin();
	virtual void setFrameEnd();

	virtual float getFrameTimeRendering() {return m_fFrameTimeRendering;}
	virtual float getFrameTimeRenderingWNT() {return m_fFrameTimeRenderingFWD;}
	virtual float getFrameTimeRenderingMNG() {return m_fFrameTimeRenderingDFD;}
	virtual float getFrameTimeRenderingCMT() {return m_fFrameTimeRenderingCMT;}

	virtual int getFrameCount() {return m_uFrameCounter;}

	virtual void displayInfo();
	virtual void profilerGUI();

	virtual float getElementTime(EProfileElement element);
	virtual void setElementTime(EProfileElement element, float elapsedTime);

	virtual void setElementStart(EProfileElement element );
	virtual void setElementEnd(EProfileElement element );

	virtual uint32 getElementCount( EProfileElement element );
	virtual void increaseElementCount( EProfileElement element, int count = 1 );

	virtual void setGpuTimerElement( const TCHAR* elementName, float elpasedTime );
	virtual float getGpuTimerElement( const TCHAR* elementName );

	void update();

private:
	void qinfo( int height );
	void features();
	void profileResource();
	void about();
	void benchmark();

	void DrawProfileCurve(int whole_screen_box_y, float* datasrc, ColorB lineColor);

private:
	uint32 m_uDPCurrentFrame;
	uint32 m_uShadowDPCurrentFrame;
	float  m_fFrameTimer;
	float  m_fFrameTimeRendering;

	float  m_fFrameTimerFWD;
	float  m_fFrameTimeRenderingFWD;

	float  m_fFrameTimerDFD;
	float  m_fFrameTimeRenderingDFD;

	float  m_fFrameTimerCMT;
	float  m_fFrameTimeRenderingCMT;

	uint32 m_uFrameCounter;

	struct IFtFont* m_profilerFont1;
	struct IFtFont* m_profilerFont;
	struct IFtFont* m_profilerFPSFont;
	struct IFtFont* m_titleFont;
	struct IFtFont* m_subtitleFont;

	gkProfileElement m_profileElements[ePe_Count];

	std::map<gkStdString, float> m_gpuTimerElements;

	float m_fFrameTimeRenderingCurve[1000];
	float m_fFramwTimeCurve[1000];
	float m_fFrameGPUTimeCurve[1000];
	int m_CurveRecorder;
};


#endif //GKSYSTEMPROFILER_H_
