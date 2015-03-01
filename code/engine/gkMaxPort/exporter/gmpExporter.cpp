/**********************************************************************
 *<
	FILE: IGameExporter.cpp

	DESCRIPTION:	Sample to test the IGame Interfaces.  It follows a similar format
					to the Ascii Exporter.  However is does diverge a little in order
					to handle properties etc..

	TODO: Break the file down into smaller chunks for easier loading.

	CREATED BY:		Neil Hazzard	

	HISTORY:		parttime coding Summer 2002

 *>	Copyright (c) 2002, All Rights Reserved.
 **********************************************************************/

#include "../system/gkMaxPort.h"
#include "decomp.h"
#include "msxml2.h"

#include "gmpExporter.h"
#include "objects/gmpObject.h"

#include "3dsmaxport.h"

#include <iostream>
#include <locale>
#include <conio.h>

//corresponds to XML schema
TCHAR* mapSlotNames[] = {
	_T("Diffuse"),
	_T("Ambient"),
	_T("Specular"),
	_T("SpecularLevel"),
	_T("Opacity"),
	_T("Glossiness"),
	_T("SelfIllumination"),
	_T("Filter"),
	_T("Bump"),
	_T("Reflection"),
	_T("Refraction"),
	_T("Displacement"),
	_T("Unknown") };

class IGameExporterClassDesc:public ClassDesc2 {
	public:
	int 			IsPublic() { return TRUE; }
	void *			Create(BOOL loading = FALSE) { gExporter = new IGameExporter(); return gExporter; }
	const TCHAR *	ClassName() { return GetString(IDS_CLASS_NAME); }
	SClass_ID		SuperClassID() { return SCENE_EXPORT_CLASS_ID; }
	Class_ID		ClassID() { return IGAMEEXPORTER_CLASS_ID; }
	const TCHAR* 	Category() { return GetString(IDS_CATEGORY); }

	const TCHAR*	InternalName() { return _T("IGameExporter"); }	// returns fixed parsable name (scripter-visible name)
	HINSTANCE		HInstance() { return hInstance; }				// returns owning module handle

};


IGameScene* IGameExporter::pIgame = NULL;
static IGameExporterClassDesc IGameExporterDesc;
ClassDesc2* GetIGameExporterDesc() { return &IGameExporterDesc; }

static bool IsFloatController(IGameControlType Type)
{
	if(Type == IGAME_FLOAT || Type==IGAME_EULER_X || Type == IGAME_EULER_Y || 
		Type == IGAME_EULER_Z || Type == IGAME_POS_X || Type == IGAME_POS_Y || 
		Type == IGAME_POS_Z)
		return true;

	return false;
}

int numVertex;

INT_PTR CALLBACK IGameExporterOptionsDlgProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam) {
   IGameExporter *exp = DLGetWindowLongPtr<IGameExporter*>(hWnd); 
	ISpinnerControl * spin;
	int ID;
	
	switch(message) {
		case WM_INITDIALOG:
			exp = (IGameExporter *)lParam;
         DLSetWindowLongPtr(hWnd, lParam); 
			CenterWindow(hWnd,GetParent(hWnd));
			spin = GetISpinner(GetDlgItem(hWnd, IDC_STATIC_FRAME_SPIN)); 
			spin->LinkToEdit(GetDlgItem(hWnd,IDC_STATIC_FRAME), EDITTYPE_INT ); 
			spin->SetLimits(0, 100, TRUE); 
			spin->SetScale(1.0f);
			spin->SetValue(exp->staticFrame ,FALSE);
			ReleaseISpinner(spin);
						
			spin = GetISpinner(GetDlgItem(hWnd, IDC_SAMPLE_FRAME_SPIN)); 
			spin->LinkToEdit(GetDlgItem(hWnd,IDC_SAMPLE_FRAME), EDITTYPE_INT ); 
			spin->SetLimits(1, 100, TRUE); 
			spin->SetScale(1.0f);
			spin->SetValue(exp->framePerSample ,FALSE);
			ReleaseISpinner(spin);
			CheckDlgButton(hWnd,IDC_EXP_GEOMETRY,exp->exportGeom);
			CheckDlgButton(hWnd,IDC_EXP_NORMALS,exp->exportNormals);
			CheckDlgButton(hWnd,IDC_EXP_CONTROLLERS,exp->exportControllers);
			CheckDlgButton(hWnd,IDC_EXP_FACESMGRP,exp->exportFaceSmgp);
			CheckDlgButton(hWnd,IDC_EXP_VCOLORS,exp->exportVertexColor);
			CheckDlgButton(hWnd,IDC_EXP_TEXCOORD,exp->exportTexCoords);
			CheckDlgButton(hWnd,IDC_EXP_MAPCHAN,exp->exportMappingChannel);
			CheckDlgButton(hWnd,IDC_EXP_MATERIAL,exp->exportMaterials);
			CheckDlgButton(hWnd,IDC_EXP_SPLINES,exp->exportSplines);
			CheckDlgButton(hWnd,IDC_EXP_MODIFIERS,exp->exportModifiers);
			CheckDlgButton(hWnd,IDC_EXP_SAMPLECONT,exp->forceSample);
			CheckDlgButton(hWnd,IDC_EXP_CONSTRAINTS,exp->exportConstraints);
			CheckDlgButton(hWnd,IDC_EXP_SKIN,exp->exportSkin);
			CheckDlgButton(hWnd,IDC_EXP_GENMOD,exp->exportGenMod);
			CheckDlgButton(hWnd,IDC_SPLITFILE,exp->splitFile);
			CheckDlgButton(hWnd,IDC_EXP_OBJECTSPACE,exp->exportObjectSpace);
			CheckDlgButton(hWnd,IDC_EXP_QUATERNIONS,exp->exportQuaternions);
			CheckDlgButton(hWnd,IDC_EXP_RELATIVE,exp->exportRelative);
			

			ID = IDC_COORD_MAX + exp->cS;
			CheckRadioButton(hWnd,IDC_COORD_MAX,IDC_COORD_OGL,ID);

			ID = IDC_NORMALS_LIST + exp->exportNormalsPerFace;
			CheckRadioButton(hWnd,IDC_NORMALS_LIST,IDC_NORMALS_FACE,ID);

			EnableWindow(GetDlgItem(hWnd, IDC_EXP_NORMALS), exp->exportGeom);
			EnableWindow(GetDlgItem(hWnd, IDC_EXP_FACESMGRP), exp->exportGeom);
			EnableWindow(GetDlgItem(hWnd, IDC_EXP_VCOLORS),  exp->exportGeom);
			EnableWindow(GetDlgItem(hWnd, IDC_EXP_TEXCOORD),  exp->exportGeom);
			EnableWindow(GetDlgItem(hWnd, IDC_EXP_MAPCHAN),  exp->exportGeom);
			EnableWindow(GetDlgItem(hWnd, IDC_EXP_OBJECTSPACE),  exp->exportGeom);
			
			EnableWindow(GetDlgItem(hWnd, IDC_EXP_CONSTRAINTS),  exp->exportControllers);
			EnableWindow(GetDlgItem(hWnd, IDC_EXP_SAMPLECONT),  exp->exportControllers);
			EnableWindow(GetDlgItem(hWnd, IDC_EXP_QUATERNIONS), exp->exportControllers);
			EnableWindow(GetDlgItem(hWnd, IDC_EXP_RELATIVE), exp->exportControllers);
	
			EnableWindow(GetDlgItem(hWnd, IDC_EXP_SKIN),  exp->exportModifiers);
			EnableWindow(GetDlgItem(hWnd, IDC_EXP_GENMOD),  exp->exportModifiers);

			EnableWindow(GetDlgItem(hWnd, IDC_NORMALS_LIST),exp->exportNormals);
			EnableWindow(GetDlgItem(hWnd, IDC_NORMALS_FACE),exp->exportNormals);

			//Versioning
			TCHAR Title [256];
            _stprintf(Title,"IGame Exporter version %.3f; IGame version %.3f",
				exp->exporterVersion, exp->igameVersion);
			SetWindowText(hWnd,Title);
			return TRUE;

		case WM_COMMAND:
			switch (LOWORD(wParam)) {

				case IDC_EXP_GEOMETRY:
					EnableWindow(GetDlgItem(hWnd, IDC_EXP_NORMALS), IsDlgButtonChecked(hWnd, IDC_EXP_GEOMETRY));
					EnableWindow(GetDlgItem(hWnd, IDC_EXP_FACESMGRP), IsDlgButtonChecked(hWnd, IDC_EXP_GEOMETRY));
					EnableWindow(GetDlgItem(hWnd, IDC_EXP_VCOLORS),  IsDlgButtonChecked(hWnd, IDC_EXP_GEOMETRY));
					EnableWindow(GetDlgItem(hWnd, IDC_EXP_TEXCOORD),  IsDlgButtonChecked(hWnd, IDC_EXP_GEOMETRY));
					EnableWindow(GetDlgItem(hWnd, IDC_EXP_MAPCHAN),  IsDlgButtonChecked(hWnd, IDC_EXP_GEOMETRY));
					EnableWindow(GetDlgItem(hWnd, IDC_EXP_OBJECTSPACE),  IsDlgButtonChecked(hWnd, IDC_EXP_GEOMETRY));
					break;
				case IDC_EXP_CONTROLLERS:
					EnableWindow(GetDlgItem(hWnd, IDC_EXP_CONSTRAINTS), IsDlgButtonChecked(hWnd, IDC_EXP_CONTROLLERS));
					EnableWindow(GetDlgItem(hWnd, IDC_EXP_SAMPLECONT),  IsDlgButtonChecked(hWnd, IDC_EXP_CONTROLLERS));
					EnableWindow(GetDlgItem(hWnd, IDC_EXP_QUATERNIONS), IsDlgButtonChecked(hWnd, IDC_EXP_CONTROLLERS));
					EnableWindow(GetDlgItem(hWnd, IDC_EXP_RELATIVE), IsDlgButtonChecked(hWnd, IDC_EXP_CONTROLLERS));
					break;
				case IDC_EXP_MODIFIERS:
					EnableWindow(GetDlgItem(hWnd, IDC_EXP_SKIN), IsDlgButtonChecked(hWnd, IDC_EXP_MODIFIERS));
					EnableWindow(GetDlgItem(hWnd, IDC_EXP_GENMOD),  IsDlgButtonChecked(hWnd, IDC_EXP_MODIFIERS));

					break;

				case IDC_EXP_NORMALS:
					if(exp->igameVersion >= 1.12)
					{
						EnableWindow(GetDlgItem(hWnd, IDC_NORMALS_FACE), IsDlgButtonChecked(hWnd, IDC_EXP_NORMALS));
					}
					EnableWindow(GetDlgItem(hWnd, IDC_NORMALS_LIST), IsDlgButtonChecked(hWnd, IDC_EXP_NORMALS));
					break;

				case IDOK:
					exp->exportGeom = IsDlgButtonChecked(hWnd, IDC_EXP_GEOMETRY);
					exp->exportNormals = IsDlgButtonChecked(hWnd, IDC_EXP_NORMALS);
					exp->exportControllers = IsDlgButtonChecked(hWnd, IDC_EXP_CONTROLLERS);
					exp->exportFaceSmgp = IsDlgButtonChecked(hWnd, IDC_EXP_FACESMGRP);
					exp->exportVertexColor = IsDlgButtonChecked(hWnd, IDC_EXP_VCOLORS);
					exp->exportTexCoords = IsDlgButtonChecked(hWnd, IDC_EXP_TEXCOORD);
					exp->exportMappingChannel = IsDlgButtonChecked(hWnd, IDC_EXP_MAPCHAN);
					exp->exportMaterials = IsDlgButtonChecked(hWnd, IDC_EXP_MATERIAL);
					exp->exportSplines = IsDlgButtonChecked(hWnd, IDC_EXP_SPLINES);
					exp->exportModifiers = IsDlgButtonChecked(hWnd, IDC_EXP_MODIFIERS);
					exp->forceSample = IsDlgButtonChecked(hWnd, IDC_EXP_SAMPLECONT);
					exp->exportConstraints = IsDlgButtonChecked(hWnd, IDC_EXP_CONSTRAINTS);
					exp->exportSkin = IsDlgButtonChecked(hWnd, IDC_EXP_SKIN);
					exp->exportGenMod = IsDlgButtonChecked(hWnd, IDC_EXP_GENMOD);
					exp->splitFile = IsDlgButtonChecked(hWnd,IDC_SPLITFILE);
					exp->exportQuaternions = IsDlgButtonChecked(hWnd,IDC_EXP_QUATERNIONS);
					exp->exportObjectSpace = IsDlgButtonChecked(hWnd,IDC_EXP_OBJECTSPACE);
					exp->exportRelative = IsDlgButtonChecked(hWnd,IDC_EXP_RELATIVE);
					if (IsDlgButtonChecked(hWnd, IDC_COORD_MAX))
						exp->cS = IGameConversionManager::IGAME_MAX;
					else if (IsDlgButtonChecked(hWnd, IDC_COORD_OGL))
						exp->cS = IGameConversionManager::IGAME_OGL;
					else
						exp->cS = IGameConversionManager::IGAME_D3D;


					exp->exportNormalsPerFace = (IsDlgButtonChecked(hWnd,IDC_NORMALS_LIST))? FALSE : TRUE ;

					spin = GetISpinner(GetDlgItem(hWnd, IDC_STATIC_FRAME_SPIN)); 
					exp->staticFrame = spin->GetIVal(); 
					ReleaseISpinner(spin);
					spin = GetISpinner(GetDlgItem(hWnd, IDC_SAMPLE_FRAME_SPIN)); 
					exp->framePerSample = spin->GetIVal(); 
					ReleaseISpinner(spin);
					EndDialog(hWnd, 1);
					break;
				case IDCANCEL:
					EndDialog(hWnd,0);
					break;
			}
		
		default:
			return FALSE;
	
	}
	return TRUE;
	
}

// Replace some characters we don't care for.
TCHAR *FixupName (TCHAR *buf)
{
	static TCHAR buffer[256];
	TCHAR* cPtr;

    _tcscpy(buffer, buf);
    cPtr = buffer;

    while(*cPtr) {
		if (*cPtr == '"') *cPtr = 39;	// Replace double-quote with single quote.
        else if (*cPtr <= 31) *cPtr = _T('_');	// Replace control characters with underscore
        cPtr++;
    }

	return buffer;
}


//--- IGameExporter -------------------------------------------------------
IGameExporter::IGameExporter()
{

	staticFrame = 0;
	framePerSample = 4;
	exportGeom = TRUE;
	exportNormals = TRUE;
	exportVertexColor = FALSE;
	exportControllers = FALSE;
	exportFaceSmgp = FALSE;
	exportTexCoords = TRUE;
	exportMappingChannel = FALSE;
	exportMaterials = TRUE;
	exportConstraints = FALSE;
	exportSplines = FALSE;
	exportModifiers = FALSE;
	forceSample = FALSE;
	exportSkin = TRUE;
	exportGenMod = FALSE;
	cS = 0;	//max default
	splitFile = TRUE;
	exportQuaternions = TRUE;
	exportObjectSpace = FALSE;
	exportRelative = FALSE;
	exportNormalsPerFace = FALSE;
	exporterVersion = 2.0f;
	
}

IGameExporter::~IGameExporter() 
{

}

int IGameExporter::ExtCount()
{
	//TODO: Returns the number of file name extensions supported by the plug-in.
	return 1;
}

const TCHAR *IGameExporter::Ext(int n)
{		
	//TODO: Return the 'i-th' file name extension (i.e. "3DS").
	return _T("gks");
}

const TCHAR *IGameExporter::LongDesc()
{
	//TODO: Return long ASCII description (i.e. "Targa 2.0 Image File")
	return _T("GameKnife Scene File");
}
	
const TCHAR *IGameExporter::ShortDesc() 
{			
	//TODO: Return short ASCII description (i.e. "Targa")
	return _T("gkScene");
}

const TCHAR *IGameExporter::AuthorName()
{			
	//TODO: Return ASCII Author name
	return _T("Yi Kaiming");
}

const TCHAR *IGameExporter::CopyrightMessage() 
{	
	// Return ASCII Copyright message
	return _T("");
}

const TCHAR *IGameExporter::OtherMessage1() 
{		
	//TODO: Return Other message #1 if any
	return _T("");
}

const TCHAR *IGameExporter::OtherMessage2() 
{		
	//TODO: Return other message #2 in any
	return _T("");
}

unsigned int IGameExporter::Version()
{				
	//TODO: Return Version number * 100 (i.e. v3.01 = 301)
	return exporterVersion*100;
}

void IGameExporter::ShowAbout(HWND hWnd)
{			
	// Optional
}

BOOL IGameExporter::SupportsOptions(int ext, DWORD options)
{
	// TODO Decide which options to support.  Simply return
	// true for each option supported by each Extension 
	// the exporter supports.

	return TRUE;
}


void IGameExporter::MakeSplitFilename(IGameNode * node, TSTR & buf)
{
	buf = splitPath;
	buf = buf + "\\" + fileName + "_" + node->GetName() + ".gks";
}

void IGameExporter::ExportSceneInfo()
{
	TSTR buf;

	struct tm *newtime;
	time_t aclock;

	time( &aclock );
	newtime = localtime(&aclock);

	TSTR today = _tasctime(newtime);	// The date string has a \n appended.
	today.remove(today.length()-1);		// Remove the \n

	// scene info node
	authorsceneInfo = createRapidXmlNode(_T("GameKnifeScene"));

	// create the rapid info node [12/4/2011 Kaiming]
	authorSceneNode = createRapidXmlNode(_T("SceneObjects"));

	buf.printf ("%.1f", exporterVersion);

	// data [12/4/2011 Kaiming]
	//authorSceneNode->AddAttribute(_T("Date"), today.data());
	




	authorsceneInfo->AddAttribute(_T("FileName"), const_cast<TCHAR*>(pIgame->GetSceneFileName()));

	buf.printf("%d",pIgame->GetSceneStartTime() / pIgame->GetSceneTicks());

	buf.printf("%d",pIgame->GetSceneEndTime()/pIgame->GetSceneTicks());

	buf.printf("%d",GetFrameRate());
	
	buf.printf("%d",pIgame->GetSceneTicks());

	if(cS == 0)
		buf = TSTR("3dsmax");
	if(cS == 1)
		buf = TSTR("directx");
	if(cS == 2)
		buf = TSTR("opengl");

	buf = (exportQuaternions)?TSTR("Quaternion"):TSTR ("AngleAxis");

	if (exportObjectSpace) buf = TSTR("true");
	else buf = TSTR ("false");
}

void IGameExporter::ExportChildNodeInfo(IGameNode * child, CObjectNode* parentAuthor)
{
	IGameKeyTab poskeys;
	IGameKeyTab rotkeys;
	IGameKeyTab scalekeys;
	TSTR buf,data;

	if(splitFile)
	{
// 		CoCreateInstance(CLSID_DOMDocument, NULL, CLSCTX_INPROC_SERVER,  IID_IXMLDOMDocument, (void**)&pSubDocMesh);
// 		pSubDocMesh->QueryInterface(IID_IXMLDOMNode, (void **)&pRootSubMesh);
//  		pSubMesh = ExportSceneInfo(pSubDocMesh, pRootSubMesh);
	}

	curNode ++;
	// 进度条控制 [12/21/2010 Kaiming-Desktop]
	buf = TSTR("Processing: ") + TSTR(child->GetName());
	GetCOREInterface()->ProgressUpdate((int)((float)curNode/pIgame->GetTotalNodeCount()*100.0f),FALSE,buf.data()); 

	// simple output [12/4/2011 Kaiming]
	parentAuthor->AddAttribute(_T("Name"), FixupName (child->GetName()));

		// 节点基本信息
		buf.printf("%d\n",child->GetNodeID());

		//parentAuthor->AddAttribute(_T("NodeID"), "0");

		IGameNode * p = child->GetNodeParent();
		if(p)
		{
			buf.printf("%d",p->GetNodeID());
		}

		// 变换信息
		DumpMatrix(child->GetLocalTM(staticFrame).ExtractMatrix3(), parentAuthor);


	// 如果是组节点，以后可以考虑使用节点形式代替组，不要组了
// 	if(child->IsGroupOwner())
// 	{
// 		AddXMLAttribute(parent,_T("SuperClass"), _T("Group"));
// 
// 		buf.printf("%d",child->GetChildCount());
// 		AddXMLAttribute(parent,_T("NumChildren"),buf.data());
// 	}
// 	else
	{
		// 输出材质index和线框颜色
		//ULONG  handle = child->GetMaxNode()->GetHandle();

		if(child->GetMaterialIndex() !=-1){
			// 写材质名
			
			buf.printf("%s.mtl",child->GetNodeMaterial()->GetMaterialName());
			parentAuthor->AddAttribute(_T("MaterialName"),buf.data());
		}
		else
		{
			IPoint3 & color = child->GetWireframeColor();
			buf.printf ("%02x%02x%02xff", color.x, color.y, color.z);
		}

		// 转换成IGameObject详细判断
		IGameObject * obj = child->GetIGameObject();

		IGameObject::MaxType T = obj->GetMaxType();

//		if(obj->IsObjectXRef())
			

		switch(obj->GetIGameType())
		{
			case IGameObject::IGAME_LIGHT:
			{
				// TODO: 以后实现灯光的输出

				//AddXMLAttribute(parent,_T("SuperClass"),_T("Light"));
				parentAuthor->AddAttribute(_T("SuperClass"),_T("Light"));
 				break;
			}
			case IGameObject::IGAME_CAMERA:
			{
				// TODO: 摄像机也直接导出？
// 				AddXMLAttribute(parent,_T("SuperClass"),_T("Camera"));
// 
// 				CComPtr <IXMLDOMNode> camNode;
// 				CreateXMLNode(pXMLDoc,parent,_T("Camera"),&camNode);
// 				DumpCamera((IGameCamera*)obj,camNode);
				//AddXMLAttribute(parent,_T("SuperClass"),_T("Camera"));
				parentAuthor->AddAttribute(_T("SuperClass"),_T("Camera"));
 				break;
			}
			case IGameObject::IGAME_UNKNOWN:
				{
					// TODO: 摄像机也直接导出？
					// 				AddXMLAttribute(parent,_T("SuperClass"),_T("Camera"));
					// 
					// 				CComPtr <IXMLDOMNode> camNode;
					// 				CreateXMLNode(pXMLDoc,parent,_T("Camera"),&camNode);
					// 				DumpCamera((IGameCamera*)obj,camNode);
					//AddXMLAttribute(parent,_T("SuperClass"),_T("Unknown"));
					parentAuthor->AddAttribute(_T("SuperClass"),_T("Unknown"));
					break;
				}
			// gkObject出现的地方
			case IGameObject::IGAME_MESH:
				if(exportGeom )
				{
					//AddXMLAttribute(parent,_T("SuperClass"),_T("Mesh"));
					parentAuthor->AddAttribute(_T("SuperClass"),_T("Mesh"));
					IGameMesh * gM = (IGameMesh*)obj;
//					CComPtr<IXMLDOMNode>subMeshNode;
					gM->SetCreateOptimizedNormalList();
					if(gM->InitializeData())
					{
						// tangent [12/19/2010 Kaiming-Desktop]
						gM->InitializeBinormalData();

						// 新建一个文件，并输出
// 						TSTR filename;
// 
// 						//fstream sfs;
// 						//sfs.open(TEST_FILE, ios_base::out);
// 						// 
// 						fstream sfs("c:\\test.gkm", ios_base::out);
// 
// 						// 看一下能不能转过来

// 
// 						freopen( "CONOUT$","w",stdout);
// 
// 						std::locale loc( "chs" );
// 						std::wcout.imbue( loc );
// 						std::wcout << _T("测试") << std::endl;
 						
						gkObjectInMax* pGkObject = NULL;
						// 看下能不能转成gkObject
						pGkObject = dynamic_cast<gkObjectInMax*>( obj->GetMaxObject() );
						if (pGkObject)
 						{
							TCHAR buffer[250];
							//std::wcout << _T("找到一个gkObject") <<std::endl;
							std::wcout << _T("找到gkObject:") << pGkObject->m_strMeshFile.c_str() <<std::endl;

							//AddXMLAttribute(parent,_T("MeshName"), pGkObject->m_strMeshFile.c_str());
							parentAuthor->AddAttribute(_T("MeshName"), pGkObject->m_strMeshFile.c_str());
							// 取得物理属性 [4/26/2011 Kaiming-Desktop]
							int bNoPhysX;
							pGkObject->pblock2->GetValue(phy_nophysx, 0, bNoPhysX, FOREVER);
// 							if (bNoPhysX)
// 								AddXMLAttribute(parent,_T("EnablePhysX"), _T("0"));
// 							else
// 								AddXMLAttribute(parent,_T("EnablePhysX"), _T("1"));	

							int physxType;
							pGkObject->pblock2->GetValue(phy_type, 0, physxType, FOREVER);

							sprintf_s(buffer, 250, "%d", physxType);
							//AddXMLAttribute(parent,_T("PhysXType"), buffer);

							int physxShape;
							pGkObject->pblock2->GetValue(phy_shape, 0, physxShape, FOREVER);

							sprintf_s(buffer, 250, "%d", physxShape);
							//AddXMLAttribute(parent,_T("PhysXShape"), buffer);
						
							//CreateXMLNode(pXMLDoc,parent,_T("Mesh"),&geomData);
							//AddXMLAttribute(geomData,_T("Filename"), pGkObject->m_strMeshFile.c_str());
// 							if (child->GetNodeMaterial() && child->GetNodeMaterial()->GetIGameFX())
// 							{
// 								CreateXMLNode(pXMLDoc,parent,_T("Material"),&matData);
// 								AddXMLAttribute(matData,_T("Filename"),child->GetNodeMaterial()->GetIGameFX()->GetEffectFileName());
// 							}
						}
						else
						{
							//AddXMLAttribute(parent,_T("MeshName"), _T("None"));
							parentAuthor->AddAttribute(_T("MeshName"), _T("None"));
							//AddXMLAttribute(geomData,_T("Error"),_T("NotGkMesh. WillSupportLater."));
						}
 						
						//CreateXMLNode(pXMLDoc,parent,_T("Mesh"),&geomData);
						//DumpMesh(gM,geomData);
					}
					else
					{
						//CreateXMLNode(pXMLDoc,parent,_T("GeomData"),&geomData);
						//AddXMLAttribute(geomData,_T("Error"),_T("BadData."));
						parentAuthor->AddAttribute(_T("Error"), _T("NoBadData."));
					}
				}
				break;	

			default:
				//AddXMLAttribute(parent,_T("SuperClass"),_T("Group"));
				parentAuthor->AddAttribute(_T("SuperClass"), _T("Group"));
				break;
		}

	}	
	
	child->ReleaseIGameObject();

	for(int i=0;i<child->GetChildCount();i++)
	{
		IGameNode * newchild = child->GetNodeChild(i);
		//CreateXMLNode(pXMLDoc,parent,_T("gkObject"),&cNode);

		// write author nodes [12/4/2011 Kaiming]
		CObjectNode* authorNode = createObjectNode( parentAuthor );

		// we deal with targets in the light/camera section
		if(newchild->IsTarget())
			continue;

		ExportChildNodeInfo(newchild, authorNode);
	}
}

void IGameExporter::DumpMatrix(Matrix3 tm, CObjectNode* node)
{
	//CComPtr <IXMLDOMNode> tmNode;
	AffineParts ap;
// 	float rotAngle;
// 	Point3 rotAxis;
	float scaleAxAngle;
	Point3 scaleAxis;
	Matrix3 m = tm;
	//TSTR Buf;
	
	decomp_affine(m, &ap);

	//Buf.printf("%f %f %f",ap.t.x,ap.t.y,ap.t.z);
	//AddXMLText(pXMLDoc,tmNode,Buf.data());
	//tmNode = NULL;

	node->SetTranslation(ap.t.x, ap.t.y, ap.t.z);

	// D3D, quat reverse;
	//Buf.printf("%f %f %f %f",-ap.q.x, -ap.q.y, -ap.q.z, ap.q.w);
	node->SetRotation(-ap.q.x, -ap.q.y, -ap.q.z, ap.q.w);

	//tmNode = NULL;

	//Buf.printf ("%f %f %f %f %f %f %f", ap.k.x, ap.k.y, ap.k.z, ap.u.x, ap.u.y, ap.u.z, ap.u.w);
	node->SetScale(ap.k.x, ap.k.y, ap.k.z);
}

void GetKeyTypeName(TSTR &name, IGameControlType Type)
{
	if(Type==IGAME_POS)
		name.printf("Position");
	else if(Type==IGAME_ROT)
		name.printf("Rotation");
	else if(Type==IGAME_POINT3)
		name.printf("Point3");
	else if(Type==IGAME_FLOAT)
		name.printf("float");
	else if(Type==IGAME_SCALE)
		name.printf("Scale");
	else if(Type == IGAME_TM)
		name.printf("NodeTM");
	else if(Type == IGAME_EULER_X)
		name.printf("EulerX");
	else if(Type == IGAME_EULER_Y)
		name.printf("EulerY");
	else if(Type == IGAME_EULER_Z)
		name.printf("EulerZ");
	else if(Type == IGAME_POS_X)
		name.printf("IndePosX");
	else if(Type == IGAME_POS_Y)
		name.printf("IndePosY");
	else if(Type == IGAME_POS_Z)
		name.printf("IndePosZ");
	else
		name.printf("Huh!!");
}

void GetConstraintTypeName(TSTR &name, IGameConstraint::ConstraintType Type)
{
	if(Type == IGameConstraint::IGAME_PATH)
		name.printf("Path");
	else if(Type == IGameConstraint::IGAME_POSITION)
		name.printf("Position");
	else if(Type == IGameConstraint::IGAME_ORIENTATION)
		name.printf("Orientation");
	else if (Type == IGameConstraint::IGAME_LOOKAT)
		name.printf("lookAt");
	else
		name.printf("Huh!!");
}

void IGameExporter::DumpLight(IGameLight *lt)
{
	if(lt->GetLightType()==TSPOT_LIGHT )
	{
		//TSTR buf;
		//buf.printf("%d",lt->GetLightTarget()->GetNodeID());
		//AddXMLAttribute(parent,_T("TargetID"),buf.data());
	}

// 	DumpNamedProperty(parent, _T("Color"), lt->GetLightColor());
// 	DumpNamedProperty(parent, _T("Multiplier"), lt->GetLightMultiplier());
// 	DumpNamedProperty(parent, _T("AspectRatio"), lt->GetLightAspectRatio());
// 	DumpNamedProperty(parent, _T("Start"), lt->GetLightAttenStart());
// 	DumpNamedProperty(parent, _T("End"), lt->GetLightAttenEnd());
// 	DumpNamedProperty(parent, _T("FallOff"), lt->GetLightFallOff());
}

void IGameExporter::DumpCamera(IGameCamera *ca)
{
	if(ca->GetCameraTarget())
	{
		//TSTR buf;
		//buf.printf("%d",ca->GetCameraTarget()->GetNodeID());
		//AddXMLAttribute(parent,_T("TargetID"),buf.data());
	}
	
// 	DumpNamedProperty( parent, _T("FOV"), ca->GetCameraFOV());
// 	DumpNamedProperty( parent, _T("FarClip"), ca->GetCameraFarClip());
// 	DumpNamedProperty( parent, _T("NearClip"), ca->GetCameraNearClip());
// 	DumpNamedProperty( parent, _T("TargetDistance"), ca->GetCameraTargetDist());
}



#define GET_INDEX_VAL( _v, _i) ((_i==0)?_v.x:((_i==1)?_v.y:_v.z))

void IGameExporter::DumpProperty(IGameProperty * prop)
{
	TSTR Buf;
	IGameKeyTab keys;

	if(!prop)	//fix me NH...
		return;

	if(prop->GetType() == IGAME_POINT3_PROP)
	{
		Point3 p; 
		prop->GetPropertyValue(p);
		Buf.printf("%f %f %f",p.x,p.y,p.z);
	}
	else if( prop->GetType() == IGAME_FLOAT_PROP)
	{
		float f;
		prop->GetPropertyValue(f);
		Buf.printf("%f", f);
	}
	else if(prop->GetType()==IGAME_STRING_PROP)
	{

#if (MAX_PRODUCT_VERSION_MAJOR >= 14)
		const MCHAR* b;
#else
		MCHAR* b;
#endif
		prop->GetPropertyValue(b);
		Buf.printf("$s",b);

	}
	else
	{
		int i;
		prop->GetPropertyValue(i);
		Buf.printf("%d", i);

	}
	//AddXMLAttribute(propData,_T("Value"),Buf.data());

// 	if(prop->IsPropAnimated() && exportControllers)
// 	{
// 		IGameControl * c = prop->GetIGameControl();
// 
// 
// 		if(prop->GetType() == IGAME_POINT3_PROP)
// 		{
// 			if(	c->GetBezierKeys(keys,IGAME_POINT3 )){
// 				Point3 p; 
// 				prop->GetPropertyValue(p);
// 				DumpBezierPosKeys(IGAME_POINT3,keys,p,propData);
// 			}
// 		}
// 		if(prop->GetType()==IGAME_FLOAT_PROP)
// 		{
// 			if(	c->GetBezierKeys(keys,IGAME_FLOAT ))
// 				DumpBezierFloatKeys(IGAME_FLOAT, keys, 0, propData);
// 			}
// 		}
// 
// 	}
}

void IGameExporter::DumpNamedProperty(TCHAR* propName, IGameProperty * prop)
{
	if(!prop)	//fix me NH...
		return;
	//CComPtr <IXMLDOMNode> propData;
	//CreateXMLNode(pXMLDoc, node, propName,&propData);
// 	DumpProperty( prop, propData);
// 	propData = NULL;
}	

void IGameExporter::DumpProperties(IGameProperty * prop)
{
	//CComPtr <IXMLDOMNode> propData;

	if(!prop)	//fix me NH...
		return;
	//CreateXMLNode(pXMLDoc,node,_T("Prop"),&propData);
	//AddXMLAttribute(propData,_T("name"),prop->GetName());
// 	DumpProperty(prop, propData);
// 	propData = NULL;
}	

void IGameExporter::DumpFXProperties(IParamBlock2* fxParamBlock, CRapidXmlAuthorNode* parentNode)
{
	int nProp = fxParamBlock->NumParams();
	for (int fxi = 0; fxi < nProp; ++fxi) {
		std::wcout << _T("参数") <<fxi<< std::endl;
		int pid = fxParamBlock->IndextoID(fxi);
		ParamDef pdef = fxParamBlock->GetParamDef(pid);

		std::string typeName;
		std::string paramName = pdef.int_name;
		std::string strdata;
		float fdata[4];
		int ndata;
		TCHAR * str;
		int wrFloat = 0;
		bool wrDword = false;
		bool wrString = false;
		bool isPath = false;
		TSTR buf;
		switch (pdef.type) {
			  case TYPE_FLOAT:
			  case TYPE_PCNT_FRAC:
				  std::wcout << _T("TYPE_FLOAT: ") << pdef.int_name << std::endl;

				  typeName = "float";
				  fdata[0] = fxParamBlock->GetFloat(pid);
				  // 格式输出
				  buf.printf(_T("%f"), fdata[0]);

				  wrFloat = 1;
				  break;
			  case TYPE_BOOL:
			  case TYPE_INT:
				  std::wcout << _T("TYPE_INT") << pdef.int_name << std::endl;

				  typeName = "int";
				  ndata = fxParamBlock->GetInt(pid);
				  buf.printf(_T("%d"), ndata);
				  wrDword = true;
				  break;
			  case TYPE_POINT2:
				  std::wcout << _T("TYPE_POINT2") << pdef.int_name << std::endl;

				  typeName = "float2";
				  (Point3 &)fdata[0] = fxParamBlock->GetPoint3(pid);
				  buf.printf(_T("%f %f"), fdata[0], fdata[1]);
				  wrFloat = 2;
				  break;

			  case TYPE_POINT3:
				  std::wcout << _T("TYPE_POINT3") << pdef.int_name << std::endl;

				  typeName = "float3";
				  (Point3 &)fdata[0] = fxParamBlock->GetPoint3(pid);
				  buf.printf(_T("%f %f %f"), fdata[0], fdata[1], fdata[2]);
				  wrFloat = 3;
				  break;
			  case TYPE_POINT4:
				  std::wcout << _T("TYPE_POINT4") << pdef.int_name <<  std::endl;

				  typeName = "float4";
				  (Point4 &)fdata[0] = fxParamBlock->GetPoint4(pid);
				  buf.printf(_T("%f %f %f %f"), fdata[0], fdata[1], fdata[2], fdata[3]);
				  wrFloat = 4;
				  break;
			  case TYPE_FRGBA:
				  std::wcout << _T("TYPE_FRGBA") << pdef.int_name <<  std::endl;

				  typeName = "float4";
				  (Point4 &)fdata[0] = fxParamBlock->GetAColor(pid);
				  buf.printf(_T("%f %f %f %f"), fdata[0], fdata[1], fdata[2], fdata[3]);
				  wrFloat = 4;
				  break;
			  case TYPE_STRING:
			  case TYPE_FILENAME:
				  std::wcout << _T("TYPE_FILENAME") << pdef.int_name <<  std::endl;

				  typeName = "string";
				  strdata = fxParamBlock->GetStr(pid);
				  buf.printf("%s", strdata.c_str());
				  wrString = (strdata.size() > 0);
				  break;
			  case TYPE_BITMAP:
				  {
					  std::wcout << _T("TYPE_BITMAP") << pdef.int_name<< std::endl;

					  typeName = "texture";

					  PBBitmap * pbb = fxParamBlock->GetBitmap(pid);
					  if (pbb) {
						  strdata = pbb->bi.Filename();
						  buf.printf("%s", strdata.c_str());
					  }
					  wrString = (strdata.size() > 0) && (strdata!= "None");
				  }
				  break;
// 			  case TYPE_TEXMAP:
// 				  {
// 					  std::wcout << _T("TYPE_TEXMAP") << std::endl;
// 
// 					  typeName = "texcoord";
// 					  isPath = true;
// 					  Texmap * tmp = fxParamBlock->GetTexmap(pid);
// 					  BitmapTex * bmt = 0;
// 					  if (tmp && tmp->ClassID().PartA() == BMTEX_CLASS_ID) {
// 						  bmt = static_cast<BitmapTex *>(tmp);
// 					  }
// 					  if (bmt) {
// 						  val = (str = bmt->GetMapName());
// 						  wrString = val.size() > 0;
// 					  }
// 				  }
// 				  break;
			  case TYPE_MATRIX3:
				  // 矩阵，不知道以后会不会添加支持
				  // 先不管了
			  default:
				  std::wcout << _T("发现不识别参数") << std::endl;

				  typeName = "invalid";
				  //LogPrint("Skipping unknown FX parameter named '%s' (id %d, type %d).\n", name.c_str(), pid, pdef.type);
				  break;
		}
		if (wrFloat || wrDword || wrString) {
			// 写入节点
			// rapidxml new way [12/4/2011 Kaiming]
			CRapidXmlAuthorNode* paramNode = parentNode->createChildNode(_T("Param"));
			paramNode->AddAttribute( _T("name"), paramName.c_str());
			paramNode->AddAttribute( _T("type"), typeName.c_str());
			paramNode->AddAttribute( _T("value"), buf.data());
			 
		}
	}
}



static void GetProfile(IGameFX::IGameFXProfile target, bool vertexprog, TCHAR * profile)
{
//	TCHAR buf [20];

	switch (target)
	{
	case IGameFX::kVSPS_20:
		if(vertexprog)
			_tcscpy(profile,_T("VS_2_0"));
		else
			_tcscpy(profile,_T("PS_2_0"));
		break;
	case IGameFX::kVSPS_2X:
		if(vertexprog)
			_tcscpy(profile,_T("vs_2_a"));
		else
			_tcscpy(profile,_T("ps_2_a"));
		break;
	case IGameFX::kVSPS_30:
		if(vertexprog)
			_tcscpy(profile,_T("vs_3_0"));
		else
			_tcscpy(profile,_T("ps_3_0"));
		break;

	default:
		if(vertexprog)
			_tcscpy(profile,_T("vs_2_0"));
		else
			_tcscpy(profile,_T("ps_2_0"));
		break;
	}

}

//////////////////////////////////////////////////////////////////////////
void IGameExporter::DumpMaterial( IGameMaterial * mat, CRapidXmlAuthorNode* rootMatNode )
{
	TSTR buf;
	rootMatNode->AddAttribute(_T("Name"),  FixupName (mat->GetMaterialName()));

	// 直接输出fx信息
	IGameFX * gameFX = mat->GetIGameFX();
	if(gameFX)
	{
		IParamBlock2 *fxpb = mat->GetMaxMaterial()->GetParamBlock(0);
		if (fxpb)
		{
			// Initialize Effect Node
			CRapidXmlAuthorNode* effectMatNode = rootMatNode->createChildNode(_T("Effect"));


			// 属性1:shader文件名
			// 砍一下路径
			CHAR strFileName[MAX_PATH];

#if (MAX_PRODUCT_VERSION_MAJOR >= 14)
			strcpy_s( strFileName, MAX_PATH, gameFX->GetEffectFile().GetFileName() );
#else
			strcpy_s( strFileName, MAX_PATH, gameFX->GetEffectFileName() );
#endif

			// 砍掉路径，保留文件名
			CHAR strPureName[MAX_PATH] = {0};
			CHAR* strLastSlash = NULL;
			strLastSlash = strrchr( strFileName, '\\' );
			if( strLastSlash )
			{
				strcpy_s( strPureName, MAX_PATH, &strLastSlash[1] );
			}

			buf.printf("%s", strPureName);
			effectMatNode->AddAttribute(_T("FileName"),  buf.data());
			// 属性2:参数计数
			buf.printf("%d",fxpb->NumParams());
			effectMatNode->AddAttribute(_T("ParamCount"),  buf.data());

			std::wcout << _T("开始导出FX参数...") << std::endl;
			DumpFXProperties(fxpb, effectMatNode);
		}
		else
		{
			std::wcout << _T("paramblock查找失败") << std::endl;
		}


		CRapidXmlAuthorNode* staticNode = rootMatNode->createChildNode(_T("Static"));
		CRapidXmlAuthorNode* sttParam = staticNode->createChildNode(_T("Param"));
		sttParam->AddAttribute(_T("name"), "uvparams");
		sttParam->AddAttribute(_T("type"), "external");
		sttParam->AddAttribute(_T("value"), "1 1 0 0");

	}


}


void IGameExporter::AuthorMaterial( Mtl * mat, LPCTSTR filename )
{
	pIgame = GetIGameInterface();

	pIgame->InitialiseIGame(false);
	pIgame->SetStaticFrame(0);

	AuthorMaterial( pIgame->GetIGameMaterial(  mat ), 0, filename);

	pIgame->ReleaseIGame();
}

void IGameExporter::AuthorMaterial(IGameMaterial * mat, int index, LPCTSTR filename )
{

	TSTR buf;
	TCHAR profile[20];
	IGameProperty *prop;

	// create a new doc to write individual matfile
	IRapidXmlAuthor matAuthor;
	matAuthor.initializeSceneWrite();

	// Initialize
	CRapidXmlAuthorNode* rootMatNode = matAuthor.createRapidXmlNode(_T("Material"));
	DumpMaterial(mat, rootMatNode);

	// here we use max mtl's sub structre
	Mtl* maxmat = mat->GetMaxMaterial();
	int subnum = maxmat->NumSubMtls();
	for( int i =0; i< subnum; ++i)
	{
		CRapidXmlAuthorNode* subMatNode = rootMatNode->createChildNode(_T("SubMaterial"));
		DumpMaterial(pIgame->GetIGameMaterial( maxmat->GetSubMtl( i )), subMatNode);
		
	}

	TCHAR matfilePath[MAX_PATH];
	if (filename)
	{
		_tcscpy( matfilePath, filename );
	}
	else
	{
		// write file [12/4/2011 Kaiming]
		getEngineRoot(matfilePath);
		_tcscat_s(matfilePath, _T("\\Media\\Material\\"));
		_tcscat_s(matfilePath, FixupName (mat->GetMaterialName()));
		_tcscat_s(matfilePath, _T(".mtl"));
	}
	





	matAuthor.writeToFile(matfilePath);

	std::wcout << _T("fx参数读取完毕") << std::endl;


	return;
}

void IGameExporter::ExportMaterials()
{
//	CComPtr <IXMLDOMNode> matNode;
	TSTR buf;
	if(exportMaterials)
	{
		int matCount = pIgame->GetRootMaterialCount();

		for(int j =0;j<matCount;j++)
			AuthorMaterial(pIgame->GetRootMaterial(j),j);
	}

	
}

// Dummy function for progress bar
DWORD WINAPI fn(LPVOID arg)
{
	return(0);
}



class MyErrorProc : public IGameErrorCallBack
{
public:
	void ErrorProc(IGameError error)
	{
		TCHAR * buf = GetLastIGameErrorText();
		DebugPrint(_T("ErrorCode = %d ErrorText = %s\n"), error,buf);
	}
};

#include "utilapi.h"
int	IGameExporter::DoExport(const TCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts, DWORD options)
{
	HRESULT hr;

	Interface * ip = GetCOREInterface();

	MyErrorProc pErrorProc;

	UserCoord Whacky = {
		1,	//Right Handed
		1,	//X axis goes right
		3,	//Y Axis goes down
		4,	//Z Axis goes in.
		1,	//U Tex axis is right
		1,  //V Tex axis is Down
	};	

	SetErrorCallBack(&pErrorProc);

	ReadConfig();
// 	bool succeeded = SUCCEEDED(init) &&SUCCEEDED(CoCreateInstance(CLSID_DOMDocument, NULL, CLSCTX_INPROC_SERVER,  IID_IXMLDOMDocument, (void**)&pXMLDoc));
// 	if(!succeeded)
// 		return false;
	
	// Check the return value, hr...
// 	hr = pXMLDoc->QueryInterface(IID_IXMLDOMNode, (void **)&pRoot);
// 	if(FAILED(hr))
// 		return false;

	// Set a global prompt display switch
	showPrompts = suppressPrompts ? FALSE : TRUE;

	exportSelected = (options & SCENE_EXPORT_SELECTED) ? true : false;
	igameVersion  = GetIGameVersion();

	DebugPrint(_T("3ds max compatible version %.2f\n"),GetSupported3DSVersion());

	if(showPrompts) 
	{
		// Prompt the user with our dialogbox, and get all the options.
		if (!DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_PANEL),
			i->GetMAXHWnd(), IGameExporterOptionsDlgProc, (LPARAM)this)) {
			return 1;
		}
	}

	curNode = 0;
	ip->ProgressStart(_T("Exporting Using IGame.."), TRUE, fn, NULL);
	//AllocConsole();
	
	pIgame = GetIGameInterface();

	IGameConversionManager * cm = GetConversionManager();
//	cm->SetUserCoordSystem(Whacky);

	// set as 3dxmax anyway [12/3/2011 Kaiming]
	cm->SetCoordSystem(IGameConversionManager::IGAME_MAX);
	//cm->SetCoordSystem((IGameConversionManager::CoordSystem)cS);
//	pIgame->SetPropertyFile(_T("hello world"));


	// IGAME initialized
	pIgame->InitialiseIGame(exportSelected);
	pIgame->SetStaticFrame(staticFrame);
	
	TSTR path,file,ext;

	// backup path for sync [9/4/2011 Kaiming-Desktop]
	gUtilEnv->SyncSceneName = name;
	SplitFilename(TSTR(name),&path,&file,&ext);

	fileName = file;
	splitPath = path;


	// start printing [12/4/2011 Kaiming]
	initializeSceneWrite();
	
	ExportSceneInfo();
	ExportMaterials();

	authorSceneNode->AddAttribute("TopLevelCount", pIgame->GetTopLevelNodeCount());
	authorSceneNode->AddAttribute("AllCount", pIgame->GetTotalNodeCount());

	for(int loop = 0; loop <pIgame->GetTopLevelNodeCount();loop++)
	{
		IGameNode * pGameNode = pIgame->GetTopLevelNode(loop);
		//check for selected state - we deal with targets in the light/camera section
		if(pGameNode->IsTarget())
			continue;

		// 第一层，那就是root嘛 [12/21/2010 Kaiming-Desktop]

		CObjectNode* thisnode = createObjectNode(authorSceneNode);

		ExportChildNodeInfo(pGameNode,thisnode);


		
	}
	pIgame->ReleaseIGame();
	//PrettyPrint(name, pXMLDoc);

	// writefile [12/4/2011 Kaiming]
	writeToFile(name);

	ip->ProgressEnd();	
	WriteConfig();
	return TRUE;
}




TSTR IGameExporter::GetCfgFilename()
{
	TSTR filename;
	
	filename += GetCOREInterface()->GetDir(APP_PLUGCFG_DIR);
	filename += "\\";
	filename += "IgameExport.cfg";
	return filename;
}

// NOTE: Update anytime the CFG file changes
#define CFG_VERSION 0x03

BOOL IGameExporter::ReadConfig()
{
	TSTR filename = GetCfgFilename();
	FILE* cfgStream;

	cfgStream = fopen(filename, "rb");
	if (!cfgStream)
		return FALSE;
	
	exportGeom = fgetc(cfgStream);
	exportNormals = fgetc(cfgStream);
	exportControllers = fgetc(cfgStream);
	exportFaceSmgp = fgetc(cfgStream);
	exportVertexColor = fgetc(cfgStream);
	exportTexCoords = fgetc(cfgStream);
	staticFrame = _getw(cfgStream);
	framePerSample = _getw(cfgStream);
	exportMappingChannel = fgetc(cfgStream);
	exportMaterials = fgetc(cfgStream);
	exportSplines = fgetc(cfgStream);
	exportModifiers = fgetc(cfgStream);
	forceSample = fgetc(cfgStream);
	exportConstraints = fgetc(cfgStream);
	exportSkin = fgetc(cfgStream);
	exportGenMod = fgetc(cfgStream);
	cS = fgetc(cfgStream);
	splitFile = fgetc(cfgStream);
	exportQuaternions = fgetc(cfgStream);
	exportObjectSpace = fgetc(cfgStream);
	exportRelative = fgetc(cfgStream);
	exportNormalsPerFace = fgetc(cfgStream);
	fclose(cfgStream);
	return TRUE;
}

void IGameExporter::WriteConfig()
{
	TSTR filename = GetCfgFilename();
	FILE* cfgStream;

	cfgStream = fopen(filename, "wb");
	if (!cfgStream)
		return;

	
	fputc(exportGeom,cfgStream);
	fputc(exportNormals,cfgStream);
	fputc(exportControllers,cfgStream);
	fputc(exportFaceSmgp,cfgStream);
	fputc(exportVertexColor,cfgStream);
	fputc(exportTexCoords,cfgStream);
	_putw(staticFrame,cfgStream);
	_putw(framePerSample,cfgStream);
	fputc(exportMappingChannel,cfgStream);
	fputc(exportMaterials,cfgStream);
	fputc(exportSplines,cfgStream);
	fputc(exportModifiers,cfgStream);
	fputc(forceSample,cfgStream);
	fputc(exportConstraints,cfgStream);
	fputc(exportSkin,cfgStream);
	fputc(exportGenMod,cfgStream);
	fputc(cS,cfgStream);
	fputc(splitFile,cfgStream);
	fputc(exportQuaternions,cfgStream);
	fputc(exportObjectSpace,cfgStream);
	fputc(exportRelative,cfgStream);
	fputc(exportNormalsPerFace, cfgStream);
	fclose(cfgStream);
}


void IGameExporter::makeValidURIFilename(TSTR& fn, bool stripMapPaths)
{
	// massage external filenames into valid URI: strip any prefix matching any declared map path,
	//
	/*map \ -> /, sp -> _, : -> $
	if (stripMapPaths) {
		int matchLen = 0, matchI;
		for (int i = 0; i < TheManager->GetMapDirCount(); i++) {
			TSTR dir = TheManager->GetMapDir(i);
			if (MatchPattern(fn, dir + TSTR("*"))) {
				if (dir.length() > matchLen) {
					matchLen = dir.length();
					matchI = i;
				}
			}
		}
		if (matchLen > 0) {
			// found map path prefix, strip it 
			TSTR dir = TheManager->GetMapDir(matchI);
			fn.remove(0, dir.length());
			if (fn[0] = _T('\\')) fn.remove(0, 1); // strip any dangling path-sep
		}
	}
	*/

	// map funny chars
	for (int i = 0; i < fn.length(); i++) {
		if (fn[i] == _T(':')) fn[i] = _T('$');
		else if (fn[i] == _T(' ')) fn[i] = _T('_');
		else if (fn[i] == _T('\\')) fn[i] = _T('/');
	}
}


