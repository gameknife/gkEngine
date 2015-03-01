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
// Name:   	gkMovieMovable.h
// Desc:	在gk中渲染flash对象的movable。承接vektrix的渲染结果
//
// 描述:	
// 
// Author:  Kaiming-Desktop
// Date:	2011/01/21
// Modify:	2011/1/21
// 
//////////////////////////////////////////////////////////////////////////
#pragma once
#include "UIPrerequisites.h"

#include "vtxMovie.h"
#include "vtxMovieFactory.h"
#include "gkMovableObject.h"

namespace vtx
{
	class GAMEKNIFEUI_API gkMovableMovieFactory : public MovieFactory, public Movie::Listener
	{
	public:
		gkMovableMovieFactory();
		virtual ~gkMovableMovieFactory();

		const String& getName() const;
		Movie* createObject(String name);
		void destroyObject(Movie* instance);

		AtlasPacker* getPacker() const;
		InstancePool* getInstancePool() const;

	protected:
		AtlasPacker* mPacker;
		InstancePool* mPool;

		MovieDebugger* _newDebugger(Movie* movie);

		bool loadingCompleted(Movie* movie);
	};

	class GAMEKNIFEUI_API gkMovableMovie : public Movie, public gkMovableObject
	{
	public:
		typedef std::map<gkRenderable*, gkRenderable*> RenderableMap;

		gkMovableMovie(const vtx::String& name, vtx::MovieFactory* creator);
		virtual ~gkMovableMovie();

		AtlasPacker* getPacker() const;

		virtual Instance* getInstance(Resource* resource);
		virtual Instance* getInstanceByType(const String& type);
		virtual void releaseInstance(Instance* instance);

		// override GameKnife MovableObject functions
		// override
		// 覆盖movable接口
		virtual void _updateRenderSequence(IRenderSequence* queue);
		// 覆盖movable接口
		virtual const gkStdString& getMovableType() const;

	protected:
		gkMovableMovieFactory* mFactory;
		//ShapeList mShapes;
		RenderableMap mRenderables;
	};
}
