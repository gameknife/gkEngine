#include "gkStableHeader.h"
#include "gkParticleInstance.h"
#include "gkParticleRenderable.h"
#include "IRenderSequence.h"
#include "gk3DEngine.h"
#include "gkSceneManager.h"
#include "IParticle.h"


gkParticleInstance::gkParticleInstance(const gkStdString& name)
{
	m_renderLayer = NULL;
}

gkParticleInstance::~gkParticleInstance()
{

}

void gkParticleInstance::loadPtcFile( const TCHAR* filename )
{
	m_proxy = gEnv->pRenderer->createGPUParticleProxy();
	m_proxy->createFromXmlNode( NULL );

	m_renderable = new gkParticleRenderable( m_proxy, 100000 );
	m_renderable->setParent( m_renderLayer );

	m_renderLayer->setMaterialName( _T("engine/assets/meshs/particle.mtl") );

	setRenderLayer( RENDER_LAYER_TRANSPARENT );


	
	// 
// 	gkMaterialPtr defaultmtl = gEnv->pSystem->getMaterialMngPtr()->load( IMAT_DEFALUTNAME );
// 	m_renderable->setMaterialName()
}

void gkParticleInstance::PreUpdateLayer( float fElapsedTime )
{
	
}

void gkParticleInstance::UpdateLayer( float fElpasedTime )
{
	
}

void gkParticleInstance::Destroy()
{
	delete m_renderable;

	gEnv->pRenderer->destroyGPUParticelProxy( m_proxy );

	// remove me
	getSceneMngPtr()->destroyMovableObject(this);
}

void gkParticleInstance::_updateRenderSequence( IRenderSequence* queue )
{
	queue->addToRenderSequence( m_renderable, RENDER_LAYER_TRANSPARENT );
	queue->addToRenderSequence( m_renderable, RENDER_LAYER_AFTERDOF );
	
}

const AABB& gkParticleInstance::getAABB( void ) const
{
	m_WorldAABB.Reset();
	m_WorldAABB.Add(Vec3(0,0,0), 1);

	return m_WorldAABB;
}

const gkStdString& gkParticleInstance::getMovableType() const
{
	return gkParticleFactory::FACTORY_TYPE_NAME;
}


//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
gkStdString gkParticleFactory::FACTORY_TYPE_NAME = _T("Particle");
//-----------------------------------------------------------------------
const gkStdString& gkParticleFactory::getType(void) const
{
	return FACTORY_TYPE_NAME;
}
//-----------------------------------------------------------------------
gkMovableObject* gkParticleFactory::createInstanceImpl( const gkStdString& name, 
	const NameValuePairList* params,
	void* userdata)
{
	gkParticleInstance* particle = new gkParticleInstance(name);
	return particle;
}
//-----------------------------------------------------------------------
void gkParticleFactory::destroyInstance( gkMovableObject* obj)
{
	SAFE_DELETE( obj );
}
