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
// Name:   	gkMesh.h
// Desc:	应该说来，下面还应该有一层submesh的，不过现在先这样了
//			少一层省一层事，下一版mesh再做
// 描述:	
// 
// Author:  Kaiming-Laptop
// Date:	2010/9/10
// Modify:	2010/12/11
//////////////////////////////////////////////////////////////////////////

#ifndef gkMesh_h_e1f4f1d7_0bed_4a10_b46e_15dde925b4a0
#define gkMesh_h_e1f4f1d7_0bed_4a10_b46e_15dde925b4a0

#include "Prerequisites.h"

#include "IMesh.h"
#include "IMeshLoader.h"
#include "gkRenderOperation.h"

struct gkMeshStreamingTask;

class gkMeshGLES2 : public IMesh {
	friend class gkMeshStreamingTask;
	friend class gkHavokMeshStreamingTask;
private:
	gkStdString m_wstrExtName;
	AABB m_AABB;

	gkNameValuePairList loadingParams;
	BoneBaseTransforms m_BoneBaseTransforms;

	gkVertexBuffer* m_pVB;
	gkIndexBuffer* m_pIB;
	gkMeshSubsets subsets;

	uint32 m_customPrimCount;

public:
	

	gkMeshGLES2(IResourceManager* creator, const gkStdString& name, gkResourceHandle handle,
		const gkStdString& group = _T("none"), gkNameValuePairList* params = NULL);
	virtual ~gkMeshGLES2(void);

	virtual void onReset() {}
	virtual void onLost() {}

	virtual BoneBaseTransforms& GetBoneBaseTransforms();
	virtual void getRenderOperation( gkRenderOperation& op, uint32 subset );

	virtual AABB& GetAABB();

	virtual bool RaycastMesh(Ray& ray, std::vector<float>& dists);
	virtual bool RaycastMesh_WorldSpace(Ray& ray, Matrix34& world_mat, std::vector<float>& dists);
	virtual uint32 getSubsetCount();

	virtual bool IsIndexDword();

	virtual gkVertexBuffer* getVertexBuffer();
	virtual gkIndexBuffer* getIndexBuffer();

	virtual void UpdateHwBuffer();
	virtual void modifyCustomPrimitiveCount(uint32 count);

	void BindHwBuffer();
	virtual void ReleaseSysBuffer();

protected:
	virtual bool loadImpl(void);	
	virtual bool loadObj(const TCHAR* wszFileName);			// 读obj文件
	virtual bool loadHavokMesh(const TCHAR* wszFileName);			// 读hkxMesh文件
	//virtual bool loadHavokMesh(IDirect3DDevice9* d3d9Device, const TCHAR* wszFileName, const TCHAR* wszBinding);			// 读hkxMesh文件
	virtual bool unloadImpl(void);

	ITask* m_loadingTask;

    bool m_bufferReady;
};

#endif // gkMesh_h_e1f4f1d7_0bed_4a10_b46e_15dde925b4a0
