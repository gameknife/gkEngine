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
// Name:   	gkMovieMovableInstanceBase.h
// Desc:	
//
// 描述:	
// 
// Author:  Kaiming-Desktop
// Date:	2011/04/06
// Modify:	2011/4/6
// 
//////////////////////////////////////////////////////////////////////////
#pragma once

#include "UIPrerequisites.h"
#include "vtxAtlasPacker.h"
#include "gkRenderable.h"
#include "gkRenderOperation.h"

#pragma warning (disable : 4355)

namespace vtx
{
	class GAMEKNIFEUI_API gkMovableInstanceBase : public AtlasPacker::Listener, public gkRenderable
	{
	public:
		gkMovableInstanceBase(DisplayObject* display_object);
		virtual ~gkMovableInstanceBase();

		// Ogre functions
		const gkMaterialPtr& getMaterial() const;
		void getRenderOperation(gkRenderOperation& op);
		void getWorldTransforms(D3DXMATRIX*) const;
		virtual bool getSkinnedMatrix(float** pStart, uint32& size) {return false;}
		//const Ogre::Quaternion& getWorldOrientation() const;
		//const Ogre::Vector3& getWorldPosition() const;
		// 覆盖renderable接口
		virtual float getSquaredViewDepth(const gkCamera* cam) const ;

	private:
		// rendering
		float* mVertex;
		//Ogre::HardwareVertexBufferSharedPtr mBuffer;
		gkRenderOperation mRenderOp;
		//Ogre::VertexDeclaration* mVertexDecl;
		//Ogre::VertexBufferBinding* mVertexBufferBind;

	protected:
		gkMovableMovie* mParentMovable;
		AtlasPacker* mPacker;
		DisplayObject* mDisplayObject;
		gkMaterialPtr mMaterial;
		D3DXMATRIX mWorldMatrix;

		UINT m_uVerticeBuffer;
		UINT m_uIndiceBuffer;

		// customZOrder [5/9/2011 Kaiming-Desktop]
		mutable int m_nCustomZOrder;

		void _lock();
		void _unlock();

		void _createBuffers();
		void _resizeBuffers(uint num_quads);
		void _addVertex(
			const D3DXVECTOR2& position, 
			const D3DXVECTOR2& texcoord, 
			const D3DXCOLOR& diffuse, 
			const D3DXCOLOR& specular);
	};
}

