#include "gkSystemStableHeader.h"

#ifdef OS_WIN32

#include "FileChangeMonitor.h"


FileChangeMonitor::FileChangeMonitor(void)
{
	m_workThread = new FileChangeMonitorThread(this);
	m_workThread->Start();

	//m_workThread->Run();
	m_workThread->Pause();
}


FileChangeMonitor::~FileChangeMonitor(void)
{
	m_workThread->Stop();
	m_workThread->Wait(1000);
	m_workThread->Terminate();
	delete m_workThread;
}

void FileChangeMonitor::StartMonitor()
{
	m_workThread->Resume();
}

void FileChangeMonitor::StopMonitor()
{
	//m_workThread->Pause();
}

void FileChangeMonitor::AddListener( IFileChangeMonitorListener* listener )
{
	for (uint32 i=0; i < m_listeners.size(); ++i)
	{
		if ( m_listeners[i] == listener)
		{
			return;
		}
	}

	for (uint32 i=0; i < m_listeners.size(); ++i)
	{
		if ( m_listeners[i] == NULL)
		{
			m_listeners[i] = listener;
			return;
		}
	}
	m_listeners.push_back(listener);
}

void FileChangeMonitor::RemoveListener( IFileChangeMonitorListener* listener )
{
	for (uint32 i=0; i < m_listeners.size(); ++i)
	{
		if ( m_listeners[i] == listener)
		{
			m_listeners[i] = NULL;
		}
	}
}

void FileChangeMonitor::PostFileChangeEvent( const TCHAR* filename )
{
	gkStdString thisChange(filename);
	for (uint32 i=0; i < m_changes.size(); ++i)
	{
		if ( m_changes[i] == thisChange)
		{
			return;
		}
	}

	m_changes.push_back(thisChange);
}

void FileChangeMonitor::Update()
{


	for (uint32 k=0; k < m_changes.size(); ++k)
	{
		gkNormalizePath(m_changes[k]);
		for (uint32 i=0; i < m_listeners.size(); ++i)
		{
			if (m_listeners[i] != NULL)
			{
				m_listeners[i]->OnFileChange(m_changes[k].c_str());
			}
		}
	}



	m_changes.clear();
}

int FileChangeMonitorThread::Run()
{
	// set the task 
	//gkLogMessage(_T("FileChangeMonitor Starting..."));

// 	DWORD dwWaitStatus; 
// 	HANDLE dwChangeHandle; //返回通知的句柄
// 	dwChangeHandle = FindFirstChangeNotification( gkGetExecRootDir().c_str(), TRUE, FILE_NOTIFY_CHANGE_LAST_WRITE );
// 	if(dwChangeHandle==INVALID_HANDLE_VALUE)
// 	{
// 		gkLogError(_T("文件监控线程启动失败"));
// 		return 1;
// 	}
// 	ReadDirectoryChangesW
// 
// 	//gkLogMessage(_T("FileChangeMonitor Running..."));
// 	dwWaitStatus=WaitForSingleObject(dwChangeHandle, INFINITE);
// 	if (dwWaitStatus == 0)
// 	{
// 		gkLogMessage(_T("Some file changed."));
// 	}

	hDir = CreateFile( gkGetExecRootDir().c_str(), FILE_LIST_DIRECTORY,
		FILE_SHARE_READ | 
		FILE_SHARE_WRITE | 
		FILE_SHARE_DELETE, NULL, 
		OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | 
		FILE_FLAG_OVERLAPPED, NULL); 

// 	HANDLE hDir = CreateFile( gkGetExecRootDir().c_str(), FILE_LIST_DIRECTORY,
// 		FILE_SHARE_READ | 
// 		FILE_SHARE_WRITE | 
// 		FILE_SHARE_DELETE, NULL, 
// 		OPEN_EXISTING, NULL 
// 		, NULL); 

	char buffer[2048];
	DWORD cbBytes;
	float lastTime = 0;

	FILE_NOTIFY_INFORMATION *pnotify=(FILE_NOTIFY_INFORMATION *)buffer; 
	FILE_NOTIFY_INFORMATION *tmp; 
	// run
	while(1)
	{
		if (m_finished)
		{
			break;
		}
		m_controlEvent->Wait();

		memset( buffer, 0, sizeof(buffer));

		// 阻塞
		if (ReadDirectoryChangesW( hDir, buffer, sizeof(buffer), TRUE, FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_FILE_NAME, &cbBytes, NULL, NULL))
		{
			tmp = pnotify;

			switch(tmp->Action)
			{
			case FILE_ACTION_MODIFIED:
			case FILE_ACTION_ADDED:
			case FILE_ACTION_RENAMED_NEW_NAME:
			case FILE_ACTION_REMOVED:
			case FILE_ACTION_RENAMED_OLD_NAME:
				{
					// 防止连续调用，手速小于0.05秒几乎只有机器办得到
					//if (gEnv->pTimer->GetCurrTime() - lastTime > 0.05f)
					{
						// 				_tcscpy(filename, tmp->FileName);
						// 				filename[tmp->FileNameLength] = 0;
						//gkLogMessage(_T("file %s changed."), tmp->FileName);
#ifndef UNICODE
	char buffer[MAX_PATH];
	WideCharToMultiByte( CP_ACP, 0, tmp->FileName, -1, buffer, MAX_PATH, NULL, NULL );
	m_monitor->PostFileChangeEvent( buffer );
#else
	m_monitor->PostFileChangeEvent( tmp->FileName );
#endif


						

						lastTime = gEnv->pTimer->GetCurrTime();
					}
				}


				break;
			}
		}
		//gkLogMessage(_T("FileChangeMonitor Running..."));
		//Sleep(1000);
	}

	
	//FindCloseChangeNotification(dwChangeHandle);

	// finish
	//gkLogMessage(_T("FileChangeMonitor Terminating..."));
	return 0;
}

FileChangeMonitorThread::FileChangeMonitorThread( FileChangeMonitor* monitor )
{
	m_monitor = monitor;
	m_finished = false;
	m_controlEvent = new CManualResetEvent(false);
	m_controlEvent->Set();
}

FileChangeMonitorThread::~FileChangeMonitorThread()
{
	delete m_controlEvent;
}

void FileChangeMonitorThread::Stop()
{
	m_finished = true;
	CloseHandle( hDir );
	Resume();
}

#endif
