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
// Name:   	gkRenderable.h
// Desc:	Render Interface, Provide all operation that 
//			every renderable object should use.
//
// 描述:	渲染接口，提供了所有渲染类需要使用的操作
// 
// Author:  Kaiming-Desktop	 
// Date:	9/10/2010 	
// 
//////////////////////////////////////////////////////////////////////////

#ifndef gkRenderable_h_6706c7ee_f332_4b59_8af6_6e573ea820d1
#define gkRenderable_h_6706c7ee_f332_4b59_8af6_6e573ea820d1

#include "gkSharedPtr.h"
#include "IMaterial.h"

struct ICamera;
struct AABB;
struct gkRenderOperation;
class gkMeshPtr;
struct IGameObjectRenderLayer;
class IMaterial;

struct UNIQUE_IFACE gkRenderable {

    virtual void getWorldTransforms(Matrix44*) const = 0;
    virtual IMaterial* getMaterial()= 0;
	virtual void setMaterialName( const gkStdString& matName ) =0;
	virtual const gkStdString& getMaterialName() const =0;
	virtual AABB& getAABB() =0;
    virtual void getRenderOperation(gkRenderOperation& op)  = 0;
	virtual bool getSkinnedMatrix(Matrix44A** pMatrixs, uint32& size) =0;
	virtual float getSquaredViewDepth(const ICamera* cam) const = 0;
	virtual float getMaskColor() const {return 0.0f;}
	virtual gkMeshPtr& getMesh() =0;
	virtual void setParent( IGameObjectRenderLayer* pParent ) =0;
	virtual bool castShadow() =0;
	virtual void enableShadow(bool enable) =0;
	virtual void RP_ShaderSet() {}
	virtual void RP_Prepare() {}

	virtual void RT_Prepare() =0;

    // virtual destructor for interface 
    virtual ~gkRenderable() { }
};

typedef SharedPtr<gkRenderable> gkRenderablePtr;

struct gkRenderableList
{
	mutable std::list<gkRenderable*> m_vecRenderable;
	mutable std::list<gkRenderable*> m_vecRenderable_Skinned;

	gkRenderableList(){/*m_vecRenderable.clear();*/}
	~gkRenderableList(){/*m_vecRenderable.clear();*/}

	void sortByDepth() const;
};


inline bool RenderableDepthCompare(gkRenderable   *lhs,   gkRenderable   *rhs)
{
	return false;
} 
//-----------------------------------------------------------------------
inline void gkRenderableList::sortByDepth() const
{
	m_vecRenderable.sort(RenderableDepthCompare);
}


//////////////////////////////////////////////////////////////////////////
// interface of 3D Engine

// struct IStaticObjRenderable*


//////////////////////////////////////////////////////////////////////////

// struct ISkinnedRenderable*





#endif // gkRenderable_h_6706c7ee_f332_4b59_8af6_6e573ea820d1
