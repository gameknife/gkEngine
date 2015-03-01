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
// K&K Studio GameKnife ENGINE Source File
//
// Name:   	gkResFileManager.h
// Desc:		
// 	
// Author:  Kaiming
// Date:	2012/6/21
// Modify:	
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _gkResFileManager_h_
#define _gkResFileManager_h_

#include "IResFile.h"
#include "crc32.h"

struct gkPak
{
	gkPak( const TCHAR* pszPakFile );
	~gkPak();

	bool isOpen() {return ( m_handle != 0 ); }

	bool checkPakFile( const TCHAR* pszRelFilename );
	bool openPakFile( const TCHAR* pszRelFilename, char** pData, size_t &size);
	bool closePakFile( const TCHAR* pszRelFilename );

	FILE* m_handle;
	gkPak_Header m_pakHeader;
	gkPakRecordMap m_pakMap;

	static Crc32Gen s_crcGenerator;
};
typedef std::map< gkStdString, gkPak* > gkPakMap;

class gkResFileManager : public IResFileManager
{
public:
	gkResFileManager();
	virtual ~gkResFileManager();

	virtual IResFile* loadResFile( const TCHAR* pszFilename, bool searchDir = false );
	virtual IResFile* loadResFileExec(const TCHAR* pszFilename, bool searchDir = false, bool loadheader = false);

	virtual IResFile* loadResFileHeader(const TCHAR* pszFilename, bool searchDir = false);

	IResFile* CreateFileInternal( TCHAR * szFullPath, bool loadheader );

	virtual void closeResFile( IResFile* file );
	virtual void closeResFile( const TCHAR* pszFilename );

	
	virtual void openPak(const TCHAR* pszFilename);
	virtual void closePak(const TCHAR* pszFilename);

	bool checkPakFile( const TCHAR* pszRelFilename );
	bool openPakFile( const TCHAR* pszRelFilename, char** pData, size_t &size);

	//////////////////////////////////////////////////////////////////////////
	// 检查文件是否存在，先从pak中检查
	virtual EFileStatus checkFileExist(const TCHAR* pszFilename, bool searchDir = false);

private:
	typedef std::map<gkStdString, IResFile*> ResFileMap;
	ResFileMap m_mapFiles;

	gkPakMap m_paks;
};

#endif


