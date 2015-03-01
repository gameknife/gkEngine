#include "StableHeader.h"

#include "AnimEntityNode.h"
#include "IGameObject.h"
#include "CharacterTrack.h"
#include "IGameObjectLayer.h"


CAnimEntityNode::CAnimEntityNode(const TCHAR* name):CAnimNodeBase(true), m_target_name( name )
{
	m_target = NULL;
}


CAnimEntityNode::~CAnimEntityNode(void)
{
}

void CAnimEntityNode::update( const SMovieContext& context )
{
 	if (!m_target)
	{
 		m_target = gEnv->pGameObjSystem->GetGameObjectByName( m_target_name );
 	}
// 
	if (m_target)
	{
		// transformation
		Vec3 pos;
		m_pos_track->get_value( context.m_curr_time, pos );
		Vec3 angle;
		m_rot_track->get_value( context.m_curr_time, angle);
		Quat rot = Quat::CreateRotationXYZ(Ang3(angle.x, angle.y, angle.z));
		Vec3 scale;
		m_scale_track->get_value( context.m_curr_time, scale );

		m_target->setPosition( pos );
		m_target->setOrientation( rot );
		m_target->setScale( scale );

		// animation
		CCharacterTrack* animTrack = (CCharacterTrack*)get_track(eATT_CharacterTrack);
		IGameObjectAnimLayer* animLayer = (IGameObjectAnimLayer*)m_target->getGameObjectLayer( eGL_AnimLayer );
		if ( animTrack && animLayer)
		{
			const TCHAR* animfile = NULL;
			float timepass = 0;
			animTrack->get_key_info( context.m_curr_time, animfile, timepass);

			if (animfile)
			{
				animLayer->playAnimation( animfile, 0 );

				//animLayer->getAnimationCount()
				animLayer->setAnimationSpeed( animfile, 0 );
				animLayer->setAnimationLocalTime( animfile, timepass );
			}
		}


	}



}

void CAnimEntityNode::create_default_track()
{
	m_pos_track = (CSplineVec3Track*)add_track( eATT_PositionTrack );
	m_rot_track = (CSplineVec3Track*)add_track( eATT_RotationTrack );
	m_scale_track = (CSplineVec3Track*)add_track( eATT_ScalingTrack );
}
