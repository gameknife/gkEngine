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
// Name:   	gkSun.h
// Desc:	GameKnife中对太阳的抽象
//
// 描述:	
// 
// Author:  Kaiming-Desktop
// Date:	2011/03/28
// Modify:	2011/3/28
// 
//////////////////////////////////////////////////////////////////////////
#pragma once
#include "gkPrerequisites.h"
#include "gkLight.h"

class gkSun : public gkLight
{
public:
	//gkSun(void);
	gkSun(gkStdString name);
	virtual ~gkSun(void);

public:
	// 绑定节点，跟随移动
	void bindSceneNode(gkSceneNode* pNode);

	// 设置太阳角度偏移，类似于从赤道到北极太阳的变化
	void setSunArc(float fArc);

	// 设置太阳方向，基本等同于扭转地形
	void setSunDir(float fDir);

	// 设置太阳时间，24小时制
	void setTime(float fTime);

	// 设置太阳时间，24小时制
	float getTime() const {return m_fTime;}

	gkCamera* getCascade1() {return m_pCascade1Cam;}
	gkCamera* getCascade2() {return m_pCascade2Cam;}

	virtual void _updateRenderSequence(IRenderSequence* queue);

	void setOffset( const Vec3& offset) {m_vOffset = offset;}

protected:
	/// override from gkLight
	virtual void update(void) const;

private:
	gkSceneNode*	m_pAttachSceneNode;
	gkSceneNode*	m_pBindSceneNode;


	gkSceneNode*	m_pAttachSceneNode1;
	gkSceneNode*	m_pAttachSceneNode2;
	float			m_fArc;
	float			m_fDir;
	float			m_fTime;
	Vec3		m_vSunVector;

	gkCamera*		m_pCascade1Cam;
	gkCamera*		m_pCascade2Cam;

	Vec3			m_vOffset;

	const int		SUN_RADIUS;
};

