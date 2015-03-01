#include "RendererD3D9StableHeader.h"
#include "gkMesh.h"
#include "gkFilePath.h"

#include "ITask.h"

#include "gkMemoryLeakDetecter.h"

struct gkMeshStreamingTask : public ITask
{
	gkMesh* targetMesh;
	gkStdString filename;

	virtual void Execute() 
	{
		targetMesh->lockres();

		// task使用了公共的Loader，要加下锁 [3/27/2013 Kaiming]

		if (!targetMesh->loadObj(targetMesh->m_pDevice, filename.c_str()))
		{
			gkLogWarning(_T("MeshSystem:: obj mesh file load failed. \n"));
			return;
		}

		targetMesh->BindHwBuffer();
		targetMesh->UpdateHwBuffer();

		targetMesh->unlockres();
	}

};

struct gkHavokMeshStreamingTask : public ITask
{
	gkMesh* targetMesh;
	gkStdString filename;

	virtual void Execute() 
	{
		// task使用了公共的Loader，要加下锁 [3/27/2013 Kaiming]

		targetMesh->loadHavokMesh(targetMesh->m_pDevice, filename.c_str());

		targetMesh->BindHwBuffer();
		targetMesh->UpdateHwBuffer();
	}

};

//-----------------------------------------------------------------------
gkMesh::gkMesh( IResourceManager* creator, const gkStdString& name, gkResourceHandle handle, const gkStdString& group /*= _T("none")*/, gkNameValuePairList* params )
			:IMesh(creator,name,handle,group)
			,m_wstrExtName(_T("gkm"))
			,m_subsetCount(0)
			,m_pVB(0)
			,m_pIB(0)
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

	m_AABB.Reset();

	m_ready = false;
}
//-----------------------------------------------------------------------
gkMesh::~gkMesh( void )
{
	if(isLoaded())
		unload();
}
//-----------------------------------------------------------------------
bool gkMesh::loadImpl( void )
{
	m_loadingTask = NULL;

	// 现在是最简单的调用内部的loadImpl [9/17/2010 Kaiming-Laptop]
	IDirect3DDevice9* pDevice = getRenderer()->getDevice();
	return loadImpl(pDevice);
}
//-----------------------------------------------------------------------
bool gkMesh::loadImpl( IDirect3DDevice9* d3d9Device )
{
	HRESULT hr = S_OK;
	int nStartTime = timeGetTime();
	m_pDevice = d3d9Device;
	subsets.clear();

	// 如果是手动创建
	// if defaultpool, just create
	bool bit16 = false;

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
		if ( it->second == _T("P3T2U4") )
		{
			vertexType = eVI_P3T2U4;
			elementSize = sizeof(GKVL_P3T2U4);
		}
		else
		if ( it->second == _T("Pt2T2T2T2T2") )
		{
			vertexType = eVI_PT2T2T2T2T2;
			elementSize = sizeof(GKVL_Pt2T2T2T2T2);
			bit16 = true;
		}
		m_pVB = new gkVertexBuffer( elementSize, 0, vertexType, eBF_Discard );
		m_pIB = new gkIndexBuffer( 0, bit16 );

		// internal loading
		BindHwBuffer();
		//UpdateHwBuffer();
	}
	//////////////////////////////////////////////////////////////////////////
	// 从文件创建
	//////////////////////////////////////////////////////////////////////////
	else
	{
		// 整理下代码，尼玛太乱了！ [3/26/2013 YiKaiming]


		// 先在目录内查找名字 [9/18/2010 Kaiming-Desktop]
		TCHAR wszPath[MAX_PATH] = _T("");
		// for havok char file, |to mark the sub bindings. [5/3/2012 Kaiming]
		TCHAR wszPathBackup[MAX_PATH] = _T("");
		TCHAR wszBindings[MAX_PATH] = _T("none");
		_tcscpy_s(wszPathBackup, m_wstrFileName.c_str());
		TCHAR* strLastSlash = NULL;
		strLastSlash = _tcsrchr( wszPathBackup, TEXT( '|' ) );
		if( strLastSlash )
		{
			_tcscpy( wszBindings, &strLastSlash[1] );
			*strLastSlash = 0;
		}
		_tcscpy_s( wszPath, wszPathBackup);

		// 读到名字后，分析一下文件类型
		// 弱分析，靠的是扩展名
		TCHAR wszExtendedName[MAX_PATH] = _T("gkm");

		strLastSlash = _tcsrchr( wszPath, TEXT( '.' ) );
		if( strLastSlash )
		{
			_tcscpy( wszExtendedName, &strLastSlash[1] );
			m_wstrExtName = wszExtendedName;
		}

		

		//TODO: 这里以后要加入大小写不敏感！
		// 根据分析的文件扩展名，开始建立mesh
		if ( m_wstrExtName == _T("gmf") || m_wstrExtName == _T("obj") )
		{
			// 先加载gmf二进制文件 [3/25/2013 Kaiming]


			
			TCHAR wszRealLoadName[MAX_PATH];
			_tcscpy( wszRealLoadName, wszPath );
			strLastSlash = _tcsrchr( wszRealLoadName, TEXT( '.' ) );
			if( strLastSlash )
			{
				_tcscpy( strLastSlash, _T(".gmf") );
			}

			IResFile* header = gEnv->pFileSystem->loadResFileHeader( wszRealLoadName, true );
			if ( !header )
			{
				strLastSlash = _tcsrchr( wszRealLoadName, TEXT( '.' ) );
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
			else
			{
				const gmfHeaderV1* geo_info = (const gmfHeaderV1*)header->DataPtr();
				subsets.assign( geo_info->subsetCount, gkMeshSubset() );
			}

			gEnv->pFileSystem->closeResFile( header );

			// 直接创建task，执行
			gkMeshStreamingTask* task = new gkMeshStreamingTask();
			task->targetMesh = this;
			task->filename = wszRealLoadName;

			m_loadingTask = task;

			gEnv->pCommonTaskDispatcher->PushTask( task, 0 );
			//gEnv->pCommonTaskDispatcher->Wait();
		}
		else if ( m_wstrExtName == _T("hkx") || m_wstrExtName == _T("hkt"))
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

	return true;
}
bool gkMesh::unloadImpl( void )
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

	// 手动释放VB, IB
	if (m_pVB)
	{
		IDirect3DVertexBuffer9* hwVB = (IDirect3DVertexBuffer9*)(m_pVB->userData);

		if ( hwVB )
		{
			uint32 refcount = hwVB->Release();
			if (refcount > 0)
			{
				gkLogError( _T("Mesh VB [%s] REFCOUNT not ZERO!"), m_wstrFileName.c_str() );
			}
			hwVB = 0;
		}
		SAFE_DELETE(m_pVB);
	}
	if (m_pIB)
	{
		IDirect3DIndexBuffer9* hwIB = (IDirect3DIndexBuffer9*)(m_pIB->userData);
		
		if ( hwIB )
		{
			uint32 refcount = hwIB->Release();
			if (refcount > 0)
			{
				gkLogError( _T("Mesh VB [%s] REFCOUNT not ZERO!"), m_wstrFileName.c_str() );
			}
			hwIB = 0;
		}

		SAFE_DELETE(m_pIB);
	}

	m_resLock->UnLock();

	return true;
}
//////////////////////////////////////////////////////////////////////////
bool gkMesh::loadHavokMesh( IDirect3DDevice9* d3d9Device, const TCHAR* wszFileName )
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


void gkMesh::UpdateHwBuffer()
{

	m_pVB->markWrite();
	m_pIB->markWrite();

	if (m_pVB->needRebind() || m_pIB->needRebind())
	{
		BindHwBuffer();
	}

	{
		IDirect3DVertexBuffer9* buffer = (IDirect3DVertexBuffer9*)(m_pVB->userData);

		void* gpuData = NULL;
		if ( SUCCEEDED( buffer->Lock( 0, m_pVB->getSize(), &gpuData, NULL /*D3DLOCK_DISCARD*/ ) ) )
		{
			memcpy( gpuData, m_pVB->data, m_pVB->getSize() );

			buffer->Unlock();
		}
	}


	{
		IDirect3DIndexBuffer9* buffer = (IDirect3DIndexBuffer9*)(m_pIB->userData);
		void* gpuData = NULL;
		if ( SUCCEEDED( buffer->Lock( 0, m_pIB->getSize(), &gpuData, NULL /*D3DLOCK_DISCARD*/ ) ) )
		{
			memcpy( gpuData, m_pIB->data, m_pIB->getSize() );

			buffer->Unlock();
		}	
	}

	// create AABB [8/21/2011 Kaiming-Desktop]
	m_AABB.Reset();

	// 计算AABB
	gkMeshUtilComuputeBoundingBox( m_pVB->data, m_pVB->elementSize, 0, m_pVB->elementCount, m_AABB );

	//pVB->ummarkWrite();

	m_ready = true;
}

void gkMesh::ReleaseSysBuffer()
{
	m_pVB->ummarkWrite();
	m_pIB->ummarkWrite();
}

void gkMesh::BindHwBuffer()
{
	// 手动释放VB, IB
	if (m_pVB)
	{
		IDirect3DVertexBuffer9* hwVB = (IDirect3DVertexBuffer9*)(m_pVB->userData);
		SAFE_RELEASE( hwVB );
	}
	if (m_pIB)
	{
		IDirect3DIndexBuffer9* hwIB = (IDirect3DIndexBuffer9*)(m_pIB->userData);
		SAFE_RELEASE( hwIB );
	}

// 	assert( m_pVB->getSize() > 0 );
// 	assert( m_pIB->getSize() > 0 );

	if ( m_pVB->getSize() > 0)
	{
		// bind HW BUFFER
		m_pDevice->CreateVertexBuffer( m_pVB->getSize(), 
			D3DUSAGE_WRITEONLY, 
			NULL, 
			D3DPOOL_DEFAULT, 
			(IDirect3DVertexBuffer9**)(&(m_pVB->userData)), NULL);
	}
	if ( m_pIB->getSize() > 0)
	{
		D3DFORMAT wordbit = D3DFMT_INDEX32;
		if (m_pIB->wordbit)
		{
			wordbit = D3DFMT_INDEX16;
		}


		m_pDevice->CreateIndexBuffer( m_pIB->getSize(), 
			D3DUSAGE_WRITEONLY, 
			wordbit, 
			D3DPOOL_DEFAULT,
			(IDirect3DIndexBuffer9**)(&(m_pIB->userData)),
			NULL );
	}

	//m_ready = true;
	m_pVB->setBind();
	m_pIB->setBind();
}


//-----------------------------------------------------------------------
bool gkMesh::loadObj( IDirect3DDevice9* d3d9Device, const TCHAR* wszFileName )
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

	return true;
}

//-----------------------------------------------------------------------
AABB& gkMesh::GetAABB()
{
	return m_AABB;
}
//-----------------------------------------------------------------------
bool gkMesh::RaycastMesh( Ray& ray, std::vector<float>& dists )
{
	dists.clear();

	bool hit = false;
	if (m_pVB && m_pIB && m_pVB->data && m_pIB->data)
	{
		//float starttime = gEnv->pTimer->GetCurrTime();
		// use Ray_Triangle to do a full round ray test, may cost time
		for (uint32 i=0; i < m_pIB->count / 3; ++i)
		{
			uint32* startIndex = (uint32*)(m_pIB->data);

			uint32 v0Idx = startIndex[i*3 + 0];
			uint32 v1Idx = startIndex[i*3 + 1];
			uint32 v2Idx = startIndex[i*3 + 2];

			Vec3* v0 = (Vec3*)(m_pVB->data + m_pVB->elementSize * v0Idx);
			Vec3* v1 = (Vec3*)(m_pVB->data + m_pVB->elementSize * v1Idx);
			Vec3* v2 = (Vec3*)(m_pVB->data + m_pVB->elementSize * v2Idx);
			Vec3 out;
			out.zero();

			
			if ( Intersect::Ray_Triangle( ray, *v0, *v2, *v1, out ) )
			{
				hit = true;
			}
			
			if (hit)
			{
				// get distance an put
				float dist = (out - ray.origin).GetLength();
				dists.push_back(dist);
				break;
			}

		}

		//gkLogMessage( _T("RayCastMesh use %.2fms"), (gEnv->pTimer->GetCurrTime() - starttime) * 1000.f);
	}
	return hit;
}

bool gkMesh::RaycastMesh_WorldSpace(Ray& ray, Matrix34& world_mat, std::vector<float>& dists)
{
	Ray rayMs = ray;
	Matrix34 world_mat_inv = world_mat.GetInverted();
	rayMs.origin = world_mat_inv.TransformPoint( rayMs.origin );
	rayMs.direction = world_mat_inv.TransformVector( rayMs.direction );

	dists.clear();

	bool hit = false;
	if (m_pVB && m_pIB && m_pVB->data && m_pIB->data)
	{
		//float starttime = gEnv->pTimer->GetCurrTime();
		// use Ray_Triangle to do a full round ray test, may cost time
		for (uint32 i=0; i < m_pIB->count / 3; ++i)
		{
			uint32* startIndex = (uint32*)(m_pIB->data);

			uint32 v0Idx = startIndex[i*3 + 0];
			uint32 v1Idx = startIndex[i*3 + 1];
			uint32 v2Idx = startIndex[i*3 + 2];

			Vec3* v0 = (Vec3*)(m_pVB->data + m_pVB->elementSize * v0Idx);
			Vec3* v1 = (Vec3*)(m_pVB->data + m_pVB->elementSize * v1Idx);
			Vec3* v2 = (Vec3*)(m_pVB->data + m_pVB->elementSize * v2Idx);
			Vec3 out;
			out.zero();


			if ( Intersect::Ray_Triangle( rayMs, *v0, *v2, *v1, out ) )
			{
				hit = true;
			}

			if (hit)
			{
				// get distance an put
				Vec3 hitted = world_mat.TransformVector(out - rayMs.origin);

				float dist = hitted.GetLength();
				dists.push_back(dist);
				break;
			}

		}

		//gkLogMessage( _T("RayCastMesh use %.2fms"), (gEnv->pTimer->GetCurrTime() - starttime) * 1000.f);
	}
	return hit;
}

//////////////////////////////////////////////////////////////////////////
void gkMesh::getRenderOperation( gkRenderOperation& op, uint32 subset )
{
	if (m_ready)
	{
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

			if (m_customPrimCount != -1)
			{
				if (m_customPrimCount == 0)
				{
					op.operationType = gkRenderOperation::OT_SKIP;
				}
				op.indexCount = m_customPrimCount * 3;
			}
			else
			{
				op.indexCount = m_pIB->count;
			}

		}
	}
	else
	{
		op.operationType = gkRenderOperation::OT_SKIP;
	}

	


}
//////////////////////////////////////////////////////////////////////////
IMesh::BoneBaseTransforms& gkMesh::GetBoneBaseTransforms()
{
	return m_BoneBaseTransforms;
}
//////////////////////////////////////////////////////////////////////////
uint32 gkMesh::getSubsetCount()
{
	return subsets.size();
}
//////////////////////////////////////////////////////////////////////////
gkVertexBuffer* gkMesh::getVertexBuffer()
{
	if(m_pVB)
		m_pVB->markWrite();
	return m_pVB;
}
//////////////////////////////////////////////////////////////////////////
gkIndexBuffer* gkMesh::getIndexBuffer()
{
	if(m_pIB)
		m_pIB->markWrite();
	return m_pIB;
}

void gkMesh::modifyCustomPrimitiveCount( uint32 count )
{
	m_customPrimCount = count;
}

void gkMesh::onReset()
{
	if( m_resLock->TryLock() )
	{
		gEnv->pCommonTaskDispatcher->CancelTask( m_loadingTask );
		m_loadingTask = NULL;
	}
	else
	{
		m_resLock->Lock();
	}	

	BindHwBuffer();
	UpdateHwBuffer();

	if (m_pResetCallback)
	{
		m_pResetCallback->OnResetCallBack();
	}

	m_resLock->UnLock();
}

void gkMesh::onLost()
{
	if (m_pVB)
	{
		IDirect3DVertexBuffer9* hwVB = (IDirect3DVertexBuffer9*)(m_pVB->userData);

		if ( hwVB )
		{
			uint32 refcount = hwVB->Release();
			if (refcount > 0)
			{
				gkLogError( _T("Mesh VB [%s] REFCOUNT not ZERO!"), m_wstrFileName.c_str() );
			}
			hwVB = 0;
			m_pVB->userData = 0;
		}
	}
	if (m_pIB)
	{
		IDirect3DIndexBuffer9* hwIB = (IDirect3DIndexBuffer9*)(m_pIB->userData);

		if ( hwIB )
		{
			uint32 refcount = hwIB->Release();
			if (refcount > 0)
			{
				gkLogError( _T("Mesh VB [%s] REFCOUNT not ZERO!"), m_wstrFileName.c_str() );
			}
			hwIB = 0;
			m_pIB->userData = 0;
		}
	}
}





