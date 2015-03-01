#include "gkSystemStableHeader.h"
#include "gkSystemInfo.h"

#ifdef OS_LINUX
#include <pthread.h>
#endif

void InitSystemInfo()
{
#ifdef OS_WIN32
	SYSTEM_INFO siSysInfo;
	GetSystemInfo(&siSysInfo);
	gEnv->pSystemInfo->cpuCoreCount = siSysInfo.dwNumberOfProcessors;
	gEnv->pSystemInfo->mainThreadId = GetCurrentThreadId();
#else
	gEnv->pSystemInfo->cpuCoreCount = 1;
	gEnv->pSystemInfo->mainThreadId = pthread_self();
#endif

}
