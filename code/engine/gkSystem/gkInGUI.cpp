#include "gkSystemStableHeader.h"
#include "gkInGUI.h"
#include "IRenderer.h"
#include "IAuxRenderer.h"
#include "IFont.h"


gkInGUI::gkInGUI(void)
{
	m_defaultFont = NULL;
}


gkInGUI::~gkInGUI(void)
{
}


uint32 gkInGUI::gkGUITab( const TCHAR* title, const Vec2& pos, int width, int height, const ColorB& textColor, const ColorB& bgColor,int selection,  IFtFont* font /*= NULL*/ )
{
	int tabHeight = 30;
	int tabWidth = 99;

	gEnv->pRenderer->getAuxRenderer()->AuxRenderScreenBox(pos + Vec2(0, tabHeight), Vec2(width,1), textColor);
	gEnv->pRenderer->getAuxRenderer()->AuxRenderScreenBox(pos + Vec2(0, tabHeight + 1), Vec2(width,height - tabHeight - 1), bgColor);

	if (!font)
	{
		font = m_defaultFont;
	}

	// split the tab title
	TCHAR buffer[MAX_PATH];
	_tcscpy( buffer, title );
	TCHAR* last = _tcsrchr( buffer, _T('|') );
	
	uint32 index = 0;
	uint32 indexInside = -1;
	ColorB unselColor = ColorB(textColor.r, textColor.g, textColor.b, textColor.a / 2);
	while( last )
	{
		Vec2 thisPos = pos + Vec2(index * (tabWidth + 1), 0);

		if (selection == index)
		{
			gEnv->pRenderer->getAuxRenderer()->AuxRenderScreenBox(thisPos, Vec2(tabWidth,tabHeight), textColor);
		}
		else
		{
			gEnv->pRenderer->getAuxRenderer()->AuxRenderScreenBox(thisPos, Vec2(tabWidth,tabHeight), unselColor);
		}
		
		gEnv->pRenderer->getAuxRenderer()->AuxRenderText(last+1, thisPos.x + 10, thisPos.y + 5, font, ColorB(255,255,255,255), eFA_Left | eFA_Top);
		*last = 0;
		last = _tcsrchr( buffer, _T('|') );

		if ( m_lastPoint.x < thisPos.x || m_lastPoint.x > thisPos.x + tabWidth || m_lastPoint.y < thisPos.y || m_lastPoint.y > thisPos.y + 20 )
		{
			
		}
		else
		{
			indexInside = index;
		}

		index++;
	}

	Vec2 thisPos = pos + Vec2(index * (tabWidth + 1), 0);
	if (selection == index)
	{
		gEnv->pRenderer->getAuxRenderer()->AuxRenderScreenBox(thisPos, Vec2(tabWidth,tabHeight), textColor);
	}
	else
	{
		gEnv->pRenderer->getAuxRenderer()->AuxRenderScreenBox(thisPos, Vec2(tabWidth,tabHeight), unselColor);
	}	
	gEnv->pRenderer->getAuxRenderer()->AuxRenderText(buffer, thisPos.x + 10, thisPos.y + 5, font, ColorB(255,255,255,255), eFA_Left | eFA_Top);
	if ( m_lastPoint.x < thisPos.x || m_lastPoint.x > thisPos.x + tabWidth || m_lastPoint.y < thisPos.y || m_lastPoint.y > thisPos.y + 20 )
	{

	}
	else
	{
		indexInside = index;
	}

	if (m_LMBState == Pressing)
	{
		return indexInside;
	}
	return -1;
}

bool gkInGUI::gkGUIButton( const TCHAR* title, const Vec2& pos, int width, int height, const ColorB& textColor, const ColorB& bgColor, IFtFont* font )
{
	bool ret = false;
	bool inside = ( m_lastPoint.x < pos.x || m_lastPoint.x > pos.x + width || m_lastPoint.y < pos.y || m_lastPoint.y > pos.y + height );
	inside = !inside;

	ColorB realBgColor = bgColor;

	if (m_LMBState == Pressing)
	{
		if ( inside )
		{
			ret = true;
		}
	}

	if ( inside )
	{
		realBgColor.a = 255;
	}

	// Draw
	if (!font)
	{
		font = m_defaultFont;
	}

	gEnv->pRenderer->getAuxRenderer()->AuxRenderScreenBox(pos, Vec2(width,height), realBgColor);
	gEnv->pRenderer->getAuxRenderer()->AuxRenderText(title, pos.x + width / 2, pos.y +  height / 2, font, textColor, eFA_HCenter | eFA_VCenter);

	return ret;
}

bool gkInGUI::OnInputEvent( const SInputEvent &event )
{
	if ( event.deviceId == eDI_Mouse)
	{
		switch( event.keyId )
		{
		case eKI_Mouse1:
			{
				if (event.state == eIS_Pressed)
				{
					m_LMBState = Pressing;

					//m_lastPoint.x = event.value;
					//m_lastPoint.y = event.value2;

					//gkLogMessage(_T("ingui pressed %.1f|%.1f"), m_lastPoint.x, m_lastPoint.y);
				}
// 				else if (event.state == eIS_Down)
// 				{
// 					m_LMBState = Hold;
// 					m_lastPoint.x = event.value;
// 					m_lastPoint.y = event.value2;
// 				}
				else if (event.state == eIS_Released)
				{
					m_LMBState = Released;
					//m_lastPoint.x = event.value;
					//m_lastPoint.y = event.value2;

					//gkLogMessage(_T("ingui released %.1f|%.1f"), m_lastPoint.x, m_lastPoint.y);
				}
			}
			break;
		case eKI_MouseX:
			{
				if (event.state == eIS_Changed)
				{
					m_lastPoint.x = event.value2;
				}
			}
			break;
		case eKI_MouseY:
			{
				if (event.state == eIS_Changed)
				{
					m_lastPoint.y = event.value2;
				}
			}
			break;
		}

	}

	if ( event.deviceId == eDI_Android || event.deviceId == eDI_IOS )
	{
		switch( event.keyId )
		{
		case eKI_Android_Touch:
			{
				if (event.state == eIS_Pressed)
				{
					m_LMBState = Pressing;
					m_lastPoint.x = event.value;
					m_lastPoint.y = event.value2;
				}
				else if (event.state == eIS_Released)
				{
					m_LMBState = Released;
					m_lastPoint.x = event.value;
					m_lastPoint.y = event.value2;
				}
			}
			break;
// 		case eKI_Android_DragX:
// 		case eKI_Android_DragY:
// 			m_LMBState = Hold;
// 			break;
		}
	}
	return false;
}

void gkInGUI::Init()
{
	m_LMBState = Released;
	m_lastPoint.zero();
	gEnv->pInputManager->addInputEventListener(this);
	m_defaultFont = gEnv->pFont->CreateFont(_T("engine/fonts/msyh.ttf"), 18, GKFONT_OUTLINE_0PX, _T("engine/fonts/segoeuil.ttf"));
}

void gkInGUI::Shutdown()
{
	gEnv->pInputManager->removeEventListener(this);
}

void gkInGUI::gkPopupMsg( const TCHAR* title )
{
	m_popupMsgs.push_back( gkPopupMsgStruct(title, 2.0f) );
}

void gkInGUI::Update( float fElapsedTime )
{
	// minus time
	std::vector<gkPopupMsgStruct>::iterator it = m_popupMsgs.begin();
	for (; it != m_popupMsgs.end(); ++it)
	{
		it->m_time -= fElapsedTime;
	}

	// remove if times up
	for (it = m_popupMsgs.begin(); it !=m_popupMsgs.end(); ++it)
	{
		if (it->m_time < 0)
		{
			m_popupMsgs.erase( it );
			break;
		}
	}

	// reserve back to display
	if (!m_popupMsgs.empty())
	{
		int startx = gEnv->pRenderer->GetScreenWidth() / 2 - 200;
		int starty = gEnv->pRenderer->GetScreenHeight() / 3 - 20;
		int startyfirst = starty;
		int ydist = 45;

		float fadeStart = m_popupMsgs[m_popupMsgs.size() - 1].m_totalTime - m_popupMsgs[m_popupMsgs.size() - 1].m_time;
		fadeStart = clamp(fadeStart * 2.0f, 0.f, 1.f);

		starty -= (fadeStart) * 45;

		std::vector<gkPopupMsgStruct>::reverse_iterator itr = m_popupMsgs.rbegin();
		int count = 0;
		for (; itr != m_popupMsgs.rend(); ++itr)
		{
			if (itr == m_popupMsgs.rbegin())
			{
				float fade = clamp(1.f - itr->m_time, 0.f, 1.f);
				fade = 1 - fade;
				gkGUIButton( itr->m_msg.c_str(), Vec2(startx, startyfirst), 400, 40, ColorB(255,255,255,255 * fade), ColorB(0,0,0,128 * fade)  );
			}
			else
			{
				float fade = clamp(1.f - itr->m_time, 0.f, 1.f);
				fade = 1 - fade;
				gkGUIButton( itr->m_msg.c_str(), Vec2(startx, starty - ydist * count++), 400, 40, ColorB(255,255,255,128 * fade), ColorB(0,0,0,128 * fade)  );
			}

		}
	}

	if (m_LMBState == Pressing)
	{
		m_LMBState = Hold;
	}


}
