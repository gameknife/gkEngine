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
// Name:   	ISystemProfiler.h
// Desc:		
// 
// Author:  Kaiming-Desktop
// Date:	2011 /8/14
// Modify:	2011 /8/14
// 
//////////////////////////////////////////////////////////////////////////

#ifndef ISYSTEMPROFILER_H_
#define ISYSTEMPROFILER_H_

#pragma once
#include "gkPlatform.h"

	/**
	 @brief Profile项目
	 @remark 
	*/
enum EProfileElement
{
	ePe_Triangle_Total = 0,			///! 总三角面数
	ePe_Triangle_Fwd,				///! 非阴影，三角面数
	ePe_Triangle_Shadow,			///! 阴影，三角面数
	ePe_Batch_Total,				///! 总批次数
	ePe_Batch_Fwd,					///! 非阴影，批次数
	ePe_Batch_Shadow,				///! 阴影，批次数

	ePe_GpuTick_GPUTime,
	ePe_GpuTick_ReflectGen,
	ePe_GpuTick_ShadowMapGen,
	ePe_GpuTick_Zpass,
	ePe_GpuTick_SSAO,
	ePe_GpuTick_ShadowMaskGen,
	ePe_GpuTick_DeferredLighting,

	ePe_GpuTick_OpaquePass,
	ePe_GpuTick_TransparentPass,

	ePe_GpuTick_HDR,
	ePe_GpuTick_PostProcess,

	ePe_Font_Cost,
	ePe_Physic_Cost,
	ePe_TrackBus_Cost,
	ePe_Input_Cost,
	ePe_3DEngine_Cost,

	ePe_ThreadSync_Cost,
	ePe_SThread_Cost,
	ePe_MThread_Cost,

	ePe_MT_Part1,
	ePe_MT_Part2,
	ePe_MT_Part3,

	ePe_Count,
};

	/**
	 @ingroup CoreModuleAPI
	 @brief Profiler系统的性能监控器
	 @remark 通过EProfilerElement来获取监控项目的数据
	 @sa EProfileElement
	*/
struct ISystemProfiler
{
public:
	virtual ~ISystemProfiler(void) {}

	virtual uint32 getElementCount(EProfileElement element) =0;
	virtual void increaseElementCount(EProfileElement element, int count = 1) =0;

	virtual float getElementTime(EProfileElement element) =0;
	virtual void setElementTime(EProfileElement element, float elapsedTime) =0;

	virtual void setGpuTimerElement( const TCHAR* elementName, float elpasedTime ) =0;
	virtual float getGpuTimerElement( const TCHAR* elementName ) =0;

	virtual void setStartRendering() =0;
	virtual void setEndRendering() =0;

	virtual void setStartWaiting() =0;
	virtual void setEndWaiting() =0;

	virtual void setStartSceneManage() =0;
	virtual void setEndSceneManage() =0;

	virtual void setStartCommit() =0;
	virtual void setEndCommit() =0;

	virtual int getFrameCount() =0;

	virtual void displayInfo() =0;

	virtual void setFrameBegin() =0;
	virtual void setFrameEnd() =0;

	virtual void setElementStart(EProfileElement element ) =0;
	virtual void setElementEnd(EProfileElement element ) =0;
};

#endif



