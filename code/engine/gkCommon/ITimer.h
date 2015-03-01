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
// Name:   	ITimer.h
// Desc:	全局时钟控制类	
// 
// Author:  Kaiming-Desktop
// Date:	2011 /8/2
// Modify:	2011 /8/2
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _ITIMER_H_
#define _ITIMER_H_
#include "gkPlatform.h"
#include "TimeValue.h"
#include "ISystem.h"

	/**
	 @ingroup CoreModuleAPI
	 @brief 全局计时器
	 @remark 
	*/
struct ITimer
{
	enum ETimer
	{
		ETIMER_LOGICTIME = 0,  // 可暂停，可放缩的时间类型
		ETIMER_REALTIME,       // 实时时间类型
		ETIMER_LAST
	};


	/**
	 @brief 重置计时器
	 @return 
	 @remark 
	*/
	virtual void ResetTimer() = 0;	


	/**
	 @brief 每帧的更新函数
	 @return 
	 @remark 
	*/
	virtual void UpdateOnFrameStart() = 0;

	/**
	 @brief 获取当前绝对时间
	 @return 
	 @param ETimer which
	 @remark 
	*/
	virtual float GetCurrTime(ETimer which = ETIMER_LOGICTIME) const = 0;

	/**
	 @brief 获取异步时间，即时时间
	 @return 
	 @remark 
	*/
	virtual CTimeValue GetAsyncTime() const = 0;

	/**
	 @brief 获取异步时间，即时时间
	 @return 
	 @remark 返回即时秒数
	*/
	virtual float GetAsyncCurTime()= 0;


	/**
	 @brief 获取帧时间
	 @return 
	 @param ETimer which
	 @remark 
	*/
	virtual float GetFrameTime(ETimer which = ETIMER_LOGICTIME) const = 0;


	/**
	 @brief 获取原生帧时间
	 @return 
	 @remark 
	*/
	virtual float GetRealFrameTime() const = 0;


	/**
	 @brief 获取时间缩放
	 @return 
	 @remark 
	*/
	virtual float GetTimeScale() const = 0;


	/**
	 @brief 设置时间缩放
	 @return 
	 @param float s
	 @remark 用于实现慢镜头，等全局时间变化的效果
	*/
	virtual void SetTimeScale(float s) = 0;

	/**
	 @brief 激活/反激活计时器
	 @return 
	 @param const bool bEnable
	 @remark 
	*/
	virtual void EnableTimer( const bool bEnable ) = 0;

	/**
	 @brief 获取计时器的激活状态
	 @return 
	 @remark 
	*/
	virtual bool IsTimerEnabled() const = 0;

	/**
	 @brief 取得帧率
	 @return 
	 @remark 
	*/
	virtual float	GetFrameRate() = 0;

	/**
	 @brief 取得平滑后的帧率
	 @return 
	 @remark 
	*/
	virtual float	GetSmoothFrameRate() =0;

	/**
	 @brief 暂停/继续 一个计时器
	 @return 
	 @param ETimer which 哪个计时器
	 @param bool bPause 暂停或继续
	 @remark 
	*/
	virtual bool PauseTimer(ETimer which, bool bPause) = 0;

	/**
	 @brief 获取一个计时器的暂停状态
	 @return 
	 @param ETimer which
	 @remark 
	*/
	virtual bool IsTimerPaused(ETimer which) = 0;

	/**
	 @brief 重新设置一个计时器的时间
	 @return 
	 @param ETimer which
	 @param float timeInSeconds
	 @remark 
	*/
	virtual bool SetTimer(ETimer which, float timeInSeconds) = 0;

protected:
	CTimeValue			m_CurrTime[ETIMER_LAST+1];
};

#endif
