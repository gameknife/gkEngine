#include "StableHeader.h"
#include "gkHkxMeshLoader.h"
#include "gkHavokAssetManagementUtil.h"
#include <Animation/Animation/hkaAnimationContainer.h>
#include <Common/Serialize/Util/hkLoader.h>
#include <Common/Serialize/Util/hkRootLevelContainer.h>

gkHkxMeshLoader::gkHkxMeshLoader(void)
{
	m_cachedVB = NULL;
	m_cachedIB = NULL;
}


gkHkxMeshLoader::~gkHkxMeshLoader(void)
{
}

bool gkHkxMeshLoader::LoadGeometry( const TCHAR* filename )
{
	// here may split name & mesh binding

 	CHAR szPath[MAX_PATH] = "";
 	#ifdef UNICODE
 	WideCharToMultiByte(CP_ACP, 0, filename, -1, szPath, MAX_PATH, NULL, NULL);
 	#else
 	_tcscpy_s( szPath, MAX_PATH, filename );
 	#endif 
 	hkStringBuf assetFile(szPath); hkAssetManagementUtil::getFilePath(assetFile);

	hkRootLevelContainer* container = getAnimationPtr()->getGlobalLoader()->load( szPath );
	HK_ASSERT2(0x27343437, container != HK_NULL , "Could not load asset");

	if (!container)
	{
		return false;
	}

	hkaAnimationContainer* ac = reinterpret_cast<hkaAnimationContainer*>( container->findObjectByType( hkaAnimationContainerClass.getName() ));
	HK_ASSERT2(0x27343435, ac && (ac->m_skins.getSize() > 0), "No skins loaded");

// 	for ( int bindingIdx = 0; bindingIdx < ac->m_skins.getSize(); ++bindingIdx )
// 	{
		hkaMeshBinding* meshbinding = ac->m_skins[0];
		int tris = 0;
		uint32 prevvertexcount = 0;

		// split by mesh
		for(int meshatt = 0; meshatt < meshbinding->m_mesh->m_sections.getSize(); ++meshatt)
		{

			// create subset
			gkMeshSubset subset;
			//subset.mtlname = gkStdString(meshsection->m_material->m_name);
			subset.indexStart = tris * 3;


			uint32 totalvertex = 0;
			uint32 totaltrangle = 0;
			uint32 totalsubset = 0;

			

			hkxMeshSection* meshsection = meshbinding->m_mesh->m_sections[meshatt];

			const hkxVertexDescription& vdesc = meshsection->m_vertexBuffer->getVertexDesc();
			const hkxVertexDescription::ElementDecl* pdecl = vdesc.getElementDecl(hkxVertexDescription::HKX_DU_POSITION, 0);
			const hkxVertexDescription::ElementDecl* ndecl = vdesc.getElementDecl(hkxVertexDescription::HKX_DU_NORMAL, 0);
			const hkxVertexDescription::ElementDecl* tdecl = vdesc.getElementDecl(hkxVertexDescription::HKX_DU_TEXCOORD, 0);
			const hkxVertexDescription::ElementDecl* tgdecl = vdesc.getElementDecl(hkxVertexDescription::HKX_DU_TANGENT, 0);
			const hkxVertexDescription::ElementDecl* bndecl = vdesc.getElementDecl(hkxVertexDescription::HKX_DU_BINORMAL, 0);
			const hkxVertexDescription::ElementDecl* bwdecl = vdesc.getElementDecl(hkxVertexDescription::HKX_DU_BLENDWEIGHTS, 0);
			const hkxVertexDescription::ElementDecl* bidecl = vdesc.getElementDecl(hkxVertexDescription::HKX_DU_BLENDINDICES, 0);

			hkxVertexBuffer* vbuf = meshsection->m_vertexBuffer;
			totalvertex += vbuf->getNumVertices();
			totaltrangle += meshsection->getNumTriangles();
			totalsubset++;

			void* pBuffer = vbuf->getVertexDataPtr((*pdecl));
			void* nBuffer = vbuf->getVertexDataPtr((*ndecl));
			void* tBuffer = vbuf->getVertexDataPtr((*tdecl));
			void* tgBuffer = vbuf->getVertexDataPtr((*tgdecl));
			void* bnBuffer = vbuf->getVertexDataPtr((*bndecl));
			void* bwBuffer = vbuf->getVertexDataPtr((*bwdecl));
			void* biBuffer = vbuf->getVertexDataPtr((*bidecl));

			if ( !pBuffer || !nBuffer || !tBuffer || !bwBuffer || !biBuffer )
			{
				assert(0);
				continue;
			}

			for (int i=0; i < vbuf->getNumVertices(); ++i)
			{
				float* pPosStart = (float*)((char*)pBuffer + (pdecl->m_byteStride * i));
				float* pNormalStart = (float*)((char*)nBuffer + (ndecl->m_byteStride * i));
				float* pTCStart = (float*)((char*)tBuffer + (tdecl->m_byteStride * i));
				float* pTGStart = (float*)((char*)tgBuffer + (tgdecl->m_byteStride * i));
				float* pBNStart = (float*)((char*)bnBuffer + (bndecl->m_byteStride * i));
				uint8* pBWStart = (uint8*)((char*)bwBuffer + (bwdecl->m_byteStride * i));
				uint8* pBIStart = (uint8*)((char*)biBuffer + (bidecl->m_byteStride * i));

				GKVL_P3F2F4F3F4U4 VERTEX;

				VERTEX.position = Vec3(pPosStart[0], pPosStart[1], pPosStart[2]);
				
				VERTEX.tangent_ti = Vec4(pTGStart[0], pTGStart[1], pTGStart[2], 1);
				VERTEX.binormal = Vec3(-pBNStart[0], -pBNStart[1], -pBNStart[2]);

				VERTEX.texcoord = Vec2(pTCStart[0], 1 - pTCStart[1]);

				Vec3 crossNormal = VERTEX.tangent_ti.GetXYZ() % VERTEX.binormal;
				Vec3 normal = Vec3(pNormalStart[0], pNormalStart[1], pNormalStart[2]);
				if ( crossNormal.dot(normal) < 0 )
				{
					VERTEX.tangent_ti.w = -1;
				}
				else
				{
					VERTEX.tangent_ti.w = 1;
				}

				VERTEX.blendWeight = Vec4_tpl<uint8>(pBWStart[0], pBWStart[1], pBWStart[2], pBWStart[3]);
				VERTEX.blendIndice = Vec4_tpl<uint8>((uint8)pBIStart[0], (uint8)pBIStart[1], (uint8)pBIStart[2], (uint8)pBIStart[3]);

				m_Vertices.push_back(VERTEX);
			}

			for (uint32 i=0; i < meshsection->getNumTriangles(); ++i)
			{
				hkUint32 a,b,c;
				meshsection->getTriangleIndices(i, a, b, c);

				a += prevvertexcount;
				b += prevvertexcount;
				c += prevvertexcount;

				m_Indices.push_back(a);
				m_Indices.push_back(b);
				m_Indices.push_back(c);

				tris++;
			}

			prevvertexcount += vbuf->getNumVertices();

			subset.indexCount = tris * 3 - subset.indexStart;

			m_Subsets.push_back(subset);
		}


		uint32 bones = meshbinding->m_boneFromSkinMeshTransforms.getSize();
		m_boneBaseTransforms.clear();
		for (uint32 i=0; i < bones; ++i)
		{
			Matrix44A* matrix = new Matrix44A;
			meshbinding->m_boneFromSkinMeshTransforms[i].get4x4ColumnMajor( &(matrix->m00) );

			m_boneBaseTransforms.push_back(*matrix);

			delete matrix;
		}

		// CreateMesh

		// create vb & ib
		m_cachedVB = new gkVertexBuffer( sizeof(GKVL_P3F2F4F3F4U4), m_Vertices.size(), eVI_P4F4F4F4U4, eBF_Discard );
		m_cachedIB = new gkIndexBuffer( m_Indices.size() );

		memcpy( m_cachedVB->data, m_Vertices.data(), sizeof(GKVL_P3F2F4F3F4U4) * m_Vertices.size() );
		memcpy( m_cachedIB->data, m_Indices.data(), sizeof(uint32) * m_Indices.size() );

	ac->removeReference();

	return true;
}

gkVertexBuffer* gkHkxMeshLoader::getVB()
{
	return m_cachedVB;
}

gkIndexBuffer* gkHkxMeshLoader::getIB()
{
	return m_cachedIB;
}

uint32 gkHkxMeshLoader::getSubsetCount()
{
	return m_Subsets.size();
}

void gkHkxMeshLoader::getSubset( uint32 index, uint32& start, uint32& count, gkStdString& mtlname )
{
	GK_ASSERT(index < m_Subsets.size());
	start = m_Subsets[index].indexStart;
	count = m_Subsets[index].indexCount;
	mtlname = m_Subsets[index].mtlname;
}

void gkHkxMeshLoader::FinishLoading()
{
	Destroy();
}

void gkHkxMeshLoader::Destroy()
{
	m_Subsets.clear();
	m_boneBaseTransforms.clear();

	m_Vertices.clear();
	m_Indices.clear();

	SAFE_DELETE( m_cachedVB );
	SAFE_DELETE( m_cachedIB );
}

void gkHkxMeshLoader::GetBoneBaseTransform( Matrix44& matrix, uint32 index )
{
	GK_ASSERT( index < m_boneBaseTransforms.size() );

	matrix = m_boneBaseTransforms[index];
}
