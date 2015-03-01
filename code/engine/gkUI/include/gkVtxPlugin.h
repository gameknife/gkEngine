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
// Name:   	gkVtxPlugin.h
// Desc:	
//
// 描述:	
// 
// Author:  Kaiming-Desktop
// Date:	2011/04/08
// Modify:	2011/4/8
// 
//////////////////////////////////////////////////////////////////////////
#pragma once

#include "UIPrerequisites.h"
#include "vtxPlugin.h"

#ifdef VTX_STATIC_LIB
void vektrix_gkVtxPlugin_startPlugin();
#else
extern "C" void GAMEKNIFEUI_API startPlugin() throw();
#endif

namespace vtx
{
	class gkVtxPlugin : public Plugin
	{
		public:
			gkVtxPlugin();
			virtual ~gkVtxPlugin();

		protected:
			InstanceManager* instMgr;

			// MovableMovie
			gkMovableMovieFactory* mMovableMovie;
			gkMovableShapeFactory* mMovableShape;

			gkMovableEditTextFactory* mMovableEditText;
			gkMovableStaticTextFactory* mMovableStaticText;

			// Generic
			gkVtxTextureFactory* mTexture;

			void initMovableMovie();
			void freeMovableMovie();

			void initTextureMovie();
			void freeTextureMovie();
	};
}