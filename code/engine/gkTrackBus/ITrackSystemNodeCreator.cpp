#include "stdafx.h"
#include "ITrackSystemNodeCreator.h"

#include "AnimCameraNode.h"
#include "AnimEntityNode.h"
#include "AnimSoundNode.h"
#include "SoundTrack.h"
#include "AnimDirectorNode.h"

IAnimNode* AnimNodeCreator::create_anim_node_by_type( UINT32 type, void* param1, void* param2 )
{
	IAnimNode* ret = NULL;

	switch( type )
	{
	case eANT_CamNode:

		ret = new CAnimCameraNode( *((tstring*)param1) );

		break;
	case eANT_DirectorNode:
		ret = new CAnimDirectorNode( );

		break;
	case eANT_EntityNode:

		ret = new CAnimEntityNode( *((tstring*)param1) );

		break;

	case eANT_SoundNode:

		ret = new CAnimSoundNode();

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
