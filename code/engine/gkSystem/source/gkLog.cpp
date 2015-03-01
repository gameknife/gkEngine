#include "gkSystemStableHeader.h"
#include "gkLog.h"
#include <iostream>

#ifdef OS_ANDROID
#include <android/log.h>

#define  LOG_TAG    "gkENGINE"
#define  LOGW(...)  __android_log_print(ANDROID_LOG_WARN,LOG_TAG,__VA_ARGS__)
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

#elif defined( OS_IOS )

#include "gkIOSLog.h"

#endif


#include "IThread.h"

#define MULTIPLE_OF_PATH 5


//-----------------------------------------------------------------------
gkLog::gkLog( ISystem* system )
{
	memset(m_szFilename,0, MAX_PATH);	
	//memset(m_szTemp,0,MAX_TEMP_LENGTH_SIZE);
	m_pSystem = system;
	m_fLastLoadingUpdateTime = -1.f;	// for streaming engine update

	m_pLogFile = NULL;

	m_wstrFilename.clear();
	m_callbacks.clear();

	m_iLastHistoryItem = 0;
	memset(m_history, 0, sizeof(m_history));

#ifdef OS_WIN32
	AllocConsole();
	freopen("CONOUT$","w+t",stdout);  
	freopen("CONIN$","r+t",stdin);

	::system("chcp 65001");
#endif
}
//-----------------------------------------------------------------------
gkLog::~gkLog( void )
{

#ifdef OS_WIN32
	fclose(stdout);
	fclose(stdin);
	FreeConsole();
#endif
}
//-----------------------------------------------------------------------
bool gkLog::SetFileName( const gkStdString& command /*= NULL*/ )
{
	// TODO [8/18/2011 Kaiming-Desktop]
	m_wstrFilename = command;
	return true;
}
//-----------------------------------------------------------------------
const gkStdString& gkLog::GetFileName()
{
	// TODO [8/18/2011 Kaiming-Desktop]
	return m_wstrFilename;
}
//-----------------------------------------------------------------------
void gkLog::SetVerbosity( int verbosity )
{

}
//-----------------------------------------------------------------------
int gkLog::GetVerbosityLevel()
{
	return 0;
}
//-----------------------------------------------------------------------
void gkLog::AddCallback( ILogCallback *pCallback )
{
	m_callbacks.push_back(pCallback);
}
//-----------------------------------------------------------------------
void gkLog::RemoveCallback( ILogCallback *pCallback )
{
	m_callbacks.remove(pCallback);
}
//-----------------------------------------------------------------------
void gkLog::LogStringToFile( const TCHAR* szString,bool bAdd,bool bError/*=false */ )
{

}
//-----------------------------------------------------------------------
void gkLog::LogStringToConsole( const TCHAR* szString,bool bAdd/*=false */ )
{
	//gkAutoLock<gkCritcalSectionLock> lock(eLID_log);

	if (!szString)
		return;

	gkStdString tempString;
	tempString = szString;

	// Call callback function.
	if (!m_callbacks.empty())
	{
		for (Callbacks::iterator it = m_callbacks.begin(); it != m_callbacks.end(); ++it)
		{
			(*it)->OnWriteToConsole(tempString.c_str(),!bAdd);
		}
	}

	// internal console
	IConsoleVariablesManager *console = gEnv->pCVManager;
	if (!console)
		return;

// 	if (tempString.length() > MAX_PATH)
// 		tempString.erase( MAX_PATH );
	// add \n at end.
	if (tempString.length() > 0 && tempString[tempString.length()-1] != '\n')
	{
		tempString += '\n';
	}

	if (bAdd)
		console->PrintLinePlus(tempString.c_str());	
	else
		console->PrintLine(tempString.c_str());

	// outside console
	// Handle the first

#ifdef OS_WIN32
	static HANDLE consolehwnd;
	consolehwnd = GetStdHandle(STD_OUTPUT_HANDLE);

	const TCHAR* cstr = tempString.c_str();
	if ( cstr[0] == _T('$') )
	{
		switch(cstr[1])
		{
		case _T('5'):
			SetConsoleTextAttribute(consolehwnd, FOREGROUND_BLUE | FOREGROUND_GREEN );
			break;
		case _T('6'):
			SetConsoleTextAttribute(consolehwnd, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY );
			break;
		case _T('4'):
			SetConsoleTextAttribute(consolehwnd, FOREGROUND_RED | FOREGROUND_INTENSITY );
			break;
		}

		tempString = tempString.substr( 2 );
	}

#ifdef UNICODE
	std::wcout<<tempString;
#else
	std::cout<<tempString;
#endif
	SetConsoleTextAttribute(consolehwnd, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE );
#endif
}
//-----------------------------------------------------------------------
FILE* gkLog::OpenLogFile( const TCHAR *filename,const TCHAR *mode )
{
	return NULL;
}
//-----------------------------------------------------------------------
void gkLog::CloseLogFile( bool force/*=false */ )
{

}
//-----------------------------------------------------------------------
void gkLog::FormatMessage( const TCHAR *szCommand, ... )
{

}
//-----------------------------------------------------------------------
void gkLog::LogV( const ELogType ineType, const TCHAR* szFormat, va_list args )
{
	if (!szFormat)
		return;

	bool bfile = false, bconsole = false;
	const TCHAR* szCommand = szFormat;

	uint8 DefaultVerbosity=0;	// 0 == Always log (except for special -1 verbosity overrides)

	switch(ineType)
	{
	case eAlways:						DefaultVerbosity=0; break;
	case eWarningAlways:		DefaultVerbosity=0; break;
	case eErrorAlways:			DefaultVerbosity=0; break;
	case eInput:						DefaultVerbosity=0; break;
	case eInputResponse:		DefaultVerbosity=0; break;
	case eError:						DefaultVerbosity=1; break;
	case eWarning:					DefaultVerbosity=2; break;
	case eMessage:					DefaultVerbosity=3; break;
	case eComment:					DefaultVerbosity=4; break;

	default: assert(0);
	}


	bool bError = false;

	gkStdString tempString;

	TCHAR szBuffer[MAX_PATH * MULTIPLE_OF_PATH + 32];
	memset( szBuffer, 0, MAX_PATH * MULTIPLE_OF_PATH + 32);

 	size_t prefixSize = 0;
 	switch(ineType)
 	{
 	case eWarning:
 	case eWarningAlways:
		{
 		bError = true;
 
		tempString = _T("$6[Warning] ");

 		break;
		}
 
 	case eError:
 	case eErrorAlways:
		{
 		bError = true;
 
		tempString = _T("$4[Error] ");
 
 		break;
		}

	case eInput:
	case eInputResponse:
		{
			bError = true;

			tempString = _T("$5[Execute] ");

			break;
		}
 
 	default:
 
 
 
 
 
 		break;
 	}

 	int bufferlen = sizeof(szBuffer)/* - prefixSize*/;
 	if ( bufferlen > 0 )
 	{
 		int count = _vstprintf( szBuffer, szCommand, args );
 		if ( count == -1 || count >=bufferlen )
 			szBuffer[_tcslen(szBuffer)-1] = '\0';
 	}

	tempString += szBuffer;
	
	// finally log to system
#ifdef OS_WIN32
	// 	if (bfile)
	// 		LogStringToFile( szAfterColour,false,bError );
	// 	if (bconsole)
	// 	{
		LogStringToConsole( tempString.c_str() );
	//	}
#elif defined( OS_ANDROID )

	// Android Log
		switch(ineType)
		{
		case eWarning:
		case eWarningAlways:
			{
				LOGW(tempString.c_str());
				break;
			}

		case eError:
		case eErrorAlways:
			{
				LOGE(tempString.c_str());
				break;
			}

		case eInput:
		case eInputResponse:
		default:
			LOGI(tempString.c_str());
			break;
		}
#elif defined( OS_IOS )
        
    IOSLog(tempString.c_str());
    
#endif
}

