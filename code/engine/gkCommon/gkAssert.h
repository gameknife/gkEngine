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
// Name:   	gkAssert.h
// Desc:	GameKnife Assert 可以将assert绑定	
// 
// Author:  Kaiming-Desktop
// Date:	2011 /7/29
// Modify:	2011 /7/29
// 
//////////////////////////////////////////////////////////////////////////

#ifndef __GKASSERT_H__
#define __GKASSERT_H__
#pragma once

#ifdef OS_WIN32
	#include <assert.h>
	#define GK_ASSERT(condition) assert(condition)
	//#define _ASSERT(condition) assert(condition)
	#define GK_ASSERT_MESSAGE(condition,message) assert(condition)
	#define GK_ASSERT_TRACE(condition,parenthese_message) assert(condition)

#endif

#ifdef OS_IOS
#define assert(a)
#define GK_ASSERT(a)
#endif

#ifdef OS_ANDROID
#define assert(a) 
#define GK_ASSERT(a)
#endif

#endif