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
// Name:   	gkRenderTexture.h
// Desc:	GameKnife渲染到纹理的RT
// 描述:	
// 
// Author:  Kaiming-Desktop	 
// Date:	12/31/2010 	
// 
//////////////////////////////////////////////////////////////////////////
#pragma once
#include "RendererD3D9Prerequisites.h"
#include "gkRenderTarget.h"
#include "RenderRes\gkTexture.h"

class gkRenderTexture : public gkRenderTarget{
public:

protected:
	gkTexturePtr		m_pTexture;
	LPDIRECT3DSURFACE9	m_pSurfForTex;
	LPDIRECT3DSURFACE9	m_pDSSurfForTex;
	BYTE				m_yRttType;
	int					m_nSkipFrame;
	UINT				m_uWidth;
	UINT				m_uHeight;

public:
	gkRenderTexture();
	gkRenderTexture(gkStdString name, BYTE rttType, UINT customW = 128, UINT customH = 128);
	virtual ~gkRenderTexture();

	virtual void onCreate(LPDIRECT3DDEVICE9 pDevice);
	virtual void onReset();
	virtual void onLost();
	virtual void onDestroy();

	// override
	virtual void updateImpl();
};
