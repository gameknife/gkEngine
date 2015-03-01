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



//////////////////////////////////////////////////////////////////////////
//
// Name:   	gkLog.h
// Desc:	gkLogSystem	
// 
// Author:  Kaiming-Desktop
// Date:	2011 /8/18
// Modify:	2011 /8/18
// 
//////////////////////////////////////////////////////////////////////////

#ifndef GKLOG_H_
#define GKLOG_H_

#include "gkSystemPrerequisites.h"
#include "ILog.h"

class gkLog: public ILog
{
public:
	gkLog(ISystem* system);
	virtual ~gkLog(void);

	typedef std::list<ILogCallback*> Callbacks;	

	virtual void Release()  { delete this; }

	// Summary:
	//	 Sets the file used to log to disk.
	virtual bool	SetFileName(const gkStdString& command = NULL);

	// Summary:
	//	 Gets the filename used to log to disk.
	virtual const gkStdString&	GetFileName();

	//all the following functions will be removed are here just to be able to compile the project ---------------------------
	//
	virtual void RegisterConsoleVariables() {}

	//
	virtual void UnregisterConsoleVariables() {}

	// Notes:
	//	 Full logging (to console and file) can be enabled with verbosity 4.
	//	 In the console 'log_Verbosity 4' command can be used.
	virtual void	SetVerbosity( int verbosity );

	virtual int		GetVerbosityLevel();

	virtual void  AddCallback( ILogCallback *pCallback );
	virtual void  RemoveCallback( ILogCallback *pCallback );


	virtual void LogV( const ELogType ineType, const TCHAR* szFormat, va_list args );

private: // -------------------------------------------------------------------

	void LogStringToFile( const TCHAR* szString,bool bAdd,bool bError=false );
	void LogStringToConsole( const TCHAR* szString,bool bAdd=false );

	FILE* OpenLogFile( const TCHAR *filename,const TCHAR *mode );
	void CloseLogFile( bool force=false );

	// will format the message into m_szTemp
	void FormatMessage(const TCHAR *szCommand, ... );


	ISystem	*					m_pSystem;														//
	float						m_fLastLoadingUpdateTime;							// for non-frequent streamingEngine update
	TCHAR						m_szFilename[MAX_PATH];			// can be with path
	FILE *						m_pLogFile;
	FILE *						m_pErrFile;
	int							m_nErrCount;
	Callbacks					m_callbacks;		//

	gkStdString				m_wstrFilename;

	//ICVar *					m_pLogIncludeTime;										//

	//IConsole *			m_pConsole;														//

	struct SLogHistoryItem
	{
		TCHAR str[MAX_PATH];
		const TCHAR *ptr;
		ELogType type;
		float time;
	};
	SLogHistoryItem m_history[16];
	int m_iLastHistoryItem;

};


#endif


