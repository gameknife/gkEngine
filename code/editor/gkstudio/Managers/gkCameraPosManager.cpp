#include "StdAfx.h"
#include "gkCameraPosManager.h"
#include "gkFilePath.h"
#include "IXmlUtil.h"
#include "ICamera.h"
#include "I3DEngine.h"
#include "IEditor.h"


//////////////////////////////////////////////////////////////////////////
gkCameraPosManager::gkCameraPosManager()
{
	m_lstCamPos.clear();
	m_lstCamPos.assign(10, SCameraRecord());
}
//////////////////////////////////////////////////////////////////////////
void gkCameraPosManager::LoadUserDefineFile( const TCHAR* file )
{
// 	TCHAR absPath[MAX_PATH];
// 	if( S_FALSE == gkFindFileRelativeGame(absPath, MAX_PATH, file) )
// 	{
// 
// 	}
	//	return;

	// loading start
	IRapidXmlParser parser;
	parser.initializeReading(file);

	CRapidXmlParseNode* pRoot = parser.getRootXmlNode(_T("Cameras"));

	if (!pRoot)
	{
		return;
	}

	CRapidXmlParseNode* pCam = pRoot->getChildNode(_T("CameraRecord"));
	for( ; pCam; pCam = pCam->getNextSiblingNode() )
	{
		int index;
		Vec3 pos;
		Quat rot;
		pCam->GetAttribute(_T("Index"), index);
		pCam->GetAttribute(_T("Pos"), pos);
		pCam->GetAttribute(_T("Rot"), rot);

		SaveCamPoseInternal(pos, rot, index);
	}

	if (m_lstCamPos[0].m_Initialized)
	{
		ICamera* cam = gEnv->p3DEngine->getMainCamera();
		cam->setPosition( m_lstCamPos[0].m_Pose.t );
		cam->setOrientation( m_lstCamPos[0].m_Pose.q );
	}
}
//////////////////////////////////////////////////////////////////////////
void gkCameraPosManager::SaveUserDefineFile( const TCHAR* file )
{
	IRapidXmlAuthor author;
	// initialize the doc [12/4/2011 Kaiming]
	author.initializeSceneWrite();

	CRapidXmlAuthorNode* head = author.createRapidXmlNode(_T("CameraSetting"));
	head->AddAttribute(_T("author"), _T("gkENGINE RapidXml Author"));
	head->AddAttribute(_T("version"), _T("1.0.0"));

	CRapidXmlAuthorNode* root = author.createRapidXmlNode(_T("Cameras"));

	// backup current campos
	ICamera* cam = gEnv->p3DEngine->getMainCamera();
	CRapidXmlAuthorNode* node = root->createChildNode(_T("CameraRecord"));
	node->AddAttribute( _T("Index"), 0 );
	node->AddAttribute( _T("Pos"), cam->getDerivedPosition() );
	node->AddAttribute( _T("Rot"), cam->getDerivedOrientation() );

	CamPosList::iterator it = m_lstCamPos.begin();
	uint8 count = 0;
	for ( ; it != m_lstCamPos.end(); ++it )
	{
		if( it->m_Initialized )
		{
			CRapidXmlAuthorNode* node = root->createChildNode(_T("CameraRecord"));
			node->AddAttribute( _T("Index"), count );
			node->AddAttribute( _T("Pos"), it->m_Pose.t );
			node->AddAttribute( _T("Rot"), it->m_Pose.q );
		}
		count++;
	}


	author.writeToFile(file);
}
//////////////////////////////////////////////////////////////////////////
void gkCameraPosManager::SwitchToCam( uint8 id )
{
	if ( id < m_lstCamPos.size())
	{
		if( m_lstCamPos[id].m_Initialized )
		{
			QuatT pose = m_lstCamPos[id].m_Pose;
			GetIEditor()->moveCameraTo(  pose.t, pose.q );
		}
	}
}
//////////////////////////////////////////////////////////////////////////
void gkCameraPosManager::SaveCamPose( uint8 id )
{
	if (id < m_lstCamPos.size())
	{
		ICamera* cam = gEnv->p3DEngine->getMainCamera();
		Vec3 pos = cam->getDerivedPosition();
		Quat rot = cam->getDerivedOrientation();
		SaveCamPoseInternal(pos, rot, id);


		// save file once
	}
}
//////////////////////////////////////////////////////////////////////////
void gkCameraPosManager::SaveCamPoseInternal( Vec3 pos, Quat rot, uint8 id )
{
	SCameraRecord camRcd;
	camRcd.m_Initialized = true;
	camRcd.m_Pose.t = pos;
	camRcd.m_Pose.q = rot;
	m_lstCamPos[id] = camRcd;
}

