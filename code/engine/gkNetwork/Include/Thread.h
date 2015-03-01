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
#include "NetworkPrerequisites.h"

#ifndef UNICOMM_TOOLS_WIN32_THREAD_INCLUDED__
#define UNICOMM_TOOLS_WIN32_THREAD_INCLUDED__


///////////////////////////////////////////////////////////////////////////////
// CThread
///////////////////////////////////////////////////////////////////////////////

class GAMEKNIFENETWORK_API IThread 
{
   public :
   
      IThread();
      
      virtual ~IThread();

      HANDLE GetHandle() const;

      void Wait() const;

      bool Wait(DWORD timeoutMillis) const;

      void Start();

      void Terminate(
         DWORD exitCode = 0);

   private :

      virtual int Run() = 0;

      static unsigned int __stdcall ThreadFunction(void *pV);

      HANDLE m_hThread;

      // No copies do not implement
      IThread(const IThread &rhs);
      IThread &operator=(const IThread &rhs);
};


#endif // UNICOMM_TOOLS_WIN32_THREAD_INCLUDED__

///////////////////////////////////////////////////////////////////////////////
// End of file
///////////////////////////////////////////////////////////////////////////////

