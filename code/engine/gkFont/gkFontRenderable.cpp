#include "gkFontRenderable.h"
#include "IRenderer.h"
#include "IRenderSequence.h"
#include "gkRenderOperation.h"


gkFontRenderable::gkFontRenderable(void)
{
	m_aabb.Reset();
	m_vbCharacterCacheSize = 500;

	m_CharacterProcessedLastFrame = 0;
	m_CharacterProcessedThisFrame = 0;
}

gkFontRenderable::~gkFontRenderable(void)
{
}

void gkFontRenderable::setMaterialName( const gkStdString& matName )
{
}

const gkStdString& gkFontRenderable::getMaterialName() const
{
	return _T("FontMaterial");
}

bool gkFontRenderable::getSkinnedMatrix( Matrix44A** pMatrixs, uint32& size )
{
	return false;
}

float gkFontRenderable::getSquaredViewDepth( const ICamera* cam ) const
{
	return 0;
}

gkMeshPtr& gkFontRenderable::getMesh()
{
	return m_mesh;
}

void gkFontRenderable::setParent( IGameObjectRenderLayer* pParent )
{
	
}

bool gkFontRenderable::castShadow()
{
	return false;
}

void gkFontRenderable::enableShadow( bool enable )
{
	
}

void gkFontRenderable::RT_Prepare()
{
}

void gkFontRenderable::getWorldTransforms( Matrix44* mat) const
{
	mat->SetIdentity(); 
}

IMaterial* gkFontRenderable::getMaterial()
{
	return m_material.getPointer();
}

AABB& gkFontRenderable::getAABB()
{
	return m_aabb;
}

void gkFontRenderable::getRenderOperation( gkRenderOperation& op )
{
	m_mesh->getRenderOperation( op );

	op.indexCount = m_CharacterProcessedLastFrame * 6;
}

void gkFontRenderable::resizeHwBuffer()
{
	m_mesh->getVertexBuffer()->resizeDiscard( m_vbCharacterCacheSize * 4 );
	m_mesh->getIndexBuffer()->resizeDiscard( m_vbCharacterCacheSize * 2 * 3 );

	gkIndexBuffer* ib = m_mesh->getIndexBuffer();
	ib->Clear();

	for (uint32 i=0; i < m_vbCharacterCacheSize; ++i)
	{
		ib->push_back(i * 4 + 0);
		ib->push_back(i * 4 + 1);
		ib->push_back(i * 4 + 2);

		ib->push_back(i * 4 + 1);
		ib->push_back(i * 4 + 3);
		ib->push_back(i * 4 + 2);
	}
}

void gkFontRenderable::flush()
{
	// commit
	if (m_CharacterProcessedThisFrame > 0)
	{
		// add
		IRenderSequence* rs = gEnv->pRenderer->RT_GetRenderSequence();
		if (rs)
		{
			rs->addToRenderSequence( this, RENDER_LAYER_HUDUI );
		}
	}

	// clean
	m_CharacterProcessedLastFrame = m_CharacterProcessedThisFrame;
	m_CharacterProcessedThisFrame = 0;
}

void gkFontRenderable::addCharacter( gkFTCharInfo& info, const Vec3i& tmpPos, const ColorB& color )
{

	GKVL_P3T2U4* VERTEX_START = (GKVL_P3T2U4*)(m_mesh->getVertexBuffer()->data);

	GKVL_P3T2U4* CHARACTER_LT = VERTEX_START + (m_CharacterProcessedThisFrame * 4);
	GKVL_P3T2U4* CHARACTER_RT = CHARACTER_LT + 1;
	GKVL_P3T2U4* CHARACTER_LB = CHARACTER_LT + 2;
	GKVL_P3T2U4* CHARACTER_RB = CHARACTER_LT + 3;


	// 安排vb

	// 这里的顶点坐标，必须是整数，否则会有渲染错误！
	Vec3i bearing(info.bearingX, -info.bearingY, 0.0f);
	Vec3i offsetingPos = tmpPos + bearing;

	float x1 = info.texcoordInfo.x + info.texcoordInfo.z;
	float y1 = info.texcoordInfo.y + info.texcoordInfo.w;

	CHARACTER_LT->position = gEnv->pRenderer->ScreenPosToViewportPos(offsetingPos);
	CHARACTER_LT->texcoord = Vec2(info.texcoordInfo.x, info.texcoordInfo.y);
	CHARACTER_LT->color = color.pack_argb8888();

	CHARACTER_RT->position = gEnv->pRenderer->ScreenPosToViewportPos(offsetingPos + Vec3(info.width, 0, 0));
	CHARACTER_RT->texcoord = Vec2(x1, info.texcoordInfo.y);
	CHARACTER_RT->color = color.pack_argb8888();;

	CHARACTER_LB->position = gEnv->pRenderer->ScreenPosToViewportPos(offsetingPos + Vec3(0, info.height, 0));
	CHARACTER_LB->texcoord = Vec2(info.texcoordInfo.x, y1);
	CHARACTER_LB->color = color.pack_argb8888();;

	CHARACTER_RB->position = gEnv->pRenderer->ScreenPosToViewportPos(offsetingPos + Vec3(info.width, info.height, 0));
	CHARACTER_RB->texcoord = Vec2(x1, y1);
	CHARACTER_RB->color = color.pack_argb8888();;

	m_CharacterProcessedThisFrame++;

	if (m_CharacterProcessedThisFrame >= m_vbCharacterCacheSize)
	{
		m_vbCharacterCacheSize = m_vbCharacterCacheSize << 1;
		resizeHwBuffer();
	}
}

void gkFontRenderable::RP_Prepare()
{
	m_mesh->UpdateHwBuffer();
}

