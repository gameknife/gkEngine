#include "StableHeader.h"

#include "AnimDirectorNode.h"
#include "DirectorTrack.h"
#include "I3DEngine.h"


CAnimDirectorNode::CAnimDirectorNode(void)
{
	m_curr_cam = NULL;
}

CAnimDirectorNode::~CAnimDirectorNode(void)
{
}

void CAnimDirectorNode::update( const SMovieContext& context )
{
	if(m_default_track)
	{
		const TCHAR* camera_name = NULL;
		float duration = 0;
		m_default_track->get_key_info( context.m_curr_time,camera_name, duration );

		// change camera
		if (camera_name)
		{		
			ICamera* cam = gEnv->p3DEngine->getCamera( camera_name );
			if (cam && cam != m_curr_cam)
			{
				if (m_curr_cam)
				{
					gEnv->p3DEngine->popReplaceCamera();
				}

				m_curr_cam = cam;
				gEnv->p3DEngine->pushReplaceCamera( cam );
			}
		}

		
	}
}

void CAnimDirectorNode::create_default_track()
{
	m_default_track = (CDirectorTrack*)add_track(eATT_DirectorTrack);
}

void CAnimDirectorNode::end()
{
	gEnv->p3DEngine->popReplaceCamera();
	m_curr_cam = NULL;
}
