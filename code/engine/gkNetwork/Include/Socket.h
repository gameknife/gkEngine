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

#ifndef UNICOMM_TOOLS_WIN32_SOCKET_INCLUDED__
#define UNICOMM_TOOLS_WIN32_SOCKET_INCLUDED__

#include "Win32Exception.h"


class GAMEKNIFENETWORK_API CIOSocket  
{
   public:

      class InternetAddress;
      class Exception;

      CIOSocket();

      explicit CIOSocket(
         SOCKET theSocket);
      
      ~CIOSocket();

      void Attach(
         SOCKET theSocket);

      SOCKET Detatch();

      void Close();

      void AbortiveClose();

      void Shutdown(
         int how);

      void Listen(
         int backlog);

      void Bind(
         const SOCKADDR_IN &address);

      void Bind(
         const struct sockaddr &address,   
         int addressLength);

   private :

      SOCKET m_socket;

      // No copies do not implement
      CIOSocket(const CIOSocket &rhs);
      CIOSocket &operator=(const CIOSocket &rhs);
};

///////////////////////////////////////////////////////////////////////////////
// CIOSocket::InternetAddress
///////////////////////////////////////////////////////////////////////////////

class  GAMEKNIFENETWORK_API CIOSocket::InternetAddress : public SOCKADDR_IN
{
   public :
      
      InternetAddress(
         unsigned long address,
         unsigned short port);
};

///////////////////////////////////////////////////////////////////////////////
// CIOSocket::Exception
///////////////////////////////////////////////////////////////////////////////

class CIOSocket::Exception : public CWin32Exception
{
   friend class CIOSocket;

   private :

      Exception(
         const CStringW &where, 
         DWORD error);
};


#endif // UNICOMM_TOOLS_WIN32_SOCKET_INCLUDED__


///////////////////////////////////////////////////////////////////////////////
// End of file
///////////////////////////////////////////////////////////////////////////////
