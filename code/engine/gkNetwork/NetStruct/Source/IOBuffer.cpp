#include "NetWorkStableHeader.h"

#include "IOBuffer.h"

#include "Exception.h"
#include "Utils.h"


///////////////////////////////////////////////////////////////////////////////
// CIOBuffer
///////////////////////////////////////////////////////////////////////////////

CIOBuffer::CIOBuffer(
   Allocator &allocator,
   DWORD size)
   :  m_operation(0),
      m_sequenceNumber(0),
      m_allocator(allocator),
      m_ref(1),
      m_size(size), 
      m_used(0)
{
   memset(this, 0, sizeof(OVERLAPPED));

   Empty();
}

WSABUF *CIOBuffer::GetWSABUF() const
{
   return const_cast<WSABUF*>(&m_wsabuf);
}

DWORD CIOBuffer::GetUsed() const
{
   return m_used;
}

DWORD CIOBuffer::GetSize() const
{
   return m_size;
}

const BYTE *CIOBuffer::GetBuffer() const
{
   return &m_buffer[0];
}

void CIOBuffer::Empty()
{
   m_wsabuf.buf = reinterpret_cast<char*>(m_buffer);
   m_wsabuf.len = m_size;

   m_used = 0;
}

void *CIOBuffer::operator new(size_t objectSize, size_t bufferSize)
{
   void *pMem = new char[objectSize + bufferSize];
   return pMem;
}

void CIOBuffer::operator delete(void *pObject, size_t bufferSize)
{
   delete [] pObject;
}  

void CIOBuffer::Use(
   DWORD dataUsed)
{
   m_used += dataUsed;
}

CIOBuffer *CIOBuffer::SplitBuffer(
   DWORD bytesToRemove)
{
   CIOBuffer *pNewBuffer = m_allocator.Allocate();

   pNewBuffer->AddData(m_buffer, bytesToRemove);

   m_used -= bytesToRemove;

   memmove(m_buffer, m_buffer + bytesToRemove, m_used);

   return pNewBuffer;
}

CIOBuffer *CIOBuffer::AllocateNewBuffer() const
{
   return m_allocator.Allocate();
}

void CIOBuffer::ConsumeAndRemove(
   DWORD bytesToRemove)
{
   m_used -= bytesToRemove;

   memmove(m_buffer, m_buffer + bytesToRemove, m_used);
}

void CIOBuffer::SetupZeroByteRead()
{
   m_wsabuf.buf = reinterpret_cast<char*>(m_buffer);
   m_wsabuf.len = 0; 
}

void CIOBuffer::SetupRead()
{
   if (m_used == 0)
   {
      m_wsabuf.buf = reinterpret_cast<char*>(m_buffer);
      m_wsabuf.len = m_size; 
   }
   else
   {
      m_wsabuf.buf = reinterpret_cast<char*>(m_buffer) + m_used;
      m_wsabuf.len = m_size - m_used; 
   }
}

void CIOBuffer::SetupWrite()
{
   m_wsabuf.buf = reinterpret_cast<char*>(m_buffer);
   m_wsabuf.len = m_used;

   m_used = 0;
}

void CIOBuffer::AddData(
   const char * const pData,
   DWORD dataLength)
{
   if (dataLength > m_size - m_used)
   {
      throw CCommException(_T("CIOBuffer::AddData"), _T("Not enough space in buffer"));
   }

   memcpy(m_buffer + m_used, pData, dataLength);

   m_used += dataLength;
}

void CIOBuffer::AddData(
   const BYTE * const pData,
   DWORD dataLength)
{
   AddData(reinterpret_cast<const char*>(pData), dataLength);
}

void CIOBuffer::AddData(
   BYTE data)
{
   AddData(&data, 1);
}

void CIOBuffer::AddRef()
{
   ::InterlockedIncrement(&m_ref);
}

void CIOBuffer::Release()
{
   if (m_ref == 0)
   {
      // Error! double release
      throw CCommException(_T("CIOBuffer::Release()"), _T("m_ref is already 0"));
   }

   if (0 == ::InterlockedDecrement(&m_ref))
   {
      m_sequenceNumber = 0;
      m_operation = 0;
      m_used = 0;

      m_allocator.Release(this);
   }
}

DWORD CIOBuffer::GetOperation() const
{
   return m_operation;
}
      
void CIOBuffer::SetOperation(
   DWORD operation)
{
   m_operation = operation;
}

DWORD CIOBuffer::GetSequenceNumber() const
{
   return m_sequenceNumber;
}
      
void CIOBuffer::SetSequenceNumber(
   DWORD sequenceNumber)
{
   m_sequenceNumber = sequenceNumber;
}

///////////////////////////////////////////////////////////////////////////////
// CIOBuffer::Allocator
///////////////////////////////////////////////////////////////////////////////

CIOBuffer::Allocator::Allocator(
   DWORD bufferSize,
   DWORD maxFreeBuffers)
   :  m_bufferSize(bufferSize),
      m_maxFreeBuffers(maxFreeBuffers)
{
   // TODO share this code with the socket pool
}

CIOBuffer::Allocator::~Allocator()
{
   try
   {
      Flush();
   }
   catch(...)
   {
   }
}

CIOBuffer *CIOBuffer::Allocator::Allocate()
{
   CCriticalSection::Owner lock(m_criticalSection);

   CIOBuffer *pBuffer = 0;

   if (!m_freeList.Empty())
   {
      pBuffer = m_freeList.PopNode();

      pBuffer->AddRef();
   }
   else
   {

	  pBuffer = new(m_bufferSize)CIOBuffer(*this, m_bufferSize);
      if (!pBuffer)
      {
         throw CCommException(_T("CIOBuffer::Allocator::Allocate()"),_T("Out of memory"));
      }

      OnBufferCreated();
   }

   m_activeList.PushNode(pBuffer);

   OnBufferAllocated();

   return pBuffer;
}

void CIOBuffer::Allocator::Release(
   CIOBuffer *pBuffer)
{
   if (!pBuffer)
   {
      throw CCommException(_T("CIOBuffer::Allocator::Release()"), _T("pBuffer is null"));
   }

   CCriticalSection::Owner lock(m_criticalSection);

   OnBufferReleased();

   // unlink from the in use list
   pBuffer->RemoveFromList();

   if (m_maxFreeBuffers == 0 || 
       m_freeList.Count() < m_maxFreeBuffers)
   {
      pBuffer->Empty();           
      
      // add to the free list
      m_freeList.PushNode(pBuffer);
   }
   else
   {
      DestroyBuffer(pBuffer);
   }
}

void CIOBuffer::Allocator::Flush()
{
   CCriticalSection::Owner lock(m_criticalSection);

   while (!m_activeList.Empty())
   {
      OnBufferReleased();
      
      DestroyBuffer(m_activeList.PopNode());
   }

   while (!m_freeList.Empty())
   {
      DestroyBuffer(m_freeList.PopNode());
   }
}

void CIOBuffer::Allocator::DestroyBuffer(CIOBuffer *pBuffer)
{
   delete pBuffer;

   OnBufferDestroyed();
}

DWORD CIOBuffer::Allocator::GetBufferSize() const
{
   return m_bufferSize;
}

///////////////////////////////////////////////////////////////////////////////
// CIOBuffer::InOrderBufferList
///////////////////////////////////////////////////////////////////////////////

CIOBuffer::InOrderBufferList::InOrderBufferList(
   CCriticalSection &criticalSection)
   :  m_next(0),
      m_criticalSection(criticalSection)
{
}

void CIOBuffer::InOrderBufferList::AddBuffer(
   CIOBuffer *pBuffer)
{
   CCriticalSection::Owner lock(m_criticalSection);

   m_list.SetAt(pBuffer->GetSequenceNumber(), pBuffer);

}

CIOBuffer *CIOBuffer::InOrderBufferList::ProcessAndGetNext()
{
   CCriticalSection::Owner lock(m_criticalSection);

   m_next++;

   CIOBuffer *pNext = 0;
   
   //找到元素，则返回元素指针，并从列表中删除该元素位置
   //注意使用时要将元素删除，否则产生泄露
   POSITION pos = m_list.GetStartPosition();
   DWORD nKeyFirst;
   if (pos != NULL)
   {
	   m_list.GetNextAssoc(pos,nKeyFirst,pNext);
	   if (nKeyFirst==m_next)
	   {
		   m_list.RemoveKey(nKeyFirst);
	   }
	   else
	   {
		   
//      DEBUG_ONLY(Output(ToString(this) + _T(" PAGN Got buffer  : ") + ToString(it->first) + _T("Want buffer : ") + ToString(m_next)));
	   }
   }


   return pNext;
}

CIOBuffer *CIOBuffer::InOrderBufferList::GetNext()
{
   CCriticalSection::Owner lock(m_criticalSection);

   CIOBuffer *pNext = 0;

   POSITION pos = m_list.GetStartPosition();
   DWORD nKeyFirst;
   if (pos != NULL)
   {
	   m_list.GetNextAssoc(pos,nKeyFirst,pNext);
	   if (nKeyFirst==m_next)
	   {
		   m_list.RemoveKey(nKeyFirst);
	   }
	   else
	   {
//      DEBUG_ONLY(Output(ToString(this) + _T(" PAGN Got buffer  : ") + ToString(it->first) + _T("Want buffer : ") + ToString(m_next)));
	   }
   }
   return pNext;
}


CIOBuffer *CIOBuffer::InOrderBufferList::GetNext(
   CIOBuffer *pBuffer)
{
   CCriticalSection::Owner lock(m_criticalSection);

   if (m_next == pBuffer->GetSequenceNumber())
   {
      return pBuffer;
   }

   m_list.SetAt(pBuffer->GetSequenceNumber(), pBuffer);

   CIOBuffer *pNext = 0;


   POSITION pos = m_list.GetStartPosition();
   DWORD nKeyFirst;
   if (pos != NULL)
   {
	   m_list.GetNextAssoc(pos,nKeyFirst,pNext);
	   if (nKeyFirst==m_next)
	   {
		   m_list.RemoveKey(nKeyFirst);
	   }
	   else
	   {
//      DEBUG_ONLY(Output(ToString(this) + _T(" PAGN Got buffer  : ") + ToString(it->first) + _T("Want buffer : ") + ToString(m_next)));
	   }
   }
   return pNext;
}

void CIOBuffer::InOrderBufferList::ProcessBuffer()
{
   CCriticalSection::Owner lock(m_criticalSection);
   //DEBUG_ONLY(Output(ToString(this) + _T(" Processed : ") + ToString(m_next)));
   m_next++;
}

void CIOBuffer::InOrderBufferList::Reset()
{
   m_next = 0;
   if (m_list.IsEmpty()==FALSE)
   {
      DEBUG_ONLY(Output(_T("List not empty when reset !")));
   }
}

bool CIOBuffer::InOrderBufferList::Empty() const
{
	return (m_list.IsEmpty() == TRUE);
}


///////////////////////////////////////////////////////////////////////////////
// End of file...
///////////////////////////////////////////////////////////////////////////////
