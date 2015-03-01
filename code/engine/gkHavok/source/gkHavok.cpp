#include "StableHeader.h"
#include "gkHavok.h"

#include "ISystem.h"

// Math and base include
#include <Common/Base/hkBase.h>
#include <Common/Base/System/hkBaseSystem.h>
#include <Common/Base/System/Error/hkDefaultError.h>
#include <Common/Base/Memory/System/Util/hkMemoryInitUtil.h>
#include <Common/Base/Monitor/hkMonitorStream.h>
#include <Common/Base/Memory/System/hkMemorySystem.h>
#include <Common/Base/Memory/Allocator/Malloc/hkMallocAllocator.h>

// Dynamics includes
#include <Physics2012/Collide/hkpCollide.h>										
#include <Physics2012/Collide/Agent/ConvexAgent/SphereBox/hkpSphereBoxAgent.h>	
#include <Physics2012/Collide/Shape/Convex/Box/hkpBoxShape.h>					
#include <Physics2012/Collide/Shape/Convex/Sphere/hkpSphereShape.h>				
#include <Physics2012/Collide/Dispatch/hkpAgentRegisterUtil.h>					

#include <Physics2012/Collide/Query/CastUtil/hkpWorldRayCastInput.h>			
#include <Physics2012/Collide/Query/CastUtil/hkpWorldRayCastOutput.h>			

#include <Physics2012/Dynamics/World/hkpWorld.h>								
#include <Physics2012/Dynamics/Entity/hkpRigidBody.h>							
#include <Physics2012/Utilities/Dynamics/Inertia/hkpInertiaTensorComputer.h>	

#include <Common/Base/Thread/Job/ThreadPool/Cpu/hkCpuJobThreadPool.h>
//#include <Common/Base/Thread/Job/ThreadPool/Spu/hkSpuJobThreadPool.h>
#include <Common/Base/Thread/JobQueue/hkJobQueue.h>

// Visual Debugger includes
#include <Common/Visualize/hkVisualDebugger.h>
#include <Physics2012/Utilities/VisualDebugger/hkpPhysicsContext.h>		

#include <Common/Serialize/Util/hkSerializeUtil.h>
#include <Common/Serialize/Resource/hkResource.h>
#include <Common/Base/System/Io/IStream/hkIStream.h>

// aNIMATION
#include <Animation/Animation/hkaAnimationContainer.h>
#include <Animation/Animation/Playback/Control/Default/hkaDefaultAnimationControl.h>
#include <Animation/Animation/Playback/hkaAnimatedSkeleton.h>
#include <Animation/Animation/Rig/hkaPose.h>
#include <Common/Serialize/Util/hkLoader.h>
#include <Common/Serialize/Util/hkRootLevelContainer.h>
#include "ITimer.h"
#include "IRenderer.h"
#include "IAuxRenderer.h"

#include "physicsentity/gkPhysicsLayer.h"


#define NUM_SKELETONS 10

static void HK_CALL gkHavokErrorReport(const char* msg, void* userArgGivenToInit)
{
	printf("%s", msg);
}
//////////////////////////////////////////////////////////////////////////
gkHavok* getHavokPtr()
{
	return static_cast<gkHavok*>(gEnv->pPhysics);
}

gkHavok::gkHavok():
m_havokLoader(NULL)
{
}

gkHavok::~gkHavok( void )
{
	
}

void gkHavok::InitPhysics()
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



		//
		// <PHYSICS-ONLY>: Create the physics world.
		// At this point you would initialize any other Havok modules you are using.
		//
		
		{
			// The world cinfo contains global simulation parameters, including gravity, solver settings etc.
			hkpWorldCinfo worldInfo;

			// Set the simulation type of the world to multi-threaded.
			worldInfo.m_simulationType = hkpWorldCinfo::SIMULATION_TYPE_MULTITHREADED;

			// Flag objects that fall "out of the world" to be automatically removed - just necessary for this physics scene
			worldInfo.m_broadPhaseBorderBehaviour = hkpWorldCinfo::BROADPHASE_BORDER_REMOVE_ENTITY;
			worldInfo.m_gravity = hkVector4(0,-9.8f,0,0);
			physicsWorld = new hkpWorld(worldInfo);

			// Disable deactivation, so that you can view timers in the VDB. This should not be done in your game.
			physicsWorld->m_wantDeactivation = false;

			


			// When the simulation type is SIMULATION_TYPE_MULTITHREADED, in the debug build, the sdk performs checks
			// to make sure only one thread is modifying the world at once to prevent multithreaded bugs. Each thread
			// must call markForRead / markForWrite before it modifies the world to enable these checks.
			physicsWorld->markForWrite();


			// Register all collision agents, even though only box - box will be used in this particular example.
			// It's important to register collision agents before adding any entities to the world.
			hkpAgentRegisterUtil::registerAllAgents( physicsWorld->getCollisionDispatcher() );

			// We need to register all modules we will be running multi-threaded with the job queue
			physicsWorld->registerWithJobQueue( jobQueue );

			physicsWorld->unmarkForWrite();
		}
		//
		// Initialize the VDB
		//
		


		// <PHYSICS-ONLY>: Register physics specific visual debugger processes
		// By default the VDB will show debug points and lines, however some products such as physics and cloth have additional viewers
		// that can show geometries etc and can be enabled and disabled by the VDB app.
		
		{
			// The visual debugger so we can connect remotely to the simulation
			// The context must exist beyond the use of the VDB instance, and you can make
			// whatever contexts you like for your own viewer types.
			context = new hkpPhysicsContext();
			hkpPhysicsContext::registerAllPhysicsProcesses(); // all the physics viewers
			context->addWorld(physicsWorld); // add the physics world so the viewers can see it
			contexts.pushBack(context);

			// Now we have finished modifying the world, release our write marker.
		}

		vdb = new hkVisualDebugger(contexts);
		vdb->serve();

	}

}

void gkHavok::DestroyPhysics()
{
	//m_havokLoader->removeReference();

	//
	// Clean up physics and graphics
	//
	DestroyAllPhysicLayer();

	// <PHYSICS-ONLY>: cleanup physics
	{
		physicsWorld->markForWrite();
		physicsWorld->removeReference();
	}
	vdb->removeReference();

	// Contexts are not reference counted at the base class level by the VDB as
	// they are just interfaces really. So only delete the context after you have
	// finished using the VDB.
	context->removeReference();
	contexts.clearAndDeallocate();

	delete jobQueue;

	//
	// Clean up the thread pool
	//

	threadPool->removeReference();
	


	hkBaseSystem::quit();
	hkMemoryInitUtil::quit();
}

void gkHavok::UpdatePhysics( float fElapsedTime )
{
	static float prevTime = gEnv->pTimer->GetCurrTime();
	
	if ( (gEnv->pTimer->GetCurrTime() - prevTime) > (1.0f / 60.0f) )
	{	
		{
			//physicsWorld->markForWrite();
			physicsWorld->lock();
			physicsWorld->stepMultithreaded( jobQueue, threadPool, (gEnv->pTimer->GetCurrTime() - prevTime) );
			physicsWorld->unlock();
			//physicsWorld->unmarkForWrite();
		}

		// Step the visual debugger. We first synchronize the timer data
		context->syncTimers( threadPool );
		vdb->step();

		// Clear accumulated timer data in this thread and all slave threads
		hkMonitorStream::getInstance().reset();
		threadPool->clearTimerData();

		prevTime = gEnv->pTimer->GetCurrTime();

	}
}

//////////////////////////////////////////////////////////////////////////
IGameObjectPhysicLayer* gkHavok::CreatePhysicLayer()
{
	gkPhysicLayer* ret = new gkPhysicLayer();
	if (ret)
	{
		m_vecPhysicLayers.push_back(ret);
	}
	return ret;
}
//////////////////////////////////////////////////////////////////////////
void gkHavok::DestroyPhysicLayer(IGameObjectPhysicLayer* target)
{
	PhysicLayerArray::iterator it = m_vecPhysicLayers.begin();

	for (; it != m_vecPhysicLayers.end(); ++it)
	{
		if ( *it == target)
		{
			SAFE_DELETE(*it);
			it = m_vecPhysicLayers.erase(it);
			break;
		}
	}
}
//////////////////////////////////////////////////////////////////////////
void gkHavok::DestroyAllPhysicLayer()
{
	PhysicLayerArray::iterator it = m_vecPhysicLayers.begin();
	for (; it != m_vecPhysicLayers.end(); ++it)
	{
		SAFE_DELETE(*it);
	}

	m_vecPhysicLayers.clear();
}

void gkHavok::HavokThreadInit()
{
	hkBaseSystem::initThread( memoryRouter );
}

void gkHavok::HavokThreadQuit()
{
	hkBaseSystem::quitThread();
}
