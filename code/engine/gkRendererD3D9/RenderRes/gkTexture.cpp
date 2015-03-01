#include "RendererD3D9StableHeader.h"
#include "gkTexture.h"
#include "gkIterator.h"
#include "gkFilePath.h"
#include "gkRendererD3D9.h"
#include "DDS.h"

#include "ITask.h"

#include "gkMemoryLeakDetecter.h"

#define RESIZE_POWER m_shrinkLevel

struct gkTextureCUBEStreamingTask : public ITask
{
	IDirect3DDevice9* m_device;
	IDirect3DCubeTexture9** m_targetTexture;
	ITexture* m_texturePtr;

	TCHAR m_filepath[MAX_PATH];

	virtual void Execute() 
	{
		gkAutoLock<gkMutexLock> lock(m_texturePtr->getLock());
		IResFile* pFile = gEnv->pFileSystem->loadResFile( m_filepath, true );

		if (!pFile)
		{
			gkLogError(_T("gkTexture::Loading Texture [%s] Failed."), m_filepath);
			return;
		}

		D3DXCreateCubeTextureFromFileInMemory(m_device, pFile->DataPtr(), pFile->Size(), m_targetTexture );
		gEnv->pFileSystem->closeResFile(pFile);
	}
};

struct gkTexture2DStreamingTask : public ITask
{
	IDirect3DDevice9* m_device;
	IDirect3DTexture9* m_targetTexture;
	ITexture* m_texturePtr;
	TCHAR m_filepath[MAX_PATH];
	uint32 m_shrinkLevel;
	

	virtual void Execute() 
	{
		gkAutoLock<gkMutexLock> lock(m_texturePtr->getLock());

		//////////////////////////////////////////////////////////////////////////
		// 验证下文件，并读取DDS的头
		IResFile* pFile = gEnv->pFileSystem->loadResFile( m_filepath, true );

		if (!pFile)
		{
			gkLogError(_T("gkTexture::Loading Texture [%s] Failed."), m_filepath);
			return;
		}

		// 利用DDS LOADER来创建
		const uint8* start = (const uint8*)(pFile->DataPtr());

		// DDS files always start with the same magic number ("DDS ")
		DWORD dwMagicNumber = *( DWORD* )( start );
		if( dwMagicNumber != DDS_MAGIC )
		{
			return;
		}

		const DDS_HEADER* pHeader = reinterpret_cast<const DDS_HEADER*>( start + sizeof( DWORD ) );

		// Verify header to validate DDS file
		if( pHeader->dwSize != sizeof(DDS_HEADER)
			|| pHeader->ddspf.dwSize != sizeof(DDS_PIXELFORMAT) )
		{
			return;
		}

		// Check for DX10 extension
		bool bDXT10Header = false;
		if ( (pHeader->ddspf.dwFlags & DDS_FOURCC)
			&& (MAKEFOURCC( 'D', 'X', '1', '0' ) == pHeader->ddspf.dwFourCC) )
		{
			return;
		}

		// setup the pointers in the process request
		INT offset = sizeof( DWORD ) + sizeof( DDS_HEADER );

		const uint8* pBitData = start + offset;
		uint32 BitSize = pFile->Size() - offset;

		UINT iWidth = pHeader->dwWidth;
		UINT iHeight = pHeader->dwHeight;
		UINT iMipCount = pHeader->dwMipMapCount;



		if (pHeader->dwCubemapFlags != 0
			|| (pHeader->dwHeaderFlags & DDS_HEADER_FLAGS_VOLUME) )
		{
			// For now only support 2D textures, not cubemaps or volumes
			return;
		}

		D3DFORMAT fmt = GetD3D9Format( pHeader->ddspf );

		D3DLOCKED_RECT LockedRect = {0};
		UINT RowBytes, NumRows;
		const BYTE* pSrcBits = pBitData;

		if (RESIZE_POWER > 0)
		{
			GetSurfaceInfo( iWidth, iHeight, fmt, NULL, &RowBytes, &NumRows );
			for( UINT h = 0; h < NumRows; h++ )
			{
				//CopyMemory( pDestBits, pSrcBits, RowBytes );
				//pDestBits += LockedRect.Pitch;
				pSrcBits += RowBytes;
			}
		}

		iWidth = iWidth >> RESIZE_POWER;
		iHeight = iHeight >> RESIZE_POWER;
		iMipCount = iMipCount - RESIZE_POWER;


		if( 0 >= iMipCount )
			iMipCount = 1;

		// lock and push

		IDirect3DTexture9* lockingTex = NULL;

		if ( gkRendererD3D9::GetManagedTexturePoolType() == D3DPOOL_DEFAULT )
		{
			m_device->CreateTexture( iWidth, iHeight, iMipCount, NULL, fmt, D3DPOOL_SYSTEMMEM, &lockingTex, NULL );
		}
		else
		{
			lockingTex = m_targetTexture;
		}
		
		for( UINT i = 0; i < iMipCount; i++ )
		{
			GetSurfaceInfo( iWidth, iHeight, fmt, NULL, &RowBytes, &NumRows );

			if( SUCCEEDED( lockingTex->LockRect( i, &LockedRect, NULL, 0 ) ) )
			{
				BYTE* pDestBits = ( BYTE* )LockedRect.pBits;

				// Copy stride line by line
				for( UINT h = 0; h < NumRows; h++ )
				{
					CopyMemory( pDestBits, pSrcBits, RowBytes );
					pDestBits += LockedRect.Pitch;
					pSrcBits += RowBytes;
				}

				lockingTex->UnlockRect(i);//UnlockRect( i );
			}
			else
			{
				//gkLogError( _T("Update texture %s failed."), m_filepath );
			}



			iWidth = iWidth >> 1;
			iHeight = iHeight >> 1;
			if( iWidth == 0 )
				iWidth = 1;
			if( iHeight == 0 )
				iHeight = 1;
		}


		lockingTex->SetLOD( 0 );

		if ( gkRendererD3D9::GetManagedTexturePoolType() == D3DPOOL_DEFAULT )
		{
			m_device->UpdateTexture( lockingTex, m_targetTexture );
			lockingTex->Release();
		}
		gEnv->pFileSystem->closeResFile(pFile);

		//gkLogMessage( _T("texture loaded") )
		m_texturePtr->calcMemUsage();
	}
};


gkTexture::gkTexture( IResourceManager* creator, const gkStdString& name, gkResourceHandle handle, const gkStdString& group, gkNameValuePairList* params )
					:ITexture(creator,name,handle,group),
					m_dwD3DPool(gkRendererD3D9::GetManagedTexturePoolType()),
					m_p2DTexture(NULL),
					m_pCubeTexture(NULL),
					m_bIsManaged(false),
					m_bIsDefaultPool(false),
					m_uWidth(1),
					m_uHeight(1),
					m_rawData(NULL),
					m_shrinkLevel(0),
					m_wstrFormat(_T("UnKnown")),
					m_rawDirty(false)
{
	// save the loading params
	if (params)
	{
		loadingParams = *params;
	}

	// creation time, add to file change monitor
	gEnv->pFileChangeMonitor->AddListener(this);
}

gkTexture::~gkTexture( void )
{
// 	if(isLoaded())
// 		unload();
// 
// 	if(m_p2DTexture && !m_bIsManaged)
// 		SAFE_RELEASE( m_p2DTexture );

	gEnv->pFileChangeMonitor->RemoveListener(this);

	//gkAutoLock<gkMutexLock>::s_lockManager[eLGID_Resource].deleteLock( (uint32)this );
}

bool gkTexture::loadImpl( void )
{
	m_loadingTask = NULL;

	// 现在是最简单的调用内部的loadImpl [9/17/2010 Kaiming-Laptop]
	IDirect3DDevice9* pDevice = getRenderer()->getDevice();
	return loadImpl(pDevice);
}

bool gkTexture::loadImpl( IDirect3DDevice9* d3d9Device )
{
	HRESULT hr = S_OK;

	m_bSizeable = false;

	// if defaultpool, just create
	gkNameValuePairList::iterator it = loadingParams.find(_T("d3dpool"));
	if (it != loadingParams.end())
	{
		// this may not a managed texture
		if (it->second == _T("D3DX_DEFAULT"))
		{
			// this is a defaultpool texture, maybe a RTT
			m_bIsDefaultPool = true;
			
			// get texture params
			gkNameValuePairList::iterator itusage = loadingParams.find(_T("usage"));
			if (itusage == loadingParams.end())
			{ gkLogError(_T("gkTexture[ %s ] LoadingFailed."), m_wstrFileName); return false; }

			if ( itusage->second == _T("RENDERTARGET") )
			{
				bool forceorigin = false;

				gkNameValuePairList::iterator itoverride = loadingParams.find(_T("override"));
				if (itoverride != loadingParams.end() && itoverride->second == _T("false"))
				{
					forceorigin = true;
				}




				gkNameValuePairList::iterator itsize = loadingParams.find(_T("size"));
				if (itsize == loadingParams.end())
				{ gkLogError(_T("gkTexture[ %s ] LoadingFailed."), m_wstrFileName); return false; }
				

				double rtResize = 1.0;
				rtResize = g_pRendererCVars->r_pixelscale;
				 
				if(forceorigin)
				{
					rtResize = 1.0f;
				}
				
				if ( itsize->second == _T("realfull") )
				{
					m_uWidth = gEnv->pRenderer->GetScreenWidth(forceorigin); 
					m_uHeight = gEnv->pRenderer->GetScreenHeight(forceorigin); 
					m_bSizeable = true;
				}
				else if ( itsize->second == _T("full") )
				{
					m_uWidth = gEnv->pRenderer->GetScreenWidth(forceorigin) * rtResize; 
					m_uHeight = gEnv->pRenderer->GetScreenHeight(forceorigin) * rtResize; 
					m_bSizeable = true;
				}
				else if ( itsize->second == _T("half") )
				{
					m_uWidth = gEnv->pRenderer->GetScreenWidth(forceorigin) / 2 * rtResize; 
					m_uHeight = gEnv->pRenderer->GetScreenHeight(forceorigin) / 2 * rtResize; 
					m_bSizeable = true;
				}
				else if ( itsize->second == _T("quad") )
				{
					m_uWidth = gEnv->pRenderer->GetScreenWidth(forceorigin) / 4 * rtResize; 
					m_uHeight = gEnv->pRenderer->GetScreenHeight(forceorigin) / 4 * rtResize; 
					m_bSizeable = true;
				}
				else if ( itsize->second == _T("quadquad") )
				{
					m_uWidth = gEnv->pRenderer->GetScreenWidth(forceorigin) / 8 * rtResize; 
					m_uHeight = gEnv->pRenderer->GetScreenHeight(forceorigin) / 8 * rtResize; 
					m_bSizeable = true;
				}
				else if ( itsize->second == _T("quadquadquad") )
				{
					m_uWidth = gEnv->pRenderer->GetScreenWidth(forceorigin) / 16 * rtResize; 
					m_uHeight = gEnv->pRenderer->GetScreenHeight(forceorigin) / 16 * rtResize; 
					m_bSizeable = true;
				}
				else
				{
					gkStdStringstream ss(itsize->second);
					ss >> m_uWidth;
					m_uHeight = m_uWidth;

					// get texture params
					gkNameValuePairList::iterator itsizey = loadingParams.find(_T("sizey"));
					if (itsizey != loadingParams.end())
					{
						gkStdStringstream ss(itsizey->second);
						ss >> m_uHeight;
					}
				}

				gkNameValuePairList::iterator itformat = loadingParams.find(_T("format"));
				if (itformat == loadingParams.end())
				{ gkLogError(_T("gkTexture[ %s ] LoadingFailed."), m_wstrFileName); return false; }

				m_wstrFormat = itformat->second;

				uint32 usage = D3DUSAGE_RENDERTARGET;

				m_format = getTextureFormat( m_wstrFormat );
				if (m_format == eTF_UnKnown )
				{
					{ gkLogError(_T("gkTexture[ %s ] Format %s Unsupport."), m_wstrFileName, m_wstrFormat); return false; }
				}
				D3DFORMAT format = getD3DFormatFromETF( m_format );
				if (format == D3DFMT_UNKNOWN)
				{
					{ gkLogError(_T("gkTexture[ %s ] Format %s Unsupport."), m_wstrFileName, m_wstrFormat); return false; }
				}


				if (isDepthTexture(m_format))
				{
					usage = D3DUSAGE_DEPTHSTENCIL;
				}

				gkNameValuePairList::iterator itmipmap = loadingParams.find(_T("mipmaprt"));
				if (itmipmap != loadingParams.end() && itmipmap->second == _T("true"))
				{
					usage |= D3DUSAGE_AUTOGENMIPMAP;
				}

				// finally, created it
				if( !SUCCEEDED( d3d9Device->CreateTexture( m_uWidth,
					m_uHeight,
					1,
					usage,
					format,
					D3DPOOL_DEFAULT,
					&m_p2DTexture,
					NULL ) ) )
				{
					gkLogError( _T("Dynamic Tex Created Failed!!!") );
				}

				gkNameValuePairList::iterator itInitColor = loadingParams.find(_T("initcolor"));
				if (itInitColor != loadingParams.end())
				{
					if( itInitColor->second == _T("WHITE") )
					{
						IDirect3DSurface9* surf;
						m_p2DTexture->GetSurfaceLevel(0, &surf);
						d3d9Device->ColorFill( surf, NULL, 0x7f7f7f7f );
						surf->Release();
					}
					else if( itInitColor->second == _T("BLACK") )
					{
						IDirect3DSurface9* surf;
						m_p2DTexture->GetSurfaceLevel(0, &surf);
						d3d9Device->ColorFill( surf, NULL, 0x00000000 );
						surf->Release();
					}
				}
				
			}
			else if ( itusage->second == _T("RAW") )
			{
				m_bIsDefaultPool = false;
				gkNameValuePairList::iterator itsize = loadingParams.find(_T("size"));
				if (itsize == loadingParams.end())
				{ gkLogError(_T("gkTexture[ %s ] LoadingFailed."), m_wstrFileName); return false; }

				{
					gkStdStringstream ss(itsize->second);
					ss >> m_uWidth;
					m_uHeight = m_uWidth;

					// get texture params
					gkNameValuePairList::iterator itsizey = loadingParams.find(_T("sizey"));
					if (itsizey != loadingParams.end())
					{
						gkStdStringstream ss(itsizey->second);
						ss >> m_uHeight;
					}
				}

				m_rawData = new uint8[m_uHeight * m_uWidth * sizeof(uint8)];
				memset(m_rawData, 0, m_uHeight * m_uWidth * sizeof(uint8));

				// finally, created it
				d3d9Device->CreateTexture( m_uWidth,
					m_uHeight,
					1,
					NULL,
					D3DFMT_A8,
					gkRendererD3D9::GetManagedTexturePoolType(),
					&m_p2DTexture,
					NULL );

				m_format = eTF_A8;
			}
			else if ( itusage->second == _T("DYNAMIC") )
			{
				m_bIsDefaultPool = false;
				gkNameValuePairList::iterator itsize = loadingParams.find(_T("size"));
				if (itsize == loadingParams.end())
				{ gkLogError(_T("gkTexture[ %s ] LoadingFailed."), m_wstrFileName); return false; }

				{
					gkStdStringstream ss(itsize->second);
					ss >> m_uWidth;
					m_uHeight = m_uWidth;

					// get texture params
					gkNameValuePairList::iterator itsizey = loadingParams.find(_T("sizey"));
					if (itsizey != loadingParams.end())
					{
						gkStdStringstream ss(itsizey->second);
						ss >> m_uHeight;
					}
				}

				m_rawData = new uint8[m_uHeight * m_uWidth * sizeof(uint8)];
				memset(m_rawData, 0, m_uHeight * m_uWidth * sizeof(uint8));

				// finally, created it
				d3d9Device->CreateTexture( m_uWidth,
					m_uHeight,
					1,
					NULL,
					D3DFMT_A8,
					gkRendererD3D9::GetManagedTexturePoolType(),
					&m_p2DTexture,
					NULL );

				m_format = eTF_A8;
			}


			// 计算内存占用
			calcMemUsage();
			return true;
		}

	}


	bool bCubemap = false;
	// load a managed pool texture

	// 先在目录内查找名字 [9/18/2010 Kaiming-Desktop]
	TCHAR wszPath[MAX_PATH] = _T("");
	
	_tcscpy(wszPath, m_wstrFileName.c_str());
	TCHAR* strLastSlash = NULL;

	// 先检查是不是raw
	// raw先不走多线程加载 [3/24/2013 Kaiming]
	strLastSlash = _tcsrchr( wszPath, _T( '.' ) );
	if( strLastSlash )
	{
		if ( !_tcsicmp( strLastSlash, _T(".raw") ) )
		{
			// 是，读取raw
			IResFile* pFile = gEnv->pFileSystem->loadResFile( wszPath, true );

			if (!pFile)
			{
				gkLogError(_T("gkTexture::Loading Texture [%s] Failed."), wszPath);
				return false;
			}

			m_uWidth = 0;
			m_uHeight = 0;

			uint32 filesize = pFile->Size();
			for (uint32 i=0; i < 2048; ++i)
			{
				if (i * i == filesize)
				{
					m_uWidth = i;
					m_uHeight = i;
					break;
				}
			}

			if (m_uHeight != 0)
			{
				// read data to raw
				m_rawData = new uint8[filesize];
				memcpy(m_rawData, pFile->DataPtr(), filesize);

				// final
			}

			gEnv->pFileSystem->closeResFile(pFile);

			m_bIsDefaultPool = false;
			return true;
		}
	}



	// 替换dds来读取
	TCHAR wszExtendedName[MAX_PATH] = _T(".dds");

	strLastSlash = _tcsrchr( wszPath, _T( '.' ) );
	if( strLastSlash )
	{
		_tcscpy( strLastSlash, wszExtendedName );
	}
	else
	{
		// not a file
		gkLogError(_T("gkTexture::Loading Texture [%s] Failed."), wszPath);
		return false;
	}
	TCHAR sourceName[MAX_PATH];
	_tcscpy( sourceName, wszPath );
	strLastSlash = _tcsrchr( sourceName, _T( '.' ) );
	if( strLastSlash )
	{
		*strLastSlash = 0;
	}

	strLastSlash = _tcsrchr( wszPath, _T( '.' ) );

	if ( _tcslen(wszPath) > 7 )
	{
		strLastSlash -= 3;
		if ( strLastSlash[0] == '_' && strLastSlash[1] == 'c' && strLastSlash[2] == 'm' )
		{
			// this is cubemap
			bCubemap = true;
		}
	}




	if (bCubemap)
	{
		// CUBE MAP先不走多线程加载 [3/24/2013 Kaiming]

		// 建立多线程加载任务
		gkTextureCUBEStreamingTask* task = new gkTextureCUBEStreamingTask;
		task->m_targetTexture = &m_pCubeTexture;
		_tcscpy( task->m_filepath, wszPath );
		task->m_device = d3d9Device;
		task->m_texturePtr = this;

		m_loadingTask = task;

		gEnv->pCommonTaskDispatcher->PushTask( task, m_syncLoad ? -1 : 0 );
	}
	else
	{
		//////////////////////////////////////////////////////////////////////////
		// 验证下文件，并读取DDS的头
		IResFile* pFile = gEnv->pFileSystem->loadResFileHeader( wszPath, true );

		if (!pFile)
		{
			if (1)
			{

				gkStdString rel_path = gkGetPurePath(sourceName);

// 				gkStdString org_path = gkGetGameRootDir() + sourceName + _T(".tga");
// 
// 				if( !IsFileExist( org_path.c_str() ) )
// 				{
// 					org_path = gkGetExecRootDir() + sourceName + _T(".tga");
// 				}

				// compile resource and try again
				TCHAR buffer[1024];

				// 判断是engine目录还是media目录
				if( gkIsEnginePath( sourceName ) )
				{
					_stprintf(buffer, _T("%stools\\texconv -nologo -f DXT5 -ft dds -o %s%s %s%s.tga"), gkGetExecRootDir().c_str(), gkGetExecRootDir().c_str(), rel_path.c_str(), gkGetExecRootDir().c_str(), sourceName);
				}
				else
				{
					_stprintf(buffer, _T("%stools\\texconv -nologo -f DXT5 -ft dds -o %s%s %s%s.tga"), gkGetExecRootDir().c_str(), gkGetGameRootDir().c_str(), rel_path.c_str(), gkGetGameRootDir().c_str(), sourceName);
				}

				

				gkStdString cmdbuffer(buffer);
				gkNormalizePath_WinCMD(cmdbuffer);

				gkLogWarning( _T("Texture force convert to platform format: \n%s"), cmdbuffer.c_str() );

				char bufferA[1024];

#ifdef UNICODE
				WideCharToMultiByte(CP_ACP, NULL, cmdbuffer.c_str(), -1, bufferA, 1024, NULL, NULL );
#else
				_tcscpy( bufferA, cmdbuffer.c_str() );
#endif
				system(bufferA);

				pFile  = gEnv->pFileSystem->loadResFileHeader( wszPath, true );
			}

			if (!pFile)
			{
				gkLogError(_T("gkTexture::Loading Texture [%s] Failed."), wszPath);
				return false;
			}
		}

		// 利用DDS LOADER来创建
		const uint8* start = (const uint8*)(pFile->DataPtr());

		// DDS files always start with the same magic number ("DDS ")
		DWORD dwMagicNumber = *( DWORD* )( start );
		if( dwMagicNumber != DDS_MAGIC )
		{
			return false;
		}

		const DDS_HEADER* pHeader = reinterpret_cast<const DDS_HEADER*>( start + sizeof( DWORD ) );

		// Verify header to validate DDS file
		if( pHeader->dwSize != sizeof(DDS_HEADER)
			|| pHeader->ddspf.dwSize != sizeof(DDS_PIXELFORMAT) )
		{
			return false;
		}

		// Check for DX10 extension
		bool bDXT10Header = false;
		if ( (pHeader->ddspf.dwFlags & DDS_FOURCC)
			&& (MAKEFOURCC( 'D', 'X', '1', '0' ) == pHeader->ddspf.dwFourCC) )
		{
			return false;
		}

		// setup the pointers in the process request
		INT offset = sizeof( DWORD ) + sizeof( DDS_HEADER );

		const uint8* pBitData = start + offset;
		uint32 BitSize = pFile->Size() - offset;

		UINT iWidth = pHeader->dwWidth;
		UINT iHeight = pHeader->dwHeight;
		UINT iMipCount = pHeader->dwMipMapCount;
		if( 0 == iMipCount )
			iMipCount = 1;

		if (pHeader->dwCubemapFlags != 0
			|| (pHeader->dwHeaderFlags & DDS_HEADER_FLAGS_VOLUME) )
		{
			// For now only support 2D textures, not cubemaps or volumes
			return false;
		}

		D3DFORMAT fmt = GetD3D9Format( pHeader->ddspf );

		if (fmt >= D3DFMT_DXT1 && fmt <= D3DFMT_DXT5 )
		{
			RESIZE_POWER = 0;
		}

		// Create Surf Now
		d3d9Device->CreateTexture( (iWidth >> RESIZE_POWER), (iHeight >> RESIZE_POWER), (iMipCount - RESIZE_POWER),
			0,
			fmt,
			gkRendererD3D9::GetManagedTexturePoolType(),
			&m_p2DTexture,
			NULL  );

		m_p2DTexture->SetLOD( iMipCount - RESIZE_POWER - 1 );

		D3DSURFACE_DESC d3dsurf;
		m_p2DTexture->GetLevelDesc(0, &d3dsurf);
		m_uWidth = d3dsurf.Width;
		m_uHeight = d3dsurf.Height;
		switch (d3dsurf.Format)
		{
		case D3DFMT_A16B16G16R16F:
			m_wstrFormat = _T("A16B16G16R16F");
			break;
		case D3DFMT_A8L8:
			m_wstrFormat = _T("A8L8");
			break;
		case D3DFMT_A8R8G8B8:
			m_wstrFormat = _T("A8R8G8B8");
			m_format = eTF_RGBA8;
			break;
		case D3DFMT_DXT1:
			m_wstrFormat = _T("DXT1");
			m_format = eTF_DXT1;
			break;
		case D3DFMT_DXT2:
			m_wstrFormat = _T("DXT2");
			m_format = eTF_DXT3;
			break;
		case D3DFMT_DXT3:
			m_wstrFormat = _T("DXT3");
			m_format = eTF_DXT3;
			break;
		case D3DFMT_DXT4:
			m_wstrFormat = _T("DXT4");
			m_format = eTF_DXT5;
			break;
		case D3DFMT_DXT5:
			m_wstrFormat = _T("DXT5");
			m_format = eTF_DXT5;
			break;
		}
		gEnv->pFileSystem->closeResFile(pFile);

		// 建立多线程加载任务
		gkTexture2DStreamingTask* task = new gkTexture2DStreamingTask;
		task->m_targetTexture = m_p2DTexture;
		_tcscpy( task->m_filepath, wszPath );
		task->m_device = d3d9Device;
		task->m_texturePtr = this;
		task->m_shrinkLevel = m_shrinkLevel;

		m_loadingTask = task;
		//m_resLock->lock();

		gEnv->pCommonTaskDispatcher->PushTask( task, m_syncLoad ? -1 : 0 );

		//gEnv->pCommonTaskDispatcher->Wait();

	}

	loadingParams[_T("format")] = m_wstrFormat;

// 	if (FAILED(hr))
// 	{		
// 		OutputDebugString(_T("++++gkTexture.cpp: Tex: ["));
// 		OutputDebugString(m_wstrFileName.c_str());
// 		OutputDebugString(_T("] loaded failed.\n"));
// 		return false;
// 	}


	// if go here, we are a managed resource [10/20/2011 Kaiming]
	m_bIsDefaultPool = false;

	return true;
}

bool gkTexture::unloadImpl(void)
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

	if ( m_p2DTexture )
	{
		uint32 refcount = m_p2DTexture->Release();
		if (refcount > 0)
		{
			gkLogError( _T("Texture [%s] REFCOUNT not ZERO!"), m_wstrFileName.c_str() );
		}
		m_p2DTexture = 0;
	}

	if ( m_pCubeTexture )
	{
		uint32 refcount = m_pCubeTexture->Release();
		if (refcount > 0)
		{
			gkLogError( _T("Texture [%s] REFCOUNT not ZERO!"), m_wstrFileName.c_str() );
		}
		m_pCubeTexture = 0;
	}

	if (m_rawData)
	{
		delete[] m_rawData;
	}

	m_resLock->UnLock();

	return true;
}

IDirect3DBaseTexture9* gkTexture::getTexture()
{
	touch();

	//assert(m_p2DTexture);
	if (m_p2DTexture)
	{
		return m_p2DTexture;
	}
	else if (m_pCubeTexture)
	{
		return m_pCubeTexture;
	}
	else
	{
		return NULL;
	}
}

IDirect3DTexture9* gkTexture::get2DTexture()
{
	touch();

	assert(m_p2DTexture);
	if (m_p2DTexture)
	{
		return m_p2DTexture;
	}
	else
	{
		return NULL;
	}
}

void gkTexture::onReset()
{
	if( m_bIsDefaultPool )
	{
		load();
	}
}

void gkTexture::onLost()
{
	if( m_bIsDefaultPool )
	{
		unload();
	}
}

void gkTexture::Apply( uint32 channel, uint8 filter )
{
	touch();

	RawDataFlush();

	if (m_p2DTexture)
	{
		getRenderer()->GetStateManager()->SetTexture(channel, m_p2DTexture);
	}
	else if (m_pCubeTexture)
	{
		getRenderer()->GetStateManager()->SetTexture(channel, m_pCubeTexture);
	}
	
}

void gkTexture::changeAttr( const gkStdString& key, const gkStdString& value )
{
	// find in loadparam
	gkNameValuePairList::iterator it = loadingParams.find( key );
	if (it != loadingParams.end())
	{
		if ( it->second != value)
		{
			it->second = value;
			reload();
		}
		
	}
}

const gkStdString& gkTexture::getAttr( const gkStdString& key ) const
{
	// find in loadparam
	gkNameValuePairList::const_iterator it = loadingParams.find( key );
	if (it != loadingParams.end())
	{
		return it->second;
	}
	else
	{
		static gkStdString notfind(_T("notfind"));
		return notfind;
	}
}

void gkTexture::OnFileChange( const TCHAR* filename )
{
	TCHAR rootpathName[MAX_PATH] = _T("media");
	_tcscat( rootpathName, m_wstrFileName.c_str() );

	TCHAR filenameReal[MAX_PATH];
	_tcscpy(filenameReal, filename);
	// 文件更改，检查是否需要重新读取
	if ( !_tcsicmp( rootpathName, filenameReal ) )
	{
		gkLogMessage(_T("Texture Change, Recompile."));
		reload();
		return;
	}

	// 没找到，去掉扩展名再来一次
	TCHAR* ext = _tcsrchr(rootpathName, _T('.'));
	if (ext)
	{
		*ext = 0;
	}

	ext = _tcsrchr(filenameReal, _T('.'));
	if (ext)
	{
		*ext = 0;
	}

	if ( !_tcsicmp( rootpathName, filenameReal ) )
	{
		gkLogMessage(_T("Texture Change, Recompile."));
		reload();

		TCHAR buffer[MAX_PATH];
		_stprintf(buffer, _T("Texture[%s] Updated."), m_wstrShortName.c_str());
		gEnv->pInGUI->gkPopupMsg( buffer );
		return;
	}

}

float gkTexture::Tex2DRAW( const Vec2& texcoord, int filter /*= 1*/ )
{


	if (filter)
	{
		float u = texcoord.x - floor(texcoord.x);
		float v = texcoord.y - floor(texcoord.y);

		// 取得纹理空间的地址
		u *= (m_uWidth - 1);
		v *= (m_uHeight - 1);

		// 取整
		int x = (int)( u );
		int y = (int)( v );

		// 计算子像素偏移
		float du = u - x;
		float dv = v - y;

		// 取得地址位置，确保在纹理采样范围内
		int left = x % m_uWidth;
		int right = (x + 1) % m_uWidth;
		int up = y % m_uHeight;
		int down = (y + 1) % m_uHeight;

// 		assert( left >= 0 && left < m_width );
// 		assert( right >= 0 && right < m_width );
// 		assert( up >= 0 && up < m_height );
// 		assert( down >= 0 && down < m_height );

		// 采样4个颜色
		float lt = Tex2DRAW( Vec2i(left,up) );
		float t = Tex2DRAW( Vec2i(right,up) );
		float l = Tex2DRAW( Vec2i(left,down) );
		float rb =  Tex2DRAW( Vec2i(right,down) );

		float final = ((1.f-du)*(1.f-dv)) * lt + t * (du*(1.f - dv)) + l * ((1.f-du)*dv) + rb * (du*dv);

		return final;
	}
	else
	{
		return 0;
	}
}

float gkTexture::Tex2DRAW( const Vec2i& texcoord )
{
	return m_rawData[ texcoord.y * m_uWidth + texcoord.x ] / 255.f;
}

void gkTexture::AutoGenMipmap()
{
	if (m_p2DTexture)
	{
		m_p2DTexture->GenerateMipSubLevels();
	}
}

bool gkTexture::lock( gkLockOperation& lockopt )
{
	if (!m_rawData && m_p2DTexture)
	{
		RECT rc;
		rc.left = lockopt.m_pos.x;
		rc.top = lockopt.m_pos.y;
		rc.right = rc.left + lockopt.m_size.x;
		rc.bottom = rc.top + lockopt.m_size.y;

		D3DLOCKED_RECT rect;

		DWORD flag = 0;
		switch( lockopt.m_flag )
		{
		case gkLockOperation::eLF_Discard:
			flag = D3DLOCK_DISCARD;
			break;
		case gkLockOperation::eLF_Keep:
			flag = 0;
			break;
		case gkLockOperation::eLF_Read:
			flag = D3DLOCK_READONLY;
			break;
		}

		m_lastLockLevel = lockopt.m_level;

		if( m_p2DTexture->LockRect( lockopt.m_level, &rect, (lockopt.m_size.x <= 0 || lockopt.m_size.y <= 0 ) ? NULL : &rc, flag) >= 0 )
		{
			lockopt.m_outData = rect.pBits;
			lockopt.m_outPitch = rect.Pitch;

			return true;
		}
	}
	else if (m_rawData)
	{
		// may raw data
		lockopt.m_outData = m_rawData + lockopt.m_pos.y * m_uWidth + lockopt.m_pos.x;
		lockopt.m_outPitch = m_uWidth;

		m_rawDirty = true;

		return true;
	}


	return false;
}

void gkTexture::unlock()
{
	if (!m_rawData && m_p2DTexture)
	{
		m_p2DTexture->UnlockRect( m_lastLockLevel );
	}
}

void gkTexture::RawDataFlush()
{
	if( m_p2DTexture && m_rawData && m_rawDirty )
	{
		// MANAGED UPLOAD
		IDirect3DTexture9* locking_texture = NULL;

		if ( gkRendererD3D9::GetManagedTexturePoolType() == D3DPOOL_DEFAULT )
		{
			getRenderer()->getDevice()->CreateTexture( m_uWidth, m_uHeight, 0, 0, getD3DFormatFromETF(m_format), D3DPOOL_SYSTEMMEM, &locking_texture, NULL );

		}
		else
		{
			locking_texture = m_p2DTexture;
		}
		
 		D3DLOCKED_RECT rect;
 		if( locking_texture->LockRect( 0, &rect, NULL, NULL) >= 0 )
 		{
 		 	for (uint32 i=0; i < m_uHeight; ++i)
 		 	{
 		 		// if aligned
 		 		memcpy( rect.pBits, m_rawData, m_uWidth * m_uHeight);
 		 
 		 	}
 		 	locking_texture->UnlockRect(0);
 		}

		if ( gkRendererD3D9::GetManagedTexturePoolType() == D3DPOOL_DEFAULT )
		{
			getRenderer()->getDevice()->UpdateTexture(locking_texture, m_p2DTexture);
			SAFE_RELEASE( locking_texture );
		}

		m_rawDirty = false;
	}
}

void gkTexture::calcMemUsage()
{
	m_nSize = m_uWidth * m_uHeight * getFormatBPP(m_format) / 4;
}
