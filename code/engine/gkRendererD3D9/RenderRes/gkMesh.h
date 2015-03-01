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

//			mesh保留pvb和pib，用来作精确的hit-test
//			D3D方面使用DEFAULT资源，设备丢失从保留的pvb重建
//			下一版本考虑支持half精度的顶点，减少地形等内存占用

// 描述:	
// 
// Author:  Kaiming-Laptop
// Date:	2010/9/10
// Modify:	2010/12/11
//////////////////////////////////////////////////////////////////////////

#ifndef gkMesh_h_e1f4f1d7_0bed_4a10_b46e_15dde925b4a0
#define gkMesh_h_e1f4f1d7_0bed_4a10_b46e_15dde925b4a0

#include "RendererD3D9Prerequisites.h"

#include "IMesh.h"
#include "gkRenderOperation.h"

struct gkMeshStreamingTask;
struct gkHavokMeshStreamingTask;
struct ITask;

class gkMesh : public IMesh {
	friend gkMeshStreamingTask;
	friend gkHavokMeshStreamingTask;
private:
	/// 文件扩展名
	gkStdString m_wstrExtName;
	AABB m_AABB;
	
	gkNameValuePairList loadingParams;
	BoneBaseTransforms m_BoneBaseTransforms;

	uint32 m_subsetCount;
	gkVertexBuffer* m_pVB;
	gkIndexBuffer* m_pIB;
	gkMeshSubsets subsets;

	uint32 m_customPrimCount;

	struct IDirect3DDevice9* m_pDevice;

	ITask* m_loadingTask;

	volatile bool m_ready;
public:
	

	gkMesh(IResourceManager* creator, const gkStdString& name, gkResourceHandle handle,
		const gkStdString& group = _T("none"), gkNameValuePairList* params = NULL);
	virtual ~gkMesh(void);

	virtual void onReset();
	virtual void onLost();

	virtual BoneBaseTransforms& GetBoneBaseTransforms();
	virtual void getRenderOperation( gkRenderOperation& op, uint32 subset );

	virtual AABB& GetAABB();

	virtual bool RaycastMesh(Ray& ray, std::vector<float>& dists);
	virtual bool RaycastMesh_WorldSpace(Ray& ray, Matrix34& world_mat, std::vector<float>& dists);

	virtual uint32 getSubsetCount();

	virtual gkVertexBuffer* getVertexBuffer();
	virtual gkIndexBuffer* getIndexBuffer();

	void BindHwBuffer();
	virtual void UpdateHwBuffer();
	virtual void ReleaseSysBuffer();

	virtual void modifyCustomPrimitiveCount(uint32 count);

protected:
	virtual bool loadImpl(void);	
	virtual bool loadImpl(IDirect3DDevice9* d3d9Device);		// dx的构建函数
	virtual bool loadObj(IDirect3DDevice9* d3d9Device, const TCHAR* wszFileName);			// 读obj文件
	virtual bool loadHavokMesh(IDirect3DDevice9* d3d9Device, const TCHAR* wszFileName);			// 读hkxMesh文件
	virtual bool unloadImpl(void);
};

#endif // gkMesh_h_e1f4f1d7_0bed_4a10_b46e_15dde925b4a0
