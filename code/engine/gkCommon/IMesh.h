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
// yikaiming (C) 2013
// gkENGINE Source File 
//
// Name:   	IMesh.h
// Desc:	
// 	
// 
// Author:  YiKaiming
// Date:	2010/9/10
// Modify:	2010/12/11
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _IMesh_h_
#define _IMesh_h_

#include "gkPlatform.h"
#include "IResource.h"
#include "Mathlib/gk_Geo.h"

struct gkRenderOperation;

#define MAX_MESHATTRIBUTE_COUNT 100
/**
 @brief VB/IB的类型
 @remark 决定是否要保留BUFFER
		 \n如果需要做几何级别的射线求交，需要保留。
*/
enum EBufferType
{
	eBF_Discard,
	eBF_Reserved,
};
/**
 @brief 顶点输入类型
 @remark 
*/
enum EVertexInputType
{
	eVI_T2T2,
	eVI_P4F4F4,
	eVI_P4F4F4F4U4,
	eVI_P3T2U4,
	eVI_PT2T2T2T2T2,			//-用于oculus disortation的顶点结构
};
/**
 @brief GPU粒子的精简顶点Layout
 @remark 
*/
struct GKVL_GpuParticle
{
	Vec2 texcoord0;
	Vec2 texcoord1;
};
/**
 @brief UI渲染的顶点格式P3T2U4
 @remark 一般的顶点均由此构成，长度为6个float
*/
struct GKVL_P3T2U4
{
	Vec3 position;
	Vec2 texcoord;
	uint32 color;
};

struct GKVL_Pt2T2T2T2T2
{
	Vec2 position;
	Vec2 timewarp_vig;
	Vec2 tc0_r;
	Vec2 tc1_g;
	Vec2 tc2_b;
};
/**
 @brief 普遍的顶点格式P3T2TG4BN3
 @remark 一般的顶点均由此构成，长度为12个float = 48Byte
*/
struct GKVL_P3F2F4F3
{
	Vec3 position;
	Vec4 texcoord;
	Vec4 tangent_ti;
	Vec3 binormal;
};
/**
 @brief 普遍的顶点格式P3T2TG4BN3 + F4U4
 @remark 一般顶点的蒙皮版本，多添加了蒙皮权重和索引
*/
struct GKVL_P3F2F4F3F4U4
{
	Vec3 position;
	Vec2 texcoord;
	Vec4 tangent_ti;
	Vec3 binormal;
	Vec4_tpl<uint8>  blendWeight;
	Vec4_tpl<uint8> blendIndice;
};

enum EGKLAYOUT_ELEMENT_TYPE
{
	eLET_Position =				1 << 0,			// 顶点数据
	eLET_VertexColor =			1 << 1,			// 顶点色
	eLET_Texcoord0 =			1 << 2,			// 第一层纹理坐标
	eLET_Texcoord1 =			1 << 3,
	eLET_Texcoord2 =			1 << 4,
	eLET_Texcoord3 =			1 << 5,
	eLET_Texcoord4 =			1 << 6,
	eLET_Texcoord5 =			1 << 7,
	eLET_Texcoord6 =			1 << 8,
	eLET_Texcoord7 =			1 << 9,
};

//////////////////////////////////////////////////////////////////////////
// mesh加载结构

/**
 @brief gmf文件头
 @remark GMF文件可简单理解为二进制版本的OBJ文件，是gkENGINE的定制模型文件
		 \n可通过gkResourceCompiler来进行生成

		 \n \b gmf文件结构:
		 @code
		 //////////////////////////////////////////////////////////////////////////
		 // DATA STRUCT
		 // 'G''M''F' | POS COUNT | TEX COUNT | FACE COUNT | SUBSET COUNT
		 // POSITION...
		 // TEXCOORD...
		 // FACE...
		 // SUBSET INDEX...(DYNAMIC)
		 @endcode
*/


/**
 @brief gmf文件头
 @remark GMF文件可简单理解为二进制版本的OBJ文件，是gkENGINE的定制模型文件
		 \n可通过gkResourceCompiler来进行生成

		 \n \b gmf文件结构:
		 @code
		 //////////////////////////////////////////////////////////////////////////
		 // DATA STRUCT
		 // 'G''M''F' | VERT COUNT | FACE COUNT | SUBSET COUNT | VERT TYPE
		 // POSITION...
		 // VERT COLOR...
		 // TEXCOORD1...
		 // TEXCOORD2...
		 // FACE...
		 // SUBSET INDEX...(DYNAMIC)
		 @endcode
*/

#define GMF_VER_1_0 0x10

struct gmfHeader
{
	char identityExt[4];

	uint32 vertexPositionCount;
	uint32 vertexTexcoordCount;
	uint32 faceCount;
	uint32 subsetCount;
	bool dwordIndex;
};

struct gmfHeaderV1
{
	char identityExt[4];

	uint32 gmf_header_version;

	uint32 vertexPositionCount;
	uint32 vertexColorCount;
	uint32 vertexTexcoordCount[8];

	uint32 faceCount;
	uint32 subsetCount;

	bool dwordIndex;
};
/**
 @brief gmf面定义
 @remark 
 */
struct gmfFace
{
	uint32 posIdx[3];		// 位置索引
	uint32 nrmIdx[3];		// 法线索引
	uint32 texIdx[3];		// 纹理坐标索引
};
/**
 @brief gmf面定义-16bit
 @remark 
 */
struct gmfFace_16
{
	uint16 posIdx[3];		// 位置索引
	uint16 nrmIdx[3];		// 法线索引
	uint16 texIdx[3];		// 纹理坐标索引
};

/**
 @brief gmf面定义
 @remark 
 */
struct gmfFace_t2
{
	uint32 posIdx[3];		// 位置索引
	uint32 nrmIdx[3];		// 法线索引
	uint32 texIdx[3];		// 纹理坐标索引
	uint32 tex1Idx[3];		// 纹理坐标索引
};
/**
 @brief gmf面定义-16bit
 @remark 
 */
struct gmfFace_t2_16
{
	uint16 posIdx[3];		// 位置索引
	uint16 nrmIdx[3];		// 法线索引
	uint16 texIdx[3];		// 纹理坐标索引
	uint16 tex1Idx[3];		// 纹理坐标索引
};

/**
 @brief gmf 子几何体定义
 @remark 
 */
struct gmfSubset
{
	uint32 faceStart;
	uint32 faceCount;
};

typedef std::vector<Vec3> Vec3Array;
typedef std::vector<Vec2> Vec2Array;
typedef std::vector<gmfFace_t2> FaceArray;
typedef std::vector<gmfSubset> SubsetArray;


/**
 @brief MESH工具函数：计算AABB
 @return 
 @param void * vertexbuffer
 @param size_t vertexsize
 @param void * posstart
 @param uint32 vertexcount
 @param AABB & aabb
 @remark 
*/
inline void gkMeshUtilComuputeBoundingBox( void* vertexbuffer, size_t vertexsize, void* posstart, uint32 vertexcount, AABB& aabb )
{
	aabb.Reset();

	for (uint32 i=0; i < vertexcount; ++i)
	{
		Vec3* pThisVert = (Vec3*)((uint8*)vertexbuffer + i*vertexsize);

		aabb.Add( *pThisVert );
	}
}
/**
 @brief VertexBuffer结构
 @remark 
 */
struct gkVertexBuffer
{
	uint8* data;
	uint32 elementSize;
	uint32 elementCount;
	EBufferType bufferType;
	EVertexInputType vertexType;
	bool m_needRebind;
	uint32 userData;

	gkVertexBuffer(uint32 pelementSize, uint32 pelementCount, EVertexInputType type, EBufferType inBufferType)
	{
		memset(this, 0, sizeof(gkVertexBuffer));
		elementSize = pelementSize;
		elementCount = pelementCount;
		vertexType = type;
		bufferType = inBufferType;

		data = (uint8*)(malloc( elementSize * elementCount ));

		m_needRebind = false;
	}

	~gkVertexBuffer()
	{
		ummarkWrite();
	}

	
	bool needRebind() {return m_needRebind;}
	void setBind() {m_needRebind = false;}

	uint32 getSize()
	{
		return elementCount * elementSize;
	}

	void setData( void* srcData, uint32 index )
	{
		if (index < elementCount)
		{
			memcpy( data + elementSize * index, srcData, elementSize);
		}
	}

	void markWrite()
	{
		if ( !data)
		{
			data = (uint8*)(malloc( elementSize * elementCount ));
		}
	}

	void ummarkWrite()
	{
		if ( data )
		{
			free( data );
			data = 0;
		}
	}

	void resizeDiscard(uint32 pelementSize)
	{
		//memset(this, 0, sizeof(gkVertexBuffer));
		elementCount = pelementSize;

		if (data)
		{
			free( data );
		}
		data = (uint8*)(malloc( elementSize * elementCount ));

		m_needRebind = true;
	}

private:
	gkVertexBuffer(const gkVertexBuffer& other);
	gkVertexBuffer& operator = (const gkVertexBuffer& other);
};
/**
 @brief IndexBuffer结构
 @remark 
 */
struct gkIndexBuffer
{
	uint8* data;
	uint32	count;
	uint32  currAddress;
	bool m_needRebind;

	uint32 userData;
	bool wordbit;

	gkIndexBuffer(uint32 pcount, bool bit16 = false)
	{
		data = 0;
		count = pcount;
		currAddress = 0;
		userData = 0;

		if (bit16)
		{
			data = new uint8[count * 2];
		}
		else
		{
			data = new uint8[count * 4];
		}
		
		wordbit = bit16;

		m_needRebind = false;
	}

	~gkIndexBuffer()
	{
		delete[] data;
	}

	bool needRebind() {return m_needRebind;}
	void setBind() {m_needRebind = false;}


	void markWrite()
	{
		if ( !data)
		{
			if (wordbit)
			{
				data = new uint8[count * 2];
			}
			else
			{
				data = new uint8[count * 4];
			}
		}
	}

	void ummarkWrite()
	{
		if ( data )
		{
			delete[] data;
			data = 0;
		}
	}

	uint32 getSize()
	{
		return count * ( wordbit ? sizeof(uint16) : sizeof(uint32));
	}

	void resizeDiscard(uint32 pelementSize)
	{
		count = pelementSize;
		currAddress = 0;
		//userData = 0;

		if (data)
		{
			delete[] data;
		}

		if (wordbit)
		{
			data = new uint8[count * 2];
		}
		else
		{
			data = new uint8[count * 4];
		}

		m_needRebind = true;
	}

	void Clear()
	{
		memset(data, 0, getSize());
		currAddress = 0;
	}

	uint32 getData(uint32 count)
	{
		if (wordbit)
		{
			uint16* data16 = (uint16*)data;
			return data16[count];
		}
		else
		{
			uint32* data32 = (uint32*)data;
			return data32[count];
		}
	}

	void push_back(uint32 index)
	{
		if (wordbit)
		{
			uint16* data16 = (uint16*)data;
			data16[currAddress++] = index;
		}
		else
		{
			uint32* data32 = (uint32*)data;
			data32[currAddress++] = index;
		}
		
		GK_ASSERT( currAddress <= count );
	}

	uint32 size()
	{
		return currAddress;
	}

private:
	gkIndexBuffer(const gkIndexBuffer& other);
	gkIndexBuffer& operator = (const gkIndexBuffer& other);
};
/**
 @brief 子几何体结构说
 @remark 子几何体，承载于MESH下的子几何体，类似于3dsmax中face id的概念
		 一个子几何体对应一个子材质，用于实现一个mesh上的多重材质效果
 */
struct gkMeshSubset
{
	gkVertexBuffer*		vb;			///< vertex buffer
	gkIndexBuffer*		ib;			///< index buffer
	uint32				indexStart;
	uint32				indexCount;
	gkStdString			mtlname;

	gkMeshSubset()
	{
		vb = 0;
		ib = 0;
		indexStart = 0;
		indexCount = 0;
	}
};
typedef std::vector<gkMeshSubset> gkMeshSubsets;

/**
 @brief MeshLoader基类
 @remark 加载模型的基类，gkENGINE需要支持多种不可合并的模型类型
		 \n目前有gmf,hkx两种，前一种由自己实现的加载器载入，后者用havok提供的api的开发。
		 \n而对于mesh资源来说，需要根据文件类型来多态调用加载器加载
 */
struct IMeshLoader
{
	struct CacheEntry
	{
		uint32 index;
		CacheEntry* pNext;
	};

	struct ObjFace
	{
		uint32 posIdx[3];		// 位置索引
		uint32 nrmIdx[3];		// 法线索引
		uint32 texIdx[3];		// 纹理坐标索引
		uint32 tex1Idx[3];		// 纹理坐标索引
		uint32 tgtIdx[3];		// 切线索引
		uint32 bnmIdx[3];		// 副法线索引
	};

	struct Subset
	{
		gkStdString mtlname;
		uint32 startIdx;
		uint32 endIdx;
	};

	/**
	 @brief 加载器加载纹理接口
	 @return 
	 @param const TCHAR * filename
	 @remark 
	*/
	virtual bool LoadGeometry( const TCHAR* filename ) =0;
	/**
	 @brief 从加载器取出VB
	 @return 
	 @remark 
	*/
	virtual gkVertexBuffer* getVB() =0;
	/**
	 @brief 从加载器取出IB
	 @return 
	 @remark 
	*/
	virtual gkIndexBuffer* getIB() =0;
	/**
	 @brief 从加载器取出subset
	 @return 
	 @remark 
	*/
	virtual uint32 getSubsetCount() =0;
	virtual void getSubset(uint32 index, uint32& start, uint32& count, gkStdString& mtlname) =0;
	/**
	 @brief 加载工作清理
	 @return 
	 @remark 
	*/
	virtual void FinishLoading() =0;
	
	// FIXME, dangerous that pass STL data between 2 dll module
	virtual uint32 GetBoneBaseTransformCount() =0;
	virtual void GetBoneBaseTransform(Matrix44& matrix, uint32 index) =0;
};

/**
 @ingroup ResourceAPI
 @brief 抽象的模型对象
 @remark 
 */
class UNIQUE_IFACE IMesh : public IResource
{
public:
	/**
	 @brief 骨骼的基础转换矩阵
	 @remark 
	*/
	typedef std::vector<Matrix44A> BoneBaseTransforms;

	IMesh(IResourceManager* creator, const gkStdString& name, gkResourceHandle handle,
		const gkStdString& group = _T("none")):IResource(eRT_Mesh, creator, name, handle, group) {}
	virtual ~IMesh(void) {}

	/**
	 @brief DX9设备重设接口
	 @return 
	 @remark 
	*/
	virtual void onReset() =0;
	/**
	 @brief DX9设备重设接口
	 @return 
	 @remark 
	*/
	virtual void onLost() =0;

	/**
	 @brief 对于蒙皮模型，缺德BONEBASE
	 @return 
	 @remark 
	*/
	virtual BoneBaseTransforms& GetBoneBaseTransforms() =0;
	/**
	 @brief 获取模型的AABB
	 @return 
	 @remark 
	*/
	virtual AABB& GetAABB() =0;

	/**
	 @brief 取得指定子几何体的RenderOperation
	 @return 
	 @param gkRenderOperation & op
	 @param uint32 subset
	 @remark RenderOperation是渲染驱动的核心，其内包含了VB,IB,渲染方式，渲染长度等一切渲染属性。
	*/
	virtual void getRenderOperation( gkRenderOperation& op, uint32 subset = 0 ) =0;
	/**
	 @brief 向模型射线求交/模型空间
	 @return 
	 @param Ray & ray
	 @param std::vector<float> & dists
	 @remark 该函数需要对模型的所有面进行一次全遍历运算，很慢，慎用
	*/
	virtual bool RaycastMesh(Ray& ray, std::vector<float>& dists) =0;
	/**
	 @brief 向模型射线求交/世界空间
	 @return 
	 @param Ray & ray
	 @param std::vector<float> & dists
	 @remark 该函数需要对模型的所有面进行一次全遍历运算，很慢，慎用
	*/
	virtual bool RaycastMesh_WorldSpace(Ray& ray, Matrix34& world_mat, std::vector<float>& dists) =0;
	/**
	 @brief 取得子几何体的个数
	 @return 
	 @remark 
	*/
	virtual uint32 getSubsetCount() =0;

	/**
	 @brief 取得VB
	 @return 
	 @remark 一般用于其它模块对模型进行分析，修改用
	*/
	virtual gkVertexBuffer* getVertexBuffer() =0;
	/**
	 @brief 取得IB
	 @return 
	 @remark 一般用于其它模块对模型进行分析，修改用
	*/
	virtual gkIndexBuffer* getIndexBuffer() =0;

	/**
	 @brief 将VB.IB重新绑定到设备资源
	 @return 
	 @remark 
	*/
	virtual void UpdateHwBuffer() =0;
	/**
	 @brief 显式的释放VB,IB以降低内存占用
	 @return 
	 @remark 对于不需要射线求交的大型模型，地形（由高度图负责求交），以降低内存消耗
	*/
	virtual void ReleaseSysBuffer() =0;
	/**
	 @brief 修改其子几何体的个数
	 @return 
	 @param uint32 count
	 @remark 用于屏蔽渲染，慎用，可能会取消此接口
	*/
	virtual void modifyCustomPrimitiveCount(uint32 count) =0;
};

class UNIQUE_IFACE gkMeshPtr : public SharedPtr<IMesh> 
{
public:
	gkMeshPtr() : SharedPtr<IMesh>() {}
	explicit gkMeshPtr(IMesh* rep) : SharedPtr<IMesh>(rep) {}
	gkMeshPtr(const gkMeshPtr& r) : SharedPtr<IMesh>(r) {} 
	gkMeshPtr(const gkResourcePtr& r) : SharedPtr<IMesh>()
	{
		{
			pRep = static_cast<IMesh*>(r.getPointer());
			pUseCount = r.useCountPointer();
			if (pUseCount)
			{
				++(*pUseCount);
			}
		}
	}

	gkMeshPtr& operator=(const gkResourcePtr& r)
	{
		if (pRep == static_cast<IMesh*>(r.getPointer()))
			return *this;
		release();
		{
			pRep = static_cast<IMesh*>(r.getPointer());
			pUseCount = r.useCountPointer();
			if (pUseCount)
			{
				++(*pUseCount);
			}
		}
		return *this;
	}
};

#endif // gkMesh_h_e1f4f1d7_0bed_4a10_b46e_15dde925b4a0
