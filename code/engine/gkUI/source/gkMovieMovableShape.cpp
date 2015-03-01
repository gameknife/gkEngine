#include "UIStableHeader.h"

#include "gkMovieMovableShape.h"
#include "gkMovieMovable.h"
#include "gkVtxTexture.h"

#include "vtxAtlasNode.h"
#include "vtxLogManager.h"
#include "vtxResource.h"
#include "vtxShapeResource.h"
#include "vtxDisplayObject.h"
#include "vtxDisplayObjectContainer.h"

#define Z_INTERVAL 100.0f

namespace vtx
{
	//-----------------------------------------------------------------------
	gkMovableShape::gkMovableShape() 
		: gkMovableInstanceBase(this), 
		mLayer(0.0f)
	{
		_createBuffers();
		_resizeBuffers(1);
	}
	//-----------------------------------------------------------------------
	gkMovableShape::~gkMovableShape()
	{

	}
	//-----------------------------------------------------------------------
	void gkMovableShape::_setParent(Movie* parent)
	{
		Shape::_setParent(parent);
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
	void gkMovableShape::initFromResource(Resource* resource)
	{
		Shape::initFromResource(resource);

		if(mPacker)
		{
			AtlasPacker::PackResultList list = mPacker->getResultList();
			AtlasPacker::PackResultList::iterator it = list.find((uint)resource);
			if(it != list.end())
			{
				setAtlasQuad(it->second);
			}
			else
			{
				VTX_EXCEPT("blah");
			}
		}
	}
	//-----------------------------------------------------------------------
	void gkMovableShape::_update(const float& delta_time)
	{
		Shape::_update(delta_time);

		const Matrix& mat = getWorldMatrix();

		mWorldMatrix = D3DXMATRIX(
			mat.m[0][0], -mat.m[0][1], 0,  mat.m[0][2], 
			-mat.m[1][0], mat.m[1][1], 0, -mat.m[1][2], 
			0,		  0,		   1,				0,
			0,		  0,		   0,				1);

		D3DXMatrixTranspose(&mWorldMatrix, &mWorldMatrix);

		mMULcolor = mTransform.getColor().mul;
		mADDcolor = mTransform.getColor().add;

		if (getParentContainer() && !(getParentContainer()->getVisible()))
		{
			mMULcolor.a = 0;
			mADDcolor.a = 0;
		}
	}
	//-----------------------------------------------------------------------
	void gkMovableShape::setAtlasQuad(const AtlasPacker::PackResult& quad)
	{
		mMaterial = static_cast<gkVtxTexture*>(quad.texture)->getMaterial();
		RectF tex_coords = quad.node->getRect().contractedCopy(1).relativeTo(
			quad.texture->getWidth(), quad.texture->getHeight());

		const BoundingBox& bb = getBoundingBox();

		if (getParentContainer() && !(getParentContainer()->getVisible()))
		{
			mMULcolor.a = 0;
			mADDcolor.a = 0;
		}

		_lock();

		// TOP-LEFT
		_addVertex(
			D3DXVECTOR2(bb.getMinX(), -bb.getMinY()), 
			D3DXVECTOR2(tex_coords.left, tex_coords.top), 
			//D3DXVECTOR2(0, 0), 
			VTXCOLOR_TO_D3DXCOLOR(mMULcolor), VTXCOLOR_TO_D3DXCOLOR(mADDcolor));

		// BOTTOM-LEFT
		_addVertex(
			D3DXVECTOR2(bb.getMinX(), -bb.getMaxY()), 
			D3DXVECTOR2(tex_coords.left, tex_coords.bottom), 
			//D3DXVECTOR2(0, 1), 
			VTXCOLOR_TO_D3DXCOLOR(mMULcolor), VTXCOLOR_TO_D3DXCOLOR(mADDcolor));

		// BOTTOM-RIGHT
		_addVertex(
			D3DXVECTOR2(bb.getMaxX(), -bb.getMaxY()), 
			D3DXVECTOR2(tex_coords.right, tex_coords.bottom), 
			//D3DXVECTOR2(1, 1), 
			VTXCOLOR_TO_D3DXCOLOR(mMULcolor), VTXCOLOR_TO_D3DXCOLOR(mADDcolor));

		// TOP-RIGHT
		_addVertex(
			D3DXVECTOR2(bb.getMaxX(), -bb.getMinY()), 
			D3DXVECTOR2(tex_coords.right, tex_coords.top), 
			//D3DXVECTOR2(1, 0), 
			VTXCOLOR_TO_D3DXCOLOR(mMULcolor), VTXCOLOR_TO_D3DXCOLOR(mADDcolor));

		_unlock();
	}
	//-----------------------------------------------------------------------
	void gkMovableShape::packed(const AtlasPacker::PackResultList& pack_result)
	{
		AtlasPacker::PackResultList::const_iterator it = pack_result.find((uint)mShapeResource);
		if(it != pack_result.end())
		{
			setAtlasQuad(it->second);
		}
	}
	//-----------------------------------------------------------------------
}
