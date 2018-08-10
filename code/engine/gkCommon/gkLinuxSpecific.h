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
// Desc:	Platform Specific Wrapping for Andriod
// 	
// Author:  Kaiming
// Date:	2012/6/2
// Modify:	
// 
//////////////////////////////////////////////////////////////////////////
#ifndef _gkLinuxSpecific_h_
#define _gkLinuxSpecific_h_

#define DEBUG_BREAK
#define RC_EXECUTABLE "rc"
#define USE_CRT 1
#define SIZEOF_PTR 4

//////////////////////////////////////////////////////////////////////////
// Standard includes.
//////////////////////////////////////////////////////////////////////////
#include <stdint.h>
#include <fcntl.h>
#include <time.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <dlfcn.h>
#include <stdarg.h>

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Define platform independent types.
//////////////////////////////////////////////////////////////////////////
typedef signed char         int8;
typedef signed short        int16;
typedef signed int					int32;
typedef signed long long		int64;
typedef signed long long		INT64;
typedef unsigned char				uint8;
typedef unsigned short			uint16;
typedef unsigned int				uint32;
typedef unsigned long long	uint64;

typedef float               f32;
typedef double              f64;
typedef double         real;

// old-style (will be removed soon) 
typedef signed char         s8;
typedef signed short        s16;
typedef signed int         s32;
typedef signed long long		s64;
typedef unsigned char				u8;
typedef unsigned short			uint16;
typedef unsigned int				uint32;
typedef unsigned long long	u64;

typedef unsigned int				DWORD;
typedef unsigned long*				LPDWORD;
typedef DWORD								DWORD_PTR;
#ifdef __x86_64__ 
typedef long INT_PTR, *PINT_PTR;
typedef unsigned long UINT_PTR, *PUINT_PTR;
typedef uint64 gk_ptr_type;
#else
typedef int INT_PTR, *PINT_PTR;
typedef unsigned int UINT_PTR, *PUINT_PTR;
#if defined(__x86_64__) || defined(__arm64__)
typedef uint64 gk_ptr_type;
#else
typedef uint32 gk_ptr_type;
#endif
#endif



typedef char *LPSTR, *PSTR;

typedef long LONG_PTR, *PLONG_PTR, *PLONG;
typedef unsigned long ULONG_PTR, *PULONG_PTR;

typedef unsigned char				BYTE;
typedef unsigned short			WORD;
typedef void*								HWND;
typedef UINT_PTR 						WPARAM;
typedef LONG_PTR 						LPARAM;
typedef LONG_PTR 						LRESULT;
#define PLARGE_INTEGER LARGE_INTEGER*
typedef const char *LPCSTR, *PCSTR;
typedef long long						LONGLONG;
typedef	ULONG_PTR						SIZE_T;
typedef unsigned char				byte;

#define __int64		long long

#define ILINE __forceinline

#define _A_RDONLY (0x01)
#define _A_SUBDIR (0x10)

//////////////////////////////////////////////////////////////////////////
// Win32 FileAttributes.
//////////////////////////////////////////////////////////////////////////
#define FILE_ATTRIBUTE_READONLY             0x00000001  
#define FILE_ATTRIBUTE_HIDDEN               0x00000002  
#define FILE_ATTRIBUTE_SYSTEM               0x00000004  
#define FILE_ATTRIBUTE_DIRECTORY            0x00000010  
#define FILE_ATTRIBUTE_ARCHIVE              0x00000020  
#define FILE_ATTRIBUTE_DEVICE               0x00000040  
#define FILE_ATTRIBUTE_NORMAL               0x00000080  
#define FILE_ATTRIBUTE_TEMPORARY            0x00000100  
#define FILE_ATTRIBUTE_SPARSE_FILE          0x00000200  
#define FILE_ATTRIBUTE_REPARSE_POINT        0x00000400  
#define FILE_ATTRIBUTE_COMPRESSED           0x00000800  
#define FILE_ATTRIBUTE_OFFLINE              0x00001000  
#define FILE_ATTRIBUTE_NOT_CONTENT_INDEXED  0x00002000  
#define FILE_ATTRIBUTE_ENCRYPTED            0x00004000

#define INVALID_FILE_ATTRIBUTES (-1)

#define DEFINE_ALIGNED_DATA( type, name, alignment ) \
	type __attribute__ ((aligned(alignment))) name;
#define DEFINE_ALIGNED_DATA_STATIC( type, name, alignment ) \
	static type __attribute__ ((aligned(alignment))) name;
#define DEFINE_ALIGNED_DATA_CONST( type, name, alignment ) \
	const type __attribute__ ((aligned(alignment))) name;

// MSVC compiler-specific keywords
#define __forceinline inline
#define _inline inline
#define __cdecl
#define _cdecl
#define __stdcall
#define _stdcall
#define __fastcall
#define _fastcall
#define IN
#define OUT

#define TCHAR char
//typedef const TCHAR *const TCHAR*;
#define CHAR char
#define MAX_PATH          260
#define _T(varr)	varr

#ifndef FALSE
#define FALSE               0
#endif

#ifndef TRUE
#define TRUE                1
#endif

#define HWND void*
#define HINSTANCE void*
#define HRESULT void*
#define HANDLE void*

// Safe memory freeing
#ifndef SAFE_DELETE
#define SAFE_DELETE(p)			{ if(p) { delete (p);		(p)=NULL; } }
#endif

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p)	{ if(p) { delete[] (p);		(p)=NULL; } }
#endif

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)			{ if(p) { (p)->Release();	(p)=NULL; } }
#endif


#ifndef SAFE_RELEASE_FORCE
#define SAFE_RELEASE_FORCE(p)		{ if(p) { (p)->ReleaseForce();	(p)=NULL; } }
#endif


#define S_OK                                   ((HRESULT)0L)
#define S_FALSE                                ((HRESULT)1L)


#define E_FAIL                           0
#define E_ACCESSDENIED                   0
#define E_NOTIMPL                        0
#define E_OUTOFMEMORY                    0
#define E_INVALIDARG                     0

#define FW_DONTCARE         0
#define FW_THIN             100
#define FW_EXTRALIGHT       200
#define FW_LIGHT            300
#define FW_NORMAL           400
#define FW_MEDIUM           500
#define FW_SEMIBOLD         600
#define FW_BOLD             700
#define FW_EXTRABOLD        800
#define FW_HEAVY            900

typedef struct _GUID {
	unsigned long  Data1;
	unsigned short Data2;
	unsigned short Data3;
	unsigned char  Data4[ 8 ];
} GUID;

#define ZeroMemory(dest, target) memset(dest, 0, target)

#include "gkLinuxTchar.h"

#endif //_gkAndroidSpecific_h_
