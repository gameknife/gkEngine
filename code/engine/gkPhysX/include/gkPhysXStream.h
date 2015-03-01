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
// Name:   	gkPhysXStream.h
// Desc:		
// 	
// Author:  Kaiming
// Date:	2012/8/12
// Modify:	
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _gkPhysXStream_h_
#define _gkPhysXStream_h_

namespace physx
{
	class PxTriangleMesh;
	class PxConvexMesh;
	class PxPhysics;
	class PxCooking;
	class PxTriangleMeshDesc;
}

class MemoryOutputStream: public physx::PxOutputStream
{
public:
	MemoryOutputStream();
	virtual				~MemoryOutputStream();

	PxU32		write(const void* src, PxU32 count);

	PxU32		getSize()	const	{	return mSize; }
	PxU8*		getData()	const	{	return mData; }
private:
	PxU8*		mData;
	PxU32		mSize;
	PxU32		mCapacity;
};

class FileOutputStream: public physx::PxOutputStream
{
public:
	FileOutputStream(const char* name);
	virtual				~FileOutputStream();

	PxU32		write(const void* src, PxU32 count);

	bool		isValid();
private:
	FILE*		mFile;
};

class MemoryInputData: public physx::PxInputData
{
public:
	MemoryInputData(PxU8* data, PxU32 length);

	PxU32		read(void* dest, PxU32 count);
	PxU32		getLength() const;
	void		seek(PxU32 pos);
	PxU32		tell() const;

private:
	PxU32		mSize;
	const PxU8*	mData;
	PxU32		mPos;
};

class FileInputData: public physx::PxInputData
{
public:
	FileInputData(const char* name);
	virtual				~FileInputData();

	PxU32		read(void* dest, PxU32 count);
	void		seek(PxU32 pos);
	PxU32		tell() const;
	PxU32		getLength() const;

	bool		isValid() const;
private:
	FILE*		mFile;
	PxU32		mLength;
};

#endif


