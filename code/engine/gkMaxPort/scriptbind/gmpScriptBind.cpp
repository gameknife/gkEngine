#include "../system/gkmaxport.h"
#include "objects/gmpObject.h"
#include "exporter/gmpMeshLoader.h"
#include "exporter/gmpExporter.h"
#include "viewport/gmpMaxViewport.h"

// expose some global function to MaxScript
#if MAX_VERSION_MAJOR > 13
#include "MAXScript/MAXScript.h"
#include "maxscript/macros/define_instantiation_functions.h"
#define OBJEXPORTER_CLASSID Class_ID(1371343970, 1730353346)			

#else
#include "MAXScrpt/MAXScrpt.h"
#include "MAXScrpt/MAXObj.h"
// The most important one.  Defines the correct flavor of dev_visible_primitive()
#include "MAXScrpt\definsfn.h"  

#endif






def_visible_primitive( GMP_SyncScene, "GMPSyncScene");

Value* GMP_SyncScene_cf(Value** arg_list, int count) {

	//MessageBox(NULL, "Sync gkMaxPort Scene...",NULL,MB_OK);

	Class_ID exporterID = IGAMEEXPORTER_CLASS_ID;
	GetCOREInterface()->ExportToFile(gUtilEnv->SyncSceneName, 1, 0, &exporterID);

	if (gUtilEnv->m_bMaxPorted)
	{
		//TCHAR wszName[MAX_PATH];
		//MultiByteToWideChar( CP_ACP, 0, gUtilEnv->SyncSceneName, -1, wszName, MAX_PATH );
		gUtilEnv->pSystem->LoadScene_s(gUtilEnv->SyncSceneName, NULL);
	}
	else
	{
		HWND h = FindWindow( "gkStudioClass", NULL );		//changed by [MG]
		if (h)
		{

// 			COPYDATASTRUCT cd;
// 			ZeroMemory(&cd, sizeof(COPYDATASTRUCT));
// 			cd.dwData = CPDATA_SYNC_SCENE;
// 			cd.cbData = gUtilEnv->SyncSceneName.length();
// 			cd.lpData = gUtilEnv->SyncSceneName.data();
// 
// 			SendMessage( h,WM_COPYDATA,(WPARAM)NULL,(LPARAM)&cd );
// 			SetActiveWindow( h );
// 			SetForegroundWindow( h );
		}
	}

	return &ok;

}



def_visible_primitive( GMP_CreateObj, "GMPCreateObj");

Value* GMP_CreateObj_cf(Value** arg_list, int count) {

	gkObjectInMax *gkOBJ = (gkObjectInMax *)CreateInstance(GEOMOBJECT_CLASS_ID, Class_ID(GEOSPHERE_CLASS_ID, 32670)); 
	OPENFILENAME ofn;
	static char szFile[256];
	static char szFileTitle[256];
	memset(&ofn,0,sizeof(ofn));

	ofn.lStructSize=sizeof(ofn);
	ofn.hwndOwner=GetCOREInterface()->GetMAXHWnd();
	ofn.lpstrFilter="gkMesh File(*.obj,*.gkm)\0*.obj;*.gkm;\0\0";
	ofn.nFilterIndex=1;
	ofn.lpstrFile=szFile;
	ofn.nMaxFile=sizeof(szFile);

	ofn.lpstrFileTitle=szFileTitle;
	ofn.nMaxFileTitle=sizeof(szFileTitle);

	ofn.Flags=OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST|OFN_EXPLORER;


	// Display the Open dialog box. 

	if (GetOpenFileName(&ofn)) 
	{
		// 得到了
		gkOBJ->pblock2->SetValue(geo_meshpath, 0, ofn.lpstrFile);	
	}

	INode *node = GetCOREInterface()->CreateObjectNode(gkOBJ);

	return &ok;

}

def_visible_primitive( GMP_SaveGMFAs, "GMPSaveGMFAs" );
Value* GMP_SaveGMFAs_cf(Value** arg_list, int count)
{
	gmpMeshLoader meshLoader;

	OPENFILENAME ofn;
	static char szFile[256];
	static char szFileTitle[256];
	memset(&ofn,0,sizeof(ofn));

	ofn.lStructSize=sizeof(ofn);
	ofn.hwndOwner=GetCOREInterface()->GetMAXHWnd();
	ofn.lpstrFilter="gkMesh File(*.gmf)\0*.gmf;\0\0";
	ofn.nFilterIndex=1;
	ofn.lpstrFile=szFile;
	ofn.nMaxFile=sizeof(szFile);
	ofn.lpstrFileTitle=szFileTitle;
	ofn.nMaxFileTitle=sizeof(szFileTitle);

	ofn.Flags=OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST|OFN_EXPLORER;

	// Display the Open dialog box. 

	if (GetSaveFileName(&ofn)) 
	{
		INode *node = GetCOREInterface()->GetSelNode(0);
		if (node)
		{
			const ObjectState& os = node->EvalWorldState(0);

			NullView1 view;
			BOOL bNeedDelete = FALSE;

			GeomObject* pGeoObj = NULL;

			if(os.obj->CanConvertToType(triObjectClassID))
			{
				TriObject* pObj = (TriObject*)(os.obj->ConvertToType(0, triObjectClassID));
				pGeoObj = (GeomObject*)pObj;
			}
			else
			{
				return &ok;
			}

			gmpMeshLoader loader;
			Mesh * pMesh = pGeoObj->GetRenderMesh( 0 , node , view , bNeedDelete );

			TCHAR szFilename[MAX_PATH];
			_tcscpy_s(szFilename, ofn.lpstrFile);

			Box3 box = pMesh->getBoundingBox();
			float radius = (box.pmax - box.pmin).Length();

			loader.SaveGeometryAsGMF( szFilename, *pMesh, radius );

			return &ok;
		}
	}
	return &ok;
}

def_visible_primitive( GMP_SaveObjAs, "GMPSaveObjAs" );
Value* GMP_SaveObjAs_cf(Value** arg_list, int count)
{
	gmpMeshLoader meshLoader;

	OPENFILENAME ofn;
	static char szFile[256];
	static char szFileTitle[256];
	memset(&ofn,0,sizeof(ofn));

	ofn.lStructSize=sizeof(ofn);
	ofn.hwndOwner=GetCOREInterface()->GetMAXHWnd();
	ofn.lpstrFilter="gkMesh File(*.obj)\0*.obj;\0\0";
	ofn.nFilterIndex=1;
	ofn.lpstrFile=szFile;
	ofn.nMaxFile=sizeof(szFile);
	ofn.lpstrFileTitle=szFileTitle;
	ofn.nMaxFileTitle=sizeof(szFileTitle);

	ofn.Flags=OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST|OFN_EXPLORER;

	// Display the Open dialog box. 

	if (GetSaveFileName(&ofn)) 
	{
		INode *node = GetCOREInterface()->GetSelNode(0);
		if (node)
		{
			const ObjectState& os = node->EvalWorldState(0);

			NullView1 view;
			BOOL bNeedDelete = FALSE;

			GeomObject* pGeoObj = NULL;

			if(os.obj->CanConvertToType(triObjectClassID))
			{
				TriObject* pObj = (TriObject*)(os.obj->ConvertToType(0, triObjectClassID));
				pGeoObj = (GeomObject*)pObj;
			}
			else
			{
				return &ok;
			}

			gmpMeshLoader loader;
			Mesh * pMesh = pGeoObj->GetRenderMesh( 0 , node , view , bNeedDelete );

			TCHAR szFilename[MAX_PATH];
			_tcscpy_s(szFilename, ofn.lpstrFile);

			Box3 box = pMesh->getBoundingBox();
			float radius = (box.pmax - box.pmin).Length();

			loader.SaveGeometryAsOBJ( szFilename, *pMesh, radius );

			return &ok;
		}
	}
	return &ok;
}

def_visible_primitive( GMP_SaveObjFast, "GMPSaveObjFast" );
Value* GMP_SaveObjFast_cf(Value** arg_list, int count)
{

	// check that we have 1 arg and that it's a string

	check_arg_count(GMP_SaveObjFast, 1, count);

	bool promote = arg_list[0]->to_bool();

	INode* node = GetCOREInterface()->GetSelNode(0);
	if (node)
	{	


		const ObjectState& os = node->EvalWorldState(0);


		NullView1 view;
		BOOL bNeedDelete = FALSE;

		GeomObject* pGeoObj = NULL;

		if(os.obj->CanConvertToType(triObjectClassID))
		{
			TriObject* pObj = (TriObject*)(os.obj->ConvertToType(0, triObjectClassID));
			pGeoObj = (GeomObject*)pObj;
		}
		else
		{
			return &ok;
		}

		char path[MAX_PATH];
		// load from engine path [9/26/2011 Kaiming]

		getEngineRoot(path);

		strcat_s(path, "\\Media\\Meshs\\Temp\\gmp_");
		strcat_s(path, node->GetName());
		strcat_s(path, ".obj");


		gmpMeshLoader loader;
		Mesh * pMesh = pGeoObj->GetRenderMesh(

			0 , node , view , bNeedDelete );

		//TCHAR szFilename[MAX_PATH];
		//MultiByteToWideChar(CP_ACP, 0,  path, -1, szFilename, MAX_PATH);

		Box3 box = pMesh->getBoundingBox();
		float radius = (box.pmax - box.pmin).Length();
		loader.SaveGeometryAsOBJ( path, *pMesh, radius );

		//GetCOREInterface()->ExportToFile(path, promote, SCENE_EXPORT_SELECTED, &(OBJEXPORTER_CLASSID));

		Matrix3 backupTm = node->GetNodeTM(0);

		// call gkStudio update this mesh [9/3/2011 Kaiming-Desktop]
		// clear this node
		Mtl* mat = node->GetMtl();

		//node->DeleteMe();

		// create a new one
		gkObjectInMax *gkOBJ = (gkObjectInMax *)CreateInstance(GEOMOBJECT_CLASS_ID, Class_ID(GEOSPHERE_CLASS_ID, 32670)); 

		gkOBJ->pblock2->SetValue(geo_meshpath, 0, path);	

		INode *nodenew = GetCOREInterface()->CreateObjectNode(gkOBJ);
		nodenew->SetMtl(mat);
		nodenew->SetNodeTM(0, backupTm);
		//nodenew->SetName(node->GetName());


		GetCOREInterface()->SelectNode(nodenew);
		GetCOREInterface()->DeleteNode(node);

		// reload this mesh
		// 砍掉路径，保留文件名

		//TCHAR wszFileName[MAX_PATH];
		//MultiByteToWideChar( CP_ACP, 0, (LPCSTR)path, -1, wszFileName, MAX_PATH );

		TCHAR wszPureName[MAX_PATH] = {0};
		TCHAR* wstrLastSlash = NULL;
		wstrLastSlash = _tcsrchr( path, '\\' );
		if( wstrLastSlash )
		{
			_tcscpy_s( wszPureName, MAX_PATH, &wstrLastSlash[1] );
		}

		// 			if (gEnv)
		// 			{
		// 				gEnv->pSystem->getMeshMngPtr()->reload(wszPureName);
		// 			}
		// 			


	}


	return &ok;
}

#include "..\..\gkCommon\I3DEngine.h"
#include "..\..\gkCommon\ITimeOfDay.h"

def_visible_primitive( GMP_ChangeTOD, "GMPChangeTOD");

Value* GMP_ChangeTOD_cf(Value** arg_list, int count) {

	// check that we have 1 arg and that it's a string

	check_arg_count(GMP_ChangeTOD, 1, count);

	float fn = arg_list[0]->to_float();

	gEnv->p3DEngine->getTimeOfDay()->setTime(fn);

	return &ok;

}

def_visible_primitive( GMP_ExportMtl, "GMPExportMtl");

Value* GMP_ExportMtl_cf(Value** arg_list, int count) {

	// check that we have 1 arg and that it's a string

	check_arg_count(GMP_ExportMtl, 1, count);

	char* path = arg_list[0]->to_string();

	INode* node = GetCOREInterface()->GetSelNode(0);
	if (node)
	{
		Mtl* mtl = node->GetMtl();
		if (mtl)
		{
			IGameExporter::AuthorMaterial( mtl, path );
		}
		
		
	}


	
	return &ok;
	

}