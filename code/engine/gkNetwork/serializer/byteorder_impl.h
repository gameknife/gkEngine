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

// byteorder.cpp
#include "byteorder.h"

#define TEMP_SWAP(a,b,tmp) tmp = a; a = b; b = tmp

// Note: If you really need the fastest implementation of these
// functions, write them in assembly for each particular platform
// and/or compiler.

/**
 * Returns physically converted 16-bit integer
 **/

uint16 ByteOrder::SwapInt16(uint16 x)
{
  // get the individual bytes
  uint8 b0 = static_cast<uint8>(x & 0xFF);
  uint8 b1 = static_cast<uint8>((x >> 8) & 0xFF);
  // put them back in reverse order
  return static_cast<uint16>((b0 << 8) | b1);
}

/**
 * Returns physically converted 32-bit integer
 **/

uint32 ByteOrder::SwapInt32(uint32 x)
{
  // get the individual bytes
  uint8 b0 = static_cast<uint8>(x & 0xFF);
  uint8 b1 = static_cast<uint8>((x >> 8) & 0xFF);
  uint8 b2 = static_cast<uint8>((x >> 16) & 0xFF);
  uint8 b3 = static_cast<uint8>((x >> 24) & 0xFF);
  // put them back in reverse order
  return (b0 << 24) | (b1 << 16) | (b2 << 8) | b3;
}

/**
 * Returns physically converted 64-bit integer
 **/

uint64 ByteOrder::SwapInt64(uint64 x)
{
  // use union hack
  union
  {
    uint64 val;
    uint8 bytes[8];
  } u;
  u.val = x;
  // 0 1 2 3 4 5 6 7 -> 7 6 5 4 3 2 1 0
  uint8 tmp;
  TEMP_SWAP(u.bytes[0], u.bytes[7], tmp);
  TEMP_SWAP(u.bytes[1], u.bytes[6], tmp);
  TEMP_SWAP(u.bytes[2], u.bytes[5], tmp);
  TEMP_SWAP(u.bytes[3], u.bytes[4], tmp);
  return u.val;
}

/**
 * Returns physically converted 32-bit float
 **/

float32 ByteOrder::SwapFloat32(float32 x)
{
  // use union hack
  union
  {
    float32 val;
    uint8 bytes[4];
  } u;
  u.val = x;
  // 0 1 2 3 -> 3 2 1 0
  uint8 tmp;
  TEMP_SWAP(u.bytes[0], u.bytes[3], tmp);
  TEMP_SWAP(u.bytes[1], u.bytes[2], tmp);
  return u.val;
}

/**
 * Returns physically converted 64-bit float
 **/

float64 ByteOrder::SwapFloat64(float64 x)
{
  // use union hack
  union
  {
    float64 val;
    uint8 bytes[8];
  } u;
  u.val = x;
  // 0 1 2 3 4 5 6 7 -> 7 6 5 4 3 2 1 0
  uint8 tmp;
  TEMP_SWAP(u.bytes[0], u.bytes[7], tmp);
  TEMP_SWAP(u.bytes[1], u.bytes[6], tmp);
  TEMP_SWAP(u.bytes[2], u.bytes[5], tmp);
  TEMP_SWAP(u.bytes[3], u.bytes[4], tmp);
  return u.val;
}
