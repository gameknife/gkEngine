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
// Name:   	TSplineTrack.h
// Desc:	
// 	
// 
// Author:  YiKaiming
// Date:	2013/9/16
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _TSplineTrack_h_
#define _TSplineTrack_h_

#include "ITrackBus.h"
#include "TBaseTrack.h"
#include "I2DSpline.h"

#define MAX_SPLINE_COUNT 4
class CSplineVec3Track : public TBaseTrack<ISplineVec3Key>
{
public:
	CSplineVec3Track(float default_value);
	~CSplineVec3Track(void);

	virtual bool set_key( IKey* key );

	virtual void remove_key( float time );

	virtual void get_value( float time, Vec3& out );

	virtual void get_value( float time, float& out );

private:
	I2DSpline<float, BezierBasis>* m_splines[3];
};

class CSplineTCBTrack : public TBaseTrack<ISplineVec3Key>
{
public:
	CSplineTCBTrack(Vec3 default_value);
	~CSplineTCBTrack(void);

	virtual bool set_key( IKey* key );

	virtual void remove_key( float time );

	virtual void get_value( float time, Vec3& out );

	virtual void get_value( float time, float& out );

private:
	I2DSpline<Vec3, HermitBasis>* m_splines[1];
};

#endif


