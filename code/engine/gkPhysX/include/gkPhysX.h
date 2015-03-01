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
// K&K Studio GameKnife ENGINE Source File
//
// Name:   	gkPhysX.h
// Desc:		
// 	
// Author:  Kaiming
// Date:	2012/8/12
// Modify:	
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _gkPhysX_h_
#define _gkPhysX_h_

#include "Prerequisites.h"
#include "IHavok.h"

#include "PxPhysicsAPI.h"
#include "extensions/PxExtensionsAPI.h"
#include "PhysXBase\gkPhysXStepper.h"


class gkCharacterInstance;
class gkPhysicLayer;

namespace physx
{
	class PxDefaultCpuDispatcher;
	class PxPhysics;
	class PxCooking;
	class PxScene;
	class PxGeometry;
	class PxMaterial;
	class PxRigidActor;

	class PxProfileZone;
	class PxProfileZoneManager;
	class PxProfileEventSender;

	class PxFoundation;
};

namespace physx
{
	namespace shdfnd{}

	namespace pxtask
	{
		class CudaContextManager;
	}
}
// 
// #include <foundation/PxSimpleTypes.h>
// #include <foundation/PxVec3.h>
// #include <foundation/PxMat33.h>
// #include <foundation/PxMat44.h>
// #include <foundation/PxTransform.h>
// #include <foundation/PxBounds3.h>
// #include <foundation/PxErrorCallback.h>
 

class gkPhysX	: public IPhysics
				, public PVD::PvdConnectionHandler //receive notifications when pvd is connected and disconnected.
{
public:
	gkPhysX();
	virtual ~gkPhysX(void) {}

	virtual void InitPhysics();
	virtual void DestroyPhysics();
	virtual void UpdatePhysics(float fElapsedTime);
	virtual void PostUpdatePhysics();

	virtual IGameObjectPhysicLayer* CreatePhysicLayer();
	virtual void DestroyPhysicLayer(IGameObjectPhysicLayer* target);
	virtual void DestroyAllPhysicLayer();


public:
	void setupPhysics();
	bool cookTriangleMesh(const physx::PxTriangleMeshDesc& desc, physx::PxOutputStream& stream);

public:
	physx::PxPhysics*	getPhysics() {return m_Physics;}
	physx::PxCooking*	getCooking() {return m_Cooking;}
	physx::PxScene*		getScene() {return m_Scene;}
	physx::PxMaterial*	getDefaultMaterial() {return m_Material;}

private:
	physx::PxRigidActor* createGrid();

private:
	///////////////////////////////////////////////////////////////////////////////
	// Implements PvdConnectionFactoryHandler
	virtual			void									onPvdSendClassDescriptions(PVD::PvdConnection&) {}
	virtual			void									onPvdConnected(PVD::PvdConnection& inFactory);
	virtual			void									onPvdDisconnected(PVD::PvdConnection& inFactory);
	void togglePvdConnection();
	void createPvdConnection();

private:
	physx::PxFoundation*							m_Foundation;
	physx::PxProfileZoneManager*					m_ProfileZoneManager;
	physx::PxPhysics*								m_Physics;
	physx::PxCooking*								m_Cooking;
	physx::PxScene*									m_Scene;
	physx::PxMaterial*								m_Material;
	physx::PxDefaultCpuDispatcher*					m_CpuDispatcher;

	int												mNbThreads;
	bool											mUseFullPvdConnection;

#ifdef PX_WINDOWS
	physx::pxtask::CudaContextManager*				m_CudaContextManager;
#endif

	typedef std::list<gkPhysicLayer*> PhysicLayerArray;
	PhysicLayerArray								m_PhysicLayers;

	physx::FixedStepper m_FixedStepper;
};

gkPhysX* getPhysXPtr();
// inline physx::PxMat44	Matrix44toPxMat44(const Matrix44& matrix);
// inline physx::PxVec3	Vec3toPxVec3(const Vec3& vec);
// 
// inline Vec3				PxVec3toVec3(const physx::PxVec3& vec);
// inline Quat				PxQuattoQuat(const physx::PxQuat& quat);

inline physx::PxMat44 Matrix44toPxMat44( const Matrix44& matrix )
{
	return physx::PxMat44( (float*)(matrix.GetData()) );
}

inline physx::PxVec3 Vec3toPxVec3( const Vec3& vec )
{
	return physx::PxVec3( vec.x, vec.y, vec.z );
}

inline Vec3 PxVec3toVec3( const physx::PxVec3& vec )
{
	return Vec3(vec.x, vec.y, vec.z);
}

inline Quat PxQuattoQuat( const physx::PxQuat& quat )
{
	return Quat( quat.w, quat.x, quat.y, quat.z );
}

#endif