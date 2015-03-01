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
// Name:   	gkFontRenderable.h
// Desc:	
// 	
// 
// Author:  YiKaiming
// Date:	2013/6/29
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _gkFontRenderable_h__
#define _gkFontRenderable_h__

#include "gkPlatform.h"
#include "gkRenderable.h"
#include "gk_Geo.h"

#include "IMaterial.h"
#include "IMesh.h"

struct gkFTCharInfo
{
	uint32 tex_index;
	Vec4 texcoordInfo;
	int32 advanceInfo;
	int32 width;
	int32 height;
	int32 bearingX;
	int32 bearingY;
};
typedef std::map<wchar_t, gkFTCharInfo> gkFTCharMap;

class gkFontRenderable : public gkRenderable
{
	friend class gkFTFont;

public:
	gkFontRenderable(void);
	~gkFontRenderable(void);

	virtual void setMaterialName( const gkStdString& matName );

	virtual const gkStdString& getMaterialName() const;

	virtual bool getSkinnedMatrix( Matrix44A** pMatrixs, uint32& size );

	virtual float getSquaredViewDepth( const ICamera* cam ) const;

	virtual gkMeshPtr& getMesh();

	virtual void setParent( IGameObjectRenderLayer* pParent );

	virtual bool castShadow();

	virtual void enableShadow( bool enable );

	virtual void RT_Prepare();

	virtual void RP_Prepare();

	virtual void getWorldTransforms( Matrix44* ) const;

	virtual IMaterial* getMaterial();

	virtual AABB& getAABB();

	virtual void getRenderOperation( gkRenderOperation& op );

	void resizeHwBuffer();

	void flush();

	void addCharacter(gkFTCharInfo& info, const Vec3i& tmpPos, const ColorB& color);

	gkTexturePtr m_texture;
	gkMaterialPtr m_material;

	// 设置多个depth_order的mesh，来处理交叠 [12/9/2014 gameKnife]
	gkMeshPtr m_mesh;

	AABB m_aabb;

	int m_vbCharacterCacheSize;
	int m_CharacterProcessedLastFrame;
	int m_CharacterProcessedThisFrame;

};


#endif // _gkFontRenderable_h__

