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
// Name:   	gkMovieMovableTextBase.h
// Desc:	vtx文字渲染基类
//
// 描述:	
// 
// Author:  Kaiming-Desktop
// Date:	2011/04/17
// Modify:	2011/4/17
// 
//////////////////////////////////////////////////////////////////////////
#pragma once

#include "UIPrerequisites.h"

#include "gkMovieMovableInstanceBase.h"
#include "vtxAtlasPacker.h"
#include "vtxGlyphStrip.h"

namespace vtx
{
		class GAMEKNIFEUI_API gkMovableTextBase : public gkMovableInstanceBase
		{
		public:
			gkMovableTextBase(DisplayObject* display_object);
			virtual ~gkMovableTextBase();

		protected:
			// TODO: remove vtx::File dependency somehow (by using FontResource* instead of a string in GlyphStrips ???)
			void _updateVertexBuffer(const GlyphStripList& glyph_strips, const AtlasPacker::PackResultList& atlas_list, File* file);
		};
}