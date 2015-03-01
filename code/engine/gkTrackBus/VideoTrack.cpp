#include "StdAfx.h"
#include "VideoTrack.h"


CVideoTrack::CVideoTrack(void)
{
	prev_key = NULL;
}


CVideoTrack::~CVideoTrack(void)
{
}

void CVideoTrack::get_key_info( float time, const TCHAR*& strings, float& time_duration )
{
	IVideoKey* result = search_prev_key( time );
	if (result && result != prev_key)
	{
		strings = result->m_filename.c_str();
		prev_key = result;
	}
}
