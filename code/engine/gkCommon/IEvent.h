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
// Name:   	IEvent.h
// Desc:		
// 	
// Author:  Kaiming
// Date:	2012/3/14
// Modify:	2012/3/14
// 
//////////////////////////////////////////////////////////////////////////

#ifndef IEVENT_H__
#define IEVENT_H__

#include "gkPlatform.h"
///////////////////////////

#ifdef OS_WIN32
static HANDLE Create(
					 LPSECURITY_ATTRIBUTES lpEventAttributes, 
					 bool bManualReset, 
					 bool bInitialState, 
					 LPCTSTR lpName)
{
	HANDLE hEvent = ::CreateEvent(lpEventAttributes, bManualReset, bInitialState, lpName);

	if (hEvent == NULL)
	{

	}

	return hEvent;
}

class IEvent 
{
public :
	IEvent(
		LPSECURITY_ATTRIBUTES lpEventAttributes,
		bool manualReset,
		bool initialState):m_hEvent(Create(lpEventAttributes, manualReset, initialState, 0)) {}

	IEvent(
		LPSECURITY_ATTRIBUTES lpEventAttributes,
		bool manualReset,
		bool initialState,
		const LPCTSTR &name):m_hEvent(Create(lpEventAttributes, manualReset, initialState, name)) {}

	virtual ~IEvent();

	HANDLE GetEvent() const;

	void Wait();

	bool Wait(
		DWORD timeoutMillis);

	void Reset();

	void Set();

	void Pulse();

private :

	HANDLE m_hEvent;

	// No copies do not implement
	IEvent(const IEvent &rhs);
	IEvent &operator=(const IEvent &rhs);
};


///////////////////////////////////////////////////////////////////////////////
// CEvent
///////////////////////////////////////////////////////////////////////////////

inline IEvent::~IEvent()
{
	::CloseHandle(m_hEvent);
}

inline HANDLE IEvent::GetEvent() const
{
	return m_hEvent;
}

inline void IEvent::Wait()
{
	if (!Wait(INFINITE))
	{

	}
}

inline bool IEvent::Wait(DWORD timeoutMillis)
{
	bool ok;

	DWORD result = ::WaitForSingleObject(m_hEvent, timeoutMillis);

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

	}

	return ok;
}

inline void IEvent::Reset()
{
	if (!::ResetEvent(m_hEvent))
	{

	}
}

inline void IEvent::Set()
{
	if (!::SetEvent(m_hEvent))
	{

	}
}

inline void IEvent::Pulse()
{
	if (!::PulseEvent(m_hEvent))
	{

	}
}

#else

#include <pthread.h>
#include <time.h>
#include <sys/time.h>

class IEvent
{
    public :
	IEvent(
           void* lpEventAttributes,
           bool manualReset,
           bool initialState) {
        pthread_mutex_init(&m_count_mutex, NULL);
        pthread_cond_init(&m_event_handle, NULL);
        m_state = initialState;
        m_manualReset = manualReset;
    }
    
	IEvent(
           void* lpEventAttributes,
           bool manualReset,
           bool initialState,
           const TCHAR* &name) {
        pthread_mutex_init(&m_count_mutex, NULL);
        pthread_cond_init(&m_event_handle, NULL);
        m_state = initialState;
        m_manualReset = manualReset;
    
    }
    
	virtual ~IEvent()
    {
        pthread_mutex_destroy(&m_count_mutex);
        pthread_cond_destroy(&m_event_handle);
    }
    
	HANDLE GetEvent() const
    {
        return (HANDLE)(&m_event_handle);
    }
    
	void Wait()
    {
        pthread_mutex_lock(&m_count_mutex);
        
        while (!m_state)
        {
            if( pthread_cond_wait(&m_event_handle, &m_count_mutex) )
            {
                pthread_mutex_unlock(&m_count_mutex);
                return;
            }
        }
        
        if(!m_manualReset)
        {
             m_state = false;
        }
       
        
        pthread_mutex_unlock(&m_count_mutex);
    }
    
	bool Wait(
              DWORD milliseconds)
    {
        int rc = 0;
        struct timespec abstime;
        struct timeval tv;
        
        gettimeofday(&tv, NULL);
        
        abstime.tv_sec  = tv.tv_sec + milliseconds / 1000;
        abstime.tv_nsec = tv.tv_usec*1000 + (milliseconds % 1000)*1000000;
        if (abstime.tv_nsec >= 1000000000)
        {
            abstime.tv_nsec -= 1000000000;
            abstime.tv_sec++;
        }
        
        pthread_mutex_lock(&m_count_mutex);
        
        while (!m_state)
        {
            if (rc = pthread_cond_timedwait(&m_event_handle, &m_count_mutex, &abstime))
            {
                if (rc == -1) break;
                pthread_mutex_unlock(&m_count_mutex);
                return -1;
            }
        }
        if (rc == 0 && !m_manualReset)
        {
            m_state = false;
        }
        pthread_mutex_unlock(&m_count_mutex);
    }
    
	void Reset()
    {
        pthread_mutex_lock(&m_count_mutex);
        
        m_state = false;
        
        pthread_mutex_unlock(&m_count_mutex);
    }
    
	void Set()
    {
        pthread_mutex_lock(&m_count_mutex);
        
        m_state = true;
        if (m_manualReset) {
            pthread_cond_broadcast(&m_event_handle);
        }
        else
        {
            pthread_cond_signal(&m_event_handle);
        }
        
        
        pthread_mutex_unlock(&m_count_mutex);
    }
    
	void Pulse()
    {
        
    }
    
    private :
    
	HANDLE m_hEvent;
    
    pthread_cond_t m_event_handle;
    pthread_mutex_t m_count_mutex;
    bool m_state;
    bool m_manualReset;
    
	// No copies do not implement
	IEvent(const IEvent &rhs);
	IEvent &operator=(const IEvent &rhs);
};

#endif


class CManualResetEvent : public IEvent
{
    public :
    
	explicit CManualResetEvent(
                               bool initialState = false);
    
	explicit CManualResetEvent(
                               const TCHAR* &name,
                               bool initialState = false);
    
    private :
    
	// No copies do not implement
	CManualResetEvent(const CManualResetEvent &rhs);
	CManualResetEvent &operator=(const CManualResetEvent &rhs);
};

inline CManualResetEvent::CManualResetEvent(
                                            bool initialState /* = false */)
:  IEvent(0, true, initialState)
{
    
}

inline CManualResetEvent::CManualResetEvent(
                                            const TCHAR* &name, 
                                            bool initialState /* = false */)
:  IEvent(0, true, initialState, name)
{
    
}

#endif