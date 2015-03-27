//////////////////////////////////////////////////////////////////////////
//
// yikaiming (C) 2014
// gkENGINE Source File 
//
// Name:   	ITextureLoader.h
// Desc:	
// 	
// 
// Author:  gameKnife
// Date:	2015/3/27
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _ITextureLoader_h_
#define _ITextureLoader_h_


enum EHwTexError
{
	PVR_SUCCESS = 0,
	PVR_FAIL = 1,
	PVR_OVERFLOW = 2
};

EHwTexError LoadPVR(const void* pointer,
	GLuint *const texName,
	const unsigned int nLoadFromLevel);

struct HWT_Loader
{
	virtual EHwTexError Load_Data(const void* pointer, uint32 size ) =0;
	virtual EHwTexError Load_Bind(GLuint *const texName, const unsigned int nLoadFromLevel) =0;
	virtual bool Load_IsCube(const void* pointer ) =0;

	virtual uint32 getWidth() =0;
	virtual uint32 getHeight() =0;
};

#endif