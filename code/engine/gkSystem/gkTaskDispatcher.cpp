#include "gkSystemStableHeader.h"
#include "gkTaskDispatcher.h"
#include "IHavok.h"

//#ifdef OS_WIN32

gkCritcalSectionLock g_lock;
gkCritcalSectionLock g_finishLock;

gkTaskThread::gkTaskThread( int tileId, gkTaskDispatcher* creator ):m_creator(creator),
	m_threadId(tileId)
{
	m_runningFlag = new CManualResetEvent(false);
	m_waitFlag = new CManualResetEvent(false);
	m_runing = false;
	m_processedSinceLastQuery = 0;
}

gkTaskThread::~gkTaskThread()
{
	delete m_runningFlag;
	delete m_waitFlag;
}

/**
 *@brief 线程的运行函数
 *@return int 
 *@remark 没有被SetReady时会一直wait，直到被SetReady后，
 开始处理任务，处理完成后将runningFlag置位，通知外部处理完成，同时进入wait状态
 */
int gkTaskThread::Run()
{
	// 第一次进入
    if(gEnv->pPhysics)
        gEnv->pPhysics->HavokThreadInit();

	while(true)
	{
		m_waitFlag->Wait();
		m_waitFlag->Reset();
		m_runing = true;

		while(true)
		{
			ITask* task = m_creator->RequestTask();
			if (!task)
			{
				// 没有任务了，退出线程
				m_runing = false;
				break;
			}
			// 执行任务
			task->RealExecute();
			// 放入已完成队列
			m_creator->FinishTask(task);
			//delete task;
			m_processedSinceLastQuery++;
		}
		m_runningFlag->Set();
	}

    if(gEnv->pPhysics)
	 gEnv->pPhysics->HavokThreadQuit();

	return 0;	
}

gkTaskDispatcher::gkTaskDispatcher(void)
{
}


gkTaskDispatcher::~gkTaskDispatcher(void)
{
}


/**
 *@brief 作业线程索取任务的接口，利用临界区保证任务索取的原子性。
 *@return SrRasTask* 
 */
ITask* gkTaskDispatcher::RequestTask()
{
	ITask* ret =  NULL;

	g_lock.lock();
	if (!m_taskStack.empty())
	{
		ret = m_taskStack.front();
		m_taskStack.pop_front();
	}
	if (!m_taskList.empty())
	{
		SrTaskList::iterator it = m_taskList.begin();
		for (; it != m_taskList.end(); ++it)
		{
			if ( ret == *it )
			{
				m_taskList.erase(it);
				break;
			}
		}
	}

	g_lock.unlock();

	return ret;
}

/**
 *@brief 初始化任务分发器
 *@return void 
 *@remark 根据cpu核心数，开启cpu核心数-1个作业线程
 */
void gkTaskDispatcher::Init()
{
	int threadCount = gEnv->pSystemInfo->cpuCoreCount - 1;
	if (threadCount < 1)
	{
		threadCount = 1;
	}

	threadCount = 1;

	for (int i=0; i < threadCount; ++i)
	{
		gkTaskThread* thisThread = new gkTaskThread(i, this);
		m_pool.push_back(thisThread);

		uint8 core = i + 1;

#ifdef OS_WIN32
        
		DWORD_PTR mask1,mask2;
		GetProcessAffinityMask( GetCurrentProcess(),&mask1,&mask2 );
		if(BIT(core) & mask1)	// Check if we have this affinity
			SetThreadAffinityMask( thisThread->GetHandle(), BIT(core) );
		else // Reserve CPU 1 for main thread.
			SetThreadAffinityMask( thisThread->GetHandle(),(mask1 & (~1)) );
#endif
        
	}
	
	SrTaskThreadPool::iterator it = m_pool.begin();
	for ( ; it != m_pool.end(); ++it)
	{
		(*it)->Start();
	}
}

/**
 *@brief 关闭任务分发器，关闭作业线程
 *@return void 
 */
void gkTaskDispatcher::Destroy()
{
	SrTaskThreadPool::iterator it = m_pool.begin();
	for ( ; it != m_pool.end(); ++it)
	{
		(*it)->Terminate();
		delete (*it);
	}
}

/**
 *@brief 协作flush模式，主线程和其他线程一起处理task栈中任务
 *@return void 
 */
void gkTaskDispatcher::FlushCoop()
{
	Flush();
	
	// main thread also begin
	while(true)
	{
		ITask* task = RequestTask();
		if (!task)
		{
			// 没有任务了，退出线程
			break;
		}
		// 执行任务
		task->Execute();
	}
}

/**
 *@brief 独立flush模式，通知线程开始处理。
 *@return void 
 */
void gkTaskDispatcher::Flush()
{
	SrTaskThreadPool::iterator it = m_pool.begin();
	for ( ; it != m_pool.end(); ++it)
	{
		(*it)->SetReady();
	}
}

/**
 *@brief 主线程等待所有线程完成工作。
 *@return void 
 */
void gkTaskDispatcher::Wait()
{
	std::vector<HANDLE> handles;
	SrTaskThreadPool::iterator it = m_pool.begin();
	for ( ; it != m_pool.end(); ++it)
	{
		handles.push_back( (*it)->getWaitingHandle() );
        
        (*it)->getWaitingEvent()->Wait();
	}
	//::WaitForMultipleObjects(handles.size(), &(handles[0]), TRUE, INFINITE);
    
    
    
}

/**
 *@brief 主线程向任务分发器中压入任务
 *@return void 
 *@param SrRasTask * task 
 */
void gkTaskDispatcher::PushTask( ITask* task, int priority )
{
	// if priority frist [4/1/2013 Kaiming]
	if ( priority == -2 )
	{
		//task->Pushed();
		task->Execute();
		task->Complete_MT();
		delete task;

		return;
	}


	g_lock.lock();

	{
		//task->Pushed();
		if (priority == -1)
		{
			m_taskStack.push_front(task);
		}
		else
		{
			m_taskStack.push_back(task);
		}
		
		m_taskList.push_back(task);
	}
	
	Flush();

	g_lock.unlock();

	

	//m_taskList.push_back(task);
}

sTaskStatus gkTaskDispatcher::QueryTaskStatus()
{
	sTaskStatus status;

	status.threadCount = m_pool.size();
	status.taskTotal = m_taskStack.size();
	for (uint32 i=0; i < m_pool.size(); ++i)
	{
		sThreadStatus tdStatus = m_pool[i]->queryStatus();
		if ( tdStatus.runing )
		{
			status.taskNum[i] = 1;
			status.taskTotal++;
		}
	}

	return status;
}

void gkTaskDispatcher::CancelTask( ITask* task )
{

	g_lock.lock();

	SrTaskList::iterator it = m_taskList.begin();
	for (; it != m_taskList.end(); ++it)
	{
		if ( task == *it )
		{
			task->Cancel();
			break;
		}
	}

	g_lock.unlock();
}

void gkTaskDispatcher::FinishTask(ITask* task)
{
	g_finishLock.lock();

	m_taskFinishedList.push_back( task );

	g_finishLock.unlock();
}

void gkTaskDispatcher::Update()
{
	float time = gEnv->pTimer->GetAsyncCurTime();

	while(1)
	{
		if (m_taskFinishedList.empty())
		{
			break;
		}

		ITask* thisTask = NULL;
		g_finishLock.lock();

		thisTask = m_taskFinishedList.front();
		m_taskFinishedList.pop_front();

		g_finishLock.unlock();

		if (thisTask)
		{
			thisTask->Complete_MT();
			delete thisTask;
		}
	}

	time = gEnv->pTimer->GetAsyncCurTime() - time;
	if (time > 0.001)
	{
		//gkLogMessage( _T("Task Complete one frame using %.2fms"), time * 1000.f );
	}
}

//#endif