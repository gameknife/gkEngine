#include "RendererD3D9StableHeader.h"
#include "gkRenderSequence.h"
#include "gkRenderable.h"
#include "gkRenderLayer.h"
#include "IAuxRenderer.h"

#include "gkMemoryLeakDetecter.h"



gkRenderSequence::gkRenderSequence( void ):m_bIsReady(false)
{
	// 创建基本渲染层
	m_mapRenderLayer.insert(
		gkRenderLayerMap::value_type(
		RENDER_LAYER_OPAQUE,
		gkRenderLayer()
		)
		);

	// 设置默认渲染层
	m_yDefaultLayer = RENDER_LAYER_OPAQUE;

	m_MainCam = NULL;
	m_ShadowCamCascade0 = NULL;
	m_ShadowCamCascade1 = NULL;
	m_ShadowCamCascade2 = NULL;

}

gkRenderSequence::~gkRenderSequence( void )
{	
	// Clear the queues
	gkRenderLayerMap::iterator i, iend;
	i = m_mapRenderLayer.begin();
	iend = m_mapRenderLayer.end();
	for (; i != iend; ++i)
	{
		i->second.clear();
		// 这时才真正的删除这个层
		//SAFE_DELETE( i->second );
	}
	m_mapRenderLayer.clear();
	m_yDefaultLayer = RENDER_LAYER_OPAQUE;
}

gkRenderLayer* gkRenderSequence::getRenderLayer( BYTE yId )
{
	// 在容器中寻找渲染层
	gkRenderLayerMap::iterator groupIt;
	gkRenderLayer* pLayer;

	groupIt = m_mapRenderLayer.find(yId);
	if (groupIt == m_mapRenderLayer.end())
	{
		// 没有找到，创建一层
		//pLayer = new gkRenderLayer();
		m_mapRenderLayer.insert(gkRenderLayerMap::value_type(yId, gkRenderLayer()));

		groupIt = m_mapRenderLayer.find(yId);

	}
	//else
	{
		pLayer = &(groupIt->second);
	}

	return pLayer;
}

void gkRenderSequence::addToRenderSequence( gkRenderable* pRend, BYTE yId )
{
	// GetLayer
	gkRenderLayer* pLayer = getRenderLayer(yId);
	gkRenderLayer* pCustomShadowLayer0 = getRenderLayer( RENDER_LAYER_CUSTOM_SHADOW_CASCADE0 );
	gkRenderLayer* pCustomShadowLayer1 = getRenderLayer( RENDER_LAYER_CUSTOM_SHADOW_CASCADE1 );
	gkRenderLayer* pCustomShadowLayer2 = getRenderLayer( RENDER_LAYER_CUSTOM_SHADOW_CASCADE2 );

	gkRenderLayer* pCustomReflLayer = getRenderLayer( RENDER_LAYER_CUSTOM_REFL );

	IShader* pShader = NULL;
	
	// Touch Material
	if (pRend->getMaterial())
		pRend->getMaterial()->touch();

	// Check Material & getShader
	if(!pRend->getMaterial())
	{
		GK_ASSERT( 0 );
		gkLogWarning(_T("gkRenderSequence::1 Object has no material, can not Render it."));
		return;
	}
	else
		pShader = pRend->getMaterial()->getShader().getPointer();


	// if shadow list generation

	if (m_yCacheMode == eRFMode_ShadowCas0 ||
		m_yCacheMode == eRFMode_ShadowCas1 ||
		m_yCacheMode == eRFMode_ShadowCas2 )
	{
		uint32 tmp = 0;
		if ( yId >= RENDER_LAYER_OPAQUE && yId <= RENDER_LAYER_TERRIAN && pRend->castShadow() && pRend->getMaterial()->getCastShadow())
		{
			switch(m_yCacheMode)
			{
			case eRFMode_ShadowCas0:

				if (!pShader->FX_GetTechnique(eSIT_ShadowPass) && pRend->getSkinnedMatrix(0, tmp))
				{
					m_lstShadowCascade0Skinned.m_vecRenderable.push_back(pRend);
				}
				else if (!pShader->FX_GetTechnique(eSIT_ShadowPass))
				{
					m_lstShadowCascade0.m_vecRenderable.push_back(pRend);
				}
				else
				{
					// That is very dangerous, may cause renderable repeat!!!
					pCustomShadowLayer0->addRenderable(pRend, pShader);
				}

				break;
			case eRFMode_ShadowCas1:

				if (!pShader->FX_GetTechnique(eSIT_ShadowPass) && pRend->getSkinnedMatrix(0, tmp))
				{
					m_lstShadowCascade1Skinned.m_vecRenderable.push_back(pRend);
				}
				else if (!pShader->FX_GetTechnique(eSIT_ShadowPass))
				{
					m_lstShadowCascade1.m_vecRenderable.push_back(pRend);
				}
				else
				{
					// That is very dangerous, may cause renderable repeat!!!
					pCustomShadowLayer1->addRenderable(pRend, pShader);
				}
				break;

			case eRFMode_ShadowCas2:
				if (!pShader->FX_GetTechnique(eSIT_ShadowPass))
				{
					m_lstShadowCascade2.m_vecRenderable.push_back(pRend);
				}
				else
				{
					// That is very dangerous, may cause renderable repeat!!!
					pCustomShadowLayer2->addRenderable(pRend, pShader);
				}
				break;
			}
		}
		return;
	}



	// in general list generation
	// if Opaque, add to ShadowGen List
	if ( yId >= RENDER_LAYER_OPAQUE && yId <= RENDER_LAYER_TERRIAN)
	{
		pLayer->addRenderable(pRend, pShader);

		uint32 tmp;
		if (!pShader->FX_GetTechnique(eSIT_Zpass_DL) && pRend->getSkinnedMatrix(0,tmp))
		{
			//m_lstZprepassSkinned.m_vecRenderable.push_back(pRend);
			gkLogWarning( _T("Shader [%s] do not have skinned zpass method, skiped."), pShader->getShortName().c_str() );
		}
		else if (!pShader->FX_GetTechnique(eSIT_Zpass_DL))
		{
			//m_lstZprepass.m_vecRenderable.push_back(pRend);
			gkLogWarning( _T("Shader [%s] do not have zpass method, skiped."), pShader->getShortName().c_str() );
		}
		else
		{
			//gkLogMessage(_T("zpass custom passed..."));
		}
	}
	// if Not, Just Add
	else
	{
		pLayer->addRenderable(pRend, pShader);
	}	
}

//-----------------------------------------------------------------------
void gkRenderSequence::addToRenderSequence( gkRenderable* pRend )
{
	// Judge the layer we should use [4/8/2012 Kaiming]

	// Touch Material
	;

	if (pRend->getMaterial())
		pRend->getMaterial()->touch();

	// Check Material & getShader
	if(!pRend->getMaterial())
	{
		GK_ASSERT( 0 );
		gkLogWarning(_T("gkRenderSequence::1 Object has no material, can not Render it."));
		return;
	}

	IMaterial* pMtl = pRend->getMaterial();
	IShader* pShader = pMtl->getShader().getPointer();

	// If Shader Set DefaultLayer
	if (pShader && pShader->getDefaultRenderLayer() != -1)
	{
		addToRenderSequence(pRend, pShader->getDefaultRenderLayer());
		return;
	}

	if( pMtl->getOpacity() == 100)
		addToRenderSequence(pRend, RENDER_LAYER_OPAQUE);
	else
		addToRenderSequence(pRend, RENDER_LAYER_TRANSPARENT);

	// if SSRL add to SSRL layer, using general zpass rendering stradgy to render it
// 	if (pMtl->getSSRL())
// 	{
// 		addToRenderSequence(pRend, RENDER_LAYER_SSRL_OBJECTS);
// 	}
}
//-----------------------------------------------------------------------
void gkRenderSequence::clear()
{
	// Clear the queues
	gkRenderLayerMap::iterator i, iend;
	i = m_mapRenderLayer.begin();
	iend = m_mapRenderLayer.end();
	for (; i != iend; ++i)
	{
		i->second.clear();
	}

	// cache mode backto 0
	m_yCacheMode = eRFMode_General;

	// Clear the Zprepass list [10/2/2011 Kaiming]
	m_lstZprepass.m_vecRenderable.clear();
	m_lstZprepassSkinned.m_vecRenderable.clear();
	m_lstShadowCascade0.m_vecRenderable.clear();
	m_lstShadowCascade0Skinned.m_vecRenderable.clear();
	m_lstShadowCascade1.m_vecRenderable.clear();
	m_lstShadowCascade1Skinned.m_vecRenderable.clear();
	m_lstShadowCascade2.m_vecRenderable.clear();

	//m_lstLightThisFrame.clear();
	m_lstRenderLight.clear();

	m_MainCam = NULL;
	m_ShadowCamCascade0 = NULL;
	m_ShadowCamCascade1 = NULL;
	m_ShadowCamCascade2 = NULL;

	m_bIsReady = false;
}

//-----------------------------------------------------------------------
gkRenderSequence::LayerMapIterator gkRenderSequence::_getLayerIterator( void )
{
	return LayerMapIterator(m_mapRenderLayer.begin(), m_mapRenderLayer.end());
}

//-----------------------------------------------------------------------
gkRenderSequence::ConstLayerMapIterator gkRenderSequence::_getLayerIterator( void ) const
{
	return ConstLayerMapIterator(m_mapRenderLayer.begin(), m_mapRenderLayer.end());
}

void gkRenderSequence::reduceShadowCascadeList()
{
	// the relation is, zprepass < cascade1 < cascade2 [10/27/2011 Kaiming]
	// so first, cut 2 from 1
	std::list<gkRenderable*>::iterator it = m_lstShadowCascade2.m_vecRenderable.begin();
	for(; it != m_lstShadowCascade2.m_vecRenderable.end();)
	{
		bool flag = false;
		std::list<gkRenderable*>::iterator it0 = m_lstShadowCascade1.m_vecRenderable.begin();
		for(; it0 != m_lstShadowCascade1.m_vecRenderable.end(); ++it0)
		{
			if ( (*it0) == (*it))
			{
				// find, erase from list2
				flag = true;
				break;
			}
		}

		if (flag)
		{
			it = m_lstShadowCascade2.m_vecRenderable.erase(it);
		}
		else
		{
			++it;
		}
	}

	// clean from cascade0
	std::list<gkRenderable*>::iterator it1 = m_lstShadowCascade1.m_vecRenderable.begin();
	for(; it1 != m_lstShadowCascade1.m_vecRenderable.end();)
	{
		bool flag = false;
		std::list<gkRenderable*>::iterator it0 = m_lstShadowCascade0.m_vecRenderable.begin();
		for(; it0 != m_lstShadowCascade0.m_vecRenderable.end(); ++it0)
		{
			if ( (*it0) == (*it1))
			{
				// find, erase from list2
				flag = true;
				break;
			}
		}

		if (flag)
		{
			it1 = m_lstShadowCascade1.m_vecRenderable.erase(it1);
		}
		else
		{
			++it1;
		}
	}
}

void gkRenderSequence::setCamera( ICamera* pCam, BYTE yMode )
{
	switch(yMode)
	{
	case eRFMode_General:
		m_MainCam = pCam;
		break;
	case eRFMode_ShadowCas0:
		m_ShadowCamCascade0 = pCam;
		break;
	case eRFMode_ShadowCas1:
		m_ShadowCamCascade1 = pCam;
		break;
	case eRFMode_ShadowCas2:
		m_ShadowCamCascade2 = pCam;
		break;
	}
}

ICamera* gkRenderSequence::getCamera( BYTE yMode )
{
	switch(yMode)
	{
	case eRFMode_General:
		return m_MainCam;
		break;
	case eRFMode_ShadowCas0:
		return m_ShadowCamCascade0;
		break;
	case eRFMode_ShadowCas1:
		return m_ShadowCamCascade1;
		break;
	case eRFMode_ShadowCas2:
		return m_ShadowCamCascade2;
		break;
	}

	return NULL;
}

void gkRenderSequence::addRenderLight( gkRenderLight& pLight )
{

	if (m_yCacheMode == eRFMode_ShadowCas0 ||
		m_yCacheMode == eRFMode_ShadowCas1 ||
		m_yCacheMode == eRFMode_ShadowCas2 )
	{
		return;
	}


	// if general list generation, add
	m_lstRenderLight.push_back(pLight);
}
//////////////////////////////////////////////////////////////////////////
void gkRenderSequence::PrepareRenderSequence()
{
	gkRenderLayerMap::iterator i, iend;
	i = m_mapRenderLayer.begin();
	iend = m_mapRenderLayer.end();
	for (; i != iend; ++i)
	{
		gkRenderLayer::ShaderGroupMapIterator shaderGroupIter = i->second._getShaderGroupIterator();

		while (shaderGroupIter.hasMoreElements())
		{
			gkShaderGroup& pRenderablelistOfShader = shaderGroupIter.getNext();

			gkShaderGroup::gkMaterialGroupRenderableMap::iterator itmat = pRenderablelistOfShader.m_opaqueMaterialMap.begin();
			gkShaderGroup::gkMaterialGroupRenderableMap::iterator itend = pRenderablelistOfShader.m_opaqueMaterialMap.end();
			for (; itmat != itend; ++itmat)
			{
				{
					std::list<gkRenderable*>::iterator itrenderable = itmat->second.m_vecRenderable.begin();
					std::list<gkRenderable*>::iterator itrenderableend = itmat->second.m_vecRenderable.end();

					for (; itrenderable != itrenderableend; ++itrenderable)
					{
						(*itrenderable)->RT_Prepare();
					}
				}

				{
					std::list<gkRenderable*>::iterator itrenderable = itmat->second.m_vecRenderable_Skinned.begin();
					std::list<gkRenderable*>::iterator itrenderableend = itmat->second.m_vecRenderable_Skinned.end();

					for (; itrenderable != itrenderableend; ++itrenderable)
					{
						(*itrenderable)->RT_Prepare();
					}
				}

			}
			

		}
	}

	//////////////////////////////////////////////////////////////////////////
	// shadow list 也需要prepare
	{
		std::list<gkRenderable*>::iterator itrenderable = m_lstShadowCascade0.m_vecRenderable.begin();
		std::list<gkRenderable*>::iterator itrenderableend = m_lstShadowCascade0.m_vecRenderable.end();

		for (; itrenderable != itrenderableend; ++itrenderable)
		{
			(*itrenderable)->RT_Prepare();
		}
	}

	{
		std::list<gkRenderable*>::iterator itrenderable = m_lstShadowCascade0Skinned.m_vecRenderable.begin();
		std::list<gkRenderable*>::iterator itrenderableend = m_lstShadowCascade0Skinned.m_vecRenderable.end();

		for (; itrenderable != itrenderableend; ++itrenderable)
		{
			(*itrenderable)->RT_Prepare();
		}
	}

	{
		std::list<gkRenderable*>::iterator itrenderable = m_lstShadowCascade1.m_vecRenderable.begin();
		std::list<gkRenderable*>::iterator itrenderableend = m_lstShadowCascade1.m_vecRenderable.end();

		for (; itrenderable != itrenderableend; ++itrenderable)
		{
			(*itrenderable)->RT_Prepare();
		}
	}

	{
		std::list<gkRenderable*>::iterator itrenderable = m_lstShadowCascade1Skinned.m_vecRenderable.begin();
		std::list<gkRenderable*>::iterator itrenderableend = m_lstShadowCascade1Skinned.m_vecRenderable.end();

		for (; itrenderable != itrenderableend; ++itrenderable)
		{
			(*itrenderable)->RT_Prepare();
		}
	}

	{
		std::list<gkRenderable*>::iterator itrenderable = m_lstShadowCascade2.m_vecRenderable.begin();
		std::list<gkRenderable*>::iterator itrenderableend = m_lstShadowCascade2.m_vecRenderable.end();

		for (; itrenderable != itrenderableend; ++itrenderable)
		{
			(*itrenderable)->RT_Prepare();
		}
	}

// 	;
// 	gkRenderableList m_lstShadowCascade0Skinned;
// 	gkRenderableList m_lstShadowCascade1;
// 	gkRenderableList m_lstShadowCascade1Skinned;
// 	gkRenderableList m_lstShadowCascade2;


}
