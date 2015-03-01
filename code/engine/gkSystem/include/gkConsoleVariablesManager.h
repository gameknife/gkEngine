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
// Name:   	gkBaseParameterSet.h
// Desc:	全局参数解析器
//			
//
// 描述:	
// 
// Author:  Kaiming-Desktop
// Date:	2011/5/16
// Modify:	
// 
//////////////////////////////////////////////////////////////////////////

#pragma once
#include "gkSystemPrerequisites.h"
#include "IInputManager.h"

class gkIniParser;

struct gkConsoleVariablesManager : public IConsoleVariablesManager, 
	public IInputEventListener, 
	public IFileChangeMonitorListener
{
	gkConsoleVariablesManager();
	virtual ~gkConsoleVariablesManager();
	virtual void reloadAllParameters();
	virtual void saveAllParameters();
	void fileListener();

	virtual void PrintLine(const TCHAR *s);

	virtual void PrintLinePlus(const TCHAR *s);
	virtual void registerVar( const gkStdString& name, uint8 type, void* var, const gkStdString& desc = _T(""), double fMin = 0.0, double fMax = 100.0, double defvalue = 0.0);

	virtual CVarNameMap& getCVarMap() {return m_mapCVarNameMap;}
	virtual gkCVar* getCVar(const gkStdString& name);


	// Interface for gkSystem call Console Renderer [2/5/2012 Kaiming]
	void update(float fElapsedTime);
	virtual void render();

	void executeCommand(const TCHAR* wszCommandline);

	void setEnabled(bool makeEnable) {m_bConsoleEnable = makeEnable;}
	virtual bool isConsoleEnable() {return m_bConsoleEnable;}

	// Input method
	void initInput();
	void destroyInput();
	void AddInputChar(const TCHAR c);
	void removeInputChar(bool bBackSpace);
	void splitCommands(const TCHAR* line, std::vector<gkStdString>& split);
	void AddLine(gkStdString str);
	void AddCommandToHistory( const TCHAR *szCommand );
	const TCHAR* GetHistoryElement( const bool bUpOrDown );

	virtual bool OnInputEvent( const SInputEvent &event );
	virtual void OnFileChange( const TCHAR* filename );


	int m_nCursorPos;
	gkStdString m_strInputBuffer;
	gkStdString	m_sReturnString;
	typedef std::deque<gkStdString> ConsoleBuffer;

	ConsoleBuffer m_dqConsoleBuffer;
	ConsoleBuffer m_dqHistory;
	int m_nHistoryPos;

	CVarNameMap m_mapCVarNameMap;

	TCHAR wbuffer[MAX_PATH];

	bool m_bConsoleEnable;

	float m_fBlinkTimer;

	gkIniParser* m_cfgFileParser;
	struct IFtFont* m_consoleFont;
};

