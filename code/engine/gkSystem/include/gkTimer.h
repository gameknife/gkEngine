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
// Name:   	gkTimer.h
// Desc:		
// 
// Author:  Kaiming-Desktop
// Date:	2011 /8/2
// Modify:	2011 /8/2
// 
//////////////////////////////////////////////////////////////////////////

#ifndef GKTIMER_H
#define GKTIMER_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "ITimer.h"						// ITimer

#define FPS_FRAMES 16
#define MAX_FRAME_AVERAGE 100


#define TIME_PROFILE_PARAMS 256
//#ifdef OS_WIN32
// Implements all common timing routines
class gkTimer : public ITimer
{
public:
	// constructor
	gkTimer();
	// destructor
	~gkTimer() {};

	bool Init( ISystem *pSystem );

	// interface ITimer ----------------------------------------------------------

	// TODO: Review m_time usage in System.cpp / SystemRender.cpp
	//       if it wants Game Time / UI Time or a new Render Time?

	virtual void ResetTimer();		
	virtual void UpdateOnFrameStart();
	virtual float GetCurrTime(ETimer which = ETIMER_LOGICTIME) const;
	virtual CTimeValue GetAsyncTime() const;
	virtual float GetAsyncCurTime();
	virtual float GetFrameTime(ETimer which = ETIMER_LOGICTIME) const
	{
		if(!m_bEnabled) 
			return 0.0f; 
		if (which != ETIMER_LOGICTIME || m_bGameTimerPaused == false)
			return m_fFrameTime;
		return 0.0f;
	}
	virtual float GetRealFrameTime() const;
	virtual float GetTimeScale() const;
	virtual void SetTimeScale(float scale);
	virtual void EnableTimer( const bool bEnable );
	virtual float	GetFrameRate();
	virtual float	GetSmoothFrameRate();
	virtual float GetProfileFrameBlending( float* pfBlendTime = 0, int* piBlendMode = 0 );
	//virtual void Serialize(TSerialize ser);
	virtual bool IsTimerEnabled() const;

	//! try to pause/unpause a timer
	//  returns true if successfully paused/unpaused, false otherwise
	virtual bool PauseTimer(ETimer which, bool bPause);

	//! determine if a timer is paused
	//  returns true if paused, false otherwise
	virtual bool IsTimerPaused(ETimer which);

	//! try to set a timer
	//  return true if successful, false otherwise
	virtual bool SetTimer(ETimer which, float timeInSeconds);

	//! make a tm struct from a time_t in UTC (like gmtime)
	virtual void SecondsToDateUTC(time_t time, struct tm& outDateUTC);

	//! make a UTC time from a tm (like timegm, but not available on all platforms)
	virtual time_t DateToSecondsUTC(struct tm& timePtr);

	//! Convert from Tics to Milliseconds
	virtual float TicksToMillis(int64 ticks)
	{
		return (float)(ticks / (m_lTicksPerSec*0.001f));
	}

private: // ---------------------------------------------------------------------

	typedef int64 (*TimeUpdateFunc) ();				//  absolute, in microseconds,

	// ---------------------------------------------------------------------------

	// updates m_CurrTime (either pass m_lCurrentTime or custom curTime)
	void RefreshGameTime(int64 curTime);
	void RefreshUITime(int64 curTime);
	void UpdateBlending();
	f32 GetAverageFrameTime(f32 sec, f32 FrameTime, f32 LastAverageFrameTime); 

	TimeUpdateFunc	m_pfnUpdate;								// pointer to the timer function (performance counter or timegettime)
	int64						m_lBaseTime;								// absolute in ticks, 1 sec = m_lTicksPerSec units
	int64						m_lLastTime;								// absolute in ticks, 1 sec = m_lTicksPerSec units, needed to compute frame time
	int64           m_lOffsetTime;              // relative in ticks; offsets frame time by some amount so we can reset it independently of AsyncTime

	int64						m_lTicksPerSec;							// units per sec

	int64						m_lCurrentTime;							// absolute, in microseconds, at the CTimer:Update() call
	float						m_fFrameTime;								// in game seconds since the last update
	float						m_fRealFrameTime;						// in real seconds since the last update

	int64           m_lForcedGameTime;
	bool						m_bEnabled;									//
	// smoothing

	float						m_previousTimes[FPS_FRAMES];//
	float						m_arrFrameTimes[MAX_FRAME_AVERAGE];
	float						m_fAverageFrameTime;
	int							m_timecount;								//

	float						m_fProfileBlend;						// current blending amount for profile.
	float						m_fAvgFrameTime;						// for weighting.

	//////////////////////////////////////////////////////////////////////////
	// Console vars.
	//////////////////////////////////////////////////////////////////////////
	float						m_fixed_time_step;					// in seconds
	float						m_max_time_step;
	float           m_time_scale;               // slow down time (or theoretically speed it up)
	int							m_TimeSmoothing;						// Console Variable, 0=off, otherwise on
	int             m_TimeDebug;								// Console Variable, 0=off, otherwise on

	// Profile averaging help.
	float						m_profile_smooth_time;			// seconds to exponentially smooth profile results.
	int							m_profile_weighting;				// weighting mode (see RegisterVar desc).

	ISystem *				m_pSystem;									// 	

	unsigned	int		m_nFrameCounter;

	bool            m_bGameTimerPaused;
	int64           m_lGameTimerPausedTime;

	double			sys_max_fps;
};
#if 0
class gkTimer : public ITimer
{
public:
	// constructor
	gkTimer() {}
	// destructor
	~gkTimer() {}

	bool Init( ISystem *pSystem ) {return true;}

	// interface ITimer ----------------------------------------------------------

	// TODO: Review m_time usage in System.cpp / SystemRender.cpp
	//       if it wants Game Time / UI Time or a new Render Time?

	virtual void ResetTimer() {}		
	virtual void UpdateOnFrameStart() {}
	virtual float GetCurrTime(ETimer which = ETIMER_GAME) const {return 0;}
	virtual CTimeValue GetAsyncTime() const {return CTimeValue(0.0);}
	virtual float GetAsyncCurTime() {return 0;}
	virtual float GetFrameTime(ETimer which = ETIMER_GAME) const {return 0;}
	virtual float GetRealFrameTime() const {return 0;}
	virtual float GetTimeScale() const {return 0;}
	virtual void SetTimeScale(float scale) {}
	virtual void EnableTimer( const bool bEnable ) {}
	virtual float	GetFrameRate() {return 0;}
	virtual float	GetSmoothFrameRate() {return 0;}
	virtual float GetProfileFrameBlending( float* pfBlendTime = 0, int* piBlendMode = 0 ) {return 0;}
	//virtual void Serialize(TSerialize ser);
	virtual bool IsTimerEnabled() const {return true;}

	//! try to pause/unpause a timer
	//  returns true if successfully paused/unpaused, false otherwise
	virtual bool PauseTimer(ETimer which, bool bPause) {return true;}

	//! determine if a timer is paused
	//  returns true if paused, false otherwise
	virtual bool IsTimerPaused(ETimer which) {return true;}

	//! try to set a timer
	//  return true if successful, false otherwise
	virtual bool SetTimer(ETimer which, float timeInSeconds) {return true;}

	//! make a tm struct from a time_t in UTC (like gmtime)
	virtual void SecondsToDateUTC(time_t time, struct tm& outDateUTC) {}

	//! make a UTC time from a tm (like timegm, but not available on all platforms)
	virtual time_t DateToSecondsUTC(struct tm& timePtr) {return 0;}

	//! Convert from Tics to Milliseconds
	virtual float TicksToMillis(int64 ticks)
	{
		return 0;
	}

private: // ---------------------------------------------------------------------

};

#endif
#endif //timer