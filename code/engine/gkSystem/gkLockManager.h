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
// Name:   	gkLockManager.h
// Desc:	
// 	
// 
// Author:  Kaiming
// Date:	2013/3/27	
// 
//////////////////////////////////////////////////////////////////////////

#ifndef gkLockManager_h__
#define gkLockManager_h__

#include "IThread.h"

enum ELockID
{
	eLID_resfile_close = 0,
};

struct gkCritcalSectionLock
{
	CRITICAL_SECTION m_cs;

	gkCritcalSectionLock() { InitializeCriticalSection( &m_cs ); }
	~gkCritcalSectionLock() { DeleteCriticalSection( &m_cs ); }
	
	void lock() { EnterCriticalSection( &m_cs ); }
	void unlock() { LeaveCriticalSection( &m_cs ); }
};

typedef std::map< uint32, void* > gkLockMap;

template <class T> struct gkLockManager
{
	gkLockMap m_locks;

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
};

template <class T> struct gkAutoLock
{
	gkAutoLock(uint32 lockId )
	{
		m_lock =  s_lockManager.getLock( lockId );
		m_lock->lock();
	}

	~gkAutoLock()
	{
		m_lock->unlock();
	}

	T* m_lock;

	static gkLockManager<T> s_lockManager;
};

template<class T> gkLockManager<T> gkAutoLock<T>::s_lockManager;

#endif // gkLockManager_h__
