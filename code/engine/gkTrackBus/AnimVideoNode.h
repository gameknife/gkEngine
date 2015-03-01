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
// Name:   	AnimVideoNode.h
// Desc:	
// 	
// 
// Author:  YiKaiming
// Date:	2013/9/26
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _AnimVideoNode_h_
#define _AnimVideoNode_h_

#include "AnimNodeBase.h"

class CVideoTrack;

class CAnimVideoNode : public CAnimNodeBase
{
public:
	CAnimVideoNode(void);
	~CAnimVideoNode(void);

	virtual void update( const SAnimateContext& context );

	virtual void create_default_track();

private:
	CVideoTrack* m_default_track;
};


#endif

