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



#ifndef _TIMEVALUE_H_
#define _TIMEVALUE_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "gkPlatform.h"


class CTimeValue
{
public:
	// Default constructor.
	ILINE CTimeValue()
	{
		m_lValue=0;
	}

	// Constructor.
	CTimeValue( const float fSeconds )
	{
		SetSeconds(fSeconds);
	}

	CTimeValue( const double fSeconds )
	{
		SetSeconds(fSeconds);
	}

	// Constructor.
	// Arguments:
	//		inllValue - positive negative, absolute or relative in 1 second= TIMEVALUE_PRECISION units.
	ILINE CTimeValue( const int64 inllValue )
	{
		m_lValue=inllValue;
	}

	// Copy constructor.
	ILINE CTimeValue( const CTimeValue &inValue )
	{
		m_lValue=inValue.m_lValue;
	}

	// Destructor.
	ILINE ~CTimeValue() {}

	// Description:
	//		Assignment operator.
	// Arguments:
	//		inRhs - Right side.
	ILINE CTimeValue& operator=( const CTimeValue &inRhs )
	{
		m_lValue = inRhs.m_lValue;
		return *this;
	};

	// Use only for relative value, absolute values suffer a lot from precision loss.
	float GetSeconds() const
	{
		return m_lValue * (1.f/TIMEVALUE_PRECISION);
	}

	// Get relative time difference in seconds - call on the endTime object:  endTime.GetDifferenceInSeconds( startTime );
	ILINE float GetDifferenceInSeconds( const CTimeValue& startTime ) const
	{
		return (m_lValue - startTime.m_lValue) * (1.f/TIMEVALUE_PRECISION);
	}

	//
	void SetSeconds( const float infSec )
	{
		m_lValue = (int64)(infSec*TIMEVALUE_PRECISION);
	}

	//
	void SetSeconds( const double infSec )
	{
		m_lValue = (int64)(infSec*TIMEVALUE_PRECISION);
	}

	//
	void SetSeconds( const int64 indwSec )
	{
		m_lValue=indwSec*TIMEVALUE_PRECISION;
	}

	//
	void SetMilliSeconds( const int64 indwMilliSec )
	{
		m_lValue=indwMilliSec*(TIMEVALUE_PRECISION/1000);
	}

	// Use only for relative value, absolute values suffer a lot from precision loss.
	float GetMilliSeconds() const
	{
		return m_lValue * (1000.f/TIMEVALUE_PRECISION);
	}

	int64 GetMilliSecondsAsInt64() const
	{
		return m_lValue * 1000 / TIMEVALUE_PRECISION;
	}

	ILINE int64 GetValue() const
	{
		return m_lValue;
	}

	ILINE void SetValue( int64 val )
	{
		m_lValue = val;
	}

	// Description:
	//		Useful for periodic events (e.g. water wave, blinking).
	//		Changing TimePeriod can results in heavy changes in the returned value.
	// Return Value:
	//   [0..1[ 
	float GetPeriodicFraction( const CTimeValue TimePeriod ) const
	{
		// todo: change float implement to int64 for more precision
		float fAbs = GetSeconds()/TimePeriod.GetSeconds();
		return fAbs-(int)(fAbs);
	}

	// math operations -----------------------

	// Minus.
	ILINE CTimeValue operator-( const CTimeValue &inRhs ) const {	CTimeValue ret;	ret.m_lValue = m_lValue - inRhs.m_lValue;return ret; };
	// Plus.
	ILINE CTimeValue operator+( const CTimeValue &inRhs ) const {	CTimeValue ret;	ret.m_lValue = m_lValue + inRhs.m_lValue;return ret;	};
	// Unary minus.
	ILINE CTimeValue operator-() const { CTimeValue ret; ret.m_lValue = -m_lValue;return ret; };

	ILINE CTimeValue& operator+=( const CTimeValue& inRhs ) { m_lValue += inRhs.m_lValue; return *this; }
	ILINE CTimeValue& operator-=( const CTimeValue& inRhs ) { m_lValue -= inRhs.m_lValue; return *this; }

	ILINE CTimeValue& operator/=( int inRhs ) { m_lValue /= inRhs; return *this; }

	// comparison -----------------------

	ILINE bool operator<( const CTimeValue &inRhs ) const {	return m_lValue < inRhs.m_lValue; };
	ILINE bool operator>( const CTimeValue &inRhs ) const {	return m_lValue > inRhs.m_lValue;	};
	ILINE bool operator>=( const CTimeValue &inRhs ) const { return m_lValue >= inRhs.m_lValue;	};
	ILINE bool operator<=( const CTimeValue &inRhs ) const { return m_lValue <= inRhs.m_lValue;	};
	ILINE bool operator==( const CTimeValue &inRhs ) const { return m_lValue == inRhs.m_lValue;	};
	ILINE bool operator!=( const CTimeValue &inRhs ) const { return m_lValue != inRhs.m_lValue;	};

	//AUTO_STRUCT_INFO

private: // ----------------------------------------------------------

	int64									m_lValue;												// absolute or relative value in 1/TIMEVALUE_PRECISION, might be negative
	static const int64		TIMEVALUE_PRECISION=100000;			// one second


	friend class gkTimer;
};

#endif // _TIMEVALUE_H_
