#include "gkStableHeader.h"
#include "gkVegetationBlockRenderable.h"
#include "IGameObjectLayer.h"
#include "gkTerrian.h"
#include "ITask.h"

int gkVegetationBlockRenderable::m_createCounter = 0;

struct gkVegetationUpdateTask : public ITask
{
	gkVegetationBlockRenderable* renderable;

	virtual void Execute() 
	{
		renderable->updateVeg();
	}

	virtual void Complete_MT() 
	{
		renderable->flushHw();
	}

};


gkVegetationBlockRenderable::gkVegetationBlockRenderable(gkTerrian* terrian, const Vec4& region, int count, int segement, int x, int y)
{
	m_count = count;
	m_segment = segement;
	m_region = region;
	m_terrian = terrian;

	TCHAR buffer[MAX_PATH];

	gkNameValuePairList createlist;
	// set params
	createlist[_T("type")] =	_T("GRASS_BLOCK");
	_stprintf(buffer, _T("Veg_0x%x_%d_%d"), (gk_ptr_type)terrian, x, y);

	m_mesh = gEnv->pSystem->getMeshMngPtr()->create( buffer, _T("terrian"), &createlist );
	m_mesh->load();
	m_mesh->bindResetCallback(this);



	int vertcount = count * (segement+1) * 2;
	int tricount = count * 2 * segement;

	m_mesh->getVertexBuffer()->resizeDiscard( vertcount );
	m_mesh->getIndexBuffer()->resizeDiscard( tricount * 3 );

	gkVegetationUpdateTask* task = new gkVegetationUpdateTask;
	task->renderable = this;
	gEnv->pCommonTaskDispatcher->PushTask( task, -1);
	//updateVeg();	
}

void gkVegetationBlockRenderable::OnResetCallBack()
{
	gkVegetationUpdateTask* task = new gkVegetationUpdateTask;
	task->renderable = this;
	gEnv->pCommonTaskDispatcher->PushTask( task, -1);
	//updateVeg();
}

gkVegetationBlockRenderable::~gkVegetationBlockRenderable(void)
{
	m_mesh->unload();
	gEnv->pSystem->getMeshMngPtr()->remove( m_mesh->getHandle() );
}

void gkVegetationBlockRenderable::getWorldTransforms( Matrix44* xform) const
{
	*xform = m_CachedTransform;
}

IMaterial* gkVegetationBlockRenderable::getMaterial()
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

void gkVegetationBlockRenderable::setMaterialName( const gkStdString& matName )
{
	m_wstrMatName = matName;
}

const gkStdString& gkVegetationBlockRenderable::getMaterialName() const
{
	return m_wstrMatName;
}

AABB& gkVegetationBlockRenderable::getAABB()
{
	return m_mesh->GetAABB();
}

void gkVegetationBlockRenderable::getRenderOperation( gkRenderOperation& op )
{
	m_mesh->getRenderOperation( op );
}

bool gkVegetationBlockRenderable::getSkinnedMatrix( Matrix44A** pMatrixs, uint32& size )
{
	return false;
}

float gkVegetationBlockRenderable::getSquaredViewDepth( const ICamera* cam ) const
{
	return 0;
}

gkMeshPtr& gkVegetationBlockRenderable::getMesh()
{
	return m_mesh;
}

void gkVegetationBlockRenderable::setParent( IGameObjectRenderLayer* pParent )
{
	m_pCreator = pParent;
}

bool gkVegetationBlockRenderable::castShadow()
{
	return m_castShadow;
}

void gkVegetationBlockRenderable::enableShadow( bool enable )
{
	m_castShadow = enable;
}

void gkVegetationBlockRenderable::RT_Prepare()
{
	m_CachedTransform = m_pCreator->GetWorldMatrix();
}


void gkVegetationBlockRenderable::createVeg( uint32 index, const Vec2& texcoord  )
{
	// use heightmap to modify mesh
	gkVertexBuffer* pVB = m_mesh->getVertexBuffer();
	GKVL_P3F2F4F3* firstVertOfStrand = (GKVL_P3F2F4F3*)(pVB->data + index * (m_segment+1) * 2 * pVB->elementSize);

	Vec3 pos(Random(-1.f, 1.f), Random(-1.f, 1.f), 0);
	Vec3 dir(Random(-1.f, 1.f), Random(-1.f, 1.f), 0);
	dir.Normalize();

	pos.x = texcoord.x + 0.02f * pos.x;
	pos.y = texcoord.y + 0.02f * pos.y;

	Vec3 flipDir(1,0,0);
	float flip = flipDir.dot(dir);
	if (flip < 0)
	{
		flip = -1;
	}
	else
	{
		flip = 1;
	}

	int texIndex = Random(3);
	float startx = texIndex / 6.f;
	for ( uint32 node =0; node < m_segment + 1; ++node)
	{
		GKVL_P3F2F4F3* left = firstVertOfStrand++;
		GKVL_P3F2F4F3* right = firstVertOfStrand++;

		left->texcoord = Vec4(startx, node / (float)m_segment, 0, 0);
		right->texcoord = Vec4(startx + 1/6.f, node / (float)m_segment, 0, 0);

		left->position = Vec3( pos + dir * 0.01f );
		right->position = Vec3( pos - dir * 0.01f );

		left->tangent_ti.w = flip;
		right->tangent_ti.w = flip;

		left->tangent_ti = Vec4( dir, flip );
		right->tangent_ti = Vec4( dir, flip );

		left->binormal = Vec3(0,0,1);
		right->binormal = Vec3(0,0,1);


		left->position.z = 1 - left->texcoord.y;
		right->position.z = 1 - left->texcoord.y;
	}
}

void gkVegetationBlockRenderable::updateVeg()
{
	// IB
	gkIndexBuffer* pIB = m_mesh->getIndexBuffer();
	pIB->Clear();
	int tricount = m_count * 2 * m_segment;

	for (int i=0; i < tricount / 2 + m_count; i++)
	{
		if ( (i + 1) % (m_segment + 1) == 0)
		{
			continue;
		}

		// tri 0
		pIB->push_back(i * 2 + 0);
		pIB->push_back(i * 2 + 1);
		pIB->push_back(i * 2 + 2);

		// tri 1
		pIB->push_back(i * 2 + 1);
		pIB->push_back(i * 2 + 3);
		pIB->push_back(i * 2 + 2);
	}


	// use heightmap to modify mesh
	gkVertexBuffer* pVB = m_mesh->getVertexBuffer();
	memset(pVB->data, 0, pVB->getSize());

	// 遍历这个block，取得植被密度生长草
	m_realCount = 0;

	uint32 blockWidth = m_terrian->GetSize() * m_region.z;
	// 重建XY位置


	bool avaliable = true;
	for ( uint32 y=0; y < blockWidth + 1 && avaliable; y += 2 )
	{
		for ( uint32 x=0; x < blockWidth + 1 && avaliable; x += 2 )
		{
			Vec2 texcoord(y / (float)blockWidth, x / (float)blockWidth);
			float density = m_terrian->GetVegetationDensity( texcoord, m_region );

			int iterateCount = density * m_count / 500;

			for (int i=0; i < iterateCount; ++i)
			{
				createVeg( m_realCount++, texcoord );

				if (m_realCount >= m_count)
				{
					avaliable = false;
					break;
				}
			}
		}
	}

	// 生成高度
	for (uint32 i=0; i < m_realCount * 2 * (m_segment + 1); ++i)
	{
		GKVL_P3F2F4F3* thisVertex = reinterpret_cast<GKVL_P3F2F4F3*>(pVB->data + i * pVB->elementSize);
		float height = m_terrian->GetHeight( Vec2(thisVertex->position.x, thisVertex->position.y), m_region );
		thisVertex->position.z +=height;
	}
	blockWidth = m_terrian->GetSize() * m_region.z;
	
	for (uint32 i=0; i < m_realCount * 2 * (m_segment + 1); ++i)
	{
		GKVL_P3F2F4F3* thisVertex = reinterpret_cast<GKVL_P3F2F4F3*>(pVB->data + i * pVB->elementSize);
		
		thisVertex->position.x = (thisVertex->position.x - 0.5f) * blockWidth;
		thisVertex->position.y = (thisVertex->position.y - 0.5f) * blockWidth;
	}

	m_mesh->modifyCustomPrimitiveCount( m_realCount * m_segment * 2 );


}

void gkVegetationBlockRenderable::flushHw()
{
	m_mesh->UpdateHwBuffer();
	m_mesh->ReleaseSysBuffer();
}


