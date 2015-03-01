#include "gkSystemStableHeader.h"
#include "gkResFileManager.h"
#include "gkResFile.h"
#include "lzma/LzmaLib.h"
#include "IThread.h"

Crc32Gen gkPak::s_crcGenerator;

IResFile* gkResFileManager::loadResFileHeader( const TCHAR* pszFilename, bool searchDir /*= false*/ )
{
	IResFile* ret = NULL;

	// 先观察是否为engine目录
	if ( gkIsEnginePath( pszFilename ) )
	{
		ret = loadResFileExec(pszFilename, searchDir, true);
		if (ret)
		{
			return ret;
		}
	}


	TCHAR szFullPath[MAX_PATH] = _T("/media/");
	_tcscat(szFullPath, pszFilename);
	ret = loadResFileExec(szFullPath, searchDir, true);

	return ret;
}


EFileStatus gkResFileManager::checkFileExist(const TCHAR* pszFilename, bool searchDir /*= false*/)
{
	IResFile* ret = NULL;

	gkStdString normalized_path;

	// 先观察是否为engine目录
	if ( gkIsEnginePath( pszFilename ) )
	{
		normalized_path = pszFilename;
	}
	else
	{
		normalized_path = _T("/media/") + gkStdString(pszFilename);
	}

	if (searchDir && gEnv->pSystem->IsEditor())
	{
		TCHAR szFullPath[MAX_PATH];

		HRESULT hr = gkFindFileRelativeExec(szFullPath, MAX_PATH, normalized_path.c_str());
		if ( hr == S_OK)
		{
			_tcscpy( szFullPath, gkGetExecRelativePath( szFullPath ).c_str() );
		}
		else
		{
			_tcscpy( szFullPath, normalized_path.c_str() );
		}		

		normalized_path = szFullPath;
	}

	gkNormalizeResName(normalized_path);

	// check pak
	if( checkPakFile( normalized_path.c_str() ))
	{
		return eFS_inPak;
	}

	// check disk
	TCHAR szFullPath[MAX_PATH];
	_tcscpy( szFullPath, gkGetExecRootDir().c_str() );
	_tcscat( szFullPath, normalized_path.c_str() );

	FILE* fp = _tfopen( szFullPath, _T("r") );
	if( fp )
	{
		return eFS_inDisk;
		fclose( fp );
	}

	// not found
	return eFS_notExsit;
}

IResFile* gkResFileManager::loadResFile( const TCHAR* pszFilename, bool searchDir )
{
	IResFile* ret = NULL;

	// 先观察是否为engine目录
	if ( gkIsEnginePath( pszFilename ) )
	{
		ret = loadResFileExec(pszFilename, searchDir);
		if (ret)
		{
			return ret;
		}
	}

	TCHAR szFullPath[MAX_PATH] = _T("/media/");
	_tcscat(szFullPath, pszFilename);
	ret = loadResFileExec(szFullPath, searchDir);

	return ret;
}

void gkResFileManager::closeResFile( IResFile* file )
{
	gkAutoLock<gkCritcalSectionLock> lock(eLGID_global, eLID_resfile_close);

	ResFileMap::iterator it = m_mapFiles.begin();

	for (; it != m_mapFiles.end(); ++it)
	{
		if (it->second == file)
		{
			file->RemoveRef();
			if (file->RefCount() <= 0)
			{
				SAFE_DELETE( it->second );
				m_mapFiles.erase(it);
			}
			return;
		}
	}
}

void gkResFileManager::closeResFile( const TCHAR* pszFilename )
{
	gkAutoLock<gkCritcalSectionLock> lock(eLGID_global, eLID_resfile_close);

	ResFileMap::iterator it = m_mapFiles.find(pszFilename);
	if (it != m_mapFiles.end())
	{
		SAFE_DELETE( it->second );
		m_mapFiles.erase(it);
		return;
	}

	TCHAR szFullPath[MAX_PATH] = _T("media/");
	_tcscat( szFullPath, pszFilename );
	it = m_mapFiles.find(szFullPath);
	if (it != m_mapFiles.end())
	{
		SAFE_DELETE( it->second );
		m_mapFiles.erase(it);
		return;
	}

}

IResFile* gkResFileManager::loadResFileExec( const TCHAR* pszFilename, bool searchDir /*= false*/, bool loadheader )
{
	TCHAR szFullPath[MAX_PATH] = _T("");
	if (searchDir && gEnv->pSystem->IsEditor())
	{

		HRESULT hr = gkFindFileRelativeExec(szFullPath, MAX_PATH, pszFilename);
		if ( hr == S_OK)
		{
			_tcscpy( szFullPath, gkGetExecRelativePath( szFullPath ).c_str() );
		}
		else
		{
			_tcscpy( szFullPath, pszFilename );
		}		
	}
	else
	{
		_tcscat( szFullPath, pszFilename );
	}

	return CreateFileInternal(szFullPath, loadheader);

}

IResFile* gkResFileManager::CreateFileInternal( TCHAR * szExecRelativePath, bool loadheader )
{
	gkStdString normalized_path(szExecRelativePath);
	gkNormalizeResName( normalized_path );

	{
		gkAutoLock<gkCritcalSectionLock> lock(eLGID_global, eLID_resfile_close);
		ResFileMap::iterator it = m_mapFiles.find( normalized_path.c_str() );
		if (it != m_mapFiles.end())
		{
			it->second->AddRef();
			return it->second;
		}
	}


	TCHAR szFullPath[MAX_PATH];
	_tcscpy( szFullPath, gkGetExecRootDir().c_str() );
	_tcscat( szFullPath, normalized_path.c_str() );

    gkResFile* pNewfile = NULL;
	bool pakPriority = false;
	
	if ( g_pSystemCVars->sys_loadPakFirst && !gEnv->pSystem->IsEditor() )
	{
		// 如果打开优先从PAK读取，且不是编辑器模式 [3/7/2013 Kaiming]
		pakPriority = true;
	}
	
	if (pakPriority)
	{
		pNewfile = new gkResFile( normalized_path.c_str(), loadheader );
	}
	else
	{
		pNewfile = new gkResFile( szFullPath, loadheader );
	}
	
	if (!pNewfile->IsOpen())
	{
		SAFE_DELETE( pNewfile );

		// 如果pak / file 不存在，反向读取
		if (pakPriority)
		{
			pNewfile = new gkResFile( szFullPath, loadheader );
		}
		else
		{
			pNewfile = new gkResFile( normalized_path.c_str(), loadheader );
		}

		// 还是失败，打开错误
		if (!pNewfile->IsOpen())
		{
			gkLogError( _T("Loading File %s Failed."), normalized_path.c_str() );
			SAFE_DELETE( pNewfile );
			return NULL;
		}
	}

	// file opened
	{
		gkAutoLock<gkCritcalSectionLock> lock(eLGID_global, eLID_resfile_close);
		pNewfile->AddRef();
		m_mapFiles.insert( ResFileMap::value_type( normalized_path.c_str(), pNewfile ) );
	}
	

	return pNewfile;
}

gkResFileManager::gkResFileManager()
{
	gkPak::s_crcGenerator.Init();
}


gkResFileManager::~gkResFileManager()
{
	{
		// close pak
		gkPakMap::iterator it = m_paks.begin();
		for ( ; it != m_paks.end(); ++it)
		{
			delete it->second;
		}
		m_paks.clear();
	}

	{
		// close file
		ResFileMap::iterator it = m_mapFiles.begin();
		for ( ; it != m_mapFiles.end(); ++it)
		{
			delete it->second;
		}
		m_mapFiles.clear();
	}

}


void gkResFileManager::openPak( const TCHAR* pszFilename )
{
	gkPak* pak = new gkPak(pszFilename);
	if (pak->isOpen())
	{
		m_paks.insert( gkPakMap::value_type( pszFilename, pak) );
	}
	else
	{
        gkLogMessage(_T("Open pak [%s] failed."), pszFilename);
		delete pak;
	}	
}

void gkResFileManager::closePak( const TCHAR* pszFilename )
{
	gkPakMap::iterator it = m_paks.find( pszFilename );
	if ( it != m_paks.end() )
	{
		delete it->second;
		m_paks.erase(it);
	}
}

bool gkResFileManager::openPakFile( const TCHAR* pszRelFilename, char** pData, size_t &size )
{
	gkPakMap::iterator it = m_paks.begin();
	for ( ; it != m_paks.end(); ++it)
	{
		if ( it->second->openPakFile( pszRelFilename, pData, size) )
		{
			return true;
		}
	}
	return false;
}

bool gkResFileManager::checkPakFile(const TCHAR* pszRelFilename)
{
	gkPakMap::iterator it = m_paks.begin();
	for ( ; it != m_paks.end(); ++it)
	{
		if ( it->second->checkPakFile( pszRelFilename) )
		{
			return true;
		}
	}
	return false;
	
}

gkPak::gkPak( const TCHAR* pszPakFile )
{
#ifdef UNICODE
	m_handle = _wfopen( gkGetAbsExecPath( pszPakFile ).c_str(), _T("rb") );
#else
	m_handle = fopen( gkGetAbsExecPath( pszPakFile ).c_str(), "rb" );
#endif

	if (m_handle)
	{
		// 读取文件头和CRC RECORD
		
		fread( &m_pakHeader, 1, sizeof(gkPak_Header), m_handle );

		gkPak_FileRecord* m_pakFileRecords = new gkPak_FileRecord[m_pakHeader.m_count];
		fread( m_pakFileRecords, 1, m_pakHeader.m_count * sizeof(gkPak_FileRecord), m_handle );

		for (uint32 i=0; i < m_pakHeader.m_count; ++i)
		{
			m_pakMap.insert( gkPakRecordMap::value_type( m_pakFileRecords[i].crc32, m_pakFileRecords[i] ) );
		}

		fseek(m_handle, 0, SEEK_SET);

		delete[] m_pakFileRecords;
	}
	else
	{
		gkLogError( _T("fopen [%s] failed."), pszPakFile );
	}
	
}

gkPak::~gkPak()
{
	if (m_handle)
	{
		fclose( m_handle );
	}
}


bool gkPak::checkPakFile(const TCHAR* pszRelFilename)
{
	// NORMALIZE THE FILENAME
	gkStdString finalpath(pszRelFilename);
	gkNormalizePath( finalpath );

	// UNICODE SHOULD TRANSLATE
#ifdef UNICODE
	char buffer[MAX_PATH];
	WideCharToMultiByte(CP_ACP, 0,  finalpath.c_str(), -1, buffer, MAX_PATH, NULL, NULL);
	uint32 crc32 = gkPak::s_crcGenerator.GetCRC32Lowercase( buffer );
#else
	uint32 crc32 = gkPak::s_crcGenerator.GetCRC32Lowercase( finalpath.c_str() );
#endif

	gkPakRecordMap::iterator it =  m_pakMap.find( crc32 );

	if ( it != m_pakMap.end() )
	{
		return true;
	}

	return false;
}

bool gkPak::openPakFile( const TCHAR* pszRelFilename, char** pData, size_t &size )
{
	// NORMALIZE THE FILENAME
	gkStdString finalpath(pszRelFilename);
	gkNormalizePath( finalpath );

	// UNICODE SHOULD TRANSLATE
#ifdef UNICODE
	char buffer[MAX_PATH];
	WideCharToMultiByte(CP_ACP, 0,  finalpath.c_str(), -1, buffer, MAX_PATH, NULL, NULL);
	uint32 crc32 = gkPak::s_crcGenerator.GetCRC32Lowercase( buffer );
#else
	uint32 crc32 = gkPak::s_crcGenerator.GetCRC32Lowercase( finalpath.c_str() );
#endif

	gkPakRecordMap::iterator it =  m_pakMap.find( crc32 );
	
	if ( it != m_pakMap.end() )
	{
		// 如果size不为0, 这里就是读文件头 [3/24/2013 Kaiming]
		bool readHeader = true;
		if (size == 0)
		{
			readHeader = false;
		}

		if ( size == 0 || size > it->second.size)
		{
			size = it->second.size;
		}

		// read the data
		char* pTmp = new char[size];

		//////////////////////////////////////////////////////////////////////////
		// 文件操作，加锁
		{
			gkAutoLock<gkCritcalSectionLock> lock(eLGID_global, eLID_file_seek );
			// find it
			fseek(m_handle, m_pakHeader.m_data_offset + it->second.start, SEEK_SET);
		}


		size_t BytesRead = fread(pTmp, 1, size, m_handle);
		//fseek(m_handle, 0, SEEK_SET);
		
		//////////////////////////////////////////////////////////////////////////

		if (BytesRead != size)
		{
		 	delete [] pTmp;
		 	size = 0;
		}
		else if (readHeader)
		{
			// 读文件头，直接返回了
			*pData = pTmp;
			return true;
		}
		else
		{			// 读取到了一个文件的数据，判断是否经过压缩 [3/7/2013 Kaiming]
			if (it->second.compressHeaderSize != 0)
			{
				float decompress_start = gEnv->pTimer->GetAsyncCurTime();


				char* pTmpUnCompressed = new char[it->second.compressHeaderSize];
				size_t unCompressedSize = it->second.compressHeaderSize - GKPAK_UNCOMPRESS_HEADER_SIZE;
				size_t realSize = size - LZMA_PROPS_SIZE - GKPAK_UNCOMPRESS_HEADER_SIZE;

				// 前1KB是不压缩的
				// | UNCOMPRESS 1024  | HEADER | COMPRESSED

				memcpy( pTmpUnCompressed, pTmp, GKPAK_UNCOMPRESS_HEADER_SIZE);

				//ELzmaStatus status;
				//SRes res = LzmaDecode((unsigned char*)pTmpUnCompressed, &unCompressedSize, (unsigned char*)(pTmp + LZMA_PROPS_SIZE), &realSize, (unsigned char*)pTmp, LZMA_PROPS_SIZE, LZMA_FINISH_END, &status, &g_Alloc);
				SRes res = LzmaUncompress( (unsigned char*)pTmpUnCompressed + GKPAK_UNCOMPRESS_HEADER_SIZE, &unCompressedSize, (unsigned char*)(pTmp + LZMA_PROPS_SIZE + GKPAK_UNCOMPRESS_HEADER_SIZE), &realSize, (unsigned char*)pTmp + GKPAK_UNCOMPRESS_HEADER_SIZE, LZMA_PROPS_SIZE );
				if (res == SZ_OK && (unCompressedSize + GKPAK_UNCOMPRESS_HEADER_SIZE) == it->second.compressHeaderSize)
				{
					*pData = pTmpUnCompressed;
					size = unCompressedSize + GKPAK_UNCOMPRESS_HEADER_SIZE;

					delete[] pTmp;
				}
				else
				{
					*pData = pTmp;
					delete[] pTmpUnCompressed;
				}

				decompress_start = gEnv->pTimer->GetAsyncCurTime() - decompress_start;

#ifdef OS_WIN32
				uint32 threadid = ::GetCurrentThreadId();
				if ( threadid == gEnv->pSystemInfo->mainThreadId)
				{
					gkLogMessage( _T("thread[main] lzma-dec [%s] using %.2fms"), finalpath.c_str(), decompress_start * 1000);
				}
				else
				{
					gkLogMessage( _T("thread[%d] lzma-dec [%s] using %.2fms"), threadid, finalpath.c_str(), decompress_start * 1000);
				}
#endif				
								
			}
			else
			{
				*pData = pTmp;
			}		 	
			return true;
		}
	}
	else
	{
		return false;
	}




	return false;
}

bool gkPak::closePakFile( const TCHAR* pszRelFilename )
{

	return true;
}
