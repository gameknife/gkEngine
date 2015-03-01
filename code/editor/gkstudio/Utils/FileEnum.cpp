// FileEnum.cpp: Implementation of the class CFileEnum.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FileEnum.h"

#include <io.h>

//////////////////////////////////////////////////////////////////////
// Construcktion / destrucktion
//////////////////////////////////////////////////////////////////////

CFileEnum::CFileEnum()
{
	m_hEnumFile = 0;
}

CFileEnum::~CFileEnum()
{
	// End the enumeration
	if (m_hEnumFile)
	{
		_findclose(m_hEnumFile);
		m_hEnumFile = 0;
	}
}

bool CFileEnum::StartEnumeration( const TCHAR* szEnumPath, TCHAR szEnumPattern[], _tfinddata64_t *pFile)
{
	//////////////////////////////////////////////////////////////////////
	// Take path and search pattern as separate arguments
	//////////////////////////////////////////////////////////////////////

	TCHAR szPath[_MAX_PATH];

	// Build enumeration path
	_tcscpy(szPath, szEnumPath);
	if (szPath[_tcslen(szPath)] != L'\\' &&
		szPath[_tcslen(szPath)] != L'/')
	{
		_tcscat(szPath, _T("\\"));
	}
	_tcscat(szPath, szEnumPattern);

	return StartEnumeration(szPath, pFile);
}

bool CFileEnum::StartEnumeration( const TCHAR *szEnumPathAndPattern, _tfinddata64_t *pFile)
{
	//////////////////////////////////////////////////////////////////////
	// Start a new file enumeration
	//////////////////////////////////////////////////////////////////////

	// End any previous enumeration
	if (m_hEnumFile)
	{
		_findclose(m_hEnumFile);
		m_hEnumFile = 0;
	}

	// Start the enumeration
	if ((m_hEnumFile = _tfindfirst64(szEnumPathAndPattern, pFile)) == -1L)
	{
		// No files found
		_findclose(m_hEnumFile);
		m_hEnumFile = 0;
		return false;
	}

	return true;
}

bool CFileEnum::GetNextFile(_tfinddata64_t *pFile)
{
	//////////////////////////////////////////////////////////////////////
	// Start a new file enumeration
	//////////////////////////////////////////////////////////////////////

	// Fill file strcuture
	if (_tfindnext64(m_hEnumFile, pFile) == -1)
	{
		// No more files left
		_findclose(m_hEnumFile);
		m_hEnumFile = 0;
		return false;
	};

	// At least one file left
	return true;
}


// Get directory contents.
inline bool ScanDirectoryRecursive( const CString &root,const CString &path,const CString &file,std::vector<CString> &files, bool recursive )
{
	struct _tfinddata64_t c_file;
	intptr_t hFile;

	bool anyFound = false;

	CString fullPath = root + path + file;
	if( (hFile = _tfindfirst64( fullPath, &c_file )) != -1L )
	{
		// Find the rest of the files.
		do {
			anyFound = true;
			files.push_back( path + c_file.name );
		}	while (_tfindnext64( hFile, &c_file ) == 0);
		_findclose( hFile );
	}

	if (recursive)
	{
		fullPath = root + path + _T("*.*");
		if( (hFile = _tfindfirst64( fullPath, &c_file )) != -1L )
		{
			// Find directories.
			do {
				if (c_file.attrib & _A_SUBDIR)
				{
					// If recursive.
					if (c_file.name[0] != L'.')
					{
						if (ScanDirectoryRecursive( root,path + c_file.name + _T("\\"),file,files,recursive ))
							anyFound = true;
					}
				}
			}	while (_tfindnext64( hFile, &c_file ) == 0);
			_findclose( hFile );
		}
	}

	return anyFound;
}

bool CFileEnum::ScanDirectory( const CString &path,const CString &file,std::vector<CString> &files, bool recursive )
{
	return ScanDirectoryRecursive(path,_T(""),file,files,recursive );
}