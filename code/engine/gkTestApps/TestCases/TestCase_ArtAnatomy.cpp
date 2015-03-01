#include "TestCaseFramework.h"
#include "TestCaseUtil_Cam.h"

#include "TestCase_Template.h"
#include "IGameFramework.h"

TEST_CASE_FASTIMPL_HEAD( TestCase_ArtAnatomy, eTcc_System )

virtual void OnInit()
{
    m_creations.clear();
    
	gEnv->pGameFramework->LoadLevel( _T("level/art_anatomy/art_anatomy.gks") );
    
	m_target = NULL;

	

    camDist = 20.0f;
}

virtual bool OnUpdate()
{
	if(!m_target)
	{
		 m_target = gEnv->pGameObjSystem->GetGameObjectByName( _T("GameObject006") );
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
    static bool first = true;
    
    HandleModelViewCam( event, 0.01, camDist, m_target);

	switch ( event.deviceId )
	{
	case eDI_Mouse:
		{

			if (event.keyId == eKI_Mouse3)
			{
				if (event.state == eIS_Released)
				{
					

//					IGameObject* go = gEnv->p3DEngine->getRayHitEntity( gEnv->pRenderer->GetRayFromScreen( gEnv->pRenderer->GetScreenWidth() / 2, gEnv->pRenderer->GetScreenHeight() / 2 ) );
//				
//					if (go)
//					{
//						m_target = go;
//					}
				}
			}
		}
	}
}

std::vector<IGameObject*> m_creations;
IGameObject* m_target;
float camDist;

TEST_CASE_FASTIMPL_TILE( TestCase_ArtAnatomy )


