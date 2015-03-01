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
// Name:   	gkDecalsManager.h
// Desc:	贴花管理器
//
// 描述:	
// 
// Author:  Kaiming-Desktop
// Date:	2011/04/01
// Modify:	2011/4/1
// 
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "gkPrerequisites.h"
#include "gkDecal.h"

class GAMEKNIFECORE_API gkDecalsManager
{
	//typedef std::vector<gkDeferredDecalUnit> DeferredDecalUnitList;
	typedef std::vector<gkDeferredDecalUnit> DeferredDecalUnitList;

public:
	gkDecalsManager();
	virtual ~gkDecalsManager();
	// 取得一共需要进行的pass数量
	int getDecalUnitNum();
	gkDeferredDecalUnit& getDecalUnit(int index);

	void AddDecal(const gkStdString& textureName, Vec3 vDir, Vec3 vCenter, float fRadius = 5, float fLength = 100000);

	// 取得指定DecalTextureName的
 	const gkStdString& getDecalTextureName(int unitIndx);
	int* getDeferredDecalTextureDivide();
 	Vec4* getDeferredDecalDataCache();
	int getDeferredDecalNum();

protected:
	DeferredDecalUnitList m_vecDeferredDecals;
	std::vector<Vec4> m_vecDefferredDecalDataCache;
	int m_aTexDivide[3];

	bool m_bCacheChanged;
};
