#include "StableHeader.h"
#include "gkPhysicsLayer.h"
#include "IMesh.h"
#include "gkPhysxStream.h"
#include "PhysXBase\gkPhysXUtil.h"

gkPhysicLayer::gkPhysicLayer( void ):
	m_staticActor(NULL),
	m_dynamicActor(NULL)
{

}

void gkPhysicLayer::PreUpdateLayer( float fElapsedTime )
{
	// pre async physx pos to gameobject
	IGameObject* pGameObject = getParentGameObject();

	if (!pGameObject)
	{
		return;
	}

	if (m_dynamicActor)
	{
		PxTransform pose = m_dynamicActor->getGlobalPose();

		pGameObject->setPosition( PxVec3toVec3( pose.p ) );
		pGameObject->setOrientation( PxQuattoQuat( pose.q ) );
	}
}

void gkPhysicLayer::UpdateLayer( float fElpasedTime )
{

}

void gkPhysicLayer::Destroy()
{
	getPhysXPtr()->DestroyPhysicLayer(this);
}

bool gkPhysicLayer::teleport( Vec3 worldpos )
{
	return true;
}

bool gkPhysicLayer::createStatic()
{
	// TODO: prebaked .cooked file
	//return true;
	// get meshres
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

	// TODO: may create more static for multi subentity
	const gkMeshPtr& pMesh = pRenderLayer->getMesh();
	if( pMesh.isNull())
	{
		return false;
	}

	// get filename and output file name
	//gkStdString cookfilename = pMesh->getName();
	//cookfilename.append(_T(".cooked"));

	// we get the mesh, start baking
	uint32 nNumVerts = pMesh->getNumVerts();
	uint32 nNumFaces = pMesh->getNumFaces();
	uint32 nVertSize = pMesh->getNumBytesVerts();
	
	void* pVexStart = pMesh->getVertexBuffer();
	void* pIndStart = pMesh->getIndexBuffer();

	Vec3 scale = pRenderLayer->getScale();
	Vec3 pos = pRenderLayer->getPosition();
	Quat rot = pRenderLayer->getOrientation();

	physx::PxTriangleMeshDesc meshDesc;
	meshDesc.points.count		= nNumVerts;
	meshDesc.triangles.count	= nNumFaces;
	meshDesc.points.stride		= nVertSize;
	meshDesc.triangles.stride	= sizeof(DWORD)*3;
	meshDesc.points.data		= pVexStart;
	meshDesc.triangles.data		= pIndStart;

	//
	//printf("Cooking object... %s", filePathCooked);
	//FileOutputStream stream( gkGetAbsGamePath( cookfilename ).c_str() );

	MemoryOutputStream writeBuffer;
	bool status = getPhysXPtr()->getCooking()->cookTriangleMesh(meshDesc, writeBuffer);
	if(!status)
		return NULL;

	MemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());

	PxTriangleMesh* triangleMesh = getPhysXPtr()->getPhysics()->createTriangleMesh(readBuffer);
	if(triangleMesh)
	{
		PxTriangleMeshGeometry triGeom;
		triGeom.triangleMesh = triangleMesh;

		

		PxTransform transform = PxTransform(Matrix44toPxMat44(pRenderLayer->GetWorldMatrix()));
		m_staticActor = getPhysXPtr()->getPhysics()->createRigidStatic(transform);

		PxShape* shape = m_staticActor->createShape(triGeom, *(getPhysXPtr()->getDefaultMaterial()));
		getPhysXPtr()->getScene()->addActor(*m_staticActor);
		//mPhysicsActors.push_back(actor);
	}


	//return physics.createTriangleMesh(readBuffer);

	//bool ok = getPhysXPtr()->cookTriangleMesh(meshDesc, stream);
	//printf(" - Done\n");

	return true;
}

bool gkPhysicLayer::createCharacterController()
{
	return true;
}

PxRigidDynamic* gkPhysicLayer::createSphere(const PxVec3& pos, PxReal radius, const PxVec3* linVel, PxReal density)
{
	PxRigidDynamic* ret = NULL;
	ret = PxCreateDynamic(*(getPhysXPtr()->getPhysics()), PxTransform(pos), PxSphereGeometry(radius), *(getPhysXPtr()->getDefaultMaterial()), density);
	PX_ASSERT(ret);

	SetupDefaultRigidDynamic(*ret);
	getPhysXPtr()->getScene()->addActor(*ret);

	if(linVel)
		ret->setLinearVelocity(*linVel);

	return ret;
}

bool gkPhysicLayer::createDynamic( EPhysicDynamicType type )
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

	
	switch(type)
	{
	case ePDT_Sphere:
		{
			PxVec3 pos = Vec3toPxVec3( pRenderLayer->getPosition() );
			f32 radius = pRenderLayer->getAABB().GetRadius();
			PxVec3 vel = PxVec3(0,0,0);
			m_dynamicActor = createSphere(pos, radius, &vel, 1);
			break;
		}
	default:
		break;
	}

	return true;
}
