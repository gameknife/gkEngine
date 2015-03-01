#include "StableHeader.h"

#include "DirectorTrack.h"



CDirectorTrack::CDirectorTrack(void)
{
}


CDirectorTrack::~CDirectorTrack(void)
{
}

void CDirectorTrack::get_key_info( float time, const TCHAR*& strings, float& time_duration )
{
	IDirectorKey* result = search_prev_key( time );
	if (result)
	{
		strings = result->m_target_cam_name.c_str();
	}
}
