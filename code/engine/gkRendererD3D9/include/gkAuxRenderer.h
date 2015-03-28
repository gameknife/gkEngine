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
// Name:   	gkHelperObjectManager.h
// Desc:	GameKnife测试时所用的帮助物件渲染器
//			为了方便的渲染各种测试时物件，提供一个管理器
//			需要渲染时在每一帧frameMove时调用相应的函数即可在此帧完成渲染
// 描述:	
// 
// Author:  Kaiming-Desktop	 
// Date:	12/24/2010 	
// 
//////////////////////////////////////////////////////////////////////////
#pragma once

#include "RendererD3D9Prerequisites.h"
#include "gkSingleton.h"
#include "IAuxRenderer.h"

// 轴FVF
#define GKFVF_HELPER_LINEVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE)
#define GKFVF_HELPER_2DVERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE)

struct STextRenderInfo
{
	TCHAR text[512];
	int posx;
	int posy;
	const IFtFont* font;
	uint32 alignment;
	ColorB color;
};

class gkAuxRenderer : public Singleton<gkAuxRenderer>, public IAuxRenderer
{
protected:
	std::vector<GK_HELPER_2DVERTEX> m_vecLineVertexBuffer;
	std::vector<GK_HELPER_2DVERTEX> m_vecSolidVertexBuffer;

	std::vector<GK_HELPER_2DVERTEX> m_vecOverHudLineVertexBuffer;
	std::vector<GK_HELPER_2DVERTEX> m_vecOverHudSolidVertexBuffer;

	std::vector<GK_HELPER_2DVERTEX> m_vecLineVertexBuffer_Render;
	std::vector<GK_HELPER_2DVERTEX> m_vecSolidVertexBuffer_Render;

	std::vector<GK_HELPER_2DVERTEX> m_vecOverHudLineVertexBuffer_Render;
	std::vector<GK_HELPER_2DVERTEX> m_vecOverHudSolidVertexBuffer_Render;

	std::vector<GK_HELPER_2DVERTEX> m_vecScreenBoxVertexBuffer;
	std::vector<GK_HELPER_2DVERTEX> m_vecScreenBoxVertexBuffer_Render;

	std::vector<GK_HELPER_2DVERTEX> m_vecScreenLineVertexBuffer;
	std::vector<GK_HELPER_2DVERTEX> m_vecScreenLineVertexBuffer_Render;

	std::vector<GK_HELPER_2DVERTEX_TEXTURED> m_vecScreenBoxTexturedVertexBuffer;
	std::vector<GK_HELPER_2DVERTEX_TEXTURED> m_vecScreenBoxTexturedVertexBuffer_Render;

	std::vector<ITexture*> m_vecScreenBoxTexture;
	std::vector<ITexture*> m_vecScreenBoxTexture_Render;

	std::vector<IGameObjectRenderLayer*> m_vecMeshFrameBuffer;
	std::vector<IGameObjectRenderLayer*> m_vecMeshFrameBuffer_Render;

	typedef std::vector<STextRenderInfo> TextRenderList;
	TextRenderList	m_vecTextRenderList;
	TextRenderList	m_vecTextRenderList_Render;

	DWORD m_curColor;

public:
	static gkAuxRenderer& getSingleton(void);
	static gkAuxRenderer* getSingletonPtr(void);

	gkAuxRenderer(void);
	virtual ~gkAuxRenderer(void);


	// framemove中调用，向batch中添加渲染物体

	// 线框渲染类
	virtual void AuxRender3DLine(const Vec3& from, const Vec3& to, ColorF& color = ColorF(1.0,1.0,1.0,1.0), bool ignoreZ = false );
	virtual void AuxRender3DGird( const Vec3& center, int row, float gap, ColorF& color = ColorF(1.0,1.0,1.0,1.0), bool ignoreZ = false  );

	virtual void AuxRender3DBoxFrameRotated(const Vec3& center, const Quat& rot, float length, ColorF& color = ColorF(1.0,1.0,1.0,1.0), bool ignoreZ = false);
	virtual void AuxRender3DBoxFrameRotated(const Vec3& center, const Quat& rot, const Vec3& size, ColorF& color = ColorF(1.0,1.0,1.0,1.0), bool ignoreZ = false);

	virtual void AuxRender3DBoxFrame(const Vec3& center, float length, ColorF& color = ColorF(1.0,1.0,1.0,1.0), bool ignoreZ = false);
	virtual void AuxRender3DBoxFrame(const Vec3& center, Vec3& size, ColorF& color = ColorF(1.0,1.0,1.0,1.0), bool ignoreZ = false);
	virtual void AuxRender3DCircle(const Vec3& center, const Vec3& normal, float radius, uint32 side = 24, ColorF& color = ColorF(1.0,1.0,1.0,1.0), bool ignoreZ = false);
	virtual void AuxRenderAABB(const AABB& aabb, ColorF& color = ColorF(1.0,1.0,1.0,1.0), bool ignoreZ = false);
	virtual void AuxRenderGizmo(const Matrix44& xform, float size, uint8 axisHighlight = 0, bool ignoreZ = false, uint8 type = 0);

	virtual void AuxRenderSkeleton(const Vec3& from, const Vec3& to, ColorF& color = ColorF(1.0,1.0,1.0,1.0), float radius = 0.05f, bool ignoreZ = false );

	// 实体渲染类
	virtual void AuxRender3DBoxSolid(const Vec3& center, float length, ColorF& color = ColorF(1.0,1.0,1.0,1.0), bool ignoreZ = false);
	virtual void AuxRender3DBoxSolid(const Vec3& center, Vec3& size, ColorF& color = ColorF(1.0,1.0,1.0,1.0), bool ignoreZ = false);
	virtual void AuxRenderPyramid(const Vec3& center,  const Vec3& dir, const float radius, ColorF& color = ColorF(1.0,1.0,1.0,1.0), bool ignoreZ = false);

	// Mesh Frame
	virtual void AuxRenderMeshFrame(IGameObjectRenderLayer* layer, ColorF& color = ColorF(1.0,1.0,1.0,1.0), bool ignoreZ = false);

	// Text Rendering
	virtual void AuxRenderText(const TCHAR* text, int posx, int posy, const IFtFont* font, const ColorB& color, uint32 alignment, uint32 style );

	// ScreenDraw
	virtual void AuxRenderScreenBox(const Vec2& pos, const Vec2& wh, const ColorB& color );
	virtual void AuxRenderScreenBox(const Vec2& pos, const Vec2& wh, const ColorB& color, ITexture* texture );

	virtual void AuxRenderScreenLine(const Vec2& from, const Vec2& to, const ColorB& color);

	// Renderer Flush调用
	virtual void _FlushAllHelper(LPDIRECT3DDEVICE9 pDevice,bool clean);
	virtual void _FlushAllHelperDeferred(LPDIRECT3DDEVICE9 pDevice,bool clean);
	virtual void _FlushAllText(LPDIRECT3DDEVICE9 pDevice);
	virtual void _swapBufferForRendering();
	virtual void _cleanBuffer();

protected:
	// 设置颜色
	virtual void SetDrawColor(ColorF& color);


};
