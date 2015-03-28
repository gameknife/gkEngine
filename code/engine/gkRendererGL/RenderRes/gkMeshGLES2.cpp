#include "StableHeader.h"
#include "gkMeshGLES2.h"
#include "gkFilePath.h"

#include "ITimer.h"
#include "IResFile.h"

#include "ITask.h"

#include "../gkRendererGL330.h"

struct gkMeshStreamingTask : public ITask
{


	gkMeshGLES2* targetMesh;
	gkStdString filename;

	virtual void Execute() 
	{
		//getRenderer()->getDeviceContex()->makeThreadContext();

		targetMesh->lockres();

		if (!targetMesh->loadObj( filename.c_str()))
		{
			gkLogWarning(_T("MeshSystem:: obj mesh file load failed. \n"));
			return;
		}

		targetMesh->unlockres();

		//getRenderer()->getDeviceContex()->makeThreadContext(true);
	}

	virtual void Complete_MT()
	{
		targetMesh->BindHwBuffer();
		targetMesh->UpdateHwBuffer();
        targetMesh->ReleaseSysBuffer();
	}

};
struct gkHavokMeshStreamingTask : public ITask
{
	gkMeshGLES2* targetMesh;
	gkStdString filename;

	virtual void Execute()
	{
		// task使用了公共的Loader，要加下锁 [3/27/2013 Kaiming]

		targetMesh->loadHavokMesh(filename.c_str());

		targetMesh->BindHwBuffer();
		targetMesh->UpdateHwBuffer();
	}

};
//-----------------------------------------------------------------------
gkMeshGLES2::gkMeshGLES2( IResourceManager* creator, const gkStdString& name, gkResourceHandle handle, const gkStdString& group /*= _T("none")*/, gkNameValuePairList* params )
	:IMesh(creator,name,handle,group)
	,m_wstrExtName(_T("gkm"))
	,m_customPrimCount(-1)
{
	if (params)
	{
		loadingParams = *params;
	}

	//finally, cut the .xxx
	int p = m_wstrShortName.rfind(_T("gmp_"));
	if (p >= 0)
	{
		m_wstrShortName = m_wstrShortName.substr(p+4);
	}

}
//-----------------------------------------------------------------------
gkMeshGLES2::~gkMeshGLES2( void )
{
	if(isLoaded())
		unload();
}

//-----------------------------------------------------------------------
bool gkMeshGLES2::loadImpl( void )
{
	m_loadingTask = NULL;
	m_bufferReady = false;

	HRESULT hr = S_OK;
	//int nStartTime = timeGetTime();
	subsets.clear();

	// 如果是手动创建
	// if defaultpool, just create
	gkNameValuePairList::iterator it = loadingParams.find(_T("type"));
	if (it != loadingParams.end())
	{
		EVertexInputType vertexType = eVI_P4F4F4;
		uint32 elementSize = sizeof(GKVL_P3F2F4F3);

		if (it->second == _T("PARTICLE_VB"))
		{
			vertexType = eVI_T2T2;
			elementSize = sizeof(GKVL_GpuParticle);
		}
		else
			if (it->second == _T("GRASS_BLOCK"))
			{
				vertexType = eVI_P4F4F4;
				elementSize = sizeof(GKVL_P3F2F4F3);
			}
			// this may not a managed texture
			else
				if (it->second == _T("TERRIAN_BLOCK"))
				{
					vertexType = eVI_P4F4F4;
					elementSize = sizeof(GKVL_P3F2F4F3);
				}
				else
					//if ( _T("P3T2U4") )
					{
						vertexType = eVI_P3T2U4;
						elementSize = sizeof(GKVL_P3T2U4);
					}

		m_pVB = new gkVertexBuffer( elementSize, 0, vertexType, eBF_Discard );
		m_pIB = new gkIndexBuffer( 0 );

		// internal loading
		BindHwBuffer();
	}
	else
	{
		// 先在目录内查找名字 [9/18/2010 Kaiming-Desktop]
		TCHAR wszPath[MAX_PATH] = _T("");


		// for havok char file, |to mark the sub bindings. [5/3/2012 Kaiming]
		TCHAR wszPathBackup[MAX_PATH] = _T("");
		TCHAR wszBindings[MAX_PATH] = _T("none");

		_tcscpy_s(wszPathBackup, m_wstrFileName.c_str());


		TCHAR* strLastSlash = NULL;
		strLastSlash = _tcsrchr( wszPathBackup, _T( '|' ) );
		if( strLastSlash )
		{
			_tcscpy_s( wszBindings, MAX_PATH, &strLastSlash[1] );
			*strLastSlash = 0;
		}

		_tcscpy_s( wszPath, wszPathBackup);

		// 读到名字后，分析一下文件类型
		// 弱分析，靠的是扩展名
		TCHAR wszExtendedName[MAX_PATH] = _T("gkm");

		strLastSlash = _tcsrchr( wszPath, _T( '.' ) );
		if( strLastSlash )
		{
			_tcscpy_s( wszExtendedName, MAX_PATH, &strLastSlash[1] );
			m_wstrExtName = wszExtendedName;
		}



		//TODO: 这里以后要加入大小写不敏感！
		// 根据分析的文件扩展名，开始建立mesh
		if ( m_wstrExtName == _T("gmf") || m_wstrExtName == _T("obj") )
		{
			TCHAR wszRealLoadName[MAX_PATH];
			_tcscpy( wszRealLoadName, wszPath );
			strLastSlash = _tcsrchr( wszRealLoadName, _T( '.' ) );
			if( strLastSlash )
			{
				_tcscpy( strLastSlash, _T(".gmf") );
			}

			IResFile* header = gEnv->pFileSystem->loadResFileHeader( wszRealLoadName, true );
			if ( !header )
			{
				strLastSlash = _tcsrchr( wszRealLoadName, _T( '.' ) );
				if( strLastSlash )
				{
					_tcscpy( strLastSlash, _T(".obj") );
				}

				header = gEnv->pFileSystem->loadResFileHeader( wszRealLoadName, true );
				if ( !header)
				{
					gkLogWarning(_T("MeshSystem:: obj mesh file load failed. \n"));
					return false;
				}
			}


			const gmfHeaderV1* geo_info = (const gmfHeaderV1*)header->DataPtr();


			subsets.assign( geo_info->subsetCount, gkMeshSubset() );

			gEnv->pFileSystem->closeResFile( header );


			gkMeshStreamingTask* task = new gkMeshStreamingTask;
			task->targetMesh = this;
			task->filename = wszRealLoadName;

			m_loadingTask = task;

			gEnv->pCommonTaskDispatcher->PushTask(task, -1);

			return true;

		}
		else if( m_wstrExtName == _T("hkx") || m_wstrExtName == _T("hkt") )
		{
			// hkx暂时就不要多线程加载了 [3/26/2013 YiKaiming]

			// 如果是havok Mesh

			// 直接创建task，执行
			gkHavokMeshStreamingTask* task = new gkHavokMeshStreamingTask();
			task->targetMesh = this;
			task->filename = wszPath;

			gEnv->pCommonTaskDispatcher->PushTask( task, -2 );
		}
		else
		{
			assert(0);
			gkLogWarning(_T("MeshSystem:: unknown mesh file. \n"));
			return false;
		}
	}



	//int nTimeLoadFile = timeGetTime() - nStartTime;
	//gkLogMessage(_T("MeshSystem:: mesh [ %s ] loaded. time: %dms. \n"), m_wstrFileName.c_str(), nTimeLoadFile);

	return true;
}
bool gkMeshGLES2::unloadImpl( void )
{
	if( m_resLock->TryLock() )
	{
		gEnv->pCommonTaskDispatcher->CancelTask( m_loadingTask );
		m_loadingTask = NULL;

		if ( !m_loadingtasks.empty() )
		{
			for (uint32 i=0; i <m_loadingtasks.size(); ++i)
			{
				gEnv->pCommonTaskDispatcher->CancelTask( m_loadingtasks[i] );
			}			
			m_loadingtasks.clear();
		}
	}
	else
	{
		m_resLock->Lock();
	}	

	if (m_pVB)
	{
		glDeleteBuffers(1, (GLuint*)(&(m_pVB->userData)));
		SAFE_DELETE(m_pVB);
	}
	if (m_pIB)
	{
		glDeleteBuffers(1, (GLuint*)(&(m_pIB->userData)));
		SAFE_DELETE(m_pIB);
	}

	m_resLock->UnLock();

	return true;
}


//-----------------------------------------------------------------------
bool gkMeshGLES2::loadObj( const TCHAR* wszFileName )
{
	// 建立一个GAMEKNIFE型的MeshLoader
	IMeshLoader* loader = gEnv->pSystem->getOBJMeshLoader();
	if (!loader)
	{
		gkLogError(_T("OBJLoader Missing!") );
		return false;
	}

	// 将文件内容读到缓存中
	if( !loader->LoadGeometry(wszFileName) )
	{
		loader->FinishLoading();
		gEnv->pSystem->ReturnMeshLoader(loader);
		return false;
	}

	gkVertexBuffer* loaderVB = loader->getVB();
	gkIndexBuffer* loaderIB = loader->getIB();

	m_pVB = new gkVertexBuffer(loaderVB->elementSize, loaderVB->elementCount, loaderVB->vertexType, eBF_Discard);
	memcpy(m_pVB->data, loaderVB->data, loaderVB->getSize() );

	// GLES2 only support 16bit index
	m_pIB = new gkIndexBuffer(loaderIB->count, false);
	m_pIB->Clear();

	gkLogMessage(_T("GLESMesh vertexbuffer created."));

	for (uint32 i=0; i < loaderIB->count; ++i)
	{
		m_pIB->push_back( loaderIB->getData(i) );
	}

	gkLogMessage(_T("GLESMesh indexbuffer created."));

	subsets.clear();

	for (uint32 i=0; i < loader->getSubsetCount(); ++i)
	{
		gkMeshSubset subset;
		loader->getSubset(i, subset.indexStart, subset.indexCount, subset.mtlname);
		subsets.push_back( subset );
	}

	gkLogMessage(_T("GLESMesh subset %d, vert %d, tri %d"), loader->getSubsetCount(), m_pVB->elementCount, m_pIB->count / 3);

	loader->FinishLoading();

	gkLogMessage(_T("GLESMesh loaded."));
	gEnv->pSystem->ReturnMeshLoader(loader);

	return true;
}

bool gkMeshGLES2::loadHavokMesh( const TCHAR* wszFileName )
{
	// 建立一个GAMEKNIFE型的MeshLoader
	IMeshLoader* loader = gEnv->pSystem->getHKXMeshLoader();
	if (!loader)
	{
		gkLogError(_T("HKXLoader Missing!") );
		return false;
	}

	// 将文件内容读到缓存中
	loader->LoadGeometry(wszFileName);

	m_BoneBaseTransforms.clear();
	for (uint32 i=0; i < loader->GetBoneBaseTransformCount(); ++i)
	{
		Matrix44 matrix;
		loader->GetBoneBaseTransform(matrix, i);
		m_BoneBaseTransforms.push_back(matrix);
	}



	gkVertexBuffer* loaderVB = loader->getVB();
	gkIndexBuffer* loaderIB = loader->getIB();

	m_pVB = new gkVertexBuffer(loaderVB->elementSize, loaderVB->elementCount, loaderVB->vertexType, eBF_Discard);
	memcpy(m_pVB->data, loaderVB->data, loaderVB->getSize() );

	m_pIB = new gkIndexBuffer(loaderIB->count);
	memcpy(m_pIB->data, loaderIB->data, loaderIB->getSize() );

	subsets.clear();

	for (uint32 i=0; i < loader->getSubsetCount(); ++i)
	{
		gkMeshSubset subset;
		loader->getSubset(i, subset.indexStart, subset.indexCount, subset.mtlname);
		subsets.push_back( subset );
	}


	loader->FinishLoading();

	gEnv->pSystem->ReturnMeshLoader(loader);
	return false;
}

//-----------------------------------------------------------------------
AABB& gkMeshGLES2::GetAABB()
{
	return m_AABB;
}
//-----------------------------------------------------------------------
bool gkMeshGLES2::RaycastMesh( Ray& ray, std::vector<float>& dists )
{
	return false;
}

bool gkMeshGLES2::RaycastMesh_WorldSpace(Ray& ray, Matrix34& world_mat, std::vector<float>& dists)
{
	return false;
}

//////////////////////////////////////////////////////////////////////////
void gkMeshGLES2::getRenderOperation( gkRenderOperation& op, uint32 subset )
{
	if (!m_bufferReady) {
		op.operationType = gkRenderOperation::OT_SKIP;
		return;
	}
	op.operationType = gkRenderOperation::OT_TRIANGLE_LIST;
	op.vertexData = m_pVB;
	op.vertexStart = 0;
	op.vertexCount = m_pVB->elementCount;
	op.vertexSize = m_pVB->elementSize;

	op.useIndexes = true;
	op.indexData = m_pIB;

	if ( subsets.size() > 0  )
	{
		op.indexStart = subsets[subset].indexStart;
		op.indexCount = subsets[subset].indexCount;
	}
	else
	{
		op.indexStart = 0;
		op.indexCount = m_pIB->count;
		if (m_customPrimCount != -1)
		{
			if (m_customPrimCount == 0)
			{
				op.operationType = gkRenderOperation::OT_SKIP;
			}
			op.indexCount = m_customPrimCount * 3;
		}

	}
}
//////////////////////////////////////////////////////////////////////////
IMesh::BoneBaseTransforms& gkMeshGLES2::GetBoneBaseTransforms()
{
	return m_BoneBaseTransforms;
}
//////////////////////////////////////////////////////////////////////////
uint32 gkMeshGLES2::getSubsetCount()
{
	return subsets.size();
}
//////////////////////////////////////////////////////////////////////////
bool gkMeshGLES2::IsIndexDword()
{
	return false;
}
//////////////////////////////////////////////////////////////////////////
gkVertexBuffer* gkMeshGLES2::getVertexBuffer()
{
	m_pVB->markWrite();
	return m_pVB;
}
//////////////////////////////////////////////////////////////////////////
gkIndexBuffer* gkMeshGLES2::getIndexBuffer()
{
	m_pIB->markWrite();
	return m_pIB;
}

void gkMeshGLES2::BindHwBuffer()
{
	// create VBO for this mesh
	glGenBuffers(1, (GLuint*)(&(m_pVB->userData)));

	// create VIO for this mesh
	glGenBuffers(1,(GLuint*)(&(m_pIB->userData)));
}

void gkMeshGLES2::UpdateHwBuffer()
{
	if (m_pVB->userData)
	{
// #ifdef OS_WIN32
// 		glDeleteBuffers(1, (GLuint*)(&(m_pVB->userData)));
// #endif
	}

	// Bind the VBO
	glBindBuffer(GL_ARRAY_BUFFER, (GLuint)(m_pVB->userData));

	// Set the buffer's data
	glBufferData(GL_ARRAY_BUFFER, m_pVB->elementCount * m_pVB->elementSize, m_pVB->data, GL_DYNAMIC_DRAW);

	// Unbind the VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);

 	if (m_pIB->userData)
 	{
// #ifdef OS_WIN32
//  	glDeleteBuffers(1, (GLuint*)(&(m_pIB->userData)));
// #endif
 	}
 
 	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (GLuint)(m_pIB->userData));
 
 	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_pIB->count * sizeof(uint32), (uint32*)(m_pIB->data) , GL_DYNAMIC_DRAW );
 
 	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// create AABB [8/21/2011 Kaiming-Desktop]
	m_AABB.Reset();

	// 计算AABB
	gkMeshUtilComuputeBoundingBox( m_pVB->data, m_pVB->elementSize, 0, m_pVB->elementCount, m_AABB );

	m_bufferReady = true;
}

void gkMeshGLES2::modifyCustomPrimitiveCount( uint32 count )
{
	m_customPrimCount = count;
}

void gkMeshGLES2::ReleaseSysBuffer()
{
	m_pVB->ummarkWrite();
    m_pIB->ummarkWrite();
	//pIB->ummarkWrite();
}
