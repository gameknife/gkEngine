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
// Name:   	CThread.h
// Desc:		
// 	
// Author:  Kaiming
// Date:	2012/3/12
// Modify:	2012/3/12
// 
//////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// CThread
///////////////////////////////////////////////////////////////////////////////
class IThread 
{
public :

	IThread(): m_hThread(INVALID_HANDLE_VALUE)	{}

	virtual ~IThread() 
	{
		if (m_hThread != INVALID_HANDLE_VALUE)
		{
			::CloseHandle(m_hThread);
		}
	}

	HANDLE GetHandle() const;

	void Wait() const;

	bool Wait(DWORD timeoutMillis) const;

	void Start();

	virtual int Run() = 0;

	void Terminate(
		DWORD exitCode = 0);

private :

	

	static unsigned int __stdcall ThreadFunction(void *pV);

	HANDLE m_hThread;

	// No copies do not implement
	IThread(const IThread &rhs);
	IThread &operator=(const IThread &rhs);
};

inline HANDLE IThread::GetHandle() const
{
	return m_hThread;
}

inline void IThread::Start()
{
	if (m_hThread == INVALID_HANDLE_VALUE)
	{
		unsigned int threadID = 0;

		m_hThread = (HANDLE)::_beginthreadex(0, 0, ThreadFunction, (void*)this, 0, &threadID);
		if (m_hThread == INVALID_HANDLE_VALUE)
		{
			//throw CWin32Exception(_T("CThread::Start() - _beginthreadex"), GetLastError());
		}
	}
	else
	{
		//throw CCommException(_T("CThread::Start()"), _T("Thread already running - you can only call Start() once!"));
	}
}

inline void IThread::Wait() const
{
	if (!Wait(INFINITE))
	{
		//throw CCommException(_T("CThread::Wait()"), _T("Unexpected timeout on infinite wait"));
	}
}

inline bool IThread::Wait(DWORD timeoutMillis) const
{

	bool ok;

	DWORD result = ::WaitForSingleObject(m_hThread, timeoutMillis);
	if (result == WAIT_TIMEOUT)
	{
		ok = false;
	}
	else if (result == WAIT_OBJECT_0)
	{
		ok = true;
	}
	else
	{
		//throw CWin32Exception(_T("CThread::Wait() - WaitForSingleObject"), ::GetLastError());
	}

	return ok;
}

inline unsigned int __stdcall IThread::ThreadFunction(void *pV)
{
	int result = 0;

	IThread* pThis = (IThread*)pV;

	if (pThis)
	{
		try
		{
			result = pThis->Run();
		}
		catch(...)
		{
		}
	}

	return result;
}

inline void IThread::Terminate(
						DWORD exitCode /* = 0 */)
{
	if (!::TerminateThread(m_hThread, exitCode))
	{
		// TODO we could throw an exception here...
	}
}

struct gkCritcalSectionLock
{
	CRITICAL_SECTION m_cs;

	gkCritcalSectionLock() { InitializeCriticalSection( &m_cs ); }
	~gkCritcalSectionLock() { DeleteCriticalSection( &m_cs ); }

	bool trylock() { return TryEnterCriticalSection((CRITICAL_SECTION*)&m_cs) != FALSE; }
	void lock() { EnterCriticalSection( &m_cs ); }
	void unlock() { LeaveCriticalSection( &m_cs ); }
};

struct gkMutexLock
{
	HANDLE m_mutex;

	gkMutexLock() { m_mutex = CreateMutex(NULL, FALSE, NULL); }
	~gkMutexLock() { CloseHandle(m_mutex); }

	bool trylock() { return (WaitForSingleObject(m_mutex, 0) != WAIT_TIMEOUT );  }
	void lock() { WaitForSingleObject( m_mutex, INFINITE); }
	void unlock() { ReleaseMutex( m_mutex ); }
};


