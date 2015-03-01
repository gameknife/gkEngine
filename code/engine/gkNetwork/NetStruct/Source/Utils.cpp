
#include "NetWorkStableHeader.h"

#include "Utils.h"
#include "Exception.h"

CString GetLastErrorMessage(DWORD last_error)
{
   static TCHAR errmsg[512];

   if (!FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 
      0,
      last_error,
      0,
      errmsg, 
      511,
      NULL))
   {
      return (GetLastErrorMessage(GetLastError()));  
   }
  
   return errmsg;
}

void Output(const CString &message)
{
    DWORD byteswritten;
	CString szout;
	szout.Format(_T("%s \r\n"),message);

	WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE), szout, szout.GetLength(), &byteswritten, NULL); 
	//TRACE0( szout );
}

///////////////////////////////////////////////////////////////////////////////
// End of file...
///////////////////////////////////////////////////////////////////////////////
