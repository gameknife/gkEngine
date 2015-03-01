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



#pragma once
#include <Common/Base/System/Io/Reader/hkStreamReader.h>
#include "Common/Serialize/Util/hkLoader.h"

struct IResFile;
class hkRootLevelContainer;

//struct gkBindResReader : public hkSeekableStreamReader

struct gkBindResReader : public hkSeekableStreamReader
{
	gkBindResReader( const TCHAR* name );
	~gkBindResReader();

	virtual hkBool isOk() const;
	virtual int read( void* buf, int nbytes );

	virtual hkResult seek( int offset, SeekWhence whence );

	virtual int tell() const;

	virtual int skip( int nbytes );

	virtual int peek(void* buf, int n);

	virtual hkBool markSupported() const;

	virtual hkResult setMark( int markLimit );

	virtual hkResult rewindToMark();

	IResFile* resfile;
	uint32 curpos;
	uint32 markpos;
};

class gkHavokLoader : public hkLoader
{
public:
	gkHavokLoader(void);
	~gkHavokLoader(void);

	virtual hkRootLevelContainer* load( const char* filename );

	virtual hkRootLevelContainer* load( hkStreamReader* reader );

	hkLoader* m_hkLoader;
};

