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
// Name:   	gmfCompiler.h
// Desc:	
// 	
// 
// Author:  Kaiming
// Date:	2013/3/25	
// 
//////////////////////////////////////////////////////////////////////////

#ifndef gmfCompiler_h__
#define gmfCompiler_h__

#include "icompiler.h"
#include "MathLib/gk_Vector3.h"
#include "IMesh.h"

struct objSubmeshStruct
{

};

class gmfCompiler : public ICompiler
{
public:
	gmfCompiler() {g_compilers.push_back( this );}
	~gmfCompiler() {}

	virtual bool encode(const char* filename, const char* cfg);
	virtual bool writeFile(const char* filename, const char* cfg);
	virtual bool decode();

	virtual bool supportInput( const char* ext );

	virtual bool supportOutput( const char* ext );

	virtual const char* getName();

	const uint8* m_dataPtr;

	Vec3Array m_positions;
	Vec2Array m_texcoords;
	FaceArray m_faces;
	SubsetArray m_subsets;
};

#endif // gmfCompiler_h__
