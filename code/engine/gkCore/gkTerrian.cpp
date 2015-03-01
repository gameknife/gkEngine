#include "gkStableHeader.h"
#include "gkTerrian.h"
#include "gkTerrianBlockRenderable.h"
#include "gkSceneNode.h"
#include "gkSceneManager.h"
#include "gk3DEngine.h"
#include "gkEntity.h"
#include "ICamera.h"
#include "gkVegetationBlockRenderable.h"
#include "IHavok.h"
#include "IXmlUtil.h"

#define TERRIAN_BLOCK 1

gkTerrian::gkTerrian(void)
{
	m_zeroOffset = 20.0;
}


gkTerrian::~gkTerrian(void)
{
}


void gkTerrian::CreateTerrianBlock()
{
	for (uint32 i=0; i < m_blockSize; ++i)
	{
		for (uint32 j=0; j < m_blockSize; ++j)
		{
			gkTerrianBlockEntityGroup group;

			if( m_sideWidth / m_blockSize >= 8 )
			{
 				group.lods[0] = CreateLod(j, i, 1, 8);
 				group.lods[1] = CreateLod(j, i, 2, 8);
 				group.lods[2] = CreateLod(j, i, 4, 8);
 				group.lods[3] = CreateLod(j, i, 8, 8);

				group.m_lodCount = 4;
			}
			else if ( m_sideWidth / m_blockSize >= 4 )
			{
 				group.lods[0] = CreateLod(j, i, 1,4);
 				group.lods[1] = CreateLod(j, i, 2,4);
				group.lods[2] = CreateLod(j, i, 4,4);

				group.m_lodCount = 3;
			}
			else
			{
				gkLogError(_T("地形块创建失败，LOD分割错误。"));
			}

			Vec4 region;
			region.x = j / (float)m_blockSize;
			region.y = i / (float)m_blockSize;
			region.z = 1.f / (float)m_blockSize;
			region.w = 1.f / (float)m_blockSize;

			group.vegetation = CreateVeg( j, i, 2500, 1 );

			m_terrianBlocks.push_back(group);
		}

	}
}

void gkTerrian::Create( const gkStdString& heightMapName, int blockSide, int sideWidth )
{
	m_material = gEnv->pSystem->getMaterialMngPtr()->load(_T("engine/assets/meshs/terrian.mtl"));
	m_heightMap = gEnv->pSystem->getTextureMngPtr()->load(heightMapName);

	m_blockSize = blockSide;
	m_sideWidth = sideWidth;

	CreateTerrianBlock();

	m_water = CreateWater(-1.0);

	gkLogMessage(_T("terrian load success."));

	for (uint32 i=0; i < m_terrianBlocks.size(); ++i)
	{
		m_terrianBlocks[i].SwitchLod(1);
	}

#ifdef OS_WIN32
	if (gEnv->pPhysics)
	{
		m_phyLayer = gEnv->pPhysics->CreatePhysicLayer();
		//m_phyLayer->setParentGameObject(NULL);
		m_phyLayer->createHeightMap( heightMapName, 100.f );
	}
#endif
}

void gkTerrian::Create( CRapidXmlParseNode* node )
{
	int blocksize;
	node->GetAttribute( _T("blocksize"), blocksize );
	int sidewidth;
	node->GetAttribute( _T("sidewidth"), sidewidth );

	gkStdString heightmap =	node->GetAttribute( _T("heightmap") );
	gkStdString vegetation_map =	node->GetAttribute( _T("vegetation_map") );
	gkStdString area_map =	node->GetAttribute( _T("area_map") );
	gkStdString color_map =	node->GetAttribute( _T("color_map") );
	gkStdString detail_map =	node->GetAttribute( _T("detail_map") );
	gkStdString detail_normal_map =	node->GetAttribute( _T("detail_normal_map") );
	gkStdString detail_spec_map =	node->GetAttribute( _T("detail_spec_map") );


	
	ModifyVegetationDensityMap(vegetation_map);
	Create( heightmap, blocksize, sidewidth );

	ModifyColorMap(color_map);
	ModifyAreaMap(area_map);
	ModifyDetailMap(detail_map, 0);
	ModifyDetailMap(detail_normal_map, 1);
	ModifyDetailMap(detail_spec_map, 2);
}

void gkTerrian::Save( CRapidXmlAuthorNode* node )
{
	node->AddAttribute( _T("blocksize"), (int)m_blockSize );
	node->AddAttribute( _T("sideWidth"), (int)m_sideWidth );
	if (m_heightMap.isNull())
	{
		gkLogError( _T("terrian heightmap missing, save failed!") );
		return;
	}
	node->AddAttribute( _T("heightmap"), m_heightMap->getName().c_str() );
	node->AddAttribute( _T("vegetation_map"), m_vegtationMap->getName().c_str() );
	node->AddAttribute( _T("area_map"), m_divideMap->getName().c_str() );
	node->AddAttribute( _T("color_map"), m_colorMap->getName().c_str() );
	node->AddAttribute( _T("detail_map"), m_detailMap->getName().c_str() );
	node->AddAttribute( _T("detail_normal_map"), m_detailNormalMap->getName().c_str() );
	node->AddAttribute( _T("detail_spec_map"), m_detailSpecMap->getName().c_str() );
}

void gkTerrian::ModifyHeightMap( const gkStdString& heightMapName )
{
	m_heightMap =  gEnv->pSystem->getTextureMngPtr()->load(heightMapName);
}

void gkTerrian::ModifyVegetationDensityMap( const gkStdString& vegMapName )
{
	m_vegtationMap =  gEnv->pSystem->getTextureMngPtr()->load(vegMapName);
}

void gkTerrian::GetTerrianBlock( int x, int y )
{
	//throw std::exception("The method or operation is not implemented.");
}

void gkTerrian::ModifyAreaMap( const gkStdString& areaMapName )
{
	m_divideMap = gEnv->pSystem->getTextureMngPtr()->load(areaMapName);

	if (!m_material.isNull())
	{
		m_material->setTexture( m_divideMap, eMS_Custom2 );
	}
}

void gkTerrian::ModifyColorMap( const gkStdString& areaMapName )
{
	m_colorMap = gEnv->pSystem->getTextureMngPtr()->load(areaMapName);

	if (!m_material.isNull())
	{
		m_material->setTexture( m_colorMap, eMS_Diffuse );
	}
}

void gkTerrian::ModifyDetailMap( const gkStdString& detailMapName, uint8 index )
{
	if (index == 0)
	{
		m_detailMap = gEnv->pSystem->getTextureMngPtr()->load(detailMapName);
		//gkTexturePtr flatnormal = gEnv->pSystem->getTextureMngPtr()->load(_T("engine/assets/textures/default/flat.tga"));
		if (!m_material.isNull())
		{
			m_material->setTexture( m_detailMap, eMS_Custom1 );
		}
	}
	else if (index == 1)
	{
		m_detailNormalMap = gEnv->pSystem->getTextureMngPtr()->load(detailMapName);

		for (uint32 i=0; i < m_terrianBlocks.size(); ++i)
		{
			if (!m_material.isNull())
			{
				m_material->setTexture( m_detailNormalMap, eMS_Detail );
			}
		}
	}
	else
	{
		m_detailSpecMap = gEnv->pSystem->getTextureMngPtr()->load(detailMapName);

		for (uint32 i=0; i < m_terrianBlocks.size(); ++i)
		{
			if (!m_material.isNull())
			{
				m_material->setTexture( m_detailSpecMap, eMS_Specular );
			}
		}
	}



}

void gkTerrian::Destroy()
{
	for (uint32 i=0; i < m_terrianBlocks.size(); ++i)
	{
		gkTerrianBlockEntityGroup& group = m_terrianBlocks[i];

		// 好像没删renderable
		if ( group.vegetation )
		{
			group.vegetation->Destroy();
			//getSceneMngPtr()->destroyMovableObject( group.vegetation );
		}
		

		for (int j=0; j < group.m_lodCount; ++j)
		{
			group.lods[j]->Destroy();
			//getSceneMngPtr()->destroyMovableObject( group.lods[j] );
		}
	}

    if (gEnv->pPhysics) {
        gEnv->pPhysics->DestroyPhysicLayer( m_phyLayer );
    }
}

gkEntity* gkTerrian::CreateLod( uint32 j, uint32 i, int lod, int minlod )
{

	//gkLogMessage(_T("terrian lod loading.."));

	gkEntity* pEnt = getSceneMngPtr()->createEntity();
	gkSceneNode* pNode = getSceneMngPtr()->getRootSceneNode()->createChildSceneNode();
	pNode->attachObject(pEnt);

	Vec4 region;
	region.x = j / (float)m_blockSize;
	region.y = i / (float)m_blockSize;
	region.z = 1.f / (float)m_blockSize;
	region.w = 1.f / (float)m_blockSize;

	uint32 subBlock = m_sideWidth / m_blockSize;

	//gkLogMessage(_T("terrian mesh building."));
	gkTerrianBlockRenderable* block = new gkTerrianBlockRenderable(this, m_heightMap, region, subBlock / lod, lod, m_sideWidth, subBlock / minlod, m_zeroOffset );

	//gkLogMessage(_T("terrian mesh builded."));
	pEnt->modifySubRenderable(block, 0);
	pEnt->setMaterialName(_T("engine/assets/meshs/terrian.mtl"));
	pEnt->setRenderPipe(RENDER_LAYER_TERRIAN);
	//pEnt->showBBox(true);
	block->setParent(pEnt);
	block->enableShadow(false);

	float interval = m_sideWidth / (float)m_blockSize;
	float startx = - m_sideWidth * .5f + interval * .5f;
	float starty = - m_sideWidth * .5f + interval * .5f;

	// set to right place
	pEnt->setPosition( Vec3(startx + interval * j,  starty + interval * i, 0) );

	return pEnt;
}

gkEntity* gkTerrian::CreateWater( float water_surface )
{
	gkEntity* pEnt = getSceneMngPtr()->createEntity();
	gkSceneNode* pNode = getSceneMngPtr()->getRootSceneNode()->createChildSceneNode();
	pNode->attachObject(pEnt);

	gkMeshPtr pMesh = gEnv->pSystem->getMeshMngPtr()->load(_T("engine/assets/meshs/plane.obj"));

	gkRenderable* pStatObj = gEnv->p3DEngine->createRenderable(pMesh, e3RT_StaticObj, 0);
	pEnt->modifySubRenderable(pStatObj, 0);
	pEnt->setMaterialName(_T("engine/assets/meshs/watersurface.mtl"));

	pEnt->setPosition(0,0,water_surface);
	pEnt->setScale(512,512,1);

	return pEnt;
}

gkEntity* gkTerrian::CreateVeg( uint32 j, uint32 i, int count, int segment )
{
	gkEntity* pEnt = getSceneMngPtr()->createEntity();
	gkSceneNode* pNode = getSceneMngPtr()->getRootSceneNode()->createChildSceneNode();
	pNode->attachObject(pEnt);

	Vec4 region;
	region.x = j / (float)m_blockSize;
	region.y = i / (float)m_blockSize;
	region.z = 1.f / (float)m_blockSize;
	region.w = 1.f / (float)m_blockSize;

	uint32 subBlock = m_sideWidth / m_blockSize;

	gkVegetationBlockRenderable* block = new gkVegetationBlockRenderable(this, region, count, segment, i, j);
	pEnt->modifySubRenderable(block, 0);
	pEnt->setMaterialName(_T("engine/assets/meshs/vegetation.mtl"));
	pEnt->setRenderPipe(RENDER_LAYER_OPAQUE);
	//pEnt->showBBox(true);
	block->setParent(pEnt);
	block->enableShadow(true);

	float interval = m_sideWidth / (float)m_blockSize;
	float startx = - m_sideWidth * .5f + interval * .5f;
	float starty = - m_sideWidth * .5f + interval * .5f;

	// set to right place
	pEnt->setPosition( Vec3(startx + interval * j,  starty + interval * i, 0) );

	return pEnt;
}

void gkTerrian::Update()
{
	// 取得摄像机位置
	Vec3 pos = gEnv->p3DEngine->getMainCamera()->getPosition();

	// 和每一个TERRIAN BLOCK求距离来改变LOD
	for (uint32 i=0; i < m_terrianBlocks.size(); ++i)
	{
		gkEntity* lod0 = m_terrianBlocks[i].lods[0];
		float Length = (lod0->getWorldPosition() - pos).GetLength();

		int lod = (int)(Length / 40.f);
		lod = clamp_tpl(lod, 0, 3);

		m_terrianBlocks[i].SwitchLod(lod);
	}
}

float gkTerrian::GetHeight( const Vec2& texcoord, const Vec4& region /*= Vec4(0,0,1,1)*/ )
{
	Vec2 tex0( texcoord.x * region.z + region.x, texcoord.y * region.w + region.y );

	return 100.f * m_heightMap->Tex2DRAW( tex0 ) - m_zeroOffset;
}

float gkTerrian::GetVegetationDensity( const Vec2& texcoord, const Vec4& region /*= Vec4(0,0,1,1)*/ )
{
	Vec2 tex0( texcoord.x * region.z + region.x, texcoord.y * region.w + region.y );

	return m_vegtationMap->Tex2DRAW( tex0 );
}

const gkTexturePtr& gkTerrian::GetHeightMap()
{
	return m_heightMap;
}

const gkTexturePtr& gkTerrian::GetVegetationDensityMap()
{
	return m_vegtationMap;
}

const gkTexturePtr& gkTerrian::GetAreaMap()
{
	return m_divideMap;
}

const gkTexturePtr& gkTerrian::GetColorMap()
{
	return m_colorMap;
}

const gkTexturePtr& gkTerrian::GetDetailMap(uint32 index)
{
	switch(index)
	{
	case 0:
		return m_detailMap;
	case 1:
		return m_detailNormalMap;
	case 2:
		return m_detailSpecMap;
	}
	return m_detailMap;
}

void gkTerrianBlockEntityGroup::SwitchLod( int lodnum )
{
	if (lodnum < m_lodCount)
	{
		if( lods[lodnum] )
		{
			for (uint32 i=0; i < 4; ++i)
			{
				if (lods[i])
				{
					lods[i]->setVisible(false);
				}					
			}
			lods[lodnum]->setVisible(true);
		}
	}
}
