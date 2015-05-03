#include "RendererD3D9StableHeader.h"
#include "gkRendererD3D9.h"


#include "RenderRes\gkTextureManager.h"
#include "RenderRes\gkMaterialManager.h"
#include "RenderRes\gkMeshManager.h"
#include "RenderRes\gkShaderManager.h"

#include "gkRenderSequence.h"
#include "gkRenderLayer.h"

#include "gkLightProbeSystem.h"
#include "RenderRes/gkShader.h"
#include "ITimer.h"


void gkRendererD3D9::RP_ProcessSingleRenderable( gkRenderable* pRenderable, IShader* pShader, int8 ShadowCascade)
{
	// 首先设置dataSource的currentRenderable
	m_pShaderParamDataSource.setCurrentRenderable(pRenderable);

	// 然后，设置world矩阵相关
	pShader->FX_SetValue( "g_mWorld", &(m_pShaderParamDataSource.getWorldMatrix()) , sizeof(Matrix44) );
	pShader->FX_SetValue( "g_mWorldI", &(m_pShaderParamDataSource.getInverseWorldMatrix()), sizeof(Matrix44) );

	bool bShadow = false;
	if (ShadowCascade == -1)
	{
		pShader->FX_SetValue( "g_mWorldViewProj", &(m_pShaderParamDataSource.getWorldViewProjMatrix()), sizeof(Matrix44) );
		pShader->FX_SetValue( "g_mWorldView", &(m_pShaderParamDataSource.getWorldViewMatrix()), sizeof(Matrix44) );	
	}
	else
	{
		bShadow = true;
		pShader->FX_SetValue( "g_mWorldViewProj", &(m_pShaderParamDataSource.getWorldViewProjMatrix_ShadowCascade(ShadowCascade)), sizeof(Matrix44) );
		pShader->FX_SetValue( "g_mWorldView", &(m_pShaderParamDataSource.getWorldViewMatrix_ShadowCascade(ShadowCascade)), sizeof(Matrix44) );
	}


	// skinned mesh check
	Matrix44A* pMatrixStart = 0;
	uint32 uSize = 0;
	if( pRenderable->getSkinnedMatrix(&pMatrixStart, uSize) )
	{
		pShader->FX_SetMatrixArray( "g_mLocalMatrixArray", (D3DXMATRIX*)pMatrixStart, uSize);
	}
	
	pRenderable->RP_ShaderSet();

// 	gkTexturePtr cube = m_pLightProbeSystem->GetCubeMap(m_pShaderParamDataSource.getWorldMatrix().GetTranslation());
// 	if (!cube.isNull())
// 	{
// 		cube->Apply(7, 0);
// 	}


	pShader->FX_Commit();

	// 最后，取出vb和ib
	gkRenderOperation op; 
	pRenderable->getRenderOperation(op);

	// 应用RP_Prepare()
	pRenderable->RP_Prepare();

	// code for drawprimitives
	_render(op, bShadow);
}

void gkRendererD3D9::RP_ProcessRenderLayer( uint8 layerID, EShaderInternalTechnique tech, bool skined, int shadowCascade )
{
	gkRenderLayer* pLayer = m_pRenderingRenderSequence->getRenderLayer(layerID);
	// 基本渲染循环，循环渲染shader子层
	gkRenderLayer::ShaderGroupMapIterator shaderGroupIter = pLayer->_getShaderGroupIterator();

	while (shaderGroupIter.hasMoreElements())
	{
		IShader* pShaderOfThisLayer = shaderGroupIter.peekNextKey();
		gkShaderGroup& pRenderablelistOfShader = shaderGroupIter.getNext();

		if ( skined )
		{
			pRenderablelistOfShader.m_pShader->switchSystemMacro( eSSM_Skinned );
		}

		RP_ProcessShaderGroup(&pRenderablelistOfShader, tech, skined, shadowCascade);

		if ( skined )
		{
			pRenderablelistOfShader.m_pShader->switchSystemMacro( 0 );
		}

	}
}


void gkRendererD3D9::RP_ProcessRenderList( const gkRenderableList* objs, IShader* pShader, bool skinned, int shadowCascade )
{
	// 渲染list中的每一个renderable
	std::list<gkRenderable*>::const_iterator iter, iterend;
	if (skinned)
	{
		iterend = objs->m_vecRenderable_Skinned.end();
		for (iter = objs->m_vecRenderable_Skinned.begin(); iter != iterend; ++iter)
		{
			RP_ProcessSingleRenderable( (*iter), pShader, shadowCascade );
		}
	}
	else
	{
		iterend = objs->m_vecRenderable.end();
		for (iter = objs->m_vecRenderable.begin(); iter != iterend; ++iter)
		{
			RP_ProcessSingleRenderable( (*iter), pShader, shadowCascade );
		}
	}

}

//-----------------------------------------------------------------------
void gkRendererD3D9::RP_ProcessShaderGroup( const gkShaderGroup* pShaderGroup, EShaderInternalTechnique tech, bool skined, int shadowCascade)
{
	gkShader* pShader = reinterpret_cast<gkShader*>(pShaderGroup->m_pShader);

	GKHANDLE hTech = NULL;
	hTech = pShader->FX_GetTechnique(tech);
	if (!hTech)
	{
		return;
	}

	pShader->FX_SetTechnique( hTech );
	// set light Direction
	pShader->FX_SetValue( "g_LightPos",&(m_pShaderParamDataSource.getLightDir()), sizeof(Vec3) );
	Vec3 pSunPos = getShaderContent().getLightDir() * 100000.0f;
	pShader->FX_SetValue("g_SunPos", &pSunPos, sizeof(Vec3));

	if( tech == eSIT_ReflGenPass )
	{
		Vec3 normal = Vec3(0,0,-1);
		float offset = -1;
		Vec4 clipplane = Vec4(normal, offset);

		pShader->FX_SetValue( "g_clipplane", &clipplane, sizeof(Vec4) );
	}

	// set light Params from TOD
	const STimeOfDayKey& tod = getShaderContent().getCurrTodKey();
	ColorF lightdif = tod.clSunLight;
	ColorF lightspec = tod.clSunLight;
	lightdif *= (tod.fSunIntensity / 10.0f);
	lightspec *= tod.fSunSpecIntensity;

	// set light pass result
	if (tech == eSIT_General)
	{
		gkTextureManager::ms_SceneDifAcc->Apply(8,0);
		gkTextureManager::ms_SceneSpecAcc->Apply(9,0);
		gkTextureManager::ms_ReflMap0->Apply(10,0);
		gkTextureManager::ms_SceneDepth->Apply(11,0);
		gkTextureManager::ms_SceneNormal->Apply(12,0);
		gkTextureManager::ms_ShadowMask->Apply(13,0);
	}

	int blendIndex = gEnv->pProfiler->getFrameCount() % 2;
	gkTextureManager::ms_Blend25[blendIndex]->Apply(14,0);
	gkTextureManager::ms_Blend50[blendIndex]->Apply(15,0);

	if ( pShader->getDefaultRenderLayer() == RENDER_LAYER_WATER )
	{
		gkTextureManager::ms_ReflMap0Tmp->Apply(14,0);
	}

	float amount = 0.4f * gEnv->p3DEngine->getSnowAmount();
	pShader->FX_SetValue( "g_fSnowAmount", &(amount), sizeof(float));

	pShader->FX_SetValue( "g_LightDiffuse", &lightdif, sizeof(ColorF) );
	pShader->FX_SetValue( "g_LightSpecular", &lightspec, sizeof(ColorF) );
	pShader->FX_Commit();

	// PreShader Install [4/8/2012 Kaiming-Laptop]
	float fRealtime = gEnv->pTimer->GetCurrTime();
	pShader->FX_SetValue( "g_mView", &(m_pShaderParamDataSource.getViewMatrix()), sizeof(Matrix44) );
	pShader->FX_SetValue( "g_mProjection", &(m_pShaderParamDataSource.getProjectionMatrix()), sizeof(Matrix44)  );
	pShader->FX_SetValue( "g_mViewI", &(m_pShaderParamDataSource.getInverseViewMatrix()), sizeof(Matrix44) );
	pShader->FX_SetValue( "g_camPos", &(m_pShaderParamDataSource.getCamPos()), sizeof(Vec4) );

	switch( shadowCascade )
	{
	case 0:
		pShader->FX_SetValue( "g_camDir", &(m_pShaderParamDataSource.getShadowCamDir(0)), sizeof(Vec4) );
		break;
	case 1:
		pShader->FX_SetValue( "g_camDir", &(m_pShaderParamDataSource.getShadowCamDir(1)), sizeof(Vec4) );
		break;
	case 2:
		pShader->FX_SetValue( "g_camDir", &(m_pShaderParamDataSource.getShadowCamDir(2)), sizeof(Vec4) );
		break;
	default:
		pShader->FX_SetValue( "g_camDir", &(m_pShaderParamDataSource.getCamDir()), sizeof(Vec4) );
		break;
	}
	
	pShader->FX_SetValue( "g_fTime",  &fRealtime, sizeof(float));
	int count = gEnv->pProfiler->getFrameCount();
	pShader->FX_SetValue( "g_FrameCount", &(count) , sizeof(int));

	if ( pShader->getDefaultRenderLayer() == RENDER_LAYER_SKIES_EARLY )
	{
		pShader->FX_SetValue( "g_SkyBottom", &(tod.clZenithBottom * 0.85f), sizeof(ColorF) );
		pShader->FX_SetValue( "g_SkyTop", &(tod.clZenithTop * 0.85f), sizeof(ColorF) );
		pShader->FX_SetFloat( "g_ZenithHeight", tod.fZenithShift);

		Vec4 params = Vec4(tod.fTurbidity, tod.fRayleigh, tod.fMieCoefficent, tod.fMieDirectionalG);

		pShader->FX_SetValue("skyv2_params", &params, sizeof(Vec4));
	}

	pShader->FX_Commit();

	{
		UINT cPasses;
		pShader->FX_Begin( &cPasses, 0 );
		for( UINT p = 0; p < cPasses; ++p )
		{
			pShader->FX_BeginPass( p );
			gkShaderGroup::gkMaterialGroupRenderableMap::const_iterator it = pShaderGroup->m_opaqueMaterialMap.begin();
			for (; it != pShaderGroup->m_opaqueMaterialMap.end(); ++it)
			{
				// Set param per Material
				IMaterial* pMtl = it->first;
				pMtl->ApplyParameterBlock( true, pShader );

				

				//gkTextureManager::ms_TestCubeRT->Apply( 7, 0 );

				// apply stencil write
				if (pMtl->getSSRL())
				{
					// if success or failed, both increase
					RS_SetRenderState( D3DRS_STENCILPASS, D3DSTENCILOP_REPLACE );
					RS_SetRenderState( D3DRS_STENCILFAIL, D3DSTENCILOP_REPLACE );
				}
				else
				{
					RS_SetRenderState( D3DRS_STENCILPASS, D3DSTENCILOP_KEEP );
					RS_SetRenderState( D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP );
				}


				RP_ProcessRenderList(&(it->second), pShader, skined, shadowCascade );
			}
			pShader->FX_EndPass();
		}
		pShader->FX_End();
	}
}
//-----------------------------------------------------------------------
void gkRendererD3D9::RP_ProcessZpassObjects( const gkRenderableList* objs, IShader* pShader, EShaderInternalTechnique tech )
{
	if(!pShader)
		return;
	
	//fix: 这里直接找一个可用技术，对于单技术shader好用，以后实现复杂功能
	GKHANDLE hTech = NULL;

	//hTech = pShader->FX_GetTechniqueByName("ZPass");

	hTech = pShader->FX_GetTechnique( tech );

	pShader->FX_SetTechnique( hTech );

	UINT cPasses;
	pShader->FX_Begin( &cPasses, 0 );
	for( UINT p = 0; p < cPasses; ++p )
	{
		pShader->FX_BeginPass( p );

		// 全局参数设置，以后可考虑放置与_renderScene

		float fRealtime = gEnv->pTimer->GetCurrTime();

		pShader->FX_SetValue( "g_mView", &(m_pShaderParamDataSource.getViewMatrix()), sizeof(Matrix44) );
		pShader->FX_SetValue( "g_mProjection", &(m_pShaderParamDataSource.getProjectionMatrix()), sizeof(Matrix44)  );
		pShader->FX_SetValue( "g_mViewI", &(m_pShaderParamDataSource.getInverseViewMatrix()), sizeof(Matrix44) );
		pShader->FX_SetValue( "g_camPos", &(m_pShaderParamDataSource.getCamPos()), sizeof(Vec4) );
		pShader->FX_SetValue( "g_camDir", &(m_pShaderParamDataSource.getCamDir()), sizeof(Vec4) );
		pShader->FX_SetValue( "g_fTime",  &fRealtime, sizeof(float));
		int count = gEnv->pProfiler->getFrameCount();
		pShader->FX_SetValue( "g_FrameCount", &(count) , sizeof(int));
		pShader->FX_Commit();

		// 渲染list中的每一个renderable
		std::list<gkRenderable*>::const_iterator iter, iterend;
		iterend = objs->m_vecRenderable.end();
		for (iter = objs->m_vecRenderable.begin(); iter != iterend; ++iter)
		{
			IMaterial* pMatPtr = (*iter)->getMaterial();

			// 设置texture， 关乎阴影的alphatest
			if(pMatPtr)
			{
				if (pMatPtr->isDoubleSide())
					RS_SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

				// set normal and alpha
				pMatPtr->getTexture(eMS_Diffuse)->Apply(eMS_Diffuse,0);
				pMatPtr->getTexture(eMS_Normal)->Apply(eMS_Normal,0);
				pMatPtr->getTexture(eMS_Detail)->Apply(eMS_Detail,0);
				// set detailnormal

				//pMatPtr->BasicApply(pShader);

				// set the g_SpecularPow
				//float specpow = pMatPtr->getGlossness();
				//pShader->FX_SetValue("g_SpecularPow", &specpow, sizeof(float));

				pMatPtr->ApplyParameterBlock( false, pShader );

				pShader->FX_Commit();
			}
			
			RP_ProcessSingleRenderable( (*iter), pShader );

			// 设置texture， 关乎阴影的alphatest
			if(pMatPtr)
				if (pMatPtr->isDoubleSide())
					RS_SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);

			// 一个物体渲染完成
		}

		pShader->FX_EndPass();
	}
	pShader->FX_End();
}

//-----------------------------------------------------------------------
void gkRendererD3D9::RP_ProcessShadowObjects( const gkRenderableList* objs, uint8 cascade, BYTE sortType /*= 0*/ )
{


	gkShaderPtr pShader = gkShaderManager::ms_ShadowMapGen;

	//fix: 这里直接找一个可用技术，对于单技术shader好用，以后实现复杂功能
	GKHANDLE hTech = NULL;

	//RS_EnableAlphaTest(true);
	if (sortType == 1)
	{
		hTech = pShader->FX_GetTechniqueByName("ShadowZPassSkinned");
	}
	else
	{
		hTech = pShader->FX_GetTechniqueByName("ShadowZPass");
	}


	pShader->FX_SetTechnique( hTech );

	float fShadowsSlopeScaleBias = 0.8f;
	//m_pd3d9Device->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, *(DWORD*)&fShadowsSlopeScaleBias);

	UINT cPasses;
	pShader->FX_Begin( &cPasses, 0 );
	for( UINT p = 0; p < cPasses; ++p )
	{
		pShader->FX_BeginPass( p );

		// 全局参数设置，以后可考虑放置与_renderScene

		float fRealtime = gEnv->pTimer->GetCurrTime();

		pShader->FX_SetValue( "g_mView", &(m_pShaderParamDataSource.getViewMatrix_ShadowCascade(cascade)), sizeof(Matrix44) );
		pShader->FX_SetValue( "g_mProjection", &(m_pShaderParamDataSource.getProjectionMatrix_ShadowCascade(cascade)), sizeof(Matrix44)  );
		pShader->FX_SetValue( "g_mViewI", &(m_pShaderParamDataSource.getInverseViewMatrix_ShadowCascade(cascade)), sizeof(Matrix44) );
		pShader->FX_SetValue( "g_camPos", &(m_pShaderParamDataSource.getCamPos()), sizeof(Vec4) );
		pShader->FX_SetValue( "g_camDir", &(m_pShaderParamDataSource.getCamDir()), sizeof(Vec4) );
		pShader->FX_SetValue( "g_fTime",  &fRealtime, sizeof(float));
		int count = gEnv->pProfiler->getFrameCount();
		pShader->FX_SetValue( "g_FrameCount", &(count) , sizeof(int));
		pShader->FX_Commit();

		// 渲染list中的每一个renderable
		std::list<gkRenderable*>::const_iterator iter, iterend;
		iterend = objs->m_vecRenderable.end();
		for (iter = objs->m_vecRenderable.begin(); iter != iterend; ++iter)
		{

			IMaterial* pMatPtr = (*iter)->getMaterial();

			// 设置texture， 关乎阴影的alphatest
// 			if(pMatPtr)
// 			{
// 				if (pMatPtr->isDoubleSide())
// 					RS_SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

				// set normal and alpha
				pMatPtr->getTexture(eMS_Diffuse)->Apply(eMS_Diffuse, 0);
			//}

			RP_ProcessSingleRenderable((*iter), pShader.getPointer(), cascade);

			// 设置texture， 关乎阴影的alphatest
// 			if(pMatPtr)
// 				if (pMatPtr->isDoubleSide())
// 					RS_SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);

			// 一个物体渲染完成

		}

		pShader->FX_EndPass();
	}
	pShader->FX_End();


}

