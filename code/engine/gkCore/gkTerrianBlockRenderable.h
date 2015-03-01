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
// Name:   	gkTerrianBlockRenderable.h
// Desc:	
// 	
// 
// Author:  Kaiming
// Date:	2013/1/7	
// 
//////////////////////////////////////////////////////////////////////////

#ifndef gkTerrianBlockRenderable_h__
#define gkTerrianBlockRenderable_h__

#include "gkPrerequisites.h"

#include "gkRenderable.h"
#include "IMesh.h"

class gkTerrian;

class gkTerrianBlockRenderable : public gkRenderable, public IResourceResetCallback
{
public:
	gkTerrianBlockRenderable(gkTerrian* creator, gkTexturePtr& heightMap, const Vec4& region, int block, float blockUnit, int totalWidth, int minBlock, float zeroOffset);
	~gkTerrianBlockRenderable(void);

	void updateTerrian();
	void flushHw();


	virtual void getWorldTransforms( Matrix44* xform) const;

	virtual IMaterial* getMaterial();

	virtual void setMaterialName( const gkStdString& matName );

	virtual const gkStdString& getMaterialName() const;

	virtual AABB& getAABB();

	virtual void getRenderOperation( gkRenderOperation& op );

	virtual bool getSkinnedMatrix( Matrix44A** pMatrixs, uint32& size );

	virtual float getSquaredViewDepth( const ICamera* cam ) const;

	virtual gkMeshPtr& getMesh();

	virtual void setParent( IGameObjectRenderLayer* pParent );

	virtual bool castShadow();

	virtual void enableShadow( bool enable );

	virtual void RT_Prepare();

	virtual void OnResetCallBack();
private:

	virtual float LocalGetHeightmap(const Vec2& texcoord);

	

private:
	IGameObjectRenderLayer* m_pCreator;

	gkStdString m_wstrMatName;
	mutable gkMaterialPtr m_pMaterial;

	gkMeshPtr m_mesh;

	bool m_castShadow;
	Matrix44 m_CachedTransform;

	gkTexturePtr m_heightMap;
	Vec4 m_region;
	uint32 m_block;
	uint32 m_totalWidth;
	float m_blockUnit;
	uint32 m_minBlock;

	float m_zeroOffset;
};


#endif // gkTerrianBlockRenderable_h__



