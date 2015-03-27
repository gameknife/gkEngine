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
// Name:   	gkRenderLayer.h
// Desc:	渲染层，由固定的枚举确定，分出UI，场景，精灵等
//			层级渲染的概念，其下应该还有一层RenderPriority层，
//			实现对同一层级的透明物体排序，非透明物体按shader分组
//			由于DEMO版需要快速实现，因此暂时先不实现了
//			version 1.0.0 by yikaiming 2010/9/27
// 描述:	
// 
// Author:  Kaiming-Laptop
// Date:	2010/9/10
// 
//////////////////////////////////////////////////////////////////////////

#ifndef gkRenderLayer_h_bc6cea8f_1a6e_4af8_910f_1aa55badfba9
#define gkRenderLayer_h_bc6cea8f_1a6e_4af8_910f_1aa55badfba9

#include "Prerequisites.h"
#include "gkRenderable.h"
#include <vector>
#include "gkIterator.h"

//////////////////////////////////////////////////////////////////////////
// in every shader, group by material, so Apply MaterialBlock can be called just one time

struct gkShaderGroup
{
	gkShaderGroup(IShader* pShader):m_pShader(pShader) {}
	~gkShaderGroup() {}
	typedef std::map<IMaterial*, gkRenderableList> gkMaterialGroupRenderableMap;
	typedef MapIterator<gkMaterialGroupRenderableMap> MaterialGroupMapIterator;
	typedef ConstMapIterator<gkMaterialGroupRenderableMap> ConstMaterialGroupMapIterator;

	virtual void addRenderable(gkRenderable* pRend, IMaterial* pMaterial);
	virtual void clear();

	gkMaterialGroupRenderableMap m_opaqueMaterialMap;
	IShader* m_pShader;
};

class gkRenderLayer {
public:
	typedef std::map<IShader*, gkShaderGroup> gkShaderGroupRenderableMap;
	typedef MapIterator<gkShaderGroupRenderableMap> ShaderGroupMapIterator;
	typedef ConstMapIterator<gkShaderGroupRenderableMap> ConstShaderGroupMapIterator;

public:
	gkRenderLayer(void);
	virtual ~gkRenderLayer(void);
	virtual void addRenderable(gkRenderable* pRend, IShader* pShader);
	virtual void clear();

	// 取得shader组的迭代器
	ShaderGroupMapIterator			_getShaderGroupIterator(void);
	ConstShaderGroupMapIterator		_getShaderGroupIterator(void) const;

protected:
	gkShaderGroupRenderableMap m_mapShaderGroupedMap;


};

#endif // gkRenderLayer_h_bc6cea8f_1a6e_4af8_910f_1aa55badfba9
