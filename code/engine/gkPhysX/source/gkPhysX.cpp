#include "StableHeader.h"
#include "gkPhysX.h"
#include "IGameObject.h"
#include "IGameObjectLayer.h"

#include <foundation/PxSimpleTypes.h>
#include <foundation/PxVec3.h>
#include <foundation/PxMat33.h>
#include <foundation/PxMat44.h>
#include <foundation/PxTransform.h>
#include <foundation/PxBounds3.h>
#include <foundation/PxErrorCallback.h>
#include "gkPhysicsLayer.h"

const int MAXSUBSTEP = 8;

using namespace physx;
PxDefaultAllocator gDefaultAllocatorCallback;
PxDefaultErrorCallback gDefaultErrorCallback;

#define	PVD_HOST	"127.0.0.1"
// 
// static PxSampleAllocator* gAllocator = NULL;
// 
// void initSampleAllocator()
// {
// 	PX_ASSERT(!gAllocator);
// 	gAllocator = new PxSampleAllocator;
// }
// 
// void releaseSampleAllocator()
// {
// 	DELETESINGLE(gAllocator);
// }
// 
// PxSampleAllocator* getSampleAllocator()
// {
// 	PX_ASSERT(gAllocator);
// 	return gAllocator;
// }

PxRigidActor* gkPhysX::createGrid()
{
	PxRigidStatic* plane = PxCreatePlane(*m_Physics, PxPlane(PxVec3(0,1,0), 0), *m_Material);

	m_Scene->addActor(*plane);

	PxShape* shape;
	plane->getShapes(&shape, 1);

	return plane;
}

///////////////////////////////////////////////////////////////////////////////

void gkPhysX::togglePvdConnection()
{
	if(!m_Physics->getPvdConnectionManager()) return;
	if (m_Physics->getPvdConnectionManager()->isConnected())
		m_Physics->getPvdConnectionManager()->disconnect();
	else
		createPvdConnection();
}

void gkPhysX::createPvdConnection()
{
	if(m_Physics->getPvdConnectionManager() == NULL)
		return;


	//The connection flags state overall what data is to be sent to PVD.  Currently
	//the Debug connection flag requires support from the implementation (don't send
	//the data when debug isn't set) but the other two flags, profile and memory
	//are taken care of by the PVD SDK.

	//Use these flags for a clean profile trace with minimal overhead
	PxVisualDebuggerConnectionFlags theConnectionFlags( PxVisualDebuggerExt::getAllConnectionFlags() );
	if (!mUseFullPvdConnection)
		theConnectionFlags = PxVisualDebuggerConnectionFlag::Profile;

	//Create a pvd connection that writes data straight to the filesystem.  This is
	//the fastest connection on windows for various reasons.  First, the transport is quite fast as
	//pvd writes data in blocks and filesystems work well with that abstraction.
	//Second, you don't have the PVD application parsing data and using CPU and memory bandwidth
	//while your application is running.
	//PxVisualDebuggerExt::createConnection(mPhysics->getPvdConnectionManager(), "c:\\temp.pxd2", theConnectionFlags);

	//The normal way to connect to pvd.  PVD needs to be running at the time this function is called.
	//We don't worry about the return value because we are already registered as a listener for connections
	//and thus our onPvdConnected call will take care of setting up our basic connection state.
	PVD::PvdConnection* theConnection = PxVisualDebuggerExt::createConnection(m_Physics->getPvdConnectionManager(), PVD_HOST, 5425, 10, theConnectionFlags );
	if (theConnection)
		theConnection->release();
}

void gkPhysX::onPvdConnected(PVD::PvdConnection& )
{
	//setup joint visualization.  This gets piped to pvd.
	m_Physics->getVisualDebugger()->setVisualizeConstraints(true);
	m_Physics->getVisualDebugger()->setVisualDebuggerFlag(PxVisualDebuggerFlags::eTRANSMIT_CONTACTS, true);
}

void gkPhysX::onPvdDisconnected(PVD::PvdConnection& )
{
}

void gkPhysX::InitPhysics()
{
	//Recording memory allocations is necessary if you want to 
	//use the memory facilities in PVD effectively.  Since PVD isn't necessarily connected
	//right away, we add a mechanism that records all outstanding memory allocations and
	//forwards them to PVD when it does connect.

	//This certainly has a performance and memory profile effect and thus should be used
	//only in non-production builds.
	bool recordMemoryAllocations = true;
#ifdef RENDERER_ANDROID
	const bool useCustomTrackingAllocator = false;
#else
	const bool useCustomTrackingAllocator = true;
#endif

	PxAllocatorCallback* allocator = &gDefaultAllocatorCallback;

	//if(useCustomTrackingAllocator)		
	//	allocator = getSampleAllocator();		//optional override that will track memory allocations

	m_Foundation = PxCreateFoundation(PX_PHYSICS_VERSION, *allocator, gDefaultErrorCallback);
	if(!m_Foundation)
	{
		gkLogError(_T("PxCreateFoundation failed!"));
	}

	m_ProfileZoneManager = &PxProfileZoneManager::createProfileZoneManager(m_Foundation);
	if(!m_ProfileZoneManager)
	{
		gkLogError(_T("PxProfileZoneManager failed!"));
	}

	if(true)
	{
#ifdef PX_WINDOWS
		pxtask::CudaContextManagerDesc cudaContextManagerDesc;
		m_CudaContextManager = pxtask::createCudaContextManager(*m_Foundation, cudaContextManagerDesc, m_ProfileZoneManager);
		if( m_CudaContextManager )
		{
			if( !m_CudaContextManager->contextIsValid() )
			{
				m_CudaContextManager->release();
				m_CudaContextManager = NULL;
			}
		}
#endif
	}

	m_Physics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_Foundation, PxTolerancesScale(), recordMemoryAllocations, m_ProfileZoneManager);
	if(!m_Physics)
	{
		gkLogError(_T("PxProfileZoneManager failed!"));
	}
	//if(getApplication().getOrCreateProfileZone(*mFoundation))
	//	mProfileZoneManager->addProfileZone(*getApplication().getProfileZone());

	if(!PxInitExtensions(*m_Physics))
	{
		gkLogError(_T("PxProfileZoneManager failed!"));
	}
	m_Cooking = PxCreateCooking(PX_PHYSICS_VERSION, *m_Foundation, PxCookingParams());
	if(!m_Cooking)
	{
		gkLogError(_T("PxProfileZoneManager failed!"));
	}

	togglePvdConnection();

	//if(mPhysics->getPvdConnectionManager())
	//	mPhysics->getPvdConnectionManager()->addHandler(*this);

	// setup default material...
	m_Material = m_Physics->createMaterial(0.5f, 0.5f, 0.1f);
	if(!m_Material)
	{
		gkLogError(_T("PxProfileZoneManager failed!"));
	}

	//PX_ASSERT(NULL == mScene);

	PxSceneDesc sceneDesc(m_Physics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, 0.0f, -9.81f);
	//getDefaultSceneDesc(sceneDesc);
	//customizeSceneDesc(sceneDesc);


	if(!sceneDesc.cpuDispatcher)
	{
		m_CpuDispatcher = PxDefaultCpuDispatcherCreate(mNbThreads);
		if(!m_CpuDispatcher)
		{
			gkLogError(_T("PxProfileZoneManager failed!"));
		}
		sceneDesc.cpuDispatcher	= m_CpuDispatcher;
	}

	if(!sceneDesc.filterShader)
		sceneDesc.filterShader	= PxDefaultSimulationFilterShader;

#ifdef PX_WINDOWS
	if(!sceneDesc.gpuDispatcher && m_CudaContextManager)
	{
		sceneDesc.gpuDispatcher = m_CudaContextManager->getGpuDispatcher();
	}
#endif

	//sceneDesc.flags |= PxSceneFlag::eENABLE_TWO_DIRECTIONAL_FRICTION;
	//sceneDesc.flags |= PxSceneFlag::eENABLE_PCM;//need to register the pcm code in the PxCreatePhysics
	//sceneDesc.flags |= PxSceneFlag::eENABLE_ONE_DIRECTIONAL_FRICTION;
	//sceneDesc.flags |= PxSceneFlag::eADAPTIVE_FORCE;
	sceneDesc.flags |= PxSceneFlag::eENABLE_ACTIVETRANSFORMS;
	m_Scene = m_Physics->createScene(sceneDesc);
	if(!m_Scene)
	{
		gkLogError(_T("PxProfileZoneManager failed!"));
	}

	PxSceneFlags flag = m_Scene->getFlags();


	m_Scene->setVisualizationParameter(PxVisualizationParameter::eSCALE,				0.0f);
	m_Scene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_SHAPES,	1.0f);


	//if(mCreateGroundPlane)
		createGrid();
	//LOG_INFO("PhysXSample", "Init sample ok!");
}

void gkPhysX::DestroyPhysics()
{
	DestroyAllPhysicLayer();


	SAFE_release(m_Scene);
	SAFE_release(m_CpuDispatcher);

	//	SAFE_RELEASE(mMaterial);
	SAFE_release(m_Cooking);

	PxCloseExtensions();

// 	if(getApplication().getProfileZone())
// 	{
// 		mProfileZoneManager->removeProfileZone(*getApplication().getProfileZone());
// 		getApplication().releaseProfileZone();
// 	}

	SAFE_release(m_Physics);

#ifdef PX_WINDOWS
	SAFE_release(m_CudaContextManager);
#endif

	SAFE_release(m_ProfileZoneManager);
	SAFE_release(m_Foundation);
}

void gkPhysX::UpdatePhysics( float fElapsedTime )
{
	m_FixedStepper.advance( m_Scene, fElapsedTime, 0, 0 );
}

void gkPhysX::PostUpdatePhysics()
{
	m_FixedStepper.renderDone();
	m_FixedStepper.wait(m_Scene);
}

IGameObjectPhysicLayer* gkPhysX::CreatePhysicLayer()
{
	gkPhysicLayer* ret = new gkPhysicLayer();
	if (ret)
	{
		m_PhysicLayers.push_back(ret);
	}

	return ret;
}

void gkPhysX::DestroyPhysicLayer( IGameObjectPhysicLayer* target )
{
	PhysicLayerArray::iterator it = m_PhysicLayers.begin();

	for (; it != m_PhysicLayers.end(); ++it)
	{
		if ( *it == target)
		{
			SAFE_DELETE(*it);
			it = m_PhysicLayers.erase(it);
			break;
		}
	}
}

void gkPhysX::DestroyAllPhysicLayer()
{
	PhysicLayerArray::iterator it = m_PhysicLayers.begin();
	for (; it != m_PhysicLayers.end(); ++it)
	{
		SAFE_DELETE(*it);
	}

	m_PhysicLayers.clear();
}

void gkPhysX::setupPhysics()
{

}

gkPhysX::gkPhysX():
	m_FixedStepper(0.016666660f, MAXSUBSTEP)
{
	mNbThreads = 1;
	mUseFullPvdConnection = true;
	m_PhysicLayers.clear();
}

bool gkPhysX::cookTriangleMesh( const PxTriangleMeshDesc& desc, PxOutputStream& stream )
{
	return m_Cooking->cookTriangleMesh(desc, stream);
}

gkPhysX* getPhysXPtr()
{
	return static_cast<gkPhysX*>(gEnv->pPhysics);
}

// inline physx::PxMat44 Matrix44toPxMat44( const Matrix44& matrix )
// {
// 	return PxMat44( (float*)(matrix.GetData()) );
// }
// 
// inline physx::PxVec3 Vec3toPxVec3( const Vec3& vec )
// {
// 	return PxVec3( vec.x, vec.y, vec.z );
// }
// 
// inline Vec3 PxVec3toVec3( const PxVec3& vec )
// {
// 	return Vec3(vec.x, vec.y, vec.z);
// }
// 
// inline Quat PxQuattoQuat( const PxQuat& quat )
// {
// 	return Quat( quat.w, quat.x, quat.y, quat.z );
// }
