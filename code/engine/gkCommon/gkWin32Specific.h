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
// Name:   	gkWin32Specific.h
// Desc:	win32 引入	
// 
// Author:  Kaiming-Desktop
// Date:	2011 /7/29
// Modify:	2011 /7/29
// 
//////////////////////////////////////////////////////////////////////////

#ifndef __GKWIN32SPECIFIC_H__
#define __GKWIN32SPECIFIC_H__

#ifndef _M_X64
#define _CPU_X86
#define _CPU_SSE
#endif 

#ifndef _AFXDLL
#include <Windows.h>
#endif

	#define DEPRICATED __declspec(deprecated)

	//////////////////////////////////////////////////////////////////////////
	// Define platform independent types.
	//////////////////////////////////////////////////////////////////////////
	typedef signed char				int8;
	typedef signed short			int16;
	typedef signed int				int32;
	typedef signed __int64			int64;
	typedef unsigned char			uint8;
	typedef unsigned char			BYTE;
	typedef unsigned short			uint16;
	typedef unsigned int			uint32;
	typedef unsigned __int64		uint64;
	typedef float					f32;
	typedef double					f64;
	typedef double					real;  //biggest float-type on this machine

	typedef unsigned long			DWORD;  //biggest float-type on this machine

	#if defined(_WIN64)
	typedef __int64 INT_PTR, *PINT_PTR;
	typedef unsigned __int64 UINT_PTR, *PUINT_PTR;

	typedef __int64 LONG_PTR, *PLONG_PTR;
	typedef unsigned __int64 ULONG_PTR, *PULONG_PTR;

	typedef ULONG_PTR DWORD_PTR, *PDWORD_PTR;

	typedef __int64 gk_ptr_type;
	#else
	typedef int gk_ptr_type;

	typedef __w64 int INT_PTR, *PINT_PTR;
	typedef __w64 unsigned int UINT_PTR, *PUINT_PTR;

	typedef __w64 long LONG_PTR, *PLONG_PTR;
	typedef __w64 unsigned long ULONG_PTR, *PULONG_PTR;

	typedef ULONG_PTR DWORD_PTR, *PDWORD_PTR;
	#endif

	typedef void *THREAD_HANDLE;
	typedef void *EVENT_HANDLE;

#if defined(DEBUG) || defined(_DEBUG)
#ifndef V
#define V(x)           { hr = (x); if( FAILED(hr) ) { /*DXUTTrace( __FILE__, (DWORD)__LINE__, hr, L#x, true )*/; } }
#endif
#ifndef V_RETURN
#define V_RETURN(x)    { hr = (x); if( FAILED(hr) ) { return hr/*DXUTTrace( __FILE__, (DWORD)__LINE__, hr, L#x, true )*/; } }
#endif
#else
#ifndef V
#define V(x)           { hr = (x); }
#endif
#ifndef V_RETURN
#define V_RETURN(x)    { hr = (x); if( FAILED(hr) ) { return hr; } }
#endif
#endif

	#ifndef SAFE_DELETE
	#define SAFE_DELETE(p)			{ if(p) { delete (p);		(p)=NULL; } }
	#endif

	#ifndef SAFE_DELETE_ARRAY
	#define SAFE_DELETE_ARRAY(p)	{ if(p) { delete [] (p);		(p)=NULL; } }
	#endif

	#ifndef SAFE_RELEASE
	#define SAFE_RELEASE(p)			{ if(p) { (p)->Release();	(p)=NULL; } }
	#endif

	#ifndef SAFE_release
	#define SAFE_release(p)			{ if(p) { (p)->release();	(p)=NULL; } }
	#endif

	//#ifdef _DEBUG
	#define ILINE inline
	//#else
	//#define ILINE inline
	//#endif
	#include <tchar.h>
#include "io.h"
#include <dos.h>
#endif