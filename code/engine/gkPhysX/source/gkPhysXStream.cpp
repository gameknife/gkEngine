#include "StableHeader.h"
#include "gkPhysXStream.h"

///////////////////////////////////////////////////////////////////////////////

MemoryOutputStream::MemoryOutputStream() :
mData		(NULL),
	mSize		(0),
	mCapacity	(0)
{
}

MemoryOutputStream::~MemoryOutputStream()
{
	if(mData)
		delete[] mData;
}

PxU32 MemoryOutputStream::write(const void* src, PxU32 size)
{
	PxU32 expectedSize = mSize + size;
	if(expectedSize > mCapacity)
	{
		mCapacity = expectedSize + 4096;

		PxU8* newData = new PxU8[mCapacity];
		PX_ASSERT(newData!=NULL);

		if(newData)
		{
			memcpy(newData, mData, mSize);
			delete[] mData;
		}
		mData = newData;
	}
	memcpy(mData+mSize, src, size);
	mSize += size;
	return size;
}

///////////////////////////////////////////////////////////////////////////////

MemoryInputData::MemoryInputData(PxU8* data, PxU32 length) :
mSize	(length),
	mData	(data),
	mPos	(0)
{
}

PxU32 MemoryInputData::read(void* dest, PxU32 count)
{
	PxU32 length = PxMin<PxU32>(count, mSize-mPos);
	memcpy(dest, mData+mPos, length);
	mPos += length;
	return length;
}

PxU32 MemoryInputData::getLength() const
{
	return mSize;
}

void MemoryInputData::seek(PxU32 offset)
{
	mPos = PxMin<PxU32>(mSize, offset);
}

PxU32 MemoryInputData::tell() const
{
	return mPos;
}

///////////////////////////////////////////////////////////////////////////////

FileOutputStream::FileOutputStream(const char* filename)
{
	mFile = NULL;
	fopen_s(&mFile, filename, "wb");
}

FileOutputStream::~FileOutputStream()
{
	if(mFile)
		fclose(mFile);
}

PxU32 FileOutputStream::write(const void* src, PxU32 count)
{
	return mFile ? (PxU32)fwrite(src, 1, count, mFile) : 0;
}

bool FileOutputStream::isValid()
{
	return mFile != NULL;
}

///////////////////////////////////////////////////////////////////////////////

FileInputData::FileInputData(const char* filename)
{
	mFile = NULL;
	fopen_s(&mFile, filename, "rb");

	if(mFile)
	{
		fseek(mFile, 0, SEEK_END);
		mLength = ftell(mFile);
		fseek(mFile, 0, SEEK_SET);
	}
	else
	{
		mLength = 0;
	}
}

FileInputData::~FileInputData()
{
	if(mFile)
		fclose(mFile);
}

PxU32 FileInputData::read(void* dest, PxU32 count)
{
	PX_ASSERT(mFile);
	const size_t size = fread(dest, 1, count, mFile);
	PX_ASSERT(PxU32(size)==count);
	return PxU32(size);
}

PxU32 FileInputData::getLength() const
{
	return mLength;
}

void FileInputData::seek(PxU32 pos)
{
	fseek(mFile, pos, SEEK_SET);
}

PxU32 FileInputData::tell() const
{
	return ftell(mFile);
}

bool FileInputData::isValid() const
{
	return mFile != NULL;
}
// 
// ///////////////////////////////////////////////////////////////////////////////
// 
// PxTriangleMesh* PxToolkit::createTriangleMesh32(PxPhysics& physics, PxCooking& cooking, const PxVec3* verts, PxU32 vertCount, const PxU32* indices32, PxU32 triCount)
// {
// 	PxTriangleMeshDesc meshDesc;
// 	meshDesc.points.count			= vertCount;
// 	meshDesc.points.stride			= sizeof(PxVec3);
// 	meshDesc.points.data			= verts;
// 
// 	meshDesc.triangles.count		= triCount;
// 	meshDesc.triangles.stride		= 3*sizeof(PxU32);
// 	meshDesc.triangles.data			= indices32;
// 
// 	PxToolkit::MemoryOutputStream writeBuffer;
// 	bool status = cooking.cookTriangleMesh(meshDesc, writeBuffer);
// 	if(!status)
// 		return NULL;
// 
// 	PxToolkit::MemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
// 	return physics.createTriangleMesh(readBuffer);
// }
// 
// PxTriangleMesh* PxToolkit::createTriangleMesh32(PxPhysics& physics, PxCooking& cooking, PxTriangleMeshDesc* meshDesc)
// {
// 	PxToolkit::MemoryOutputStream writeBuffer;
// 	bool status = cooking.cookTriangleMesh(*meshDesc, writeBuffer);
// 	if(!status)
// 		return NULL;
// 
// 	PxToolkit::MemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
// 	return physics.createTriangleMesh(readBuffer);
// }
// 
// PxConvexMesh* PxToolkit::createConvexMesh(PxPhysics& physics, PxCooking& cooking, const PxVec3* verts, PxU32 vertCount, PxConvexFlags flags)
// {
// 	PxConvexMeshDesc convexDesc;
// 	convexDesc.points.count			= vertCount;
// 	convexDesc.points.stride		= sizeof(PxVec3);
// 	convexDesc.points.data			= verts;
// 	convexDesc.flags				= flags;
// 
// 	MemoryOutputStream buf;
// 	if(!cooking.cookConvexMesh(convexDesc, buf))
// 		return NULL;
// 
// 	PxToolkit::MemoryInputData input(buf.getData(), buf.getSize());
// 	return physics.createConvexMesh(input);
// }
// 
