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
// Name:   	gmpMeshLoader.h
// Desc:		
// 	
// Author:  Kaiming
// Date:	2012/8/19
// Modify:	
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _gmpMeshLoader_h_
#define _gmpMeshLoader_h_


#include "../system/gkMaxPort.h"
#include "IMesh.h"

struct gkVertexP4F4F4
{
	Point3 position;
	Point3 normal;
	Point2 texcoord;
};

struct gkVertexP3N3T2G3B3
{
	Point3 position;
	Point3 normal;
	Point2 texcoord;
	Point3 tangent;
	Point3 binormal;
};

struct MAXFACE
{
	uint32 dwFirstFace;
	uint32 dwSecondFace;
	uint32 dwThirdFace;
	uint32 dwMatID;
};
// Used for a hashtable vertex cache when creating the mesh from a .obj file
struct CacheEntry
{
	uint32 index;
	CacheEntry* pNext;
};

class gmpMeshLoader
{
public:
	gmpMeshLoader();
	~gmpMeshLoader();

	void Destroy();

	int GetVertexNumber()
	{
		return Positions.size();
	}
	int GetTexVertexNumber()
	{
		return TexCoords.size();
	}
	int GetFaceNumber()
	{
		return m_MaxPointFaces.size();
	}

public:

	bool LoadGeometryFromOBJ( const TCHAR* strMeshData );
	static bool SaveGeometryAsOBJ( const TCHAR* strFilename , Mesh& mesh, float radius = 1);
	static bool SaveGeometryAsGMF( const TCHAR* strFilename , Mesh& mesh, float radius = 1);

	void   DeleteCache();

	bool	m_bIsLoaded;

	std::vector <Point3> Positions;
	std::vector <Point2> TexCoords;
	std::vector <Point3> Normals;

	std::vector <CacheEntry*> m_VertexCache;   // Hashtable cache for locating duplicate vertices
	std::vector <gkVertexP4F4F4> m_Vertices;     // Filled and copied to the vertex buffer
	std::vector <DWORD> m_Indices;       // Filled and copied to the index buffer
	std::vector <DWORD> m_Attributes;    // Filled and copied to the attribute buffer


	std::vector <MAXFACE> m_MaxPointFaces;
	std::vector <MAXFACE> m_MaxTexcoordFaces;
};

#endif


