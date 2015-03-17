#include "gkSystemStableHeader.h"

//#include "gkMeshFileLoader.h"
#include "TangentSpaceCalculation.h"

#include <fstream>
#include <iomanip>
#include <iostream>


#include "ISystem.h"
#include "IHavok.h"
#include "IAnimation.h"
#include "ITimer.h"
#include "IResFile.h"

#include "gkMemoryLeakDetecter.h"


#include <vector>

CObjMeshLoader::CObjMeshLoader()
{
	m_cachedVB = 0;
	m_cachedIB = 0;
	m_Subsets.clear();
}

//--------------------------------------------------------------------------------------
CObjMeshLoader::~CObjMeshLoader()
{
	Destroy();
}

//--------------------------------------------------------------------------------------
void CObjMeshLoader::Destroy()
{
	{
		std::vector<Vec3>().swap(Positions);
	}
	
	{
		for (int i=0; i < 8; ++i)
		{
			std::vector<Vec2>().swap(TexCoords[i]);
		}
		
	}
	
	{
		std::vector<Vec3>().swap(Normals);
	}
	
	{
		std::vector<Vec3>().swap( Tangents );
	}
	
	{
		std::vector<Vec3>().swap( Binormals );
	}	

	{
		std::vector<GKVL_P3F2F4F3>().swap(m_Vertices);
	}

	{
		std::vector<uint32>().swap(m_Indices);
	}
	
	{
		gkMeshSubsets().swap(m_Subsets);
	}	

	{
		std::vector<ObjFace>().swap(m_Triangles);
	}


	SAFE_DELETE(m_cachedVB);
	SAFE_DELETE(m_cachedIB);
}

//////////////////////////////////////////////////////////////////////////
void CObjMeshLoader::CreateMesh( EVertexInputType vertexType )
{
	// build the vb first
	// Create Real Vertex
	for (uint32 i=0; i < m_Triangles.size(); ++i)
	{
		for (uint32 j=0; j < 3; ++j)
		{
			GKVL_P3F2F4F3 vertex;

			// ok compresse data here
			{
				vertex.position = Positions[m_Triangles[i].posIdx[j]];
				// tangent & binormal
				vertex.tangent_ti = Vec4(Tangents[m_Triangles[i].tgtIdx[j]], 1);
				vertex.binormal = Binormals[m_Triangles[i].bnmIdx[j]];
				// texcoord
				if (TexCoords[1].size() > 0)
				{
					vertex.texcoord = Vec4( TexCoords[0][m_Triangles[i].texIdx[j]].x, TexCoords[0][m_Triangles[i].texIdx[j]].y, TexCoords[1][m_Triangles[i].tex1Idx[j]].x, TexCoords[1][m_Triangles[i].tex1Idx[j]].y);
				}
				else
				{
					vertex.texcoord = Vec4( TexCoords[0][m_Triangles[i].texIdx[j]].x, TexCoords[0][m_Triangles[i].texIdx[j]].y, 0, 0);
				}
				// normal flip
				Vec3 crossNormal = vertex.tangent_ti.GetXYZ() % vertex.binormal;
				crossNormal.normalize();
				Vec3 normal = Normals[m_Triangles[i].nrmIdx[j]];
				if ( crossNormal.dot(normal) < 0 )
				{
					vertex.tangent_ti.w = -1;
				}
				else
				{
					vertex.tangent_ti.w = 1;
				}
			}
			GKVL_P3F2F4F3* realPtr = reinterpret_cast<GKVL_P3F2F4F3*>(&vertex);



			uint32 index = AddVertex(m_Triangles[i].posIdx[j] + 1, realPtr);
			m_Indices.push_back(index);
		}
	}

	// create vb & ib
	m_cachedVB = new gkVertexBuffer( sizeof(GKVL_P3F2F4F3), m_Vertices.size(), eVI_P4F4F4, eBF_Discard );
	m_cachedIB = new gkIndexBuffer( m_Indices.size() );

	memcpy( m_cachedVB->data, &(m_Vertices[0]), sizeof(GKVL_P3F2F4F3) * m_Vertices.size() );
	memcpy( m_cachedIB->data, &(m_Indices[0]), sizeof(uint32) * m_Indices.size() );

}


bool CObjMeshLoader::LoadGeometryFromGMF( const uint8* binMeshData )
{
	const uint8* readPtr = binMeshData;

	//////////////////////////////////////////////////////////////////////////
	// read header
	gmfHeaderV1 header;
	memcpy( &header, readPtr, sizeof(gmfHeaderV1) );
	readPtr += sizeof(gmfHeaderV1);


	// read position
	Positions.assign( header.vertexPositionCount, Vec3(0,0,0));
	memcpy( Positions.data(), readPtr, header.vertexPositionCount * sizeof(Vec3) );
	readPtr += header.vertexPositionCount * sizeof(Vec3);


	// read texcoord
	for (int i=0; i < 8; ++i)
	{
		TexCoords[i].assign( header.vertexTexcoordCount[i], Vec2(0,0) );
		memcpy( TexCoords[i].data(), readPtr, header.vertexTexcoordCount[i] * sizeof(Vec2) );
		readPtr += header.vertexTexcoordCount[i] * sizeof(Vec2);
	}

	// read trangle
	m_Triangles.assign( header.faceCount, ObjFace() );
	if (header.dwordIndex)
	{
		gmfFace_t2* faceStart = (gmfFace_t2*)readPtr;

		for (uint32 i=0; i < header.faceCount; ++i)
		{
			for (uint32 j=0; j < 3; ++j)
			{
				m_Triangles[i].posIdx[j] = faceStart[i].posIdx[j];
				m_Triangles[i].nrmIdx[j] = faceStart[i].nrmIdx[j];
				m_Triangles[i].texIdx[j] = faceStart[i].texIdx[j];
				m_Triangles[i].tex1Idx[j] = faceStart[i].tex1Idx[j];
			}			
		}

		readPtr += header.faceCount * sizeof(gmfFace_t2);
	}
	else
	{
		gmfFace_t2_16* faceStart = (gmfFace_t2_16*)readPtr;

		for (uint32 i=0; i < header.faceCount; ++i)
		{
			for (uint32 j=0; j < 3; ++j)
			{
				m_Triangles[i].posIdx[j] = faceStart[i].posIdx[j];
				m_Triangles[i].nrmIdx[j] = faceStart[i].nrmIdx[j];
				m_Triangles[i].texIdx[j] = faceStart[i].texIdx[j];
				m_Triangles[i].tex1Idx[j] = faceStart[i].tex1Idx[j];
			}	
		}

		readPtr += header.faceCount * sizeof(gmfFace_t2_16);
	}
	
	m_Subsets.assign(header.subsetCount, gkMeshSubset() );
	for ( uint32 i=0; i < header.subsetCount; ++i )
	{
		gmfSubset* subset = (gmfSubset*)readPtr;
		m_Subsets[i].indexStart = subset[i].faceStart;
		m_Subsets[i].indexCount = subset[i].faceCount;
	}


	// Recreate the Tangent here
	// create TangentSpace
	// CalcTangentSpace
	CTangentSpaceCalculation tangentCalc;
	tangentCalc.CalculateTangentSpace( *this );

	// VERTEX COUNT

	uint32 dwCnt = tangentCalc.GetBaseCount();

	// REBUILD INDICES
	for ( uint32 dwTriNo=0; dwTriNo<GetTriangleCount();++dwTriNo)
	{
		uint32 dwBaseIndex[3];
		tangentCalc.GetTriangleBaseIndices( dwTriNo, dwBaseIndex);
		//……store the base index for the 3 triangle corners

		m_Triangles[dwTriNo].tgtIdx[0] = dwBaseIndex[0];
		m_Triangles[dwTriNo].tgtIdx[1] = dwBaseIndex[1];
		m_Triangles[dwTriNo].tgtIdx[2] = dwBaseIndex[2];

		m_Triangles[dwTriNo].bnmIdx[0] = dwBaseIndex[0];
		m_Triangles[dwTriNo].bnmIdx[1] = dwBaseIndex[1];
		m_Triangles[dwTriNo].bnmIdx[2] = dwBaseIndex[2];

		m_Triangles[dwTriNo].nrmIdx[0] = dwBaseIndex[0];
		m_Triangles[dwTriNo].nrmIdx[1] = dwBaseIndex[1];
		m_Triangles[dwTriNo].nrmIdx[2] = dwBaseIndex[2];
	}

	// REBUILD VERTEX
	Normals.clear();

	for( unsigned int dwC=0; dwC< dwCnt;++dwC)
	{
		float vU[3];
		float vV[3];
		float vN[3];
		tangentCalc.GetBase( dwC,vU,vV,vN);
		Tangents.push_back(Vec3(vU[0], vU[1], vU[2]));
		Binormals.push_back(Vec3(vV[0], vV[1], vV[2]));
		Normals.push_back(Vec3(vN[0], vN[1], vN[2]));
	}


	//int elpsedtime = gEnv->pTimer->GetCurrTime() - starttime;
	//gkLogMessage(_T("ObjMeshFileLoader::Parsing use %d ms."), elpsedtime);

	CreateMesh( eVI_P4F4F4 );

	gkLogMessage(_T("ObjMeshFileLoader::Mesh Created."));

	DeleteCache();

	return true;
}

//--------------------------------------------------------------------------------------
bool CObjMeshLoader::LoadGeometryFromOBJ( const TCHAR* strMeshData )
{
	// Find the file
	// 找文件

	// Create temporary storage for the input data. Once the data has been loaded into
	// a reasonable format we can create a D3DXMesh object and load it with the mesh data.
	int starttime = gEnv->pTimer->GetCurrTime();

	// File input
	TCHAR strCommand[256] = {0};
	gkStdStringstream InFile( strMeshData );

	if( !InFile )
		return false;

	for(; ; )
	{
		InFile >> strCommand;
		if( !InFile )
			break;

		if( 0 == _tcscmp( strCommand, _T("#") ) )
		{
			// Comment
		}
		else if( 0 == _tcscmp( strCommand, _T("v") ) )
		{
			// Vertex Position
			float x, y, z;
			InFile >> x >> y >> z;
			Positions.push_back( Vec3( x, y, z ) );
		}
		else if( 0 == _tcscmp( strCommand, _T("vt") ) )
		{
			// Vertex TexCoord
			float u, v;
			InFile >> u >> v;
			TexCoords[0].push_back( Vec2( u, 1-v ) );
		}
		else if( 0 == _tcscmp( strCommand, _T("vn"))  )
		{
			// Vertex Normal
			float x, y, z;
			InFile >> x >> y >> z;
			Normals.push_back( Vec3( x, y, z ) );
		}
		else if( 0 == _tcscmp( strCommand, _T("f") ) )
		{
			// Face
			int iPosition, iTexCoord, iNormal;
			ObjFace face;
			memset(&face, 0 ,sizeof(face));

			for( uint32 iFace = 0; iFace < 3; iFace++ )
			{
				// OBJ format uses 1-based arrays
				InFile >> iPosition;
				iPosition = abs(iPosition);
				face.posIdx[iFace] = iPosition - 1;

				if( '/' == InFile.peek() )
				{
					InFile.ignore();

					if( '/' != InFile.peek() )
					{
						// Optional texture coordinate
						InFile >> iTexCoord;
						iTexCoord = abs(iTexCoord);
						face.texIdx[iFace] = iTexCoord - 1;
					}

					if( '/' == InFile.peek() )
					{
						InFile.ignore();

						// Optional vertex normal
						InFile >> iNormal;
						iNormal = abs(iNormal);
						face.nrmIdx[iFace] = iNormal - 1;
					}
				}
			}

			m_Triangles.push_back(face);
		}
		else if ( 0 == _tcscmp( strCommand, _T("usemtl") ) )
		{
			if (!m_Subsets.empty())
			{
				m_Subsets.back().indexCount = m_Triangles.size() * 3 - m_Subsets.back().indexStart;
			}

			gkMeshSubset subset;
			InFile >> subset.mtlname;
			subset.indexStart = m_Triangles.size() * 3;
			
			m_Subsets.push_back(subset);
		}
		else
		{
			// Unimplemented or unrecognized command
		}

		InFile.ignore( 1000, '\n' );
	}

	if (!m_Subsets.empty())
	{
		m_Subsets.back().indexCount = m_Triangles.size() * 3 - m_Subsets.back().indexStart;
	}
	else
	{
		// not sub mtl used
		gkMeshSubset subset;
		subset.mtlname = _T("undefined");
		subset.indexStart = 0;
		subset.indexCount = m_Triangles.size() * 3;
		m_Subsets.push_back(subset);
	}

	// Recreate the Tangent here
	// create TangentSpace
	// CalcTangentSpace
	CTangentSpaceCalculation tangentCalc;
	tangentCalc.CalculateTangentSpace( *this );

	// VERTEX COUNT

	uint32 dwCnt = tangentCalc.GetBaseCount();

	// REBUILD INDICES
	for ( uint32 dwTriNo=0; dwTriNo<GetTriangleCount();++dwTriNo)
	{
		uint32 dwBaseIndex[3];
		tangentCalc.GetTriangleBaseIndices( dwTriNo, dwBaseIndex);
		//……store the base index for the 3 triangle corners

		m_Triangles[dwTriNo].tgtIdx[0] = dwBaseIndex[0];
		m_Triangles[dwTriNo].tgtIdx[1] = dwBaseIndex[1];
		m_Triangles[dwTriNo].tgtIdx[2] = dwBaseIndex[2];

		m_Triangles[dwTriNo].bnmIdx[0] = dwBaseIndex[0];
		m_Triangles[dwTriNo].bnmIdx[1] = dwBaseIndex[1];
		m_Triangles[dwTriNo].bnmIdx[2] = dwBaseIndex[2];

		m_Triangles[dwTriNo].nrmIdx[0] = dwBaseIndex[0];
		m_Triangles[dwTriNo].nrmIdx[1] = dwBaseIndex[1];
		m_Triangles[dwTriNo].nrmIdx[2] = dwBaseIndex[2];
	}

	// REBUILD VERTEX
	Normals.clear();

	for( unsigned int dwC=0; dwC< dwCnt;++dwC)
	{
		float vU[3];
		float vV[3];
		float vN[3];
		tangentCalc.GetBase( dwC,vU,vV,vN);
		Tangents.push_back(Vec3(vU[0], vU[1], vU[2]));
		Binormals.push_back(Vec3(vV[0], vV[1], vV[2]));
		Normals.push_back(Vec3(vN[0], vN[1], vN[2]));
	}


	int elpsedtime = gEnv->pTimer->GetCurrTime() - starttime;
	gkLogMessage(_T("ObjMeshFileLoader::Parsing use %d ms."), elpsedtime);

	CreateMesh( eVI_P4F4F4 );

	gkLogMessage(_T("ObjMeshFileLoader::Mesh Created."));

	DeleteCache();

	return true;
}

//--------------------------------------------------------------------------------------
uint32 CObjMeshLoader::AddVertex( uint32 hash, GKVL_P3F2F4F3* pVertex )
{
	bool bFoundInList = false;
	uint32 index = 0;

	if( ( uint32 )m_VertexCache.size() > hash )
	{
		CacheEntry* pEntry = m_VertexCache[hash];
		while( pEntry != NULL )
		{
			GKVL_P3F2F4F3* pCacheVertex = &(m_Vertices[0]) + pEntry->index;
			if( 0 == memcmp( pVertex, pCacheVertex, sizeof( GKVL_P3F2F4F3 ) ) )
			{
				bFoundInList = true;
				index = pEntry->index;
				break;
			}
			pEntry = pEntry->pNext;
		}
	}
	if( !bFoundInList )
	{
		index = m_Vertices.size();
		m_Vertices.push_back( *pVertex );

		CacheEntry* pNewEntry = new CacheEntry;
		if( pNewEntry == NULL )
			return 0;

		pNewEntry->index = index;
		pNewEntry->pNext = NULL;

		if ( ( uint32 )m_VertexCache.size() <= hash )
		{
			m_VertexCache.resize( hash + 1, NULL );
		}

		CacheEntry* pCurEntry = m_VertexCache[hash];
		if( pCurEntry == NULL )
		{
			m_VertexCache[hash] = pNewEntry;
		}
		else
		{
			while( pCurEntry->pNext != NULL )
			{
				pCurEntry = pCurEntry->pNext;
			}

			pCurEntry->pNext = pNewEntry;
		}
	}

	return index;
}
//--------------------------------------------------------------------------------------
void CObjMeshLoader::DeleteCache()
{
	// Iterate through all the elements in the cache and subsequent linked lists
	for( uint32 i = 0; i < m_VertexCache.size(); i++ )
	{
		CacheEntry* pEntry = m_VertexCache[i];
		while( pEntry != NULL )
		{
			CacheEntry* pNext = pEntry->pNext;
			if (pEntry)
			{
				delete pEntry;
			}
			pEntry = pNext;
		}
	}

	std::vector<CacheEntry*> swaper;
	m_VertexCache.swap(swaper);
}

uint32 CObjMeshLoader::GetTriangleCount( void ) const
{
	return m_Triangles.size();
}

void CObjMeshLoader::GetTriangleIndices( const uint32 indwTriNo, uint32 outdwPos[3], uint32 outdwNorm[3], uint32 outdwUV[3] ) const
{
	for (int i=0; i < 3; ++i)
	{
		outdwPos[i] = m_Triangles[indwTriNo].posIdx[i];
		outdwNorm[i] = m_Triangles[indwTriNo].nrmIdx[i];
		outdwUV[i] = m_Triangles[indwTriNo].texIdx[i];
	}

}

void CObjMeshLoader::GetPos( const uint32 indwPos, float outfPos[3] ) const
{
	outfPos[0] = Positions[indwPos].x;
	outfPos[1] = Positions[indwPos].y;
	outfPos[2] = Positions[indwPos].z;
}

void CObjMeshLoader::GetUV( const uint32 indwPos, float outfUV[2] ) const
{
	// 从第一层取
	outfUV[0] = TexCoords[0][indwPos].x;
	outfUV[1] = TexCoords[0][indwPos].y;
}

void CObjMeshLoader::getSubset( uint32 index, uint32& start, uint32& count, gkStdString& mtlname )
{
	//GK_ASSERT(index < m_Subsets.size());
	start = m_Subsets[index].indexStart;
	count = m_Subsets[index].indexCount;
	//mtlname = m_Subsets[index].mtlname;
}

bool CObjMeshLoader::LoadGeometry( const TCHAR* filename )
{
	bool ret = false;

	// 将文件内容读到缓存中
	IResFile* pMeshfile = gEnv->pFileSystem->loadResFile( filename, false );
	if ( !pMeshfile )
	{
		return false;
	}



	if ( !_tcsicmp( &(filename[_tcslen(filename) - 4]), _T(".obj") ))
	{
#ifdef _UNICODE
		int size = pMeshfile->Size() / sizeof(char);
		TCHAR* textbuf = new TCHAR[size + 1];
		MultiByteToWideChar( CP_ACP, 0, (char*)pMeshfile->DataPtr(), pMeshfile->Size(), textbuf, size + 1 );
		textbuf[size] = 0;
#else
		TCHAR* textbuf = (char*)pMeshfile->DataPtr();
#endif

		ret = LoadGeometryFromOBJ( textbuf );

#ifdef _UNICODE
		SAFE_DELETE_ARRAY( textbuf );
#endif
	}
	else
	{
		ret = LoadGeometryFromGMF( (const uint8*)(pMeshfile->DataPtr()) );
	}
	




	gkLogMessage(_T("MeshLoaded closing file..."));

	gEnv->pFileSystem->closeResFile( pMeshfile );

	return ret;
}

void CObjMeshLoader::FinishLoading()
{
	Destroy();
}

uint32 CObjMeshLoader::GetBoneBaseTransformCount()
{
	return 0;
}

void CObjMeshLoader::GetBoneBaseTransform( Matrix44& matrix, uint32 index )
{
	return;
}
