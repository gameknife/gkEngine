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
// Name:   	gkSubEntity.h
// Desc:	todo...
// 描述:	
// 
// Author:  Kaiming-Laptop
// Date:	2010/9/10
// 
//////////////////////////////////////////////////////////////////////////

#ifndef gkSubEntity_h_d506a401_8ae8_4ccb_b8a9_db7a22457374
#define gkSubEntity_h_d506a401_8ae8_4ccb_b8a9_db7a22457374

#include "gkPrerequisites.h"

#include "gkRenderable.h"
#include "IMesh.h"

class gkStaticObjRenderable : virtual public gkRenderable {
	friend struct IGameObjectRenderLayer;
protected:
    IGameObjectRenderLayer* m_pCreator;

	gkStdString m_wstrMatName;
	mutable gkMaterialPtr m_pMaterial;
    gkMeshPtr m_pMesh;
	uint32 m_uSubsetID;
	bool m_castShadow;
	Matrix44 m_CachedTransform;
	bool m_matrixCaching;

public:
	gkStaticObjRenderable(void);
	gkStaticObjRenderable(gkMeshPtr& pMesh, uint32 subsetID);
	virtual ~gkStaticObjRenderable();

	void setParent( IGameObjectRenderLayer* pParent ) { m_pCreator = pParent; }
	void setMaterial( const gkMaterialPtr& material );
	virtual void setMaterialName( const gkStdString& matName );
	const gkStdString& getMaterialName() const
	{
		return m_wstrMatName;
	}

	virtual AABB& getAABB();

	// override
	// 覆盖renderable接口
	void getWorldTransforms( Matrix44* xform ) const;
	// 覆盖renderable接口
	IMaterial* getMaterial(void);
	// 覆盖renderable接口
	void getRenderOperation(gkRenderOperation& op);
	// 覆盖renderable接口
	virtual bool getSkinnedMatrix(Matrix44A** pStart, uint32& size) {return false;}
	// 覆盖renderable接口
	virtual float getSquaredViewDepth(const ICamera* cam) const ;

	virtual float getMaskColor() const;

	virtual gkMeshPtr& getMesh();

	virtual void RT_Prepare();

	virtual bool castShadow() {return m_castShadow;}

	virtual void enableShadow( bool enable ) {m_castShadow = enable;}

};

#endif // gkSubEntity_h_d506a401_8ae8_4ccb_b8a9_db7a22457374
