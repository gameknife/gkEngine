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
// Name:   	IAuxRenerer.h
// Desc:	Aux物体 渲染接口	
// 
// Author:  Kaiming-Desktop
// Date:	2011 /8/20
// Modify:	2011 /8/20
// 
//////////////////////////////////////////////////////////////////////////
#ifndef IAUXRENDERER_H_
#define IAUXRENDERER_H_

#include "gkPlatform.h"
#include "gk_Color.h"
#include "gk_Geo.h"
#include "IFont.h"

struct ITexture;
struct IGameObject;
struct IGameObjectRenderLayer;

struct GK_HELPER_2DVERTEX
{
	Vec3 m_vPosition;
	DWORD m_dwDiffuse;

	GK_HELPER_2DVERTEX()
	{
	}

	GK_HELPER_2DVERTEX( Vec3 pos, DWORD color)
	{
		m_vPosition = pos;
		m_dwDiffuse = color;
	}
	~GK_HELPER_2DVERTEX()
	{

	}
};

struct GK_HELPER_2DVERTEX_TEXTURED
{
	Vec3 m_vPosition;
	Vec2 m_vTexcorrd;
	DWORD m_dwDiffuse;

	GK_HELPER_2DVERTEX_TEXTURED()
	{
	}

	GK_HELPER_2DVERTEX_TEXTURED( Vec3 pos, Vec2 texcoord, DWORD color)
	{
		m_vPosition = pos;
		m_vTexcorrd = texcoord;
		m_dwDiffuse = color;
	}
	~GK_HELPER_2DVERTEX_TEXTURED()
	{

	}
};

struct IAuxRenderer
{
	virtual ~IAuxRenderer(void) {}

	// 线框渲染类
	virtual void AuxRender3DLine(const Vec3& from, const Vec3& to, ColorF& color, bool ignoreZ = false ) =0;
	virtual void AuxRender3DGird( const Vec3& center, int row, float gap, ColorF& color, bool ignoreZ = false  ) =0;

	virtual void AuxRender3DBoxFrameRotated(const Vec3& center, const Quat& rot, float length, ColorF& color, bool ignoreZ = false) =0;
	virtual void AuxRender3DBoxFrameRotated(const Vec3& center, const Quat& rot, const Vec3& size, ColorF& color, bool ignoreZ = false) =0;

	virtual void AuxRender3DBoxFrame(const Vec3& center, float length, ColorF& color, bool ignoreZ = false) =0;
	virtual void AuxRender3DBoxFrame(const Vec3& center, Vec3& size, ColorF& color, bool ignoreZ = false) =0;
	virtual void AuxRender3DCircle(const Vec3& center, const Vec3& normal, float radius, uint32 side, ColorF& color, bool ignoreZ = false) =0;
	virtual void AuxRenderAABB(const AABB& aabb, ColorF& color, bool ignoreZ = false) =0;
	virtual void AuxRenderGizmo(const Matrix44& xform, float size, uint8 axisHighlight = 0, bool ignoreZ = false, uint8 type = 0) =0;

	virtual void AuxRenderSkeleton(const Vec3& from, const Vec3& to, ColorF& color, float radius = 0.05f, bool ignoreZ = false ) =0;

	// 实体渲染类
	virtual void AuxRender3DBoxSolid(const Vec3& center, float length, ColorF& color, bool ignoreZ = false) =0;
	virtual void AuxRender3DBoxSolid(const Vec3& center, Vec3& size, ColorF& color, bool ignoreZ = false) =0;
	virtual void AuxRenderPyramid(const Vec3& center,  const Vec3& dir, const float radius, ColorF& color, bool ignoreZ = false) =0;

	// Mesh Frame
	virtual void AuxRenderMeshFrame(IGameObjectRenderLayer* layer, ColorF& color, bool ignoreZ = false) =0;

	// Text Rendering
	virtual void AuxRenderText(const TCHAR* text, int posx, int posy, const IFtFont* font, const ColorB& color = ColorB(255,255,255,200), uint32 alignment = eFA_Left | eFA_Top, uint32 style = 0 ) =0;

	// ScreenDraw
	virtual void AuxRenderScreenBox(const Vec2& pos, const Vec2& wh, const ColorB& color) {}
	virtual void AuxRenderScreenBox(const Vec2& pos, const Vec2& wh, const ColorB& color, ITexture* texture ) {}

	virtual void AuxRenderScreenLine(const Vec2& from, const Vec2& to,  const ColorB& color) {}

	// sceneManager调用
	virtual void _swapBufferForRendering() =0;
	virtual void _cleanBuffer() =0;
};

#endif
