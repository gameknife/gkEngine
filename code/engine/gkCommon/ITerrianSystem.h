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
// Name:   	ITerrianSystem.h
// Desc:	地形系统
// 	
// 
// Author:  Kaiming
// Date:	2013/1/9	
// 
//////////////////////////////////////////////////////////////////////////

#ifndef ITerrianSystem_h__
#define ITerrianSystem_h__

struct CRapidXmlParseNode;
struct CRapidXmlAuthorNode;

struct ITerrianSystem
{
	virtual ~ITerrianSystem() {}

	// 创建地形，需要heightMap名，分块的边数，边长
	virtual void Create( const gkStdString& heightMapName, int blockSide, int sideWidth ) =0;
	virtual void Create( CRapidXmlParseNode* node ) =0;
	virtual void Save( CRapidXmlAuthorNode* node ) =0;

	// 替换heightmap
	virtual void ModifyHeightMap( const gkStdString& heightMapName ) =0;
	// 替换vegtationmap
	virtual void ModifyVegetationDensityMap( const gkStdString& vegMapName) =0;


	// 取得地块
	virtual void GetTerrianBlock( int x, int y) =0;
	// 设置dividingMap : RGBA四个通道四层，背景一层
	virtual void ModifyAreaMap( const gkStdString& areaMapName ) =0;
	// 设置colorMap : 背景颜色贴图
	virtual void ModifyColorMap( const gkStdString& areaMapName ) =0;
	// 设置DetailMap
	virtual void ModifyDetailMap( const gkStdString& detailMapName, uint8 index ) =0;

	virtual const gkTexturePtr& GetHeightMap() =0;
	virtual const gkTexturePtr& GetVegetationDensityMap() =0;
	virtual const gkTexturePtr& GetAreaMap() =0; 
	virtual const gkTexturePtr& GetColorMap() =0; 
	virtual const gkTexturePtr& GetDetailMap(uint32 index) =0;	

	// 每帧更新
	virtual void Update() =0;

	// 销毁地形
	virtual void Destroy() =0;
};


#endif // ITerrianSystem_h__
