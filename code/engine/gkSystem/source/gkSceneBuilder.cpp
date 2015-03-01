#include "gkSystemStableHeader.h"
#include "gkSceneBuilder.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include "IResourceManager.h"
#include "IMaterial.h"
#include "ITimeOfDay.h"
#include "IMesh.h"

#include "IGameObject.h"
#include "IGameObjectLayer.h"
#include "ITerrianSystem.h"

#include "gkMemoryLeakDetecter.h"






//#include "gkD3DMathExtended.h"
//#include "gkdecalsmanager.h"

enum GKS_NODETYPE
{
	GKS_NODETYPE_UNKNOWN,
	GKS_NODETYPE_MESH,
	GKS_NODETYPE_CAMERA,
	GKS_NODETYPE_LIGHT,
	GKS_NODETYPE_GROUP,
};

gkSceneBuilder::EntityBuildingList gkSceneBuilder::m_lastEntityList;
gkSceneBuilder::EntityBuildingList gkSceneBuilder::m_currEntityList;
bool gkSceneBuilder::m_bBuiltInPak = false;
uint8 gkSceneBuilder::m_uRenderLayer = 0;
int gkSceneBuilder::ms_nAllNodeCount = 0;
int gkSceneBuilder::ms_nLoadedNodeCount = 0;

//-----------------------------------------------------------------------
gkSceneBuilder::gkSceneBuilder( void )
{

}
//-----------------------------------------------------------------------
gkSceneBuilder::~gkSceneBuilder( void )
{

}

//-----------------------------------------------------------------------
void gkSceneBuilder::buildSceneFromFile( gkStdString filename,bool syncMode, bool bPak , Vec3 position, uint8 uRenderLayer, bool builtIn)
{
	HRESULT hr = S_OK;

	m_bBuiltInPak = builtIn;
	m_uRenderLayer = uRenderLayer;

	if (!builtIn)
	{
		// 		gkStdString relScenePath = gkGetGameRelativePath(wszPath);
		// 
		TCHAR todfilename[MAX_PATH];
		_tcscpy_s(todfilename, filename.c_str());
		todfilename[_tcslen(todfilename) - 4] = 0;
		_tcscat_s(todfilename, _T(".tod"));

		IRapidXmlParser todParser;
		todParser.initializeReading(todfilename);
		if (!todParser.getRootXmlNode())
		{
			return;
		}
		gEnv->p3DEngine->getTimeOfDay()->loadTODSequence(todParser.getRootXmlNode(), true);
		todParser.finishReading();


		// 


		//		gkLogMessage( _T("gkSceneBuilder::TodFile Loaded. [%s]"), filename.c_str() );
	}

	ms_nAllNodeCount = 0;
	ms_nLoadedNodeCount = 0;

	// if not syncMode, we should clear the cache
	if (!syncMode)
	{
		m_currEntityList.clear();
		m_lastEntityList.clear();
	}

	// first drim to a pure filename [8/20/2011 Kaiming-Desktop]
// 	uint8 index = filename.find_last_of('\\') + 1;
// 	uint8 count = filename.length() - index;
// 	filename = filename.substr(index, count);
// 
// 	// 先在目录内查找名字 [9/18/2010 Kaiming-Desktop]
// 	TCHAR wszPath[MAX_PATH] = _T("");
// 	hr = gkFindFileRelativeGame(wszPath, MAX_PATH, filename.c_str());
// 	assert( hr == S_OK );

	// if not builtIn, load tod file [1/8/2012 Kaiming]



	gkLogMessage( _T("gkSceneBuilder::Creating: [%s] ..."), filename.c_str() );
	float nStartTime = gEnv->pTimer->GetAsyncCurTime();

	IRapidXmlParser sceneParser;
	sceneParser.initializeReading(filename.c_str());

	// 建立地形 [4/4/2013 Kaiming]
	CRapidXmlParseNode* terrianNode = sceneParser.getRootXmlNode(_T("Terrian"));
	if (terrianNode)
	{
		ITerrianSystem* terrian = gEnv->p3DEngine->createTerrian();
		terrian->Create( terrianNode );
	}

	CRapidXmlParseNode* rootNode = sceneParser.getRootXmlNode(_T("SceneObjects"));

	rootNode->GetAttribute(_T("AllCount"), ms_nAllNodeCount);

	CRapidXmlParseNode* firstObject = rootNode->getChildNode(_T("gkObject"));

	if (firstObject)
	{
		// parsing one by one
		parseSceneObject(firstObject);	
	}
	sceneParser.finishReading();

	gEnv->pSystem->updateProgress( 100 );

	removeEntitiesDeleted();

	float nTimeLoadFile = gEnv->pTimer->GetAsyncCurTime() - nStartTime;

	gkLogWarning( _T("gkSceneBuilder::Loaded: [%s], use %0.2f seconds. \n"), filename.c_str() , nTimeLoadFile );
}

//-----------------------------------------------------------------------
void gkSceneBuilder::addExistEntity2List( IGameObject* entity )
{
	if (m_bBuiltInPak)
		return;
	EntityBuildingList::iterator it = m_lastEntityList.begin();

	for (;it != m_lastEntityList.end();)
	{
		if ( *it == entity)
		{
			it = m_lastEntityList.erase(it);
		}
		else
		{
			++it;
		}
	}

	m_currEntityList.push_back(entity);
}
//-----------------------------------------------------------------------
void gkSceneBuilder::removeEntitiesDeleted()
{
	if (m_bBuiltInPak)
		return;
	EntityBuildingList::iterator it = m_lastEntityList.begin();

	for (;it != m_lastEntityList.end(); ++it)
	{
		gEnv->pGameObjSystem->DestoryGameObject((*it));
	}

	//swap two list
	m_lastEntityList.clear();
	it = m_currEntityList.begin();

	for (;it != m_currEntityList.end(); ++it)
	{
		m_lastEntityList.push_back(*it);
	}

	m_currEntityList.clear();
}

//-----------------------------------------------------------------------
void gkSceneBuilder::exportChildInfo(IGameObject* child, CObjectNode* parentAuthor, IRapidXmlAuthor* author, uint32& counter)
{
	// name
	parentAuthor->AddAttribute(_T("Name"), child->getName().c_str());

	// meshname
	switch(child->getGameObjectSuperClass())
	{
		case eGOClass_GROUP:
			{
				parentAuthor->AddAttribute(_T("SuperClass"), _T("Group"));
			}
			break;
		case eGOClass_STATICMESH:
			{
				parentAuthor->AddAttribute(_T("SuperClass"), _T("Mesh"));

				// Mesh name to fullpath [12/30/2011 Kaiming]
				IGameObjectRenderLayer* pRenderLayer = child->getRenderLayer();
				if (pRenderLayer)
				{
					gkStdString relPath = gkGetGameRelativePath( pRenderLayer->getMesh()->getName());
					parentAuthor->AddAttribute(_T("MeshName"), relPath.c_str());
					relPath = pRenderLayer->getMaterialName();
					if (relPath.size() > 4)
					{
						TCHAR extension[10];
						_tcscpy_s(extension, 10, &(relPath.c_str()[relPath.size()-4]));
						if (_tcsicmp(extension, _T(".MTL")))
						{
							relPath += _T(".mtl");
						}
						relPath = gkGetGameRelativePath( relPath );
					}

					parentAuthor->AddAttribute(_T("MaterialName"), relPath.c_str());
				}


				IGameObjectPhysicLayer* pPhysicLayer = child->getPhysicLayer();
				if (pPhysicLayer)
				{
					parentAuthor->AddAttribute(_T("Physical"), "true");
				}
			}
			break;
		case eGOClass_LIGHT:
			{
				parentAuthor->AddAttribute(_T("SuperClass"), _T("Light"));

				IGameObjectLightLayer* pLightLayer = static_cast<IGameObjectLightLayer*>(child->getGameObjectLayer(eGL_LightLayer));

				parentAuthor->AddAttribute(_T("Color"), pLightLayer->getDiffuseColor());
				parentAuthor->AddAttribute(_T("Radius"), pLightLayer->getRadius());
				parentAuthor->AddAttribute(_T("FakeShadow"), pLightLayer->getFakeShadow());
				parentAuthor->AddAttribute(_T("GloabalShadow"), pLightLayer->getGlobalShadow());
			}
			break;

	}

	// matrix
	parentAuthor->SetTranslation(child->getPosition().x, child->getPosition().y, child->getPosition().z);
	parentAuthor->SetRotation(child->getOrientation().v.x, child->getOrientation().v.y, child->getOrientation().v.z, child->getOrientation().w);
	parentAuthor->SetScale(child->getScale().x, child->getScale().y, child->getScale().z);

	counter++;
	// child situation
	uint32 childcount = child->getChildCount();
	for(uint32 i=0; i < childcount; ++i)
	{
		if (child->getChild(i) && child->getChild(i)->getGameObjectSuperClass() != eGOClass_SYSTEM)
		{
			// write author nodes [12/4/2011 Kaiming]
			CObjectNode* authorNode = author->createObjectNode( parentAuthor );
			exportChildInfo(child->getChild(i), authorNode, author, counter);
		}
	}


}

//-----------------------------------------------------------------------
void gkSceneBuilder::saveSceneToFile( const TCHAR* filename )
{
	float estTime = gEnv->pTimer->GetAsyncCurTime();

	IRapidXmlAuthor author;
	// initialize the doc [12/4/2011 Kaiming]
	author.initializeSceneWrite();

	CRapidXmlAuthorNode* head = author.createRapidXmlNode(_T("GameKnifeScene"));
	head->AddAttribute(_T("author"), _T("gkENGINE RapidXml Author"));
	head->AddAttribute(_T("version"), _T("1.0.0"));
	head->AddAttribute(_T("level"), filename);

	//////////////////////////////////////////////////////////////////////////
	// 导出地形
	ITerrianSystem* terrian = gEnv->p3DEngine->getTerrian();
	if (terrian)
	{
		CRapidXmlAuthorNode* terrianNode = author.createRapidXmlNode(_T("Terrian"));
		
		terrian->Save( terrianNode );
	}

	CRapidXmlAuthorNode* root = author.createRapidXmlNode(_T("SceneObjects"));
	
	uint32 count = gEnv->pGameObjSystem->getGameObjectCount();
	uint32 realcount = 0;
	uint32 fullcount = 0;
	for( uint32 i=0; i < count; ++i)
	{
		IGameObject* go = gEnv->pGameObjSystem->GetGameObjectById(i);
		if (go && go->getParent() == NULL && go->getGameObjectSuperClass() != eGOClass_SYSTEM)
		{
			CObjectNode* authorNode = author.createObjectNode(root);
			exportChildInfo(go, authorNode, &author, fullcount);
			realcount++;
			fullcount++;
		}
	}
	root->AddAttribute(_T("TopLevelCount"), (int)realcount);
	root->AddAttribute(_T("AllCount"), (int)fullcount);


	author.writeToFile(filename);

	// write the same name tod file [1/8/2012 Kaiming]
	gkStdString relScenePath = gkGetGameRelativePath(filename);

	TCHAR todfilename[MAX_PATH];
	_tcscpy_s(todfilename, relScenePath.c_str());
	todfilename[_tcslen(todfilename) - 4] = 0;
	_tcscat_s(todfilename, _T(".tod"));

	gEnv->p3DEngine->getTimeOfDay()->saveTODSequence(todfilename);

	estTime = gEnv->pTimer->GetAsyncCurTime() - estTime;
// 	gkLogMessage(_T("Scene [ %s ] saved successfully. Use %.4f seconds."), filename, estTime);
// 	gkLogMessage(_T("TOD [ %s ] saved successfully. Use %.4f seconds."), todfilename, estTime);


}
//////////////////////////////////////////////////////////////////////////
void gkSceneBuilder::parseSceneObject( CRapidXmlParseNode* firstnode, IGameObject* parent )
{
	for ( ; firstnode != NULL; firstnode = firstnode->getNextSiblingNode(_T("gkObject")) )
	{
		IGameObject* pCreated = NULL;

		pCreated = gEnv->pGameObjSystem->CreateGameObjectFromXml( firstnode );
		if (m_bBuiltInPak)
		{
			pCreated->setGameObjectSuperClass(eGOClass_SYSTEM);
		}

		if (pCreated)
		{
			IGameObjectRenderLayer* pRenderLayer = pCreated->getRenderLayer();
			//pRenderLayer->setRenderLayer(m_uRenderLayer);
			// if has parent, attach
			if (parent)
			{
				parent->attachChild(pCreated);
			}

			addExistEntity2List(pCreated);

			if (ms_nAllNodeCount != 0)
				gEnv->pSystem->updateProgress( ++ms_nLoadedNodeCount * 100 / ms_nAllNodeCount);

			// if we got child
			CRapidXmlParseNode* childNode = firstnode->getChildNode(_T("gkObject"));
			if (childNode)
			{
				parseSceneObject(childNode, pCreated);
			}

			
		}

	}
}

