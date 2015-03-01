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

// byteorder.h

#ifndef INCLUDED_BYTEORDER_H
#define INCLUDED_BYTEORDER_H

#include "gkplatform.h"
#include "basetypes.h"

/**
 * Functions and macros for converting data between big-endian and
 * little-endian architectures.  It handles various sizes of integers
 * (8, 16, 32, 64) and floats (32, 64).
 **/

namespace ByteOrder
{
  uint16 SwapInt16(uint16 x);
  uint32 SwapInt32(uint32 x);
  uint64 SwapInt64(uint64 x);
  float32 SwapFloat32(float32 x);
  float64 SwapFloat64(float64 x);
};

// byteorder definitions

#define BYTEORDER_BIG     'B'
#define BYTEORDER_LITTLE  'L'

//#if defined(CPU_X86)
//	# define BYTEORDER_HOST BYTEORDER_LITTLE
# define BYTEORDER_HOST BYTEORDER_BIG		// 目前不需要跨平台使用这些函数，所以不需要做转换
// #elif defined(CPU_SPARC)
// # define BYTEORDER_HOST BYTEORDER_BIG
// #else
// # error "Unrecognized architecture."
// #endif

// network byte order; doesn't matter which one you choose

#define BYTEORDER_NET BYTEORDER_BIG

// conversion macros

#if (BYTEORDER_HOST == BYTEORDER_NET)
# define HTON_I16(x) (x)
# define HTON_I32(x) (x)
# define HTON_I64(x) (x)
# define HTON_F32(x) (x)
# define HTON_F64(x) (x)
#else
# define HTON_I16(x) (ByteOrder::SwapInt16(static_cast<uint16>(x)))
# define HTON_I32(x) (ByteOrder::SwapInt32(static_cast<uint32>(x)))
# define HTON_I64(x) (ByteOrder::SwapInt64(static_cast<uint64>(x)))
# define HTON_F32(x) (ByteOrder::SwapFloat32(x))
# define HTON_F64(x) (ByteOrder::SwapFloat64(x))
#endif

#define NTOH_I16(x) HTON_I16(x)
#define NTOH_I32(x) HTON_I32(x)
#define NTOH_I64(x) HTON_I64(x)
#define NTOH_F32(x) HTON_F32(x)
#define NTOH_F64(x) HTON_F64(x)

#endif // !INCLUDED_BYTEORDER_H
