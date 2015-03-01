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
// Name:   	gkPostProcessManager.h
// Desc:	后期特效处理管理器
//			移植自GameKnife v1.0，以后考虑做一些改动
// 描述:	
// 
// Author:  Kaiming-Desktop	 
// Date:	12/26/2010 	
// 
//////////////////////////////////////////////////////////////////////////
#pragma once
#include "RendererD3D9Prerequisites.h"
#include "RenderRes\gkTexture.h"
#include "RenderRes\gkTextureManager.h"

//--------------------------------------------------------------------------------------
// This is the vertex format used with the quad during post-process.
// POSTPROCESS 用的顶点声明 | rhw坐标系，不参加VS
struct PPVERT
{
	Vec4 post;
	Vec4 texcoord0;       // Texcoord for post-process source
	Vec4 texcoord1;       // Texcoord for post-process source

	const static D3DVERTEXELEMENT9 Decl[4];
};

class gkRendererD3D9;

// 先不用单件了，大家都从root里面拿吧
class gkPostProcessManager
{
private:
	static IDirect3DVertexDeclaration9*    m_pVertDeclPP;				// Vertex decl for post-processing
public:
	// 与设备相关函数，后期处理所用的tex, shader都自己接管了，不找manager生产了
	static HRESULT OnCreateDevice(IDirect3DDevice9* pd3dDevice);
	static void OnDestroyDevice();

	static void DrawFullScreenQuad(int nTexWidth, int nTexHeight, Vec4& region = Vec4(0,0,1,1), Vec2& repeat = Vec2(1,1) );
	static void DrawFullScreenQuad(gkTexturePtr targetTex, Vec4& region = Vec4(0,0,1,1), Vec2& repeat = Vec2(1,1) );
	static void DrawScreenQuad( Vec4& region );
};