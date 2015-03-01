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
// Name:   	AnimCameraNode.h
// Desc:	
// 	
// 
// Author:  YiKaiming
// Date:	2013/9/16
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _AnimCameraNode_h_
#define _AnimCameraNode_h_

#include "AnimNodeBase.h"

class CSplineVec3Track;
class CSplineFloatTrack;
class ICamera;


class CAnimCameraNode : public CAnimNodeBase
{
public:
	CAnimCameraNode(const TCHAR* cam_name);
	~CAnimCameraNode(void);

	virtual void update( const SMovieContext& context );

	virtual void create_default_track();

private:
	ICamera* m_target;
	gkStdString m_cam_name;

	CSplineVec3Track* m_pos_track;
	CSplineVec3Track* m_target_track;
	CSplineFloatTrack* m_fov_track;
};


#endif

