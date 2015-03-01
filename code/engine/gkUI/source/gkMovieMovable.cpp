#include "UIStableHeader.h"
#include "gkMovieMovable.h"

#include "gkMovieMovableEditText.h"
#include "gkMovieMovableShape.h"
#include "gkMovieMovableStaticText.h"

#include "gkMovieShapeAtlasElement.h"
#include "gkVtxGlyphAtlasElement.h"

#include "vtxFileManager.h"
#include "vtxInstance.h"
#include "vtxInstancePool.h"
#include "vtxLogManager.h"
#include "vtxResource.h"

#include "vtxButton.h"
#include "vtxMovieClip.h"
#include "vtxShape.h"
#include "vtxShapeResource.h"
#include "vtxFontResource.h"
#include "vtxInstanceManager.h"


namespace vtx
{
	//-----------------------------------------------------------------------
	gkMovableMovie::gkMovableMovie(const String& name, MovieFactory* creator) 
		: Movie(name, creator)
	{
		//mAAB.setInfinite();
		mFactory = static_cast<gkMovableMovieFactory*>(creator);
	}
	//-----------------------------------------------------------------------
	gkMovableMovie::~gkMovableMovie()
	{
		destroy();
	}
	//-----------------------------------------------------------------------
	AtlasPacker* gkMovableMovie::getPacker() const
	{
		return mFactory->getPacker();
	}
	//-----------------------------------------------------------------------
	Instance* gkMovableMovie::getInstance(Resource* resource)
	{
		if(!resource) return NULL;

		if(resource->getType() == Button::TYPE)
		{
			Button* button = new Button();
			button->_setParent(this);
			button->initFromResource(resource);
			return button;
		}
		else if(resource->getType() == MovieClip::TYPE)
		{
			MovieClip* movieclip = new MovieClip();
			movieclip->_setParent(this);
			movieclip->initFromResource(resource);
			//std::cout << "Created object: " << resource->getID() << std::endl;
			return movieclip;
		}

		if(resource->getType() == Shape::TYPE)
		{
			ShapeResource* shape = static_cast<ShapeResource*>(resource);
			if(!mFactory->getPacker()->containsElement((uint)shape))
			{
				mFactory->getPacker()->addElement(new ShapeAtlasElement(shape));
				mFactory->getPacker()->packAtlas();
				mFactory->getPacker()->renderAtlas();
			}
		}

#ifndef NO_INST_POOLING
		Instance* instance = mFactory->getInstancePool()->pop(resource->getType());

		// instance received from instance pool
		if(instance)
		{
			instance->_setParent(this);
			instance->initFromResource(resource);
		}
		// no instance available, create a new one
		else
#else
		Instance* instance;
#endif
		{
			if(!mFactory)
			{
				VTX_WARN("MovieFactory not available");
				return NULL;
			}

			InstanceFactory* factory = mFactory->getFactory(resource->getType());
			if(factory)
			{
				instance = factory->createObject();
				instance->_setParent(this);
				instance->initFromResource(resource);

				if(instance->getType() == EditText::TYPE)
				{
					gkMovableEditText* edit_text = static_cast<gkMovableEditText*>(instance);
					//edit_text->setAtlasList(mPacker->getResultList());
					//edit_text->setPacker(mPacker);
					mFactory->getPacker()->addListener(edit_text);
				}
				if(instance->getType() == Shape::TYPE)
				{
					gkMovableShape* shape = static_cast<gkMovableShape*>(instance);
					mFactory->getPacker()->addListener(shape);
				}

				//VTX_LOG("\"%s\": CREATED %s with id %s, using Factory %s", 
				//	movie->getName().c_str(), inst->getType().c_str(), id.c_str(), factory->getName().c_str());
			}
		}

		if(!instance)
		{
			VTX_EXCEPT("No suitable factory for the resource type \"%s\" was found, the object with the id \"%s\" can not be created!!!", 
				resource->getType().c_str(), resource->getID().c_str());
		}

		gkRenderable* renderable = NULL;

		if(instance->getType() == Shape::TYPE)
			renderable = static_cast<gkMovableShape*>(instance);
		else if(instance->getType() == EditText::TYPE)
			renderable = static_cast<gkMovableEditText*>(instance);
		else if(instance->getType() == StaticText::TYPE)
			renderable = static_cast<gkMovableStaticText*>(instance);

		if(renderable)
		{
			RenderableMap::iterator it = mRenderables.find(renderable);
			if(it == mRenderables.end())
			{
				mRenderables.insert(std::make_pair(renderable, renderable));
			}
		}

		return instance;
	}
	//-----------------------------------------------------------------------
	Instance* gkMovableMovie::getInstanceByType(const String& type)
	{
		Instance* instance = mFactory->getInstancePool()->pop(type);

		// instance received from instance pool
		if(instance)
		{
			instance->_setParent(this);
		}
		// no instance available, create a new one
		else
		{
			if(type == Button::TYPE)
			{
				Button* button = new Button();
				button->_setParent(this);
				return button;
			}
			else if(type == MovieClip::TYPE)
			{
				MovieClip* movieclip = new MovieClip();
				movieclip->_setParent(this);
				return movieclip;
			}

			if(!mFactory)
			{
				VTX_WARN("MovieFactory not available");
				return NULL;
			}

			InstanceFactory* factory = mFactory->getFactory(type);
			if(factory)
			{
				instance = factory->createObject();
				instance->_setParent(this);

				//VTX_LOG("\"%s\": CREATED %s with id %s, using Factory %s", 
				//	movie->getName().c_str(), inst->getType().c_str(), id.c_str(), factory->getName().c_str());
			}
		}

		if(!instance)
		{
			VTX_EXCEPT("No suitable factory for the instance type \"%s\" was found, the object with this type can not be created!!!", 
				type.c_str());
		}

		gkRenderable* renderable = NULL;

		if(instance->getType() == Shape::TYPE)
			renderable = static_cast<gkMovableShape*>(instance);
		else if(instance->getType() == EditText::TYPE)
			renderable = static_cast<gkMovableEditText*>(instance);
		else if(instance->getType() == StaticText::TYPE)
			renderable = static_cast<gkMovableStaticText*>(instance);

		if(renderable)
		{
			RenderableMap::iterator it = mRenderables.find(renderable);
			if(it == mRenderables.end())
			{
				mRenderables.insert(std::make_pair(renderable, renderable));
			}
		}

		return instance;
	}
	//-----------------------------------------------------------------------
	void gkMovableMovie::releaseInstance(Instance* instance)
	{
		gkRenderable* renderable = NULL;

		if(instance->getType() == Shape::TYPE)
			renderable = static_cast<gkMovableShape*>(instance);
		else if(instance->getType() == EditText::TYPE)
			renderable = static_cast<gkMovableEditText*>(instance);
		else if(instance->getType() == StaticText::TYPE)
			renderable = static_cast<gkMovableStaticText*>(instance);

		if(renderable)
		{
			RenderableMap::iterator it = mRenderables.find(renderable);
			if(it != mRenderables.end())
			{
				mRenderables.erase(it);
			}
		}

		if(instance->getType() == MovieClip::TYPE || instance->getType() == Button::TYPE)
		{
			DisplayObjectContainer* cont = static_cast<DisplayObjectContainer*>(instance);
			cont->clearLayers();

			delete instance;
			return;
		}

#ifndef NO_INST_POOLING
		mFactory->getInstancePool()->push(instance);
		instance->_setParent(NULL);
#else
		delete instance;
#endif

		//vtx::Movie::releaseInstance(instance);
	}
	//-----------------------------------------------------------------------
	const gkStdString& gkMovableMovie::getMovableType() const
	{
		static gkStdString temp = _T("swfMovie");
		return temp;
	}

	//-----------------------------------------------------------------------
	void gkMovableMovie::_updateRenderSequence(IRenderSequence* queue)
	{
		RenderableMap::iterator it = mRenderables.begin();
		RenderableMap::iterator end = mRenderables.end();

		while(it != end)
		{
			queue->addToRenderSequence(it->second);
			++it;
		}
	}
	//-----------------------------------------------------------------------



		//-----------------------------------------------------------------------
		gkMovableMovieFactory::gkMovableMovieFactory()
		{
			InstanceManager* inst_mgr = InstanceManager::getSingletonPtr();

			mFactories[EditText::TYPE] = inst_mgr->getFactory("gkMovableEditText");
			mFactories[Shape::TYPE] = inst_mgr->getFactory("gkMovableShape");
			mFactories[StaticText::TYPE] = inst_mgr->getFactory("gkMovableStaticText");

			TextureFactory* texture_factory = inst_mgr->textures()->getFactory("gkVtxTexture");

			mPacker = new AtlasPacker(texture_factory);
			mPool = new InstancePool();
		}
		//-----------------------------------------------------------------------
		gkMovableMovieFactory::~gkMovableMovieFactory()
		{
			delete mPool;
			mPool = NULL;

			delete mPacker;
			mPacker = NULL;
		}
		//-----------------------------------------------------------------------
		const String& gkMovableMovieFactory::getName() const
		{
			static String name = "gkMovableMovie";
			return name;
		}
		//-----------------------------------------------------------------------
		Movie* gkMovableMovieFactory::createObject(String name)
		{
			gkMovableMovie* movie = new gkMovableMovie(name, this);
			movie->addListener(this);
			return movie; 
		}
		//-----------------------------------------------------------------------
		void gkMovableMovieFactory::destroyObject(Movie* instance)
		{
			delete static_cast<gkMovableMovie*>(instance);
			instance = NULL;
		}
		//-----------------------------------------------------------------------
		AtlasPacker* gkMovableMovieFactory::getPacker() const
		{
			return mPacker;
		}
		//-----------------------------------------------------------------------
		InstancePool* gkMovableMovieFactory::getInstancePool() const
		{
			return mPool;
		}
		//-----------------------------------------------------------------------
		bool gkMovableMovieFactory::loadingCompleted(Movie* movie)
		{
			const File* file = movie->getFile();
			std::cout << "packing file: " << file->getFilename() << std::endl;

			const ResourceList& shape_list = file->getResourcesByType("Shape");
			ResourceList::const_iterator shape_it = shape_list.begin();
			ResourceList::const_iterator shape_end = shape_list.end();

			// add shapes
			while(shape_it != shape_end)
			{
				ShapeResource* shape = static_cast<ShapeResource*>(*shape_it);
				if(shape)
				{
					mPacker->addElement(new ShapeAtlasElement(shape));
				}

				++shape_it;
			}

			const ResourceList& font_list = file->getResourcesByType("Font");
			ResourceList::const_iterator font_it = font_list.begin();
			ResourceList::const_iterator font_end = font_list.end();

			// add glyphs
			while(font_it != font_end)
			{
				FontResource* font = static_cast<FontResource*>(*font_it);
				if(font)
				{
					const FontResource::GlyphList& glyphs = font->getGlyphList();
					FontResource::GlyphList::const_iterator glyph_it = glyphs.begin();
					FontResource::GlyphList::const_iterator glyph_end = glyphs.end();

					while(glyph_it != glyph_end)
					{
						mPacker->addElement(new GlyphAtlasElement(*glyph_it));
						++glyph_it;
					}
				}

				++font_it;
			}

			mPacker->packAtlas();
			mPacker->renderAtlas();
			// stop listening to this movie
			return true;
		}
		//-----------------------------------------------------------------------
		MovieDebugger* gkMovableMovieFactory::_newDebugger( Movie* movie )
		{
			return NULL;
		}
		//-----------------------------------------------------------------------
}
