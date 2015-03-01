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
// Name:   	DemoSceneBuilder.h
// Desc:	demo工程用的场景构建器，只负责构建，不存储任何数据
// 描述:	
// 
// Author:  Kaiming-Desktop	 
// Date:	12/23/2010 	
// 
//////////////////////////////////////////////////////////////////////////
#pragma once
#include "gkSystemPrerequisites.h"
#include "IXmlUtil.h"
#include <list>

struct IGameObject;

class gkSceneBuilder
{
public:
	gkSceneBuilder(void);
	virtual ~gkSceneBuilder(void);

	// 场景读取入口
	static void buildSceneFromFile(gkStdString filename, bool syncMode = false, bool bPak = false, Vec3 position = Vec3(0,0,0), uint8 uRenderLayer = RENDER_LAYER_OPAQUE, bool builtIn = false);
	void saveSceneToFile(const TCHAR* filename);

private:
	typedef std::list<IGameObject*> EntityBuildingList;

	static EntityBuildingList m_lastEntityList;
	static EntityBuildingList m_currEntityList;

	static bool m_bBuiltInPak;
	static uint8 m_uRenderLayer;

	static int ms_nAllNodeCount;
	static int ms_nLoadedNodeCount;
	
	static void parseSceneObject(CRapidXmlParseNode* firstnode, IGameObject* parent = NULL);

	static void addExistEntity2List(IGameObject* entity);
	static void removeEntitiesDeleted();

	void exportChildInfo(IGameObject* child, CObjectNode* parentAuthor, IRapidXmlAuthor* author, uint32& counter);
};