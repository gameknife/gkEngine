#include "UIStableHeader.h"

#include "gkVtxGlyphAtlasElement.h"
#include "vtxAtlasNode.h"
#include "vtxColor.h"
#include "vtxGlyphResource.h"
#include "vtxMathHelper.h"
#include "vtxRasterizer.h"
#include "vtxRect.h"

namespace vtx {
	//-----------------------------------------------------------------------
	GlyphAtlasElement::GlyphAtlasElement(GlyphResource* glyph) 
		: mGlyph(glyph)
	{

	}
	//-----------------------------------------------------------------------
	const uint GlyphAtlasElement::getPackID() const
	{
		return (uint)mGlyph;
	}
	//-----------------------------------------------------------------------
	const uint GlyphAtlasElement::getPackableWidth()
	{
		const BoundingBox& bb = mGlyph->getBoundingBox();
		return MathHelper::findClosestPowerOfTwo(bb.getWidth()) * 2;
	}
	//-----------------------------------------------------------------------
	const uint GlyphAtlasElement::getPackableHeight()
	{
		const BoundingBox& bb = mGlyph->getBoundingBox();
		return MathHelper::findClosestPowerOfTwo(bb.getHeight()) * 2;
	}
	//-----------------------------------------------------------------------
	void GlyphAtlasElement::paintToNode(AtlasNode* node, Rasterizer* rasterizer)
	{
		Rect rect = node->getRect();
		rect.contract(1);

		Vector2 offset(-mGlyph->getBoundingBox().getMinX(), -mGlyph->getBoundingBox().getMinY());
		Vector2 scale(rect.w()/mGlyph->getBoundingBox().getWidth(), rect.h()/mGlyph->getBoundingBox().getHeight());

		rasterizer->startPaint(rect, offset, scale, node->getTexture());
		rasterizer->setColorFill(Color());
		rasterizer->drawShapeElements(mGlyph->getElementList());
		rasterizer->finishPaint();
	}
	//-----------------------------------------------------------------------
}
