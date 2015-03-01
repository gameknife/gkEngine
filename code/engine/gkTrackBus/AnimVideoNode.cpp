#include "StdAfx.h"
#include "AnimVideoNode.h"
#include "VideoTrack.h"
#include "TrackSystem.h"


CAnimVideoNode::CAnimVideoNode(void)
{
}


CAnimVideoNode::~CAnimVideoNode(void)
{
}

void CAnimVideoNode::update( const SAnimateContext& context )
{
	if (m_default_track)
	{
		const TCHAR* filename = NULL;
		float duration;
		m_default_track->get_key_info( context.m_curr_time, filename, duration );

		if (filename)
		{
			if(get_xsystem()->m_video_manager)
			{

			}
		}
	}
}

void CAnimVideoNode::create_default_track()
{
	m_default_track = (CVideoTrack*)add_track( eATT_VideoTrack );
}
