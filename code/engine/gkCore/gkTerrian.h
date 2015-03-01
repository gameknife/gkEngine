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
// Name:   	gkTerrian.h
// Desc:	
// 	
// 
// Author:  Kaiming
// Date:	2013/1/8	
// 
//////////////////////////////////////////////////////////////////////////

#ifndef gkTerrian_h__
#define gkTerrian_h__
#include "ITerrianSystem.h"
#include "IMaterial.h"
class gkEntity;
struct IGameObjectPhysicLayer;
struct CRapidXmlAuthorNode;
struct CRapidXmlParseNode;

struct gkTerrianBlockEntityGroup
{
	int m_lodCount;

	gkEntity* lods[4];
	gkEntity* vegetation;

	gkTerrianBlockEntityGroup(void)
	{
		memset(this, 0, sizeof(gkTerrianBlockEntityGroup));
	}

	void SwitchLod(int lodnum);
};

class gkTerrian : public ITerrianSystem
{
public:
	gkTerrian();
	~gkTerrian(void);

	virtual void Create( const gkStdString& heightMapName, int blockSide, int sideWidth );

	virtual void Create( CRapidXmlParseNode* node );
	virtual void Save( CRapidXmlAuthorNode* node );

	virtual void ModifyHeightMap( const gkStdString& heightMapName );

	virtual void ModifyVegetationDensityMap( const gkStdString& vegMapName);

	virtual void GetTerrianBlock( int x, int y );

	virtual void ModifyAreaMap( const gkStdString& areaMapName );

	virtual void ModifyColorMap( const gkStdString& areaMapName );

	virtual void ModifyDetailMap( const gkStdString& detailMapName, uint8 index );

	virtual void Update();

	virtual void Destroy();

	virtual const gkTexturePtr& GetHeightMap();
	virtual const gkTexturePtr& GetVegetationDensityMap();
	virtual const gkTexturePtr& GetAreaMap(); 
	virtual const gkTexturePtr& GetColorMap(); 
	virtual const gkTexturePtr& GetDetailMap(uint32 index); 

	void CreateTerrianBlock();
	
	float GetVegetationDensity(const Vec2& texcoord, const Vec4& region = Vec4(0,0,1,1));
	float GetHeight(const Vec2& texcoord, const Vec4& region = Vec4(0,0,1,1));
	float GetSize() {return m_sideWidth;}

	gkEntity* CreateLod( uint32 j, uint32 i, int lod, int minlod );
	gkEntity* CreateVeg( uint32 j, uint32 i, int count, int segment );
	gkEntity* CreateWater( float water_surface );

private:
	std::vector<gkTerrianBlockEntityGroup> m_terrianBlocks;

	gkTexturePtr m_heightMap;
	gkTexturePtr m_vegtationMap;
	gkTexturePtr m_divideMap;
	gkTexturePtr m_colorMap;

	gkTexturePtr m_detailMap;
	gkTexturePtr m_detailNormalMap;
	gkTexturePtr m_detailSpecMap;

	uint32 m_blockSize;
	int m_sideWidth;

	gkMaterialPtr m_material;
	IGameObjectPhysicLayer* m_phyLayer;

	float m_zeroOffset;

	gkEntity* m_water;
};

#endif // gkTerrian_h__


