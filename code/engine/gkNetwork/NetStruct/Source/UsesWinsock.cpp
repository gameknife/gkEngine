
#include "NetWorkStableHeader.h"

#include "UsesWinsock.h"
#include "Win32Exception.h"


///////////////////////////////////////////////////////////////////////////////
// CUsesWinsock
///////////////////////////////////////////////////////////////////////////////

CUsesWinsock::CUsesWinsock()
{
   WORD wVersionRequested = 0x202;

   //if (0 != ::WSAStartup(wVersionRequested, &m_data))
   //{
   //   throw CWin32Exception(_T("CUsesWinsock::CUsesWinsock()"), ::WSAGetLastError());
   //}
   	if (!AfxSocketInit())
	{
		throw CWin32Exception(_T("CUsesWinsock::CUsesWinsock()"), ::WSAGetLastError());
	}
}
      
CUsesWinsock::~CUsesWinsock()
{
   //WSACleanup();
}

///////////////////////////////////////////////////////////////////////////////
// End of file...
///////////////////////////////////////////////////////////////////////////////

