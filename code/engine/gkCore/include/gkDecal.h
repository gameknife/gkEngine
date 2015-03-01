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
// Name:   	gkDecal.h
// Desc:	
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

class GAMEKNIFECORE_API gkDecal
{
public:
	gkDecal(Vec3 vDir, Vec3 vCenter, float fRadius = 5, float fLength = 100000 );
	virtual ~gkDecal();

	// 取得方向向量组
	Vec4 getDirectionVector();
	// 取得位置向量组
	Vec4 getCenterVector();

	// 设置方向，位置
	void setDirection(Vec3& vDir);
	void setCenter(Vec3& vCenter);

protected:
	Vec3 m_vDirection;
	Vec3 m_vCenter;

	float		m_fRadius;
	float		m_fLength;
};

class GAMEKNIFECORE_API gkDeferredDecalUnit
{
public:
	gkDeferredDecalUnit(gkStdString wstrName);
	virtual ~gkDeferredDecalUnit();

	void addDecal(gkDecal pDecal);
	Vec4* getDecalsDataHandle();
	int	getDecalNum();
	const gkStdString& getTextureName();

protected:
	gkStdString m_wstrName;
	std::vector<gkDecal> m_vecDecalList;
	std::vector<Vec4> m_vecCachedData;
};
