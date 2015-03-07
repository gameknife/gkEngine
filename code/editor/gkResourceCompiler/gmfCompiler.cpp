#include "gmfCompiler.h"
#include "atcCompiler.h"

bool gmfCompiler::encode(const char* filename, const char* cfg)
{
	std::string m_filename(filename);

	if ( m_filename.empty() )
	{
		return false;
	}

	// open file
	FILE* tmpFile = fopen( m_filename.c_str(), "rb" );

	TCHAR* strMeshData = NULL;

	if (tmpFile)
	{
		fseek(tmpFile, 0, SEEK_END);
		uint32 size = ftell(tmpFile);
		fseek(tmpFile, 0, SEEK_SET);

		strMeshData = new TCHAR[size];
		fread( strMeshData, 1, size, tmpFile );

		fclose(tmpFile);
	}

	// File input
	TCHAR strCommand[256] = {0};
	gkStdStringstream InFile( strMeshData );

	if( !InFile )
	{
		delete[] strMeshData;
		return false;
	}
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
			m_positions.push_back( Vec3( x, y, z ) );
		}
		else if( 0 == _tcscmp( strCommand, _T("vt") ) )
		{
			// Vertex TexCoord
			float u, v;
			InFile >> u >> v;
			m_texcoords.push_back( Vec2( u, 1-v ) );
		}
		else if( 0 == _tcscmp( strCommand, _T("vn"))  )
		{
			// Vertex Normal
// 			float x, y, z;
// 			InFile >> x >> y >> z;
// 			Normals.push_back( Vec3( x, y, z ) );
		}
		else if( 0 == _tcscmp( strCommand, _T("f") ) )
		{
			// Face
			int iPosition, iTexCoord, iNormal;
			gmfFace_t2 face;
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

			m_faces.push_back(face);
		}
		else if ( 0 == _tcscmp( strCommand, _T("usemtl") ) )
		{
			if (!m_subsets.empty())
			{
				m_subsets.back().faceCount = m_faces.size() * 3 - m_subsets.back().faceStart;
			}

			gmfSubset subset;
			subset.faceStart = m_faces.size() * 3;

			m_subsets.push_back(subset);
		}
		else
		{
			// Unimplemented or unrecognized command
		}

		InFile.ignore( 1000, '\n' );
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

	delete[] strMeshData;




	return true;
}


bool gmfCompiler::writeFile(const char* filename, const char* cfg)
{
	// write data

	// 创建header
	gmfHeaderV1 header;

	memset(&header, 0, sizeof(header));
	memcpy( header.identityExt, "GMF1", 4);
	header.gmf_header_version = GMF_VER_1_0;

	header.vertexPositionCount = m_positions.size();
	header.vertexTexcoordCount[0] = m_texcoords.size();
	header.faceCount = m_faces.size();
	header.subsetCount = m_subsets.size();
	header.dwordIndex = ( m_positions.size() > (1 << 16) ) || ( m_texcoords.size() > (1 << 16) );

	// 打开文件写入
	FILE* file = fopen( filename, "wb" );
	if ( !file)
	{
		printf("FATAL ERROR! 创建文件失败\n");
		return -1;
	}
	
	fseek( file, 0, SEEK_SET );
	// 写header
	fwrite( &header, 1, sizeof(gmfHeaderV1), file );
	// 写vertex
	fwrite( m_positions.data(), 1, m_positions.size() * sizeof(Vec3), file );
	// 写texcoord
	fwrite( m_texcoords.data(), 1, m_texcoords.size() * sizeof(Vec2), file );
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

	return 0;
}


bool gmfCompiler::decode()
{
	if (m_dataPtr == NULL)
	{
		return false;
	}


	return true;
}

bool gmfCompiler::supportInput( const char* ext )
{
	if (!stricmp(ext, ".obj"))
	{
		return true;
	}
	return false;
}

bool gmfCompiler::supportOutput( const char* ext )
{
	if (!stricmp(ext, ".gmf"))
	{
		return true;
	}
	return false;
}

const char* gmfCompiler::getName()
{
	return "gmfCompiler";
}
