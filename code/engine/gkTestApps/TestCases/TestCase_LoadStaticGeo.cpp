#include "TestCaseFramework.h"
#include "IResourceManager.h"
#include "IHavok.h"
#include "TestCase_Template.h"
#include "I3DEngine.h"
#include "ICamera.h"
#include "TestCaseUtil_Cam.h"
#include "IMaterial.h"

TEST_CASE_FASTIMPL_HEAD( TestCase_LoadStaticGeo, eTcc_Loading )

	float m_camDist;
	virtual void OnInit() 
	{
        ICamera* maincam = gEnv->p3DEngine->getMainCamera();
        maincam->setPosition(0, -2, 0);
        maincam->setOrientation( Quat::CreateRotationXYZ( Ang3( 0, 0,0 ) ) );
        
		m_timer = 0;
		m_index = 0;
		m_camDist = 1.0f;
		m_creations.clear();

		Vec3 zeropos = Vec3(0,0,0);
		Quat zerorot = Quat::CreateIdentity();
		IGameObject* m_plane = gEnv->pGameObjSystem->CreateStaticGeoGameObject( _T("test_case_plane"), _T("objects/basic/plane.gmf"), zeropos, zerorot );
		if (m_plane)
		{
			IGameObjectRenderLayer* pRenderLayer = m_plane->getRenderLayer();
			if (pRenderLayer)
			{
				pRenderLayer->setMaterialName(_T("objects/basic/grid10.mtl"));

				GKSHADERPARAM* para = pRenderLayer->getMaterial()->getLoadingParameterBlock()->getParam("g_MatDiffuse");
				if (para)
				{
					para->setValue( Vec4(0.3,0.3,0.3,1.0) );
				}
			}

			m_plane->setPosition(0,0,-0.005);
			m_plane->setScale( 20, 20, 20);
			m_creations.push_back(m_plane);
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

		gEnv->pSystem->cleanGarbage();
	}

	virtual void OnInputEvent( const SInputEvent &event ) 
	{
        static int count = 0;

		HandleModelViewCam(event, 0.002f, m_camDist, 0);

		if ( (event.keyId == eKI_Android_Touch && event.state == eIS_Pressed ) || (event.keyId == eKI_L && event.state == eIS_Pressed) )
		{
            if (count++ > 8) {
                return;
            }
            
			Vec3 pos;
			Quat rot;
			pos = Vec3((m_index % 3 - 1) * 0.5, (m_index / 3 - 1) * 0.5,0);
			rot = Quat::CreateIdentity();

			float now = gEnv->pTimer->GetAsyncCurTime();

			IGameObject* object = gEnv->pGameObjSystem->CreateStaticGeoGameObject( _T("objects/scene/m_venus.gmf"), pos, rot );
			if (object)
			{
				IGameObjectRenderLayer* pRenderLayer = object->getRenderLayer();
				if (pRenderLayer)
				{
					switch( count % 5 )
					{
						case 0:
							pRenderLayer->setMaterialName(_T("objects/scene/m_venus.mtl"));
							break;
						case 1:
							pRenderLayer->setMaterialName(_T("objects/scene/m_venus_blue.mtl"));
							break;
						case 2:
							pRenderLayer->setMaterialName(_T("objects/scene/m_venus_gold.mtl"));
							break;
						case 3:
							pRenderLayer->setMaterialName(_T("objects/scene/m_venus_metal.mtl"));
							break;
						case 4:
							pRenderLayer->setMaterialName(_T("objects/scene/m_venus_red.mtl"));
							break;
					}
					
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

TEST_CASE_FASTIMPL_TILE( TestCase_LoadStaticGeo )
