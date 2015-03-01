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
// Name:   	gkCommonStructs.h
// Desc:		
// 
// Author:  Kaiming-Desktop
// Date:	2011 /8/9
// Modify:	2011 /8/9
// 
//////////////////////////////////////////////////////////////////////////
#ifndef GKCOMMONSTRUCTS_H_
#define GKCOMMONSTRUCTS_H_
#pragma once

#include <utility>
#include <sstream>

typedef std::map<gkStdString, gkStdString> gkNameValuePairList;

typedef const char* GKHANDLE;

/// Utility class to generate a sequentially numbered series of names
class UNIQUE_IFACE gkNameGenerator
{
protected:
	gkStdString mPrefix;
	unsigned long long int mNext;
public:
	gkNameGenerator(const gkNameGenerator& rhs)
		: mPrefix(rhs.mPrefix), mNext(rhs.mNext) {}

	gkNameGenerator(const gkStdString& prefix) : mPrefix(prefix), mNext(1) {}

	/// Generate a new name
	gkStdString generate()
	{
		gkStdOStringstream s;
		s << mPrefix << mNext++;
		return s.str();
	}

	/// Reset the internal counter
	void reset()
	{
		mNext = NULL;
	}

	/// Manually set the internal counter (use caution)
	void setNext(unsigned long long int val)
	{
		mNext = val;
	}

	/// Get the internal counter
	unsigned long long int getNext() const
	{
		return mNext;
	}
};

enum gkTransformSpace
{
	/// Transform is relative to the local space
	TS_LOCAL,
	/// Transform is relative to the space of the parent node
	TS_PARENT,
	/// Transform is relative to world space
	TS_WORLD
};

enum gkRenderLayerID
{
	/// Use this queue for objects which must be rendered first e.g. backgrounds
	RENDER_LAYER_BACKGROUND = 0,
	/// First queue (after backgrounds), used for skyboxes if rendered first
	RENDER_LAYER_SKIES_EARLY = 5,
	RENDER_LAYER_1 = 10,
	RENDER_LAYER_2 = 20,
	RENDER_LAYER_WORLD_GEOMETRY_1 = 25,
	RENDER_LAYER_3 = 30,
	RENDER_LAYER_4 = 40,
	/// The default render queue
	RENDER_LAYER_OPAQUE = 50,
	RENDER_LAYER_TERRIAN = 60,
	RENDER_LAYER_DECAL = 65,
	RENDER_LAYER_WATER = 70,
	RENDER_LAYER_TRANSPARENT = 71,
	RENDER_LAYER_AFTERHDR = 80,
	RENDER_LAYER_AFTERDOF = 82,
	/// Penultimate queue(before overlays), used for skyboxes if rendered last
	RENDER_LAYER_AFTERPOSTPROCESS = 85,
	RENDER_LAYER_CUSTOMRT0 = 90,
	/// Use this queue for objects which must be rendered last e.g. overlays
	RENDER_LAYER_HUDUI = 100, 
	/// Final possible render queue, don't exceed this
	RENDER_LAYER_CUSTOM_SHADOW_CASCADE0 = 101,
	RENDER_LAYER_CUSTOM_SHADOW_CASCADE1 = 102,
	RENDER_LAYER_CUSTOM_SHADOW_CASCADE2 = 103,

	RENDER_LAYER_SSRL_OBJECTS = 104,

	RENDER_LAYER_STEREO_DEVICE = 105,

	RENDER_LAYER_CUSTOM_REFL = 106,

	RENDER_LAYER_MAX,
};



#endif //GKCOMMONSTRUCTS_H_