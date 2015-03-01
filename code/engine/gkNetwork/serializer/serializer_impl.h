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

// serializer.cpp
#include "serializer.h"

#include "MemoryBuffer.h"
#include "byteorder.h"
#include "misc.h"
#include <cassert>

#include "gkMemoryLeakDetecter.h"

/**
 * Constructor
 **/

CSerializer::CSerializer()
  : m_state(eStateNone),
    m_tagsEnabled(false),
    m_buffer(0),
	m_nOperaCount(0)
{
  // empty
}
CSerializer::CSerializer(const CSerializer& other)
:m_buffer(other.m_buffer),
m_state(other.m_state),
m_nOperaCount(other.m_nOperaCount),
m_tagsEnabled(other.m_tagsEnabled)
{

}
/**
 * Destructor
 **/

CSerializer::~CSerializer()
{
  m_state = eStateNone;
  m_buffer = 0;
  m_nOperaCount = 0;
}

/**
 * Initialize for writing
 **/

bool CSerializer::BeginWriting(CMemoryBuffer* outBuf, bool tagging)
{
  assert(outBuf != 0);

  m_state = eStateWriting;
  m_tagsEnabled = tagging;
  m_buffer = outBuf;
  m_nOperaCount = 0;
  return true;
}

/**
 * Finalize writing
 **/

bool CSerializer::EndWriting()
{
  m_state = eStateNone;
  m_buffer = 0;
  return true;
}

/**
 * Initialize for reading
 **/

bool CSerializer::BeginReading(CMemoryBuffer* inBuf, bool tagging)
{
  assert(inBuf != 0);

  m_state = eStateReading;
  m_tagsEnabled = tagging;
  m_buffer = inBuf;
  m_nOperaCount = 0;
  return true;
}

/**
 * Finalize reading
 **/

bool CSerializer::EndReading()
{
  m_state = eStateNone;
  m_buffer = 0;
  return true;
}


/**
 * Builtin Put/Get functions
 **/

bool CSerializer::Put(int8 x)
{
  return WriteNumber(x, eTagInt8);
}

bool CSerializer::Get(int8& x)
{
  return ReadNumber(x, eTagInt8);
}

bool CSerializer::Put(uint8 x)
{
  return WriteNumber(x, eTagUInt8);
}

bool CSerializer::Get(uint8& x)
{
  return ReadNumber(x, eTagUInt8);
}

bool CSerializer::Put(int16 x)
{
  x = HTON_I16(x);
  return WriteNumber(x, eTagInt16);
}

bool CSerializer::Get(int16& x)
{
  bool ret = ReadNumber(x, eTagInt16);
  x = NTOH_I16(x);
  return ret;
}

bool CSerializer::Put(uint16 x)
{
  x = HTON_I16(x);
  return WriteNumber(x, eTagUInt16);
}

bool CSerializer::Get(uint16& x)
{
  bool ret = ReadNumber(x, eTagUInt16);
  x = NTOH_I16(x);
  return ret;
}

bool CSerializer::Put(int32 x)
{
  x = HTON_I32(x);
  return WriteNumber(x, eTagInt32);
}

bool CSerializer::Get(int32& x)
{
  bool ret = ReadNumber(x, eTagInt32);
  x = NTOH_I32(x);
  return ret;
}

bool CSerializer::Put(uint32 x)
{
  x = HTON_I32(x);
  return WriteNumber(x, eTagUInt32);
}

bool CSerializer::Get(uint32& x)
{
  bool ret = ReadNumber(x, eTagUInt32);
  x = NTOH_I32(x);
  return ret;
}

bool CSerializer::Put(DWORD x)
{
	x = HTON_I32(x);
    return WriteNumber(x, eTagUInt32);
}

bool CSerializer::Get(DWORD& x)
{
	bool ret = ReadNumber(x, eTagUInt32);
	x = NTOH_I32(x);
	return ret;
}

bool CSerializer::Put(int64 x)
{
  x = HTON_I64(x);
  return WriteNumber(x, eTagInt64);
}

bool CSerializer::Get(int64& x)
{
  bool ret = ReadNumber(x, eTagInt64);
  x = NTOH_I64(x);
  return ret;
}

bool CSerializer::Put(uint64 x)
{
  x = HTON_I64(x);
  return WriteNumber(x, eTagUInt64);
}

bool CSerializer::Get(uint64& x)
{
  bool ret = ReadNumber(x, eTagUInt64);
  x = NTOH_I64(x);
  return ret;
}

bool CSerializer::Put(float32 x)
{
  x = HTON_F32(x);
  return WriteNumber(x, eTagFloat32);
}

bool CSerializer::Get(float32& x)
{
  bool ret = ReadNumber(x, eTagFloat32);
  x = NTOH_F32(x);
  return ret;
}

bool CSerializer::Put(float64 x)
{
  x = HTON_F64(x);
  return WriteNumber(x, eTagFloat64);
}

bool CSerializer::Get(float64& x)
{
  bool ret = ReadNumber(x, eTagFloat64);
  x = NTOH_F64(x);
  return ret;
}

bool CSerializer::Put(bool x)
{
  uint8 val = x ? 1 : 0;
  return WriteNumber(val, eTagBool);
}

bool CSerializer::Get(bool& x)
{
  uint8 val;
  bool ret = ReadNumber(val, eTagBool);
  x = (val == 1);
  return ret;
}

bool CSerializer::Put(const std::string& x)
{
  // tag
  if (!WriteTag(eTagString))
    return false;
  // size
  uint32 size = (uint32)x.size();
  if (!PutCount(size))
    return false;
  // data (only if there is some)
  return (size == 0) || WriteBytes(x.data(), size);
}

bool CSerializer::Put(const char* x)
{
  // could just return Put(string(x)), but that makes a copy..
  assert(x != 0);
  // tag
  if (!WriteTag(eTagString))
    return false;
  // size
  uint32 size = (uint32)strlen(x);
  if (!PutCount(size))
    return false;
  // data (only if there is some)
  return (size == 0) || WriteBytes(x, size);
}

bool CSerializer::Get(std::string& x)
{
  // erase contents
  x.erase();
  // tag
  if (!ReadAndCheckTag(eTagString))
    return false;
  // size
  uint32 size;
  if (!GetCount(size))
    return false;
  // data (only if there is some)
  if (size == 0)
    return true;
  // since we can't get a pointer to string's internal data, have to
  // use a temp buffer and copy it...
  char* tempString = new char[size];
  bool ok = ReadBytes(tempString, size);
  if (ok)
    x.assign(tempString, size);
  delete [] tempString;
  return ok;
}

bool CSerializer::Put(const std::wstring& x)
{
	// tag
	if (!WriteTag(eTagWString))
		return false;
	// size
	uint32 size = (uint32)x.size();
	if (!PutCount(size))
		return false;
	// data (only if there is some)
	return (size == 0) || WriteBytes(x.data(), size * sizeof(x[0]));
}

// bool CSerializer::Put(const CStringA& x)
// {
// 	// tag
// 	if (!WriteTag(eTagString))
// 		return false;
// 	// size
// 	uint32 size = (uint32)x.GetLength();
// 	if (!PutCount(size))
// 		return false;
// 	// data (only if there is some)
// 	return (size == 0) || WriteBytes(x.GetString(), size);
// }
// 
// bool CSerializer::Get(CStringA& x)
// {	
// 	// tag
// 	if (!ReadAndCheckTag(eTagString))
// 		return false;
// 	// size
// 	uint32 size;
// 	if (!GetCount(size))
// 		return false;
// 	// data (only if there is some)
// 	if (size == 0)
// 		return true;
// 
// 	bool ok = ReadBytes(x.GetBufferSetLength(size), size);	
// 	return ok;
// }
// 
// bool CSerializer::Put(const CStringW& x)
// {
// 	// tag
// 	if (!WriteTag(eTagWString))
// 		return false;
// 	// size
// 	uint32 size = (uint32)x.GetLength();
// 	if (!PutCount(size))
// 		return false;
// 	// data (only if there is some)
// 	return (size == 0) || WriteBytes(x.GetString(), size*sizeof(x[0]));
// }
// 
// bool CSerializer::Get(CStringW& x)
// {
// 	// tag
// 	if (!ReadAndCheckTag(eTagWString))
// 		return false;
// 	// size
// 	uint32 size;
// 	if (!GetCount(size))
// 		return false;
// 	// data (only if there is some)
// 	if (size == 0)
// 		return true;
// 
// 	bool ok = ReadBytes(x.GetBufferSetLength(size), size*sizeof(x[0]));	
// 	return ok;
// }

bool CSerializer::Put(const wchar_t* x)
{
	// could just return Put(string(x)), but that makes a copy..
	assert(x != 0);
	// tag
	if (!WriteTag(eTagWString))
		return false;
	// size
	uint32 size = (uint32)lstrlenW(x);
	if (!PutCount(size))
		return false;
	
	COMPILE_ASSERT( CStringSizeCheck, sizeof(x[0]) == sizeof(wchar_t) ); 

	// data (only if there is some)
	return (size == 0) || WriteBytes(x, size * sizeof(x[0]));

}

bool CSerializer::Get(std::wstring& x)
{
	// erase contents
	x.erase();
	// tag
	if (!ReadAndCheckTag(eTagWString))
		return false;
	// size
	uint32 size;
	if (!GetCount(size))
		return false;
	// data (only if there is some)
	if (size == 0)
		return true;
	// since we can't get a pointer to string's internal data, have to
	// use a temp buffer and copy it...
	wchar_t* tempString = new wchar_t[size];
	bool ok = ReadBytes(tempString, size * sizeof(wchar_t));
	if (ok)
		x.assign(tempString, size);
	delete [] tempString;
	return ok;
}

//bool CSerializer::Put(const D3DXVECTOR3& value)
//{
//	if(!Put(value.x))
//		return false;
//	if(!Put(value.y))
//		return false;
//	return Put(value.z);
//}
//
//bool CSerializer::Get(D3DXVECTOR3& value)
//{
//	if(!Get(value.x))
//		return false;
//	if(!Get(value.y))
//		return false;
//	return Get(value.z);
//}

/**
 * Raw byte arrays
 **/

bool CSerializer::PutRaw(const void* buf, uint32 len)
{
  assert(buf != 0);
  assert(len > 0);

  // note: no length field
  return WriteTag(eTagRawBytes) && WriteBytes(buf, len);
}

bool CSerializer::GetRaw(void* buf, uint32 len)
{
  assert(buf != 0);
  assert(len > 0);

  // note: no length field
  return ReadAndCheckTag(eTagRawBytes) && ReadBytes(buf, len);
}


/**
 * "Packed" count
 *
 * - 1-5 bytes, least significant bits first
 * - low 7 bits are data
 * - high bit is a stop bit
 **/

bool CSerializer::PutCount(uint32 count)	// Smg
{
	if (!WriteTag(eTagCount))
		return false;

	//int retval = 1;
	uint32 num1 = (uint32)count;
	while (num1 >= 0x80)
	{
		uint8 tmp = ((byte)(num1 | 0x80));
		if (!WriteBytes(&tmp, 1))
			return false;
		num1 = num1 >> 7;
		//retval++;
	}

	uint8 tmp = ((byte)num1);
	if (!WriteBytes(&tmp, 1))
		return false;
	//return retval;
	return true;
}

//bool CSerializer::PutCount(uint32 count)
//{
//  if (!WriteTag(eTagCount))
//    return false;
//
//  // encode loop
//  do
//  {
//    // get data
//    uint8 tmp = (count & 0x7F);
//    // shift it off
//    count = count >> 7;
//    // set stop bit
//    if (count == 0)
//      tmp |= 0x80;
//    // write
//    if (!WriteBytes(&tmp, 1))
//      return false;
//  } while (count != 0);
//
//  return true;
//}
//
//bool CSerializer::GetCount(uint32& count)
//{
//  if (!ReadAndCheckTag(eTagCount))
//    return false;
//
//  // decode loop
//  count = 0;
//  bool stopBit = false;
//  for (uint32 i = 0; (i < 5) && !stopBit; ++i)
//  {
//    // read
//    uint8 tmp;
//    if (!ReadBytes(&tmp, 1))
//      return false;
//    // stop bit?
//    stopBit = ((tmp & 0x80) != 0);
//    // mask data, shift up, or into count
//    count |= ((uint32) (tmp & 0x7F) << (i * 7));
//  }
//
//  // no stop bit == bad
//  return stopBit;
//}
bool CSerializer::GetCount(uint32& count) 	// Smg
{
	if (!ReadAndCheckTag(eTagCount))
		return false;

	int num1 = 0;
	int num2 = 0;
	while (true)
	{
		//if (num2 == 0x23)
		//	throw new FormatException("Bad 7-bit encoded integer");

		byte num3 = 0;
		if (!ReadBytes(&num3, 1))
			return false;
		//byte num3 = this.ReadByte();
		num1 |= (num3 & 0x7f) << (num2 & 0x1f);
		num2 += 7;
		if ((num3 & 0x80) == 0)
		{
			count = (uint32)num1;
			return true;
			//return (uint32)num1;
		}
	}
	return false;
}

/**
 * Write a tag
 *
 * If not in 'tagging' mode, then automatically succeeds.
 **/

bool CSerializer::WriteTag(const SerializeTag tag)
{
  assert((tag >= 0) && (tag <= 255)); // fit inside uint8

  uint8 tmp = tag;
  return !m_tagsEnabled || WriteBytes(&tmp, 1);
}

/**
 * Read a tag, and check it against an expected value
 *
 * If not in 'tagging' mode, then automatically succeeds.
 **/

bool CSerializer::ReadAndCheckTag(SerializeTag expectedTag)
{
  uint8 tmp;
  return !m_tagsEnabled || (ReadBytes(&tmp, 1) && (tmp == expectedTag));
}


/**
 * Write to the output CMemoryBuffer
 **/

bool CSerializer::WriteBytes(const void* buf, uint32 len)
{
  assert(buf != 0);
  assert(len > 0);
  assert(m_state == eStateWriting);

  return m_buffer->Write(buf, len);
}

/**
 * Read from the input CMemoryBuffer
 **/

bool CSerializer::ReadBytes(void* buf, uint32 len)
{
  assert(buf != 0);
  assert(len > 0);
  assert(m_state == eStateReading);

  return m_buffer->Read(buf, len);
}

void CSerializer::Reset()
{
	m_buffer->Reset();
}
