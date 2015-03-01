#include "StableHeader.h"
#include "CharacterTrack.h"


CCharacterTrack::CCharacterTrack(void)
{
}


CCharacterTrack::~CCharacterTrack(void)
{
}

void CCharacterTrack::get_key_info( float time, const TCHAR*& strings, float& time_duration )
{
	ICharacterKey* result = search_prev_key( time );
	if( result )
	{
		strings = result->m_animname.c_str();
		// time passed
		time_duration = ( time - result->get_time() ) * result->m_speedScale;
	}

}
