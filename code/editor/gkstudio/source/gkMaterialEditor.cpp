// F:\_PersonalProjects\_gkEngine\trunk\Projects\gkStudio\source\gkMaterialEditor.cpp : implementation file
//

#include "stdafx.h"
#include "gkMaterialEditor.h"
#include "IResourceManager.h"
#include "..\Controls\CustomItems.h"
#include "IMaterial.h"
#include "gkObjectManager.h"
#include "gkFilePath.h"
#include "EditorUtils.h"
#include "IGameObjectLayer.h"

const TCHAR* MaterialSlotNames[] = {
	_T("漫反射通道"),
	_T("法线通道"),
	_T("高光通道"),
	_T("细节通道"),
	_T("自定义1"),
	_T("自定义2"),
	_T("环境通道1"),
	_T("环境通道2"),
};

#define ITEM_IMAGE_MATERIAL          0
#define ITEM_IMAGE_MATERIAL_SELECTED 1
#define ITEM_IMAGE_FOLDER            2
#define ITEM_IMAGE_FOLDER_OPEN       3
#define ITEM_IMAGE_MULTI_MATERIAL    4
#define ITEM_IMAGE_MULTI_MATERIAL_SELECTED 5
// gkMaterialEditor dialog

//IMPLEMENT_DYNAMIC(gkMaterialEditor, CXTResizeDialog)

gkMaterialEditor::gkMaterialEditor(CWnd* pParent /*=NULL*/)
	: gkToolBarPaneBase(gkMaterialEditor::IDD, pParent)
{
}

gkMaterialEditor::~gkMaterialEditor()
{
}

void gkMaterialEditor::DoDataExchange(CDataExchange* pDX)
{
	gkToolBarPaneBase::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(gkMaterialEditor, gkToolBarPaneBase)
	ON_MESSAGE(XTPWM_PROPERTYGRID_NOTIFY, OnGridNotify)
	ON_WM_SIZE()

	ON_COMMAND_RANGE(ID_BUTTON_MATEDIT_GET, ID_BUTTON_MATEDT_CATE, OnEmptyCommandRange)
	ON_NOTIFY(NM_CLICK, AFX_IDW_PANE_FIRST, &gkMaterialEditor::OnTreeBrowserClick)
END_MESSAGE_MAP()

BOOL gkMaterialEditor::OnInitDialog()
{
	gkToolBarPaneBase::OnInitDialog();

	SetResize(IDC_MATEDTLIST,       SZ_TOP_LEFT, SZ_BOTTOM_CENTER);
	//SetResize(IDC_MATEDTGRID,       SZ_TOP_CENTER, SZ_BOTTOM_LEFT);
	RefreshMetrics(TRUE);



// 	CRect rc;
// 	GetClientRect(rc)
// 
// 	m_wndPlaceHolder.Create(_T(""), WS_CHILD | WS_VISIBLE, rc, this);


	m_treeMaterialList.Create( WS_VISIBLE|WS_CHILD|WS_TABSTOP|WS_BORDER|TVS_HASBUTTONS|TVS_SHOWSELALWAYS|TVS_LINESATROOT|TVS_HASLINES|
		TVS_FULLROWSELECT|TVS_INFOTIP,
		CRect(0,0,100,400), this, AFX_IDW_PANE_FIRST );

	m_treeMaterialList.SetItemHeight(18);
	m_treeMaterialList.SetIndent(0);
	// TreeCtrl must be already created.
	CMFCUtils::LoadTrueColorImageList( m_imageList,IDB_MATEDTLIST,16,RGB(255,255,255) );
	//CMFCUtils::LoadTrueColorImageList( m_imageList,IDB_FILE_STATUS,20,RGB(255,0,255) );
	m_treeMaterialList.SetImageList(&m_imageList,TVSIL_NORMAL);

	InitPropertyTable();

	RegisterPane(&m_treeMaterialList, _T("MaterialBrowser"), xtpPaneDockLeft, CSize(200,1024), xtpPaneNoCaption);
	OpenPane(_T("MaterialBrowser"));

	RegisterPane(&m_wndPropertyGrid, _T("MaterialProperty"), xtpPaneDockRight, CSize(2048,1024), xtpPaneNoCaption);
	OpenPane(_T("MaterialProperty"));
// 	RegisterPane(&m_treeMaterialList, _T("MaterialBrowser2"));
// 	OpenPane(_T("MaterialBrowser2"));

	

	InitToolBar(_T("ToolBar_MatEdt"), IDR_TOOLBAR_MATERIALEDITOR);


// 	m_wndVSplitter.CreateStatic( this,1,2,WS_CHILD|WS_VISIBLE, AFX_IDW_PANE_FIRST );
// 	m_wndHSplitter.CreateStatic( &m_wndVSplitter,2,1,WS_CHILD|WS_VISIBLE, AFX_IDW_PANE_FIRST );
// 	m_wndVSplitter.SetNoBorders();
// 	m_wndHSplitter.SetNoBorders();






	//m_wndVSplitter.SetPane(0,0, &m_treeMaterialList, CSize(200,100));
	//m_wndVSplitter.SetPane(0,1, &m_wndPropertyGrid, CSize(200,100));

	return TRUE;
}

BOOL gkMaterialEditor::InitPropertyTable()
{
	// create the property grid.
	if ( m_wndPropertyGrid.Create(CRect(0,0,0,0), this, IDC_MATEDTPROPERTY ) )
	{
		m_wndPropertyGrid.SetOwner(this);
		m_wndPropertyGrid.SetVariableItemsHeight(TRUE);

		LOGFONT lf;
		GetFont()->GetLogFont( &lf );


		CXTPPropertyGridItem* pItem = NULL;

		// create global settings category.
		CXTPPropertyGridItem* pGlobals      = m_wndPropertyGrid.AddCategory(_T("材质参数"));
	}

	//SetResize(IDC_MATEDTPROPERTY, SZ_TOP_CENTER, SZ_BOTTOM_RIGHT);

	return TRUE;
}

void gkMaterialEditor::RefreshMaterialList()
{
	// 0. clean the list
	m_treeMaterialList.DeleteAllItems();
	m_namemap.clear();

	for (uint32 i=0; i < m_items.size(); ++i)
	{
		SAFE_DELETE( m_items[i] );
	}
	m_items.clear();

	const gkResourceMap& map = gEnv->pSystem->getMaterialMngPtr()->getResourceMap();

	gkResourceMap::const_iterator it = map.begin();

	for(; it != map.end(); ++it)
	{
		AddMaterialIntoTree(reinterpret_cast<IMaterial*>( it->second.getPointer() ));
	}

	m_treeMaterialList.Expand(m_treeMaterialList.GetRootItem(), TVE_EXPAND);
}

void gkMaterialEditor::ModifyCurMaterialDword(int index, uint32 value)
{
	gkMaterialPtr curMtl = gEnv->pSystem->getMaterialMngPtr()->getByName(gkStdString(m_curSelMtlName));

	if (!curMtl.isNull())
	{
		gkMaterialParams* pParas = curMtl->getLoadingParameterBlock();
		if (pParas)
		{
			m_curSelMtlParaNum = pParas->m_vecParams.size();

			if (index < m_curSelMtlParaNum && pParas->m_vecParams[index]->Type == GKSHADERT_DWORD)
			{
				//memcpy_s(pParas->m_vecParams[index]->pValue, length, value, length);
				*((uint32*)(pParas->m_vecParams[index]->pValue)) = value;
				gEnv->pSystem->getMaterialMngPtr()->reload_s(gkStdString(m_curSelMtlName));
			}
		}
	}
	else
	{
		m_curSelMtlParaNum = 0;
	}
}

void gkMaterialEditor::ModifyCurMaterialTexture(int index, const TCHAR* value)
{
	gkStdString saveName(m_curSelMtlName);

	IMaterial* curMtl = static_cast<IMaterial* >( gEnv->pSystem->getMaterialMngPtr()->getByName(gkStdString(m_curSelMtlName)).getPointer() );

	if (!curMtl)
	{
		// 没找到，应该是个submtl，通过mainmtl来找
		int find = m_curSelMtlName.ReverseFind(_T('|'));
		if (find != -1)
		{
			curMtl = static_cast<IMaterial* >( gEnv->pSystem->getMaterialMngPtr()->getByName( gkStdString(m_curSelMtlName).substr(0, find) ).getPointer() );
			if (curMtl)
			{
				for ( uint32 i=0; i < curMtl->getSubMtlCount(); ++i )
				{
					if( !_tcsicmp( curMtl->getSubMaterial(i)->getName().c_str(), gkStdString(m_curSelMtlName).c_str() ) )
					{
						saveName = curMtl->getName();
						curMtl = curMtl->getSubMaterial(i);
						break;
					}
				}
				
			}
		}
	}

	if ( curMtl )
	{
		// new routine, directly load texture here [12/31/2014 gameKnife]

		gkTexturePtr newTex;
		newTex = gEnv->pSystem->getTextureMngPtr()->load( value );
		curMtl->setTexture( newTex, index );

		// old history //  [6/28/2014 gameKnife]
// 		gkMaterialParams* pParas = curMtl->getLoadingParameterBlock();
// 		if (pParas)
// 		{
// 			m_curSelMtlParaNum = pParas->m_vecParams.size();
// 
// 			if (index < m_curSelMtlParaNum && pParas->m_vecParams[index]->Type == GKSHADERT_STRING)
// 			{
// 				//memcpy_s(pParas->m_vecParams[index]->pValue, length, value, length);
// 				_tcscpy_s((TCHAR*)(pParas->m_vecParams[index]->pValue), MAX_PATH, value);
// 				gEnv->pSystem->getMaterialMngPtr()->reload_s(saveName);
// 			}
// 		}

		// new method

		//curMtl->getTexture()
		//gEnv->pSystem->getMaterialMngPtr()->reload_s(saveName);
	}
	else
	{
		m_curSelMtlParaNum = 0;
	}
}

LRESULT gkMaterialEditor::OnGridNotify(WPARAM wParam, LPARAM lParam)
{
	if (wParam == XTP_PGN_INPLACEBUTTONDOWN)
	{
		CXTPPropertyGridInplaceButton* pButton = (CXTPPropertyGridInplaceButton*)lParam;
		if (pButton->GetItem()->GetID() == 510 && pButton->GetID() == XTP_ID_PROPERTYGRID_EXPANDBUTTON) // 2 Buttons
		{
			AfxMessageBox(_T("Expand button"));
			return TRUE;
		}
	}

	if (wParam == XTP_PGN_SORTORDER_CHANGED)
	{
		//m_nSort = m_wndPropertyGrid.GetPropertySort();
		UpdateData(FALSE);

		TRACE(_T("SortOrder Changed.\n"));
	}
	if (wParam == XTP_PGN_ITEMVALUE_CHANGED)
	{
		CXTPPropertyGridItem* pItem = (CXTPPropertyGridItem*)lParam;
		TRACE(_T("Value Changed. Caption = %s, ID = %i, Value = %s\n"), pItem->GetCaption(), pItem->GetID(), pItem->GetValue());

		if (DYNAMIC_DOWNCAST(CXTPPropertyGridItemEnum, pItem))
		{
			if (pItem->GetMetrics(TRUE, FALSE))
			{
				pItem->GetMetrics(TRUE, FALSE)->m_nImage = ((CXTPPropertyGridItemEnum*)pItem)->GetEnum();
			}
		}

		if (pItem->GetID() == MATEDT_SHADERSET)
		{
			gkStdString saveName = m_curSelMtlName;

			// rebuild shader
			IMaterial* curMtl = static_cast<IMaterial* >( gEnv->pSystem->getMaterialMngPtr()->getByName(gkStdString(m_curSelMtlName)).getPointer() );

			if (!curMtl)
			{
				// 没找到，应该是个submtl，通过mainmtl来找
				int find = m_curSelMtlName.ReverseFind(_T('|'));
				if (find != -1)
				{
					curMtl = static_cast<IMaterial* >( gEnv->pSystem->getMaterialMngPtr()->getByName( gkStdString(m_curSelMtlName).substr(0, find) ).getPointer() );
					if (curMtl)
					{
						for ( uint32 i=0; i < curMtl->getSubMtlCount(); ++i )
						{
							if( !_tcsicmp( curMtl->getSubMaterial(i)->getName().c_str(), gkStdString(m_curSelMtlName).c_str() ) )
							{
								saveName = curMtl->getName();
								curMtl = curMtl->getSubMaterial(i);
								break;
							}
						}

					}
				}
			}

			if ( curMtl )
			{
				//curMtl->rebuildMarco();

				
				curMtl->changeShader( pItem->GetValue() );
				curMtl->getShaderMarcos().clear();
				curMtl->rebuildMarco();

				gkMaterialPtr mainMtl = gEnv->pSystem->getMaterialMngPtr()->getByName(saveName);
				if (!mainMtl.isNull())
				{
					mainMtl->rebuildMarco();
					gEnv->pSystem->getMaterialMngPtr()->reload_s(saveName);
				}

			}
		}

		if (pItem->GetID() == MATEDT_MACROSET)
		{
			gkStdString saveName = m_curSelMtlName;
			// rebuild shader
			IMaterial* curMtl = static_cast<IMaterial* >( gEnv->pSystem->getMaterialMngPtr()->getByName(gkStdString(m_curSelMtlName)).getPointer() );

			if (!curMtl)
			{
				// 没找到，应该是个submtl，通过mainmtl来找
				int find = m_curSelMtlName.ReverseFind(_T('|'));
				if (find != -1)
				{
					curMtl = static_cast<IMaterial* >( gEnv->pSystem->getMaterialMngPtr()->getByName( gkStdString(m_curSelMtlName).substr(0, find) ).getPointer() );
					if (curMtl)
					{
						for ( uint32 i=0; i < curMtl->getSubMtlCount(); ++i )
						{
							if( !_tcsicmp( curMtl->getSubMaterial(i)->getName().c_str(), gkStdString(m_curSelMtlName).c_str() ) )
							{
								saveName = curMtl->getName();
								curMtl = curMtl->getSubMaterial(i);
								break;
							}
						}

					}
				}
			}

			if ( curMtl )
			{
				curMtl->rebuildMarco();

				gkMaterialPtr mainMtl = gEnv->pSystem->getMaterialMngPtr()->getByName(saveName);
				if (!mainMtl.isNull())
				{
					mainMtl->rebuildMarco();
					gEnv->pSystem->getMaterialMngPtr()->reload_s(saveName);
				}
				
			}
			
		}


		if( pItem->GetID() >= MATEDT_PARAMSTART && pItem->GetID() < MATEDT_PARAMSTART + m_curSelMtlParaNum)
		{
			CXTPPropertyGridItemNumber* dword = dynamic_cast<CXTPPropertyGridItemNumber*>(pItem);
			if (dword)
				ModifyCurMaterialDword(pItem->GetID() - MATEDT_PARAMSTART, dword->GetNumber());


			//gEnv->pSystem->getMaterialMngPtr()->reload_s(gkStdString(m_curSelMtlName));		
		}

		if(  pItem->GetID() >= MATEDT_TEXSET && pItem->GetID() < MATEDT_TEXSET + eMS_Invalid )
		{
			CCustomItemFileBox* filebox = dynamic_cast<CCustomItemFileBox*>(pItem);
			if (filebox)
			{
				ModifyCurMaterialTexture(pItem->GetID() - MATEDT_TEXSET, filebox->GetValue().GetString());
			}
		}
	}
	if (wParam == XTP_PGN_SELECTION_CHANGED)
	{
		CXTPPropertyGridItem* pItem = (CXTPPropertyGridItem*)lParam;
		if (pItem)
		{
			if (pItem->IsKindOf(RUNTIME_CLASS(CXTPPropertyGridItemColor)))
			{
				TRACE(_T("Color Item. Selection Changed. Item = %s\n"), pItem->GetCaption());
			}
		}
	}
	if (wParam == XTP_PGN_EDIT_CHANGED)
	{
		CXTPPropertyGridInplaceEdit* pEdit = DYNAMIC_DOWNCAST(CXTPPropertyGridInplaceEdit, (CWnd*)lParam);
		if (pEdit && pEdit->GetItem())
		{
			// Custom Validation
			// Custom Validation
			if (pEdit->GetItem()->GetCaption() == _T("ItemsInMRUList"))
			{
				CString str;
				pEdit->CEdit::GetWindowText(str);

				int i = _ttoi(str);
				if (i > 20)
				{
					MessageBeep((UINT)-1);
					pEdit->SetSel(0, -1);
					pEdit->ReplaceSel(_T("20"));
				}
			}
		}
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////
void gkMaterialEditor::OnBnClickedMaterialSet()
{
	IGameObject* pSelect = GetIEditor()->getObjectMng()->getCurrSelection();
	if (pSelect)
	{
		IGameObjectRenderLayer* pRL = pSelect->getRenderLayer();
		if (pRL)
		{
			pRL->setMaterialName( gkStdString( m_curSelMtlName ) );
		}
	}
}


void gkMaterialEditor::OnBnClickedMaterialGet()
{
	// get cur object mtl
	IGameObject* pSelect = GetIEditor()->getObjectMng()->getCurrSelection();
	if (pSelect)
	{
		IGameObjectRenderLayer* pRL = pSelect->getRenderLayer();
		if (pRL)
		{
			IMaterial* pMtl = pRL->getMaterial();
			if (pMtl)
			{
				MatItemSet::iterator it = m_items.begin();
				for (; it != m_items.end(); ++it )
				{
					if( (*it)->m_pMaterial == pMtl )
					{
						m_treeMaterialList.SelectItem( (*it)->m_hItem );
						m_treeMaterialList.Expand((*it)->m_hItem,TVE_EXPAND);
						RefreshMtl( (*it)->m_pMaterial );
						return;
					}
				}
			}
		}
	}
}

void gkMaterialEditor::OnBnClickedMatedtrefresh()
{
	// TODO: Add your control notification handler code here
	RefreshMaterialList();
}


void gkMaterialEditor::OnBnClickedMaterialSaveAll()
{
	gEnv->pSystem->getMaterialMngPtr()->saveAllMaterial();
}


void gkMaterialEditor::OnEmptyCommandRange(UINT ID)
{
	switch (ID)
	{
	case ID_BUTTON_MATEDIT_GET:
		OnBnClickedMaterialGet();
		return;
	case ID_BUTTON_MATEDIT_SET:
		OnBnClickedMaterialSet();
		return;
	case ID_BUTTON_MATEDT_REFRESH:
		OnBnClickedMatedtrefresh();
		return;
	case ID_BUTTON_MATEDT_CATE:
		OnBnClickedMaterialSaveAll();
		return;
	}
}
//////////////////////////////////////////////////////////////////////////
void gkMaterialEditor::RefreshMtl( IMaterial* curMtl )
{
	m_curMtl = curMtl;

	m_wndPropertyGrid.ResetContent();
	m_curSelMtlName = curMtl->getName().c_str();
	gkMaterialParams* pParas = curMtl->getLoadingParameterBlock();
	if (pParas)
	{

		// create global settings category.
		CXTPPropertyGridItem* pShader		= m_wndPropertyGrid.AddCategory( _T("Shader"));
		CXTPPropertyGridItem* pColors		= m_wndPropertyGrid.AddCategory(_T("材质颜色"));
		CXTPPropertyGridItem* pVars			= m_wndPropertyGrid.AddCategory(_T("材质参数"));
		CXTPPropertyGridItem* pTextures     = m_wndPropertyGrid.AddCategory(_T("材质纹理"));
		CXTPPropertyGridItem* pMarcos     = m_wndPropertyGrid.AddCategory(_T("材质属性"));

		//m_wndPropertyGrid.AddStringVar( pShader, _T("Shader"), pParas->m_wstrFxFileName.c_str(), _T("渲染着色器") );

		CXTPPropertyGridItemEnum* shader_enum = (CXTPPropertyGridItemEnum*)(pShader->AddChildItem( new CXTPPropertyGridItemEnum( _T("Shader") ) ) );
		shader_enum->SetID(MATEDT_SHADERSET);
		shader_enum->GetConstraints()->AddConstraint( m_curMtl->getShaderName().c_str() );

		// 可编辑gfx可能存放在三个地方

		// engine/shaders/template/built_in		内建
		// engine/shaders/template/extern			扩展
		gkStdString shaderPath = gkGetExecRootDir() + _T("engine/shaders/template/built_in/");
		std::vector<gkStdString> files;
		enum_all_files_in_folder( shaderPath.c_str(), files, false );
		shaderPath = gkGetExecRootDir() + _T("engine/shaders/template/extern/");
		enum_all_files_in_folder( shaderPath.c_str(), files, false );
		for (int i=0; i < files.size(); ++i)
		{
			if ( gkCheckFileExtension( files[i].c_str(), _T("gfx")  ) )
			{
				gkStdString purename = gkGetPureFilename( files[i].c_str() );
				shader_enum->GetConstraints()->AddConstraint( purename.c_str() );
			}
		}



		//gkShaderPtr realShader = curMtl->getShader();
		//pParas = realShader->getShaderDefaultParams();

		m_curSelMtlParaNum = pParas->m_vecParams.size();
		for (uint32 i=0; i < pParas->m_vecParams.size(); ++i)
		{
			TCHAR wszBuffer[MAX_PATH];
#ifdef UNICODE
MultiByteToWideChar( CP_ACP, 0, pParas->m_vecParams[i]->pParamName, -1, wszBuffer, MAX_PATH );
#else
			_tcscpy_s( wszBuffer, MAX_PATH, pParas->m_vecParams[i]->pParamName);
#endif // UNICODE
			
			

			switch(pParas->m_vecParams[i]->Type)
			{
			case GKSHADERT_FLOAT:
				{
					float minvalue = pParas->m_vecParams[i]->minValue;
					float maxvalue = pParas->m_vecParams[i]->maxValue;

					// add temporary param range [11/18/2011 Kaiming]
					if ( !strcmp( pParas->m_vecParams[i]->pParamName, "g_Fresnel"))
					{
						m_wndPropertyGrid.AddFloatVar(pVars, _T("菲涅尔强度"), MATEDT_PARAMSTART + i, (float*)(pParas->m_vecParams[i]->pValue), minvalue, maxvalue, _T("控制反射随平面衰减的程度"));
					}
					else if (!strcmp( pParas->m_vecParams[i]->pParamName, "g_FresnelScale"))
					{
						m_wndPropertyGrid.AddFloatVar(pVars, _T("菲涅尔增益"), MATEDT_PARAMSTART + i, (float*)(pParas->m_vecParams[i]->pValue), minvalue, maxvalue, _T("控制菲涅尔的增益"));
					}
					else if (!strcmp( pParas->m_vecParams[i]->pParamName, "g_FresnelBia"))
					{
						m_wndPropertyGrid.AddFloatVar(pVars, _T("菲涅尔渗透"), MATEDT_PARAMSTART + i, (float*)(pParas->m_vecParams[i]->pValue), minvalue, maxvalue, _T("控制菲涅尔的渗透范围"));
					}
					else if (!strcmp( pParas->m_vecParams[i]->pParamName, "g_SpecularPow"))
					{
						m_wndPropertyGrid.AddFloatVar(pVars, _T("光泽度"), MATEDT_PARAMSTART + i, (float*)(pParas->m_vecParams[i]->pValue), minvalue, maxvalue, _T("控制材质的光泽度"));
					}
					else if (!strcmp( pParas->m_vecParams[i]->pParamName, "g_fAOScaler"))
					{
						m_wndPropertyGrid.AddFloatVar(pVars, _T("植被AO倍增"), MATEDT_PARAMSTART + i, (float*)(pParas->m_vecParams[i]->pValue), minvalue, maxvalue, _T("控制植被AO倍增"));
					}
					else
					{
						m_wndPropertyGrid.AddFloatVar(pVars, wszBuffer, MATEDT_PARAMSTART + i, (float*)(pParas->m_vecParams[i]->pValue), minvalue, maxvalue, _T("描述为空"));
					}
				}

				break;
			case GKSHADERT_FLOAT2:
				//m_wndPropertyGrid.AddFloatVar(pVars, wszBuffer, MATEDT_PARAMSTART + i, (float*)(pParas->m_vecParams[i]->pValue), 0, 1, _T(""));
				break;
			case GKSHADERT_FLOAT3:  
				//m_wndPropertyGrid.AddFloatVar(pVars, wszBuffer, MATEDT_PARAMSTART + i, (float*)(pParas->m_vecParams[i]->pValue), 0, 1, _T(""));
				break;
			case GKSHADERT_FLOAT4:
				if ( !strcmp( pParas->m_vecParams[i]->pParamName, "g_Diffuse"))
				{
					m_wndPropertyGrid.AddColorVar(pColors, _T("漫反射颜色"), MATEDT_PARAMSTART + i, (float*)(pParas->m_vecParams[i]->pValue), _T(""), this);
				}
				else if ( !strcmp( pParas->m_vecParams[i]->pParamName, "g_Specular"))
				{
					m_wndPropertyGrid.AddColorVar(pColors, _T("高光颜色"), MATEDT_PARAMSTART + i, (float*)(pParas->m_vecParams[i]->pValue), _T(""), this);
				}
				else
				{
					m_wndPropertyGrid.AddColorVar(pColors, wszBuffer, MATEDT_PARAMSTART + i, (float*)(pParas->m_vecParams[i]->pValue), _T(""), this);
				}
				break;
// 			case GKSHADERT_DWORD: 
// 				{
// 					CXTPPropertyGridItemNumber* pDword = (CXTPPropertyGridItemNumber*)(pVars->AddChildItem(new CXTPPropertyGridItemNumber(wszBuffer)));
// 					pDword->SetID(MATEDT_PARAMSTART + i);
// 					pDword->AddSliderControl();
// 					pDword->AddSpinButton();
// 				}
// 
// 				break;
			}
		}

		// final add texUV slider [11/19/2011 Kaiming]
		float& fU = curMtl->getUVTill().x;
		float& fV = curMtl->getUVTill().y;
		m_wndPropertyGrid.AddFloatVar(pTextures, _T("UV重复X"), MATEDT_PARAMSSTAT, &fU, 0, 50, _T("横向UV重复"));
		m_wndPropertyGrid.AddFloatVar(pTextures, _T("UV重复Y"), MATEDT_PARAMSSTAT + 1, &fV, 0, 50, _T("纵向UV重复"));


		m_wndPropertyGrid.AddBoolVar( pMarcos, _T("投射阴影"), 0, &(curMtl->getCastShadow()), _T("是否投射阴影"));


		// read texture
		for (int i=0; i < eMS_Invalid; ++i)
		{
			gkTexturePtr tex = curMtl->getTexture((EMaterialSlot)i);

			if (!tex.isNull())
			{
				CCustomItemFileBox* texture = (CCustomItemFileBox*)(pTextures->AddChildItem(new CCustomItemFileBox( MaterialSlotNames[i] )));
				texture->OnValueChanged(CString((TCHAR*)( tex->getName().c_str() )));
				texture->SetID(MATEDT_TEXSET + i);
				texture->GetCaptionMetrics()->m_nImage = 7;
			}
			else
			{
				CCustomItemFileBox* texture = (CCustomItemFileBox*)(pTextures->AddChildItem(new CCustomItemFileBox( MaterialSlotNames[i] )));
				//texture->OnValueChanged(CString((TCHAR*)( tex->getName().c_str() )));
				texture->SetID(MATEDT_TEXSET + i);
				texture->GetCaptionMetrics()->m_nImage = 7;
			}

		}
		


		// read marco
		ShaderMacros& macros = curMtl->getShaderMarcos();
		uint32 macroMask = curMtl->getShaderMarcoMask();

		ShaderMacros::iterator it = macros.begin();
		for (; it != macros.end(); ++it)
		{
			if (!it->second.m_hidden)
			{
				m_wndPropertyGrid.AddBoolVar( pMarcos, it->second.m_desc.c_str(), MATEDT_MACROSET, &(it->second.m_active), _T("材质属性设置")  );
			}
		}

		m_wndPropertyGrid.ExpandAll();
	}
}
//////////////////////////////////////////////////////////////////////////
void gkMaterialEditor::AddMaterialIntoTree( IMaterial* mat )
{
	TCHAR fullpath[MAX_PATH];
	_tcscpy_s( fullpath, mat->getName().c_str() );

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
			hRoot = m_treeMaterialList.InsertItem( tokenName, ITEM_IMAGE_FOLDER, ITEM_IMAGE_FOLDER_OPEN, hRoot );
			m_treeMaterialList.SetItemBold(hRoot);
			m_namemap[tokenName] = hRoot;
		}
	}

	InsertMaterialIntoTree(mat, hRoot);
}
//////////////////////////////////////////////////////////////////////////
void gkMaterialEditor::InsertMaterialIntoTree( IMaterial* mat, HTREEITEM parent, int32 nMatSlot /*= -1*/ )
{
	CString itemText;

	// submaterial [5/11/2012 Kaiming]
	HTREEITEM hItem;
	if (mat->getSubMtlCount() > 0)
	{
		hItem = m_treeMaterialList.InsertItem( mat->getShortName().c_str(),ITEM_IMAGE_MULTI_MATERIAL,ITEM_IMAGE_MULTI_MATERIAL_SELECTED,parent );
		m_treeMaterialList.SetItemBold(hItem);

		for (int i=0; i < mat->getSubMtlCount(); ++i)
		{
			InsertMaterialIntoTree( mat->getSubMaterial(i),hItem,i );
		}
	}
	else
	{
		hItem = m_treeMaterialList.InsertItem(mat->getShortName().c_str(), ITEM_IMAGE_MATERIAL,ITEM_IMAGE_MATERIAL_SELECTED, parent);
		m_treeMaterialList.SetItemBold(hItem);
	}

	SMatItemInfo* pItemInfo = new SMatItemInfo;
	pItemInfo->m_hItem = hItem;
	pItemInfo->m_pMaterial = mat;

	m_items.push_back(pItemInfo);
	m_treeMaterialList.SetItemData( hItem,(DWORD_PTR)(pItemInfo) );
}

afx_msg void gkMaterialEditor::OnTreeBrowserClick( NMHDR *pNMHDR, LRESULT *pResult )
{

	*pResult = FALSE;

	// Show helper menu.
	CPoint point;
	// Find node under mouse.
	GetCursorPos( &point );
	m_treeMaterialList.ScreenToClient( &point );
	// Select the item that is at the point myPoint.
	UINT uFlags;
	HTREEITEM hItem = m_treeMaterialList.HitTest(point,&uFlags);
	if (hItem)
	{
		SMatItemInfo* pItem = (SMatItemInfo*)(m_treeMaterialList.GetItemData(hItem));
		if (pItem && pItem->m_pMaterial)
		{
			RefreshMtl( pItem->m_pMaterial );
		}
	}


	*pResult = 0;
}
//////////////////////////////////////////////////////////////////////////
void gkMaterialEditor::OnSize( UINT nType, int cx, int cy )
{
	gkToolBarPaneBase::OnSize(nType,cx,cy);
}

void gkMaterialEditor::OnColorChange( COLORREF clr )
{
	//gEnv->pSystem->getMaterialMngPtr()->reload_s(gkStdString(m_curSelMtlName));		
	gEnv->pSystem->Update();
	gEnv->pSystem->PostUpdate();
}
