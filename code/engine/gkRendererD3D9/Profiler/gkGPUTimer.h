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
// Name:   	gkGPUTimer.h
// Desc:		
// 	
// Author:  Kaiming
// Date:	2012/2/2
// Modify:	2012/2/2
// 
//////////////////////////////////////////////////////////////////////////

#ifndef GKGPUTIMER_H_
#define GKGPUTIMER_H_
#include "RendererD3D9Prerequisites.h"

class gkGpuTimer
{
public:
	gkGpuTimer();
	gkGpuTimer(const TCHAR* name);
	virtual ~gkGpuTimer() {}


	gkStdString& getName() {return m_wstrName;}
	float getTime() {return m_fElapsedTime;}
	float getAvgTime() {return m_fAvgElapsedTime;}
	// init D3DQUERY [2/2/2012 Kaiming]
	void init(IDirect3DDevice9* pDevice);
	void destroy();

	// mark the timestamp
	void start();
	void stop();

	// get timestep when finish this frame
	void update();

private:
	gkStdString m_wstrName;

	IDirect3DQuery9* m_pEventStart;
	IDirect3DQuery9* m_pEventStop;
	IDirect3DQuery9* m_pEventFreq;

	float m_fElapsedTime;
	float m_fAvgElapsedTime;

	float m_tmpTime[20];
	int m_recorder;

	bool m_skip;


};

#endif

