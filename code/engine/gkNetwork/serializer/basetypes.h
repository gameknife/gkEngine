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



// Game Programming Gems 3
// Template-Based Object Serialization
// Author: Jason Beardsley

// basetypes.h

// Define base integral types: [u]int{8,16,32,64}
// Define base floating types: float32, float64
// Define MAKE_INT64 and MAKE_UINT64 macros for constants
// Define PRINTF_INT64 and PRINTF_UINT64 macros for printf format

#ifndef INCLUDED_BASETYPES_H
#define INCLUDED_BASETYPES_H

#include "gkplatform.h"

#if defined(OS_LINUX)
# include <stdint.h>
#endif

// gcc linux x86

#if defined(COMPILER_GCC) && defined(OS_LINUX) && defined(CPU_X86)

typedef int8_t int8;
typedef uint8_t uint8;
typedef int16_t int16;
typedef uint16_t uint16;
typedef int32_t int32;
typedef uint32_t uint32;
typedef int64_t int64;
typedef uint64_t uint64;

typedef float float32;
typedef double float64;

#define MAKE_INT64(x) (x ## LL)
#define MAKE_UINT64(x) (x ## ULL)

#define PRINTF_INT64 "%lld"
#define PRINTF_UINT64 "%llu"

// gcc NetBSD sparc

#elif defined(COMPILER_GCC) && defined(OS_NETBSD) && defined(CPU_SPARC)

typedef signed char int8;
typedef unsigned char uint8;
typedef signed short int16;
typedef unsigned short uint16;
typedef signed int int32;
typedef unsigned int uint32;
typedef signed long long int64;
typedef unsigned long long uint64;

typedef float float32;
typedef double float64;

#define MAKE_INT64(x) (x ## LL)
#define MAKE_UINT64(x) (x ## ULL)

#define PRINTF_INT64 "%lld"
#define PRINTF_UINT64 "%llu"

// gcc win32 x86

#elif defined(COMPILER_GCC) && defined(OS_WIN32) && defined(CPU_X86)

typedef signed char int8;
typedef unsigned char uint8;
typedef signed short int16;
typedef unsigned short uint16;
typedef signed int int32;
typedef unsigned int uint32;
typedef signed long long int64;
typedef unsigned long long uint64;

typedef float float32;
typedef double float64;

#define MAKE_INT64(x) (x ## LL)
#define MAKE_UINT64(x) (x ## ULL)

#define PRINTF_INT64 "%lld"
#define PRINTF_UINT64 "%llu"

// msvc win32 x86

#else // defined(COMPILER_MSVC) && defined(OS_WIN32) && defined(CPU_X86)

typedef signed char int8;
typedef unsigned char uint8;
typedef signed short int16;
typedef unsigned short uint16;
typedef signed int int32;
typedef unsigned int uint32;
typedef signed __int64 int64;
typedef unsigned __int64 uint64;

typedef float float32;
typedef double float64;

#define MAKE_INT64(x) (x ## I64)
#define MAKE_UINT64(x) (x ## UI64)

#define PRINTF_INT64 "%I64d"
#define PRINTF_UINT64 "%I64u"

//#else
//# error "Platform not recognized"
#endif

#endif // !INCLUDED_BASETYPES_H
