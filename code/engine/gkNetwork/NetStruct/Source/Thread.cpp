
#include "NetWorkStableHeader.h"

#include "Thread.h"

#include <process.h>

#include "Win32Exception.h"

///////////////////////////////////////////////////////////////////////////////
// CThread
///////////////////////////////////////////////////////////////////////////////

IThread::IThread()
   :  m_hThread(INVALID_HANDLE_VALUE)
{

}
      
IThread::~IThread()
{
   if (m_hThread != INVALID_HANDLE_VALUE)
   {
      ::CloseHandle(m_hThread);
   }

}

HANDLE IThread::GetHandle() const
{
   return m_hThread;
}

void IThread::Start()
{
   if (m_hThread == INVALID_HANDLE_VALUE)
   {
      unsigned int threadID = 0;

      m_hThread = (HANDLE)::_beginthreadex(0, 0, ThreadFunction, (void*)this, 0, &threadID);

      if (m_hThread == INVALID_HANDLE_VALUE)
      {
         throw CWin32Exception(_T("CThread::Start() - _beginthreadex"), GetLastError());
      }
   }
   else
   {
      throw CCommException(_T("CThread::Start()"), _T("Thread already running - you can only call Start() once!"));
   }
}

void IThread::Wait() const
{
   if (!Wait(INFINITE))
   {
      throw CCommException(_T("CThread::Wait()"), _T("Unexpected timeout on infinite wait"));
   }
}

bool IThread::Wait(DWORD timeoutMillis) const
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
        throw CWin32Exception(_T("CThread::Wait() - WaitForSingleObject"), ::GetLastError());
    }
    
    return ok;
}

unsigned int __stdcall IThread::ThreadFunction(void *pV)
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

void IThread::Terminate(
   DWORD exitCode /* = 0 */)
{
   if (!::TerminateThread(m_hThread, exitCode))
   {
      // TODO we could throw an exception here...
   }
}


///////////////////////////////////////////////////////////////////////////////
// End of file...
///////////////////////////////////////////////////////////////////////////////

