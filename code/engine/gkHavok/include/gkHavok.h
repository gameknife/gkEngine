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
// Name:   	gkHavok.h
// Desc:		
// 	
// Author:  Kaiming
// Date:	2012/3/6
// Modify:	2012/3/6
// 
//////////////////////////////////////////////////////////////////////////

#pragma once
#include "Prerequisites.h"
#include "IHavok.h"
#include "IAnimation.h"


#include <Common/Base/hkBase.h>

#include <Common/Base/Memory/System/Util/hkMemoryInitUtil.h>
#include <Common/Base/Memory/Allocator/Malloc/hkMallocAllocator.h>

#include <Common/Base/Container/String/hkStringBuf.h>
#include <Common/Base/Ext/hkBaseExt.h>

class hkLoader;

class hkpWorld;
class hkpRigidBody;
class hkpWorld;
class hkpPhysicsContext;
class hkVisualDebugger;

class hkJobThreadPool;
class hkJobQueue;

class hkMemoryRouter;
class hkProcessContext;
class hkpConvexShape;
class gkCharacterInstance;

struct IMeshLoader;
class gkPhysicLayer;
class gkHavokLoader;

class gkHavok : public IPhysics
{
public:
	gkHavok();
	virtual ~gkHavok(void);

	virtual void InitPhysics();
	virtual void DestroyPhysics();
	virtual void UpdatePhysics(float fElapsedTime);

	virtual IGameObjectPhysicLayer* CreatePhysicLayer();
	virtual void DestroyPhysicLayer(IGameObjectPhysicLayer* target);
	virtual void DestroyAllPhysicLayer();

	void setupPhysics(hkpWorld* physicsWorld);
	void createBrickWall( hkpWorld* world, int height, int length, const hkVector4& position, hkReal gapWidth, hkpConvexShape* box, hkVector4Parameter halfExtents );

	hkpWorld* getPhyicsWorld() {return physicsWorld;}

	virtual void HavokThreadInit();
	virtual void HavokThreadQuit();

private:

	//////////////////////////////////////////////////////////////////////////
	// memory and threading objects
	hkMemoryRouter* memoryRouter;

	hkJobThreadPool* threadPool;
	hkJobQueue* jobQueue ;
	hkVisualDebugger* vdb;

	// physics world
	hkpWorld* physicsWorld;

	hkArray<hkProcessContext*> contexts;
	hkpPhysicsContext* context;

	typedef std::list<gkPhysicLayer*> PhysicLayerArray;
	PhysicLayerArray m_vecPhysicLayers;

	// loader
	hkLoader*			m_havokLoader;
};

gkHavok* getHavokPtr();

