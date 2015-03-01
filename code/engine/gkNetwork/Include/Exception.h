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
#ifndef UNICOMM_TOOLS_WIN32_EXCEPTION__
#define UNICOMM_TOOLS_WIN32_EXCEPTION__
#include "NetworkPrerequisites.h"
//#pragma warning(disable: 4201)   // nameless struct/union
//#pragma warning(disable: 4201)

///////////////////////////////////////////////////////////////////////////////
// CCommException
///////////////////////////////////////////////////////////////////////////////

class GAMEKNIFENETWORK_API CCommException
{
   public : 

      CCommException(
         const CStringW &where, 
         const CStringW &message);

      virtual ~CCommException() {}

      virtual CStringW GetWhere() const;

      virtual CStringW GetMessage() const; 

      void MessageBox(
         HWND hWnd = NULL) const; 

   protected :
      
      const CStringW m_where;
      const CStringW m_message;
};


#endif // UNICOMM_TOOLS_WIN32_EXCEPTION__

///////////////////////////////////////////////////////////////////////////////
// End of file
///////////////////////////////////////////////////////////////////////////////
