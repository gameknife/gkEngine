#include "StableHeader.h"

#include "gkTrackBus.h"
#include "gkMovieShot.h"
#include "AnimCameraNode.h"
#include "AnimDirectorNode.h"
#include "AnimEntityNode.h"


gkTrackBus::gkTrackBus(void)
{
}


gkTrackBus::~gkTrackBus(void)
{
}

void gkTrackBus::Update( float frameTime )
{
	
	float elpaseTime = gEnv->pTimer->GetFrameTime();

	MovieShotMap::iterator it = m_movieShots.begin();
	for (; it != m_movieShots.end(); ++it)
	{
		if (it->second)
		{
			it->second->Update( elpaseTime );
		}
	}
}

IMovieShot* gkTrackBus::CreateMovieShot(const gkStdString& name)
{
	MovieShotMap::iterator it = m_movieShots.find( name );
	if (it != m_movieShots.end())
	{
		return it->second;
	}

	gkMovieShot* new_seq = new gkMovieShot();

	m_movieShots.insert( MovieShotMap::value_type(name, new_seq) );

	return new_seq;
}

IMovieShot* gkTrackBus::GetMovieShot( const gkStdString& name )
{
	MovieShotMap::iterator it = m_movieShots.find( name );
	if (it != m_movieShots.end())
	{
		return it->second;
	}

	return NULL;
}

IMovieActorNode* gkTrackBus::CreateActor( EAnimNodeType type, void* param1, void* param2 )
{
	IMovieActorNode* ret = NULL;

	switch( type )
	{
	case eANT_CamNode:

		ret = new CAnimCameraNode( (TCHAR*)param1 );

		break;
	case eANT_DirectorNode:
		ret = new CAnimDirectorNode( );

		break;
	case eANT_EntityNode:
		ret = new CAnimEntityNode( (TCHAR*)param1 );
		break;

	case eANT_SoundNode:

		//ret = new CAnimSoundNode();

		break;
	default:

		break;
	}



	if (ret)
	{
		ret->create_default_track();
	}

	return ret;
}
