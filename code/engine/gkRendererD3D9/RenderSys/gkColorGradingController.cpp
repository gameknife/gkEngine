#include "RendererD3D9StableHeader.h"
#include "gkColorGradingController.h"
#include "RenderRes\gkTextureManager.h"
#include "RenderRes\gkShaderManager.h"
#include "gkRendererD3D9.h"
#include "ITimer.h"


//////////////////////////////////////////////////////////////////////////
gkColorGradingController::gkColorGradingController()
{
	m_pTargetColorChart.setNull();
	m_fBlendAmount = 1.0f;
}

//////////////////////////////////////////////////////////////////////////
gkTexturePtr gkColorGradingController::getCurColorChart()
{
	return gkTextureManager::ms_MergedColorChart;
}

//////////////////////////////////////////////////////////////////////////
void gkColorGradingController::_RT_RenderingMergedColorChart()
{
	static bool bFirst = true;
	gkShaderPtr pShader = gkShaderManager::ms_PostCommon;
//	getRenderer()->FX_PushRenderTarget(0, gkTextureManager::ms_CurrColorChart);

// 	GKHANDLE hTech = pShader->FX_GetTechniqueByName("TransitionColorChart");
// 	pShader->FX_SetTechnique( hTech );
// 
// 	if (bFirst)
// 	{
// 		gkTextureManager::ms_DefaultColorChart->Apply(0, 0);
// 		bFirst = false;
// 	}
// 	else
// 	{
// 		gkTextureManager::ms_PrevColorChart->Apply(0, 0);
// 	}
// 	
// 
// 	if (m_pTargetColorChart.isNull())
// 	{
// 		gkTextureManager::ms_DefaultColorChart->Apply(1, 0);
// 	}
// 	else
// 	{
// 		m_pTargetColorChart->Apply(1, 0);
// 	}
// 	
// 
// 
// 	{
// 		UINT cPasses;
// 		pShader->FX_Begin( &cPasses, 0 );
// 		for( UINT p = 0; p < cPasses; ++p )
// 		{
// 			pShader->FX_BeginPass( p );
// 
// 			float elapsedTime = gEnv->pTimer->GetFrameTime();
// 			elapsedTime = 1 - powf( 0.98f, 4.0f * 33.0f * elapsedTime);
// 			pShader->FX_SetFloat("fTransitionLerp", elapsedTime);
// 			pShader->FX_Commit();
// 
// 			getRenderer()->FX_DrawFullScreenQuad(gkTextureManager::ms_CurrColorChart);
// 
// 			pShader->FX_EndPass();
// 		}
// 		pShader->FX_End();
// 	}
// 	getRenderer()->FX_PopRenderTarget(0);
// 	getRenderer()->FX_StrechRect( gkTextureManager::ms_CurrColorChart, gkTextureManager::ms_PrevColorChart, false );




	// MERGE Color Corrections


	getRenderer()->FX_PushRenderTarget(0, gkTextureManager::ms_MergedColorChart);
	GKHANDLE hTech = pShader->FX_GetTechniqueByName("MergeColorChart");
	pShader->FX_SetTechnique( hTech );


	gkTextureManager::ms_DefaultColorChart->Apply(0, 0);

	const STimeOfDayKey& tod = getRenderer()->getShaderContent().getCurrTodKey();

	{
		UINT cPasses;
		pShader->FX_Begin( &cPasses, 0 );
		for( UINT p = 0; p < cPasses; ++p )
		{
			pShader->FX_BeginPass( p );

			Vec4 pParam;
			pParam = Vec4(tod.fLevelInputMin, tod.fLevel, tod.fLeveInputMax, 0);
			pShader->FX_SetValue("ColorGradingParams0", &pParam, sizeof(Vec4));

			pParam = Vec4(255, tod.fSaturate, 0, 0);
			pShader->FX_SetValue("ColorGradingParams1", &pParam, sizeof(Vec4));


			pShader->FX_Commit();

			getRenderer()->FX_DrawFullScreenQuad(gkTextureManager::ms_MergedColorChart);

			pShader->FX_EndPass();
		}
		pShader->FX_End();
	}



	getRenderer()->FX_PopRenderTarget(0);
}

