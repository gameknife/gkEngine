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
// Name:   	IThread.h
// Desc:		
// 	
// Author:  Kaiming
// Date:	2012/3/12
// Modify:	2012/3/12
// 
//////////////////////////////////////////////////////////////////////////

#ifndef ITHREAD_H__
#define ITHREAD_H__

#include "gkPlatform.h"

enum ELockGroup
{
	eLGID_global =0,
	eLGID_Resource,

	eLGID_Count,
};

enum ELockID
{
	eLID_resfile_close = 0,
	eLID_resfile_insert,
	eLID_log,
	eLID_file_seek,
	eLID_consolelog,
};

#ifdef OS_WIN32
#include <process.h>
#include "IThread_win32.h"
#else
#include "IThread_linux.h"
#endif

typedef std::map< uint32, void* > gkLockMap;

template <class T> struct gkLockManager
{
	gkLockMap m_locks;

	~gkLockManager()
	{
		gkLockMap::iterator it = m_locks.begin();
		for (; it != m_locks.end(); ++it)
		{
			delete (T*)(it->second);
		}

		m_locks.clear();
	}

	T* getLock( uint32 lockId )
	{
		gkLockMap::iterator it = m_locks.find( lockId );
		if (it != m_locks.end())
		{
			return (T*)(it->second);
		}
		else
		{
			T* newLock = new T();
			m_locks.insert( gkLockMap::value_type( lockId, newLock ) );
			return newLock;
		}
	}

	void deleteLock( uint32 lockId )
	{
		gkLockMap::iterator it = m_locks.find( lockId );
		if (it != m_locks.end())
		{
			delete (T*)(it->second);
			m_locks.erase(it);
		}
	}

	static std::map< uint32, gkLockManager<T> > s_lockManager;
};
template<class T> std::map< uint32, gkLockManager<T> > gkLockManager<T>::s_lockManager;

template <class T> struct gkScopedLock
{
	gkScopedLock(uint32 lockGroupId, uint32 lockId ): 
m_lockGroup( lockGroupId ),
	m_lockID( lockId )
{
	m_lock =  gkLockManager<T>::s_lockManager[lockGroupId].getLock( lockId );
}

~gkScopedLock()
{
	gkLockManager<T>::s_lockManager[m_lockGroup].deleteLock( m_lockID );
}

void Lock() volatile
{
	m_lock->lock();
}

void UnLock() volatile
{
	m_lock->unlock();
}

bool TryLock() volatile
{
	return m_lock->trylock();
}

T* m_lock;
uint32 m_lockGroup;
uint32 m_lockID;
};

template <class T> struct gkAutoLock
{
	gkAutoLock(uint32 lockGroupId, uint32 lockId )
	{
		m_lock =  gkLockManager<T>::s_lockManager[lockGroupId].getLock( lockId );
		m_lock->lock();
	}

	gkAutoLock( gkScopedLock<T>* scopedLock )
	{
		m_lock = scopedLock->m_lock;
		m_lock->lock();
	}

	~gkAutoLock()
	{
		m_lock->unlock();
	}

	T* m_lock;

	
};


inline void gkSleep( int msecond )
{
#ifdef OS_WIN32
	::Sleep( msecond );
#else
	//::sleep(msecond * 1000);
#endif
}


inline gk_thread_id gkGetCurrentThread()
{
#ifdef OS_WIN32
	return ::GetCurrentThreadId();
#else
	return pthread_self();
#endif
}

#endif

