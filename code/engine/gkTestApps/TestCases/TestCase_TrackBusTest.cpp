#include "TestCaseFramework.h"
#include "TestCase_Template.h"
#include "ITerrianSystem.h"
#include "I3DEngine.h"
#include "ICamera.h"
#include "TestCaseUtil_Cam.h"
#include "IGameFramework.h"
#include "ITimeOfDay.h"

#include "ITrackBus.h"
#include "IAuxRenderer.h"

TEST_CASE_FASTIMPL_HEAD( TestCase_TrackBusTest, eTcc_Animation )

	IGameObject* object;
	Vec3 m_pos;
	IMovieShot* shot0;

virtual void OnInit() 
{
	// set camera
	ICamera* maincam = gEnv->p3DEngine->getMainCamera();

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


	ICamera* cam0 = gEnv->p3DEngine->createCamera( _T("cam0") );
	cam0->setFOVy( DEG2RAD(45) );
	cam0->setPosition(0,0,0);
	ICamera* cam1 = gEnv->p3DEngine->createCamera( _T("cam1") );
	cam1->setFOVy( DEG2RAD(60) );
	cam1->setPosition(10,10,10);
	cam1->lookAt(0,0,0);

	object = gEnv->pGameObjSystem->CreateAnimGameObject( _T("actor_ent0"), _T("objects/characters/prophet/prophet.chr"), zeropos, zerorot);
	if (object)
	{
		IGameObjectRenderLayer* pRenderLayer = object->getRenderLayer();
		if (pRenderLayer)
		{
			pRenderLayer->setMaterialName(_T("objects/characters/prophet/prophet.mtl"));

			//object->showBBox(true);
			object->setOrientation( Quat::CreateRotationZ(DEG2RAD(180)) );
// 			IGameObjectAnimLayer* anilayer = reinterpret_cast<IGameObjectAnimLayer*>( object->getGameObjectLayer(eGL_AnimLayer) );
// 			anilayer->playAnimation(_T("walk_f"), 0);
		}

		object->setPosition(Vec3(0,0,0));
		object->showBBox(true);
	}

	gEnv->p3DEngine->getTimeOfDay()->bindSunFocus( object );

	shot0 = gEnv->pTrackBus->CreateMovieShot( _T("shot0") );
	
	SSequenceParam param;
	param.m_length = 8;
	param.m_loop = 1;
	
	shot0->SetParam( param );

	//////////////////////////////////////////////////////////////////////////
	// entity node test
	IMovieActorNode* director = gEnv->pTrackBus->CreateActor( eANT_DirectorNode );
	shot0->AddAnimNode( director );

	IMovieTrack* track_director = director->get_track( eATT_DirectorTrack );

	{
		IDirectorKey key(0);
		key.m_target_cam_name = _T("cam0");
		track_director->set_key( &key );
	}

	{
		IDirectorKey key(4);
		key.m_target_cam_name = _T("cam1");
		track_director->set_key( &key );
	}

	{
		IDirectorKey key(6);
		key.m_target_cam_name = _T("MainCamera");
		track_director->set_key( &key );
	}

	TCHAR* name = _T("actor_ent0");
	IMovieActorNode* node = gEnv->pTrackBus->CreateActor( eANT_EntityNode, name );
	shot0->AddAnimNode( node );

	TCHAR* cam_name = _T("cam0");
	IMovieActorNode* camnode = gEnv->pTrackBus->CreateActor( eANT_CamNode, cam_name );
	shot0->AddAnimNode( camnode );

	IMovieTrack* track_pos = node->get_track( eATT_PositionTrack);
	IMovieTrack* track_rot = node->get_track( eATT_RotationTrack);
	IMovieTrack* track_chr = node->add_track( eATT_CharacterTrack );

	IMovieTrack* track_cam_pos = camnode->get_track( eATT_PositionTrack );
	IMovieTrack* track_cam_at = camnode->get_track( eATT_CamTargetTrack );

	ICharacterKey key(0);
	key.m_animname = _T("walk_f");
	key.m_speedScale = 1.0;
	track_chr->set_key( &key );


	float radius = 4.0;
	Vec3 spline[5];
	spline[0] = Vec3(0,0,0);
	spline[1] = Vec3(radius,radius,0);
	spline[2] = Vec3(radius * 2,0,0);
	spline[3] = Vec3(radius,-radius,0);
	spline[4] = Vec3(0,0,0);


	for (uint8 i=0; i < 5; ++i)
	{
		Vec3 tangent = Vec3(0,0,0);
		if (i > 0 && i < 4)
		{
			tangent = spline[i + 1] - spline[i - 1];
		}
		else
		{
			tangent = Vec3(0,10,0);
		}

		ISplineVec3Key key(i * 2);
		key.m_value = spline[i];
		key.m_dd = tangent;
		key.m_ds = -tangent;

		track_pos->set_key(  &key );
		track_cam_at->set_key( &key );

		key.m_value = Vec3(0,0,DEG2RAD(-90 * i));
		key.m_dd = Vec3(0,0,0);
		key.m_ds = Vec3(0,0,0);

		track_rot->set_key( &key );
	}

	{
		ISplineVec3Key key(0);
		key.m_value = Vec3(0,-10,5);
		key.m_dd = Vec3(0,0,0);
		key.m_ds = Vec3(0,0,0);
		track_cam_pos->set_key( &key );
	}


	{
		ISplineVec3Key key(8);
		key.m_value = Vec3(0,-10,5);
		key.m_dd = Vec3(0,0,0);
		key.m_ds = Vec3(0,0,0);
		track_cam_pos->set_key( &key );
	}
}

virtual bool OnUpdate() 
{
	ColorF col = ColorF(0,0,0,0.3);

	gEnv->pRenderer->getAuxRenderer()->AuxRender3DGird(ZERO_POS, 40, 1, col );

	Matrix44 mat;
	mat.SetIdentity();
	gEnv->pRenderer->getAuxRenderer()->AuxRenderGizmo( mat, 1 );


	if( gEnv->pInGUI->gkGUIButton( _T("Play"), Vec2(10, 100), 100, 50, ColorB(255,255,255,255), ColorB(0,0,0,128) ) )
	{
		SSequenceStartParam startParam;
		shot0->Play( startParam );
	}

	if( gEnv->pInGUI->gkGUIButton( _T("Pause"), Vec2(10, 151), 100, 50, ColorB(255,255,255,255), ColorB(0,0,0,128) ) )
	{
		static bool playing = true;
		if (playing)
		{
			shot0->Pause();
		}
		else
		{
			shot0->Resume();
		}
		
		playing = !playing;
	}

	return true;
}

virtual void OnDestroy() 
{
	gEnv->pGameObjSystem->DestoryGameObject(object);
	//gEnv->pGameFramework->LoadLevel( _T("void") );
	gEnv->pSystem->cleanGarbage();
}

virtual void OnInputEvent( const SInputEvent &event ) 
{
	HandleFreeCam( event );

	switch( event.keyId )
	{
	case eKI_P:
		{
			if (event.state == eIS_Released)
			{
				SSequenceStartParam startParam;
				shot0->Play( startParam );
			}
		}
		break;
	case eKI_L:
		{
			if (event.state == eIS_Released)
			{
				static bool playing = true;
				if (playing)
				{
					shot0->Pause();
				}
				else
				{
					shot0->Resume();
				}

				playing = !playing;
			}
		}
		break;
	}
}

TEST_CASE_FASTIMPL_TILE( TestCase_TrackBusTest )