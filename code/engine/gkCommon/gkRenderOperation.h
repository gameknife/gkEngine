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
// Name:   	gkRenderOperation.h
// Desc:	渲染操作的抽象
// 描述:	
// 
// Author:  Kaiming-Desktop	 
// Date:	9/23/2010 	
// 
//////////////////////////////////////////////////////////////////////////

#pragma once
#include "gkPlatform.h"

struct gkRenderable;

struct IDirect3DVertexBuffer9;
struct IDirect3DVertexDeclaration9;
struct IDirect3DIndexBuffer9;
struct ID3DXMesh;

struct gkVertexBuffer;
struct gkIndexBuffer;
#if defined( RENDERER_D3D9 )
typedef IDirect3DVertexDeclaration9* gkVertexDecl;
#elif RENDERER_GLES
typedef uint8 gkVertexDecl;
#else
typedef IDirect3DVertexDeclaration9* gkVertexDecl;
#endif

struct gkRenderOperation {
public:
	/// 绘制类型枚举
	enum OperationType {
		/// A list of points, 1 vertex per point
		OT_POINT_LIST = 1,
		/// A list of lines, 2 vertices per line
		OT_LINE_LIST = 2,
		/// A strip of connected lines, 1 vertex per line plus 1 start vertex
		OT_LINE_STRIP = 3,
		/// A list of triangles, 3 vertices per triangle
		OT_TRIANGLE_LIST = 4,
		/// A strip of triangles, 3 vertices for the first triangle, and 1 per triangle after that 
		OT_TRIANGLE_STRIP = 5,
		/// A fan of triangles, 3 vertices for the first triangle, and 1 per triangle after that
		OT_TRIANGLE_FAN = 6,
		/// D3DXMESH
		OT_MESHBIND = 7,

		OT_SKIP = 8,
	};

	/// 顶点缓存指针
	gkVertexBuffer *vertexData;
	uint32 vertexCount;
	uint32 vertexStart;
	size_t vertexSize;
	gkVertexDecl vertexDecl;

	/// 绘制类型
	OperationType operationType;

	/// 是否使用索引
	bool useIndexes;

	/// 索引缓存指针
	gkIndexBuffer *indexData;
	uint32 indexCount;
	uint32 indexStart;
	/// 创建者
	const gkRenderable* srcRenderable;

	gkRenderOperation() :
	vertexData(0), vertexCount(0), operationType(OT_TRIANGLE_LIST), useIndexes(false),
		indexData(0), indexCount(0), srcRenderable(0){}


};
