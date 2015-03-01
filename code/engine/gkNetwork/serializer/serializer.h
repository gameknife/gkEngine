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

// serializer.h
#pragma once
#ifndef INCLUDED_SERIALIZER_H
#define INCLUDED_SERIALIZER_H
#include "gkplatform.h"
//#include "BaseDefExport.h"
#include "basetypes.h"
#include "serdefs.h"

//#include "MemArchive.h"

#include <string>


class CMemoryBuffer; // fwd

class UNIQUE_IFACE CSerializerException
{
	std::string	m_strInfo;
public:
	CSerializerException(const char* lpszInfo)
	{
		m_strInfo = lpszInfo;
	};

	~CSerializerException(){};
	const char *ShowReason() const 
	{
		return m_strInfo.c_str();
	}
};

/**
 * CSerializer class
 **/
// 描述：	提供了网络序列化常用数据类型的功能
// 注：本类在读取或存储时，如果发生错误会抛出异常CSerializerException，使用时要注意处理
class UNIQUE_IFACE _tag_serializer_call_me_with_getref_and_getval;
class UNIQUE_IFACE  CSerializer
{
	int16	m_nOperaCount;		// 操作计数
public:
  CSerializer();
  ~CSerializer();

  bool BeginWriting(CMemoryBuffer* outBuf, bool tagging = false);
  bool EndWriting();

  bool BeginReading(CMemoryBuffer* inBuf, bool tagging = false);
  bool EndReading();

  // 功能：	用于返回当前是否在读取数据
  bool IsLoading() const
  {
	  return m_state == eStateReading;
  }

  // 功能：	用于返回当前是否在存储数据
  bool IsStoring() const
  {
	  return m_state == eStateWriting;
  }

  // add by smg 2008.11.18
  // 增加重置函数以便多处读取。
  void Reset();

  /**
   * Store an object.
   **/

  template <typename T>
  bool Put(const T& obj)
  {
    SerializeTag tag = SerializeHelper::ComputeTag(obj);
    return WriteTag(tag) && SerializeHelper::PutInto(*this, obj);
  }

  template < template<typename, typename, typename> class _CNetworkVar, typename _Ty, typename _Mod >
	  bool Put( const _CNetworkVar<_Ty, _Mod, _tag_serializer_call_me_with_getref_and_getval>& obj )
  {
	  return Put(obj.GetVal());
  }

  /**
   * Read an object.
   **/

  template <typename T>
  bool Get(T& obj)
  {
    SerializeTag tag = SerializeHelper::ComputeTag(obj);
    return ReadAndCheckTag(tag) && SerializeHelper::GetFrom(*this, obj);
  }

  template < template<typename, typename, typename> class _CNetworkVar, typename _Ty, typename _Mod >
	  bool Get( _CNetworkVar<_Ty, _Mod, _tag_serializer_call_me_with_getref_and_getval>& obj )
  {
	  return Get(obj.GetRef());
  }

  // Support for builtin types; these could be handled through the
  // above template functions, but it's undoubtedly more efficient to
  // put them here (less template parsing for the compiler).

  template <typename T>
  CSerializer& operator<<(const T& x)
  {
	  if(!Put(x))
	  {
		  //CStringA strInfo;
		  //strInfo.Format("CSerializer在第 %d 次存储数据内容时发生错误，读取中断，后续的内容将无效",  m_nOperaCount);
		  //CSerializerException exception(strInfo);
		  //throw exception;
	  }

	  m_nOperaCount++;
	  return *this;
  }

  template <typename T>
  CSerializer& operator>>(T& x)
  {	  
	  if(!Get(x))
	  {
// 		  CStringA strInfo;
// 		  strInfo.Format("CSerializer在第 %d 次读取数据内容时发生错误，读取中断，后续的内容将无效",  m_nOperaCount);
// 		  CSerializerException exception(strInfo);
// 		  throw exception;
	  }
	  m_nOperaCount++;
	  return *this;
  }

  bool Put(int8 x);
  bool Get(int8& x);
  bool Put(uint8 x);
  bool Get(uint8& x);
  bool Put(int16 x);
  bool Get(int16& x);
  bool Put(uint16 x);
  bool Get(uint16& x);
  bool Put(int32 x);
  bool Get(int32& x);
  bool Put(uint32 x);
  bool Get(uint32& x);
  bool Put(DWORD x);
  bool Get(DWORD& x);
  bool Put(int64 x);
  bool Get(int64& x);
  bool Put(uint64 x);
  bool Get(uint64& x);

  bool Put(float32 x);
  bool Get(float32& x);
  bool Put(float64 x);
  bool Get(float64& x);

  bool Put(bool x);
  bool Get(bool& x);

  bool Put(const std::string& x);
  bool Put(const char* x);
  bool Get(std::string& x);
  bool Put(const std::wstring& x);
  bool Put(const wchar_t* x);
  bool Get(std::wstring& x);
  //bool Put(const CStringA& x);
  //bool Get(CStringA& x);
  //bool Put(const CStringW& x);
  //bool Get(CStringW& x);
  //bool Put(const D3DXVECTOR3& x);
  //bool Get(D3DXVECTOR3& x);

  //
  // "non-standard" Put/Get methods
  //

  // raw byte array of known size
  bool PutRaw(const void* buf, uint32 len);
  bool GetRaw(void* buf, uint32 len);

  // packed count (for variable-length things)
  bool PutCount(uint32 count);
  bool GetCount(uint32& count);

  /**
   * Store an array of objects
   **/

  template <typename T>
  bool PutArray(const T* objs, uint32 numItems)
  {
    // write tag
    if (!WriteTag(eTagArray))
      return false;
    // store size
    if (!PutCount(numItems))
      return false;
    // store each element
    for (uint32 i = 0; i < numItems; ++i)
      if (!Put(objs[i]))
        return false;
    return true;
  }

  /**
   * Read an array of objects
   **/

  template <typename T>
  bool GetArray(T* objs, uint32& numInOut)
  {
    // check tag
    if (!ReadAndCheckTag(eTagArray))
      return false;
    // retrieve the size
    uint32 numItems;
    if (!GetCount(numItems))
      return false;
    // too many to read?
    if (numItems > numInOut)
      return false;
    // read each element
    for (uint32 i = 0; i < numItems; ++i)
      if (!Get(objs[i]))
        return false;
    numInOut = numItems;
    return true;
  }

private:

  // Helper functions for Put/Get on numeric types; assume that
  // byteorder swapping happens elsewhere

  template <typename T>
  inline bool WriteNumber(T x, SerializeTag tag)
  {
    return WriteTag(tag) && WriteBytes(&x, sizeof(T));
  }

  template <typename T>
  inline bool ReadNumber(T& x, SerializeTag tag)
  {
    return ReadAndCheckTag(tag) && ReadBytes(&x, sizeof(T));
  }

  // tags
  bool WriteTag(SerializeTag tag);
  bool ReadAndCheckTag(SerializeTag expectedTag);

  // low-level CMemoryBuffer access
  bool WriteBytes(const void* buf, uint32 len);
  bool ReadBytes(void* buf, uint32 len);

  enum State
  {
    eStateNone,
    eStateReading,
    eStateWriting
  };

  State m_state;
  bool m_tagsEnabled;
  CMemoryBuffer* m_buffer;

  // disabled - no copying
  CSerializer(const CSerializer& other);
  void operator=(const CSerializer&);



}; // end CSerializer class

#endif // !INCLUDED_SERIALIZER_H
