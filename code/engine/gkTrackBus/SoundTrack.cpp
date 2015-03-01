#include "StdAfx.h"
#include "SoundTrack.h"

CSoundTrack::CSoundTrack(void)
{
	prev_key = NULL;
}


CSoundTrack::~CSoundTrack(void)
{
}

void CSoundTrack::get_key_info( float time, const TCHAR*& strings, float& time_duration )
{
	ISoundKey* result = search_prev_key( time );
	if (result && result != prev_key)
	{
		strings = result->m_filename.c_str();
		prev_key = result;
	}
}
