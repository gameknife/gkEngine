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
#include "NetWorkPrerequisites.h"

#ifndef UNICOMM_TOOLS_NODE_LIST_INCLUDED__
#define UNICOMM_TOOLS_NODE_LIST_INCLUDED__

#include <wtypes.h>

///////////////////////////////////////////////////////////////////////////////
// CNodeList
///////////////////////////////////////////////////////////////////////////////

class GAMEKNIFENETWORK_API CNodeList
{
   public :

      class  GAMEKNIFENETWORK_API Node
      {
         public :

            Node *Next() const;

            void Next(Node *pNext);

            void AddToList(CNodeList *pList);

            void RemoveFromList();

         protected :

            Node();
            ~Node();

         private :

            friend class CNodeList;

            void Unlink();

            Node *m_pNext;
            Node *m_pPrev;

            CNodeList *m_pList;
      };

      CNodeList();

      void PushNode(Node *pNode);

      Node *PopNode();

      Node *Head() const;

      DWORD Count() const;

      bool Empty() const;

   private :

      friend void Node::RemoveFromList();

      void RemoveNode(Node *pNode);

      Node *m_pHead; 

      DWORD m_numNodes;
};

///////////////////////////////////////////////////////////////////////////////
// TNodeList
///////////////////////////////////////////////////////////////////////////////

template <class T> class TNodeList : public CNodeList
{
   public :
         
      T *PopNode();
   
      T *Head() const;

      static T *Next(const T *pNode);
};

template <class T>
 T *TNodeList<T>::PopNode()
{
   return static_cast<T*>(CNodeList::PopNode());
}

template <class T>
T *TNodeList<T>::Head() const
{
   return static_cast<T*>(CNodeList::Head());
}

template <class T>
T *TNodeList<T>::Next(const T *pNode)
{
   return static_cast<T*>(pNode->Next());
}

#endif //UNICOMM_TOOLS_NODE_LIST_INCLUDED__

///////////////////////////////////////////////////////////////////////////////
// End of file
///////////////////////////////////////////////////////////////////////////////
