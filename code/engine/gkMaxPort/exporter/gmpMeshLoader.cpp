#include "gmpMeshLoader.h"

#include <fstream>
#include <iomanip>
#include <iostream>

gmpMeshLoader::gmpMeshLoader()
{

}

gmpMeshLoader::~gmpMeshLoader()
{
	Destroy();
}

void gmpMeshLoader::Destroy()
{
	Positions.clear();
	TexCoords.clear();
	Normals.clear();

	m_Vertices.clear();
	m_Indices.clear();
	m_Attributes.clear();

	m_MaxPointFaces.clear();
	m_MaxTexcoordFaces.clear();
}

void gmpMeshLoader::DeleteCache()
{
	// Iterate through all the elements in the cache and subsequent linked lists
	for( int i = 0; i < m_VertexCache.size(); i++ )
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

	m_VertexCache.clear();
}

//--------------------------------------------------------------------------------------
bool gmpMeshLoader::LoadGeometryFromOBJ( const TCHAR* strMeshData )
{
	// File input
	TCHAR strCommand[256] = {0};
	gkStdStringstream InFile( strMeshData );

	if( !InFile )
		return S_FALSE;

	for( ; ; )
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

			Positions.push_back( Point3( x, y, z ) );
			gkVertexP4F4F4 tmpVertex;
			tmpVertex.position = Point3( x,y,z );
			m_Vertices.push_back( tmpVertex );

			// for3DsMax, 加入点列，随后方便走面的时候，输入normal和tc
		}
		else if( 0 == _tcscmp( strCommand, _T("vt") ) )
		{
			// Vertex TexCoord
			float u, v;
			InFile >> u >> v;
			// for3DsMax, 加入点列，随后方便走面的时候，输入normal和tc

			TexCoords.push_back( Point2( u, v ) );
		}
		else if( 0 == _tcscmp( strCommand, _T("vn"))  )
		{
			// Vertex Normal
			float x, y, z;
			InFile >> x >> y >> z;

			Normals.push_back( Point3( x, y, z ) );
		}
		else if( 0 == _tcscmp( strCommand, _T("f") ) )
		{
			// Face
			int iPosition, iTexCoord, iNormal;
			gkVertexP4F4F4 vertex;

			MAXFACE sMaxPointFace;
			MAXFACE sMaxTcFace;

			//WORD	dwIndex[3];
			for( UINT iFace = 0; iFace < 3; iFace++ )
			{
				ZeroMemory( &vertex, sizeof( gkVertexP4F4F4 ) );

				// OBJ format uses 1-based arrays
				InFile >> iPosition;
				iPosition = abs(iPosition);
				vertex.position = Positions[ iPosition - 1 ];


				switch(iFace)
				{
				case 0 :
					sMaxPointFace.dwFirstFace = iPosition - 1;
					break;
				case 1 :
					sMaxPointFace.dwSecondFace = iPosition - 1;
					break;
				case 2 :
					sMaxPointFace.dwThirdFace = iPosition - 1;
					break;
				}

				if( '/' == InFile.peek() )
				{
					InFile.ignore();

					if( '/' != InFile.peek() )
					{
						// Optional texture coordinate
						InFile >> iTexCoord;
						iTexCoord = abs(iTexCoord);
						vertex.texcoord = TexCoords[ iTexCoord - 1];

						switch(iFace)
						{
						case 0 :
							sMaxTcFace.dwFirstFace = iTexCoord - 1;
							break;
						case 1 :
							sMaxTcFace.dwSecondFace = iTexCoord - 1;
							break;
						case 2 :
							sMaxTcFace.dwThirdFace = iTexCoord - 1;
							break;
						}

					}

					if( '/' == InFile.peek() )
					{
						InFile.ignore();

						// Optional vertex normal
						InFile >> iNormal;
						iNormal = abs(iNormal);
						vertex.normal = Normals[ iNormal - 1 ];
					}
				}

				// for3DSMAX
				m_Vertices[iPosition - 1] = vertex;
			}
			// for3DsMax, maxFace
			// command f, the last if exist, attribute written
			if( '\n' != InFile.peek() )
			{
				InFile.ignore();
				// push forward, judge again
				if ('\n' != InFile.peek())
				{
					DWORD attribute = 0;
					InFile >> attribute;
					sMaxPointFace.dwMatID = attribute;
				}
			}

			m_MaxPointFaces.push_back( sMaxPointFace );
			m_MaxTexcoordFaces.push_back( sMaxTcFace );				
		}
		else
		{
			// Unimplemented or unrecognized command
		}

		InFile.ignore( 1000, '\n' );
	}


	// Cleanup
	//InFile.close();
	DeleteCache();
	m_bIsLoaded = true;

	return S_OK;
}

void GetRNormalFromSGroup(
	Point3& vrn/*获得的顶点法线[out]*/,
	Mesh& mesh/*目标网格*/,
	int faceId/*目标面片的id*/,
	int vertId/*目标顶点的id*/)
{
	RVertex *pRVert = mesh.getRVertPtr(vertId);

	if (!pRVert)
	{
		return;
	}

	const Face& face = mesh.faces[faceId];
	const DWORD smGroup = face.smGroup;
	const int normalCT = (pRVert->rFlags) & NORCT_MASK;



	if(pRVert->rFlags & SPECIFIED_NORMAL)
	{
		vrn = pRVert->rn.getNormal();
		return;
	}
	else if((normalCT>0)&&(smGroup != 0))
	{
		if(normalCT == 1)
		{
			vrn = pRVert->rn.getNormal();
			return;
		}
		else
		{
			for(int normalId = 0;normalId < normalCT ; normalId++)
			{
				if(pRVert->ern[normalId].getSmGroup() & smGroup)
				{
					vrn = pRVert->ern[normalId].getNormal();
					return;
				}
			}
		}
	}
	vrn = mesh.getFaceNormal(faceId);
}

bool gmpMeshLoader::SaveGeometryAsGMF(const TCHAR* strFilename , Mesh& mesh, float radius /*= 1*/)
{
	int vnum = mesh.getNumVerts();
	int tnum = mesh.getNumTVerts();
	int inum = mesh.getNumFaces();

	MaxObjVertexRef<Point3> posRef;
	MaxObjVertexRef<Point3> normRef;

	// multi uv channel
	MaxObjVertexRef<Point2> multi_texcRef[8];

	posRef.SetPrecision(0.0001f);
	normRef.SetPrecision(0.0001f);

	for (int i=0; i < 8; ++i)
	{
		multi_texcRef[i].SetPrecision(0.0001f);
	}
	mesh.buildNormals();

	for ( int f=0; f < mesh.getNumFaces(); ++f)
	{
		for (int i=0; i < 3; ++i)
		{
			Point3 normal;
			GetRNormalFromSGroup( normal, mesh, f, mesh.faces[f].v[i] );
			normRef.AddNormalized(normal);
			posRef.Add( mesh.getVert( mesh.faces[f].v[i] ) );
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// multiple texcoord

	for (int i=0; i < 8; ++i)
	{
		if( mesh.mapSupport(i+1) )
		{
			TVFace* faces = mesh.mapFaces(i+1);
			UVVert* verts = mesh.mapVerts(i+1);

			for (int j=0; j < mesh.getNumMapVerts(i+1);++j)
			{
				multi_texcRef[i].Add( Point2( verts[j].x, 1 - verts[j].y ) );
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// go through once to emit matids...
	int num_matids = 0;
	std::vector<MtlID> id_map;
	for (int i=0; i< inum; ++i)
	{
		bool exist = false;
		for (int j=0; j < id_map.size(); ++j)
		{
			if (id_map[j] == mesh.faces[i].getMatID())
			{
				exist = true;
				break;
			}
		}

		if (exist)
		{
			continue;
		}

		num_matids++;
		id_map.push_back( mesh.faces[i].getMatID() );
	}


	std::vector<gmfFace_t2> m_faces;
	SubsetArray m_subsets;

	//////////////////////////////////////////////////////////////////////////
	// go through second to write faces every matids...
	for (int m=0; m < num_matids; ++m)
	{

		if (!m_subsets.empty())
		{
			m_subsets.back().faceCount = m_faces.size() * 3 - m_subsets.back().faceStart;
		}

		gmfSubset subset;
		subset.faceStart = m_faces.size() * 3;

		for (int i=0; i< inum; ++i)
		{
			if ( mesh.faces[i].getMatID() == (m) )
			{
				gmfFace_t2 face_this;

				face_this.posIdx[0] = posRef.normalIdx[i*3 + 0] - 1;
				face_this.posIdx[1] = posRef.normalIdx[i*3 + 1] - 1;
				face_this.posIdx[2] = posRef.normalIdx[i*3 + 2] - 1;

				if (mesh.mapSupport(1))
				{
					TVFace* faces = mesh.mapFaces(1);

					face_this.texIdx[0] = multi_texcRef[0].normalIdx[faces[i].t[0]] - 1;
					face_this.texIdx[1] = multi_texcRef[0].normalIdx[faces[i].t[1]] - 1;
					face_this.texIdx[2] = multi_texcRef[0].normalIdx[faces[i].t[2]] - 1;
				}

				if (mesh.mapSupport(2))
				{
					TVFace* faces = mesh.mapFaces(2);

					face_this.tex1Idx[0] = multi_texcRef[1].normalIdx[faces[i].t[0]] - 1;
					face_this.tex1Idx[1] = multi_texcRef[1].normalIdx[faces[i].t[1]] - 1;
					face_this.tex1Idx[2] = multi_texcRef[1].normalIdx[faces[i].t[2]] - 1;
				}

				face_this.nrmIdx[0] = normRef.normalIdx[i*3 + 0] - 1;
				face_this.nrmIdx[1] = normRef.normalIdx[i*3 + 1] - 1;
				face_this.nrmIdx[2] = normRef.normalIdx[i*3 + 2] - 1;

				m_faces.push_back(face_this);
			}
		}

		m_subsets.push_back(subset);
	}


	if (!m_subsets.empty())
	{
		m_subsets.back().faceCount = m_faces.size() * 3 - m_subsets.back().faceStart;
	}
	else
	{
		// not sub mtl used
		gmfSubset subset;
		subset.faceStart = 0;
		subset.faceCount = m_faces.size() * 3;
		m_subsets.push_back(subset);
	}

	FILE* file = fopen( strFilename, "wb" );
	if ( !file)
	{
		printf("FATAL ERROR! 创建文件失败\n");
		return -1;
	}

	gmfHeaderV1 header;
	memset(&header, 0, sizeof(header));
	memcpy( header.identityExt, "GMF1", 4);
	header.gmf_header_version = GMF_VER_1_0;
	header.vertexPositionCount = posRef.m_vecVectors.size();

	for (int i=0; i < 8; ++i)
	{
		header.vertexTexcoordCount[i] = multi_texcRef[i].m_vecVectors.size();	
	}

	header.faceCount = m_faces.size();
	header.subsetCount = m_subsets.size();
	header.dwordIndex = ( posRef.m_vecVectors.size() > (1 << 16) ) || ( posRef.m_vecVectors.size() > (1 << 16) );

	fseek( file, 0, SEEK_SET );
	// 写header
	fwrite( &header, 1, sizeof(gmfHeaderV1), file );
	// 写vertex
	fwrite( posRef.m_vecVectors.data(), 1, posRef.m_vecVectors.size() * sizeof(Point3), file );

	for (int i=0; i < 8; ++i)
	{
		fwrite( multi_texcRef[i].m_vecVectors.data(), 1, multi_texcRef[i].m_vecVectors.size() * sizeof(Point2), file );
	}

	// 写face
	// 如果不是dwordIndex， 转换一下
	if ( !header.dwordIndex )
	{
		gmfFace_t2_16* faces_16 = new gmfFace_t2_16[ m_faces.size() ];
		for ( uint32 i=0; i < m_faces.size(); ++i )
		{
			for ( uint32 j=0; j < 3; ++j)
			{
				faces_16[i].posIdx[j] = m_faces[i].posIdx[j];
				faces_16[i].nrmIdx[j] = m_faces[i].nrmIdx[j];
				faces_16[i].texIdx[j] = m_faces[i].texIdx[j];
				faces_16[i].tex1Idx[j] = m_faces[i].tex1Idx[j];
			}
		}
		fwrite( faces_16, 1, m_faces.size() * sizeof(gmfFace_t2_16), file );
	}
	else
	{
		fwrite( m_faces.data(), 1, m_faces.size() * sizeof(gmfFace_t2), file );
	}


	// 写subset
	fwrite( m_subsets.data(), 1, m_subsets.size() * sizeof(m_subsets), file );

	fclose( file );

	return true;
}

bool gmpMeshLoader::SaveGeometryAsOBJ( const TCHAR* strFilename , Mesh& mesh, float radius /*= 1*/ )
{
	int vnum = mesh.getNumVerts();
	int tnum = mesh.getNumTVerts();
	int inum = mesh.getNumFaces();

	MaxObjVertexRef<Point3> posRef;
	MaxObjVertexRef<Point3> normRef;
	MaxObjVertexRef<Point2> texcRef;
	posRef.SetPrecision(0.0001f);
	normRef.SetPrecision(0.0001f);
	texcRef.SetPrecision(0.0001f);

	std::wofstream InFile( strFilename );
	if( !InFile )
		return false;

	InFile << "# gameKnife wavefront obj file. this file was exported by gkObject." << std::endl << std::endl;
	
	mesh.buildNormals();

	for ( int f=0; f < mesh.getNumFaces(); ++f)
	{
		for (int i=0; i < 3; ++i)
		{
			Point3 normal;
			GetRNormalFromSGroup( normal, mesh, f, mesh.faces[f].v[i] );
			normRef.AddNormalized(normal);
			posRef.Add( mesh.getVert( mesh.faces[f].v[i] ) );
		}
	}

	for (int i=0; i < posRef.m_vecVectors.size(); ++i)
	{
		InFile << std::setprecision(4) << "v " << posRef.m_vecVectors[i].x <<" "<< posRef.m_vecVectors[i].y << " "<< posRef.m_vecVectors[i].z <<std::endl;
	}

	InFile << "# " << vnum  << " vertices" << std::endl;

	InFile << std::endl;

	for (int i=0; i < tnum; ++i)
	{
		texcRef.Add( Point2(mesh.tVerts[i].x, mesh.tVerts[i].y) );
		//InFile << "vt " << mesh.tVerts[i].x <<" "<<  mesh.tVerts[i].y <<" "<< mesh.tVerts[i].z << std::endl;
	}

	for (int i=0; i < texcRef.m_vecVectors.size(); ++i)
	{
		InFile << std::setprecision(4) << "vt " << texcRef.m_vecVectors[i].x <<" "<< texcRef.m_vecVectors[i].y << std::endl;
	}

	InFile << "# " << texcRef.m_vecVectors.size() << " texcoords" << std::endl;
	InFile << std::endl;

	for (int i=0; i < normRef.m_vecVectors.size(); ++i)
	{
		InFile << "vn " << std::setprecision(4) << normRef.m_vecVectors[i].x <<" "<<  normRef.m_vecVectors[i].y <<" "<< normRef.m_vecVectors[i].z << std::endl;
	}

	InFile << "# " << normRef.m_vecVectors.size()  << " normals" << std::endl;
	InFile << std::endl;

	InFile << "g " << "gkmesh_obj" << std::endl;

	InFile << std::endl;

	//////////////////////////////////////////////////////////////////////////
	// go through once to emit matids...
	int num_matids = 0;
	std::vector<MtlID> id_map;
	for (int i=0; i< inum; ++i)
	{
		bool exist = false;
		for (int j=0; j < id_map.size(); ++j)
		{
			if (id_map[j] == mesh.faces[i].getMatID())
			{
				exist = true;
				break;
			}
		}

		if (exist)
		{
			continue;
		}

		num_matids++;
		id_map.push_back( mesh.faces[i].getMatID() );
	}

	//////////////////////////////////////////////////////////////////////////
	// go through second to write faces every matids...
	for (int m=0; m < num_matids; ++m)
	{
		InFile << "usemtl " << m << std::endl;

		for (int i=0; i< inum; ++i)
		{
			if ( mesh.faces[i].getMatID() == (m) )
			{
				InFile << "f " <<posRef.normalIdx[i*3 + 0] << "/" << texcRef.normalIdx[mesh.tvFace[i].t[0]] << "/" << normRef.normalIdx[i*3 + 0] <<
					" "<< posRef.normalIdx[i*3 + 1] << "/" << texcRef.normalIdx[mesh.tvFace[i].t[1]] << "/" << normRef.normalIdx[i*3 + 1] <<
					" "<< posRef.normalIdx[i*3 + 2] << "/" << texcRef.normalIdx[mesh.tvFace[i].t[2]] << "/" << normRef.normalIdx[i*3 + 2] << std::endl;
			}
		}
	}

	InFile << "# " << inum  << " faces" << std::endl;
	InFile << std::endl;

	InFile << "# gameKnife wavefront obj file end.";

	InFile.close();

	return true;
}