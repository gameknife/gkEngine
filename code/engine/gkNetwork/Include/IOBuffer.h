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



#pragma once

#ifndef UNICOMM_TOOLS_WIN32_IO_BUFFER_INCLUDED__
#define UNICOMM_TOOLS_WIN32_IO_BUFFER_INCLUDED__

#include "NetWorkPrerequisites.h"
#include "CriticalSection.h" 

#include "NodeList.h"
#include "OpaqueUserData.h"
#include <afxtempl.h>
#include "afxcmn.h"


#pragma warning(disable: 4200)


///////////////////////////////////////////////////////////////////////////////
// CIOBuffer::Allocator
///////////////////////////////////////////////////////////////////////////////

class GAMEKNIFENETWORK_API CIOBuffer : public OVERLAPPED, public CNodeList::Node, public COpaqueUserData
{
   public :

      class Allocator;

      friend class Allocator;

      class InOrderBufferList;

      WSABUF *GetWSABUF() const;

      DWORD GetUsed() const;

      DWORD GetSize() const;

      const BYTE *GetBuffer() const;

      void SetupZeroByteRead();

      void SetupRead();

      void SetupWrite();
      
      void AddData(
         const char * const pData,
         DWORD dataLength);

      void AddData(
         const BYTE * const pData,
         DWORD dataLength);

      void AddData(
         BYTE data);

      void Use(
         DWORD dataUsed);

      CIOBuffer *SplitBuffer(
         DWORD bytesToRemove);

      CIOBuffer *AllocateNewBuffer() const;

      void ConsumeAndRemove(
         DWORD bytesToRemove);

      void Empty();

      void AddRef();
      void Release();

      DWORD GetOperation() const;
      
      void SetOperation(
         DWORD operation);

      DWORD GetSequenceNumber() const;
      
      void SetSequenceNumber(
         DWORD sequenceNumber);

   private :

      DWORD m_operation;
      DWORD m_sequenceNumber;

      WSABUF m_wsabuf;

      Allocator &m_allocator;

      long m_ref;
      const DWORD m_size;
      DWORD m_used;
      BYTE m_buffer[0];      // start of the actual buffer, must remain the last
                             // data member in the class.

   private :

      static void *operator new(size_t objSize, size_t bufferSize);
	  static void operator delete(void *pObj, size_t bufferSize);

	  //void* operator new(size_t nSize)
      //  { return new char[nSize];}
     

	  CIOBuffer(
         Allocator &m_allocator,
         DWORD size);

      // No copies do not implement
      CIOBuffer(const CIOBuffer &rhs);
      CIOBuffer &operator=(const CIOBuffer &rhs);
};

///////////////////////////////////////////////////////////////////////////////
// CIOBuffer::Allocator
///////////////////////////////////////////////////////////////////////////////

class  GAMEKNIFENETWORK_API CIOBuffer::Allocator
{
   public :

      friend class CIOBuffer;

      explicit Allocator(
         DWORD bufferSize,
         DWORD maxFreeBuffers);

      virtual ~Allocator();

      CIOBuffer *Allocate();

      DWORD GetBufferSize() const;

   protected :

      void Flush();

   private :

      void Release(
         CIOBuffer *pBuffer);

      virtual void OnBufferCreated() {}
      virtual void OnBufferAllocated() {}
      virtual void OnBufferReleased() {}
      virtual void OnBufferDestroyed() {}

      void DestroyBuffer(
         CIOBuffer *pBuffer);

      const DWORD m_bufferSize;

      typedef TNodeList<CIOBuffer> BufferList;
      
      BufferList m_freeList;
      BufferList m_activeList;
      
      const DWORD m_maxFreeBuffers;

      CCriticalSection m_criticalSection;

      // No copies do not implement
      Allocator(const Allocator &rhs);
      Allocator &operator=(const Allocator &rhs);
};

///////////////////////////////////////////////////////////////////////////////
// CIOBuffer::InOrderBufferList
///////////////////////////////////////////////////////////////////////////////

class  GAMEKNIFENETWORK_API CIOBuffer::InOrderBufferList
{
   public:

      explicit InOrderBufferList(
         CCriticalSection &lock);

      void AddBuffer(
         CIOBuffer *pBuffer);

      void ProcessBuffer();

      CIOBuffer *ProcessAndGetNext();

      CIOBuffer *GetNext();

      CIOBuffer *GetNext(
         CIOBuffer *pBuffer);

      void Reset();

      bool Empty() const;

   private :

      DWORD m_next;
   
      //typedef std::map<DWORD, CIOBuffer *> BufferSequence;
	  
	  typedef CMap<DWORD,DWORD,CIOBuffer*,CIOBuffer*&> BufferSequence;

      BufferSequence m_list;

      CCriticalSection &m_criticalSection;
};

#endif // UNICOMM_TOOLS_WIN32_IO_BUFFER_INCLUDED__

///////////////////////////////////////////////////////////////////////////////
// End of file
///////////////////////////////////////////////////////////////////////////////
