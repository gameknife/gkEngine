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
// Name:   	gkPhysicsLayer.h
// Desc:		
// 	
// Author:  Kaiming
// Date:	2012/8/11
// Modify:	
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _gkPhysicsLayer_h_
#define _gkPhysicsLayer_h_


#include "IGameObjectLayer.h"


class gkPhysicLayer : public IGameObjectPhysicLayer
{
public:
	gkPhysicLayer(void);
	virtual ~gkPhysicLayer(void) {}

	virtual void PreUpdateLayer( float fElapsedTime );
	virtual void UpdateLayer(float fElpasedTime);
	virtual void Destroy();

	virtual bool teleport(Vec3 worldpos);

	// create static physics based on mesh
	virtual bool createStatic();
	virtual bool createDynamic(EPhysicDynamicType type);
	virtual bool createHeightMap(const gkStdString& heightMapName, float maxHeight) {return false;}
	// create a cc based on charamesh
	virtual bool createCharacterController();

	virtual EPhysicLayerType getPhysicType() {return m_phytype;}

private:
	PxRigidDynamic* createSphere(const PxVec3& pos, PxReal radius, const PxVec3* linVel, PxReal density);
private:

	EPhysicLayerType m_phytype;
	physx::PxRigidStatic* m_staticActor;
	physx::PxRigidDynamic* m_dynamicActor;
	Vec3 m_cachedPos;
};


#endif


