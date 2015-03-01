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
// Name:   	IRenderSequence.h
// Desc:	渲染序列结构
// 	
// Author:  Kaiming
// Date:	2012/4/8
// Modify:	2012/4/8
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _IRenderSequence_h_
#define _IRenderSequence_h_

struct gkRenderable;
struct gkRenderLight;
struct ICamera;

enum ERENDERABLE_FILLMODE
{
	eRFMode_General,
	eRFMode_ShadowCas0,
	eRFMode_ShadowCas1,
	eRFMode_ShadowCas2,
};

struct UNIQUE_IFACE IRenderSequence
{
	virtual void addToRenderSequence(gkRenderable* pRend, BYTE yId) =0;
	virtual void addToRenderSequence(gkRenderable* pRend) =0;
	virtual void addRenderLight(gkRenderLight& pLight) =0;

	virtual	void setCamera(ICamera* pCam, BYTE yMode)=0;
	virtual void setRenderableFillMode(BYTE yMode)=0;

	virtual void markFinished()=0;

	virtual void clear()=0;
};

#endif
