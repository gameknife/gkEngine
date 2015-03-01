#include "UIStableHeader.h"

#include "gkMovieMovableStaticText.h"
#include "gkMovieMovable.h"
#include "vtxStaticTextResource.h"

namespace vtx
{
		//-----------------------------------------------------------------------
		gkMovableStaticText::gkMovableStaticText() 
			: gkMovableTextBase(this), 
			mResource(NULL)
		{
			_createBuffers();
		}
		//-----------------------------------------------------------------------
		gkMovableStaticText::~gkMovableStaticText()
		{

		}
		//-----------------------------------------------------------------------
		void gkMovableStaticText::_setParent(Movie* parent)
		{
			StaticText::_setParent(parent);
			mParentMovable = static_cast<gkMovableMovie*>(parent);

			if(mParentMovable)
			{
				mPacker = mParentMovable->getPacker();

				if(mPacker)
				{
					mPacker->addListener(this);
				}
			}
			else
			{
				if(mPacker)
				{
					mPacker->removeListener(this);
				}
			}
		}
		//-----------------------------------------------------------------------
		void gkMovableStaticText::initFromResource(Resource* resource)
		{
			StaticText::initFromResource(resource);

			if(resource->getType() == "StaticText")
			{
				mResource = static_cast<StaticTextResource*>(resource);

				uint glyph_count = 0;
				const GlyphStripList& glyph_strips = mResource->getGlyphStrips();
				GlyphStripList::const_iterator it = glyph_strips.begin();
				GlyphStripList::const_iterator end = glyph_strips.end();
				while(it != end)
				{
					glyph_count += (*it).glyphs.size();
					++it;
				}

				_resizeBuffers(glyph_count);

				if(mPacker && mParentMovie && mResource)
				{
					_updateVertexBuffer(mResource->getGlyphStrips(), mPacker->getResultList(), mParentMovie->getFile());
				}
			}
		}
		//-----------------------------------------------------------------------
		void gkMovableStaticText::_update(const float& delta_time)
		{
			StaticText::_update(delta_time);

			const Matrix& mat = getWorldMatrix();

			mWorldMatrix = D3DXMATRIX(
				mat.m[0][0], -mat.m[0][1], 0,  mat.m[0][2], 
				-mat.m[1][0], mat.m[1][1], 0, -mat.m[1][2], 
				0,		  0,		   1,				0,
				0,		  0,		   0,				1);

			D3DXMatrixTranspose(&mWorldMatrix, &mWorldMatrix);
		}
		//-----------------------------------------------------------------------
		void gkMovableStaticText::packed(const AtlasPacker::PackResultList& pack_result)
		{
			if(mPacker && mParentMovie && mResource)
			{
				_updateVertexBuffer(mResource->getGlyphStrips(), mPacker->getResultList(), mParentMovie->getFile());
			}
		}
		//-----------------------------------------------------------------------
}
