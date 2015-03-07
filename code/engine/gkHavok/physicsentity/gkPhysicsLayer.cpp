#include "StableHeader.h"
#include "gkPhysicsLayer.h"
#include "IMesh.h"

#include <Physics2012/Collide/Filter/Group/hkpGroupFilter.h>
#include <Physics2012/Collide/Query/CastUtil/hkpWorldRayCastInput.h>
#include <Physics2012/Collide/Query/CastUtil/hkpWorldRayCastOutput.h>

// Stuff to create MOPP from TK file.
#include <Physics2012/Collide/Shape/Compound/Tree/Mopp/hkpMoppCompilerInput.h>
#include <Physics2012/Internal/Collide/Mopp/Code/hkpMoppCode.h>
#include <Physics2012/Collide/Shape/Compound/Tree/Mopp/hkpMoppUtility.h>
#include <Physics2012/Collide/Shape/Compound/Tree/Mopp/hkpMoppBvTreeShape.h>
#include <Physics2012/Collide/Shape/Compound/Collection/SimpleMesh/hkpSimpleMeshShape.h>
#include <Physics2012/Collide/Shape/Convex/Capsule/hkpCapsuleShape.h>
#include <Physics2012/Collide/Shape/Convex/Cylinder/hkpCylinderShape.h>
#include <Physics2012/Collide/Shape/Convex/ConvexVertices/hkpConvexVerticesShape.h>
#include <Physics2012/Collide/Shape/Convex/Box/hkpBoxShape.h>
#include <Physics2012/Collide/Shape/Convex/Sphere/hkpSphereShape.h>
#include <Physics2012/Collide/Shape/Compound/Collection/StorageExtendedMesh/hkpStorageExtendedMeshShape.h>
//#include <Physics2012/Collide/Shape/Compound/Collection/CompressedMesh/hkpCompressedMeshShapeBuilder.h>
#include "Physics2012/Dynamics/Entity/hkpRigidBody.h"
#include "gkHavok.h"
#include "Physics2012/Dynamics/World/hkpWorld.h"


// cc
#include <Physics2012/Collide/Shape/Convex/Capsule/hkpCapsuleShape.h>
#include <Physics2012/Dynamics/Phantom/hkpSimpleShapePhantom.h>
#include <Physics2012/Utilities/CharacterControl/CharacterProxy/hkpCharacterProxy.h>
#include "gkPhysicsTerrian.h"
#include "IResourceManager.h"
#include "ITask.h"


struct gkStaticPhysicCookingTask : public ITask
{
	IMesh* pMesh;
	IGameObjectRenderLayer* pRenderLayer;
	hkpMoppBvTreeShape* pShapeBaked;

	virtual void Complete_MT()
	{
		Vec3 scale = pRenderLayer->getScale();
		Vec3 pos = pRenderLayer->getPosition();
		Quat rot = pRenderLayer->getOrientation();

		hkpRigidBodyCinfo ci;
		ci.m_shape = pShapeBaked;
		ci.m_motionType = hkpMotion::MOTION_FIXED;
		ci.m_position = hkVector4( pos.x, pos.z, -pos.y );

		// rotate with x
		rot = Quat::CreateRotationX(DEG2RAD(-90)) * rot;

		ci.m_rotation = hkQuaternion(rot.v.x, rot.v.y, rot.v.z, rot.w );
		ci.m_collisionFilterInfo = hkpGroupFilter::calcFilterInfo( 0, 1 );
		ci.m_restitution = 0.0f;

		hkpRigidBody* entity = new hkpRigidBody(ci);
		pShapeBaked->removeReference();

		getHavokPtr()->getPhyicsWorld()->lock();
		getHavokPtr()->getPhyicsWorld()->markForWrite();

		getHavokPtr()->getPhyicsWorld()->addEntity(entity);

		getHavokPtr()->getPhyicsWorld()->unmarkForWrite();
		getHavokPtr()->getPhyicsWorld()->unlock();


		entity->removeReference();
	}

	virtual void Execute() 
	{
		pMesh->lockres();

		gkVertexBuffer* pvb = pMesh->getVertexBuffer();
		gkIndexBuffer* pib = pMesh->getIndexBuffer();
		
		Vec3 scale = pRenderLayer->getScale();
		Vec3 pos = pRenderLayer->getPosition();
		Quat rot = pRenderLayer->getOrientation();

		pShapeBaked = gkPhysicLayer::loadMOPPShape( hkVector4(scale.x,scale.y,scale.z,1), pvb, pib );

		pMesh->unlockres();
	}

};

//////////////////////////////////////////////////////////////////////////
gkPhysicLayer::gkPhysicLayer( void )
{
	m_pCharacterController = NULL;
	m_phantom = NULL;
	m_standShape = NULL;
	m_terrian = NULL;
}


//////////////////////////////////////////////////////////////////////////
bool gkPhysicLayer::createStatic()
{
	IGameObject* pObject = getParentGameObject();
	if (!pObject)
	{
		return false;
	}

	IGameObjectRenderLayer* pRenderLayer = pObject->getRenderLayer();
	if (!pRenderLayer)
	{
		return false;
	}


	const gkMeshPtr& pMesh = pRenderLayer->getMesh();
	if( pMesh.isNull())
	{
		return false;
	}

	m_phytype = ePLT_Static;

	gkStaticPhysicCookingTask* task = new gkStaticPhysicCookingTask;
	task->pMesh = pMesh.getPointer();
	task->pRenderLayer = pRenderLayer;

	pMesh->bindtask( task );

	// 这个需要单线程处理了，会崩溃 [12/5/2014 gameKnife]
	gEnv->pCommonTaskDispatcher->PushTask( task, 0 );

	return true;
}
//////////////////////////////////////////////////////////////////////////
bool gkPhysicLayer::createCharacterController()
{
	// Construct a shape
	m_phytype = ePLT_Character;

	const hkReal totalHeight = 1.7f;
	const hkReal radius = .4f;
	const hkReal capsulePoint = totalHeight*0.5f - radius;

	hkVector4 vertexA(0, capsulePoint, 0 );
	hkVector4 vertexB(0, -capsulePoint, 0 );

	// Create a capsule to represent the character standing
	m_standShape = new hkpCapsuleShape(vertexA, vertexB, radius);

	// Construct a Shape Phantom
	m_phantom = new hkpSimpleShapePhantom( m_standShape, hkTransform::getIdentity() );

	// Add the phantom to the world
	getHavokPtr()->getPhyicsWorld()->markForWrite();
	getHavokPtr()->getPhyicsWorld()->addPhantom(m_phantom);
	m_phantom->removeReference();
	
	// Construct a character proxy
	hkpCharacterProxyCinfo cpci;
	cpci.m_position.set(0, 50, 0);
	//cpci.m_position.set(-5, 20, 1);
	cpci.m_staticFriction = 0.0f;
	cpci.m_dynamicFriction = 1.0f;
	cpci.m_up.setNeg4( getHavokPtr()->getPhyicsWorld()->getGravity() );
	cpci.m_up.normalize3();
	cpci.m_userPlanes = 4;
	cpci.m_maxSlope = HK_REAL_PI / 3.0f;
	cpci.m_characterMass = 5.0f;
	cpci.m_characterStrength = 5.0f;
	cpci.m_shapePhantom = m_phantom;

	m_pCharacterController = new hkpCharacterProxy( cpci );

	getHavokPtr()->getPhyicsWorld()->unmarkForWrite();
	return true;
}
//////////////////////////////////////////////////////////////////////////
hkpMoppBvTreeShape* gkPhysicLayer::loadMOPPShape( hkVector4& scale, gkVertexBuffer* pvb, gkIndexBuffer* pib)
{
	hkpMoppBvTreeShape* pShape = HK_NULL;

	hkpSimpleMeshShape* meshStorage = new hkpSimpleMeshShape(hkConvexShapeDefaultRadius);

	uint32 numVert = pvb->elementCount;
	uint32 numTri = pib->count / 3;

		hkVector4 vertex;
		meshStorage->m_vertices.setSize(numVert);
		meshStorage->m_triangles.setSize(numTri);

		// read the vertices from the TK file
		for( uint32 i = 0; i < numVert; i++ )
		{
			float* pThisVert = (float*)(pvb->data + i * pvb->elementSize);

			vertex(0) = pThisVert[0];
			vertex(1) = pThisVert[1];
			vertex(2) = pThisVert[2];

			vertex(0) *= scale(0);
			vertex(1) *= scale(1);
			vertex(2) *= scale(2);
			vertex(3) = 0;

			meshStorage->m_vertices[i] = vertex;
		}

		meshStorage->m_vertices.setSize( numVert );
		meshStorage->m_triangles.setSize( numTri );

		for( uint32 j = 0; j < numTri; j++ )
		{
			//uint32* pThisVert = pib->data + j * 3;

			meshStorage->m_triangles[j].m_a = pib->getData(j*3 + 0);
			meshStorage->m_triangles[j].m_b = pib->getData(j*3 + 1);
			meshStorage->m_triangles[j].m_c = pib->getData(j*3 + 2);
		}


		hkpMoppCompilerInput mfr;
		mfr.setAbsoluteFitToleranceOfAxisAlignedTriangles( hkVector4( 0.1f, 0.1f, 0.1f ) );
		hkpMoppCode* pCode = hkpMoppUtility::buildCode( meshStorage, mfr );
		pShape = new hkpMoppBvTreeShape( meshStorage, pCode );

		pCode->removeReference();
		meshStorage->removeReference();

		return pShape;
}

//////////////////////////////////////////////////////////////////////////
void gkPhysicLayer::PreUpdateLayer( float fElapsedTime )
{
	if (!m_pCharacterController)
	{
		return;
	}

	getHavokPtr()->getPhyicsWorld()->markForRead();

	hkVector4 posCC = m_pCharacterController->getPosition();
	IGameObject* pParent = getParentGameObject();
	if (pParent)
	{
		pParent->setPosition(posCC(0), -posCC(2), posCC(1) - 0.9f);
		m_cachedPos = pParent->getPosition();
	}

	getHavokPtr()->getPhyicsWorld()->unmarkForRead();
}

//////////////////////////////////////////////////////////////////////////
void gkPhysicLayer::UpdateLayer( float fElpasedTime )
{

	if (!m_pCharacterController)
	{
		return;
	}

	hkStepInfo si;

	si.m_deltaTime = fElpasedTime;
	si.m_invDeltaTime = 1.0f / fElpasedTime;

	getHavokPtr()->getPhyicsWorld()->markForWrite();

		IGameObject* pParent = getParentGameObject();
		if (pParent)
		{
			Vec3 motionDesire = pParent->getPosition() - m_cachedPos;
			motionDesire *= si.m_invDeltaTime;
			hkVector4 motionDesireHavok(motionDesire.x, motionDesire.z, -motionDesire.y, 0);

			hkVector4 downDir(0,-1,0);
			hkpSurfaceInfo ground;
			
			m_pCharacterController->checkSupport(downDir, ground);

			if (ground.m_supportedState == hkpSurfaceInfo::UNSUPPORTED)
			{
				// apply gravity
				motionDesireHavok.add4(getHavokPtr()->getPhyicsWorld()->getGravity());
			}

			

			m_pCharacterController->setLinearVelocity(motionDesireHavok);
		}
		
		m_pCharacterController->integrate( si, getHavokPtr()->getPhyicsWorld()->getGravity() );

	getHavokPtr()->getPhyicsWorld()->unmarkForWrite();

}
//////////////////////////////////////////////////////////////////////////
void gkPhysicLayer::Destroy()
{
	if (m_pCharacterController)
	{
		getHavokPtr()->getPhyicsWorld()->lock();

		getHavokPtr()->getPhyicsWorld()->removePhantom(m_pCharacterController->getShapePhantom());

		m_pCharacterController->removeReference();
		m_standShape->removeReference();

		m_pCharacterController = NULL;
		getHavokPtr()->getPhyicsWorld()->unlock();
	}
	else if (m_terrian)
	{
		delete m_terrian;
	}
	else
	{

	}

	getHavokPtr()->DestroyPhysicLayer(this);
}
//////////////////////////////////////////////////////////////////////////
bool gkPhysicLayer::teleport( Vec3 worldpos )
{
	if (m_pCharacterController)
	{
		getHavokPtr()->getPhyicsWorld()->markForWrite();
		m_pCharacterController->setPosition( hkVector4( worldpos.x, worldpos.y, worldpos.z) );
		getHavokPtr()->getPhyicsWorld()->unmarkForWrite();
		return true;
	}



	return false;
}

bool gkPhysicLayer::createHeightMap(const gkStdString& heightMapName, float maxHeight, float zeroOffset )
{
	gkTexturePtr heightmap = gEnv->pSystem->getTextureMngPtr()->load(heightMapName);

	m_terrian = new gkPhysicsTerrian( heightmap, maxHeight, zeroOffset );

	//	Create terrain as a fixed rigid body
	{
		hkpRigidBodyCinfo rci;
		rci.m_motionType = hkpMotion::MOTION_FIXED;
		//rci.m_position = hkVector4(0,0,0,0);
 		rci.m_position = m_terrian->m_proxy->m_extents; // center the heighfield
 		rci.m_position.mul4( hkVector4(-0.5f, 0, -0.5f, 0) );
		rci.m_shape = m_terrian->m_proxy;
		rci.m_friction = 0.5f;

		//Quat rot = Quat::CreateRotationX( DEG2RAD( -90 ) );

		//rci.m_rotation = hkQuaternion(rot.v.x, rot.v.y, rot.v.x, rot.w );

		hkpRigidBody* terrain = new hkpRigidBody( rci );

		getHavokPtr()->getPhyicsWorld()->markForWrite();
		getHavokPtr()->getPhyicsWorld()->addEntity(terrain);
		getHavokPtr()->getPhyicsWorld()->unmarkForWrite();


		terrain->removeReference();
	}

	m_terrian->m_proxy->removeReference();

	return true;
}
