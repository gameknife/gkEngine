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



/**
  @file FileChangeMonitor.h
  
  @author Kaiming

  更改日志 history
  ver:1.0
   
 */

#ifndef _FileChangeMonitor_H_
#define _FileChangeMonitor_H_

#include "gkPlatform.h"
#include "ISystem.h"
#include "IThread.h"
#include "IEvent.h"

class FileChangeMonitor;

class FileChangeMonitorThread : public IThread
{
public:
	FileChangeMonitorThread(FileChangeMonitor* monitor);
	~FileChangeMonitorThread();
	virtual int Run();
	
	void Pause() {m_controlEvent->Reset();}
	void Resume() {m_controlEvent->Set();}
	void Stop();

private:
	CManualResetEvent* m_controlEvent;
	int m_finished;
	FileChangeMonitor* m_monitor;
	HANDLE hDir;
};

typedef std::vector<IFileChangeMonitorListener*> FileChangeListener;

class FileChangeMonitor : public IFileChangeMonitor
{
public:
	FileChangeMonitor(void);
	~FileChangeMonitor(void);

	void StartMonitor();
	void StopMonitor();

	virtual void AddListener( IFileChangeMonitorListener* listener );
	virtual void RemoveListener( IFileChangeMonitorListener* listener );

	void Update();
	void PostFileChangeEvent( const TCHAR* filename );

private:
	FileChangeMonitorThread* m_workThread;
	FileChangeListener m_listeners;

	std::vector<gkStdString> m_changes;
};

#endif



