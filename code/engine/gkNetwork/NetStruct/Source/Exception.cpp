#include "NetWorkStableHeader.h"

#include "Exception.h"

///////////////////////////////////////////////////////////////////////////////
// CCommException
///////////////////////////////////////////////////////////////////////////////

CCommException::CCommException(
   const CStringW &where, 
   const CStringW &message)
   :  m_where(where), 
      m_message(message)
{
}

CStringW CCommException::GetWhere() const 
{ 
   return m_where; 
}

CStringW CCommException::GetMessage() const 
{ 
   return m_message; 
}

void CCommException::MessageBox(HWND hWnd /* = NULL */) const 
{ 
   //lint -e{1933} call to unqualified virtual function

	::MessageBox(hWnd, GetMessage(), GetWhere(), MB_ICONSTOP); 
}


///////////////////////////////////////////////////////////////////////////////
// End of file...
///////////////////////////////////////////////////////////////////////////////
