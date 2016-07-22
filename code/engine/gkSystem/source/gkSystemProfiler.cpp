#include "gkSystemStableHeader.h"
#include "gkSystemProfiler.h"
#include "IRenderer.h"
#include "IAuxRenderer.h"
#include "ICamera.h"
#include "IResourceManager.h"
#include "IMaterial.h"
#include "ITask.h"
#include "IMesh.h"

#define MAKE_SWITCH( x ) \
	gkCVar* var = gEnv->pCVManager->getCVar(_T(#x));\
	if( var && var->getInt() == 0 )\
		{\
		buttonBgColor = ColorB(0,0,0,150);\
		_stprintf(buffer, ( _T(#x) _T(" 1")));\
		}\
		else\
		{\
		buttonBgColor = ColorB(40,127,200,150);\
		_stprintf(buffer, ( _T(#x) _T(" 0")));\
		}

//-----------------------------------------------------------------------
gkSystemProfiler::gkSystemProfiler( void )
{
	m_uDPCurrentFrame = 0;
	m_fFrameTimer = 0;
	m_fFrameTimeRendering = 0;
	m_profilerFont1 = NULL;
	m_profilerFont = NULL;
	m_profilerFPSFont = NULL;
	m_titleFont = NULL;
	m_subtitleFont = NULL;

	m_fFrameTimerFWD =0;
	m_fFrameTimeRenderingFWD =0;

	m_fFrameTimerDFD =0;
	m_fFrameTimeRenderingDFD =0;

	m_fFrameTimerCMT =0;
	m_fFrameTimeRenderingCMT =0;

	m_uFrameCounter = 0;

	m_CurveRecorder = 0;

	for (int i=0; i < 1000; ++i)
	{
		m_fFrameGPUTimeCurve[i] = 0;
		m_fFrameTimeRenderingCurve[i] = 0;
		m_fFramwTimeCurve[i] = 0;
	}
}
//-----------------------------------------------------------------------
void gkSystemProfiler::setCurrFrameDP( uint32 frameDP )
{
	m_uDPCurrentFrame = frameDP;
}
void gkSystemProfiler::setCurrFrameShadowDP( uint32 frameShadowDP )
{
	m_uShadowDPCurrentFrame = frameShadowDP;
}
//-----------------------------------------------------------------------
void gkSystemProfiler::setStartRendering()
{
	m_fFrameTimer = gEnv->pTimer->GetAsyncCurTime();
}
//-----------------------------------------------------------------------
void gkSystemProfiler::setEndRendering()
{
	m_fFrameTimeRendering = gEnv->pTimer->GetAsyncCurTime() - m_fFrameTimer;
	m_uFrameCounter++;
	m_uFrameCounter %= 1000000;
}
//-----------------------------------------------------------------------
void gkSystemProfiler::setStartWaiting()
{
	m_fFrameTimerFWD = gEnv->pTimer->GetAsyncCurTime();
}
//-----------------------------------------------------------------------
void gkSystemProfiler::setEndWaiting()
{
	m_fFrameTimeRenderingFWD = gEnv->pTimer->GetAsyncCurTime() - m_fFrameTimerFWD;
}
//-----------------------------------------------------------------------
void gkSystemProfiler::setStartSceneManage()
{
	m_fFrameTimerDFD = gEnv->pTimer->GetAsyncCurTime();
}

//-----------------------------------------------------------------------
void gkSystemProfiler::setEndSceneManage()
{
	m_fFrameTimeRenderingDFD = gEnv->pTimer->GetAsyncCurTime() - m_fFrameTimerDFD;
}
//-----------------------------------------------------------------------
void gkSystemProfiler::setStartCommit()
{
	m_fFrameTimerCMT = gEnv->pTimer->GetAsyncCurTime();
}
//-----------------------------------------------------------------------
void gkSystemProfiler::setEndCommit()
{
	m_fFrameTimeRenderingCMT = gEnv->pTimer->GetAsyncCurTime() - m_fFrameTimerCMT;
}

void gkSystemProfiler::displayInfo()
{
	if (!m_profilerFont)
	{
		m_profilerFont = gEnv->pFont->CreateFont( _T("engine/fonts/msyh.ttf"), 12, GKFONT_OUTLINE_0PX );
	}

	if (!m_profilerFont1)
	{
		m_profilerFont1 = gEnv->pFont->CreateFont( _T("engine/fonts/msyh.ttf"), 13, GKFONT_OUTLINE_0PX, _T("engine/fonts/segoeuil.ttf") );
	}

	if (!m_profilerFPSFont)
	{
		m_profilerFPSFont = gEnv->pFont->CreateFont( _T("engine/fonts/FSEX300.ttf"),50, GKFONT_OUTLINE_0PX );
	}

	if (!m_titleFont)
	{
		m_titleFont = gEnv->pFont->CreateFont( _T("engine/fonts/msyh.ttf"), 35, GKFONT_OUTLINE_0PX, _T("engine/fonts/segoeuil.ttf") );
	}

	if(!m_subtitleFont)
	{
		m_subtitleFont = gEnv->pFont->CreateFont( _T("engine/fonts/msyh.ttf"), 20, GKFONT_OUTLINE_0PX, _T("engine/fonts/segoeuil.ttf") );
	}
	profilerGUI();


	update();
}

void gkSystemProfiler::profilerGUI()
{
	TCHAR buffer[MAX_PATH];
	_stprintf_s(buffer, MAX_PATH, _T("%2.0f"), gEnv->pTimer->GetSmoothFrameRate());



	static bool showFeature = false;
	static int highlightFeature = -1;

	ColorB buttonTextColor(180,180,180,255);
	ColorB buttonTextColorPressed(255,255,255,255);
	ColorB buttonBgColorHighlight(150,15,18,255);
	ColorB buttonBgColor(0,0,0,150);
	ColorB buttonBgColorPressed(0,0,0,250);

	ColorB greencol(114, 243, 0, 255);
	ColorB whitecol(255, 255, 255, 255);

	ColorB blackButton(0,0,0,120);

	int leftStart = 32;
	int rightStart = gEnv->pRenderer->GetScreenWidth();
	int bottomstart = gEnv->pRenderer->GetScreenHeight();

	static bool showStats = true;

	int topBarHeight = 32;
	int topBarSegment = 100;
	//gkLogMessage( _T("Wait: %.2fms"), getFrameTimeRenderingWNT() * 1000.f);
	//gEnv->pInGUI->gkGUIButton( buffer, Vec2(rightStart - 80, bottomstart - 40), 80, 40, greencol, blackButton, m_profilerFPSFont );
	//return;

	if (g_pSystemCVars->sys_displayStats)
	{
		qinfo(80);
		;
	}
	enum
	{
		tab_none,
		tab_profiler,
		tab_feature,
		tab_dyntex,
		tab_resourcemng,
		tab_about,
	};
	static int currentTab = tab_none;


	switch( currentTab )
	{
	case tab_feature:
		features();
		break;
	case tab_about:
		about();
		break;
	case tab_resourcemng:
		profileResource();
		break;
	case tab_profiler:
		benchmark();
		break;
	case tab_none:  

		break;
	}

	static gkTexturePtr tex = gEnv->pSystem->getTextureMngPtr()->loadSync( _T("Engine/Assets/Textures/default/logo30x30.tga"), _T("internal") );
	gEnv->pRenderer->getAuxRenderer()->AuxRenderScreenBox( Vec2(0,0), Vec2(32, 32), buttonBgColor );
	gEnv->pRenderer->getAuxRenderer()->AuxRenderScreenBox( Vec2(0,0), Vec2(32, 32), whitecol, tex.getPointer() );

	{
		static bool buttonSwitch = false;
		if ( gEnv->pInGUI->gkGUIButton( _T("Qinfo"), Vec2(leftStart, 0), topBarSegment, topBarHeight, 
			buttonSwitch ? buttonTextColorPressed : buttonTextColor , 
			buttonSwitch ? buttonBgColorPressed : buttonBgColor ) )
		{
			buttonSwitch = !buttonSwitch;
			g_pSystemCVars->sys_displayStats = buttonSwitch;
		}
		leftStart += topBarSegment;
	}

	{
		if ( gEnv->pInGUI->gkGUIButton( _T("Profiler"), Vec2(leftStart, 0), topBarSegment, topBarHeight, 
			currentTab == tab_profiler ? buttonTextColorPressed : buttonTextColor , 
			currentTab == tab_profiler ? buttonBgColorPressed : buttonBgColor ) )
		{
			if (currentTab == tab_profiler)
			{
				currentTab = tab_none;
			}
			else
			{
				currentTab = tab_profiler;
			}
		}
		leftStart += topBarSegment;
	}

	{
		if ( gEnv->pInGUI->gkGUIButton( _T("Feature"), Vec2(leftStart, 0), topBarSegment, topBarHeight, 
			currentTab == tab_feature ? buttonTextColorPressed : buttonTextColor , 
			currentTab == tab_feature ? buttonBgColorPressed : buttonBgColor ) )
		{
			if (currentTab == tab_feature)
			{
				currentTab = tab_none;
			}
			else
			{
				currentTab = tab_feature;
			}
		}
		leftStart += topBarSegment;
	}

	{
		if ( gEnv->pInGUI->gkGUIButton( _T("Resource"), Vec2(leftStart, 0), topBarSegment, topBarHeight, 
			currentTab == tab_resourcemng ? buttonTextColorPressed : buttonTextColor , 
			currentTab == tab_resourcemng ? buttonBgColorPressed : buttonBgColor ) )
		{
			if (currentTab == tab_resourcemng)
			{
				currentTab = tab_none;
			}
			else
			{
				currentTab = tab_resourcemng;
			}
		}
		leftStart += topBarSegment;
	}

	{
		if ( gEnv->pInGUI->gkGUIButton( _T("About"), Vec2(leftStart, 0), topBarSegment, topBarHeight, 
			currentTab == tab_about ? buttonTextColorPressed : buttonTextColor , 
			currentTab == tab_about ? buttonBgColorPressed : buttonBgColor ) )
		{
			if (currentTab == tab_about)
			{
				currentTab = tab_none;
			}
			else
			{
				currentTab = tab_about;
			}
		}
		leftStart += topBarSegment;
	}

	{
		if ( gEnv->pInGUI->gkGUIButton( _T("TEST"), Vec2(leftStart, 0), topBarSegment, topBarHeight, 
			buttonTextColor , 
			buttonBgColor ) )
		{
			gEnv->pRenderer->SavePositionCubeMap( Vec3(0,0,1.2), _T("conf_test") );
		}
		leftStart += topBarSegment;
	}

	{
		gEnv->pRenderer->getAuxRenderer()->AuxRenderScreenBox( Vec2(leftStart, 0), Vec2(rightStart - leftStart, topBarHeight), buttonBgColor );
	}
}

uint32 gkSystemProfiler::getElementCount( EProfileElement element )
{
	return m_profileElements[element].m_count;
}

void gkSystemProfiler::increaseElementCount( EProfileElement element, int count /*= 1 */ )
{
	m_profileElements[element].m_count += count;
}

void gkSystemProfiler::update()
{
	for (uint32 i=0; i < ePe_Count; ++i)
	{
		m_profileElements[i].Clear();
	}
}

#define RES_LINE_NORMAL 30

void gkSystemProfiler::profileResource()
{
	//int& leftStart, int& bottomstart, ColorB& buttonTextColor, ColorB& buttonBgColor, 

	int startY = 100;

	ColorB textColor(255,255,255,255);
	ColorB textColorHighlight(40,127,200,255);
	ColorB buttonTextColor(180,180,180,255);
	ColorB buttonTextColorPressed(255,255,255,255);
	ColorB buttonBgColorHighlight(40,127,200,150);
	ColorB buttonBgColor(0,0,0,150);
	ColorB buttonBgColorPressed(0,0,0,250);

	static bool bShowResInfo = false;
	static uint32 showIndex = 0;

	int tab_height = gEnv->pRenderer->GetScreenHeight() - 120;

	gEnv->pRenderer->getAuxRenderer()->AuxRenderScreenBox( Vec2(0, 0), Vec2(gEnv->pRenderer->GetScreenWidth(), gEnv->pRenderer->GetScreenHeight()), buttonBgColor );

	gEnv->pRenderer->getAuxRenderer()->AuxRenderText( _T("RESOURCE MANAGER"), 50, 50, m_titleFont );
	{
		// draw resource gui & info
		static uint32 tabIndex = 0;
		uint32 newIndex = gEnv->pInGUI->gkGUITab( _T("DYNTEX|TEXTURE|MESH|SHADER|MATERIAL"), Vec2(10, startY), gEnv->pRenderer->GetScreenWidth() - 20, tab_height, buttonBgColorHighlight, ColorB(0,0,0,225), tabIndex );
		if ( newIndex != -1 )
		{
			tabIndex = newIndex;
			showIndex = 0;
		}

		const gkResourceMap* map = gEnv->pSystem->getMaterialMngPtr()->getResourceMapPtr();
		switch( tabIndex )
		{
		case 0:
			map = gEnv->pSystem->getMaterialMngPtr()->getResourceMapPtr();
			break;
		case 1:
			map = gEnv->pSystem->getShaderMngPtr()->getResourceMapPtr();
			break;
		case 2:
			map = gEnv->pSystem->getMeshMngPtr()->getResourceMapPtr();
			break;
		case 3:
			map = gEnv->pSystem->getTextureMngPtr()->getResourceMapPtr();
			break;
		case 4:
			map = gEnv->pSystem->getTextureMngPtr()->getResourceMapPtr();
			break;
		}

		// list all resource
		gkResourceMap::const_iterator it = map->begin();

		typedef std::list<gkResourcePtr> Resources;

		Resources collections;
		// collect once
		for (; it != map->end(); ++it)
		{
			collections.push_back( it->second );
		}

		uint32 index = 0;
		int tableInfoY = startY + 40;
		int tableY = startY + 60;

		// filter
		{
			Resources::iterator it = collections.begin();
			for (; it != collections.end(); )
			{
				if ( (*it)->getType() == eRT_Texture ) 
				{
					ITexture* tex = static_cast<ITexture*>( (*it).getPointer() );
					if ((tabIndex == 3 && tex->dynamic()) || (tabIndex == 4 && !tex->dynamic()) || !tex->isLoaded())
					{
						it = collections.erase(it);
						continue;
					}
				}

				++it;
			}

			showIndex = showIndex >= collections.size() ? (collections.size() - 1) : showIndex;
		}

		int lineCount = (tab_height - 60) / 15;

		int tabX[6];
		tabX[0] = 15; tabX[1] = gEnv->pRenderer->GetScreenWidth() / 2 - 40; 
		tabX[2] = gEnv->pRenderer->GetScreenWidth() / 2 - 35; tabX[3] = gEnv->pRenderer->GetScreenWidth() / 2 + 35;
		tabX[4] = gEnv->pRenderer->GetScreenWidth() / 2 + 40; tabX[5] = gEnv->pRenderer->GetScreenWidth() - 15; 

		// draw 25 lines
		for (int i=0; i < lineCount; ++i)
		{
			gEnv->pRenderer->getAuxRenderer()->AuxRenderScreenBox( Vec2(tabX[0], tableY + i * 15), Vec2( tabX[1] - tabX[0], 15), i % 2 == 0 ? ColorB(90,90,90,128) : ColorB(60,60,60,128));
		}

		// draw table info
		TCHAR basicinfo[255];
		_stprintf(basicinfo, _T("%d / %d"), 1, collections.size() );
		gEnv->pRenderer->getAuxRenderer()->AuxRenderText(basicinfo , tabX[1] - 80, tableInfoY, m_profilerFont, ColorB(255,255,255,255) );
		_stprintf(basicinfo, _T(" id  | ref | usage | name") );
		gEnv->pRenderer->getAuxRenderer()->AuxRenderText(basicinfo , 25, tableInfoY, m_profilerFont, ColorB(255,255,255,255) );
		// list 25

		int beginIndex = 0;
		if (showIndex > lineCount - 1)
		{
			beginIndex = showIndex - lineCount + 1;
		}

		IResource* selectedRes = NULL;
		{
			Resources::iterator it = collections.begin();
			for (int i=0; it != collections.end(); ++it, ++i)
			{
				int tableRealY = (i - beginIndex);

				if ( tableRealY >= lineCount)
				{
					break;
				}

				bool selected = (showIndex == i);

				if(selected)
				{
					selectedRes = it->getPointer();
				}

				if (tableRealY >= 0)
				{
					TCHAR listStr[255];
					_stprintf(listStr, _T("#%.3d | %.3d | %.2fmb | %s"), i, it->useCount() - 3, ((float)(*it)->getMemUsage()) / 1024.f / 1024.f, (*it)->getName().c_str() );
					gEnv->pRenderer->getAuxRenderer()->AuxRenderText( listStr, tabX[0] + 10, tableY + tableRealY * 15, m_profilerFont, selected ? textColorHighlight : textColor );
				}


			}
		}

		if( gEnv->pInGUI->gkGUIButton( _T("UP"), Vec2(tabX[2], tableY), tabX[3] - tabX[2], 50, ColorB(255,255,255,255), ColorB(90,90,90,128) ) )
		{
			showIndex--;
		}
		if( gEnv->pInGUI->gkGUIButton( _T("DOWN"), Vec2(tabX[2], tableY + 51), tabX[3] - tabX[2], 50, ColorB(255,255,255,255), ColorB(90,90,90,128) ) )
		{
			showIndex++;
		}

		// selecte item
		if (selectedRes)
		{
			eResourceType type = selectedRes->getType();

			switch( type)
			{
			case eRT_Texture:
				{
					ITexture* tex = static_cast<ITexture*>(selectedRes);
					gEnv->pRenderer->getAuxRenderer()->AuxRenderText( tex->getShortName().c_str(), gEnv->pRenderer->GetScreenWidth() / 2 + 50, startY + 50, m_titleFont, ColorB(150,150,150,255) );
					TCHAR buffer[255];
					_stprintf(buffer, _T("%d x %d | %s"), tex->getWidth(), tex->getHeight(), tex->getAttr(_T("format")).c_str() );			
					gEnv->pRenderer->getAuxRenderer()->AuxRenderText( buffer, gEnv->pRenderer->GetScreenWidth() / 2 + 50, startY + 90, m_subtitleFont, ColorB(80,80,80,255) );

					float aspect = tex->getWidth() / (float)(tex->getHeight());
					int drawWidth = gEnv->pRenderer->GetScreenWidth() / 2 - 100;
					int drawHeight =  drawWidth / aspect;
                    
                    int heightLimit = gEnv->pRenderer->GetScreenHeight() - 260;

					if (drawHeight > heightLimit)
					{
						drawHeight = heightLimit;
						drawWidth = drawHeight * aspect;
					}

					gEnv->pRenderer->getAuxRenderer()->AuxRenderScreenBox( Vec2(gEnv->pRenderer->GetScreenWidth() / 2 + 50, startY + 130), Vec2(drawWidth, drawHeight), ColorB(255,255,255,255), tex );
				}
				break;

			case eRT_Mesh:
				{
					IMesh* mesh = static_cast<IMesh*>(selectedRes);
					gEnv->pRenderer->getAuxRenderer()->AuxRenderText( mesh->getShortName().c_str(), gEnv->pRenderer->GetScreenWidth() / 2 + 50, startY + 50, m_titleFont, ColorB(150,150,150,255) );

					TCHAR buffer[255];
					_stprintf(buffer, _T("Verts: %u, Tris: %u, Subset: %u"), mesh->getVertexBuffer()->elementCount, mesh->getIndexBuffer()->getSize(), mesh->getSubsetCount() );			
					gEnv->pRenderer->getAuxRenderer()->AuxRenderText( buffer, gEnv->pRenderer->GetScreenWidth() / 2 + 50, startY + 90, m_subtitleFont, ColorB(80,80,80,255) );
				}
				break;
			}

		}
	}
	
}

void gkSystemProfiler::qinfo( int boxheight )
{
	int leftstart = 5;

	uint32 width = gEnv->pRenderer->GetScreenWidth();
	uint32 height = gEnv->pRenderer->GetScreenHeight();

	ColorB textColor(255,255,255,255);
	ColorB buttonTextColor(180,180,180,255);
	ColorB buttonTextColorPressed(255,255,255,255);
	ColorB buttonBgColorHighlight(40,127,200,255);
	ColorB buttonBgColor(0,0,0,150);
	ColorB buttonBgColorPressed(0,0,0,250);
	//gEnv->pRenderer->getAuxRenderer()->AuxRenderScreenBox( Vec2(0, height - boxheight), Vec2(width, boxheight), buttonBgColor );

	TCHAR buffer[MAX_PATH];
	_stprintf_s(buffer, MAX_PATH, _T("%03.0f"), gEnv->pTimer->GetSmoothFrameRate());

	

	uint32 poxx = width - 5;
	uint32 poxy = 35;
	gEnv->pRenderer->getAuxRenderer()->AuxRenderText( buffer, 4, poxy - 8, m_profilerFPSFont, buttonBgColor );
	gEnv->pRenderer->getAuxRenderer()->AuxRenderText( buffer, 2, poxy - 10, m_profilerFPSFont, buttonBgColorHighlight );

	uint32 texth = 15;
	bool bMTR = true;

	ICamera* cam = gEnv->p3DEngine->getMainCamera();
	if (cam)
	{	
#ifdef _DEBUG
		const TCHAR* devVersion = _T("DEBUG");
#elif _RELEASE
		const TCHAR* devVersion = _T("RELEASE");
#else
		const TCHAR* devVersion = _T("PROFILE");
#endif

		ERendererAPI rendererType = gEnv->pRenderer->GetRendererAPI();
		TCHAR renderAPI[100];
		switch(rendererType)
		{
		case ERdAPI_D3D9:
			_tcscpy(renderAPI, _T("RendererD3D9"));
			break;
		case ERdAPI_OPENGLES:
			_tcscpy(renderAPI, _T("RendererGLES2"));
			break;
		case ERdAPI_OPENGL:
			_tcscpy(renderAPI, _T("RendererGL320"));
			break;
		default:
			_tcscpy(renderAPI, _T("RendererProto"));
			break;
		}

		_stprintf_s(buffer, MAX_PATH, _T("%s | %s | %d x %d | Cam: x=%0.1f y=%0.1f z=%0.1f | Zn:%0.2f | Zf:%0.2f | Fov:%0.1f"),
			renderAPI, devVersion, gEnv->pRenderer->GetScreenWidth(), gEnv->pRenderer->GetScreenHeight(), cam->getDerivedPosition().x, cam->getDerivedPosition().y, cam->getDerivedPosition().z, cam->getNearPlane(), cam->getFarPlane(), RAD2DEG(cam->getFOVy()));
	}
	else
	{
		_stprintf_s(buffer, MAX_PATH, _T("gkENGINE v1.0"));
	}

	gEnv->pRenderer->getAuxRenderer()->AuxRenderText( buffer, poxx, poxy, m_profilerFont, textColor, eFA_Right | eFA_Top, 1 );
	poxy += texth;

	_stprintf_s(buffer, MAX_PATH, _T("Fps: %6.2f(%6.2f) Time: %5.2f ms WAIT: %5.2f ms CMT: %5.2f ms SCEMNG: %5.2f ms"), 
		/*gEnv->pTimer->GetFrameRate() >= 100.0f ? _T("") : _T(" "),*/ gEnv->pTimer->GetFrameRate(),
		/*gEnv->pTimer->GetSmoothFrameRate() >= 100.0f ? _T("") : _T(" "),*/ gEnv->pTimer->GetSmoothFrameRate(), 
		gEnv->pTimer->GetFrameTime() * 1000,

		bMTR ? getFrameTimeRenderingWNT() * 1000 : 0, 
		getFrameTimeRenderingCMT() * 1000,
		getFrameTimeRenderingMNG() * 1000		
		);
	gEnv->pRenderer->getAuxRenderer()->AuxRenderText( buffer, poxx, poxy, m_profilerFont, textColor, eFA_Right | eFA_Top , 1 );
	poxy += texth;

	_stprintf_s(buffer, MAX_PATH, _T("Total DP(Shadow DP): %d(%d) | Total Tri(Shadow): %d(%d)"), getElementCount(ePe_Batch_Total), 
		getElementCount(ePe_Batch_Shadow), 
		getElementCount(ePe_Triangle_Total), 
		getElementCount(ePe_Triangle_Shadow));
	gEnv->pRenderer->getAuxRenderer()->AuxRenderText( buffer, poxx, poxy, m_profilerFont, textColor, eFA_Right | eFA_Top , 1 );
	poxy += texth;

	gkCVar* hdr = gEnv->pCVManager->getCVar( _T("r_HDRRendering") );
	gkCVar* msaa = gEnv->pCVManager->getCVar( _T("r_PostMsaa") );
	gkCVar* shadow = gEnv->pCVManager->getCVar( _T("r_Shadow") );
	// 
	_stprintf_s(buffer, MAX_PATH, _T("%s | HDR %s | Shadows %s | sRGB | MSAA %s"), 
		gEnv->pSystemInfo->gpuDesc,
		(hdr && hdr->getInt()) ? _T("on") : _T("off"), _T("on"), 
		(msaa && msaa->getInt()) ? _T("on") : _T("off"));
	gEnv->pRenderer->getAuxRenderer()->AuxRenderText( buffer, poxx, poxy, m_profilerFont, textColor, eFA_Right | eFA_Top , 1 );
	poxy += texth;

	// task
	sTaskStatus status = gEnv->pCommonTaskDispatcher->QueryTaskStatus();
	_stprintf_s(buffer, MAX_PATH, _T("Task(Total|T0|T01|T2|T3): %d | %d | %d | %d | %d"), 
		status.taskTotal, status.taskNum[0], status.taskNum[1], status.taskNum[2], status.taskNum[3]
	);
	gEnv->pRenderer->getAuxRenderer()->AuxRenderText( buffer, poxx, poxy, m_profilerFont, textColor, eFA_Right | eFA_Top , 1 );
	poxy += texth;
}

#if defined( OS_IOS ) && !defined( OS_APPLE )
    #import <UIKit/UIKit.h>
#endif

#if defined( OS_APPLE )
    #import <AppKit/AppKit.h>
#endif

void gkSystemProfiler::about()
{
	TCHAR buffer[250];
	TCHAR bufferT[250];
	ColorB textColor(255,255,255,255);
	ColorB textColorHighlight(40,127,200,255);
	ColorB buttonTextColor(180,180,180,255);
	ColorB buttonBgColor(0,0,0,150);
    ColorB textColorHighlight1(0,0,0,255);
	ColorB buttonBgColor1(255,255,255,180);

	int buttonWidth = 200;
	int buttonHeight = 120;

	int buttonLeftStart = 50;
	int buttonLeft = buttonLeftStart;
	int buttonTop = 100;

	gEnv->pRenderer->getAuxRenderer()->AuxRenderScreenBox( Vec2(0, 0), Vec2(gEnv->pRenderer->GetScreenWidth(), gEnv->pRenderer->GetScreenHeight()), buttonBgColor );

	gkTexturePtr tex = gEnv->pSystem->getTextureMngPtr()->loadSync( _T("Engine/Assets/Textures/default/logobanner.tga"), _T("internal") );
	int width = gEnv->pRenderer->GetScreenWidth() / 2;
	int height = width / 4;
	gEnv->pRenderer->getAuxRenderer()->AuxRenderScreenBox( Vec2(buttonLeftStart,100), Vec2(512, 128), buttonBgColor1, tex.getPointer() );

	gEnv->pRenderer->getAuxRenderer()->AuxRenderText( _T("ABOUT"), buttonLeftStart, 50, m_titleFont );



	gEnv->pRenderer->getAuxRenderer()->AuxRenderText( _T("gkENGINE是一个开源跨平台游戏引擎，致力于提供尖端的实时渲染技术和惊人的效率。\n")
		_T("gkENGINE is a cross-platform game engine with cutting-edge real-time rendering tech & fantastic speed.\n")
        _T("\n")
		_T("联系作者: kaimingyi@163.com\n")
        _T("contact me: kaimingyi@163.com\n")
        _T("\n")
		_T("Github项目主页: | Github Website: \n"),
		
		buttonLeftStart, 300, m_subtitleFont );

	if ( gEnv->pInGUI->gkGUIButton( _T("https://gkengine.codeplex.com >>"), Vec2(buttonLeftStart, 450), 350, 80, textColorHighlight1, buttonBgColor1, m_subtitleFont ) )
	{
#ifdef OS_WIN32
		system("start https://github.com/gameKnife/gkEngine");
#endif

#ifdef OS_APPLE
        [[NSWorkspace sharedWorkspace] openURL: [NSURL URLWithString:@"https://github.com/gameKnife/gkEngine"]];
#elif defined( OS_IOS )
        [[UIApplication sharedApplication] openURL:[NSURL URLWithString:@"https://github.com/gameKnife/gkEngine"]];
#else
        
#endif
	}
}

void gkSystemProfiler::features()
{
	TCHAR buffer[250];
	TCHAR bufferT[250];
	ColorB textColor(255,255,255,255);
	ColorB buttonBgColor(0,0,0,150);

	int buttonWidth = 200;
	int buttonHeight = 120;

	int buttonLeftStart = 50;
	int buttonLeft = buttonLeftStart;
	int buttonTop = 100;

	gEnv->pRenderer->getAuxRenderer()->AuxRenderScreenBox( Vec2(0, 0), Vec2(gEnv->pRenderer->GetScreenWidth(), gEnv->pRenderer->GetScreenHeight()), buttonBgColor );

	gEnv->pRenderer->getAuxRenderer()->AuxRenderText( _T("RENDERING FEATURES"), buttonLeftStart, 50, m_titleFont );

	for (uint32 i=0; i < 6; ++i)
	{
		buttonBgColor = ColorB(0,0,0,150);

		switch(i)
		{
		case 0:
			{
				MAKE_SWITCH(r_hdrrendering);

// 				gkCVar* var = gEnv->pCVManager->getCVar(_T("r_hdrrendering")); \
// 					if (var && var->getInt() == 0)\
// 					{\
// 						buttonBgColor = ColorB(0, 0, 0, 150); \
// 						_stprintf(buffer, (_T("r_hdrrendering")_T(" 1"))); \
// 					}\
// 					else\
// 					{\
// 						buttonBgColor = ColorB(40, 127, 200, 150); \
// 						_stprintf(buffer, (_T("r_hdrrendering")_T(" 0"))); \
// 					}

				_stprintf(bufferT, _T("HDR PROCESS"), i);
			}

			break;
		case 1:
			{
				MAKE_SWITCH( r_postmsaa );
				_stprintf(bufferT, _T("POST MSAA"), i);
			}

			break;
		case 2:
			{
				MAKE_SWITCH( r_dof );
				_stprintf(bufferT, _T("DEPTH OF FIELD"), i);
			}

			break;
		case 3:
			{
				MAKE_SWITCH( r_shadow );
				_stprintf(bufferT, _T("SHADOW"), i);
			}

			break;
		case 4:
			{
				MAKE_SWITCH( r_SSAO );
				_stprintf(bufferT, _T("SSAO"), i);
			}
            break;
        case 5:
            {
                MAKE_SWITCH( r_Fog );
				_stprintf(bufferT, _T("FOG"), i);
            }
			break;
		}

		///draw button
		if( gEnv->pInGUI->gkGUIButton( bufferT, Vec2(buttonLeft, buttonTop), buttonWidth, buttonHeight, textColor, buttonBgColor, m_subtitleFont ) )
		{
			gEnv->pCVManager->executeCommand(buffer);
		}

		buttonLeft += buttonWidth + 2;
		if (buttonLeft >= gEnv->pRenderer->GetScreenWidth() - buttonLeftStart)
		{
			buttonLeft = buttonLeftStart;
			buttonTop += buttonHeight + 2;
		}
	}

}

#define POST_GPU_ITME( X ) 	{\
TCHAR text[MAX_PATH];\
	_stprintf( text, _T(#X) _T("： %.2fms"), getGpuTimerElement( _T(#X) ) );\
gEnv->pRenderer->getAuxRenderer()->AuxRenderText( text, start_pos_x, start_pos_y+=20, m_profilerFont );\
	total_sum+=getGpuTimerElement( _T(#X) );\
}

#define POST_ELEMENT_ITME( X ) 	{\
	TCHAR text[MAX_PATH];\
	_stprintf( text, _T(#X) _T("： %.2fms"), getElementTime( X ) * 1000.0f );\
	gEnv->pRenderer->getAuxRenderer()->AuxRenderText( text, start_pos_x, start_pos_y+=20, m_profilerFont );\
}

#define DISP_GPU_TIME( X ) 	{\
	TCHAR text[MAX_PATH];\
	_stprintf( text, _T("%.2fms"), getGpuTimerElement( _T(#X) ) );\
float length = getGpuTimerElement( _T(#X) ) / total_sum;\
ColorB thisColor;\
thisColor.fromHSV( length, 0.5f, 0.7f );\
thisColor.a = 180;\
int bar_len = whole_len * length;\
gEnv->pRenderer->getAuxRenderer()->AuxRenderScreenBox( Vec2( start_bar_x, bar_y), Vec2( bar_len, 85 ), thisColor );\
if(bar_len>1)\
{\
	gEnv->pRenderer->getAuxRenderer()->AuxRenderText( _T(#X), start_bar_x+5, disp_count % 2 == 0 ? bar_y + 10 : bar_y + 55, m_profilerFont  );\
	gEnv->pRenderer->getAuxRenderer()->AuxRenderText( text, start_bar_x+5, disp_count % 2 == 0 ? bar_y + 25 : bar_y + 70, m_profilerFont  );\
}\
start_bar_x += (bar_len);\
	disp_count++;\
}

void gkSystemProfiler::benchmark()
{
	gEnv->pRenderer->getAuxRenderer()->AuxRenderText( _T("INTEGRATE PROFILER"), 50, 50, m_titleFont );

	int whole_screen_box_y = 0;
	int whole_screen_height = 350;
	whole_screen_box_y = gEnv->pRenderer->GetScreenHeight() - whole_screen_height - 10;

	gEnv->pRenderer->getAuxRenderer()->AuxRenderScreenBox( Vec2( 10, whole_screen_box_y), Vec2( gEnv->pRenderer->GetScreenWidth() - 20, whole_screen_height), ColorB(0,0,0,100) );

	ColorB textColor(255,255,255,255);
	ColorB buttonBgColor(0,0,0,150);

	// + / - pixel density
	int button_width = 50;
	if( gEnv->pInGUI->gkGUIButton( _T("-"), Vec2(0, gEnv->pRenderer->GetScreenHeight() - button_width), button_width, button_width, textColor, buttonBgColor, m_subtitleFont ) )
	{
		float ratio = gEnv->pRenderer->GetPixelReSize();
		gEnv->pRenderer->SetPixelReSize( ratio * 0.5f );
	}

	if( gEnv->pInGUI->gkGUIButton( _T("+"), Vec2(button_width + 5, gEnv->pRenderer->GetScreenHeight() - button_width), button_width, button_width, textColor, buttonBgColor, m_subtitleFont ) )
	{
		float ratio = gEnv->pRenderer->GetPixelReSize();
		gEnv->pRenderer->SetPixelReSize( ratio * 2.0f );
	}

	// DRAW TIME BAR
	int start_pos_y = whole_screen_box_y - 230;
	int start_pos_x = 50;
	float total_sum = 0;

	//POST_GPU_ITME( GPUTime )
	POST_GPU_ITME( ReflectGen )
	POST_GPU_ITME( ShadowMapGen )
	POST_GPU_ITME( Zpass )
	POST_GPU_ITME( SSAO )
	POST_GPU_ITME( ShadowMaskGen )
	POST_GPU_ITME( Deferred Lighting )
	POST_GPU_ITME( Opaque )
	POST_GPU_ITME( Transparent )
	POST_GPU_ITME( HDR )
	POST_GPU_ITME( PostProcess )

	start_pos_x = 200;
	start_pos_y = whole_screen_box_y - 230;

	POST_ELEMENT_ITME( ePe_3DEngine_Cost )
	POST_ELEMENT_ITME( ePe_Font_Cost )
	POST_ELEMENT_ITME( ePe_Physic_Cost )
	POST_ELEMENT_ITME( ePe_TrackBus_Cost )
	POST_ELEMENT_ITME( ePe_ThreadSync_Cost )

	POST_ELEMENT_ITME( ePe_SThread_Cost )
	POST_ELEMENT_ITME( ePe_MThread_Cost )
	
	POST_ELEMENT_ITME( ePe_MT_Part1 )
	POST_ELEMENT_ITME( ePe_MT_Part2 )
	POST_ELEMENT_ITME( ePe_MT_Part3 )

	//whole_screen_box_y

	int whole_len = gEnv->pRenderer->GetScreenWidth() - 100;
	int start_bar_x = 50;
	int disp_count = 0;
	int bar_y = whole_screen_box_y + 60;

	gEnv->pRenderer->getAuxRenderer()->AuxRenderText( _T("GPU TIME MEASURE"), 50, whole_screen_box_y + 20, m_subtitleFont );


	gkCVar* pgpu = gEnv->pCVManager->getCVar(_T("r_profilegpu"));
	if ( pgpu && pgpu->getInt() != 0 )
	{
			DISP_GPU_TIME( ReflectGen )
			DISP_GPU_TIME( ShadowMapGen )
			DISP_GPU_TIME( Zpass )
			DISP_GPU_TIME( SSAO )
			DISP_GPU_TIME( ShadowMaskGen )
			DISP_GPU_TIME( Deferred Lighting )
			DISP_GPU_TIME( Opaque )
			DISP_GPU_TIME( Transparent )
			DISP_GPU_TIME( HDR )
			DISP_GPU_TIME( PostProcess )
	}
	else
	{
		gEnv->pRenderer->getAuxRenderer()->AuxRenderText( _T("ProfileGpu is not enable, please set r_profilegpu = 1."), 50, whole_screen_box_y + 50, m_profilerFont );
	}

	

	gEnv->pRenderer->getAuxRenderer()->AuxRenderText( _T("FRAME TIME CURVE"), 50, whole_screen_box_y + 160, m_subtitleFont );

	// DRAW FRAME TIME CURVE
	gEnv->pRenderer->getAuxRenderer()->AuxRenderText( _T("0ms"), 10, whole_screen_box_y + 285, m_profilerFont  );
	gEnv->pRenderer->getAuxRenderer()->AuxRenderScreenBox( Vec2( 10, whole_screen_box_y + 300 ), Vec2( gEnv->pRenderer->GetScreenWidth() - 20, 1), ColorB(255,255,255,100) );
	gEnv->pRenderer->getAuxRenderer()->AuxRenderText( _T("25ms"), 10, whole_screen_box_y + 235, m_profilerFont  );
	gEnv->pRenderer->getAuxRenderer()->AuxRenderScreenBox( Vec2( 10, whole_screen_box_y + 250 ), Vec2( gEnv->pRenderer->GetScreenWidth() - 20, 1), ColorB(255,255,255,100) );
	gEnv->pRenderer->getAuxRenderer()->AuxRenderText( _T("50ms"), 10, whole_screen_box_y + 185, m_profilerFont  );
	gEnv->pRenderer->getAuxRenderer()->AuxRenderScreenBox( Vec2( 10, whole_screen_box_y + 200 ), Vec2( gEnv->pRenderer->GetScreenWidth() - 20, 1), ColorB(255,255,255,100) );

	{
		float* datasrc = m_fFrameTimeRenderingCurve;
		ColorB lineColor(255,200,200,255);
		DrawProfileCurve(whole_screen_box_y, datasrc, lineColor);
	}

	{
		float* datasrc = m_fFramwTimeCurve + 1;
		ColorB lineColor(200,200,255,255);
		DrawProfileCurve(whole_screen_box_y, datasrc, lineColor);
	}

}

float gkSystemProfiler::getElementTime(EProfileElement element)
{
	return m_profileElements[element].m_elpasedTime;
}

void gkSystemProfiler::setElementTime(EProfileElement element, float elapsedTime)
{
	m_profileElements[element].m_elpasedTime = elapsedTime;
}

void gkSystemProfiler::setGpuTimerElement(const TCHAR* elementName, float elpasedTime)
{
	std::map<gkStdString, float>::iterator it = m_gpuTimerElements.find( elementName );
	if ( it != m_gpuTimerElements.end() )
	{
		it->second = elpasedTime;
	}
	else
	{
		m_gpuTimerElements.insert( std::map<gkStdString, float>::value_type( elementName, elpasedTime ) );
	}
}

float gkSystemProfiler::getGpuTimerElement(const TCHAR* elementName)
{
	float ret = 0;
	
	std::map<gkStdString, float>::iterator it = m_gpuTimerElements.find( elementName );
	if ( it != m_gpuTimerElements.end() )
	{
		ret = it->second;
	}

	return ret;
}

void gkSystemProfiler::DrawProfileCurve(int whole_screen_box_y, float* datasrc, ColorB lineColor)
{
	float step = (gEnv->pRenderer->GetScreenWidth() - 100) / 200.f;
	for (int i=0; i<200; ++i)
	{
		float height_now = datasrc[((m_CurveRecorder - i - 1) + 1000) % 1000];
		float height_next = datasrc[((m_CurveRecorder - i - 2) + 1000) % 1000];
		float ratio_now = ( height_now * 1000.f) / 50.0f;
		float ratio_next = ( height_next * 1000.f) / 50.0f;

		ratio_now = clamp(ratio_now, 0.0f, 1.0f);
		ratio_next = clamp(ratio_next, 0.0f, 1.0f);

		gEnv->pRenderer->getAuxRenderer()->AuxRenderScreenLine( Vec2(50 + i * step, whole_screen_box_y + 300 - ratio_now * 100), Vec2(50 + (i + 1) * step, whole_screen_box_y + 300 - ratio_next * 100), lineColor );
	}
}

void gkSystemProfiler::setFrameBegin()
{

}

void gkSystemProfiler::setFrameEnd()
{
	m_fFrameTimeRenderingCurve[m_CurveRecorder] = m_fFrameTimeRendering;
	m_fFramwTimeCurve[m_CurveRecorder] = gEnv->pTimer->GetRealFrameTime();

	m_CurveRecorder++;
	m_CurveRecorder %= 1000;
}

void gkSystemProfiler::setElementStart(EProfileElement element)
{
	m_profileElements[element].m_startTime = gEnv->pTimer->GetAsyncCurTime();
}

void gkSystemProfiler::setElementEnd(EProfileElement element)
{
	m_profileElements[element].m_elpasedTime = gEnv->pTimer->GetAsyncCurTime() - m_profileElements[element].m_startTime;
}



