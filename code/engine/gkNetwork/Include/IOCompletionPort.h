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

#ifndef UNICOMM_TOOLS_WIN32_IO_COMPLETION_PORT_INCLUDED__
#define UNICOMM_TOOLS_WIN32_IO_COMPLETION_PORT_INCLUDED__


#include "NetWorkPrerequisites.h"
//#ifndef _WINDOWS_
//#define WIN32_LEAN_AND_MEAN
//#include <windows.h>
//#undef WIN32_LEAN_AND_MEAN
//#endif

///////////////////////////////////////////////////////////////////////////////
// CIOCompletionPort
///////////////////////////////////////////////////////////////////////////////

class GAMEKNIFENETWORK_API CIOCompletionPort
{
   public:
   
      CIOCompletionPort(
         DWORD maxConcurrency);

      ~CIOCompletionPort();

   void AssociateDevice(
      HANDLE hDevice, 
      ULONG_PTR completionKey);

   void PostStatus(
      ULONG_PTR completionKey, 
      DWORD dwNumBytes = 0, 
      OVERLAPPED *pOverlapped = 0);

   void GetStatus(
      ULONG_PTR *pCompletionKey, 
      PDWORD pdwNumBytes,
      OVERLAPPED **ppOverlapped);

   bool GetStatus(
      ULONG_PTR *pCompletionKey, 
      PDWORD pdwNumBytes,
      OVERLAPPED **ppOverlapped, 
      DWORD dwMilliseconds);

   private:
      
      HANDLE m_iocp;

      // No copies do not implement
      CIOCompletionPort(const CIOCompletionPort &rhs);
      CIOCompletionPort &operator=(const CIOCompletionPort &rhs);
};



#endif //UNICOMM_TOOLS_WIN32_IO_COMPLETION_PORT_INCLUDED__

///////////////////////////////////////////////////////////////////////////////
// End of file
///////////////////////////////////////////////////////////////////////////////
