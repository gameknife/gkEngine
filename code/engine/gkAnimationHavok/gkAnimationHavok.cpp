#include "gkAnimationHavok.h"
#include "gkHavokLoader.h"
#include "CharacterInstance.h"
#include "gkHkxMeshLoader.h"

#include <Common/Base/Memory/System/Util/hkMemoryInitUtil.h>
#include <Common/Base/Memory/Allocator/Malloc/hkMallocAllocator.h>

#include <Common/Base/System/hkBaseSystem.h>
#include <Common/Base/System/Error/hkDefaultError.h>
#include <Common/Base/Memory/System/Util/hkMemoryInitUtil.h>
#include <Common/Base/Monitor/hkMonitorStream.h>
#include <Common/Base/Memory/System/hkMemorySystem.h>

#include <Common/Base/Container/String/hkStringBuf.h>
#include <Common/Base/Ext/hkBaseExt.h>

#include <Common/Base/Thread/Job/ThreadPool/Cpu/hkCpuJobThreadPool.h>
#include <Common/Base/Thread/JobQueue/hkJobQueue.h>

static void HK_CALL gkHavokErrorReport(const char* msg, void* userArgGivenToInit)
{
	printf("%s", msg);
}

gkAnimationHavok::gkAnimationHavok(void)
{
	m_havokLoader = NULL;
}


gkAnimationHavok::~gkAnimationHavok(void)
{
}

void gkAnimationHavok::InitAnimation()
{
	//
	// Initialize the base system including our memory system
	//
	// Allocate 0.5MB of physics solver buffer.

	memoryRouter = hkMemoryInitUtil::initDefault( hkMallocAllocator::m_defaultMallocAllocator, hkMemorySystem::FrameInfo( 1024 * 1024 ) );
	hkBaseSystem::init( memoryRouter, gkHavokErrorReport );

	{
		//
		// Initialize the multi-threading classes, hkJobQueue, and hkJobThreadPool
		//

		// They can be used for all Havok multithreading tasks. In this exmaple we only show how to use
		// them for physics, but you can reference other multithreading demos in the demo framework
		// to see how to multithread other products. The model of usage is the same as for physics.
		// The hkThreadpool has a specified number of threads that can run Havok jobs.  These can work
		// alongside the main thread to perform any Havok multi-threadable computations.
		// The model for running Havok tasks in Spus and in auxilary threads is identical.  It is encapsulated in the
		// class hkJobThreadPool.  On PlayStation(R)3 we initialize the SPU version of this class, which is simply a SPURS taskset.
		// On other multi-threaded platforms we initialize the CPU version of this class, hkCpuJobThreadPool, which creates a pool of threads
		// that run in exactly the same way.  On the PlayStation(R)3 we could also create a hkCpuJobThreadPool.  However, it is only
		// necessary (and advisable) to use one Havok PPU thread for maximum efficiency. In this case we simply use this main thread
		// for this purpose, and so do not create a hkCpuJobThreadPool.


		// We can cap the number of threads used - here we use the maximum for whatever multithreaded platform we are running on. This variable is
		// set in the following code sections.
		int totalNumThreadsUsed;


		// Get the number of physical threads available on the system
		hkHardwareInfo hwInfo;
		hkGetHardwareInfo(hwInfo);
		totalNumThreadsUsed = hwInfo.m_numThreads;

		// We use one less than this for our thread pool, because we must also use this thread for our simulation
		hkCpuJobThreadPoolCinfo threadPoolCinfo;
		threadPoolCinfo.m_numThreads = totalNumThreadsUsed - 1;

		// This line enables timers collection, by allocating 200 Kb per thread.  If you leave this at its default (0),
		// timer collection will not be enabled.
		threadPoolCinfo.m_timerBufferPerThreadAllocation = 200000;
		threadPool = new hkCpuJobThreadPool( threadPoolCinfo );


		// We also need to create a Job queue. This job queue will be used by all Havok modules to run multithreaded work.
		// Here we only use it for physics.
		hkJobQueueCinfo info;
		info.m_jobQueueHwSetup.m_numCpuThreads = totalNumThreadsUsed;
		jobQueue = new hkJobQueue(info);

		//
		// Enable monitors for this thread.
		//

		// Monitors have been enabled for thread pool threads already (see above comment).
		hkMonitorStream::getInstance().resize(200000);

	}
	m_havokLoader = new gkHavokLoader();

	m_rigManager.Init();
	m_animationManager.Init();
}

void gkAnimationHavok::DestroyAnimation()
{
	m_rigManager.Destroy();
	m_animationManager.Destroy();

	delete m_havokLoader;
}

IGameObjectAnimLayer* gkAnimationHavok::CreateAnimLayer(const gkStdString& name)
{
	// check if we got one
	AnimLayerMap::iterator it = m_mapAnimLayers.find(name);

	if (it != m_mapAnimLayers.end())
	{
		gkLogError(_T("gkHavok::CreateAnimLayer Faild. Layer:[%s] has been created!"), name.c_str());
		return it->second;
	}


	gkCharacterInstance* pCI = new gkCharacterInstance(name.c_str());
	//pCI->loadChrFile(chrfile.c_str());

	m_mapAnimLayers.insert(AnimLayerMap::value_type(name, pCI));

	return pCI;
}

void gkAnimationHavok::DestroyAnimLayer(const gkStdString& name)
{
	// check if we got one
	AnimLayerMap::iterator it = m_mapAnimLayers.find(name);

	if (it != m_mapAnimLayers.end())
	{
		// ensure destroyed
		//it->second->Destroy();

		SAFE_DELETE( it->second );
	}

	m_mapAnimLayers.erase(it);
}

void gkAnimationHavok::DestroyAllAnimLayer()
{
	AnimLayerMap::iterator it = m_mapAnimLayers.begin();
	for (; it != m_mapAnimLayers.end(); ++it)
	{
		SAFE_DELETE( it->second );
	}
	m_mapAnimLayers.clear();
}

IMeshLoader* gkAnimationHavok::getHKXMeshLoader()
{
	return (new gkHkxMeshLoader() );
}

void gkAnimationHavok::_updateAnimation(float fElapsedTime)
{
    
	//throw std::logic_error("The method or operation is not implemented.");
    
    // Clear accumulated timer data in this thread and all slave threads
    hkMonitorStream::getInstance().reset();
    threadPool->clearTimerData();
    
}
