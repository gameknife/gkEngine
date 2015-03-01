#include "TestCaseFramework.h"
#include "TestCaseUtil_Cam.h"

#include "TestCase_Template.h"

TEST_CASE_FASTIMPL_HEAD( TestCase_CharacterAnimation, eTcc_Animation )

	virtual void OnInit() 
	{
		m_index = 0;
		m_creations.clear();
		m_camDist = 5.0f;

		Vec3 pos;
		Quat rot;
		pos = Vec3(0, 0, 0);
		rot = Quat::CreateIdentity();

		float now = gEnv->pTimer->GetAsyncCurTime();

		Vec3 postmp(0,0,0);
		Quat rottmp = Quat::CreateRotationZ(DEG2RAD(180));
		IGameObject* object = gEnv->pGameObjSystem->CreateAnimGameObject( _T("objects/characters/prophet/prophet.chr"), postmp, rottmp);
		if (object)
		{
			m_tgt = object;
			IGameObjectRenderLayer* pRenderLayer = object->getRenderLayer();
			if (pRenderLayer)
			{
				pRenderLayer->setMaterialName(_T("objects/characters/prophet/prophet.mtl"));

				//object->showBBox(true);
				object->setOrientation( Quat::CreateRotationZ(DEG2RAD(180)) );

			}

			object->setPosition(pos);
			//object->showBBox(true);

			m_creations.push_back(object);
		}

		Vec3 zeropos = Vec3(0,0,0);
		Quat zerorot = Quat::CreateIdentity();

		IGameObject* m_plane = gEnv->pGameObjSystem->CreateStaticGeoGameObject( _T("test_case_plane"), _T("objects/basic/plane.gmf"), zeropos, zerorot );
		if (m_plane)
		{
			IGameObjectRenderLayer* pRenderLayer = m_plane->getRenderLayer();
			if (pRenderLayer)
			{
				pRenderLayer->setMaterialName(_T("objects/basic/grid10.mtl"));
			}

			m_plane->setPosition(0,0,-0.005);
			m_plane->setScale( 20, 20, 20);
		}
	}

	virtual bool OnUpdate() 
	{
		if(!m_tgt)
		{
			return true;
		}

		if( gEnv->pInGUI->gkGUIButton( _T("idle"), Vec2(10, 100), 100, 50, ColorB(255,255,255,255), ColorB(0,0,0,128) ) )
		{
			IGameObjectAnimLayer* anilayer = reinterpret_cast<IGameObjectAnimLayer*>( m_tgt->getGameObjectLayer(eGL_AnimLayer) );
			anilayer->stopAllAnimation(0);
			anilayer->playAnimation(_T("idle"), 0);
			anilayer->setAnimationSpeed(_T("idle"), 1.0);
		}

		if( gEnv->pInGUI->gkGUIButton( _T("walk"), Vec2(10, 151), 100, 50, ColorB(255,255,255,255), ColorB(0,0,0,128) ) )
		{
			IGameObjectAnimLayer* anilayer = reinterpret_cast<IGameObjectAnimLayer*>( m_tgt->getGameObjectLayer(eGL_AnimLayer) );
			anilayer->stopAllAnimation(0);
			anilayer->playAnimation(_T("walk"), 0);
			anilayer->setAnimationSpeed(_T("walk"), 1.0);
		}

		if( gEnv->pInGUI->gkGUIButton( _T("run"), Vec2(10, 202), 100, 50, ColorB(255,255,255,255), ColorB(0,0,0,128) ) )
		{
			IGameObjectAnimLayer* anilayer = reinterpret_cast<IGameObjectAnimLayer*>( m_tgt->getGameObjectLayer(eGL_AnimLayer) );
			anilayer->stopAllAnimation(0);
			anilayer->playAnimation(_T("run"), 0);
			anilayer->setAnimationSpeed(_T("run"), 1.0);
		}

		if( gEnv->pInGUI->gkGUIButton( _T("crouch_opt"), Vec2(10, 253), 100, 50, ColorB(255,255,255,255), ColorB(0,0,0,128) ) )
		{
			IGameObjectAnimLayer* anilayer = reinterpret_cast<IGameObjectAnimLayer*>( m_tgt->getGameObjectLayer(eGL_AnimLayer) );
			anilayer->stopAllAnimation(0);
			anilayer->playAnimation(_T("crouch_opt"), 0);
			anilayer->setAnimationSpeed(_T("crouch_opt"), 1.0);
		}

		if( gEnv->pInGUI->gkGUIButton( _T("swing"), Vec2(10, 304), 100, 50, ColorB(255,255,255,255), ColorB(0,0,0,128) ) )
		{
			IGameObjectAnimLayer* anilayer = reinterpret_cast<IGameObjectAnimLayer*>( m_tgt->getGameObjectLayer(eGL_AnimLayer) );
			anilayer->stopAllAnimation(0);
			anilayer->playAnimation(_T("swing"), 0);
			anilayer->setAnimationSpeed(_T("swing"), 1.0);
		}
		return true;
	}

	virtual void OnDestroy() 
	{
		for (uint32 i=0; i < m_creations.size(); ++i)
		{
			gEnv->pGameObjSystem->DestoryGameObject( m_creations[i] );
		}
	}

	virtual void OnInputEvent( const SInputEvent &event ) 
	{
		HandleModelViewCam(event, 0.002f, m_camDist, m_tgt);

        static bool first = true;
		if ( (event.keyId == eKI_Android_Touch && event.state == eIS_Pressed ) || (event.keyId == eKI_L && event.state == eIS_Pressed))
		{

		}
	}


	float m_camDist;
	int m_index;
	std::vector<IGameObject*> m_creations;
	IGameObject* m_tgt;

TEST_CASE_FASTIMPL_TILE( TestCase_CharacterAnimation )


