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

#ifndef UNICOMM_TOOLS_OPAQUE_USER_DATA_INCLUDED__
#define UNICOMM_TOOLS_OPAQUE_USER_DATA_INCLUDED__

///////////////////////////////////////////////////////////////////////////////
// COpaqueUserData
///////////////////////////////////////////////////////////////////////////////

//关闭警告 warning C4311: “类型转换” : 从“void *”到“unsigned long”的指针截断
#pragma warning(disable: 4311)   
//关闭警告 warning C4312: “类型转换” : 从“unsigned long”转换到更大的“void *”
#pragma warning(disable: 4312)   

class GAMEKNIFENETWORK_API COpaqueUserData 
{
   public:

      void *GetUserPtr() const
      {
         return InterlockedExchangePointer(&(const_cast<void*>(m_pUserData)), m_pUserData);
      }
      
      void SetUserPtr(void *pData)
      {
         InterlockedExchangePointer(&m_pUserData, pData);
      }

      unsigned long GetUserData() const
      {
         return reinterpret_cast<unsigned long>(GetUserPtr());
      }

      void SetUserData(unsigned long data)
      {
         SetUserPtr(reinterpret_cast<void*>(data));
      }

   protected :
      
      COpaqueUserData()
         : m_pUserData(0)
      {
      }

      ~COpaqueUserData()
      {
         m_pUserData = 0;
      }

   private :

      void *m_pUserData;

      // No copies do not implement
      COpaqueUserData(const COpaqueUserData &rhs);
      COpaqueUserData &operator=(const COpaqueUserData &rhs);
};


#endif // UNICOMM_TOOLS_OPAQUE_USER_DATA_INCLUDED__

///////////////////////////////////////////////////////////////////////////////
// End of file
///////////////////////////////////////////////////////////////////////////////

