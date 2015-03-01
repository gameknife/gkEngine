#include "StdAfx.h"
#include "SwissKnifePaneModify.h"
#include "IGameObject.h"
#include "IGameObjectLayer.h"
#include "IHavok.h"

#define ID_CREATE_PHYSIC_LAYER 512

BEGIN_MESSAGE_MAP(CSwissKnifePaneModify, CXTPShortcutBarPane)
	//{{AFX_MSG_MAP(CMainFrame)
	//ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
	ON_MESSAGE(XTPWM_PROPERTYGRID_NOTIFY, OnGridNotify)

END_MESSAGE_MAP()
//////////////////////////////////////////////////////////////////////////
CSwissKnifePaneModify::CSwissKnifePaneModify()
{

}
//////////////////////////////////////////////////////////////////////////
CSwissKnifePaneModify::~CSwissKnifePaneModify()
{

}
//////////////////////////////////////////////////////////////////////////
BOOL CSwissKnifePaneModify::Create( LPCTSTR lpszCaption, CXTPShortcutBar* pParent )
{
	if (!CXTPShortcutBarPane::Create(lpszCaption, pParent))
		return FALSE;

	if ( m_wndPropertyGridBasic.Create( CXTPEmptyRect(), this, 0 ) )
	{
		m_wndPropertyGridBasic.SetVariableItemsHeight(TRUE);
		m_wndPropertyGridBasic.SetOwner(this);
		m_wndPropertyGridBasic.ShowHelp(FALSE);
	}

	if ( m_wndPropertyGridSpecific.Create( CXTPEmptyRect(), this, 1 ) )
	{
		m_wndPropertyGridSpecific.SetVariableItemsHeight(TRUE);
		m_wndPropertyGridSpecific.SetOwner(this);
		m_wndPropertyGridSpecific.ShowHelp(FALSE);
	}

	AddItem(_T("Basic"), &m_wndPropertyGridBasic, 150);
	AddItem(_T("Specific"), &m_wndPropertyGridSpecific, 125);

	return TRUE;
}
//////////////////////////////////////////////////////////////////////////
void CSwissKnifePaneModify::UpdateTheme( BOOL bOffice2010Theme )
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
}
//////////////////////////////////////////////////////////////////////////
void CSwissKnifePaneModify::RefreshProperties( IGameObject* pTarget )
{
	m_currTarget = pTarget;

	m_wndPropertyGridBasic.ResetContent();
	m_wndPropertyGridSpecific.ResetContent();

	if (!pTarget)
	{
		return;
	}

	

	SetCaption( pTarget->getName().c_str() );

	// 添加基础信息 [4/4/2013 Kaiming]
	CXTPPropertyGridItem* info = m_wndPropertyGridBasic.AddCategory(_T("BasicInfo"));
	m_wndPropertyGridBasic.AddStringVar( info, _T("Name"), pTarget->getName().c_str(), _T("GameObjectName") );
	m_wndPropertyGridBasic.AddStringVar( info, _T("Class"), pTarget->getGameObjectClassName().c_str(), _T("GameObjectClassName") );
	
	// 分层读取

	// 几何
	IGameObjectRenderLayer* renderLayer = pTarget->getRenderLayer();
	if (renderLayer)
	{
		CXTPPropertyGridItem* geoinfo = m_wndPropertyGridSpecific.AddCategory(_T("Geometry"));

		m_wndPropertyGridSpecific.AddFileVar( geoinfo, _T("material"), renderLayer->getMaterialName().c_str(), _T("matrerial name")  );
	}

	// 物理
	IGameObjectPhysicLayer* physicLayer = pTarget->getPhysicLayer();
	//if (physicLayer)
	{
		CXTPPropertyGridItem* phyinfo = m_wndPropertyGridSpecific.AddCategory(_T("Physics"));
// 		if (physicLayer)
// 		{
			//m_wndPropertyGridSpecific.AddIntegerVar( phyinfo, _T("phy_type"), 0, &(physicLayer->getPhysicType()), _T("physic type") );

			m_wndPropertyGridSpecific.AddBoolVar( phyinfo, _T("enable"), ID_CREATE_PHYSIC_LAYER, pTarget->getPhysicEnable(), _T(""));
// 		}
// 		else
// 		{
// 
// 		}
	}

	// 动作
	IGameObjectAnimLayer* animLayer = static_cast<IGameObjectAnimLayer*>(pTarget->getGameObjectLayer( eGL_AnimLayer ));
	if (animLayer)
	{
		CXTPPropertyGridItem* aniinfo = m_wndPropertyGridSpecific.AddCategory(_T("Animation"));
	}

	// 灯光
	IGameObjectLightLayer* lightLayer = static_cast<IGameObjectLightLayer*>(pTarget->getGameObjectLayer( eGL_LightLayer ) );
	if (lightLayer)
	{
		CXTPPropertyGridItem* lightinfo = m_wndPropertyGridSpecific.AddCategory(_T("Light"));

		m_wndPropertyGridSpecific.AddFloatVar( lightinfo, _T("Radius"), 0, lightLayer->getRadiusPtr_Editor(), 0, 50, _T("光源半径") );
		m_wndPropertyGridSpecific.AddColorVar( lightinfo, _T("Color"), 1, lightLayer->getDiffuseColorPtr_Editor(), _T("光源颜色") );
		m_wndPropertyGridSpecific.AddFloatVar( lightinfo, _T("Intensity"), 2, lightLayer->getLightIntensityPtr_Editor(), 0, 100, _T("光源亮度"));
		m_wndPropertyGridSpecific.AddFloatVar( lightinfo, _T("IntensitySpec"), 3, lightLayer->getLightIntensitySpecPtr_Editor(), 0, 100, _T("光源高光亮度"));
		m_wndPropertyGridSpecific.AddBoolVar( lightinfo, _T("FakeShadow"), 4, lightLayer->getFakeShadowPtr_Editor(), _T("假阴影"));
		m_wndPropertyGridSpecific.AddBoolVar( lightinfo, _T("GlobalShadow"), 5, lightLayer->getGlobalShadowPtr_Editor(), _T("全局阴影"));
	}


	m_wndPropertyGridBasic.ExpandAll();
	m_wndPropertyGridSpecific.ExpandAll();
}

LRESULT CSwissKnifePaneModify::OnGridNotify( WPARAM wParam, LPARAM lParam )
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


		if( pItem->GetID() == ID_CREATE_PHYSIC_LAYER)
		{
			CXTPPropertyGridItemBool* itembool = (CXTPPropertyGridItemBool*)pItem;
			if (m_currTarget && itembool->GetBool())
			{
				if (!m_currTarget->getPhysicLayer())
				{
#ifdef OS_WIN32

						IGameObjectPhysicLayer* pPhysicLayer = gEnv->pPhysics->CreatePhysicLayer();
						m_currTarget->setGameObjectLayer(pPhysicLayer);
						pPhysicLayer->createStatic();

						
#endif

				}
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
