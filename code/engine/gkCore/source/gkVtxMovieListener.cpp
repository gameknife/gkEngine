#include "gkStableHeader.h"

#if 0
#include "gkVtxMovieListener.h"


#include "vtxScriptEngine.h"

//#ifndef WIN64
//-----------------------------------------------------------------------
gkVtxMovieListener::gkVtxMovieListener():m_pAttachedMovie(NULL)
{
}
//-----------------------------------------------------------------------
gkVtxMovieListener::~gkVtxMovieListener()
{

}
//-----------------------------------------------------------------------
vtx::ScriptParam gkVtxMovieListener::scriptCallback( const vtx::String& callback_name, const vtx::ScriptParamList& args )
{
	functionCallFromAS3(callback_name);
	return __TIMESTAMP__;
}
//-----------------------------------------------------------------------
bool gkVtxMovieListener::loadingCompleted( vtx::Movie* movie )
{
	m_pAttachedMovie = movie;
	movie->getScriptEngine()->setCallbackListener(this);
	return false;
}
//-----------------------------------------------------------------------
bool gkVtxMovieListener::loadingFailed( vtx::Movie* movie )
{
	return true;
}
//-----------------------------------------------------------------------
void gkVtxMovieListener::callFunctionInAS3( const std::string& callback_name, int argc /*= 0*/, float argv[] /*= 0 */ )
{
	if(m_pAttachedMovie)
	{
		vtx::ScriptParamList args;
		for (int i=0; i<argc; ++i)
		{
			args.push_back(argv[i]);
		}

		m_pAttachedMovie->getScriptEngine()->callScriptFunction(callback_name, args);
	}
}

#endif