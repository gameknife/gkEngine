
#include "NetWorkStableHeader.h"

#include "Socket.h"

///////////////////////////////////////////////////////////////////////////////
// CIOSocket
///////////////////////////////////////////////////////////////////////////////

CIOSocket::CIOSocket()
   :  m_socket(INVALID_SOCKET)
{
}

CIOSocket::CIOSocket(
   SOCKET theSocket)
   :  m_socket(theSocket)
{
   if (INVALID_SOCKET == m_socket)
   {
      throw Exception(_T("CIOSocket::CIOSocket()"),  WSAENOTSOCK);
   }
}
      
CIOSocket::~CIOSocket()
{
   if (INVALID_SOCKET != m_socket)
   {
      try
      {
         AbortiveClose();
      }
      catch(...)
      {
      }
   }
}

void CIOSocket::Attach(
   SOCKET theSocket)
{
   AbortiveClose();

   m_socket = theSocket;
}

SOCKET CIOSocket::Detatch()
{
   SOCKET theSocket = m_socket;

   m_socket = INVALID_SOCKET;

   return theSocket;
}

void CIOSocket::Close()
{
   if (0 != ::closesocket(m_socket))
   {
      throw Exception(_T("CIOSocket::Close()"), ::WSAGetLastError());
   }
}

void CIOSocket::AbortiveClose()
{
   LINGER lingerStruct;

   lingerStruct.l_onoff = 1;
   lingerStruct.l_linger = 0;

   if (SOCKET_ERROR == ::setsockopt(m_socket, SOL_SOCKET, SO_LINGER, (char *)&lingerStruct, sizeof(lingerStruct)))
   {
      throw Exception(_T("CIOSocket::AbortiveClose()"), ::WSAGetLastError());
   }
   
   Close();
}

void CIOSocket::Shutdown(
   int how)
{
   if (0 != ::shutdown(m_socket, how))
   {
      throw Exception(_T(" CIOSocket::Shutdown()"), ::WSAGetLastError());
   }
}

void CIOSocket::Listen(
   int backlog)
{
   if (SOCKET_ERROR == ::listen(m_socket, backlog))
   {
      throw Exception(_T("CIOSocket::Listen()"), ::WSAGetLastError());
   }
}

void CIOSocket::Bind(
   const SOCKADDR_IN &address)
{
   if (SOCKET_ERROR == ::bind(m_socket, reinterpret_cast<struct sockaddr *>(const_cast<SOCKADDR_IN*>(&address)), sizeof(SOCKADDR_IN)))
   {
      throw Exception(_T("CIOSocket::Bind()"), ::WSAGetLastError());
   }
}

void CIOSocket::Bind(
   const struct sockaddr &address,   
   int addressLength)
{
   //lint -e{713} Loss of precision (arg. no. 3) (unsigned int to int)
   if (SOCKET_ERROR == ::bind(m_socket, const_cast<struct sockaddr *>(&address), addressLength))
   {
      throw Exception(_T("CIOSocket::Bind()"), ::WSAGetLastError());
   }
}

///////////////////////////////////////////////////////////////////////////////
// CIOSocket::InternetAddress
///////////////////////////////////////////////////////////////////////////////

CIOSocket::InternetAddress::InternetAddress(
   unsigned long address,
   unsigned short port)
{
   sin_family = AF_INET;
   sin_port = htons(port);
   sin_addr.s_addr = htonl(address);  
   //lint -e{1401} member 'sockaddr_in::sin_zero not initialised
}

///////////////////////////////////////////////////////////////////////////////
// CIOSocket::Exception
///////////////////////////////////////////////////////////////////////////////

CIOSocket::Exception::Exception(
   const CStringW &where, 
   DWORD error)
   : CWin32Exception(where, error)
{
}

///////////////////////////////////////////////////////////////////////////////
// End of file
///////////////////////////////////////////////////////////////////////////////

