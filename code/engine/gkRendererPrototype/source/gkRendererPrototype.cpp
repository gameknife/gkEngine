#include "StableHeader.h"
#include "gkRendererPrototype.h"
#include "gkAuxRenderer.h"

IResourceManager*	gkRendererPrototype::m_pTextureManager = NULL;
IResourceManager*	gkRendererPrototype::m_pMeshManager = NULL;
IResourceManager*	gkRendererPrototype::m_pMaterialManager = NULL;
IResourceManager*	gkRendererPrototype::m_pShaderManager = NULL;

gkAuxRenderer*		gkRendererPrototype::m_pAuxRenderer = 0;

gkRendererPrototype::gkRendererPrototype( void )
{
	m_pAuxRenderer = new gkAuxRenderer;
}

gkRendererPrototype::~gkRendererPrototype( void )
{
	SAFE_DELETE(m_pAuxRenderer)
}

void gkRendererPrototype::RT_CleanRenderSequence()
{
	throw std::exception("The method or operation is not implemented.");
}

void gkRendererPrototype::RT_SkipOneFrame()
{
	throw std::exception("The method or operation is not implemented.");
}

void gkRendererPrototype::FX_ColorGradingTo( gkTexturePtr& pCch, float fAmount )
{
	throw std::exception("The method or operation is not implemented.");
}

void gkRendererPrototype::RP_StretchRefraction()
{
	throw std::exception("The method or operation is not implemented.");
}
