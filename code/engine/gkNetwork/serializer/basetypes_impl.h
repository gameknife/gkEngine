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

// basetypes.cpp
#include "basetypes.h"
#include "misc.h"

// Check sizes of the base types

COMPILE_ASSERT( Size_int8, sizeof(int8) == 1 );
COMPILE_ASSERT( Size_uint8, sizeof(uint8) == 1 );
COMPILE_ASSERT( Size_int16, sizeof(int16) == 2 );
COMPILE_ASSERT( Size_uint16, sizeof(uint16) == 2 );
COMPILE_ASSERT( Size_int32, sizeof(int32) == 4 );
COMPILE_ASSERT( Size_uint32, sizeof(uint32) == 4 );
COMPILE_ASSERT( Size_int64, sizeof(int64) == 8 );
COMPILE_ASSERT( Size_uint64, sizeof(uint64) == 8 );

COMPILE_ASSERT( Size_float32, sizeof(float32) == 4 );
COMPILE_ASSERT( Size_float64, sizeof(float64) == 8 );
