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
// Name:   	gkMaterialManager.h
// Desc:	材质管理器，提供一些默认的共享资源，组织材质
//			控制渲染
// 描述:	
// 
// Author:  Kaiming-Laptop
// Date:	2010/9/20
// 
//////////////////////////////////////////////////////////////////////////

#pragma  once

#include "Prerequisites.h"
#include "IResourceManager.h"
#include "gkSingleton.h"

class gkMaterialPtr;

class gkMaterialManager : public IResourceManager, public Singleton<gkMaterialManager>
{
public:
	static gkMaterialManager& getSingleton(void);
	static gkMaterialManager* getSingletonPtr(void);

	gkMaterialManager(void);
	virtual ~gkMaterialManager(void);

	virtual void saveAllMaterial();

	static gkMaterialPtr ms_DefaultMaterial;

protected:
	virtual IResource* createImpl(const gkStdString& name, gkResourceHandle handle, const gkStdString& group, gkNameValuePairList* params = NULL);
};