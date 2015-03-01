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

// buffer.h

#ifndef INCLUDED_BUFFER_H
#define INCLUDED_BUFFER_H

#include "gkPlatform.h"
#include "basetypes.h"
#include "serdefs.h"

#include <vector>

// serialization support
class UNIQUE_IFACE CMemoryBuffer;
SERIALIZE_DEFINE_TAG(CMemoryBuffer, eTagBuffer);
SERIALIZE_DECLARE_PUTGET(CMemoryBuffer);

/**
 * An automatically resizable byte array, using vector<uint8> for
 * underlying storage.
 *
 * Writing to the buffer always appends to the end.
 *
 * Reading is non-destructive, and uses a "read head" which can be
 * reset back to the front.
 **/

// 注：类级线程安全的，请注意不要使用全局或静态变量等线程不安全的东西，使用了打PP

class UNIQUE_IFACE CMemoryBuffer
{

public:

  CMemoryBuffer();

  // 功能：	根据传入的数据构造本类
  // 参数：	dataToRead - 指定用于从中读取数据的类
  //		len - 指定上面缓冲的长度
  CMemoryBuffer(const void* dataToRead, uint32 len);
  CMemoryBuffer(const CMemoryBuffer& other);

  ~CMemoryBuffer();

  const CMemoryBuffer& operator=(const CMemoryBuffer& other);

  bool Write(const void* data, uint32 len);
  bool Read(void* dataOut, uint32 len);
  const void* GetData() const;
  uint32 GetLength() const;
  void Clear();
  void Reset();
  uint32 GetReadLength() const;

  void Swap(CMemoryBuffer& other);

private:

  std::vector<uint8> m_buf; // buffer
  uint32 m_readOffset; // "read head" offset from m_buf[0]

  // serialization needs friend access
  SERIALIZE_DECLARE_FRIEND(CMemoryBuffer);

};

// equality operator

bool operator==(const CMemoryBuffer& lhs, const CMemoryBuffer& rhs);

// inline methods

/**
 * Constructor
 **/

inline CMemoryBuffer::CMemoryBuffer()
  : m_readOffset(0)
{
}

/**
 * Copy constructor
 **/

inline CMemoryBuffer::CMemoryBuffer(const CMemoryBuffer& other)
  : m_buf(other.m_buf),
    m_readOffset(other.m_readOffset)
{
}

inline CMemoryBuffer::CMemoryBuffer(const void* dataToRead, uint32 len) 
	: m_readOffset(0)
{
	if(len > 5*1024*1024)
	{	// 这里的支持的数据都要小于5M
		assert(FALSE);
	}
	else
	{
		//m_buf.resize(len);
		const uint8* pDataToRead = static_cast<const uint8*>(dataToRead);
		m_buf.assign( pDataToRead, pDataToRead + len);
	}	
}

/**
 * Destructor
 **/

inline CMemoryBuffer::~CMemoryBuffer()
{
}

/**
 * Assignment operator
 **/

inline const CMemoryBuffer& CMemoryBuffer::operator=(const CMemoryBuffer& other)
{
  // check self-assignment
  if (this != &other)
  {
    m_buf = other.m_buf;
    m_readOffset = other.m_readOffset;
  }
  return *this;
}

/**
 * Swap contents with another buffer
 **/

inline void CMemoryBuffer::Swap(CMemoryBuffer& other)
{
  m_buf.swap(other.m_buf);
  uint32 tmp = m_readOffset;
  m_readOffset = other.m_readOffset;
  other.m_readOffset = tmp;
}

/**
 * Return a pointer to the beginning of the buffer, or NULL if the
 * buffer is empty.
 **/

inline const void* CMemoryBuffer::GetData() const
{
  return m_buf.empty() ? 0 : &m_buf[0];
}

/**
 * Return the buffer size
 **/

inline uint32 CMemoryBuffer::GetLength() const
{
  return (uint32)m_buf.size();
}

/**
 * Erase buffer contents
 **/

inline void CMemoryBuffer::Clear()
{
  m_buf.clear();
  m_readOffset = 0;
}

/**
 * Reset read head
 **/

inline void CMemoryBuffer::Reset()
{
  m_readOffset = 0;
}

/**
 * Return bytes available for reading
 **/

inline uint32 CMemoryBuffer::GetReadLength() const
{
  return (uint32)(m_buf.size() - m_readOffset);
}

#endif // INCLUDED_BUFFER_H
