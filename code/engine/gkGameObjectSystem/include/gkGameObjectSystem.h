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
// Name:   	gkGameObject.h
// Desc:		
// 	
// Author:  Kaiming
// Date:	2012/3/8
// Modify:	2012/3/8
// 
//////////////////////////////////////////////////////////////////////////

#pragma once
#include "Prerequisites.h"
#include "IGameObject.h"

struct CRapidXmlParseNode;

class gkGameObjectSystem : public IGameObjectSystem
{
public:
	gkGameObjectSystem();
	virtual ~gkGameObjectSystem();

	virtual IGameObject* CreateStaticGeoGameObject( const gkStdString& meshfile, Vec3& pos, Quat& rot);
	virtual IGameObject* CreateVoidGameObject( Vec3& pos, Quat& rot);
	virtual IGameObject* CreateLightGameObject( Vec3& lightpos, float lightRadius, ColorF& lightColor, bool isFakeShadow = false);
	virtual IGameObject* CreateAnimGameObject( const gkStdString& chrfile, Vec3& pos, Quat& rot);
	virtual IGameObject* CreateParticleGameObject( const gkStdString& ptcFilename, const Vec3& pos, const Quat& rot );

	virtual IGameObject* CreateStaticGeoGameObject(const gkStdString& name, const gkStdString& meshfile, Vec3& pos, Quat& rot);
	virtual IGameObject* CreateVoidGameObject(const gkStdString& name, Vec3& pos, Quat& rot);
	virtual IGameObject* CreateLightGameObject(const gkStdString& name, Vec3& lightpos, float lightRadius, ColorF& lightColor, bool isFakeShadow);
	virtual IGameObject* CreateAnimGameObject(const gkStdString& name, const gkStdString& chrfile, Vec3& pos, Quat& rot);
	virtual IGameObject* CreateParticleGameObject(const gkStdString& name, const gkStdString& ptcFilename, const Vec3& pos, const Quat& rot );

	virtual IGameObject* CreateStaticGeoGameObject(CRapidXmlParseNode* node);
	virtual IGameObject* CreateVoidGameObject(CRapidXmlParseNode* node);
	virtual IGameObject* CreateLightGameObject(CRapidXmlParseNode* node);
	virtual IGameObject* CreateAnimGameObject(CRapidXmlParseNode* node);

	virtual IGameObject* CreateGameObjectFromXml(CRapidXmlParseNode* node);

	virtual bool DestoryGameObject(IGameObject* pTarget);
	virtual bool DestoryGameObject(uint32 id);

	bool DestroyGameObjectImm( uint32 id );

	virtual uint32 getGameObjectCount();

	virtual IGameObject* GetGameObjectById(uint32 goID);
	virtual IGameObject* GetGameObjectByName(const gkStdString& name);

	virtual bool syncupdate();
	virtual void preupdate(float fElapsedTime);
	virtual void update(float fElapsedTime);
	virtual void destroy();
	
	virtual const IGameObjectQueue& GetGameObjects() const {return (const IGameObjectQueue&)m_queGameObject;}

public:

	gkGameObject* _createGameObjectInternal(const gkStdString& name);
	uint32 _findPosInQueue();

private:
	gkGameObjectQueue m_queGameObject;
	gkGameObjectNameMap m_mapGameObject;
	
	std::vector<uint32> m_destroyList;
	gkNameGenerator* m_MovableNameGenerator;
};

