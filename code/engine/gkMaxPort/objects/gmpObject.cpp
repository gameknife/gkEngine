#include "gmpObject.h"
//#include "exporter/gmpMeshLoader.h"
//#include "exporter/gmpExporter.h"


#include "gkStudioWndMsg.h"
//#include "IResourceManager.h"

// in prim.cpp  - The dll instance handle
extern HINSTANCE hInstance;

//--- ClassDescriptor and class vars ---------------------------------

static BOOL sInterfaceAdded = FALSE;

extern gkObjectInMax* g_gkObjInMax = NULL;

// From GeomObject
int gkObjectInMax::IntersectRay(
	TimeValue t, Ray& ray, float& at, Point3& norm)
{
	//SS 9/29/2000 KENNY MERGE: Looks as though the simple calculation below;
	// based solely on the radius, doesn't take into account changes to the
	// hemisphere or base pivot parameters. This seems like a general fix,
	// so it's being rolled into Magma.
	int smooth, hemi, basePivot;
	pblock2->GetValue(geo_smooth, t, smooth, FOREVER);
	//pblock2->GetValue(geo_hemi, t, hemi, FOREVER);
	//pblock2->GetValue(geo_basepivot, t, basePivot, FOREVER);
	if (!smooth || hemi || basePivot) {
		//if (!smooth) {
		return SimpleObject::IntersectRay(t, ray, at, norm);
	}	

	float r;
	float a, b, c, ac4, b2, at1, at2;
	float root;
	BOOL neg1, neg2;

	//pblock2->GetValue(geo_radius, t, r, FOREVER);

	a = DotProd(ray.dir, ray.dir);
	b = DotProd(ray.dir, ray.p) * 2.0f;
	c = DotProd(ray.p, ray.p) - r*r;

	ac4 = 4.0f * a * c;
	b2 = b*b;

	if (ac4 > b2) return 0;

	// We want the smallest positive root
	root = Sqrt(b2-ac4);
	at1 = (-b + root) / (2.0f * a);
	at2 = (-b - root) / (2.0f * a);
	neg1 = at1<0.0f;
	neg2 = at2<0.0f;
	if (neg1 && neg2) return 0;
	else
		if (neg1 && !neg2) at = at2;
		else 
			if (!neg1 && neg2) at = at1;
			else
				if (at1<at2) at = at1;
				else at = at2;

				norm = Normalize(ray.p + at*ray.dir);

				return 1;
}

#include "exporter/gmpMeshLoader.h"
#include "exporter/gmpExporter.h"


// The class descriptor for gkObjectInMax
class gkObjectInMaxClassDesc: public ClassDesc2 
{
	public:
	int 			IsPublic() { return 1; }
	void *			Create(BOOL loading = FALSE)
    {
#ifdef PHYSICAL_SCALE_UVS
		if (!sInterfaceAdded) {
			AddInterface(&gRealWorldMapSizeDesc);
			sInterfaceAdded = TRUE;
		}
#endif

		g_gkObjInMax = new gkObjectInMax(loading);
        return g_gkObjInMax;
    }
	const TCHAR *	ClassName() { return GetString(IDS_GEOSPHERE_CLASS); }
	SClass_ID		SuperClassID() { return GEOMOBJECT_CLASS_ID; }
	Class_ID		ClassID() { return Class_ID(GEOSPHERE_CLASS_ID, 32670); }
	const TCHAR* 	Category() { return GetString(IDS_RB_PRIMITIVES); }
// JBW:  the ResetClassParams() has gone since this is automatic now
//       using the default values in the descriptors

// JBW: new descriptor data accessors added.  Note that the 
//      internal name is hardwired since it must not be localized.
	const TCHAR*	InternalName() { return _T("GeoSphere"); }	// returns fixed parsable name (scripter-visible name)
	HINSTANCE		HInstance() { return hInstance; }			// returns owning module handle
};

static gkObjectInMaxClassDesc gkObjectInMaxDesc;
ClassDesc* GetgkObjectInMaxDesc() { return &gkObjectInMaxDesc; }

IObjParam* gkObjectInMax::ip = NULL;
BOOL gkObjectInMax::typeinCreate       = FALSE;

// JBW:  all the old static class variables have gone, most now hosted in 
//       static param blocks

// Misc stuff
#define MAX_SEGMENTS	200
#define MIN_SEGMENTS	1

#define MIN_RADIUS		float(0)
#define MAX_RADIUS		float(1.0E30)

#define DEF_SEGMENTS	4
#define DEF_RADIUS		float(0.0)
#define DEF_BASETYPE	2
#define DEF_SMOOTH		1
#define DEF_HEMI		0
#define DEF_BASEPIVOT	0
#define DEF_MAPPING		0

// JBW: Here follows the new parameter block descriptors.  There are now 3, 
//      two new STATIC ones to hold the old class var parameters, one for the main
//		per-instance parameters.  Apart from all the extra 
//      metadata you see in the definitions, one important new idea is the
//      folding of ParamMap description info into the parameter descriptor and
//      providing a semi-automatic rollout desipaly mechanism.
//      

// Parameter Block definitions

// JBW: First come the position and version independent IDs for each
//      of the blocks and the parameters in each block.  These IDs are used
//	    in subsequent Get/SetValue() parameter access, etc. and for version-independent
//      load and save



// JBW: here are the two static block descriptors.  This form of 
//      descriptor declaration uses a static NParamBlockDesc instance whose constructor
//      uses a varargs technique to walk through all the param definitions.
//      It has the advantage of supporting optional and variable type definitions, 
//      but may generate a tad more code than a simple struct template.  I'd
//      be interested in opinions about this.

//      I'll briefly describe the first definition so you can figure the others.  Note
//      that in certain places where strings are expected, you supply a string resource ID rather than
//      a string at it does the lookup for you as needed.
//
//		line 1: block ID, internal name, local (subanim) name, flags
//																 AUTO_UI here means the rollout will
//																 be automatically created (see BeginEditParams for details)
//      line 2: since AUTO_UI was set, this line gives: 
//				dialog resource ID, rollout title, flag test, appendRollout flags
//		line 3: required info for a parameter:
//				ID, internal name, type, flags, local (subanim) name
//		lines 4-6: optional parameter declaration info.  each line starts with a tag saying what
//              kind of spec it is, in this case default value, value range, and UI info as would
//              normally be in a ParamUIDesc less range & dimension
//	    the param lines are repeated as needed for the number of parameters defined.

// class type-in block
static ParamBlockDesc2 geo_typein_blk ( geo_type_in, _T("gkObjectTypeIn"),  0, &gkObjectInMaxDesc, P_CLASS_PARAMS + P_AUTO_UI, 
	//rollout
	IDD_GSPHERE3, IDS_KEYBOARD_ENTRY, BEGIN_EDIT_CREATE, DONTAUTOCLOSE, NULL,
	// params
	geo_ti_pos, 			_T("typeInPos"), 		TYPE_POINT3, 		0, 	IDS_RB_POS,
		p_default, 		Point3(0,0,0), 
		p_range, 		-99999999.0, 99999999.0, 
		p_ui, 			TYPE_SPINNER, EDITTYPE_UNIVERSE, IDC_TI_POSX, IDC_TI_POSXSPIN, IDC_TI_POSY, IDC_TI_POSYSPIN, IDC_TI_POSZ, IDC_TI_POSZSPIN, SPIN_AUTOSCALE, 
		end, 
	geo_ti_radius, 		_T("typeInRadius"), 		TYPE_FLOAT, 		0, 	IDS_RB_RADIUS, 
		p_default, 		25.0, 
		p_range, 		MIN_RADIUS, MAX_RADIUS, 
		p_ui, 			TYPE_SPINNER, EDITTYPE_UNIVERSE, IDC_RADIUS, IDC_RADSPINNER, SPIN_AUTOSCALE, 
		end, 
	end
	);

// JBW: this descriptor defines the main per-instance parameter block.  It is flagged as AUTO_CONSTRUCT which
//      means that the CreateInstance() will automatically create one of these blocks and set it to the reference
//      number given (0 in this case, as seen at the end of the line).

// per instance geosphere block
static ParamBlockDesc2 geo_param_blk ( geo_params, _T("gkObjectParameters"),  0, &gkObjectInMaxDesc, P_AUTO_CONSTRUCT + P_AUTO_UI, PBLOCK_REF_NO, 
	//rollout
	IDD_GSPHERE2, IDS_PARAMETERS, 0, 0, NULL,
	// params
	geo_meshpath, 	_T("meshpath"),		TYPE_STRING, 		0,				IDS_MESH_PATH,
		p_default, 		_T(""), 
		p_ui, 			TYPE_EDITBOX, 	IDC_MESHPATH,
		end, 

	geo_smooth, _T("smooth"), 		TYPE_BOOL, 		P_ANIMATABLE,				IDS_RB_SMOOTH,
		p_default, 		TRUE, 
		p_ui, 			TYPE_SINGLECHEKBOX, IDC_OBSMOOTH, 
		end, 

	phy_nophysx, _T("nophy"), 		TYPE_BOOL, 		0,				IDS_RB_NOPHY,
		p_default, 		FALSE, 
		p_ui, 			TYPE_SINGLECHEKBOX, IDC_OBSMOOTH2, 
		end, 

	phy_type, 	_T("physxType"),			TYPE_INT, 		0,				IDS_RB_DYNPHY,
		p_default, 		0, 
		p_range, 		0, 3, 
		p_ui, 			TYPE_RADIO, 	4, IDC_RADIO1, IDC_RADIO2, IDC_RADIO3, IDC_RADIO4,
		end, 
	
	phy_shape, _T("physxShape"), 		TYPE_INT, 		0,				IDS_RB_BOXSHAPE,
		p_default, 		0, 
		p_range, 		0, 3, 
		p_ui, 			TYPE_RADIO,		4, IDC_RADIO5, IDC_RADIO6, IDC_RADIO7, IDC_RADIO8, 
		end,
	end
	);

// JBW: pre-ParamBlock2 version info, similar to old scheme except here we
//      install ParamBlockDesc2 parameter IDs into the ParamBlockDescIDs for
//      each version so the post-load callback can do the right mapping to the new ParamBlock2
#define NUM_OLDVERSIONS	0

// static ParamBlockDesc2ID descVer0[] = {
// 	{ TYPE_STRING, NULL, FALSE,  geo_meshpath }
// 	{ TYPE_INT, NULL, FALSE,  geo_smooth }
// };
// 
// static ParamBlockDescID descVer0[] = {
// 	{ TYPE_STRING, NULL, FALSE, geo_meshpath },		
// 	{ TYPE_INT, NULL, FALSE,   geo_smooth }
// };

#define PBLOCK_LENGTH	2  

// Array of old ParamBlock Ed. 1 versions
// static ParamVersionDesc versions[] = {
// 	ParamVersionDesc (descVer0, 2, 0),
// };

// JBW:  for loading old ParamBlock versions, register an Ed. 2 param block converter post-load callback
//       This one takes the ParamBlockDescID version array as before, but now takes the 
//       ParamBlockDesc2 for the block to describe the current version and will convert the loaded ParamBlock
//       into a corresponding ParamBlock2
IOResult
gkObjectInMax::Load(ILoad *iload) 
{	
	ParamBlock2PLCB* plcb = new ParamBlock2PLCB(NULL, NUM_OLDVERSIONS, &geo_param_blk, this, PBLOCK_REF_NO);
	iload->RegisterPostLoadCallback(plcb);
	return IO_OK;
}

//--- TypeInDlgProc --------------------------------
class gkObjectInMaxTypeInDlgProc : public ParamMap2UserDlgProc {
public:
	gkObjectInMax *so;

	gkObjectInMaxTypeInDlgProc(gkObjectInMax *s) {so=s;}
	INT_PTR DlgProc(TimeValue t, IParamMap2 *map, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	void DeleteThis() {delete this;}
};

INT_PTR gkObjectInMaxTypeInDlgProc::DlgProc(
		TimeValue t, IParamMap2 *map, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (msg != WM_COMMAND) return FALSE;
	if (LOWORD(wParam) != IDC_TI_CREATE) return FALSE;
// JBW: access to STATIC paramblock data is via functions on
//      on the descriptor
	// 如果路径为空，返回，不要创建
	//std::string tmpStr = geo_param_blk.GetString(geo_meshpath);
	//if (tmpStr == "") return TRUE;
	
	// We only want to set the value if the object is 
	// not in the scene.
// 	if (so->TestAFlag(A_OBJ_CREATING)) {
// 		so->pblock2->SetValue(geo_meshpath, 0, geo_typein_blk.GetString(geo_meshpath));
// 		}
	so->typeinCreate = TRUE;

	Matrix3 tm(1);
	tm.SetTrans(geo_typein_blk.GetPoint3(geo_ti_pos));
	so->suspendSnap = FALSE;
	so->ip->NonMouseCreate(tm);

	// NOTE that calling NonMouseCreate will cause this
	// object to be deleted. DO NOT DO ANYTHING BUT RETURN.
	return TRUE;	
}

class gkObjectInMaxParamDlgProc : public ParamMap2UserDlgProc {
	public:
		gkObjectInMax *mpgkObjectInMaxObj;
        HWND mhWnd;
		gkObjectInMaxParamDlgProc(gkObjectInMax *o) {mpgkObjectInMaxObj=o;}
	   INT_PTR DlgProc(TimeValue t, IParamMap2 *map, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		void DeleteThis() {delete this;}
        void Update();
        BOOL GetRWSState();
	};

void gkObjectInMaxParamDlgProc::Update()
{
    BOOL usePhysUVs = mpgkObjectInMaxObj->GetUsePhysicalScaleUVs();
    CheckDlgButton(mhWnd, IDC_REAL_WORLD_MAP_SIZE, usePhysUVs);
    EnableWindow(GetDlgItem(mhWnd, IDC_REAL_WORLD_MAP_SIZE), mpgkObjectInMaxObj->HasUVW());
	// 设置一下目前的mesh地址
	//::SendMessage(GetDlgItem(mhWnd, IDC_MESHPATH),   EM_SETSEL,   -1,   -1);
	SetWindowText(GetDlgItem(mhWnd, IDC_MESHPATH), mpgkObjectInMaxObj->pblock2->GetStr(geo_meshpath, 0, 0));

	// 这里得到了纯净文件名，赋值给外面看一下
	SetWindowText(GetDlgItem(mhWnd, IDC_MESHNAME), (LPCTSTR)mpgkObjectInMaxObj->m_strMeshFile.c_str());
}

BOOL gkObjectInMaxParamDlgProc::GetRWSState()
{
    BOOL check = IsDlgButtonChecked(mhWnd, IDC_REAL_WORLD_MAP_SIZE);
    return check;
}

class NullView : public View
{
public:
	Point2 ViewToScreen(Point3 p)
	{ return Point2(p.x,p.y); }
	NullView() {
		worldToView.IdentityMatrix();
		screenW=640.0f; screenH = 480.0f;
	}
};

INT_PTR gkObjectInMaxParamDlgProc::DlgProc(TimeValue t, IParamMap2 *map, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
    case WM_INITDIALOG: {
        mhWnd = hWnd;
        Update();
        break;
    }
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDC_MAPPING:
            Update();
            break;

        case IDC_REAL_WORLD_MAP_SIZE: {
            BOOL check = IsDlgButtonChecked(hWnd, IDC_REAL_WORLD_MAP_SIZE);
            theHold.Begin();
            mpgkObjectInMaxObj->SetUsePhysicalScaleUVs(check);
            theHold.Accept(GetString(IDS_DS_PARAMCHG));
            mpgkObjectInMaxObj->ip->RedrawViews(mpgkObjectInMaxObj->ip->GetTime());
            break;

		
        }
        case IDC_CHOOSE_MESH:{ 
			//win32 api
			OPENFILENAME ofn;
			static char szFile[256];
			static char szFileTitle[256];
			memset(&ofn,0,sizeof(ofn));

			ofn.lStructSize=sizeof(ofn);
			ofn.hwndOwner=hWnd;
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
  				mpgkObjectInMaxObj->pblock2->SetValue(geo_meshpath, 0, ofn.lpstrFile);	
				// this func may break out all! warning!
				//geo_param_blk.SetValue(geo_meshpath, 0,  ofn.lpstrFile);
				Update();
			}
			break;
		
			}
		case IDC_UPDATE_MESH:
			{
				Update();

// 				INode *node = GetCOREInterface()->GetSelNode(0);
// 				node->GetObjectRef()->DeleteAllRefsToMe();

				Interval valid = FOREVER;
				int smooth = 0;
				mpgkObjectInMaxObj->pblock2->GetValue(geo_meshpath, 0, smooth, valid);
				mpgkObjectInMaxObj->pblock2->SetValue(geo_meshpath, 0, smooth);	
				break;
			}
		case IDC_COMMIT_MESH:
			{

				gmpMeshLoader meshLoader;

				const TCHAR* wszFileName = (LPCTSTR)mpgkObjectInMaxObj->pblock2->GetStr(geo_meshpath, 0, 0);
		
				INode *node = GetCOREInterface()->GetSelNode(0);
				const ObjectState& os = node->EvalWorldState(0);
				
				NullView view;
				BOOL bNeedDelete = FALSE;

				TriObject* pObj = (TriObject*)(os.obj->ConvertToType(0, triObjectClassID));

				GeomObject* pGeoObj = (GeomObject*)pObj;

				Mesh * pMesh = pGeoObj->GetRenderMesh(

					0 , node , view , bNeedDelete );

				

				meshLoader.SaveGeometryAsOBJ(wszFileName, *pMesh);

				// call gkStudio update this mesh [9/3/2011 Kaiming-Desktop]

				if (gUtilEnv->m_bMaxPorted)
				{
					TCHAR wszName[MAX_PATH];
					//gUtilEnv->pSystem->reloadResource_s(GK_RESOURCE_MANAGER_TYPE_MESH, mpgkObjectInMaxObj->m_szMeshFilename );
				}
				else
				{
					HWND h = FindWindow( "gkStudioClass", NULL );		//changed by [MG]
					if (h)
					{
						COPYDATASTRUCT cd;
						ZeroMemory(&cd, sizeof(COPYDATASTRUCT));
						cd.dwData = CPDATA_UPDATE_MESH;
						cd.cbData = strlen(mpgkObjectInMaxObj->m_szMeshFilename);
						cd.lpData = mpgkObjectInMaxObj->m_szMeshFilename;

						SendMessage( h,WM_COPYDATA,(WPARAM)NULL,(LPARAM)&cd );
						SetActiveWindow( h );
						SetForegroundWindow( h );
					}
				}

				break;
			}
		case IDC_SYNC:
			{
				Class_ID exporterID = IGAMEEXPORTER_CLASS_ID;
				GetCOREInterface()->ExportToFile(gUtilEnv->SyncSceneName, 1, 0, &exporterID);

				if (gUtilEnv->m_bMaxPorted)
				{
					gUtilEnv->pSystem->LoadScene_s(gUtilEnv->SyncSceneName, NULL);
				}
				else
				{
					HWND h = FindWindow( "gkStudioClass", NULL );		//changed by [MG]
					if (h)
					{

						COPYDATASTRUCT cd;
						ZeroMemory(&cd, sizeof(COPYDATASTRUCT));
						cd.dwData = CPDATA_SYNC_SCENE;
						cd.cbData = gUtilEnv->SyncSceneName.length();
						cd.lpData = gUtilEnv->SyncSceneName.data();

						SendMessage( h,WM_COPYDATA,(WPARAM)NULL,(LPARAM)&cd );
						SetActiveWindow( h );
						SetForegroundWindow( h );
					}
				}



				break;
			}
        }
        break;
        
    }
	return FALSE;
}

//--- gkObjectInMax methods -------------------------------

// JBW: the GeoSphere constructor has gone.  The paramblock creation and wiring and
//		the intial value setting is automatic now.

// JBW: BeginEditParams() becomes much simpler with automatic UI param blocks.
//      you redirect the BeginEditParams() to the ClassDesc instance and it
//      throws up the appropriate rollouts.

void gkObjectInMax::BeginEditParams(IObjParam *ip, ULONG flags, Animatable *prev)
{
	SimpleObject::BeginEditParams(ip, flags, prev);

    gkObjectInMaxParamDlgProc* dlg = static_cast<gkObjectInMaxParamDlgProc*>(geo_param_blk.GetUserDlgProc());
    if (dlg != NULL) {
        BOOL rws = dlg->GetRWSState();
        SetUsePhysicalScaleUVs(rws);
    }

	this->ip = ip;

	// If this has been freshly created by type-in, set creation values:
// 	if (typeinCreate) {
// 		pblock2->SetValue(geo_meshpath, 0, geo_typein_blk.GetFloat(geo_ti_radius));
// 		typeinCreate = FALSE;
// 	}

	// throw up all the appropriate auto-rollouts
	gkObjectInMaxDesc.BeginEditParams(ip, this, flags, prev);
	// install a callback for the type in.
	geo_typein_blk.SetUserDlgProc(new gkObjectInMaxTypeInDlgProc(this));
	// install a callback for the params.
	geo_param_blk.SetUserDlgProc(new gkObjectInMaxParamDlgProc(this));
}
		
// JBW: similarly for EndEditParams and you also don't have to snapshot
//		current parameter values as initial values for next object as
//		this is automatic for the new ParamBlock params unless disabled.

void gkObjectInMax::EndEditParams(IObjParam *ip, ULONG flags, Animatable *next)
{		
	SimpleObject::EndEditParams(ip, flags, next);
	this->ip = NULL;
	// tear down the appropriate auto-rollouts
	gkObjectInMaxDesc.EndEditParams(ip, this, flags, next);
}

// CONSTRUCTING THE MESH:

void gkObjectInMax::UpdateUI()
{
	if (ip == NULL)
		return;
	gkObjectInMaxParamDlgProc* dlg = static_cast<gkObjectInMaxParamDlgProc*>(geo_param_blk.GetUserDlgProc());
	dlg->Update();
}

BOOL gkObjectInMax::GetUsePhysicalScaleUVs()
{
	return ::GetUsePhysicalScaleUVs(this);
}


void gkObjectInMax::SetUsePhysicalScaleUVs(BOOL flag)
{
	BOOL curState = GetUsePhysicalScaleUVs();
	if (curState == flag)
		return;
	if (theHold.Holding())
		theHold.Put(new RealWorldScaleRecord<gkObjectInMax>(this, curState));
	::SetUsePhysicalScaleUVs(this, flag);
	if (pblock2 != NULL)
		pblock2->NotifyDependents(FOREVER, PART_GEOM, REFMSG_CHANGE);
	UpdateUI();
	macroRec->SetProperty(this, _T("realWorldMapSize"), mr_bool, flag);
}

BOOL gkObjectInMax::HasUVW() { 
// 	BOOL genUVs;
// 	Interval v;
// 	pblock2->GetValue(geo_mapping, 0, genUVs, v);
// 	return genUVs; 
	return TRUE;
	}

void gkObjectInMax::SetGenUVW(BOOL sw) {  
	if (sw==HasUVW()) return;
	//pblock2->SetValue(geo_mapping, 0, sw);				
	UpdateUI();
	}


// Now put it all together sensibly
#define EPSILON 1e-5f
void gkObjectInMax::BuildMesh(TimeValue t)
{
	ivalid = FOREVER;
	gmpMeshLoader meshLoader;

	const TCHAR* wszFileName = (LPCTSTR)pblock2->GetStr(geo_meshpath, 0, 0);

	CHAR strFileName[MAX_PATH];
	strcpy_s( strFileName, MAX_PATH, pblock2->GetStr(geo_meshpath, 0, 0) );

	// 砍掉路径，保留文件名
	CHAR strPureName[MAX_PATH] = {0};
	CHAR* strLastSlash = NULL;
	strLastSlash = strrchr( strFileName, '\\' );
	if( strLastSlash )
	{
		strcpy_s( strPureName, MAX_PATH, &strLastSlash[1] );
	}
	m_strMeshFile = strPureName;
	strcpy_s( m_szMeshFilename, MAX_PATH, strPureName );

	if ( S_FALSE == meshLoader.LoadGeometryFromOBJ(wszFileName))
	{

		meshLoader.DeleteCache();
		char path[MAX_PATH];
		
		getEngineRoot(path);

		strcat_s(path, MAX_PATH, "\\media\\meshs\\temp\\");
		strcat_s(path, MAX_PATH, m_strMeshFile.c_str());

		//pblock2->SetValue(geo_meshpath, 0, path);	

		meshLoader.LoadGeometryFromOBJ(path);
	}
	



	//meshLoader.Create(_T("file!"));
	mesh.FreeAll();

	mesh.setNumVerts(meshLoader.GetVertexNumber());
	mesh.setNumFaces(meshLoader.GetFaceNumber());
	// smooth
	int bSmooth;
	pblock2->GetValue(geo_smooth, t, bSmooth, ivalid);
	if (bSmooth)
	{
		mesh.setSmoothFlags(1);
	}

	// texcoord
	mesh.setNumTVerts (meshLoader.GetTexVertexNumber());
	mesh.setNumTVFaces (meshLoader.GetFaceNumber());
	//mesh.setMapSupport(0, true);
	//mesh->buildNormals();
	//mesh.setNumMapVerts(0, meshLoader.GetVertexNumber());

	// 顶点
	for(int i=0; i<meshLoader.GetVertexNumber(); i++)
	{
		mesh.setVert(i, meshLoader.m_Vertices[i].position);
		mesh.setNormal(i, meshLoader.m_Vertices[i].normal);
	}

	for (int i=0; i<meshLoader.GetTexVertexNumber(); i++)
	{
		// texcoord
		UVVert texcoord;
		texcoord.x = meshLoader.TexCoords[i].x;
		texcoord.y = meshLoader.TexCoords[i].y;
		texcoord.z = 0.0f;
		mesh.setTVert(i, texcoord);
	}

	// 面
	for(int i=0; i<meshLoader.GetFaceNumber(); i++)
	{
		MAXFACE face = meshLoader.m_MaxPointFaces[i];
		MAXFACE tface = meshLoader.m_MaxTexcoordFaces[i];
		
		mesh.faces[i].setVerts(face.dwFirstFace, face.dwSecondFace, face.dwThirdFace );
		mesh.tvFace[i].setTVerts(tface.dwFirstFace, tface.dwSecondFace, tface.dwThirdFace );
		mesh.faces[i].setEdgeVisFlags(1, 1, 1);

		mesh.faces[i].setSmGroup (0);
		mesh.faces[i].setMatID (face.dwMatID);
	}

	if (bSmooth)
	{
		mesh.AutoSmooth( 1.04f, FALSE );
	}
	mesh.InvalidateTopologyCache();
	
	m_bCreated = true;
	m_bNeedUpdate = true;
}

class gkObjectInMaxObjCreateCallBack : public CreateMouseCallBack {
	IPoint2 sp0;
	gkObjectInMax *ob;
	Point3 p0;
public:
	int proc( ViewExp *vpt, int msg, int point, int flags, IPoint2 m, Matrix3& mat);
	void SetObj(gkObjectInMax *obj) {ob = obj;}
};

int gkObjectInMaxObjCreateCallBack::proc(ViewExp *vpt, int msg, int point, int flags, IPoint2 m, Matrix3& mat ) {
	float r;
	Point3 p1, center;

	if (msg == MOUSE_FREEMOVE)
	{
		vpt->SnapPreview(m, m, NULL, SNAP_IN_3D);
	}


	if (msg==MOUSE_POINT||msg==MOUSE_MOVE) {
		switch(point) {
		case 0:  // only happens with MOUSE_POINT msg
			ob->suspendSnap = TRUE;				
			sp0 = m;
			p0 = vpt->SnapPoint(m, m, NULL, SNAP_IN_3D);
			mat.SetTrans(p0);
			return CREATE_STOP;
			break;
// 		case 1:
// 			mat.IdentityMatrix();
// 			if (geo_crtype_blk.GetInt(geo_create_meth)) {
// 				p1 = vpt->SnapPoint(m, m, NULL, SNAP_IN_3D);
// 				r = Length(p1-p0);
// 				mat.SetTrans(p0);
// 			} else {
// 				p1 = vpt->SnapPoint(m, m, NULL, SNAP_IN_3D);
// 				center = (p0+p1)/float(2);
// 				mat.SetTrans(center);
// 				r = Length(center-p0);
// 				mat.SetTrans(center);
// 			} 
// 			//ob->pblock2->SetValue(geo_radius, 0, r);
// 			geo_param_blk.InvalidateUI();
// 
// 			if (flags&MOUSE_CTRL) {
// 				float ang = (float)atan2(p1.y-p0.y, p1.x-p0.x);					
// 				mat.PreRotateZ(ob->ip->SnapAngle(ang));
// 			}
// 
// 			if (msg==MOUSE_POINT) {
// 				ob->suspendSnap = FALSE;
// 				return (Length(m-sp0)<3)?CREATE_ABORT:CREATE_STOP;
// 			}
// 			break;					   
		}
	} else {
		if (msg == MOUSE_ABORT) return CREATE_ABORT;
	}

	return TRUE;
}

static gkObjectInMaxObjCreateCallBack gkObjectInMaxCreateCB;

CreateMouseCallBack* gkObjectInMax::GetCreateMouseCallBack() 
	{
	gkObjectInMaxCreateCB.SetObj(this);
	return(&gkObjectInMaxCreateCB);
	}


BOOL gkObjectInMax::OKtoDisplay(TimeValue t) 
	{
// 	float radius;
// 	pblock2->GetValue(geo_radius, t, radius, FOREVER);
// 	if (radius==0.0f) return FALSE;
// 	else return TRUE;

	if( !m_bCreated )
	{
		return FALSE;	
	}
// 	if( m_bNeedUpdate )
// 	{
// 		m_bNeedUpdate = false;
// 		return FALSE;
// 	}

	return TRUE;
	}

void gkObjectInMax::InvalidateUI() 
{
	// if this was caused by a NotifyDependents from pblock2, LastNotifyParamID()
	// will contain ID to update, else it will be -1 => inval whole rollout
	geo_param_blk.InvalidateUI(pblock2->LastNotifyParamID());
}

RefTargetHandle gkObjectInMax::Clone(RemapDir& remap) 
{
	gkObjectInMax* newob = new gkObjectInMax(FALSE);	
	newob->ReplaceReference(0, remap.CloneRef(pblock2));
	newob->ivalid.SetEmpty();	
	BaseClone(this, newob, remap);
	return(newob);
}

Object* gkObjectInMax::ConvertToType(TimeValue t, Class_ID obtype)
	{
		return SimpleObject::ConvertToType(t, obtype);
	}

int gkObjectInMax::CanConvertToType(Class_ID obtype)
	{	
		return SimpleObject::CanConvertToType(obtype);
	}



