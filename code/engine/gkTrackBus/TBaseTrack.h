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
// VR Platform (C) 2013
// OpenVRP Kernel Source File 
//
// Name:   	TBaseTrack.h
// Desc:	
// 	
// 
// Author:  YiKaiming
// Date:	2013/9/25
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _TBaseTrack_h_
#define _TBaseTrack_h_

#include "ITrackBus.h"

template <class T>
class TBaseTrack : public IMovieTrack
{
public:
	TBaseTrack(void)
	{
		m_epsilon = 1.0 / 33.3;
	}
	virtual ~TBaseTrack(void)
	{

	}

	virtual bool set_key( IKey* key );
	virtual void remove_key( float time );

	virtual void get_value( float time, Vec3& out ) {}

	virtual int get_key_count() {return m_keys.size();}

	virtual T* search_prev_key(float time);

	virtual KeyArray& get_keylist();



private:
	bool search_near_key_for_insert(float time, int& out);

	void rebuild_query_list();

private:
	std::vector<T> m_keys;
	KeyArray m_keys_for_query;
	float m_epsilon;
};

template <class T>
void TBaseTrack<T>::rebuild_query_list()
{
	m_keys_for_query.clear();

	std::vector<T>::iterator it = m_keys.begin();
	for (; it != m_keys.end(); ++it)
	{
		m_keys_for_query.push_back( &(*it) );
	}
}

template <class T>
KeyArray& TBaseTrack<T>::get_keylist()
{
	return m_keys_for_query;
}

template <class T>
void TBaseTrack<T>::remove_key( float time )
{
	int count = 0;
	std::vector<T>::iterator it = m_keys.begin();
	for ( ; it != m_keys.end(); ++it)
	{
		// 找到一个比当前time大的关键帧
		if ( it->get_time() + m_epsilon > time && it->get_time() - m_epsilon < time)
		{
			m_keys.erase( it );
			rebuild_query_list();
			break;
		}
		count++;
	}
}

template <class T>
bool TBaseTrack<T>::set_key( IKey* key )
{
	if (key)
	{
		int index = 0;
		if( search_near_key_for_insert( key->get_time(), index ) )
		{
			if (index != -1)
			{
				std::vector<T>::iterator it = m_keys.insert( m_keys.begin() + index, *((T*)key) );
				rebuild_query_list();
			}

		}
	}

	return false;

}

template <class T>
bool TBaseTrack<T>::search_near_key_for_insert( float time, int& out )
{
	//////////////////////////////////////////////////////////////////////////
	// 没有或只有一个关键帧
	if (m_keys.size() < 2)
	{
		if (m_keys.empty() )
		{
			out = 0;
			return true;
		}
		else
		{
			if (time < m_keys[0].get_time())
			{
				out = 0;
				return true;
			}
			else
			{
				out = 1;
				return true;
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	// 有两个以上的关键帧
	else
	{
		int count = 0;
		std::vector<T>::iterator it = m_keys.begin();
		for ( ; it != m_keys.end(); ++it, ++count)
		{
			// 找到一个比当前time大的关键帧
			if ( it->get_time() + m_epsilon > time)
			{
				out = count;
				// 如果和之前的这一帧差距小于epsilon，那么直接返回之前帧
				if ( it->get_time() - m_epsilon < time)
				{
					return false;
				}
				// 否则可以插入
				return true;
			}
		}

		// 这个是最后的帧，直接插入到队尾
		out = count;
		return true;
	}
	return false;
}

template <class T>
T* TBaseTrack<T>::search_prev_key( float time )
{
	T* ret = NULL;

	std::vector<T>::reverse_iterator it = m_keys.rbegin();
	for ( ; it != m_keys.rend(); ++it)
	{
		if ( it->get_time() + m_epsilon < time)
		{
			ret = &(*it);
			break;
		}
	}

	return ret;
}

#endif


