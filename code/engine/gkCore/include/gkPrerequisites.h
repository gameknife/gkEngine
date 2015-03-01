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
// Name:   	gkPrerequisites.h
// Desc:	All header file should inlude this file. It inludes some
//			platfrom infomation. And, it allow headers to use 
//			the pointer of classed whthout include the individual header.

// 描述:	所有的头文件都应该包含此前提文件，他包含了基础信息
//			以及类的声明，让头文件可以使用类的指针而无需包含
//			独立的头文件。
// 
// Author:  Kaiming-Desktop	 
// Date:	9/9/2010 	
// 
//////////////////////////////////////////////////////////////////////////

#pragma once
#include "gkPlatform.h"

/// dll export flag

#ifdef GAMEKNIFECORE_EXPORTS
#define GAMEKNIFECORE_API DLL_EXPORT
#else
#define GAMEKNIFECORE_API DLL_IMPORT
#endif

/// D3D9 headers
// #include <d3d9.h>
// #include <d3dx9.h>
// #include <DxErr.h>

#include "gk_Math.h"
#include "gk_Color.h"

// declaration for objects
struct gkRenderOperation;
struct gkRenderable;
class gkBasicRenderable;
class gkBillboard;
class gkCamera;
class gkEntity;
class gkLight;
class gkSun;
class gkFrustum;
class gkMovableObject;
class gkNode;
class IResource;
class gkSceneNode;
class gkStaticGeometry;
class gkStaticObjRenderable;
class gkSubMesh;
class gkShaderParamDataSource;
class gkBillboardSet;
class gkBillboardParticleRenderer;

// declaration for sturcts
class gkRenderLayer;
class gkRenderPriority;
struct IRenderSequence;
class gkRenderSystem;
class gkRenderTarget;
class gkRenderWindow;
class gkRenderTexture;
class gkViewPort;
class gkCamera;

// declaration for Managers

class gkD3D9RenderSystem;
struct IRenderSequence;
class IResourceManager;
// class gkTextureManager;
// class gkShaderManager;
class gkSceneManager;
// class gkMaterialManager;
// class gkMeshManager;
class gkHelperObjectManager;
class gkPostProcessManager;
class gkVtxLayerManager;
class gkConsoleVariablesManager;

class gk3DEngine;
class gkMovableObjectFactory;

// misc
class gkNameGenerator;

// TODO
// to add all class here...
typedef std::map<gkStdString, gkStdString> NameValuePairList;
