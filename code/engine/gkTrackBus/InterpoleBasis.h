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
// Name:   	InterpoleBasis.h
// Desc:	
// 	
// 
// Author:  YiKaiming
// Date:	2013/9/18
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _InterpoleBasis_h_
#define _InterpoleBasis_h_

template	<int N>
class	BasisFunction	{
public:
	const float& operator[]( int i ) const { return m_f[i]; };
protected:
	float	m_f[N];
};

// Special functions that makes parameter zero.
template <class T>
inline void Zero( T &val )
{
	memset( &val,0,sizeof(val) );
}

///////////////////////////////////////////////////////////////////////////////
// HermitBasis.
class	HermitBasis : public BasisFunction<4>
{
public:
	HermitBasis( float t ) {
		float t2,t3,t2_3,t3_2,t3_t2;

		t2 = t*t;												// t2 = t^2;
		t3 = t2*t;											// t3 = t^3;

		t3_2 = t3 + t3;
		t2_3 = 3*t2;
		t3_t2 = t3 - t2;
		m_f[0] = t3_2 - t2_3 + 1;
		m_f[1] = -t3_2 + t2_3;
		m_f[2] = t3_t2 - t2 + t;
		m_f[3] = t3_t2;
	}
};

///////////////////////////////////////////////////////////////////////////////
// BezierBasis.
class BezierBasis : public BasisFunction<4>
{
public:
	BezierBasis( const float t ) 
	{
		const float t2 = t*t;
		const float t3 = t2*t;
		m_f[0] = -t3+3*t2-3*t+1;
		m_f[1] = 3*t3-6*t2+3*t;
		m_f[2] = -3*t3+3*t2;
		m_f[3] = t3;
	}
};

#endif