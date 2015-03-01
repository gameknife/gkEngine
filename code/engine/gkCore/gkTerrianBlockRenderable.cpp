#include "gkStableHeader.h"
#include "gkTerrianBlockRenderable.h"
#include "IGameObjectLayer.h"
#include "gkTerrian.h"
#include "ITask.h"

#define MAX_HEIGHT 100.0f

struct gkTerrianUpdateTask : public ITask
{
	gkTerrianBlockRenderable* renderable;

	virtual void Execute() 
	{
		renderable->updateTerrian();
	}

	virtual void Complete_MT() 
	{
		renderable->flushHw();
	}

};

gkTerrianBlockRenderable::gkTerrianBlockRenderable(gkTerrian* creator,  gkTexturePtr& heightMap, const Vec4& region, int block, float blockUnit,  int totalWidth, int minBlock, float zeroOffset )
{
	m_zeroOffset = zeroOffset;
	m_heightMap = heightMap;
	m_region = region;
	m_block = block;
	m_totalWidth = totalWidth;
	m_blockUnit = blockUnit;
	m_minBlock = minBlock;

	float totalx = heightMap->getWidth();
	float totaly = heightMap->getHeight();

	float width = totalx * region.z;
	float height = totaly * region.w;

	// build mesh from region and heightmap
	TCHAR buffer[MAX_PATH];
	

	gkNameValuePairList createlist;
	// set params
	createlist[_T("type")] =	_T("TERRIAN_BLOCK");
	_stprintf(buffer, _T("TerrianBlock_0x%x_LOD%d_%d_%d_%d_%d"), (gk_ptr_type)creator, (int)blockUnit, (int)(region.x * 1000), (int)(region.y * 1000), (int)(region.z * 1000), (int)(region.w * 1000));

	m_mesh = gEnv->pSystem->getMeshMngPtr()->create( buffer, _T("terrian"), &createlist );
	m_mesh->load();
	m_mesh->bindResetCallback( this );

	gkTerrianUpdateTask* task = new gkTerrianUpdateTask;
	task->renderable = this;
	gEnv->pCommonTaskDispatcher->PushTask( task, -1);

	//updateTerrian();
}

void gkTerrianBlockRenderable::OnResetCallBack()
{
	gkTerrianUpdateTask* task = new gkTerrianUpdateTask;
	task->renderable = this;

	static int count = 1;
	//gkLogMessage(_T("[%d] tblock_%d_%d_%d require"), count++, (int)m_blockUnit, (int)(m_region.x * 1000), (int)(m_region.y * 1000));

	gEnv->pCommonTaskDispatcher->PushTask( task, -1);
	//updateTerrian();
}

void gkTerrianBlockRenderable::updateTerrian()
{
	//gkLogMessage(_T("tblock_%d_%d_%d update"), (int)m_blockUnit, (int)(m_region.x * 1000), (int)(m_region.y * 1000));

	uint32 vertCount = (m_block+1) * (m_block+1);
	uint32 midVertCount = m_block * m_block;
	uint32 triCount = 4 * m_block * m_block;

	m_mesh->getVertexBuffer()->resizeDiscard( vertCount + midVertCount );
	m_mesh->getIndexBuffer()->resizeDiscard( triCount * 3);

	// use heightmap to modify mesh
	gkVertexBuffer* pVB = m_mesh->getVertexBuffer();
	gkIndexBuffer* pIB = m_mesh->getIndexBuffer();

	if (!pVB || !pIB || !pVB->data || !pIB->data)
	{
		gkLogError( _T("terrian buffer missing!!!") );
		return;
	}

	{
		pIB->Clear();

		uint32 width = m_block;
		uint32 vertCount = (width+1) * (width+1);
		uint32 midVertCount = width * width;
		uint32 triCount = 4 * width * width;
		uint32 midVertStart = vertCount;

		// IB
		for (uint32 i=0; i < triCount / 4 + width; ++i)
		{
			if ( (i + 1) % (width + 1) == 0)
			{
				midVertStart--;
				continue;
			}
			// tri 0
			pIB->push_back(i + 0);
			pIB->push_back(i + 1);
			pIB->push_back(i + midVertStart);

			// tri 1
			pIB->push_back(i + 0);
			pIB->push_back(i + midVertStart);
			pIB->push_back(i + width + 1);

			// tri 2
			pIB->push_back(i + midVertStart);
			pIB->push_back(i + width + 2);
			pIB->push_back(i + width + 1);

			// tri 3
			pIB->push_back(i + midVertStart);
			pIB->push_back(i + 1);
			pIB->push_back(i + width + 2);
		}
	}


	//  [4/6/2013 Kaiming]
	{
		for (uint32 i=0; i < m_block + 1; ++i)
		{
			for (uint32 j=0; j < m_block + 1; ++j)
			{
				GKVL_P3F2F4F3* thisVertex = reinterpret_cast<GKVL_P3F2F4F3*>(pVB->data + i * pVB->elementSize * (m_block + 1) + j * pVB->elementSize);

				float u = j / (float)m_block;
				float v = i / (float)m_block;

				thisVertex->position = Vec3( (u - 0.5f) * m_block * m_blockUnit, (v - 0.5f) * m_block *m_blockUnit,0);
				thisVertex->texcoord = Vec4(u * m_region.z + m_region.x, v * m_region.w + m_region.y, 0, 0);
			}
		}

		for (uint32 i=0; i < m_block; ++i)
		{
			for (uint32 j=0; j < m_block; ++j)
			{
				GKVL_P3F2F4F3* thisVertex = reinterpret_cast<GKVL_P3F2F4F3*>(pVB->data + (m_block + 1) * (m_block + 1) * pVB->elementSize + i * pVB->elementSize * (m_block) + j * pVB->elementSize );

				float u = (j + 0.5f) / (float)m_block;
				float v = (i + 0.5f) / (float)m_block;

				thisVertex->position = Vec3( (u - 0.5f) * m_block * m_blockUnit, (v - 0.5f) * m_block * m_blockUnit,0);
				thisVertex->texcoord = Vec4(u * m_region.z + m_region.x, v * m_region.w + m_region.y, 0, 0);
			}
		}
	}

	// 生成高度
	for (uint32 i=0; i < pVB->elementCount; ++i)
	{
		GKVL_P3F2F4F3* thisVertex = reinterpret_cast<GKVL_P3F2F4F3*>(pVB->data + i * pVB->elementSize);
		float heightScale = m_heightMap->Tex2DRAW( thisVertex->texcoord.GetXY() ) * MAX_HEIGHT;

		thisVertex->position.z = heightScale;
	}

	// 对于地块的边缘一圈，我们要把第1,3,5...个顶点使用相邻两点的线性插值来计算，以消除LOD切换时造成的空洞
	// Update: 边缘的顶点，按照minBlock的规格采样。

	std::vector<float> minBlockUs;
	std::vector<float> minBlockVs;
	for (uint32 i=0; i < m_minBlock + 1; ++i)
	{
		minBlockUs.push_back( i / (float)m_minBlock * m_region.z + m_region.x);
		minBlockVs.push_back( i / (float)m_minBlock * m_region.w + m_region.y);
	}

	if (m_minBlock != m_block)
	{
		// 顶边
		for (uint32 i=0; i < m_block+ 1; i++)
		{
			GKVL_P3F2F4F3* thisVertex = reinterpret_cast<GKVL_P3F2F4F3*>(pVB->data + i * pVB->elementSize);
			float u = thisVertex->texcoord.x;
			// 找到于u匹配的两个texcoord，采样重构其height
			for (uint32 j=1; j < minBlockUs.size(); ++j)
			{
				if ( minBlockUs[j] > u )
				{
					float height0 = LocalGetHeightmap( Vec2(minBlockUs[j-1], m_region.y) ) * MAX_HEIGHT;
					float height1 = LocalGetHeightmap( Vec2(minBlockUs[j], m_region.y) ) * MAX_HEIGHT;
					float scale = (u - minBlockUs[j-1]) / (minBlockUs[j] - minBlockUs[j-1]);
					thisVertex->position.z = LerpT(height0, height1, scale  );
					break;
				}
			}
		}

		for (uint32 i=(m_block + 1) * m_block; i < (m_block+ 1) * (m_block+ 1); i++)
		{
			GKVL_P3F2F4F3* thisVertex = reinterpret_cast<GKVL_P3F2F4F3*>(pVB->data + i * pVB->elementSize);
			float u = thisVertex->texcoord.x;
			// 找到于u匹配的两个texcoord，采样重构其height
			for (uint32 j=1; j < minBlockUs.size(); ++j)
			{
				if ( minBlockUs[j] > u )
				{
					float height0 = LocalGetHeightmap( Vec2(minBlockUs[j-1], m_region.y + m_region.w) ) * MAX_HEIGHT;
					float height1 = LocalGetHeightmap( Vec2(minBlockUs[j], m_region.y + m_region.w) ) * MAX_HEIGHT;
					thisVertex->position.z = LerpT(height0, height1, (u - minBlockUs[j-1]) / (minBlockUs[j] - minBlockUs[j-1]) );
					break;
				}
			}
		}

		for ( uint32 i= 0; i < (m_block + 1) * m_block; i += (m_block + 1) )
		{
			GKVL_P3F2F4F3* thisVertex = reinterpret_cast<GKVL_P3F2F4F3*>(pVB->data + i * pVB->elementSize);
			float v = thisVertex->texcoord.y;
			// 找到于u匹配的两个texcoord，采样重构其height
			for (uint32 j=1; j < minBlockVs.size(); ++j)
			{
				if ( minBlockVs[j] > v )
				{
					float height0 = LocalGetHeightmap( Vec2(m_region.x, minBlockVs[j-1]) ) * MAX_HEIGHT;
					float height1 = LocalGetHeightmap( Vec2(m_region.x, minBlockVs[j]) ) * MAX_HEIGHT;
					thisVertex->position.z = LerpT(height0, height1, (v - minBlockVs[j-1]) / (minBlockVs[j] - minBlockVs[j-1]) );
					break;
				}
			}
		}

		for ( uint32 i= m_block; i < (m_block + 1) * (m_block + 1); i += (m_block + 1) )
		{
			GKVL_P3F2F4F3* thisVertex = reinterpret_cast<GKVL_P3F2F4F3*>(pVB->data + i * pVB->elementSize);
			float v = thisVertex->texcoord.y;
			// 找到于u匹配的两个texcoord，采样重构其height
			for (uint32 j=1; j < minBlockVs.size(); ++j)
			{
				if ( minBlockVs[j] > v )
				{
					float height0 = LocalGetHeightmap( Vec2(m_region.x + m_region.z, minBlockVs[j-1]) ) * MAX_HEIGHT;
					float height1 = LocalGetHeightmap( Vec2(m_region.x + m_region.z, minBlockVs[j]) ) * MAX_HEIGHT;
					thisVertex->position.z = LerpT(height0, height1, (v - minBlockVs[j-1]) / (minBlockVs[j] - minBlockVs[j-1]) );
					break;
				}
			}
		}
	}
	else
	{
		//gkLogMessage(_T("Create Back LOD TerrianBlock") );
	}

	for (uint32 i=0; i < pVB->elementCount; ++i)
	{
		GKVL_P3F2F4F3* thisVertex = reinterpret_cast<GKVL_P3F2F4F3*>(pVB->data + i * pVB->elementSize);
		thisVertex->position.z -= m_zeroOffset;
	}

	// 底边
	if (1)
	{
		for (uint32 i=0; i < pVB->elementCount; ++i)
		{
			GKVL_P3F2F4F3* thisVertex = reinterpret_cast<GKVL_P3F2F4F3*>(pVB->data + i * pVB->elementSize);
			float halfBlock = 0.5f * m_blockUnit / (float)m_totalWidth;

			{
				float heightScaleLeft = m_heightMap->Tex2DRAW( Vec2(thisVertex->texcoord.x - halfBlock, thisVertex->texcoord.y) );
				float heightScaleRight = m_heightMap->Tex2DRAW( Vec2(thisVertex->texcoord.x + halfBlock, thisVertex->texcoord.y) );
				Vec3 dir = Vec3(1 * m_blockUnit,0, (heightScaleRight - heightScaleLeft) * MAX_HEIGHT );
				thisVertex->tangent_ti = Vec4( dir.GetNormalized(), 1 );
			}

			{
				float heightScaleLeft = m_heightMap->Tex2DRAW( Vec2(thisVertex->texcoord.x, thisVertex->texcoord.y + halfBlock) );
				float heightScaleRight = m_heightMap->Tex2DRAW( Vec2(thisVertex->texcoord.x, thisVertex->texcoord.y - halfBlock) );
				Vec3 dir = Vec3(0,-1 * m_blockUnit, (heightScaleRight - heightScaleLeft) * MAX_HEIGHT );
				thisVertex->binormal = -dir.GetNormalized();
			}

			
		}
	}
	else
	{
		// generate tangent & binormal
		for (uint32 i=0; i < m_block + 1; ++i)
		{
			for (uint32 j=0; j < m_block + 1; ++j)
			{
				int row = i + 1;
				int line = j + 1;
				int rowmul = 1;
				int linemul = 1;
				if (i == m_block)
				{
					row = i - 1;
					rowmul = -1;
				}
				if (j == m_block)
				{
					line = j - 1;
					linemul = -1;
				}
				GKVL_P3F2F4F3* thisVertex = reinterpret_cast<GKVL_P3F2F4F3*>(pVB->data + i * pVB->elementSize * (m_block + 1) + j * pVB->elementSize);
				GKVL_P3F2F4F3* rightVertex = reinterpret_cast<GKVL_P3F2F4F3*>(pVB->data + i * pVB->elementSize * (m_block + 1) + line * pVB->elementSize);
				GKVL_P3F2F4F3* downVertex = reinterpret_cast<GKVL_P3F2F4F3*>(pVB->data + row * pVB->elementSize * (m_block + 1) + j * pVB->elementSize);

				thisVertex->tangent_ti = Vec4( rowmul * (downVertex->position - thisVertex->position).GetNormalized(), 1 );
				thisVertex->binormal = linemul * -(rightVertex->position - thisVertex->position).GetNormalized();
			}
		}
	}
}


void gkTerrianBlockRenderable::flushHw()
{
	m_mesh->UpdateHwBuffer();
	//gkLogMessage(_T("tblock_%d_%d_%d release"), (int)m_blockUnit, (int)(m_region.x * 1000), (int)(m_region.y * 1000));
	m_mesh->ReleaseSysBuffer();
}

gkTerrianBlockRenderable::~gkTerrianBlockRenderable(void)
{
	m_mesh->unload();
	gEnv->pSystem->getMeshMngPtr()->remove( m_mesh->getHandle() );
}

void gkTerrianBlockRenderable::getWorldTransforms( Matrix44* xform ) const
{
	*xform = m_CachedTransform;
}

IMaterial* gkTerrianBlockRenderable::getMaterial()
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

void gkTerrianBlockRenderable::setMaterialName( const gkStdString& matName )
{
	m_wstrMatName = matName;
}

const gkStdString& gkTerrianBlockRenderable::getMaterialName() const
{
	return m_wstrMatName;
}

AABB& gkTerrianBlockRenderable::getAABB()
{
	return m_mesh->GetAABB();
}

void gkTerrianBlockRenderable::getRenderOperation( gkRenderOperation& op )
{
	m_mesh->getRenderOperation( op );
}

bool gkTerrianBlockRenderable::getSkinnedMatrix( Matrix44A** pMatrixs, uint32& size )
{
	return false;
}

float gkTerrianBlockRenderable::getSquaredViewDepth( const ICamera* cam ) const
{
	return 0;
}

gkMeshPtr& gkTerrianBlockRenderable::getMesh()
{
	return m_mesh;
}

void gkTerrianBlockRenderable::setParent( IGameObjectRenderLayer* pParent )
{
	m_pCreator = pParent;
}

bool gkTerrianBlockRenderable::castShadow()
{
	return m_castShadow;
}

void gkTerrianBlockRenderable::enableShadow( bool enable )
{
	m_castShadow = enable;
}

void gkTerrianBlockRenderable::RT_Prepare()
{
	m_CachedTransform = m_pCreator->GetWorldMatrix();
}

float gkTerrianBlockRenderable::LocalGetHeightmap( const Vec2& texcoord )
{
	//return m_heightMap->Tex2DRAW( Vec2(texcoord.x * m_region.z + m_region.x, texcoord.y * m_region.w + m_region.y) );
	return m_heightMap->Tex2DRAW( texcoord );
}


