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
// Name:   	gkFilePath.h
// Desc:		
// 
// Author:  Kaiming-Desktop
// Date:	2011 /8/13
// Modify:	2011 /8/13
// 
//////////////////////////////////////////////////////////////////////////
#ifndef GKFILEPATH_H_
#define GKFILEPATH_H_

#pragma once

#include "gkPlatform.h"

#ifdef OS_IOS
std::string macCachePath();
std::string macBundlePath(void);
std::string iOSDocumentsDirectory(void);
#endif

inline bool IsFileExist( const TCHAR* filename )
{
	FILE* fp = NULL;
#ifdef UNICODE
	char path[MAX_PATH];
	WideCharToMultiByte( CP_ACP, 0, filename, -1, path, MAX_PATH, NULL, NULL );
	fp = fopen( path, "rb" );
#else
	fp = fopen( filename, "rb" );
#endif
	if (fp)
	{
		fclose(fp);
		return true;
	}

	return false;
}

// 不要带.
inline bool gkCheckFileExtension( const TCHAR* filename, const TCHAR* ext )
{
	// 读到名字后，分析一下文件类型
	// 弱分析，靠的是扩展名
	TCHAR wszExtendedName[MAX_PATH] = _T("");

	const TCHAR* strLastSlash = _tcsrchr( filename, _T( '.' ) );
	if( strLastSlash )
	{
		_tcscpy( wszExtendedName, &strLastSlash[1] );

		if (!_tcsicmp( wszExtendedName, ext ))
		{
			return true;
		}
	}
	return false;
}

inline bool is_end_with_slash( const TCHAR* filename )
{
	size_t len = _tcslen(filename);
	if (filename[len-1] == _T('\\') || filename[len-1] == _T('/'))
	{
		return true;
	}
	else
	{
		return false;
	}
}

#define _ascii_tolower(c)     ( (((c) >= L'A') && ((c) <= L'Z')) ? ((c) - L'A' + L'a') : (c) )


//! Removes the trailing slash or backslash from a given path.
inline gkStdString RemoveSlash( const gkStdString &path )
{
	if (path.empty() || (path[path.length()-1] != '/' && path[path.length()-1] != '\\'))
		return path;
	return path.substr(0,path.length()-1);
}

inline void gkNormalizeResName( gkStdString &path )
{
	gkStdString::iterator it = path.begin();
	for (; it != path.end(); ++it)
	{
		*it =  _ascii_tolower(*it);

		if ( *it == '\\')
		{
			*it = '/';
		}
	}

	int pos = path.find( _T("//"), 0 );
	while( gkStdString::npos != pos )
	{
		path.replace( pos, 2, _T("/") );
		pos = path.find( _T("//"), 0 );
	}

	if ( path.at(0) == '/' )
	{
		path.erase(0, 1);
	}
}

inline bool gkIsRelPath( const gkStdString& path )
{
	if ( _tcsstr( path.c_str(), _T(":/") ) )
	{
		return false;
	}

	if ( _tcsstr( path.c_str(), _T(":\\") ) )
	{
		return false;
	}

	return true;
}

inline void gkNormalizePath( gkStdString &path )
{
	gkStdString::iterator it = path.begin();
	for (; it != path.end(); ++it)
	{
		*it =  _ascii_tolower(*it);

		if ( *it == '\\')
		{
			*it = '/';
		}
	}

	int pos = path.find( _T("//"), 0 );
	while( gkStdString::npos != pos )
	{
		path.replace( pos, 2, _T("/") );
		pos = path.find( _T("//"), 0 );
	}


	if ( path.at(0) != '/' )
	{
		path.insert(0,1,'/');
	}
}

inline void gkNormalizePath_WinCMD( gkStdString &path )
{
	//gkNormalizePath(path);

	gkStdString::iterator it = path.begin();
	for (; it != path.end(); ++it)
	{
		*it =  _ascii_tolower(*it);

		if ( *it == '/')
		{
			*it = '\\';
		}
	}
}

inline bool gkIsEnginePath( const gkStdString &path )
{
	gkStdString tmpPath(path);
	gkNormalizePath( tmpPath );

	if ( _tcsstr( tmpPath.c_str(), _T("/engine/") ) )
	{
		return true;
	}
// 	if ( _tcsstr( tmpPath.c_str(), _T("/Engine/") ) )
// 	{
// 		return true;
// 	}

	return false;
}

inline void RemoveRelativeParts(gkStdString& strPath)
{
	// Remove all dir/..
	size_t nBack;
	while ((nBack = strPath.find(_T("\\.."))) != gkStdString::npos)
	{
		int nPrev;
		for (nPrev = nBack-1; nPrev > 0; nPrev--)
		{
			if (strPath[nPrev] == _T('\\'))
				break;
		}
		strPath.replace( nPrev, nBack + 3 -nPrev, _T(""), 0 );
		//strPath.Delete(nPrev, nBack+3 - nPrev);
	}

	// Remove all ./
	//strPath.Replace(".\\", "");
}

inline gkStdString gkGetExecRootDir()
{
#if defined ( OS_ANDROID )
	return gkStdString(ANDROID_APP_SDCARDROOT);
#elif defined ( OS_IOS ) 
    return iOSDocumentsDirectory() + "/";
#else
	// Get the exe name, and exe path
	TCHAR strExePath[MAX_PATH] = {0};
	TCHAR* strLastSlash = NULL;
	GetModuleFileName( NULL, strExePath, MAX_PATH );
	strExePath[MAX_PATH - 1] = 0;

	// 再砍一层目录
	strLastSlash = _tcsrchr( strExePath, L'\\' );
	if( strLastSlash )
		*strLastSlash = 0;

	strLastSlash = _tcsrchr( strExePath, L'\\' );
	if( strLastSlash )
		*strLastSlash = 0;

	_tcscat( strExePath, _T("\\") );

	gkStdString ret(strExePath);
	RemoveRelativeParts(ret);

	return ret;
#endif
}

inline gkStdString gkGetEngineRootDir()
{
	return (gkGetExecRootDir() + _T("engine/"));
}

inline gkStdString gkGetGameRootDir()
{
	return (gkGetExecRootDir() + _T("media/"));
}

inline gkStdString gkGetMaterialDir()
{
	return (gkGetExecRootDir() + _T("media/material/"));
}

typedef void (*fnGoThrough)(const TCHAR* filename);

inline bool gkGoThroughFolder(const TCHAR* SrcFolderpath, fnGoThrough func )
{
#if defined ( OS_ANDROID ) || defined ( OS_IOS ) 
	return S_FALSE;

#else

	TCHAR path[1024];
	_tcscpy_s(path, SrcFolderpath);
	_tcscat_s(path, _T("\\*.*"));

	WIN32_FIND_DATA fd;
	HANDLE hff = FindFirstFile(path, &fd);
	BOOL bIsFind = TRUE;
	TCHAR newpath[1024];

	while(hff && bIsFind)
	{
		if(fd.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
		{
			if(_tcscmp(fd.cFileName, _T(".")) && _tcscmp(fd.cFileName, _T("..")))
			{
				_stprintf_s(newpath, _T("%s\\%s"), SrcFolderpath, fd.cFileName);
				gkGoThroughFolder(newpath, func);
			}
		}
		else
		{
			TCHAR buffer[MAX_PATH];
			_tcscpy(buffer, SrcFolderpath);
			_tcscat(buffer, _T("\\"));
			_tcscat(buffer, fd.cFileName);
			func( buffer );
		}
		bIsFind = FindNextFile(hff, &fd);
	}
	return false;
#endif
}

inline bool gkFindFirstFile(TCHAR* destFilepath, const TCHAR* SrcFolderpath, const TCHAR* pureFilename)
{
#if defined ( OS_ANDROID ) || defined ( OS_IOS ) 
	return S_FALSE;

#else

	TCHAR path[1024];
	_tcscpy_s(path, SrcFolderpath);
	_tcscat_s(path, _T("\\*.*"));

	WIN32_FIND_DATA fd;
	HANDLE hff = FindFirstFile(path, &fd);
	BOOL bIsFind = TRUE;
	TCHAR newpath[1024];

	while(hff && bIsFind)
	{
		if(fd.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
		{
			if(_tcscmp(fd.cFileName, _T(".")) && _tcscmp(fd.cFileName, _T("..")))
			{
				_stprintf_s(newpath, _T("%s\\%s"), SrcFolderpath, fd.cFileName);
				if (gkFindFirstFile(destFilepath, newpath, pureFilename))
				{
					::FindClose(hff);
					return true;
				}
			}
		}
		else
		{
			if(!_tcsicmp(pureFilename, fd.cFileName))
			{
				_stprintf_s( destFilepath, MAX_PATH, _T("%s\\%s"), SrcFolderpath, pureFilename );
				{
					::FindClose(hff);
					return true;
				}
				
			}
		}
		bIsFind = FindNextFile(hff, &fd);
	}
	::FindClose(hff);
	return false;
	#endif
}

inline bool gkFindMediaSearchParentDirs( TCHAR* strSearchPath, int cchSearch, const TCHAR* strLeaf,
										const TCHAR* strExePath )
{
// #ifdef OS_WIN32
// 	if( GetFileAttributes( strLeaf ) != 0xFFFFFFFF )
// 	{
// 		_stprintf( strSearchPath, _T("%s"), strLeaf );
// 		return true;
// 	}
// #endif

	gkStdString normalized = RemoveSlash(strExePath);
	_stprintf( strSearchPath, _T("%s/%s"), normalized.c_str(), strLeaf );
	
#ifdef OS_WIN32
	if ( GetFileAttributes( strLeaf ) != 0xFFFFFFFF )
	{
		_tcscpy( strSearchPath, strLeaf );
		return true;
	}


	if( GetFileAttributes( strSearchPath ) != 0xFFFFFFFF )
		return true;
	return false;
#endif

	return true;
}

inline HRESULT gkFindFileRelativeExec( TCHAR* strDestPath, int cchDest, const TCHAR* strFilename )
{
	bool bFound;

	if( NULL == strFilename || strFilename[0] == 0 || NULL == strDestPath || cchDest < 10 )
		return S_FALSE;

	// Backup Gameroot
	gkStdString wstrGameRoot = gkGetExecRootDir();

	// Try to access file directly
	bFound = gkFindMediaSearchParentDirs( strDestPath, cchDest, strFilename, wstrGameRoot.c_str() );
	if( bFound )
		return S_OK;

#ifdef OS_ANDROID
	return S_FALSE;
#endif

	// chop path to filename
	TCHAR* strLastSlash = NULL;
	TCHAR strPureFilename[MAX_PATH] = {0};
	_tcscpy( strPureFilename, strFilename );
// 	strLastSlash = _tcsrchr( strPureFilename, _T( '\\' ) );
// 	if (strLastSlash)
// 	{
// 		_tcscpy( strPureFilename, strLastSlash );
// 	}
// 	else
// 	{
		strLastSlash = _tcsrchr( strPureFilename, _T( '/' ) );
		if (strLastSlash)
		{
			_tcscpy( strPureFilename, strLastSlash + 1 );
		}
//	}


	bFound = gkFindFirstFile(strDestPath, wstrGameRoot.c_str(), strPureFilename);
	if( bFound )
		return S_OK;

	// On failure, return the file as the path but also return an error code
	_tcscpy( strDestPath, strFilename );

	return S_FALSE;
}

inline HRESULT gkFindFileRelativeGame( TCHAR* strDestPath, int cchDest, const TCHAR* strFilename )
{
	bool bFound;
	if( NULL == strFilename || strFilename[0] == 0 || NULL == strDestPath || cchDest < 10 )
		return S_FALSE;

	// Backup Gameroot
	gkStdString wstrGameRoot = gkGetGameRootDir();

	// Try to access file directly
	bFound = gkFindMediaSearchParentDirs( strDestPath, cchDest, strFilename, wstrGameRoot.c_str() );
	if( bFound )
		return S_OK;

#ifdef OS_ANDROID
	return S_FALSE;
#endif

	// chop path to filename
	TCHAR* strLastSlash = NULL;
	TCHAR strPureFilename[MAX_PATH] = {0};
	_tcscpy( strPureFilename, strFilename );
	strLastSlash = _tcsrchr( strPureFilename, _T( '/' ) );
	if (strLastSlash)
		_tcscpy( strPureFilename, strLastSlash + 1 );
		

	bFound = gkFindFirstFile(strDestPath, wstrGameRoot.c_str(), strPureFilename);
	if( bFound )
		return S_OK;

	// do not help find exec dir
// 	HRESULT hr = gkFindFileRelativeExec(strDestPath, cchDest, strPureFilename);
// 	if (hr == S_OK)
// 		return S_OK;

	// On failure, return the file as the path but also return an error code
	_tcscpy( strDestPath, strFilename );

	return S_FALSE;
}

inline gkStdString gkGetAbsGamePath( const gkStdString relPath )
{
	return (gkGetGameRootDir() + relPath);
}

inline gkStdString gkGetAbsExecPath( const gkStdString relPath )
{
	return (gkGetExecRootDir() + relPath);
}

inline gkStdString gkGetExecRelativePath(const gkStdString absPath, bool purename = false)
{
	// 如果已经是相对路径了，直接返回
	if ( gkIsRelPath(absPath) && !purename )
	{
		return absPath;
	}

	gkStdString normalizedPath(absPath);
	gkNormalizeResName( normalizedPath );

	TCHAR fullpath[MAX_PATH];
	HRESULT hr = gkFindFileRelativeExec(fullpath, MAX_PATH, normalizedPath.c_str());
	if (hr == S_FALSE)
	{
		//gkLogError( _T("find file %s failed."), fullpath );
	}
	uint32 start = _tcslen(gkGetExecRootDir().c_str());

	if (start > _tcslen(fullpath))
	{
		return _T("bad");
	}

	gkStdString relpath(fullpath + start);
	gkNormalizePath(relpath);
	return relpath;
}

inline gkStdString gkGetGameRelativePath(const gkStdString absPath, bool purename = false)
{
	// 如果已经是相对路径了，直接返回
	if ( !purename && gkIsRelPath(absPath) )
	{
		return absPath;
	}

	gkStdString normalizedPath(absPath);
	gkNormalizeResName( normalizedPath );

	TCHAR fullpath[MAX_PATH];
	gkFindFileRelativeGame(fullpath, MAX_PATH, normalizedPath.c_str());
	uint32 start = _tcslen(gkGetGameRootDir().c_str());

	if (start > _tcslen(fullpath))
	{
		// here find ExecPath
		return gkGetExecRelativePath(normalizedPath, purename);
	}

	gkStdString relpath(fullpath + start);

	gkNormalizePath(relpath);
	return relpath;
}

inline gkStdString gkShortName( gkStdString& fullname )
{
	gkStdString shortname;
	int p = fullname.rfind('|');
	if (p >= 0)
	{
		shortname = fullname.substr(p + 1);
	}
	else
	{
		p = fullname.rfind('\\');
		if (p >= 0)
		{
			shortname = fullname.substr(p + 1);
		}
		else
		{
			p = fullname.rfind('/');
			if (p >= 0)
			{
				shortname = fullname.substr(p + 1);
			}
			else
			{
				shortname = fullname;
			}
		}
	}

	//finally, cut the .xxx
	p = shortname.rfind('.');
	if (p >= 0)
	{
		shortname = shortname.substr(0, p);
	}

	return shortname;
}

inline gkStdString gkGetFilename( const TCHAR* filename )
{
	gkStdString ret(filename);

	int p = ret.rfind('/');
	if (p >= 0)
	{
		ret = ret.substr(p);
	}
	return ret;
}

inline gkStdString gkGetPureFilename( const TCHAR* filename )
{
	gkStdString ret(filename);

	gkNormalizeResName( ret );

	ret = gkGetFilename( ret.c_str() );

	int p = ret.rfind('.');
	if (p >= 0)
	{
// #ifdef OS_ANDROID
// 		ret = ret.substr(p);
// #else
		ret = ret.substr(0, p);
//#endif
	}
	return ret;
}

inline gkStdString gkGetPurePath( const TCHAR* filename)
{
	gkStdString ret(filename);

	gkNormalizeResName( ret );

	int p = ret.rfind('/');
	if (p >= 0)
	{
// #ifdef OS_ANDROID
// 		ret = ret.substr(p);
// #else
		ret = ret.substr(0, p);
//#endif
	}
	return ret;
}

#ifdef OS_WIN32
inline void enum_all_files_in_folder( const TCHAR* root_path,std::vector<gkStdString>& result,bool inc_sub_folders/*=false*/ )
{

	if(!root_path)
	{
		return;
	}

	//要查找的目录
	gkStdString str = root_path;
	if (!is_end_with_slash(root_path))
	{
		str += _T("/");
	}
	std::stack<gkStdString> path_buf;
	path_buf.push(str);

	while(path_buf.size())
	{
		//取出来栈顶item
		gkStdString path = path_buf.top();
		path_buf.pop();
		size_t k=path_buf.size();

		gkStdString find_path = path + _T("*.*");

		_tfinddata_t file;
		intptr_t longf = _tfindfirst(find_path.c_str(), &file);

		if(longf !=-1)
		{
			gkStdString tempName;
			while(_tfindnext(longf, &file ) != -1)
			{
				tempName = _T("");
				tempName = file.name;
				if (tempName == _T("..") || tempName == _T("."))
				{
					continue;
				}
				if (file.attrib == _A_SUBDIR)
				{
					if (inc_sub_folders)
					{
						tempName += _T("\\");
						tempName = path + tempName;
						path_buf.push(tempName);
					}
				}
				else
				{
					//tempName = path + tempName;
					//gkStdString cur_path = get_relative_file_or_path(tempName.c_str(),root_path);
					//result.push_back(cur_path);

					result.push_back(tempName);
				}
			}
		}
		_findclose(longf);
	}
}
#elif defined( OS_ANDROID )
#include "dirent.h"
inline void enum_all_files_in_folder( const TCHAR* root_path,std::vector<gkStdString>& result,bool inc_sub_folders/*=false*/ )
{
	DIR *dp;  
	struct dirent *dmsg;  
	int i=0;  
	char addpath[PATH_MAX] = {'\0'}, *tmpstr;  
	if ((dp = opendir(root_path)) != NULL)  
	{  

		while ((dmsg = readdir(dp)) != NULL)  
		{  
			if (!strcmp(dmsg->d_name, ".") || !strcmp(dmsg->d_name, ".."))  
				continue;  

			strcpy(addpath, root_path);  
			strcat(addpath, "/");  
			strcat(addpath, dmsg->d_name);  
			if (dmsg->d_type == DT_DIR )  
			{  
				char *temp;  
				temp=dmsg->d_name;  
				if(strchr(dmsg->d_name, '.'))  
				{  
					if((strcmp(strchr(dmsg->d_name, '.'), dmsg->d_name)==0))  
					{  
						continue;  
					}  
				}  
				//LOGI("........directname:%s",dmsg->d_name);  

				if (inc_sub_folders)
				{
					enum_all_files_in_folder(addpath, result, inc_sub_folders);  
				}			
			}  
			else
			{
				//gkLogMessage( addpath );
				gkStdString normedpath(addpath);
				gkNormalizePath(normedpath);
				result.push_back( normedpath.c_str() );
			}
		}  
	}  
	closedir(dp);     
}
#else
void enum_all_files_in_folder( const TCHAR* root_path,std::vector<gkStdString>& result,bool inc_sub_folders/*=false*/ );
#endif

#endif //GKFILEPATH_H_
