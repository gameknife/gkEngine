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
// yikaiming (C) 2013
// gkENGINE Source File 
//
// Name:   	RendPipe_ShadingPassDeferredLighting.h
// Desc:	
// 	
// 
// Author:  YiKaiming
// Date:	2013/5/9
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _RendPipe_ShadingPassDeferredLighting_h_
#define _RendPipe_ShadingPassDeferredLighting_h_

#include "RendPipelineBase.h"

class RendPipe_ShadingPassDeferredLighting : public IRendPipeBase
{
public:
	RendPipe_ShadingPassDeferredLighting(void);
	~RendPipe_ShadingPassDeferredLighting(void);

	virtual void Prepare( gkRenderSequence* renderSeq );

	virtual void Execute( gkRenderSequence* renderSeq );

	virtual void End( gkRenderSequence* renderSeq );

};

#endif



