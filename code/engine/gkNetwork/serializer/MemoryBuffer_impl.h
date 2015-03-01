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

// buffer.cpp
#include "MemoryBuffer.h"
#include "serializer.h"

#include <cassert>
#include <cstring>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/**
 * Equality operator
 **/

bool operator==(const CMemoryBuffer& lhs, const CMemoryBuffer& rhs)
{
  return (lhs.GetLength() == rhs.GetLength()) &&
    (memcmp(lhs.GetData(), rhs.GetData(), lhs.GetLength()) == 0);
}

/**
 * Append data to the end of the buffer
 **/

bool CMemoryBuffer::Write(const void* data, uint32 len)
{
  assert(data != 0);
  assert(len > 0);

  // resize to new length
  uint32 oldSize = (uint32)m_buf.size();
  m_buf.resize(oldSize + len);
  // copy directly into the vector's memory
  memcpy(&m_buf[oldSize], data, len);
  return true;
}

/**
 * Read bytes from the current read position
 **/

bool CMemoryBuffer::Read(void* dataOut, uint32 len)
{
  assert(dataOut != 0);
  assert(len > 0);

  // not enough to read?
  if (len > (m_buf.size() - m_readOffset))
    return false;
  // copy, advance read head
  memcpy(dataOut, &m_buf[m_readOffset], len);
  m_readOffset += len;
  return true;
}

/**
 * Serialization
 **/

SERIALIZE_DEFINE_PUT(CMemoryBuffer, obj, serializer)
{
  // store size and bytes, but not read offset
  uint32 len = obj.GetLength();
  return serializer.PutCount(len) &&
         ((len == 0) || serializer.PutRaw(obj.GetData(), len));
}

SERIALIZE_DEFINE_GET(CMemoryBuffer, obj, serializer)
{
  // clear existing contents
  obj.Clear();
  // read size
  uint32 len;
  if (!serializer.GetCount(len))
    return false;
  if (len == 0)
    return true;
  // resize and read data
  obj.m_buf.resize(len);
  return serializer.GetRaw(&obj.m_buf[0], len);
}
