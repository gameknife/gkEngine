#include "TestCaseFramework.h"
#include "IResourceManager.h"
#include "IHavok.h"
#include "TestCase_Template.h"
#include "I3DEngine.h"
#include "ICamera.h"

TEST_CASE_FASTIMPL_HEAD( TestCase_LoadStaticGeo, eTcc_Loading )

	virtual void OnInit() 
	{
        ICamera* maincam = gEnv->p3DEngine->getMainCamera();
        maincam->setPosition(0, -25, 10);
        maincam->setOrientation( Quat::CreateRotationXYZ( Ang3( DEG2RAD(-25), 0,0 ) ) );
        
		m_timer = 0;
		m_index = 0;
		m_creations.clear();

		// create a plane
		Vec3 postmp(0,0,0);
		Quat rottmp = Quat::CreateRotationZ(DEG2RAD(180));
		m_plane = gEnv->pGameObjSystem->CreateStaticGeoGameObject( _T("test_case_plane"), _T("objects/test_case/test_lightmap.gmf"), postmp, rottmp );
		if (m_plane)
		{
			IGameObjectRenderLayer* pRenderLayer = m_plane->getRenderLayer();
			if (pRenderLayer)
			{
				pRenderLayer->setMaterialName(_T("objects/test_case/test_lightmap.mtl"));
			}

			//m_plane->setScale( 10, 10, 10);
		}

	}

	virtual bool OnUpdate() 
	{
		m_timer += gEnv->pTimer->GetFrameTime();

		//gEnv->pRenderer->getAuxRenderer()->AuxRenderScreenBox( Vec2(0, 0), Vec2(gEnv->pRenderer->GetScreenWidth(), 50), ColorB(0,0,0,128) );
		gEnv->pInGUI->gkGUIButton( _T("PRESS [L] TO CREATE"), Vec2(gEnv->pRenderer->GetScreenWidth() / 2 - 100, 100), 200, 60, ColorB(255,255,255,255), ColorB(0,0,0,128));

		return true;
	}

	virtual void OnDestroy() 
	{
		for (uint32 i=0; i < m_creations.size(); ++i)
		{
			gEnv->pGameObjSystem->DestoryGameObject( m_creations[i] );
		}
		
		gEnv->pGameObjSystem->DestoryGameObject( m_plane );

		gEnv->pSystem->cleanGarbage();
	}

	virtual void OnInputEvent( const SInputEvent &event ) 
	{
        static bool first = true;
		if ( (event.keyId == eKI_Android_Touch && event.state == eIS_Pressed ) || (event.keyId == eKI_L && event.state == eIS_Pressed) )
		{
            if (!first) {
                return;
            }
            
			Vec3 pos;
			Quat rot;
			pos = Vec3(m_index % 10 - 5, m_index / 10,0);
			rot = Quat::CreateIdentity();

			float now = gEnv->pTimer->GetAsyncCurTime();

			IGameObject* object = gEnv->pGameObjSystem->CreateStaticGeoGameObject( _T("objects/characters/faraa/faraa_rig.gmf"), pos, rot );
			if (object)
			{
				IGameObjectRenderLayer* pRenderLayer = object->getRenderLayer();
				if (pRenderLayer)
				{
					pRenderLayer->setMaterialName(_T("objects/characters/faraa/faraa.mtl"));
				}

				object->setPosition(pos);
				m_creations.push_back(object);

				if (gEnv->pPhysics)
				{
					IGameObjectPhysicLayer* pPhysicLayer = gEnv->pPhysics->CreatePhysicLayer();
					object->setGameObjectLayer(pPhysicLayer);
					pPhysicLayer->createStatic();
				}


				

			}
			
			m_index++;

			now = gEnv->pTimer->GetAsyncCurTime() - now;

			gkLogMessage(_T("Load Mesh use %.2fms."), now * 1000);
		}
	}

	float m_timer;
	int m_index;
	std::vector<IGameObject*> m_creations;
	IGameObject* m_plane;

TEST_CASE_FASTIMPL_TILE( TestCase_LoadStaticGeo )
