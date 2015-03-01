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



#pragma once

#include "gkPlatform.h"
#include "gkRenderable.h"
#include "gk_Geo.h"

#include "IMaterial.h"
#include "IMesh.h"

#include "./../../ThirdParty/libovr/Include/OVR.h"

class gkOculus;

class gkOculusDisortationRenderable : public gkRenderable
{
public:
	gkOculusDisortationRenderable(gkOculus*);
	~gkOculusDisortationRenderable(void);

	virtual void getWorldTransforms(Matrix44*) const;

	virtual IMaterial* getMaterial();

	virtual void setMaterialName(const gkStdString& matName);

	virtual const gkStdString& getMaterialName() const;

	virtual AABB& getAABB();

	virtual void getRenderOperation(gkRenderOperation& op);

	virtual bool getSkinnedMatrix(Matrix44A** pMatrixs, uint32& size) {return false;}

	virtual float getSquaredViewDepth(const ICamera* cam) const {return 0;}

	virtual gkMeshPtr& getMesh();

	virtual void setParent(IGameObjectRenderLayer* pParent) {}

	virtual bool castShadow() {return false;}

	virtual void enableShadow(bool enable) {}

	virtual void RP_ShaderSet();
	virtual void RP_Prepare();

	virtual void RT_Prepare();


	gkTexturePtr m_texture;
	gkMaterialPtr m_material;
	gkMeshPtr m_mesh;
	AABB m_aabb;

	ovrVector2f         UVScaleOffset[2][2];

	int m_eye_index;
	ovrHmd HMD;

	gkOculus* m_oculusDevice;
};

