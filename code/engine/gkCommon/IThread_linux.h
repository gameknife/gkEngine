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



#include <pthread.h>
 
class IThread
 {
 public :
 
    IThread() {}
 
 	virtual ~IThread()
 	{
        pthread_detach(m_thread_handle);
 	}
 
 	void Wait() const
     {
         pthread_join(m_thread_handle, NULL);
     }
 
 	bool Wait(DWORD timeoutMillis) const
     {
         pthread_join(m_thread_handle, NULL);
         return true;
     }
 
 	void Start()
     {
         pthread_create( &m_thread_handle, NULL, ThreadFunction, (void*)this);
     }
 
 	virtual int Run() = 0;
 
 	void Terminate(
 		DWORD exitCode = 0)
     {
#ifndef OS_ANDROID
         pthread_cancel(m_thread_handle);
#endif
     }
 
 private :
 	static void* ThreadFunction(void* pV)
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
         
         return NULL;
     }
     
     pthread_t m_thread_handle;
 };


struct gkCritcalSectionLock
{
	gkCritcalSectionLock() { pthread_mutex_init(&m_handle, NULL); }
	~gkCritcalSectionLock() { pthread_mutex_destroy(&m_handle); }

	bool trylock() { return ( 0 == pthread_mutex_trylock(&m_handle)); }
	void lock() { pthread_mutex_lock(&m_handle);}
	void unlock() { pthread_mutex_unlock(&m_handle); }
    
	pthread_mutex_t m_handle;
};

struct gkMutexLock
{
	gkMutexLock() { pthread_mutex_init(&m_handle, NULL); }
	~gkMutexLock() { pthread_mutex_destroy(&m_handle); }

	bool trylock() { return ( 0 == pthread_mutex_trylock(&m_handle)); }
	void lock() { pthread_mutex_lock(&m_handle);}
	void unlock() { pthread_mutex_unlock(&m_handle); }

	pthread_mutex_t m_handle;
};