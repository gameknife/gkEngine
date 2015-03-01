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
// Name:   	gkSkinRenderable.h
// Desc:		
// 	
// Author:  Kaiming
// Date:	2012/4/12
// Modify:	2012/4/12
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _gkSkinRenderable_h_
#define _gkSkinRenderable_h_

#include "gkRenderable.h"
#include "IMesh.h"
class gkCharacterInstance;


class gkSkinRenderable : virtual public gkRenderable {
	friend struct IGameObjectRenderLayer;
	friend struct IGameObjectAnimLayer;

protected:
	IGameObjectRenderLayer* m_pCreator;
	gkCharacterInstance* m_pCharacter;

	gkStdString m_wstrMatName;
	mutable gkMaterialPtr m_pMaterial;
	gkMeshPtr m_pMesh;
	uint32 m_uSubsetID;
	  
	bool m_castShadow;
	IMesh::BoneBaseTransforms m_vecCachedTransforms;
	Matrix44 m_CachedTransforms;

public:
	gkSkinRenderable(void);
	gkSkinRenderable(IGameObjectRenderLayer* pCreator, gkCharacterInstance* pCharacter, gkMeshPtr& pMesh, uint32 subsetid);
	virtual ~gkSkinRenderable();

	virtual void setParent( IGameObjectRenderLayer* pParent ) { m_pCreator = pParent; }
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
	virtual bool getSkinnedMatrix(Matrix44A** pMatrixs, uint32& size);
	// 覆盖renderable接口
	virtual float getSquaredViewDepth(const ICamera* cam) const ;

	virtual float getMaskColor() const;

	virtual gkMeshPtr& getMesh() {return m_pMesh;}

	virtual void RT_Prepare();

	virtual bool castShadow() {return m_castShadow;}

	virtual void enableShadow( bool enable ) {m_castShadow = enable;}
};

#endif
