#include "NetWorkStableHeader.h"

#include "NodeList.h"

///////////////////////////////////////////////////////////////////////////////
// CNodeList
///////////////////////////////////////////////////////////////////////////////

CNodeList::CNodeList() 
   :  m_pHead(0), 
      m_numNodes(0) 
{
}

void CNodeList::PushNode(
   Node *pNode)
{
   pNode->AddToList(this);

   pNode->Next(m_pHead);

   m_pHead = pNode;

   ++m_numNodes;
}

CNodeList::Node *CNodeList::PopNode()
{
   Node *pNode = m_pHead;

   if (pNode)
   {
      RemoveNode(pNode);
   }

   return pNode;
}

CNodeList::Node *CNodeList::Head() const
{
   return m_pHead;
}


DWORD CNodeList::Count() const
{
   return m_numNodes;
}

bool CNodeList::Empty() const
{
   return (0 == m_numNodes);
}

void CNodeList::RemoveNode(Node *pNode)
{
   if (pNode == m_pHead)
   {
      //lint -e{613} Possible use of null pointer 
      m_pHead = pNode->Next();
   }

   //lint -e{613} Possible use of null pointer 
   pNode->Unlink();

   --m_numNodes;
}

///////////////////////////////////////////////////////////////////////////////
// CNodeList::Node
///////////////////////////////////////////////////////////////////////////////

CNodeList::Node::Node() 
   :  m_pNext(0), 
      m_pPrev(0), 
      m_pList(0) 
{
}

CNodeList::Node::~Node() 
{
   try
   {
      RemoveFromList();   
   }
   catch(...)
   {
   }

   m_pNext = 0;
   m_pPrev = 0;
   m_pList = 0;
}

CNodeList::Node *CNodeList::Node::Next() const
{
   return m_pNext;
}

void CNodeList::Node::Next(Node *pNext)
{
   m_pNext = pNext;

   if (pNext)
   {
      pNext->m_pPrev = this;
   }
}

void CNodeList::Node::AddToList(CNodeList *pList)
{
   m_pList = pList;
}

void CNodeList::Node::RemoveFromList()
{
   if (m_pList)
   {
      m_pList->RemoveNode(this);
   }
}

void CNodeList::Node::Unlink()
{
   if (m_pPrev)
   {
      m_pPrev->m_pNext = m_pNext;
   }

   if (m_pNext)
   {
      m_pNext->m_pPrev = m_pPrev;
   }
   
   m_pNext = 0;
   m_pPrev = 0;

   m_pList = 0;
}

\
///////////////////////////////////////////////////////////////////////////////
// End of file
///////////////////////////////////////////////////////////////////////////////
