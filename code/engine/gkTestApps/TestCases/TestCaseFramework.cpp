#include "TestCaseFramework.h"
#include "IXmlUtil.h"
#include "ISystem.h"
#include "IGameObject.h"
#include "IGameObjectLayer.h"
#include "ITimer.h"


#ifndef _STATIC_LIB
#include "gkPlatform_impl.h"
#endif
#include "I3DEngine.h"
#include "ICamera.h"
#include "ITimeOfDay.h"
#include "IRenderer.h"
#include "IAuxRenderer.h"
#include "IGameFramework.h"
#include "ITerrianSystem.h"
#include "ITask.h"
#include "IFont.h"

#define MAIN_CATEGORY_X 58
#define SUB_CATEGORY_X 288

#define START_POS_Y 80
#define ELEMENT_HEIGHT 35
#define ELEMENT_WIDTH 100

TestCaseFramework* g_Game;

const TCHAR* s_category[] = {
	_T("Loading"),
	_T("Animation"),
	_T("Rendering"),
	_T("Performance"),
    _T("System"),
    _T("Preserve"),
    _T("Preserve"),
    _T("Preserve"),
};

#ifndef _STATIC_LIB

extern "C" DLL_EXPORT IGame* gkModuleInitialize(SSystemGlobalEnvironment* pEnv)
{
	if (pEnv)
	{
		gEnv = pEnv;
		g_Game = new TestCaseFramework();
		return g_Game;
	}
	return NULL;
}

extern "C" DLL_EXPORT void gkModuleUnload(void)
{
	SAFE_DELETE( g_Game );
}

#else

IGame* gkLoadStaticModule_gkGame( SSystemGlobalEnvironment* pEnv )
{
	if (pEnv)
	{
		gEnv = pEnv;
		g_Game = new TestCaseFramework();
		return g_Game;
	}
}

void gkFreeStaticModule_gkGame()
{
	SAFE_DELETE( g_Game );
}

#include "TestCase_StaticLoader.h"

#endif

TestCases g_cateTestCases[eTcc_Count];

TestCaseFramework::TestCaseFramework( void )
{
	m_testCase_Point = 0;
	m_testCase_sub_Point = 0;
	m_runningCase = NULL;
	m_widget = 0;
}

TestCaseFramework::~TestCaseFramework( void )
{

}

bool TestCaseFramework::OnInit()
{
#ifdef _STATIC_LIB
    load_static_testcases();
#endif
	// set camera
	ICamera* maincam = gEnv->p3DEngine->getMainCamera();
	maincam->setPosition(Vec3(0,-8,2.f));
	//maincam->setFOVy(DEG2RAD(45.f));
	maincam->setOrientation(Quat::CreateRotationXYZ(Ang3(0,0,0)));
	maincam->setNearPlane(0.05f);
	maincam->setFarPlane(4000.f);

	// set env setting
	gEnv->p3DEngine->getTimeOfDay()->loadTODSequence(_T("engine/config/default.tod"), true);

	// create skysphere
	Vec3 pos = Vec3(0,0,-0.0005);
	Quat rot = Quat::CreateIdentity();

	IGameObject* sky = gEnv->pGameObjSystem->CreateStaticGeoGameObject( _T("skysphere"), _T("engine/assets/meshs/skysphere.obj"), pos, rot);
	if (sky)
	{
		IGameObjectRenderLayer* pRenderLayer = sky->getRenderLayer();
		if (pRenderLayer)
		{
			pRenderLayer->setMaterialName(_T("engine/assets/skysphere.mtl"));
			pRenderLayer->setRenderPipe( RENDER_LAYER_SKIES_EARLY );
			pRenderLayer->enableCastShadow(false);
		}

		sky->setScale(2000,2000,2000);
	}

	m_mainFont = gEnv->pFont->CreateFont( _T("engine/fonts/msyh.ttf"), 60, GKFONT_OUTLINE_0PX, _T("engine/fonts/segoeuil.ttf") );
	m_menuFont = gEnv->pFont->CreateFont( _T("engine/fonts/msyh.ttf"), 35, GKFONT_OUTLINE_0PX, _T("engine/fonts/segoeuil.ttf") );

	gEnv->pInputManager->addInputEventListener(this);

	//gEnv->pCVManager->executeCommand( _T("sys_displayStats 0") );

	for (uint32 i=0; i < eTcc_Count; ++i)
	{
		if ( g_cateTestCases[i].empty() )
		{
			g_cateTestCases[i].clear();
		}
	}


	for (uint32 i=0; i < eTcc_Count; ++i)
	{
		RectF rect( MAIN_CATEGORY_X, gEnv->pRenderer->GetScreenHeight() * 0.3f + START_POS_Y + i * ELEMENT_HEIGHT , ELEMENT_WIDTH, 29 );
		m_buttons.push_back(rect);
	}

	return true;
}

bool TestCaseFramework::OnDestroy()
{
	return true;
}

bool TestCaseFramework::OnUpdate()
{
	if (!m_runningCase)
	{
		UpdateGUI(true);
		Vec3 zero(0,0,0);
		ColorF color(0,0,0,0.3);
		gEnv->pRenderer->getAuxRenderer()->AuxRender3DGird(zero, 50, 0.1f, color);
	}
	
	if (m_runningCase)
	{
		UpdateGUI(false);
		if (m_runningCase->m_status == eTS_stop)
		{
			m_runningCase->Init();
			return true;
		}

		if( !m_runningCase->Update() )
		{
			m_runningCase->Destroy();
			m_runningCase = NULL;

			gEnv->pSystem->cleanGarbage();

			RestoreSetting();
		}
	}

	return true;
}

bool TestCaseFramework::OnLevelLoaded()
{
	return true;
}

bool TestCaseFramework::OnInputEvent( const SInputEvent &event )
{
	switch ( event.deviceId )
	{
	case eDI_Keyboard:
		{
			if (event.keyId == eKI_Up && event.state == eIS_Pressed)
			{
				if (m_widget)
				{
					m_testCase_sub_Point--;
					if (m_testCase_sub_Point < 0)
					{
						m_testCase_sub_Point = 0;
					}
				}
				else
				{
					m_testCase_Point--;
					if (m_testCase_Point < 0)
					{
						m_testCase_Point = 0;
					}
				}

			}
			if (event.keyId == eKI_Down && event.state == eIS_Pressed)
			{
				if (m_widget)
				{
					m_testCase_sub_Point++;
					if (m_testCase_sub_Point >= g_cateTestCases[m_testCase_Point].size())
					{
						m_testCase_sub_Point = g_cateTestCases[m_testCase_Point].size() - 1;
					}
				}
				else
				{
					m_testCase_Point++;
					if (m_testCase_Point >= eTcc_Count)
					{
						m_testCase_Point = eTcc_Count - 1;
					}
				}

			}
			if(event.keyId == eKI_Right && event.state == eIS_Pressed)
			{
                IntoSubSection();
			}
			if(event.keyId == eKI_Left && event.state == eIS_Pressed)
			{
                IntoMainSection();
			}
			if (event.keyId == eKI_Enter && event.state == eIS_Pressed)
			{
				if (m_widget == 1)
				{
					m_runningCase = g_cateTestCases[m_testCase_Point][m_testCase_sub_Point];
				}
				
			}
			if (event.keyId == eKI_Escape && event.state == eIS_Pressed)
			{
				if (m_runningCase)
				{
					m_runningCase->Destroy();
					m_runningCase = NULL;

					gEnv->pSystem->cleanGarbage();

					RestoreSetting();
				}
			}
			
		}
		break;
	case eDI_Android:
	case eDI_IOS:
		{
			if (event.keyId == eKI_Android_Touch && event.state == eIS_Released)
			{
				Vec2 touchpos(event.value, event.value2);

				OnTouch( touchpos );
				return false;
			}
		}
		break;
	case eDI_Mouse:
		{
			if (event.keyId == eKI_Mouse1 && event.state == eIS_Released)
			{
				Vec2 touchpos(event.value, event.value2);

				OnTouch( touchpos );
			}
		}
		break;
	default:
		break;
	}

	if (m_runningCase)
	{
		m_runningCase->InputEvent(event);
	}

	return true;
}

void TestCaseFramework::IntoSubSection()
{
    m_widget++;
    if (m_widget > 1)
    {
        m_widget = 1;
    }
    
    if (m_widget == 1)
    {
        m_section_buttons.clear();
        for (uint32 i=0; i < g_cateTestCases[m_testCase_Point].size(); ++i)
        {
            RectF rect(SUB_CATEGORY_X, gEnv->pRenderer->GetScreenHeight() * 0.3f + START_POS_Y + i * ELEMENT_HEIGHT, 400,29);
            m_section_buttons.push_back(rect);
        }
        m_testCase_sub_Point = 0;
    }
}

void TestCaseFramework::IntoMainSection()
{
    m_widget--;
    if (m_widget < 0)
    {
        m_widget = 0;
    }
}

void TestCaseFramework::UpdateGUI(bool menuMode)
{
	if (menuMode)
	{
		gEnv->pRenderer->getAuxRenderer()->AuxRenderScreenBox( Vec2(0, 0), Vec2(gEnv->pRenderer->GetScreenWidth(), gEnv->pRenderer->GetScreenHeight() * 0.3 + 60), ColorB(0,0,0,128) );
		gEnv->pRenderer->getAuxRenderer()->AuxRenderText( _T("GAMEKNIFE TESTCASES"), 35, gEnv->pRenderer->GetScreenHeight() * 0.3f, m_mainFont );

		for (uint32 i=0; i < eTcc_Count; ++i)
		{
			if ( m_testCase_Point == i)
			{
				TCHAR text[512];
				_stprintf( text, _T("> %s"), s_category[i] );
				gEnv->pRenderer->getAuxRenderer()->AuxRenderText( text, MAIN_CATEGORY_X - 40, gEnv->pRenderer->GetScreenHeight() * 0.3f + START_POS_Y + i * ELEMENT_HEIGHT, m_menuFont, ColorB(255,255,255,255) );
			}
			else
			{
				gEnv->pRenderer->getAuxRenderer()->AuxRenderText( s_category[i], MAIN_CATEGORY_X, gEnv->pRenderer->GetScreenHeight() * 0.3f + START_POS_Y + i * ELEMENT_HEIGHT, m_menuFont, ColorB(0,0,0,128) );
			}

		}

		if (m_widget == 1)
		{
			for (uint32 i=0; i < g_cateTestCases[m_testCase_Point].size(); ++i)
			{
				TCHAR text[512];
				if ( m_testCase_sub_Point == i)
				{
					_stprintf( text, _T("> %s"), g_cateTestCases[m_testCase_Point][i]->GetName() );
					gEnv->pRenderer->getAuxRenderer()->AuxRenderText( text, SUB_CATEGORY_X - 40, gEnv->pRenderer->GetScreenHeight() * 0.3f + START_POS_Y + i * ELEMENT_HEIGHT, m_menuFont, ColorB(255,255,255,255) );
				}
				else
				{
					_stprintf( text, _T("%s"), g_cateTestCases[m_testCase_Point][i]->GetName());
					gEnv->pRenderer->getAuxRenderer()->AuxRenderText(text , SUB_CATEGORY_X, gEnv->pRenderer->GetScreenHeight() * 0.3f + START_POS_Y + i * ELEMENT_HEIGHT, m_menuFont, ColorB(0,0,0,128) );
				}


			}
		}

	}
	else
	{
// 		gEnv->pRenderer->getAuxRenderer()->AuxRenderScreenBox( Vec2(0, 0), Vec2(gEnv->pRenderer->GetScreenWidth(), 50), ColorB(0,0,0,128) );
// 		TCHAR buffer[MAX_PATH];
// 		_stprintf(buffer, _T("%s running"), m_runningCase->GetName());
// 		gEnv->pRenderer->getAuxRenderer()->AuxRenderText( buffer, gEnv->pRenderer->GetScreenWidth() - 10, 10, m_menuFont, ColorB(255,255,255,200), eFA_Right | eFA_Top );
// 		
		//gEnv->pRenderer->getAuxRenderer()->AuxRenderText( _T("PRESS ESC TO RETURN"), gEnv->pRenderer->GetScreenWidth() - 300, 10, m_menuFont );
	}

	
}

void TestCaseFramework::RestoreSetting()
{
	// set camera
	ICamera* maincam = gEnv->p3DEngine->getMainCamera();
	maincam->setPosition(Vec3(0,-8,2.f));
	//maincam->setFOVy(DEG2RAD(45.f));
	maincam->setOrientation(Quat::CreateRotationXYZ(Ang3(0,0,0)));
	maincam->setNearPlane(0.05f);
	maincam->setFarPlane(4000.f);

	// set env setting
	gEnv->p3DEngine->getTimeOfDay()->loadTODSequence(_T("engine/config/default.tod"), true);
}


void TestCaseFramework::OnTouch( const Vec2& touchpos )
{
	if( gEnv->pTimer->GetCurrTime() < 1.0f ) return;
	if (!m_runningCase)
	{
		gkLogMessage(_T("try to touch: %.2f %.2f"), touchpos.x, touchpos.y);

		for (uint32 i=0; i < m_buttons.size(); ++i)
		{
			if( m_buttons[i].Insection( touchpos ) )
			{
                m_testCase_Point = i;
                IntoSubSection();
				//m_runningCase = g_testCases[i];
			}
		}
        
        for (uint32 i=0; i < m_section_buttons.size(); ++i)
		{
			if( m_section_buttons[i].Insection( touchpos ) )
			{
				//m_runningCase = g_testCases[i];
                m_testCase_sub_Point = i;
                m_runningCase = g_cateTestCases[m_testCase_Point][m_testCase_sub_Point];
			}
		}
	}
	else
	{
		RectF rectclose( gEnv->pRenderer->GetScreenWidth() - 50, 0, 50, 50 );
		if (rectclose.Insection( touchpos ))
		{
			m_runningCase->Destroy();
			m_runningCase = NULL;

			gEnv->pSystem->cleanGarbage();

			RestoreSetting();
		}


	}

}

