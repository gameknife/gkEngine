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
// Name:   	gkRenderSequence.h
// Desc:	todo...
// 描述:	
// 
// Author:  Kaiming-Laptop
// Date:	2010/9/10
// 
//////////////////////////////////////////////////////////////////////////

#ifndef gkRenderSequence_h_00a391ee_b7b5_4392_a65b_8821db1486ab
#define gkRenderSequence_h_00a391ee_b7b5_4392_a65b_8821db1486ab

#include "prerequisites.h"
#include "IRenderSequence.h"
#include "gkIterator.h"
#include "gkRenderable.h"
#include "gkRenderLayer.h"

class gkRenderSequence : public IRenderSequence{
public:
	typedef std::map<BYTE, gkRenderLayer> gkRenderLayerMap;
	/// Iterator over queue groups
	typedef MapIterator<gkRenderLayerMap> LayerMapIterator;
	typedef ConstMapIterator<gkRenderLayerMap> ConstLayerMapIterator;

public:
	gkRenderSequence(void);
	virtual ~gkRenderSequence(void);

	gkRenderLayer*	getRenderLayer(BYTE yId);

	virtual void addToRenderSequence(gkRenderable* pRend, BYTE yId);
	virtual void addToRenderSequence(gkRenderable* pRend);
	virtual void addRenderLight(gkRenderLight& pLight);
	virtual void setRenderableFillMode(BYTE yMode) { m_yCacheMode = yMode; }
	void setCamera(ICamera* pCam, BYTE yMode);
	virtual void clear();
	virtual void markFinished() { m_bIsReady = true; }

	LayerMapIterator _getLayerIterator(void);
	ConstLayerMapIterator _getLayerIterator(void) const;

	virtual gkRenderableList* getZprepassObjects() { return &m_lstZprepass; }
	virtual gkRenderableList* getZprepassObjectsSkinned() { return &m_lstZprepassSkinned; }

	virtual gkRenderableList* getShadowCascade0Objects() { return &m_lstShadowCascade0; }
	virtual gkRenderableList* getShadowCascade1Objects() { return &m_lstShadowCascade1; }
	virtual gkRenderableList* getShadowCascade0ObjectsSkinned() { return &m_lstShadowCascade0Skinned; }
	virtual gkRenderableList* getShadowCascade1ObjectsSkinned() { return &m_lstShadowCascade1Skinned; }
	virtual gkRenderableList* getShadowCascade2Objects() { return &m_lstShadowCascade2; }
	virtual gkRenderLightList& getRenderLightList() { return m_lstRenderLight; }

	virtual void reduceShadowCascadeList();

	ICamera* getCamera(BYTE yMode);


	void PrepareRenderSequence();

protected:
	gkRenderLayerMap m_mapRenderLayer;
	BYTE m_yDefaultLayer;

	BYTE m_yCacheMode;

	// 主渲染序列的Zprepass队列 [10/2/2011 Kaiming]
	bool m_bIsMainSequence;

	// Render to Other RT Lists [3/18/2012 Kaiming]
	gkRenderableList m_lstZprepass;
	gkRenderableList m_lstZprepassSkinned;
	gkRenderableList m_lstShadowCascade0;
	gkRenderableList m_lstShadowCascade0Skinned;
	gkRenderableList m_lstShadowCascade1;
	gkRenderableList m_lstShadowCascade1Skinned;
	gkRenderableList m_lstShadowCascade2;

	//gkLightList	m_lstLightThisFrame;
	gkRenderLightList m_lstRenderLight;

	// Render to Other RT Cams;
	ICamera*		 m_MainCam;
	ICamera*		 m_ShadowCamCascade0;
	ICamera*		 m_ShadowCamCascade1;
	ICamera*		 m_ShadowCamCascade2;

public:
	bool m_bIsReady;
};

#endif // gkRenderSequence_h_00a391ee_b7b5_4392_a65b_8821db1486ab
