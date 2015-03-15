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
// Name:   	CharacterInstance.h
// Desc:		
// 	
// Author:  Kaiming
// Date:	2012/3/30
// Modify:	2012/3/30
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _CharacterInstance_h_
#define _CharacterInstance_h_

#include "IGameObjectLayer.h"

class hkaAnimatedSkeleton;
class hkaDefaultAnimationControl;
class hkaPose;

class gkCharacterInstance : public IGameObjectAnimLayer
{
public:
	gkCharacterInstance(const TCHAR* name);
	virtual ~gkCharacterInstance() {}


	//////////////////////////////////////////////////////////////////////////
	// override of IGameObjectAnimLayer
	// assets loading
	virtual bool loadChrFile(const TCHAR* filename);

	// animation control
	virtual void playAnimation(const TCHAR* animName, float blendTime);
	virtual void stopAnimation(const TCHAR* animName, float blendTime);

	virtual void setAnimationWeight(const TCHAR* animName, float weight);

	virtual void setAnimationSpeed(const TCHAR* animName, float speed);

	virtual void setAnimationLocalTime(const TCHAR* animName, float localTime  );


	virtual void stopAllAnimation(float blendTime);

	// rendering helper
	virtual float* getTransformStart() {return NULL;}
	virtual uint32 getTransformLength() {return 0;}

	virtual float getAnimationAbsTime(const TCHAR* animName);

	//////////////////////////////////////////////////////////////////////////
	// override of IGameObjectLayer
	virtual void Init();
	virtual void UpdateLayer(float fElpasedTime);
	virtual void Destroy();


	hkaPose* getPose() {return m_pPose;}


private:
	void DebugDraw();

	virtual int getAnimationCount();

	virtual const TCHAR* getAnimationName(int index);

private:
	gkStdString m_wstrName;

	hkaAnimatedSkeleton* m_pSkeleton;
	hkaPose* m_pPose;

	typedef std::map<gkStdString, hkaDefaultAnimationControl*> AnimationControlMap;
	AnimationControlMap m_mapAnimations;
};

#endif