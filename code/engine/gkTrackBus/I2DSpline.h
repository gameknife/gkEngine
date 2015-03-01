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
// yikaiming (C) 2013
// gkENGINE Source File 
//
// Name:   	I2DSpline.h
// Desc:	
// 	
// 
// Author:  YiKaiming
// Date:	2013/10/12
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _I2DSpline_h_
#define _I2DSpline_h_

#include "InterpoleBasis.h"

template <class ValueType>
struct I2DSplineKey
{
	float time;
	ValueType value;

	ValueType dd;
	ValueType ds;

	I2DSplineKey(float a, ValueType b, ValueType c, ValueType d):time(a), value(b), dd(c), ds(d) {}
};

template <class T, class InterpoleType>
class I2DSpline
{
public:
	I2DSpline(T default_value)
	{
		m_frame_per_second = 30.f;
		m_epsilon = 1.f / m_frame_per_second;
		m_default_value = default_value;
	}
	~I2DSpline(void)
	{

	}

	//void add_key( float time, float value );
	void set_key( float time, T value, T dd, T ds )
	{
		int insert_count;
		if( search_near_key_for_insert(time, insert_count) )
		{
			if (insert_count != -1)
			{
				m_keys.insert( m_keys.begin() + insert_count, I2DSplineKey<T>( time, value, dd, ds) );
			}
		}
		else
		{
			m_keys[insert_count] = I2DSplineKey<T>( time, value, dd, ds );
		}

	}
	void remove_key( float time )
	{
		std::vector< I2DSplineKey<T> >::iterator it = m_keys.begin();
		for ( ; it != m_keys.end(); ++it)
		{
			// 找到位于epsilon中的这个key
			if (it->time + m_epsilon > time && it->time - m_epsilon < time)
			{
				// 干掉它！
				m_keys.erase( it );
				// 跳出
				break;
			}
		}
	}

	T interpole( float time );


private:
	void search_near_key(float time, int& prev_count, int& next_count)
	{
		if (m_keys.size() < 2)
		{
			prev_count = -1;
			return;
		}

		int count = 0;
		std::vector<I2DSplineKey<T>>::iterator it = m_keys.begin();
		for ( ; it != m_keys.end(); ++it, ++count)
		{
			if (it->time + m_epsilon > time)
			{
				// first key next to curr time
				prev_count = count - 1;
				next_count = count;
				if (prev_count < 0)
				{
					prev_count = 0;
					next_count = 1;
				}
				else if (next_count >= m_keys.size() - 1)
				{
					prev_count = m_keys.size() - 2;
					next_count = m_keys.size() - 1;
				}
				return;
			}
		}

		prev_count = count - 1;
		next_count = count;
		if (prev_count < 0)
		{
			prev_count = 0;
			next_count = 1;
		}
		else if (next_count >= m_keys.size() - 1)
		{
			prev_count = m_keys.size() - 2;
			next_count = m_keys.size() - 1;
		}
	}
	bool search_near_key_for_insert(float time, int& out)
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
				if (time < m_keys[0].time)
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
			std::vector<I2DSplineKey<T>>::iterator it = m_keys.begin();
			for ( ; it != m_keys.end(); ++it, ++count)
			{
				// 找到一个比当前time大的关键帧
				if (it->time + m_epsilon > time)
				{
					out = count;
					// 如果和之前的这一帧差距小于epsilon，那么直接返回之前帧
					if (it->time - m_epsilon < time)
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

	float m_frame_per_second;
	float m_epsilon;
	T m_default_value;
	std::vector< I2DSplineKey<T> > m_keys;
};

template <class T, class InterpoleType>
inline T I2DSpline<T, InterpoleType>::interpole( float time )
{
	int count, next_count;
	search_near_key( time, count, next_count );
	if (count != -1)
	{
		I2DSplineKey<T>& prev_key = m_keys[count];
		I2DSplineKey<T>& next_key = m_keys[next_count];

		float amount = (time - prev_key.time) / (next_key.time - prev_key.time);

		// linear interpole
		// ...

		// beizer interpole
		InterpoleType basis( amount );

		// 			const T p0 = prev_key.value;
		// 			const T p3 = next_key.value;
		// 			const T p1 = p0 + prev_key.dd;
		// 			const T p2 = p3 - next_key.ds;

		const T p0 = prev_key.value;
		const T p1 = next_key.value;
		const T p2 = prev_key.dd;
		const T p3 = -next_key.ds;

		T ret_value = (basis[0] * p0) + (basis[1] * p1) + (basis[2] * p2) + (basis[3] * p3);

		return ret_value;
	}

	return m_default_value;
}

template <>
inline float I2DSpline<float, BezierBasis>::interpole( float time )
{
	int count, next_count;
	search_near_key( time, count, next_count );
	if (count != -1)
	{
		I2DSplineKey<float>& prev_key = m_keys[count];
		I2DSplineKey<float>& next_key = m_keys[next_count];

		float amount = (time - prev_key.time) / (next_key.time - prev_key.time);

		// linear interpole
		// ...

		// beizer interpole
		BezierBasis basis( amount );

		const float p0 = prev_key.value;
		const float p3 = next_key.value;
		const float p1 = p0 + prev_key.dd;
		const float p2 = p3 - next_key.ds;

		float ret_value = (basis[0] * p0) + (basis[1] * p1) + (basis[2] * p2) + (basis[3] * p3);

		return ret_value;
	}

	return m_default_value;
}

template <>
inline Vec3 I2DSpline<Vec3, HermitBasis>::interpole( float time )
{
	int count, next_count;
	search_near_key( time, count, next_count );
	if (count != -1)
	{
		I2DSplineKey<Vec3>& prev_key = m_keys[count];
		I2DSplineKey<Vec3>& next_key = m_keys[next_count];

		float amount = (time - prev_key.time) / (next_key.time - prev_key.time);

		// linear interpole
		// ...

		// beizer interpole
		HermitBasis basis( amount );

		const Vec3 p0 = prev_key.value;
		const Vec3 p1 = next_key.value;
		const Vec3 p2 = prev_key.dd;
		const Vec3 p3 = -next_key.ds;

		Vec3 ret_value = (basis[0] * p0) + (basis[1] * p1) + (basis[2] * p2) + (basis[3] * p3);

		return ret_value;
	}

	return m_default_value;
}

#endif

