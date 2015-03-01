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
// Name:   	ITask.h
// Desc:	
// 	
// 
// Author:  Kaiming
// Date:	2013/3/24	
// 
//////////////////////////////////////////////////////////////////////////

#ifndef ITask_h__
#define ITask_h__

struct ITask
{
	volatile bool m_scheduled;

	ITask(): m_scheduled(true) {}
	virtual ~ITask() {}
	void Cancel() {m_scheduled = false;}
	void RealExecute() {if (m_scheduled) Execute();}
	virtual void Execute() =0;
	virtual void Complete_MT() {}
};

struct sTaskStatus
{
	int threadCount;
	int taskTotal;
	int taskNum[8];

	sTaskStatus()
	{
		memset( this, 0, sizeof(sTaskStatus));
	}
};

struct ITaskDispatcher
{
	~ITaskDispatcher(void) {}

	virtual void CancelTask(ITask* task) =0;
	virtual void PushTask(ITask* task, int priority = 0) =0;
	virtual void FlushCoop() =0;
	virtual void Flush() =0;
	virtual void Wait() =0;

	virtual sTaskStatus QueryTaskStatus() =0;
};

#endif // ITask_h__
