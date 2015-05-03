#include "stdafx.h"
#include "gkObjectManager.h"
#include "IAuxRenderer.h"
#include "WorkspaceView.h"
#include "I3DEngine.h"
#include "IInputManager.h"
#include "IGameObjectLayer.h"
#include "Panels\gkSwissKnifeBar.h"
#include "ISystem.h"
#include "IResourceManager.h"



IGameObject* gkObjectManager::ms_pCurrentPick = NULL;
IGameObject* gkObjectManager::ms_pCurrentHover = NULL;
uint8 gkObjectManager::ms_selectedAxis = NULL;
int8 gkObjectManager::ms_dragInvertX = 1;
int8 gkObjectManager::ms_dragInvertY = 1;
uint8 gkObjectManager::ms_objselmode = GKSTUDIO_OBJMODE_SEL;
//-----------------------------------------------------------------------
gkObjectManager::gkObjectManager()
{
	gEnv->pInputManager->addInputEventListener(this);
	m_isDarging = false;


	m_bIsTargetDataReady = false;
	m_vTargetPos = Vec3(0,0,0);
	m_vStartPos = Vec3(0,0,0);
	m_qTargetRot = Quat::CreateIdentity();
	m_dragOffset.zero();

	m_cloneMode = false;
}
//-----------------------------------------------------------------------
gkObjectManager::~gkObjectManager()
{

}
//-----------------------------------------------------------------------
void gkObjectManager::setCurrSelected( IGameObject* pEntity )
{
	if (ms_pCurrentPick)
	{
		ms_pCurrentPick->showBBox(false);
	}

// 	if (ms_pCurrentPick)
// 	{
// 		ms_pCurrentPick->setMaskColor(0.0f);
// 	}
	ms_pCurrentPick = pEntity;
//	ms_pCurrentPick->setMaskColor(0.6f);

	if (pEntity)
	{
		GetIEditor()->getSwissKnifeBar()->RefreshObj( pEntity );
	}
	
}
//-----------------------------------------------------------------------
void gkObjectManager::Update()
{
	if (ms_pCurrentPick)
	{	
		if (!m_isDarging)
		{
			// update hover states
			UpdateGizmoHover(ms_objselmode);
		}
		else
		{
			// update move/rot/scale draging
			UpdateGizmoDraging(ms_objselmode);
		}

		// draw gizmo and Frame

		// gizmo length
		float gizmo_len = ms_pCurrentPick->getAABB().GetRadius() > 5.0 ? 5.0 : ms_pCurrentPick->getAABB().GetRadius();

		gEnv->pRenderer->getAuxRenderer()->AuxRender3DGird( ms_pCurrentPick->getWorldPosition(), 21, 0.1f, ColorF(0.f,0.f,0.f,0.1f), false );
		gEnv->pRenderer->getAuxRenderer()->AuxRenderGizmo( ms_pCurrentPick->GetWorldMatrix(), gizmo_len, ms_selectedAxis, true, ms_objselmode );
		gEnv->pRenderer->getAuxRenderer()->AuxRenderMeshFrame( ms_pCurrentPick->getRenderLayer(), ColorF(0,1,0,1) );
		//gEnv->pRenderer->getAuxRenderer()->AuxRenderAABB( ms_pCurrentPick->getAABB(), ColorF(1,1,1,0.5) );

		Vec2 ptx, pty, ptz, getPt;
		if( GetIEditor()->getMainViewport()->getScreenPosFromScene(ms_pCurrentPick->getWorldPosition(), getPt) )
		{
			// gizmo text
			Vec3 posx = ms_pCurrentPick->getWorldPosition() + ms_pCurrentPick->getWorldOrientation().GetColumn0() * (gizmo_len + 2.0f);
			Vec3 posy = ms_pCurrentPick->getWorldPosition() + ms_pCurrentPick->getWorldOrientation().GetColumn1() * (gizmo_len + 2.0f);
			Vec3 posz = ms_pCurrentPick->getWorldPosition() + ms_pCurrentPick->getWorldOrientation().GetColumn2() * (gizmo_len + 2.0f);

			GetIEditor()->getMainViewport()->getScreenPosFromScene(posx, ptx);
			GetIEditor()->getMainViewport()->getScreenPosFromScene(posy, pty);
			GetIEditor()->getMainViewport()->getScreenPosFromScene(posz, pty);

			gEnv->pRenderer->getAuxRenderer()->AuxRenderText( _T("x"), ptx.x, ptx.y, GetIEditor()->getDefaultFont(), ColorF(1,1,1,1), 9u, true);
			gEnv->pRenderer->getAuxRenderer()->AuxRenderText( _T("y"), pty.x, pty.y, GetIEditor()->getDefaultFont(), ColorF(1,1,1,1), 9u, true);
			gEnv->pRenderer->getAuxRenderer()->AuxRenderText( _T("z"), ptz.x, ptz.y, GetIEditor()->getDefaultFont(), ColorF(1,1,1,1), 9u, true);
			gEnv->pRenderer->getAuxRenderer()->AuxRenderText( ms_pCurrentPick->getName().c_str(), getPt.x, getPt.y, GetIEditor()->getDefaultFont(), ColorF(1,1,1,1), eFA_HCenter | eFA_HCenter , true);
		}
	}

	// 绘制特殊gameobject的图标 [12/31/2014 gameKnife]
	{
		const IGameObjectQueue& list = gEnv->pGameObjSystem->GetGameObjects();

		for (IGameObjectQueue::const_iterator it = list.begin(); it != list.end(); ++it)
		{
			switch( (*it)->getGameObjectSuperClass() )
			{
			case eGOClass_LIGHT:
				{
					//m_icon_lightstatic gkTexturePtr light_icon;
					m_icon_light = gEnv->pSystem->getTextureMngPtr()->load( _T("/engine/assets/textures/editor/icon_light.tga") );
					if (!m_icon_light.isNull())
					{
						Vec3 pos = (*it)->getRenderLayer()->getWorldPosition();
						Vec2 curcor_pos = GetIEditor()->getMainViewport()->getCursorOnClientScreen();

						Vec2 ptx;
						if( GetIEditor()->getMainViewport()->getScreenPosFromScene(pos, ptx) )
						{
							ColorB color_icon = ColorB(255,255,255,180);
							if ((curcor_pos - ptx).GetLength() < 16 )
							{
								color_icon = ColorB(0,169,255,180);
							}

							if( ms_pCurrentPick == *it )
							{
								color_icon = ColorB(0,169,255,255);
							}

							ptx.x -= 16;
							ptx.y -= 16;

							gEnv->pRenderer->getAuxRenderer()->AuxRenderScreenBox( ptx, Vec2(32, 32), color_icon, m_icon_light.getPointer() );
						}

					}
				}
				break;
			default:

				break;
			}
		}


	}
}
//-----------------------------------------------------------------------
bool gkObjectManager::checkSelected( uint8 type, f32 size, bool draging )
{
	// first of all, get the 3 axis end point at screenspace [8/25/2011 Kaiming-Desktop]
	Vec2 vCursor = GetIEditor()->getMainViewport()->getCursorOnClientScreen();
	Vec3 vAxis3D;
	Vec3 vCenter3D;

	Vec3 vCenterReal = ms_pCurrentPick->getWorldPosition();
	Vec3 vDirReal(0,0,0);

	vCenter3D = gEnv->pRenderer->ProjectScreenPos( vCenterReal );

	switch(type)
	{
	case GKSTUDIO_AXIS_X:
		vDirReal = ms_pCurrentPick->getOrientation().GetColumn0();
		break;
	case GKSTUDIO_AXIS_Y:
		vDirReal = ms_pCurrentPick->getOrientation().GetColumn1();
		break;
	case GKSTUDIO_AXIS_Z:
		vDirReal = ms_pCurrentPick->getOrientation().GetColumn2();
		break;
	}

	vAxis3D = gEnv->pRenderer->ProjectScreenPos( ms_pCurrentPick->getWorldPosition() + size * vDirReal );

	// make two 2D vector
	Vec2 vCenter(vCenter3D.x, vCenter3D.y);
	Vec2 vAxis(vAxis3D.x, vAxis3D.y);

	Vec2 vPoint = vCursor - vCenter;
	Vec2 vAxisPoint = vAxis - vCenter;

	ms_dragInvertX = vAxisPoint.x > 0 ? 1 : -1;
	ms_dragInvertY = vAxisPoint.y > 0 ? 1 : -1;

	// judge this
	if (vPoint.GetLength() - vAxisPoint.GetLength() < size + 2.0f)
	{
		vPoint.Normalize();
		vAxisPoint.Normalize();

		if (vPoint.Dot(vAxisPoint) > 0.95f)
			return true;
	}

	return false;
}
//-----------------------------------------------------------------------
bool gkObjectManager::UpdateGizmoHover(uint8 selmode)
{
//	// add a pre highlight [10/14/2011 Kaiming]
// 	Ray ray = GetIEditor()->getMainViewport()->getRayFronScreen();
// 	IgkEntity* pPick = gEnv->p3DEngine->getRayHitEntity(ray);
// 	if (pPick &&  pPick != ms_pCurrentPick)
// 		GetIEditor()->getObjectMng()->setCurrHover(pPick);
	// disable now [2/12/2012 Kaiming]
		// while not draging, check the axis user check
	ms_selectedAxis = 0;

	float size = ms_pCurrentPick->getAABB().GetRadius() > 5.0? 5.0 : ms_pCurrentPick->getAABB().GetRadius();

	if (selmode)
	{
		if(checkSelected(GKSTUDIO_AXIS_X, size ))
		{
			ms_selectedAxis = GKSTUDIO_AXIS_X;
			return true;
		}
		else if (checkSelected(GKSTUDIO_AXIS_Y, size))
		{
			ms_selectedAxis = GKSTUDIO_AXIS_Y;
			return true;
		}
		else if (checkSelected(GKSTUDIO_AXIS_Z, size))
		{
			ms_selectedAxis = GKSTUDIO_AXIS_Z;
			return true;
		}
	}
	return false;
}

void gkObjectManager::setCurrHover( IGameObject* pEntity )
{
// 	if (ms_pCurrentHover && ms_pCurrentHover != ms_pCurrentPick)
// 	 ms_pCurrentHover->setMaskColor(0);
// 	ms_pCurrentHover = pEntity;
// 	ms_pCurrentHover->setMaskColor(0.9f);
}

IGameObject* gkObjectManager::getCurrSelection()
{
	return ms_pCurrentPick;
}

bool gkObjectManager::OnInputEvent( const SInputEvent &event )
{
	switch (event.deviceId)
	{
	case eDI_Keyboard:
		{
			switch(event.keyId)
			{
			case eKI_Delete:

				if( ms_pCurrentPick )
				{
					gEnv->pGameObjSystem->DestoryGameObject( ms_pCurrentPick );
					ms_pCurrentPick = NULL;
				}

				break;
			case eKI_LShift:

				if (event.state == eIS_Pressed)
				{
					m_cloneMode = true;
				}
				else if (event.state == eIS_Released)
				{
					m_cloneMode = false;
				}
			}

			return false;
		}
	case eDI_Mouse:
		{
			if (event.state == eIS_Pressed)
			{
				switch (event.keyId)
				{
				case eKI_Mouse1:
					OnLButtonPressed();
					m_isDarging = true;
					return false;
				}
				return false;
			}
			if (event.state == eIS_Released)
			{
				switch (event.keyId)
				{
				case eKI_Mouse1:
					m_isDarging = false;
					return false;
				}
				return false;
			}


			return false;
		}
	}
	return false;
}


void gkObjectManager::OnLButtonPressed()
{
	// check icon first
	//  [1/1/2015 gameKnife]
	Vec2 curcor_pos = GetIEditor()->getMainViewport()->getCursorOnClientScreen();

	const IGameObjectQueue& list = gEnv->pGameObjSystem->GetGameObjects();

	for (IGameObjectQueue::const_iterator it = list.begin(); it != list.end(); ++it)
	{
		switch( (*it)->getGameObjectSuperClass() )
		{
		case eGOClass_LIGHT:
			{
				Vec3 pos = (*it)->getRenderLayer()->getWorldPosition();
				Vec2 ptx;
				if( GetIEditor()->getMainViewport()->getScreenPosFromScene(pos, ptx) )
				{
					if( (ptx - curcor_pos).GetLength() < 16.0f )
					{
						setCurrSelected(*it);
						return;
					}
					
				}
			}
			break;
		default:

			break;
		}
	}

	// check only in views
	if ( !(GetIEditor()->getMainViewport()->isCursorInView()))
		return;

	// if we got select a axis, just move it [8/27/2011 Kaiming-Desktop]
	if(ms_selectedAxis == 0)
	{
		Ray rayFromScreen = GetIEditor()->getMainViewport()->getRayFronScreen();

		static int pick_index = 0;

		IGameObject* pPick = gEnv->p3DEngine->getRayHitEntity(rayFromScreen, 0);
		if (pPick)
		{
			GetIEditor()->getObjectMng()->setCurrSelected(pPick);	
			ms_pCurrentPick->showBBox(true);
			//pick_index++;
		}
		else
		{
			GetIEditor()->getObjectMng()->setCurrSelected(NULL);	
			//ms_pCurrentPick = NULL;
		}
		return;
	}
	else
	{
		if (ms_pCurrentPick)
		{
			Vec2 getPt;
			GetIEditor()->getMainViewport()->getScreenPosFromScene(ms_pCurrentPick->getWorldPosition(), getPt);
			POINT pt;
			pt.x = getPt.x;
			pt.y = getPt.y;
			pt =  GetIEditor()->getMainViewport()->getPtClientToScreen(pt);

			Vec2 curPt = GetIEditor()->getMainViewport()->getCursorOnClientScreen();
			m_dragOffset = getPt - curPt;
		}
	}

}

void gkObjectManager::UpdateGizmoDraging( uint8 selmode )
{
	if (!ms_pCurrentPick || !ms_selectedAxis)
		return;

	// select a axis and draging
		switch(selmode)
		{
		case GKSTUDIO_OBJMODE_MOV:
			{
				//m_dragOffset
				if (m_cloneMode)
				{
					ms_pCurrentPick = ms_pCurrentPick->clone(_T(""));

					m_cloneMode = false;
				}

				Ray rayFromScreen = GetIEditor()->getMainViewport()->getRayFronScreenOffset( m_dragOffset );
				Plane intersectionPlane;
				Vec3 secondProjectionNormal;
				Vec3 raycastPos;
				Vec3 finalPos;

				switch ( ms_selectedAxis )
				{
				case GKSTUDIO_AXIS_X:
					// use axis z as plane normal
					intersectionPlane = Plane::CreatePlane( ms_pCurrentPick->getOrientation().GetColumn2(), ms_pCurrentPick->getWorldPosition() );
					// use axis y as second proj normal
					secondProjectionNormal = ms_pCurrentPick->getOrientation().GetColumn1();
					break;
				case GKSTUDIO_AXIS_Y:
					// use axis z as plane normal
					intersectionPlane = Plane::CreatePlane( ms_pCurrentPick->getOrientation().GetColumn2(), ms_pCurrentPick->getWorldPosition() );
					// use axis X as second proj normal
					secondProjectionNormal = ms_pCurrentPick->getOrientation().GetColumn0();
					break;
				case GKSTUDIO_AXIS_Z:
					// use axis X as plane normal
					intersectionPlane = Plane::CreatePlane( ms_pCurrentPick->getOrientation().GetColumn0(), ms_pCurrentPick->getWorldPosition() );
					// use axis y as second proj normal
					secondProjectionNormal = ms_pCurrentPick->getOrientation().GetColumn1();
					break;
				}

				// get the insection point from SCREENRAY on specific plane
				if( Intersect::Ray_Plane( rayFromScreen, intersectionPlane, raycastPos, false ) )
				{
					gEnv->pRenderer->getAuxRenderer()->AuxRender3DLine( ms_pCurrentPick->getWorldPosition(), raycastPos, ColorF(1,0,0,1) );

					// cast raycastpoint to a moving vector
					raycastPos = raycastPos - ms_pCurrentPick->getWorldPosition();



					// project the moving vector to the second specific plane
					finalPos = Vec3::CreateProjection( raycastPos, secondProjectionNormal );

					// 如果向量過長，應該是出問題了
					//if ( finalPos.GetLengthSquared() < 10000)
					{
						ms_pCurrentPick->setPosition(ms_pCurrentPick->getWorldPosition() + finalPos);
					}
				}


				
				break;
			}
		case GKSTUDIO_OBJMODE_ROT:
			{
				Vec2 mouseDelta = GetIEditor()->getMouseDelta();
				Vec3 rotateAxis;

				switch ( ms_selectedAxis )
				{
					case GKSTUDIO_AXIS_X:
						rotateAxis = Vec3(1,0,0);
						break;
					case GKSTUDIO_AXIS_Y:
						rotateAxis = Vec3(0,1,0);
						break;
					case GKSTUDIO_AXIS_Z:
						rotateAxis = Vec3(0,0,1);
						break;


				}


				ms_pCurrentPick->rotate( rotateAxis, mouseDelta.x * 0.002f, TS_LOCAL );
				break;
			}
		case GKSTUDIO_OBJMODE_SCL:
			{
				Vec2 mouseDelta = GetIEditor()->getMouseDelta();
				Vec3 rotateAxis;

				switch ( ms_selectedAxis )
				{
				case GKSTUDIO_AXIS_X:
					rotateAxis = Vec3(1,0,0);
					break;
				case GKSTUDIO_AXIS_Y:
					rotateAxis = Vec3(0,1,0);
					break;
				case GKSTUDIO_AXIS_Z:
					rotateAxis = Vec3(0,0,1);
					break;


				}


				//ms_pCurrentPick->rotate( rotateAxis, mouseDelta.x * 0.002f, TS_LOCAL );
				ms_pCurrentPick->setScale(  ms_pCurrentPick->getScale() * ( 1 + mouseDelta.x * 0.002f) );
				break;
			}
		}

}

void gkObjectManager::Destroy()
{
	m_icon_light.setNull();
}

