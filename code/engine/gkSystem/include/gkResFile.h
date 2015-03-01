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
// Name:   	gkResFile.h
// Desc:		
// 	
// Author:  Kaiming
// Date:	2012/6/21
// Modify:	
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _gkResFile_h_
#define _gkResFile_h_

#include "IResFile.h"

#include <stdlib.h>

typedef void* (*PFNLoadFileFunc)(const TCHAR*, char** pData, size_t &size);
typedef bool  (*PFNReleaseFileFunc)(void* handle);

/*!***************************************************************************
 @Class CPVRTResourceFile
 @Brief Simple resource file wrapper
*****************************************************************************/
class gkResFile : public IResFile
{
public:
	/*!***************************************************************************
	@Function			SetReadPath
	@Input				pszReadPath The path where you would like to read from
	@Description		Sets the read path
	*****************************************************************************/
	static void SetReadPath(const TCHAR* pszReadPath);

	/*!***************************************************************************
	@Function			GetReadPath
	@Returns			The currently set read path
	@Description		Returns the currently set read path
	*****************************************************************************/
	static gkStdString GetReadPath();

	/*!***************************************************************************
	@Function			SetLoadReleaseFunctions
	@Input				pLoadFileFunc Function to use for opening a file
	@Input				pReleaseFileFunc Function to release any data allocated by the load function
	@Description		This function is used to override the CPVRTResource file loading functions. If
	                    you pass NULL in as the load function CPVRTResource will use the default functions.
	*****************************************************************************/
	static void SetLoadReleaseFunctions(void* pLoadFileFunc, void* pReleaseFileFunc);

	/*!***************************************************************************
	@Function			CPVRTResourceFile
	@Input				pszFilename Name of the file you would like to open
	@Description		Constructor
	*****************************************************************************/
	gkResFile(const TCHAR* pszFilename);

	gkResFile(const TCHAR* pszFilename, bool header);

	/*!***************************************************************************
	@Function			CPVRTResourceFile
	@Input				pData A pointer to the data you would like to use
	@Input				i32Size The size of the data
	@Description		Constructor
	*****************************************************************************/
	gkResFile(const char* pData, size_t i32Size);

	/*!***************************************************************************
	@Function			~CPVRTResourceFile
	@Description		Destructor
	*****************************************************************************/
	virtual ~gkResFile();

	/*!***************************************************************************
	@Function			IsOpen
	@Returns			true if the file is open
	@Description		Is the file open
	*****************************************************************************/
	bool IsOpen() const;

	/*!***************************************************************************
	@Function			IsMemoryFile
	@Returns			true if the file was opened from memory
	@Description		Was the file opened from memory
	*****************************************************************************/
	bool IsMemoryFile() const;

	/*!***************************************************************************
	@Function			Size
	@Returns			The size of the opened file
	@Description		Returns the size of the opened file
	*****************************************************************************/
	size_t Size() const;

	/*!***************************************************************************
	@Function			DataPtr
	@Returns			A pointer to the file data
	@Description		Returns a pointer to the file data. If the data is expected
						to be a string don't assume that it is null-terminated.
	*****************************************************************************/
	const void* DataPtr() const;

	/*!***************************************************************************
	@Function			Close
	@Description		Closes the file
	*****************************************************************************/
	void Close();

	virtual void AddRef() { m_refcount++; }  
	virtual void RemoveRef() { m_refcount--; }
	virtual int RefCount() {return m_refcount;}

protected:
	bool m_bOpen;
	bool m_bMemoryFile;
	size_t m_Size;
	const char* m_pData;
	void *m_Handle;

	int m_refcount;

	static gkStdString s_ReadPath;
	static PFNLoadFileFunc s_pLoadFileFunc;
	static PFNReleaseFileFunc s_pReleaseFileFunc;
};

#endif


