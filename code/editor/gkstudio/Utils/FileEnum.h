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
// Name:   	FileEnum.h
// Desc:		
// 	
// Author:  Kaiming
// Date:	2012/5/20
// Modify:	2012/5/20
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _FileEnum_h_
#define _FileEnum_h_


class CFileEnum  
{
public:
	static bool ScanDirectory( const CString &path,const CString &file,std::vector<CString> &files, bool recursive=true );

	bool GetNextFile(struct _tfinddata64_t *pFile);
	bool StartEnumeration( const TCHAR *szEnumPathAndPattern, _tfinddata64_t *pFile);
	bool StartEnumeration( const TCHAR *szEnumPath, TCHAR szEnumPattern[], _tfinddata64_t *pFile);
	CFileEnum();
	virtual ~CFileEnum();
protected:
	intptr_t m_hEnumFile;
};

#endif
