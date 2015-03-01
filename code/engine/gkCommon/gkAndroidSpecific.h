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
// Name:   	gkAndroidSpecific.h
// Desc:	platform specific for android	
// 	
// Author:  Kaiming
// Date:	2012/6/13
// Modify:	
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _gkAndroidSpecific_h_
#define _gkAndroidSpecific_h_

inline unsigned long timeGetTime()
{
	timespec time;
	clock_gettime(CLOCK_REALTIME, &time);

	return (time.tv_sec * 1000 + time.tv_nsec / 1000000);

	// #define CLOCK_REALTIME             0
	// #define CLOCK_MONOTONIC            1
	// #define CLOCK_PROCESS_CPUTIME_ID   2
	// #define CLOCK_THREAD_CPUTIME_ID    3
	// #define CLOCK_REALTIME_HR          4
	// #define CLOCK_MONOTONIC_HR         5

}

#endif


