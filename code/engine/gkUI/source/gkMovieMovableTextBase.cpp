#include "UIStableHeader.h"

#include "gkMovieMovableTextBase.h"

#include "gkVtxTexture.h"

#include "vtxAtlasNode.h"
#include "vtxFile.h"
#include "vtxFontResource.h"
#include "vtxGlyphResource.h"

namespace vtx
{
		//-----------------------------------------------------------------------
		gkMovableTextBase::gkMovableTextBase(DisplayObject* display_object) 
			: gkMovableInstanceBase(display_object)
		{

		}
		//-----------------------------------------------------------------------
		gkMovableTextBase::~gkMovableTextBase()
		{

		}
		//-----------------------------------------------------------------------
		void gkMovableTextBase::_updateVertexBuffer(const GlyphStripList& glyph_strips, const AtlasPacker::PackResultList& atlas_list, File* file)
		{
			//File* file = mResource->getFile();
			gkVtxTexture* texture = NULL;

			GlyphStrip::GlyphList::const_iterator glyph_it;
			GlyphStrip::GlyphList::const_iterator glyph_end;

			// update the buffer
			_lock();

			// x-Position of glyphs
			float glyph_x = 0.0f;

			// iterate GlyphStrips
			GlyphStripList::const_iterator it = glyph_strips.begin();
			GlyphStripList::const_iterator end = glyph_strips.end();
			while(it != end)
			{
				const GlyphStrip& glyph_strip = *it;
				FontResource* font = static_cast<FontResource*>(file->getResource(glyph_strip.fontid));

				// jump to a new line
				if(glyph_strip.newline)
				{
					glyph_x = 0.0f;
				}

				glyph_x += glyph_strip.x;

				if(font)
				{
					glyph_it = glyph_strip.glyphs.begin();
					glyph_end = glyph_strip.glyphs.end();

					// iterate individual Glyphs
					while(glyph_it != glyph_end)
					{
						const GlyphStrip::Glyph& glyph = *glyph_it;

						GlyphResource* glyph_res = font->getGlyphByIndex(glyph.index);
						if(glyph_res)
						{
							AtlasPacker::PackResultList::const_iterator atlas_it = atlas_list.find((uint)glyph_res);
							if(atlas_it != atlas_list.end())
							{
								const AtlasPacker::PackResult& atlas_quad = atlas_it->second;

								if(!texture)
								{
									texture = static_cast<gkVtxTexture*>(atlas_quad.texture);
								}

								const float size = glyph_strip.size * 0.05f;
								const BoundingBox& bb = glyph_res->getBoundingBox();
								const RectF& uv_rect = atlas_quad.node->getRect().contractedCopy(1).relativeTo(
									atlas_quad.texture->getWidth(), atlas_quad.texture->getHeight());

								const RectF pos_rect(
									bb.getMinX() * size + glyph_x, // left
									-bb.getMinY() * size - glyph_strip.y, // top
									bb.getMaxX() * size + glyph_x, // right
									-bb.getMaxY() * size - glyph_strip.y // bottom
									);

								// TOP-LEFT
								_addVertex(
									D3DXVECTOR2(pos_rect.left, pos_rect.top), 
									D3DXVECTOR2(uv_rect.left, uv_rect.top), 
									VTXCOLOR_TO_D3DXCOLOR(glyph_strip.color), VTXCOLOR_TO_D3DXCOLOR(glyph_strip.color));

								// BOTTOM-LEFT
								_addVertex(
									D3DXVECTOR2(pos_rect.left, pos_rect.bottom), 
									D3DXVECTOR2(uv_rect.left, uv_rect.bottom), 
									VTXCOLOR_TO_D3DXCOLOR(glyph_strip.color), VTXCOLOR_TO_D3DXCOLOR(glyph_strip.color));

								// BOTTOM-RIGHT
								_addVertex(
									D3DXVECTOR2(pos_rect.right, pos_rect.bottom), 
									D3DXVECTOR2(uv_rect.right, uv_rect.bottom), 
									VTXCOLOR_TO_D3DXCOLOR(glyph_strip.color), VTXCOLOR_TO_D3DXCOLOR(glyph_strip.color));

								// TOP-RIGHT
								_addVertex(
									D3DXVECTOR2(pos_rect.right, pos_rect.top), 
									D3DXVECTOR2(uv_rect.right, uv_rect.top), 
									VTXCOLOR_TO_D3DXCOLOR(glyph_strip.color), VTXCOLOR_TO_D3DXCOLOR(glyph_strip.color));

							} // if(atlas_list_result)

						} // if(glyph_res)

						glyph_x += glyph.x_advance;

						++glyph_it;

					} // while(glyphs)

				} // if(font)

				++it;

			} // while(glyph_strips)

			_unlock();

			if(texture)
			{
				mMaterial = texture->getMaterial();
			}
		}
		//-----------------------------------------------------------------------
}