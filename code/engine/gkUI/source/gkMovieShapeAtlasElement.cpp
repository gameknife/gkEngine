#include "UIStableHeader.h"
#include "gkMovieShapeAtlasElement.h"

#include "vtxAtlasNode.h"
#include "vtxRasterizer.h"
#include "vtxRect.h"
#include "vtxShapeResource.h"
#include "vtxSubshapeResource.h"

namespace vtx {
	//-----------------------------------------------------------------------
	ShapeAtlasElement::ShapeAtlasElement(ShapeResource* shape) 
		: mShape(shape)
	{

	}
	//-----------------------------------------------------------------------
	const uint ShapeAtlasElement::getPackID() const
	{
		return (uint)mShape;
	}
	//-----------------------------------------------------------------------
	const uint ShapeAtlasElement::getPackableWidth()
	{
		return mShape->getMaximumWidth_PoT();
	}
	//-----------------------------------------------------------------------
	const uint ShapeAtlasElement::getPackableHeight()
	{
		return mShape->getMaximumHeight_PoT();
	}
	//-----------------------------------------------------------------------
	void ShapeAtlasElement::paintToNode(AtlasNode* node, Rasterizer* rasterizer)
	{
		Rect rect = node->getRect();
		rect.contract(1);

		Vector2 offset(-mShape->getBoundingBox().getMinX(), -mShape->getBoundingBox().getMinY());
		Vector2 scale(rect.w()/mShape->getWidth(), rect.h()/mShape->getHeight());

		rasterizer->startPaint(rect, offset, scale, node->getTexture());

		const ShapeResource::SubshapeList& subshapes = mShape->getSubshapeList();
		ShapeResource::SubshapeList::const_iterator subshape_it = subshapes.begin();
		ShapeResource::SubshapeList::const_iterator subshape_end = subshapes.end();

		while(subshape_it != subshape_end)
		{
			SubshapeResource* subshape = (*subshape_it);
			MaterialResource* material = subshape->getMaterial();

			rasterizer->setMaterialFill(material);
			rasterizer->drawShapeElements(subshape->getElementList());

			++subshape_it;
		}

		rasterizer->finishPaint();
	}
	//-----------------------------------------------------------------------
}