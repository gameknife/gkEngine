#include "StableHeader.h"

#include "AnimNodeBase.h"
#include "SoundTrack.h"
#include "TSplineTrack.h"
#include "DirectorTrack.h"
#include "SplineFloatTrack.h"
#include "CharacterTrack.h"

CAnimNodeBase::CAnimNodeBase(bool construct_tcb)
{
	m_tracks.assign( eATT_Max, NULL );
}

CAnimNodeBase::~CAnimNodeBase(void)
{
	TrackList::iterator it = m_tracks.begin();
	for (; it != m_tracks.end(); ++it)
	{
		delete (*it);
	}
}

void CAnimNodeBase::update( const SMovieContext& context )
{

}

IMovieTrack* CAnimNodeBase::add_track( EAnimTrackType type )
{
	IMovieTrack* ret = NULL;
	switch( type )
	{
	case eATT_PositionTrack:
		ret = new CSplineTCBTrack(Vec3(0,0,0));
		break;
	case eATT_RotationTrack:
		ret = new CSplineVec3Track(0.0f);
		break;
	case eATT_ScalingTrack:
		ret = new CSplineVec3Track(1.0f);
		break;
	case eATT_CamTargetTrack:
		ret = new CSplineTCBTrack(Vec3(0,0,0));
		break;
	case eATT_SoundTrack:
		//ret = new CSoundTrack();
		break;
	case eATT_DirectorTrack:
		ret = new CDirectorTrack();
		break;
	case eATT_CamFovTrack:
		ret = new CSplineFloatTrack(1.0f);
		break;
	case eATT_CharacterTrack:
		ret = new CCharacterTrack();
		break;
	default:
		break;
	}

	if (ret)
	{
		m_tracks[type] = ret;
	}

	return ret;
}

void CAnimNodeBase::remove_track( IMovieTrack* track )
{
	TrackList::iterator it = m_tracks.begin();
	for (; it != m_tracks.end(); ++it)
	{
		if (*it == track)
		{
			m_tracks.erase(it);
			break;
		}
	}
}

TrackList& CAnimNodeBase::get_tracklist()
{
	return m_tracks;
}

IMovieTrack* CAnimNodeBase::get_track( EAnimTrackType type )
{
	assert( type < eATT_Max );
	return m_tracks[type];
}
