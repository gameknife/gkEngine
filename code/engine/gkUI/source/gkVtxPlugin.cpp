#include "UIStableHeader.h"
#include "gkVtxPlugin.h"

#include "gkMovieMovable.h"
#include "gkMovieMovableShape.h"
#include "gkMovieMovableEditText.h"
#include "gkMovieMovableStaticText.h"
#include "gkVtxTexture.h"

#include "vtxInstanceManager.h"
#include "vtxRoot.h"

//-----------------------------------------------------------------------
#ifdef VTX_STATIC_LIB
void vektrix_gkVtxPlugin_startPlugin();
#else
extern "C" void GAMEKNIFEUI_API startPlugin() throw()
#endif
{
	vtx::Root::getSingletonPtr()->registerPlugin(new vtx::gkVtxPlugin());
}
//-----------------------------------------------------------------------

namespace vtx
{
		//-----------------------------------------------------------------------
		gkVtxPlugin::gkVtxPlugin() 
			: instMgr(InstanceManager::getSingletonPtr())
		{
			// Generic
			mTexture = new gkVtxTextureFactory;
			instMgr->textures()->addFactory(mTexture);

			// Movies
			initMovableMovie();
			initTextureMovie();
		}
		//-----------------------------------------------------------------------
		gkVtxPlugin::~gkVtxPlugin()
		{
			freeMovableMovie();
			freeTextureMovie();

			// Generic
			instMgr->textures()->removeFactory(mTexture);
			delete mTexture;
		}
		//-----------------------------------------------------------------------
		void gkVtxPlugin::initMovableMovie()
		{
			// Instances
			mMovableEditText = new gkMovableEditTextFactory;
			mMovableShape = new gkMovableShapeFactory;
			mMovableStaticText = new gkMovableStaticTextFactory;

			instMgr->addFactory(mMovableEditText);
			instMgr->addFactory(mMovableShape);
			instMgr->addFactory(mMovableStaticText);

			// Movie
			mMovableMovie = new gkMovableMovieFactory;
			Root::getSingletonPtr()->addFactory(mMovableMovie);
		}
		//-----------------------------------------------------------------------
		void gkVtxPlugin::freeMovableMovie()
		{
			// Movie
			Root::getSingletonPtr()->removeFactory(mMovableMovie);
			delete mMovableMovie;

			// Instances
			instMgr->removeFactory(mMovableShape);
			instMgr->removeFactory(mMovableStaticText);
			instMgr->removeFactory(mMovableEditText);

			delete mMovableShape;
		}
		//-----------------------------------------------------------------------
		void gkVtxPlugin::initTextureMovie()
		{
			// Movie
		}
		//-----------------------------------------------------------------------
		void gkVtxPlugin::freeTextureMovie()
		{
			// Movie
		}
		//-----------------------------------------------------------------------
}
