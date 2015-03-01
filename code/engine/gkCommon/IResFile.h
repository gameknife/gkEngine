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
// Name:   	IResFile.h
// Desc:		
// 	
// Author:  Kaiming
// Date:	2012/6/21
// Modify:	
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _IResFile_h_
#define _IResFile_h_

#define GKPAK_UNCOMPRESS_HEADER_SIZE 1024

struct gkPak_FileRecord
{
	uint32 crc32;
	uint32 start;
	uint32 size;
	uint32 compressHeaderSize;
};
typedef std::map<uint32, gkPak_FileRecord> gkPakRecordMap;

struct gkPak_Header
{
	uint32 m_size;
	uint32 m_count;
	uint32 m_data_offset;

	gkPak_Header()
	{
		m_size = 0;
		m_count = 0;
		m_data_offset = 0;
	}
};

struct IResFile
{
	virtual ~IResFile() {}

	/*!***************************************************************************
	@Function			IsOpen
	@Returns			true if the file is open
	@Description		Is the file open
	*****************************************************************************/
	virtual bool IsOpen() const =0;

	/*!***************************************************************************
	@Function			IsMemoryFile
	@Returns			true if the file was opened from memory
	@Description		Was the file opened from memory
	*****************************************************************************/
	virtual bool IsMemoryFile() const =0;

	/*!***************************************************************************
	@Function			Size
	@Returns			The size of the opened file
	@Description		Returns the size of the opened file
	*****************************************************************************/
	virtual size_t Size() const =0;

	/*!***************************************************************************
	@Function			DataPtr
	@Returns			A pointer to the file data
	@Description		Returns a pointer to the file data. If the data is expected
						to be a string don't assume that it is null-terminated.
	*****************************************************************************/
	virtual const void* DataPtr() const =0;

	/*!***************************************************************************
	@Function			Close
	@Description		Closes the file
	*****************************************************************************/
	virtual void Close() =0;

	virtual void AddRef() =0;
	virtual void RemoveRef() =0;
	virtual int RefCount() =0;
};

enum EFileStatus
{
	eFS_inPak,
	eFS_inDisk,
	eFS_notExsit,
};

struct IResFileManager
{
	virtual ~IResFileManager() {}
	//////////////////////////////////////////////////////////////////////////
	// function load resfile
	virtual IResFile* loadResFile(const TCHAR* pszFilename, bool searchDir = false) =0;
	//////////////////////////////////////////////////////////////////////////
	// function load resfile header
	virtual IResFile* loadResFileHeader(const TCHAR* pszFilename, bool searchDir = false) =0;

	//////////////////////////////////////////////////////////////////////////
	// function close resfile
	virtual void closeResFile( IResFile* file ) =0;
	virtual void closeResFile( const TCHAR* pszFilename ) =0;

	virtual void openPak(const TCHAR* pszFilename) =0;
	virtual void closePak(const TCHAR* pszFilename) =0;

	//////////////////////////////////////////////////////////////////////////
	// 检查文件是否存在，先从pak中检查
	virtual EFileStatus checkFileExist(const TCHAR* pszFilename, bool searchDir = false) =0;
};

#endif


