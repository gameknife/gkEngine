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
// Name:   	gkTaskDispatcher.h
// Desc:	
// 	
// 
// Author:  Kaiming
// Date:	2013/3/24	
// 
//////////////////////////////////////////////////////////////////////////

#ifndef gkTaskDispatcher_h__
#define gkTaskDispatcher_h__

#include "ITask.h"

//#ifdef OS_WIN32
#include "IThread.h"
#include "IEvent.h"

class gkTaskDispatcher;

struct sThreadStatus
{
	bool runing;
	int processedTask;
};

class gkTaskThread : public IThread
{
public:
	gkTaskThread( int threadId, gkTaskDispatcher* creator );
	virtual ~gkTaskThread();
	virtual int Run();

	void SetReady()
	{
		m_runningFlag->Reset();
		m_waitFlag->Set();
	}

	HANDLE getWaitingHandle()
	{
		return m_runningFlag->GetEvent();
	}
    
    IEvent* getWaitingEvent()
    {
        return m_runningFlag;
    }

	sThreadStatus queryStatus()
	{
		sThreadStatus status;
		status.runing = m_runing;
		status.processedTask = m_processedSinceLastQuery;
		m_processedSinceLastQuery = 0;

		return status;
	}

private:
	int m_threadId;

	CManualResetEvent* m_runningFlag;		/// 运行完成标志
	CManualResetEvent* m_waitFlag;			/// 等待下次运行标志
	gkTaskDispatcher* m_creator;			/// 任务分发器

	bool m_runing;
	int m_processedSinceLastQuery;
};

typedef std::vector<gkTaskThread*> SrTaskThreadPool;
typedef std::deque<ITask*> SrTaskStack;
typedef std::vector<ITask*> SrTaskList;

class gkTaskDispatcher : public ITaskDispatcher
{
public:
	gkTaskDispatcher(void);
	~gkTaskDispatcher(void);

	void Init();
	void Destroy();

	void CancelTask(ITask* task);
	void PushTask(ITask* task, int priority = 0);
	void FlushCoop();
	void Flush();
	void Wait();
	virtual sTaskStatus QueryTaskStatus();

	void Update();
	ITask* RequestTask();
	void FinishTask(ITask* task);

	SrTaskThreadPool m_pool;				///< 处理任务的线程池，任务线程个数为CPU核心数-1
	SrTaskStack m_taskStack;				///< 用于存放任务的栈，索取任务直接从栈顶取出，直至取完。
	SrTaskList  m_taskList;				///< 用于存放任务数据的数组，负责任务的生存期。
	SrTaskStack  m_taskFinishedList;			///< 用于存放完成任务的数组，每帧由主线程调用Complete回调
};

#endif // gkTaskDispatcher_h__
