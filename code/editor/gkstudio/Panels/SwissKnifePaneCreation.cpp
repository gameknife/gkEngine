#include "StdAfx.h"
#include "SwissKnifePaneCreation.h"
#include "resource.h"
#include "EditorUtils.h"
#include "gkFilePath.h"
#include "Utils\FileEnum.h"
#include "IGameObjectLayer.h"

#define SWISSKNIFEBAR_ICON_STATICOBJ		0
#define SWISSKNIFEBAR_ICON_STATICOBJ_SEL	1
#define SWISSKNIFEBAR_ICON_GAMEOBJ			2
#define SWISSKNIFEBAR_ICON_GAMEOBJ_SEL		3
#define SWISSKNIFEBAR_ICON_LIGHTS			4
#define SWISSKNIFEBAR_ICON_LIGHTS_SEL		5
#define SWISSKNIFEBAR_ICON_CAMERA			6
#define SWISSKNIFEBAR_ICON_CAMERA_SEL		7



#define SWISSKNIFEBAR_ICON_FOLDER			18
#define SWISSKNIFEBAR_ICON_FOLDER_SEL		19



BEGIN_MESSAGE_MAP(CSwissKnifePaneCreation, CXTPShortcutBarPane)
	//{{AFX_MSG_MAP(CMainFrame)
	//ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
	ON_NOTIFY(NM_CLICK, 33985, &CSwissKnifePaneCreation::OnCatePaneClick)
	ON_NOTIFY(NM_DBLCLK, 33986, &CSwissKnifePaneCreation::OnDetailPaneClick)

END_MESSAGE_MAP()







//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSwissKnifePaneCreation::CSwissKnifePaneCreation()
{

}

CSwissKnifePaneCreation::~CSwissKnifePaneCreation()
{

}


BOOL CSwissKnifePaneCreation::Create(LPCTSTR lpszCaption, CXTPShortcutBar* pParent)
{
	if (!CXTPShortcutBarPane::Create(lpszCaption, pParent))
		return FALSE;


	VERIFY(m_wndTreeCategories.Create(WS_VISIBLE|TVS_HASBUTTONS|TVS_LINESATROOT, CXTPEmptyRect(), this, 33985));
	m_wndTreeCategories.SetItemHeight(18);
	m_wndTreeCategories.SetOwner(this);
	// TreeCtrl must be already created.
	CMFCUtils::LoadTrueColorImageList( m_imageList,IDB_SKB_ICONLIST,16,0xffffff );

	m_wndTreeCategories.SetImageList(&m_imageList,TVSIL_NORMAL);


	VERIFY(m_wndTreeDetail.Create(WS_VISIBLE|TVS_HASBUTTONS|TVS_LINESATROOT, CXTPEmptyRect(), this, 33986));
	m_wndTreeDetail.SetItemHeight(18);
	m_wndTreeDetail.SetOwner(this);
	// TreeCtrl must be already created.
	CMFCUtils::LoadTrueColorImageList( m_imageList,IDB_SKB_ICONLIST,16,0xffffff );
	m_wndTreeDetail.SetImageList(&m_imageList,TVSIL_NORMAL);

	CreateCategories();

	AddItem(_T("Categories"), &m_wndTreeCategories, 80);
	AddItem(_T("Detail"), &m_wndTreeDetail, 125);

	return TRUE;
}

void CSwissKnifePaneCreation::UpdateTheme(BOOL bOffice2010Theme)
{
	if (bOffice2010Theme)
	{
		SetFlatStyle(TRUE);
	}
	else
	{
		SetFlatStyle(FALSE);
	}

	COLORREF clrBackground = GetBackgroundColor();
	COLORREF clrTextColor = GetTextColor();

 
 	m_wndTreeCategories.SendMessage(TVM_SETBKCOLOR, 0, clrBackground); 
 	m_wndTreeCategories.SendMessage(TVM_SETTEXTCOLOR, 0, clrTextColor); 
 
 	m_wndTreeDetail.SendMessage(TVM_SETBKCOLOR, 0, clrBackground); 
 	m_wndTreeDetail.SendMessage(TVM_SETTEXTCOLOR, 0, clrTextColor); 
}
//////////////////////////////////////////////////////////////////////////
void CSwissKnifePaneCreation::CreateCategories()
{
	m_wndTreeCategories.InsertItem (_T("StaticGeo"), SWISSKNIFEBAR_ICON_STATICOBJ, SWISSKNIFEBAR_ICON_STATICOBJ_SEL);
	m_wndTreeCategories.InsertItem (_T("GameObject"), SWISSKNIFEBAR_ICON_GAMEOBJ, SWISSKNIFEBAR_ICON_GAMEOBJ_SEL);
	m_wndTreeCategories.InsertItem (_T("Light"), SWISSKNIFEBAR_ICON_LIGHTS, SWISSKNIFEBAR_ICON_LIGHTS_SEL);
	m_wndTreeCategories.InsertItem (_T("Camera"), SWISSKNIFEBAR_ICON_CAMERA, SWISSKNIFEBAR_ICON_CAMERA_SEL);
}

//////////////////////////////////////////////////////////////////////////
void CSwissKnifePaneCreation::OnDetailPaneClick( NMHDR *pNMHDR, LRESULT *pResult )
{
	*pResult = FALSE;

	// Show helper menu.
	CPoint point;
	// Find node under mouse.
	GetCursorPos( &point );
	m_wndTreeDetail.ScreenToClient( &point );
	// Select the item that is at the point myPoint.
	UINT uFlags;
	HTREEITEM hItem = m_wndTreeDetail.HitTest(point,&uFlags);
	if (hItem )
	{
		SFileItemInfo* pItem = (SFileItemInfo*)(m_wndTreeDetail.GetItemData(hItem));
		if ( pItem )
		{
			switch ( pItem->type)
			{
			case SWISSKNIFEBAR_ICON_LIGHTS:
				{
					IGameObject* pLight = gEnv->pGameObjSystem->CreateLightGameObject( findNextName(_T("PointLight")), Vec3(0,0,0), 5, ColorF(1,1,1,1) );
				}
				break;
			case SWISSKNIFEBAR_ICON_STATICOBJ:
				{
					IGameObject* pGo = gEnv->pGameObjSystem->CreateStaticGeoGameObject( findNextName(_T("GameObject")), pItem->m_wstrFullname, Vec3(ZERO), Quat::CreateIdentity());

					if (pGo)
					{
						TCHAR wszPath[MAX_PATH] = _T("");
						_tcscpy(wszPath, pItem->m_wstrFullname.c_str());

						TCHAR* strLastSlash = NULL;
						strLastSlash = _tcsrchr( wszPath, _T( '.' ) );
						if( strLastSlash )
						{
							_tcscpy_s( strLastSlash, MAX_PATH, _T(".mtl") );
						}

						IGameObjectRenderLayer* pRenderLayer = pGo->getRenderLayer();
						if (pRenderLayer)
						{
							gkLogMessage(_T("Load Material"));
							pRenderLayer->setMaterialName(wszPath);
							gkLogMessage(_T("Material Loaded"));
						}
					}
				}
				break;
			case SWISSKNIFEBAR_ICON_GAMEOBJ:
				{
					IGameObject* pGo = gEnv->pGameObjSystem->CreateParticleGameObject( findNextName(_T("Particle")), pItem->m_wstrFullname, Vec3(ZERO), Quat::CreateIdentity());
				}
				break;
			}			
		}
	}

	*pResult = 0;
}

void CSwissKnifePaneCreation::OnCatePaneClick( NMHDR *pNMHDR, LRESULT *pResult )
{

	*pResult = FALSE;

	// Show helper menu.
	CPoint point;
	// Find node under mouse.
	GetCursorPos( &point );
	m_wndTreeCategories.ScreenToClient( &point );
	// Select the item that is at the point myPoint.
	UINT uFlags;
	HTREEITEM hItem = m_wndTreeCategories.HitTest(point,&uFlags);
	if (hItem )
	{
		CString text = m_wndTreeCategories.GetItemText(hItem);
		if (text == _T("StaticGeo"))
		{
			CreateStaticGeoDetail();
		}
		else if (text == _T("GameObject"))
		{
			CreateGamoObjectDetail();
		}
		else if (text == _T("Light"))
		{
			CreateLightDetail();
		}
		else if (text == _T("Camera"))
		{
			CreateCameraDetail();
		}

	}

	*pResult = 0;
}
//////////////////////////////////////////////////////////////////////////
void CSwissKnifePaneCreation::CreateStaticGeoDetail()
{
	ClearDetailTree();


	gkStdString gameroot = gkGetGameRootDir();

	{
		std::vector<CString> files;
		CFileEnum::ScanDirectory( gameroot.c_str(), _T("*.obj"), files , true );


		for ( uint32 i=0; i < files.size(); ++i)
		{
			AddFileItemIntoTree( files[i], SWISSKNIFEBAR_ICON_STATICOBJ );
			//m_wndTreeDetail.InsertItem (gkShortName(gkStdString( files[i] )).c_str(), SWISSKNIFEBAR_ICON_CAMERA, SWISSKNIFEBAR_ICON_CAMERA_SEL);
		}
	}

	{
		std::vector<CString> files;
		CFileEnum::ScanDirectory( gameroot.c_str(), _T("*.ptc"), files , true );


		for ( uint32 i=0; i < files.size(); ++i)
		{
			AddFileItemIntoTree( files[i], SWISSKNIFEBAR_ICON_GAMEOBJ );
			//m_wndTreeDetail.InsertItem (gkShortName(gkStdString( files[i] )).c_str(), SWISSKNIFEBAR_ICON_CAMERA, SWISSKNIFEBAR_ICON_CAMERA_SEL);
		}
	}
	
}
//////////////////////////////////////////////////////////////////////////
void CSwissKnifePaneCreation::CreateGamoObjectDetail()
{
	ClearDetailTree();
}
//////////////////////////////////////////////////////////////////////////
void CSwissKnifePaneCreation::CreateLightDetail()
{
	ClearDetailTree();

	AddFileItemIntoTree( _T("Point Light"), SWISSKNIFEBAR_ICON_LIGHTS );
}
//////////////////////////////////////////////////////////////////////////
void CSwissKnifePaneCreation::CreateCameraDetail()
{
	ClearDetailTree();
}
//////////////////////////////////////////////////////////////////////////
const TCHAR* CSwissKnifePaneCreation::findNextName( const gkStdString& prefix )
{
	int count = 1;
	while(1)
	{
		_stprintf_s( m_wstrNextName, MAX_PATH, _T("%s%.3d"), prefix.c_str(), count );

		if ( !(gEnv->pGameObjSystem->GetGameObjectByName(m_wstrNextName)) )
		{
			return m_wstrNextName;
		}

		count = count + 1;
	}
}
//////////////////////////////////////////////////////////////////////////
void CSwissKnifePaneCreation::ClearDetailTree()
{
	m_wndTreeDetail.DeleteAllItems();
	m_namemap.clear();
	m_items.clear();
}

//////////////////////////////////////////////////////////////////////////
void CSwissKnifePaneCreation::AddFileItemIntoTree( CString fullname, int type )
{
	TCHAR fullpath[MAX_PATH];
	_tcscpy_s( fullpath, fullname );

	HTREEITEM hRoot = TVI_ROOT;

	TCHAR* token = NULL;
	token = _tcstok( fullpath, _T("/\\") );

	while (token != NULL)
	{
		//m_treeMaterialList.InsertItem()
		CString tokenName = token;
		token = _tcstok(NULL, _T("/\\"));
		if (!token)
		{
			break;
		}

		NameTreeMap::iterator it = m_namemap.find(tokenName);
		if( it != m_namemap.end() )
		{
			// find it
			hRoot = m_namemap[tokenName];
		}
		else
		{
			hRoot = m_wndTreeDetail.InsertItem( tokenName, SWISSKNIFEBAR_ICON_FOLDER, SWISSKNIFEBAR_ICON_FOLDER_SEL, hRoot );
			//m_treeMaterialList.SetItemBold(hRoot);
			m_namemap[tokenName] = hRoot;
		}
	}

	InsertFileItemIntoTree(fullname, hRoot, type);
}
//////////////////////////////////////////////////////////////////////////
void CSwissKnifePaneCreation::InsertFileItemIntoTree( CString fullname, HTREEITEM parent, int type )
{
	CString itemText;

	// submaterial [5/11/2012 Kaiming]
	HTREEITEM hItem;
	{
		hItem = m_wndTreeDetail.InsertItem( gkShortName(gkStdString( fullname )).c_str(), type, SWISSKNIFEBAR_ICON_STATICOBJ_SEL, parent);
		//m_treeMaterialList.SetItemBold(hItem);
	}

	SFileItemInfo* pItemInfo = new SFileItemInfo;
	pItemInfo->m_hItem = hItem;
	pItemInfo->m_wstrFullname = fullname;
	pItemInfo->type = type;

	m_items.push_back(pItemInfo);
	m_wndTreeDetail.SetItemData( hItem,(DWORD_PTR)(pItemInfo) );
}
