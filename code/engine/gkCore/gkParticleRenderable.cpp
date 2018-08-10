#include "gkStableHeader.h"
#include "gkParticleRenderable.h"
#include "IGameObjectLayer.h"
#include "IParticle.h"

gkParticleRenderable::gkParticleRenderable( IParticleProxy* proxy, uint32 maxParticleCount )
{
	m_proxy = proxy;

	int rtsize = XPARA_RT_WIDTH;

	gkNameValuePairList createlist;

	TCHAR buffer[MAX_PATH];
	// set params
	createlist[_T("type")] =	_T("PARTICLE_VB");
#if defined(__x86_64__) || defined(__arm64__)
	_stprintf(buffer, _T("particle_0x%x"), (uint64)this );
#else
    _stprintf(buffer, _T("particle_0x%x"), (uint32)this );
#endif
    
	

	m_mesh = gEnv->pSystem->getMeshMngPtr()->create( buffer, _T("particle"), &createlist );
	m_mesh->load();

	// random build
	gkVertexBuffer* pVB = m_mesh->getVertexBuffer();
	gkIndexBuffer* pIB = m_mesh->getIndexBuffer();

	pVB->resizeDiscard( maxParticleCount * 4 );
	pIB->resizeDiscard( maxParticleCount * 2 );

	for (uint32 i=0; i < pVB->elementCount / 4; ++i)
	{
		GKVL_GpuParticle* vertex0 = (GKVL_GpuParticle*)(pVB->data) + i * 4 + 0;
		GKVL_GpuParticle* vertex1 = (GKVL_GpuParticle*)(pVB->data) + i * 4 + 1;
		GKVL_GpuParticle* vertex2 = (GKVL_GpuParticle*)(pVB->data) + i * 4 + 2;
		GKVL_GpuParticle* vertex3 = (GKVL_GpuParticle*)(pVB->data) + i * 4 + 3;

		// bind vtf sampler
		vertex0->texcoord1 = Vec2( (i % rtsize + 0.5) / (float)rtsize, (i / rtsize + 0.5) / (float)rtsize );
		vertex1->texcoord1 = Vec2( (i % rtsize + 0.5) / (float)rtsize, (i / rtsize + 0.5) / (float)rtsize );
		vertex2->texcoord1 = Vec2( (i % rtsize + 0.5) / (float)rtsize, (i / rtsize + 0.5) / (float)rtsize );
		vertex3->texcoord1 = Vec2( (i % rtsize + 0.5) / (float)rtsize, (i / rtsize + 0.5) / (float)rtsize );

		vertex0->texcoord0 = Vec2(0,0);
		vertex1->texcoord0 = Vec2(1,0);
		vertex2->texcoord0 = Vec2(0,1);
		vertex3->texcoord0 = Vec2(1,1);
	}

	pIB->Clear();
	uint32 totalcount = 0;
	for (uint32 i=0; i < pIB->count / 6; ++i)
	{
		pIB->push_back( totalcount );
		pIB->push_back( totalcount + 1 );
		pIB->push_back( totalcount + 2 );

		pIB->push_back( totalcount + 2 );
		pIB->push_back( totalcount + 1 );
		pIB->push_back( totalcount + 3 );

		totalcount += 4;
	}

	m_mesh->UpdateHwBuffer();
}

gkParticleRenderable::~gkParticleRenderable( void )
{

}

void gkParticleRenderable::getWorldTransforms( Matrix44* xform) const
{
	*xform = m_CachedTransform;
}

IMaterial* gkParticleRenderable::getMaterial()
{
	if (!m_pMaterial.isNull())
	{
		return m_pMaterial.getPointer();
	}

	// if not find material, find parents
	if( m_pCreator->getMaterial() )
	{
		if (m_pCreator->getMaterial()->getSubMtlCount() > 0)
		{
			return m_pCreator->getMaterial()->getSubMaterial(0);
		}
		else
		{
			return m_pCreator->getMaterial();
		}
	}

	return NULL;
}

void gkParticleRenderable::setMaterialName( const gkStdString& matName )
{
	m_wstrMatName = matName;

	m_pMaterial = gEnv->pSystem->getMaterialMngPtr()->load( m_wstrMatName );

}

const gkStdString& gkParticleRenderable::getMaterialName() const
{
	return m_wstrMatName;
}

AABB& gkParticleRenderable::getAABB()
{
	return m_mesh->GetAABB();
}

void gkParticleRenderable::getRenderOperation( gkRenderOperation& op )
{
	m_mesh->getRenderOperation( op );
}

bool gkParticleRenderable::getSkinnedMatrix( Matrix44A** pMatrixs, uint32& size )
{
	return false;
}

float gkParticleRenderable::getSquaredViewDepth( const ICamera* cam ) const
{
	return 0;
}

gkMeshPtr& gkParticleRenderable::getMesh()
{
	return m_mesh;
}

void gkParticleRenderable::setParent( IGameObjectRenderLayer* pParent )
{
	m_pCreator = pParent;
}

bool gkParticleRenderable::castShadow()
{
	return m_castShadow;
}

void gkParticleRenderable::enableShadow( bool enable )
{
	m_castShadow = enable;
}


void gkParticleRenderable::RP_Prepare()
{
	m_proxy->ApplyGpuData();
}

void gkParticleRenderable::RT_Prepare()
{
	m_CachedTransform = m_pCreator->GetWorldMatrix();
}
