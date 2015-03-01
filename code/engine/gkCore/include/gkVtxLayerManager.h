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
// Name:   	gkVtxLayerManager.h
// Desc:	vektrix在gameapp中的管理模块
//
// 描述:	
// 
// Author:  Kaiming-Desktop
// Date:	2011/04/12
// Modify:	2011/4/12
// 
//////////////////////////////////////////////////////////////////////////

#include "gkPrerequisites.h"
//#include "gkVtxMovieListener.h"

#if 0
//#ifndef WIN64

namespace vtx{
	class Root;
	class gkMovableMovie;
	class Movie;
}

class gkVtxMovieListener;

class GAMEKNIFECORE_API gkVtxLayerManager
{
	typedef std::map<std::string, vtx::gkMovableMovie*> gkVtxMovieMap;
public:
	gkVtxLayerManager();
	~gkVtxLayerManager();

	void initializeVtxSystem();
	void OnFrameMove(float fElapsedTime, POINT ptMousePos, bool isMouseLeft);

	gkSceneNode* createMovie(const std::string& name, const std::string& filename, const std::string& factoryname, gkVtxMovieListener* pMovieListener = NULL);
	void destroyMovie(const std::string& name);
	gkSceneNode* getMovie(const std::string& name);

	void setEnable(bool bEnable) {m_bEnable = bEnable;}
	bool getEnable() {return m_bEnable;}

	void setVerticalLean(float fAmount, float fDamping = 0.1f);
	void setHorizonLean(float fAmount, float fDamping = 0.1f);
	void setScreenWeker(float fDuration, float fFreq = 10.0f, float fAmount = 1.0f);

private:
	// vtx stuff
	vtx::Root* m_pVtxRoot; 
	gkCamera* m_pVtxCamera;
	bool m_bEnable;

	gkVtxMovieMap m_mapVtxMovies;

	// 垂直和水平倾斜角度
	float m_fVerticalLeanAmount;
	float m_fHorizonLeanAmount;
	// 倾斜速度
	float m_fVerticalDamping;
	float m_fHorizonDamping;

	// 目标垂直和水平倾斜角度
	float m_fDstVerticalLeanAmount;
	float m_fDstHorizonLeanAmount;

	// 屏幕震动计时器
	float m_fWekerTimer;
	float m_fWekerAmount;
	float m_fWekerFerq;
};

#endif