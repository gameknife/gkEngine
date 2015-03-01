#include "StdAfx.h"
#include "AnimSoundNode.h"
#include "SoundTrack.h"
#include "TrackSystem.h"
#include "SoundManager.h"

CAnimSoundNode::CAnimSoundNode(void)
{
}


CAnimSoundNode::~CAnimSoundNode(void)
{
}

void CAnimSoundNode::update( const SAnimateContext& context )
{
	if (m_default_track)
	{
		const TCHAR* filename = NULL;
		float duration;
		m_default_track->get_key_info( context.m_curr_time, filename, duration );

		if (filename)
		{
			get_xsystem()->m_sound_manager->play_background_music( filename );
		}
	}
}

void CAnimSoundNode::create_default_track()
{
	m_default_track = (CSoundTrack*)(add_track( eATT_SoundTrack));
}
