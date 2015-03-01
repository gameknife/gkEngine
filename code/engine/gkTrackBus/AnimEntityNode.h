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
// Name:   	AnimEntityNode.h
// Desc:	
// 	
// 
// Author:  YiKaiming
// Date:	2013/9/18
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _AnimEntityNode_h_
#define _AnimEntityNode_h_

#include "AnimNodeBase.h"

class IGameObject;

class CAnimEntityNode : public CAnimNodeBase
{
public:
	CAnimEntityNode( const TCHAR* name);
	~CAnimEntityNode(void);

	virtual void update( const SMovieContext& context );

	virtual void create_default_track();

private:
	IGameObject* m_target;
	gkStdString m_target_name;

	CSplineVec3Track* m_pos_track;
	CSplineVec3Track* m_rot_track;
	CSplineVec3Track* m_scale_track;
};


#endif

