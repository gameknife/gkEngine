#include "gkSystemStableHeader.h"
#include "gkConsoleVariablesManager.h"
//#include "gkMisc.h"

#include <iostream>
#include <locale>

#ifdef OS_WIN32
#include <conio.h>
#endif

#include "IInputManager.h"
#include "IRenderer.h"
#include "gkIniParser.h"
#include "IAuxRenderer.h"


void NormalizeName(gkStdString& str)
{
#ifdef UNICODE
	std::transform( str.begin(), str.end(), str.begin(), towlower );
#else
	std::transform( str.begin(), str.end(), str.begin(), tolower );
#endif
}

void gkConsoleVariablesManager::registerVar( const gkStdString& name, uint8 type, void* var, const gkStdString& desc, double fMin, double fMax, double defvalue)
{
	gkStdString innerName(name);
	NormalizeName(innerName);
	m_mapCVarNameMap.insert(CVarNameMap::value_type(innerName, gkCVar(type, var, desc, fMin, fMax)));

	if (type == GK_CVARTYPE_FLOAT)
	{
		*((double*)var) = defvalue;
	}
	else if (type == GK_CVARTYPE_INT)
	{
		*((int*)var) = (int)defvalue;
	}
}


//-----------------------------------------------------------------------
gkConsoleVariablesManager::gkConsoleVariablesManager()
{
	m_cfgFileParser = new gkIniParser(_T("engine/config/engineconsolevariables.cfg"));
	m_consoleFont = 0;
#ifdef OS_WIN32
	//AllocConsole();
#endif
}
//-----------------------------------------------------------------------
void gkConsoleVariablesManager::reloadAllParameters()
{
	m_cfgFileParser->Parse();

	CVarNameMap::iterator it = m_mapCVarNameMap.begin();
	for (; it != m_mapCVarNameMap.end(); ++it)
	{
		gkStdString ret = m_cfgFileParser->FetchValue(_T("engineconsolevaribles"), it->first.c_str());

		if(ret != _T(""))
		{
			if( it->second.m_type == 0 )
			{
				// double
				*((double*)it->second.m_ptr) = _tcstod(ret.c_str(), NULL);
				gkLogMessage( _T("cvar [%s] change to %f."), it->first.c_str(), it->second.getFloat() );
			}
			else if (it->second.m_type == 1)
			{
				// int
				*((int*)it->second.m_ptr) = _ttoi(ret.c_str());
				gkLogMessage( _T("cvar [%s] change to %d."), it->first.c_str(), it->second.getInt() );
			}
		}

	}

	gkLogMessage(_T("Cfg File Reload."));
}
//-----------------------------------------------------------------------
void gkConsoleVariablesManager::fileListener()
{
// 	DWORD dwWaitStatus;
// 	HANDLE dwChangeHandle;
// 	dwChangeHandle=FindFirstChangeNotification(m_wstrPath.c_str(),false,FILE_NOTIFY_CHANGE_FILE_NAME);
// 	if(dwChangeHandle==INVALID_HANDLE_VALUE)
// 		ExitProcess(GetLastError());
// 	while(true){
// 		dwWaitStatus=WaitForSingleObject(dwChangeHandle,-1);
// 		switch(dwWaitStatus){
// 			case 0:
// 				reloadAllParameters();
// 				FindCloseChangeNotification(dwChangeHandle);
// 				exit(EXIT_SUCCESS);
// 			default:
// 				ExitProcess(GetLastError());
// 		}
// 	}
}
//-----------------------------------------------------------------------
gkConsoleVariablesManager::~gkConsoleVariablesManager()
{
	SAFE_DELETE( m_cfgFileParser );
	//FreeConsole();
}
//-----------------------------------------------------------------------
void gkConsoleVariablesManager::PrintLine( const TCHAR *s )
{
// 	freopen( "CONOUT$","w",stdout);
// 	std::locale loc("chs");
// 	std::wcout.imbue( loc );
// 	std::wcout << s << std::endl;

	AddLine(gkStdString(s));
}
//-----------------------------------------------------------------------
void gkConsoleVariablesManager::PrintLinePlus( const TCHAR *s )
{
//  	freopen( "CONOUT$","w",stdout);
//  	std::locale loc( "chs" );
//  	std::wcout.imbue( loc );
//  	std::wcout << s << std::endl;
	AddLine(gkStdString(s));
}

void gkConsoleVariablesManager::saveAllParameters()
{
	#ifdef OS_WIN32
	// find file
	// 先在目录内查找名字 [9/18/2010 Kaiming-Desktop]
	TCHAR wszPath[MAX_PATH] = _T("");
	HRESULT hr = gkFindFileRelativeExec(wszPath, MAX_PATH, _T("engine/config/engineconsolevariables.cfg"));
	assert( hr == S_OK );
	m_wstrPath = wszPath;

	TCHAR buffer[MAX_PATH];
	CVarNameMap::iterator it = m_mapCVarNameMap.begin();
	for (; it != m_mapCVarNameMap.end(); ++it)
	{
		if( it->second.m_type == 0 )
		{
				// double
			_stprintf_s(buffer, MAX_PATH, _T("%.4f"), *((double*)it->second.m_ptr));
								
		}
		else if (it->second.m_type == 1)
		{
			// int
			_stprintf_s(buffer, MAX_PATH, _T("%d"), *((int*)it->second.m_ptr));
		}


		WritePrivateProfileString(_T("engineconsolevaribles"), it->first.c_str(), buffer, wszPath);

	}

	#endif
}

void gkConsoleVariablesManager::update(float fElapsedTime)
{
	

	//processInput();
	m_fBlinkTimer += fElapsedTime;
}

void gkConsoleVariablesManager::render()
{
	if (!m_consoleFont)
	{
		m_consoleFont = gEnv->pFont->CreateFont( _T("engine/fonts/msyh.ttf"), 14, GKFONT_OUTLINE_0PX );
	}
// 	if (m_bConsoleEnable)
// 	{
// 		gEnv->pRenderer->FX_RenderText(wbuffer);
// 	}

	if (m_bConsoleEnable)
	{
		TCHAR buffer[MAX_PATH];
		// 		MultiByteToWideChar( CP_ACP, 0, m_strInputBuffer.c_str(), -1, buffer, MAX_PATH );
		// 
		// 		wcscpy_s(wbuffer, _T(""));
		// 		for (int i=0; i < (; ++i)
		// 		{
		// 			m_dqConsoleBuffer.rbegin();
		// 			wcscat_s(wbuffer, _T("\n"));
		// 		}

		int m_nScrollLine = m_dqConsoleBuffer.size() - (gEnv->pRenderer->GetScreenHeight() / 24) - 4;
		m_nScrollLine = 0;
		int csize = 12;
		int yPos = gEnv->pRenderer->GetScreenHeight() / 2 - 35;

		ConsoleBuffer::reverse_iterator ritor;
		ritor=m_dqConsoleBuffer.rbegin();
		int nScroll=0;
		while(ritor!=m_dqConsoleBuffer.rend() && yPos>=0)  
		{
			if(nScroll>=m_nScrollLine)
			{
				//if (yPos+csize>0) 
// #ifdef UNICODE
// 				MultiByteToWideChar( CP_ACP, 0, ritor->c_str(), -1, buffer, MAX_PATH );
// #else
				_tcscpy(buffer, ritor->c_str());
//#endif
				ColorB whitecol = ColorB(255,255,255,255);

				gEnv->pRenderer->getAuxRenderer()->AuxRenderText( buffer, 5, yPos, m_consoleFont, whitecol, eFA_Left | eFA_Top  );

				yPos-=csize;
			}
			nScroll++;

			++ritor;
		}

		_tcscpy_s(wbuffer, _T("$3gkENGINE > "));

// #ifdef UNICODE
// 		MultiByteToWideChar( CP_ACP, 0, m_strInputBuffer.c_str(), -1, buffer, MAX_PATH );
// #else
		_tcscpy_s(buffer, MAX_PATH, m_strInputBuffer.c_str());
//#endif
		_tcscat_s(wbuffer, buffer);

		

		
 		if (m_fBlinkTimer > 0.4f)
 		{
 			_tcscat_s(wbuffer, _T("_"));
 			if ( m_fBlinkTimer > 0.8f)
 				m_fBlinkTimer = 0;
 		}

		ColorB color = ColorB(144, 243, 0, 180);
		gEnv->pRenderer->getAuxRenderer()->AuxRenderText( wbuffer, 5, gEnv->pRenderer->GetScreenHeight() / 2 - 20, m_consoleFont, color, eFA_Left | eFA_Top  );
	}
}

void gkConsoleVariablesManager::splitCommands(const TCHAR* line, std::vector<gkStdString>& split)
{
	const TCHAR *start = line;
	gkStdString working;

	while(true)
	{
		TCHAR ch = *line++;
		switch(ch)
		{
		case _T('\''):
		case _T('\"'):		
			while((*line++ != ch) && *line);
			break;
		case _T('\n'):
		case _T('\r'):
		case _T(';'):
		case _T(' '):
		case _T('\0'):
			{
				working.assign(start, line-1);
				//working.trim();

				if (!working.empty())
					split.push_back(working);
				start = line;

				if (ch == _T('\0'))
					return;
			}
			break;
		}
	}
}

std::string ws2s(const std::wstring& ws)
{
	std::string curLocale = setlocale(LC_ALL, NULL); // curLocale = "C";
	setlocale(LC_ALL, "chs"); 
	const wchar_t* _Source = ws.c_str();
	size_t _Dsize = 2 * ws.size() + 1;
	char *_Dest = new char[_Dsize];
	memset(_Dest,0,_Dsize);
	wcstombs(_Dest,_Source,_Dsize);
	std::string result = _Dest;
	delete []_Dest;
	setlocale(LC_ALL, curLocale.c_str());
	return result;
}

std::wstring s2ws(const std::string& s)
{
	std::string curLocale = setlocale(LC_ALL, NULL); // curLocale = "C";
	setlocale(LC_ALL, "chs"); 
	const char* _Source = s.c_str();
	size_t _Dsize = s.size() + 1;
	wchar_t *_Dest = new wchar_t[_Dsize];
	wmemset(_Dest, 0, _Dsize);
	mbstowcs(_Dest,_Source,_Dsize);
	std::wstring result = _Dest;
	delete []_Dest;
	setlocale(LC_ALL, curLocale.c_str());
	return result;
}

void gkConsoleVariablesManager::executeCommand(const TCHAR* wszCommandline)
{
	// exract command
	std::vector<gkStdString> lineCommands;
	splitCommands(wszCommandline, lineCommands);

	if (lineCommands.size() > 1)
	{
		CVarNameMap::iterator it = m_mapCVarNameMap.begin();
// #ifdef UNICODE
// 		gkStdString wCommand = s2ws(lineCommands[0]);
// #else
		gkStdString wCommand = lineCommands[0];
//#endif

		for (; it != m_mapCVarNameMap.end(); ++it)
		{
			if ( 0 == _tcsicmp(it->first.c_str(), wCommand.c_str() ))
			{
				gkStdStringstream ss(lineCommands[1]);
				switch( it->second.m_type )
				{
				case GK_CVARTYPE_FLOAT:

					ss >> (*((double*)(it->second.m_ptr)));
					gkLogInput( _T("Command [ %s %.3f ] executed successfully."), wCommand.c_str(), *((float*)(it->second.m_ptr)) );
					break;

				case GK_CVARTYPE_INT:
					ss >> (*((int*)(it->second.m_ptr)));
					gkLogInput( _T("Command [ %s %d ] executed successfully."), wCommand.c_str(), *((int*)(it->second.m_ptr)) );
					break;
				}
				AddCommandToHistory(wszCommandline);
				return;
			}

			
		}

		gkLogInput( _T("Command [ %s ] not find, executed failed."), wCommand.c_str() );
	}
	m_nCursorPos = 0;
}

void gkConsoleVariablesManager::removeInputChar(bool bBackSpace)
{
	if(m_strInputBuffer.empty())
		return;

	if(bBackSpace)
	{
		if(m_nCursorPos>0)
		{
			m_strInputBuffer.erase(m_nCursorPos-1,1);
			m_nCursorPos--;
		}
	}
	else
	{
		if(m_nCursorPos<(int)(m_strInputBuffer.length()))
			m_strInputBuffer.erase(m_nCursorPos,1);
	}
}

void gkConsoleVariablesManager::AddInputChar(const TCHAR c)
{
	if (c == _T('\0'))
	{
		return;
	}
	if(m_nCursorPos<(int)(m_strInputBuffer.length()))
		m_strInputBuffer.insert(m_nCursorPos,1,c);
	else
	{
		TCHAR buf[2];
		buf[0] = c;
		buf[1] = _T('\0');
		m_strInputBuffer = m_strInputBuffer + buf;
	}
	m_nCursorPos++;
}

void gkConsoleVariablesManager::initInput()
{
	m_nCursorPos = 0;
	m_bConsoleEnable = false;

	m_dqConsoleBuffer.clear();

	m_fBlinkTimer = 0;
	m_nHistoryPos = -1;

	if (gEnv->pInputManager)
	{
		gEnv->pInputManager->addInputEventListener(this);
	}	

	if (gEnv->pFileChangeMonitor)
	{
		gEnv->pFileChangeMonitor->AddListener(this);
	}

	
}


void gkConsoleVariablesManager::destroyInput()
{
	gEnv->pInputManager->removeEventListener(this);
}


//////////////////////////////////////////////////////////////////////////
void gkConsoleVariablesManager::AddLine(gkStdString wstr)
{
// #ifdef UNICODE
// 	std::string str = ws2s(wstr);
// #else
	gkStdString str = wstr;
//#endif
 	// strip trailing \n or \r.
 	if (!str.empty() && (str[str.size()-1]== _T('\n') || str[str.size()-1]== _T('\r')))
 		str.resize(str.size()-1);
 
	gkStdString::size_type nPos;
 	while ((nPos=str.find(_T('\n')))!=gkStdString::npos)
 	{
 		str.replace(nPos,1,1,_T(' '));
 	}
 
 	while ((nPos=str.find(_T('\r')))!=gkStdString::npos)
 	{
 		str.replace(nPos,1,1,_T(' '));
 	}
 
 	m_dqConsoleBuffer.push_back(str);
 
 	int nBufferSize = 500;
 
 	while(((int)(m_dqConsoleBuffer.size()))>nBufferSize)
 	{
 		m_dqConsoleBuffer.pop_front();
 	}

}

//////////////////////////////////////////////////////////////////////////
void gkConsoleVariablesManager::AddCommandToHistory( const TCHAR *szCommand )
{
	assert(szCommand);

	m_nHistoryPos=-1;

	if(!m_dqHistory.empty())
	{
		// add only if the command is != than the last
		if(m_dqHistory.front()!=szCommand)
			m_dqHistory.push_front(szCommand);
	}
	else
		m_dqHistory.push_front(szCommand);

	while(m_dqHistory.size()>10)
		m_dqHistory.pop_back();
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
const TCHAR* gkConsoleVariablesManager::GetHistoryElement( const bool bUpOrDown )
{
	if(bUpOrDown)
	{
		if(!m_dqHistory.empty())
		{
			if(m_nHistoryPos<(int)(m_dqHistory.size()-1))
			{
				m_nHistoryPos++;
				m_sReturnString = m_dqHistory[m_nHistoryPos];
				return m_sReturnString.c_str();
			}
		}
	}
	else
	{		
		if(m_nHistoryPos>0)
		{
			m_nHistoryPos--;
			m_sReturnString = m_dqHistory[m_nHistoryPos];
			return m_sReturnString.c_str();
		}
	}

	return 0;
}

bool gkConsoleVariablesManager::OnInputEvent( const SInputEvent &event )
{

	if (event.keyId != 0 && event.state == eIS_Released)
	{
		if( event.keyId == eKI_Tilde)
		{
			if (m_bConsoleEnable)
			{
				setEnabled(false);
				gEnv->pInputManager->setExclusiveListener(NULL);
			}
			else
			{
				setEnabled(true);
				gEnv->pInputManager->setExclusiveListener(this);
			}

			return true;	
		}

		if (m_bConsoleEnable)
		{
			switch(event.keyId)
			{
			case eKI_Enter:
				// execute

				executeCommand(m_strInputBuffer.c_str());
				m_strInputBuffer.clear();
				break;

			case eKI_Backspace:
				// backspace
				removeInputChar(true);
				break;

			case eKI_Up:
				{
					const TCHAR *szHistoryLine=GetHistoryElement(true);		// true=UP

					if(szHistoryLine)
					{
						m_strInputBuffer = szHistoryLine;
						m_nCursorPos=(int)m_strInputBuffer.size();
					}
					break;
				}


			case eKI_Down:
				{
					const TCHAR *szHistoryLine=GetHistoryElement(false);		// true=UP

					if(szHistoryLine)
					{
						m_strInputBuffer = szHistoryLine;
						m_nCursorPos=(int)m_strInputBuffer.size();
					}
					break;
				}



			default:
				const char*sKeyName = gEnv->pInputManager->GetKeyName(event, 1);

				if (sKeyName)
				{
					if(strlen(sKeyName)!=1)
						return true;
					AddInputChar( sKeyName[0] );
					return true;
				}
			}
		}

	}

	if( m_bConsoleEnable )
		return true;
	else
		return false;
}

gkCVar* gkConsoleVariablesManager::getCVar( const gkStdString& name )
{
	gkStdString innerName(name);
	NormalizeName(innerName);
	CVarNameMap::iterator it = m_mapCVarNameMap.find(innerName);
	if (it != m_mapCVarNameMap.end())
	{
		return &(it->second);
	}
	else
	{
		return NULL;
	}
}

void gkConsoleVariablesManager::OnFileChange( const TCHAR* filename )
{
	//gkLogMessage()

	if ( !_tcsicmp(filename, _T("/engine/config/engineconsolevariables.cfg")))
	{
		reloadAllParameters();
		gEnv->pInGUI->gkPopupMsg(_T("Config File Changed."));
	}
	
}
