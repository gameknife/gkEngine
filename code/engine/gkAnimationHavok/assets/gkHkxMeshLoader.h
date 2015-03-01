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



/**
  @file gkHkxMeshLoader.h
  
  @author Kaiming

  更改日志 history
  ver:1.0
   
 */

#ifndef gkHkxMeshLoader_h__
#define gkHkxMeshLoader_h__

#include "IMesh.h"

class gkHkxMeshLoader : public IMeshLoader
{
public:
	gkHkxMeshLoader(void);
	~gkHkxMeshLoader(void);

	virtual bool LoadGeometry( const TCHAR* filename );

	virtual gkVertexBuffer* getVB();

	virtual gkIndexBuffer* getIB();

	virtual uint32 getSubsetCount();

	virtual void getSubset( uint32 index, uint32& start, uint32& count, gkStdString& mtlname );

	virtual void FinishLoading();

	void Destroy();

	virtual uint32 GetBoneBaseTransformCount() {return m_boneBaseTransforms.size();}

	virtual void GetBoneBaseTransform( Matrix44& matrix, uint32 index );

	std::vector<GKVL_P3F2F4F3F4U4> m_Vertices;     // Filled and copied to the vertex buffer
	std::vector<uint32> m_Indices;       // Filled and copied to the index buffer

	gkMeshSubsets m_Subsets;

	gkVertexBuffer* m_cachedVB;
	gkIndexBuffer* m_cachedIB;

	IMesh::BoneBaseTransforms m_boneBaseTransforms;
};

#endif


