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
// Name:   	gkShaderManager.h
// Desc:	全局管理shader的管理器，使得每一个effect只有一份实体
//			并且所有effect共用一些shared的参数
// 描述:	
// 
// Author:  Kaiming-Laptop
// Date:	2010/9/19
// 
//////////////////////////////////////////////////////////////////////////

#pragma  once

#include "Prerequisites.h"
#include "IResourceManager.h"
#include "gkSingleton.h"
#include "IShader.h"

class gkShaderManager : public IResourceManager, public Singleton<gkShaderManager>
{
public:
	static gkShaderManager& getSingleton(void);
	static gkShaderManager* getSingletonPtr(void);

	gkShaderManager(void);
	virtual ~gkShaderManager(void);

	virtual void resetAll();
	virtual void lostAll();

	static gkShaderPtr ms_PostCommon;
	static gkShaderPtr ms_HDRProcess;

	static gkShaderPtr ms_SSAO;
	static gkShaderPtr ms_ShadowMaskGen;
	static gkShaderPtr ms_ShadowStencilGen;

	static gkShaderPtr ms_DeferredSnow;
	static gkShaderPtr ms_SpecilEffects;

	static gkShaderPtr ms_ShadowMapGen;

	static gkShaderPtr ms_ShowDepth;
	static gkShaderPtr ms_GeneralZpass;
	static gkShaderPtr ms_GeneralZpassSkinned;
	static gkShaderPtr ms_ReflGen;
	static gkShaderPtr ms_AuxRenderer;
	static gkShaderPtr ms_AuxRenderer_Tex;

	static gkShaderPtr ms_DefaultRender;

	static gkShaderPtr ms_postcopy;
	static gkShaderPtr ms_postbrightpass;
	static gkShaderPtr ms_postbloom;
	static gkShaderPtr ms_postbloomdof;
	static gkShaderPtr ms_postgaussian;
	static gkShaderPtr ms_postmsaa;

	static gkShaderPtr ms_postcvtlineardepth;
	static gkShaderPtr ms_postssao;
	static gkShaderPtr ms_postforprocess;

	static gkShaderPtr ms_DeferredLighting_Ambient;
	static gkShaderPtr ms_DeferredLighting_PointLight;

	static gkShaderPtr ms_Simple;

protected:
	virtual IResource* createImpl(const gkStdString& name, gkResourceHandle handle, const gkStdString& group, gkNameValuePairList* params = NULL);
};
