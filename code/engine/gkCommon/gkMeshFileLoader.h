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
// Name:   	gkMeshFileLoader.h
// Desc:	GameKnife的模型读取工具
// 描述:	
// 
// Author:  Kaiming-Desktop	 
// Date:	12/11/2010 	
// 
//////////////////////////////////////////////////////////////////////////
#pragma once

#include "gkRenderOperation.h"

#include "IMeshLoader.h"

class CObjMeshLoader : public IMeshLoader
{
public:
	CObjMeshLoader();
	IMesh::BoneBaseTransforms& GetBoneBaseTransforms() {return m_BoneBaseTransforms;}
public:
	bool LoadGeometryFromOBJ( const TCHAR* strMeshData );
	bool LoadGeometryFromGMF( const uint8* binMeshData );
	virtual bool LoadGeometry( const TCHAR* filename);


	virtual gkVertexBuffer* getVB() {return m_cachedVB;}
	virtual gkIndexBuffer* getIB() {return m_cachedIB;}

	virtual uint32 getSubsetCount() {return m_Subsets.size();}
	virtual void getSubset(uint32 index, uint32& start, uint32& count, gkStdString& mtlname);
	virtual void FinishLoading();


	IMesh::BoneBaseTransforms m_BoneBaseTransforms;

	~CObjMeshLoader();
	
	void Destroy();


	
	
	virtual void CreateMesh( EVertexInputType vertexType );
	uint32   AddVertex( uint32 hash, GKVL_P3F2F4F3* pVertex );
	void    DeleteCache();

public:
	uint32 GetTriangleCount( void ) const;	

	//! /param indwTriNo 0..
	//! /param outdwPos
	//! /param outdwNorm
	//! /param outdwUV
	void GetTriangleIndices( const uint32 indwTriNo, uint32 outdwPos[3], uint32 outdwNorm[3], uint32 outdwUV[3] ) const;	

	//! /param indwPos 0..
	//! /param outfPos
	void GetPos( const uint32 indwPos, float outfPos[3] ) const;

	//! /param indwPos 0..
	//! /param outfUV 
	void GetUV( const uint32 indwPos, float outfUV[2] ) const;

	virtual uint32 GetBoneBaseTransformCount();

	virtual void GetBoneBaseTransform( Matrix44& matrix, uint32 index );

	std::vector<Vec3> Positions;
	std::vector<Vec2> TexCoords[8];
	std::vector<Vec3> Normals;
	std::vector<Vec3> Tangents;
	std::vector<Vec3> Binormals;

	std::vector<CacheEntry*> m_VertexCache;   // Hashtable cache for locating duplicate vertices
	std::vector<GKVL_P3F2F4F3> m_Vertices;     // Filled and copied to the vertex buffer
	std::vector<uint32> m_Indices;       // Filled and copied to the index buffer

	std::vector<ObjFace> m_Triangles;

	gkMeshSubsets m_Subsets;

	gkVertexBuffer* m_cachedVB;
	gkIndexBuffer* m_cachedIB;
};
