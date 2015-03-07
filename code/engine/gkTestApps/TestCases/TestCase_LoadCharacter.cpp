#include "TestCaseFramework.h"
#include "TestCaseUtil_Cam.h"

#include "TestCase_Template.h"

TEST_CASE_FASTIMPL_HEAD( TestCase_LoadCharacter, eTcc_Loading )

	virtual void OnInit() 
	{
		m_index = 0;
		m_creations.clear();
	}

	virtual bool OnUpdate() 
	{
		gEnv->pInGUI->gkGUIButton( _T("PRESS [L] TO CREATE"), Vec2(gEnv->pRenderer->GetScreenWidth() / 2 - 100, 100), 200, 60, ColorB(255,255,255,255), ColorB(0,0,0,128));
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
        static bool first = true;
		if ( (event.keyId == eKI_Android_Touch && event.state == eIS_Pressed ) || (event.keyId == eKI_L && event.state == eIS_Pressed))
		{
//             if (!first) {
//                 return;
//             }
//             first = false;
			Vec3 pos;
			Quat rot;
			pos = Vec3(m_index % 10 - 5, m_index / 10,0);
			rot = Quat::CreateIdentity();

			float now = gEnv->pTimer->GetAsyncCurTime();

			Vec3 postmp(0,0,0);
			Quat rottmp = Quat::CreateRotationZ(DEG2RAD(180));
			IGameObject* object = gEnv->pGameObjSystem->CreateAnimGameObject( _T("objects/characters/prophet/prophet.chr"), postmp, rottmp);
			if (object)
			{
				IGameObjectRenderLayer* pRenderLayer = object->getRenderLayer();
				if (pRenderLayer)
				{
					pRenderLayer->setMaterialName(_T("objects/characters/prophet/prophet.mtl"));

					//object->showBBox(true);
					object->setOrientation( Quat::CreateRotationZ(DEG2RAD(180)) );
					IGameObjectAnimLayer* anilayer = reinterpret_cast<IGameObjectAnimLayer*>( object->getGameObjectLayer(eGL_AnimLayer) );
					anilayer->playAnimation(_T("idle"), 0);
                    anilayer->setAnimationSpeed(_T("run"), 1.0);
				}

				object->setPosition(pos);
				//object->showBBox(true);

				m_creations.push_back(object);
			}

			m_index++;

			now = gEnv->pTimer->GetAsyncCurTime() - now;

			gkLogMessage(_T("Load Chr use %.2fms."), now * 1000);
		}

		
	}

	int m_index;
	std::vector<IGameObject*> m_creations;

TEST_CASE_FASTIMPL_TILE( TestCase_LoadCharacter )


