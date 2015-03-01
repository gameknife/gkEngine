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
// Name:   	gkVtxTexture.h
// Desc:	
//
// 描述:	
// 
// Author:  Kaiming-Desktop
// Date:	2011/03/05
// Modify:	2011/3/5
// 
//////////////////////////////////////////////////////////////////////////
#pragma once
#include "UIPrerequisites.h"

#include "vtxTexture.h"
#include "vtxMovie.h"
#include "gkTexture.h"
#include "gkMaterial.h"

namespace vtx{
	class GAMEKNIFEUI_API gkVtxTexture : public Texture
	{
	public:
		gkVtxTexture(const uint& width, const uint& height);
		virtual ~gkVtxTexture();

		void paintPixelsToRect(const Rect& coordinates, unsigned char* pixelData);

		const gkMaterialPtr& getMaterial() const;

	protected:
		gkTexturePtr m_pTexture;
		gkMaterialPtr m_pMaterial;
	};

	//-----------------------------------------------------------------------
	FactoryImpl_P2(gkVtxTexture, Texture, const uint&, const uint&);
	//-----------------------------------------------------------------------
}
