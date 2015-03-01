#include "UIStableHeader.h"

#include "gkMovieMovableEditText.h"
#include "gkMovieMovable.h"

#include "vtxEditTextResource.h"

namespace vtx
{
		//-----------------------------------------------------------------------
		gkMovableEditText::gkMovableEditText() 
			: gkMovableTextBase(this)
		{
			_createBuffers();
		}
		//-----------------------------------------------------------------------
		gkMovableEditText::~gkMovableEditText()
		{

		}
		//-----------------------------------------------------------------------
		void gkMovableEditText::_setParent(Movie* parent)
		{
			EditText::_setParent(parent);
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
		void gkMovableEditText::initFromResource(Resource* resource)
		{
			EditText::initFromResource(resource);

			if(resource->getType() == "EditText")
			{
				EditTextResource* res = static_cast<EditTextResource*>(resource);
				setHtmlText(res->getInitialText());
			}
		}
		//-----------------------------------------------------------------------
		void gkMovableEditText::_update(const float& delta_time)
		{
			EditText::_update(delta_time);

			const Matrix& mat = getWorldMatrix();

			mWorldMatrix = D3DXMATRIX(
				mat.m[0][0], -mat.m[0][1], 0,  mat.m[0][2], 
				-mat.m[1][0], mat.m[1][1], 0, -mat.m[1][2], 
				0,		  0,		   1,				0,
				0,		  0,		   0,				1);

			D3DXMatrixTranspose(&mWorldMatrix, &mWorldMatrix);
		}
		//-----------------------------------------------------------------------
		void gkMovableEditText::packed(const AtlasPacker::PackResultList& pack_result)
		{
			_updateGraphics();
		}
		//-----------------------------------------------------------------------
		void gkMovableEditText::_updateGraphics()
		{
			_resizeBuffers(mGlyphCount);
			if(mParentMovie && mPacker)
			{
				_updateVertexBuffer(mGlyphStrips, mPacker->getResultList(), mParentMovie->getFile());
			}
		}
		//-----------------------------------------------------------------------
	
}
