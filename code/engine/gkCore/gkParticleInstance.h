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
// Name:   	gkParticleInstance.h
// Desc:	
// 	
// 
// Author:  Kaiming
// Date:	2013/4/7	
// 
//////////////////////////////////////////////////////////////////////////

#ifndef gkParticleInstance_h__
#define gkParticleInstance_h__

#include "gkPrerequisites.h"
#include "IGameObjectLayer.h"
#include "gkMovableObject.h"

struct IParticleProxy;
class gkParticleRenderable;
struct IGameObjectRenderLayer;

class gkParticleInstance : public gkMovableObject, public IGameObjectParticleLayer
{
public:
	gkParticleInstance(const gkStdString& name);
	~gkParticleInstance();

	virtual void _updateRenderSequence( IRenderSequence* queue );

	virtual const AABB& getAABB( void ) const;

	virtual const gkStdString& getMovableType() const;

	virtual void bindRenderLayer( IGameObjectRenderLayer* renderLayer ) {m_renderLayer = renderLayer;}

	//////////////////////////////////////////////////////////////////////////
	// override IGameObjectParticleLayer
	virtual void loadPtcFile( const TCHAR* filename );

	virtual void PreUpdateLayer( float fElapsedTime );

	virtual void UpdateLayer( float fElpasedTime );

	virtual void Destroy();


private:
	IParticleProxy* m_proxy;
	gkParticleRenderable* m_renderable;
	IGameObjectRenderLayer* m_renderLayer;
};

/** Factory object for creating Light instances */
class gkParticleFactory : public gkMovableObjectFactory
{
protected:
	gkMovableObject* createInstanceImpl( const gkStdString& name, const NameValuePairList* params, void* userdata);
public:
	gkParticleFactory() {}
	~gkParticleFactory() {}

	static gkStdString FACTORY_TYPE_NAME;

	const gkStdString& getType(void) const;
	void destroyInstance( gkMovableObject* obj);  

};
#endif // gkParticleInstance_h__
