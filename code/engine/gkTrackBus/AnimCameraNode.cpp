#include "StableHeader.h"

#include "AnimCameraNode.h"
#include "SplineFloatTrack.h"
#include "I3DEngine.h"
#include "ICamera.h"
#include "IAuxRenderer.h"



CAnimCameraNode::CAnimCameraNode(const TCHAR* cam_name):CAnimNodeBase(true), m_cam_name(cam_name)
{
	m_target = NULL;
}

CAnimCameraNode::~CAnimCameraNode(void)
{
}

void CAnimCameraNode::update( const SMovieContext& context )
{
	if (!m_target)
	{
		m_target = gEnv->p3DEngine->getCamera( m_cam_name );
	}

	if (m_target)
	{
		Vec3 pos;
		m_pos_track->get_value( context.m_curr_time, pos );
		m_target->setPosition( pos );

		Vec3 at;
		m_target_track->get_value( context.m_curr_time, at );
		m_target->lookAt( at );
	}
}

void CAnimCameraNode::create_default_track()
{
	m_pos_track = (CSplineVec3Track*)add_track(eATT_PositionTrack);
	m_target_track = (CSplineVec3Track*)add_track(eATT_CamTargetTrack);
	m_fov_track = (CSplineFloatTrack*)add_track(eATT_CamFovTrack);
}
