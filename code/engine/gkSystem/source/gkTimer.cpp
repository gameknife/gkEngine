#include "gkSystemStableHeader.h"
#include "gkTimer.h"
#include <time.h>
#include "ISystem.h"
/////////////////////////////////////////////////////

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#include "Mmsystem.h"
#endif

//this should not be included here
#include "IRenderer.h"									// needed for m_pSystem->GetIRenderer()->EF_Query(EFQ_RecurseLevel)

//#define PROFILING 1
#ifdef PROFILING
static int64 g_lCurrentTime = 0;
#endif

//! Profile smoothing time in seconds (original default was .8 / log(10) ~= .35 s)
static const float fDEFAULT_PROFILE_SMOOTHING = 1.0f;
static const float fMAX_BLEND_FRAME_TIME = 1.0f;


/////////////////////////////////////////////////////
// get the timer in microseconds. using QueryPerformanceCounter
static int64 GetPerformanceCounterTime()
{
#ifdef PROFILING
	return g_lCurrentTime;
#endif

#ifdef OS_WIN32
	LARGE_INTEGER lNow;

	QueryPerformanceCounter(&lNow);

	// for the following calculation even 64bit integer isn't enough
	//	int64 lCurTime=(lNow.QuadPart*TIMEVALUE_PRECISION) / m_lTicksPerSec;
	//	int64 lCurTime = (int64)((double)lNow.QuadPart / m_lTicksPerMicroSec);

	return lNow.QuadPart;
#else
	return timeGetTime();	
#endif
}



//#ifdef WIN32
// get the timer in microseconds. using winmm
static int64 GetMMTime()
{		
	int64 lNow=timeGetTime();

	return lNow;
}
//#endif // WIN32


#define DEFAULT_FRAME_SMOOTHING 1
/////////////////////////////////////////////////////
gkTimer::gkTimer() 
{
	m_pfnUpdate=NULL;
	m_lBaseTime=0;
	m_lLastTime=0;
	m_lTicksPerSec=0;
	m_lCurrentTime=0;
	m_fFrameTime=0;
	m_fRealFrameTime=0;
	m_lOffsetTime=0;

	sys_max_fps=0;
	m_fixed_time_step = 0;
	m_max_time_step = 0.25f;
  m_time_scale = 1.0f;

	// note: frame numbers (old version - commented out) are not used but is based on time
	m_TimeSmoothing = DEFAULT_FRAME_SMOOTHING; 
	
// smoothing
	m_timecount=0;
	for (int k=0;k<FPS_FRAMES;k++) 
   m_previousTimes[k]=0;

	m_fAverageFrameTime=1.0f/30.0f;	//lets hope the game will run with 30 frames on average
	for (int i=0; i<MAX_FRAME_AVERAGE; i++) 
		m_arrFrameTimes[i]=m_fAverageFrameTime; 
	
	m_profile_smooth_time = fDEFAULT_PROFILE_SMOOTHING;
	m_profile_weighting = 0;
	m_fAvgFrameTime = 0;

	m_bEnabled = true;
	m_TimeDebug = 0;
	m_lGameTimerPausedTime = 0;
	m_bGameTimerPaused = false;
	m_lForcedGameTime = -1;

	m_nFrameCounter=0;
#ifdef OS_WIN32
	LARGE_INTEGER TTicksPerSec;

	if(QueryPerformanceFrequency(&TTicksPerSec))
	{ 
		// performance counter is available, use it instead of multimedia timer
		LARGE_INTEGER t;
		QueryPerformanceCounter( &t );
		m_lTicksPerSec=TTicksPerSec.QuadPart;
		m_pfnUpdate = &GetPerformanceCounterTime;
		ResetTimer();
	}
	else
	{ 
#ifdef WIN32
		//Use MM timer if unable to use the High Frequency timer
		m_lTicksPerSec=1000;
		m_pfnUpdate = &GetMMTime;
		ResetTimer();
#endif // WIN32
	}
#else
	m_lTicksPerSec=1000;
	m_pfnUpdate = &GetMMTime;
	ResetTimer();
#endif
	
}

/////////////////////////////////////////////////////
bool gkTimer::Init(ISystem *pSystem)
{
	m_pSystem=pSystem;

	REGISTER_CVARFLOAT_DESC( sys_max_fps,
		_T("固定更新频率\n")
		_T("Usage: sys_max_fps [0.0 - 999.0]\n")
		_T("默认为60.0"), 0.1f, 999.0f, 999.0f);

	// MartinM: renamed cvars for consistency
	// if game code was accessing them by name there was something wrong anyway

// 	REGISTER_CVAR2("t_Smoothing",&m_TimeSmoothing,DEFAULT_FRAME_SMOOTHING,0,
// 		"time smoothing\n"
// 		"0=off, 1=on");
// 
// 	REGISTER_CVAR2("t_FixedStep",&m_fixed_time_step,0,0,
// 		"Game updated with this fixed frame time\n"
// 		"0=off, number specifies the frame time in seconds\n"
// 		"e.g. 0.033333(30 fps), 0.1(10 fps), 0.01(100 fps)" );
// 
// 	REGISTER_CVAR2("t_MaxStep",&m_max_time_step,0.25f,0,
// 		"Game systems clamped to this frame time" );
// 
// 	// todo: reconsider exposing that as cvar (negative time, same value is used by Trackview, better would be another value multipled with the internal one)
//   REGISTER_CVAR2("t_Scale",&m_time_scale,1.0f,0,
// 		"Game time scaled by this - for variable slow motion" );
// 
// 	REGISTER_CVAR2("t_Debug",&m_TimeDebug,0,0, "Timer debug" );
// 
// 	// -----------------
// 
// 	REGISTER_CVAR2("profile_smooth",&m_profile_smooth_time, fDEFAULT_PROFILE_SMOOTHING, 0,
// 		"Profiler exponential smoothing interval (seconds)" );
// 
// 	REGISTER_CVAR2("profile_weighting",&m_profile_weighting,0,0,
// 		"Profiler smoothing mode: 0 = legacy, 1 = average, 2 = peak weighted, 3 = peak hold" );

	return true;
}


/////////////////////////////////////////////////////
//SPU_INDIRECT(CommandBufferExecute(M))
float gkTimer::GetCurrTime(ETimer which) const 
{
	return m_CurrTime[(int)which].GetSeconds();
}

/////////////////////////////////////////////////////
float gkTimer::GetRealFrameTime() const 
{ 
	if(!m_bEnabled) 
		return 0.0f; 

	return m_fRealFrameTime;   
} 

/////////////////////////////////////////////////////
float gkTimer::GetTimeScale() const 
{ 
	return m_time_scale;   
} 

/////////////////////////////////////////////////////
void gkTimer::SetTimeScale(float scale)
{ 
	m_time_scale = scale;
} 

/////////////////////////////////////////////////////
float gkTimer::GetAsyncCurTime()  
{ 
	assert(m_pfnUpdate);		// call Init() before

	int64 llNow=(*m_pfnUpdate)()-m_lBaseTime;
	double dVal=(double)llNow;
	float fRet=(float)(dVal/(double)(m_lTicksPerSec));

	return fRet; 
}

/////////////////////////////////////////////////////
float gkTimer::GetFrameRate()
{
	// Use real frame time.
	if (m_fRealFrameTime != 0.f)
		return 1.f / m_fRealFrameTime;
	return 0.f;
}


float gkTimer::GetSmoothFrameRate()
{
	// Use real frame time.
	if (m_fRealFrameTime != 0.f)
		return 1.f / m_fAvgFrameTime;
	return 0.f;
}


void gkTimer::UpdateBlending()
{
	// Accumulate smoothing time up to specified max.
	float fSmoothTime = min(m_profile_smooth_time, m_CurrTime[ETIMER_REALTIME].GetSeconds());
	float fFrameTime = min(m_fRealFrameTime, fMAX_BLEND_FRAME_TIME);
	if (fSmoothTime == 0.f)
	{
		m_fAvgFrameTime = fFrameTime;
		m_fProfileBlend = 1.f;
		return;
	}

	// Blend current frame into blended previous.
	m_fProfileBlend = 1.f - expf(-fFrameTime / fSmoothTime);
	if (m_profile_weighting >= 3)
	{
		// Decay avg frame time, set as new peak.
		m_fAvgFrameTime *= 1.f - m_fProfileBlend;
		if (fFrameTime > m_fAvgFrameTime)
		{
			m_fAvgFrameTime = fFrameTime;
			m_fProfileBlend = 1.f;
		}
		else
			m_fProfileBlend = 0.f;
	}
	else
	{
		// Track average frame time.
		m_fAvgFrameTime += m_fProfileBlend * (fFrameTime - m_fAvgFrameTime);
		if (m_profile_weighting == 1)
			// Weight all frames evenly.
			m_fProfileBlend = 1.f - expf(-m_fAvgFrameTime / fSmoothTime);
	}
}

float gkTimer::GetProfileFrameBlending( float* pfBlendTime, int* piBlendMode )
{
	if (piBlendMode)
		*piBlendMode = m_profile_weighting;

	if (pfBlendTime)
	{
		// Accumulate smoothing time up to specified max.
		if (*pfBlendTime > m_profile_smooth_time)
			*pfBlendTime = m_profile_smooth_time;
		else
		{
			// Return smoothing for custom blend time.
			if (*pfBlendTime == 0.f)
				return 1.f;
			if (m_profile_weighting == 1)
				// Weight all frames evenly.
				return 1.f - expf(-m_fAvgFrameTime / *pfBlendTime);
			else if (m_profile_weighting == 2)
				// Weight according to frame time.
				return 1.f - expf(-min(m_fRealFrameTime, fMAX_BLEND_FRAME_TIME) / *pfBlendTime);
		}
	}
	return m_fProfileBlend;
}

/////////////////////////////////////////////////////
void gkTimer::RefreshGameTime(int64 curTime)
{
	double dVal=(double) ( curTime + m_lOffsetTime );
	
	//	m_fCurrTime=(float)(dVal/(double)(m_lTicksPerSec));

	// can be done much better

	m_CurrTime[ETIMER_LOGICTIME].SetSeconds((float)(dVal/(double)(m_lTicksPerSec)));

	assert(dVal>=0);
}

/////////////////////////////////////////////////////
void gkTimer::RefreshUITime(int64 curTime)
{
	double dVal=(double) ( curTime );

	//	m_fCurrTime=(float)(dVal/(double)(m_lTicksPerSec));

	// can be done much better

	m_CurrTime[ETIMER_REALTIME].SetSeconds((float)(dVal/(double)(m_lTicksPerSec)));

	assert(dVal>=0);
}


/////////////////////////////////////////////////////
void gkTimer::UpdateOnFrameStart()
{
	if(!m_bEnabled)
		return;

	if (sys_max_fps != 0)
	{
		CTimeValue timeFrameMax;
		timeFrameMax.SetMilliSeconds((int64)(1000.f/(float)sys_max_fps));
		static CTimeValue sTimeLast = gEnv->pTimer->GetAsyncTime();
		const CTimeValue timeLast = timeFrameMax + sTimeLast;
		while(timeLast.GetValue() > gEnv->pTimer->GetAsyncTime().GetValue())
		{
			volatile int i=0;
			while(i++ < 1000);
		}
		sTimeLast = gEnv->pTimer->GetAsyncTime();
	}




	assert(m_pfnUpdate);		// call Init() before

	if ((m_nFrameCounter & 127)==0)
	{
		// every bunch of frames, check frequency to adapt to
		// CPU power management clock rate changes
#ifdef OS_WIN32
		LARGE_INTEGER TTicksPerSec;
		if (QueryPerformanceFrequency(&TTicksPerSec))
		{ 
			// if returns false, no performance counter is available
			m_lTicksPerSec=TTicksPerSec.QuadPart;
		}
#endif
	}

	m_nFrameCounter++;

#ifdef PROFILING
	m_fFrameTime = 0.020f; // 20ms = 50fps
	g_lCurrentTime += (int)(m_fFrameTime*(float)(CTimeValue::TIMEVALUE_PRECISION));
	m_lCurrentTime = g_lCurrentTime;
	m_lLastTime = m_lCurrentTime;
	RefreshGameTime(m_lCurrentTime);
	RefreshUITime(m_lCurrentTime);
	return;
#endif
 
	int64 now = (*m_pfnUpdate)();

	if (m_lForcedGameTime >= 0)
	{
		// m_lCurrentTime contains the time, which should be current
		// but time has passed since Serialize until UpdateOnFrameStart
		// so we have to make sure to compensate!
		m_lOffsetTime = m_lForcedGameTime - now + m_lBaseTime;
		m_lLastTime = now - m_lBaseTime;
		m_lForcedGameTime = -1;
	}

	// Real time. 
	m_lCurrentTime = now - m_lBaseTime;

	//m_fRealFrameTime = m_fFrameTime = (float)(m_lCurrentTime-m_lLastTime) / (float)(m_lTicksPerSec);
	m_fRealFrameTime = m_fFrameTime = (float)((double)(m_lCurrentTime-m_lLastTime) / (double)(m_lTicksPerSec));

	if( 0 != m_fixed_time_step) // Absolute zero used as a switch. looks wrong, but runs right ;-)
	{
#ifdef OS_WIN32
		if (m_fixed_time_step < 0)
		{
			// Enforce real framerate by sleeping.
			float sleep = -m_fixed_time_step - m_fFrameTime;
	
			if (sleep > 0)
			{
				// while first
//				float now = GetAsyncCurTime();
				bool breaksleep = 0;
// 				if (sleep < 0.01f)
// 				{
// 					for (int i=0; i < 1000000; ++i)
// 					{
// 						if( GetAsyncCurTime() - now > sleep )
// 						{
// 							breaksleep = 1;
// 							break;
// 						}
// 					}
// 				}

				if (!breaksleep)
				{
					Sleep((unsigned int)(sleep*1000.f));
					m_lCurrentTime = (*m_pfnUpdate)() - m_lBaseTime;
					//m_fRealFrameTime = (float)(m_lCurrentTime-m_lLastTime) / (float)(m_lTicksPerSec);
					m_fRealFrameTime = (float)((double)(m_lCurrentTime-m_lLastTime) / (double)(m_lTicksPerSec));	
				}
			
			}
		}
#endif
		m_fFrameTime = abs(m_fixed_time_step);
	}
	else
	{
		// Clamp it
		m_fFrameTime = min(m_fFrameTime, m_max_time_step);

		// Dilate it.
		m_fFrameTime *= m_time_scale;
	}

	if (m_fFrameTime < 0) 
		m_fFrameTime = 0;



//-----------------------------------------------

	if(m_TimeSmoothing>0)
	{
	/*
		if(m_TimeSmoothing>FPS_FRAMES-1)
			m_TimeSmoothing=FPS_FRAMES-2;

		if(m_fFrameTime < 0.0000001f)
			m_fFrameTime = 0.0000001f;

		m_previousTimes[m_timecount] = m_fFrameTime;

		m_timecount++;
		if(m_timecount>=m_TimeSmoothing)
			m_timecount=0;

		// average multiple frames together to smooth changes out a bit
		float total = 0;
		for(int i = 0 ; i < m_TimeSmoothing+1; i++ ) 
			total += m_previousTimes[i];	
	    
		if(!total) 
			total = 1;

		m_fFrameTime=total/(float)(m_TimeSmoothing+1);
*/
		m_fAverageFrameTime = GetAverageFrameTime( 0.25f, m_fFrameTime, m_fAverageFrameTime); 
		m_fFrameTime=m_fAverageFrameTime;
	}

	//else

	{	
		// Adjust.
		if (m_fFrameTime != m_fRealFrameTime)
		{
			float fBefore = GetAsyncCurTime();
			int64 nAdjust = (int64)((m_fFrameTime - m_fRealFrameTime) * (double)(m_lTicksPerSec));
			m_lCurrentTime += nAdjust;
			m_lBaseTime -= nAdjust;
		}
	}

	RefreshUITime(m_lCurrentTime);
	if (m_bGameTimerPaused == false)
		RefreshGameTime(m_lCurrentTime);

	m_lLastTime = m_lCurrentTime;

	if (m_fRealFrameTime < 0.f)
		// Can happen after loading a saved game and 
		// at any time on dual core AMD machines and laptops :)
		m_fRealFrameTime = 0.0f;

	UpdateBlending();
}

//------------------------------------------------------------------------
//--  average frame-times to avoid stalls and peaks in framerate
//--    note that is is time-base averaging and not frame-based
//------------------------------------------------------------------------
f32 gkTimer::GetAverageFrameTime(f32 sec, f32 FrameTime, f32 LastAverageFrameTime) 
{ 
	uint32 numFT=	MAX_FRAME_AVERAGE;
	for (int32 i=(numFT-2); i>-1; i--)
		m_arrFrameTimes[i+1] = m_arrFrameTimes[i];

	if (FrameTime>0.4f) FrameTime = 0.4f;
	if (FrameTime<0.0f) FrameTime = 0.0f;
	m_arrFrameTimes[0] = FrameTime;

	//get smoothed frame
	uint32 avrg_ftime = 1;
	if (LastAverageFrameTime)
	{
		avrg_ftime = uint32(sec/LastAverageFrameTime+0.5f); //average the frame-times for a certain time-period (sec)
		if (avrg_ftime>numFT)	avrg_ftime=numFT;
		if (avrg_ftime<1)	avrg_ftime=1;
	}

	f32 AverageFrameTime=0;
	for (uint32 i=0; i<avrg_ftime; i++)	
		AverageFrameTime += m_arrFrameTimes[i];
	AverageFrameTime /= avrg_ftime;

	//don't smooth if we pause the game
	if (FrameTime<0.0001f)
		AverageFrameTime=FrameTime;
	
//	g_YLine+=66.0f;
//	float fColor[4] = {1,0,1,1};
//	g_pIRenderer->Draw2dLabel( 1,g_YLine, 1.3f, fColor, false,"AverageFrameTime: keys:%d   time:%f %f", avrg_ftime ,AverageFrameTime,sec/LastAverageFrameTime);	
//	g_YLine+=16.0f;

	return AverageFrameTime;
}


/////////////////////////////////////////////////////
void gkTimer::ResetTimer()
{
	assert(m_pfnUpdate);		// call Init() before

	m_lBaseTime = (*m_pfnUpdate)();
	m_lLastTime = m_lCurrentTime = 0;
	m_fFrameTime = 0;
	RefreshGameTime(m_lCurrentTime);
	RefreshUITime(m_lCurrentTime);
	m_lForcedGameTime = -1;
	m_bGameTimerPaused = false;
	m_lGameTimerPausedTime = 0;
}

/////////////////////////////////////////////////////
void gkTimer::EnableTimer( const bool bEnable ) 
{ 
	m_bEnabled = bEnable; 
}

bool gkTimer::IsTimerEnabled() const
{
	return m_bEnabled;
}

/////////////////////////////////////////////////////
CTimeValue gkTimer::GetAsyncTime() const
{
//	assert(m_pfnUpdate);		// call Init() before
	
	if(!m_pfnUpdate)		// call Init() before
		return CTimeValue();

	int64 llNow=(*m_pfnUpdate)();

	// can be done much better
	double fac=(double)CTimeValue::TIMEVALUE_PRECISION/(double)m_lTicksPerSec;

	return CTimeValue(int64(llNow*fac));
}

//! try to pause/unpause a timer
//  returns true if successfully paused/unpaused, false otherwise
bool gkTimer::PauseTimer(ETimer which, bool bPause)
{
	if (which != ETIMER_LOGICTIME)
		return false;

	if (m_bGameTimerPaused == bPause)
		return false;

	if (bPause)
	{
		if (m_lForcedGameTime >= 0)
			m_lGameTimerPausedTime = m_lForcedGameTime;
		else
			m_lGameTimerPausedTime = m_lCurrentTime + m_lOffsetTime;
	}
	else
	{
		if (m_lGameTimerPausedTime > 0)
		{
			m_lOffsetTime = 0;
			RefreshGameTime(m_lGameTimerPausedTime);
			m_lForcedGameTime = m_lGameTimerPausedTime;
		}
		m_lGameTimerPausedTime = 0;
	}
	m_bGameTimerPaused = bPause;
	return true;
}

//! determine if a timer is paused
//  returns true if paused, false otherwise
bool gkTimer::IsTimerPaused(ETimer which)
{
	if (which != ETIMER_LOGICTIME)
		return false;
	return m_bGameTimerPaused;
}

//! try to set a timer
//  return true if successful, false otherwise
bool gkTimer::SetTimer(ETimer which, float timeInSeconds)
{
	if (which != ETIMER_LOGICTIME)
		return false;
	m_lOffsetTime = 0;
	m_lForcedGameTime = (int64) ((double)(timeInSeconds)*(double)(m_lTicksPerSec));
	RefreshGameTime(m_lForcedGameTime);
	return true;
}

void gkTimer::SecondsToDateUTC(time_t inTime, struct tm& outDateUTC)
{
	outDateUTC = *gmtime(&inTime);
}

#if defined (WIN32) || defined(WIN64)
time_t gmt_to_local_win32(void)
{
	TIME_ZONE_INFORMATION tzinfo;
	DWORD dwStandardDaylight;
	long bias;

	dwStandardDaylight = GetTimeZoneInformation(&tzinfo);
	bias = tzinfo.Bias;

	if (dwStandardDaylight == TIME_ZONE_ID_STANDARD)
		bias += tzinfo.StandardBias;

	if (dwStandardDaylight == TIME_ZONE_ID_DAYLIGHT)
		bias += tzinfo.DaylightBias;

	return (- bias * 60);
}
#endif

time_t gkTimer::DateToSecondsUTC(struct tm& inDate)
{
#if defined (WIN32)
	return mktime(&inDate) + gmt_to_local_win32();
#elif defined (LINUX)
#if defined (HAVE_TIMEGM)
	// return timegm(&inDate);
#else
	// craig: temp disabled the +tm.tm_gmtoff because i can't see the intention here
	// and it doesn't compile anymore
	// alexl: tm_gmtoff is the offset to greenwhich mean time, whereas mktime uses localtime
	//        but not all linux distributions have it...
	return mktime(&inDate) /*+ tm.tm_gmtoff*/;
#endif












#else
	return mktime(&inDate);
#endif
}
