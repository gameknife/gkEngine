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
// yikaiming (C) 2014
// gkENGINE Source File 
//
// Name:   	gkAnimationHavok.h
// Desc:	
// 	
// 
// Author:  gameKnife
// Date:	2014/1/17
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _gkAnimationHavok_h_
#define _gkAnimationHavok_h_

#include "prerequistes.h"
#include "AnimationManager.h"
#include "RigManager.h"

struct IGameObjectAnimLayer;
struct IMeshLoader;
class hkLoader;
class gkCharacterInstance;

class gkAnimationHavok : public IAnimation
{
public:
	gkAnimationHavok(void);
	~gkAnimationHavok(void);

	virtual void InitAnimation();

	virtual void DestroyAnimation();

	virtual IGameObjectAnimLayer* CreateAnimLayer(const gkStdString& name);

	virtual void DestroyAnimLayer(const gkStdString& name);

	virtual void DestroyAllAnimLayer();

	virtual IMeshLoader* getHKXMeshLoader();

	virtual void _updateAnimation(float fElapsedTime);

	// loader access
	hkLoader*			getGlobalLoader() {return m_havokLoader;}
	gkAnimationManager& getAnimationManager() {return m_animationManager;}
	gkRigManager&		getRigManager() {return m_rigManager;}

private:

	//////////////////////////////////////////////////////////////////////////
	// memory and threading objects
	hkMemoryRouter* memoryRouter;

	class hkJobThreadPool* threadPool;
	class hkJobQueue* jobQueue ;

	// loader
	hkLoader*			m_havokLoader;
	// animation assets managers
	gkAnimationManager	m_animationManager;
	gkRigManager		m_rigManager;

	typedef std::map<gkStdString, gkCharacterInstance*> AnimLayerMap;
	AnimLayerMap m_mapAnimLayers;
};

#endif

