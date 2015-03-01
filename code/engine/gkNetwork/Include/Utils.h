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

#ifndef UNICOMM_TOOLS_WIN32_UTILS__
#define UNICOMM_TOOLS_WIN32_UTILS__

#include "NetWorkPrerequisites.h"
#include <atlbase.h>       // USES_CONVERSION

#pragma warning(default: 4201)


///////////////////////////////////////////////////////////////////////////////
// Debugging defines...
///////////////////////////////////////////////////////////////////////////////

#ifndef DEBUG_ONLY
#ifdef _DEBUG
#define DEBUG_ONLY(x)   x
#else
#define DEBUG_ONLY(x)
#endif
#endif

///////////////////////////////////////////////////////////////////////////////
// Functions defined in this file...
///////////////////////////////////////////////////////////////////////////////


template <class T>
 CString __declspec(dllexport) ToString(T num)
{
   CString strNum = _T("");
   strNum.Format(_T("%d"),num);

   return strNum;

}


 CString __declspec(dllexport) GetLastErrorMessage(
   DWORD last_error);

 inline bool __declspec(dllexport) BOOL_to_bool(const BOOL bResult)
{
   return (TRUE == bResult);
}

void GAMEKNIFENETWORK_API Output( const CString &message);

#endif // UNICOMM_TOOLS_WIN32_UTILS__

///////////////////////////////////////////////////////////////////////////////
// End of file
///////////////////////////////////////////////////////////////////////////////

