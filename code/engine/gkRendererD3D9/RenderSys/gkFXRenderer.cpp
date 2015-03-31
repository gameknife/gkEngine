#include "RendererD3D9StableHeader.h"
#include "gkRendererD3D9.h"


#include "RenderRes\gkTextureManager.h"
#include "RenderRes\gkMaterialManager.h"
#include "RenderRes\gkMeshManager.h"
#include "RenderRes\gkShaderManager.h"

// pp
#include "gkPostProcessManager.h"
#include "IAuxRenderer.h"


static float GaussianDistribution1D(float x, float rho)
{
	float g = 1.0f / ( rho * sqrtf(2.0f * 3.14159265358979323f)); 
	g *= expf( -(x * x)/(2.0f * rho * rho) );
	return g;
}

void gkRendererD3D9::FX_TexBlurDirectional(gkTexturePtr pTex, const Vec2 &vDir, int nIterationsMul, gkTexturePtr pTmp)
{
	// selete tech
	gkShaderPtr pShader = gkShaderManager::ms_PostCommon;
	GKHANDLE hTech = pShader->FX_GetTechniqueByName("BlurDirectional");
	pShader->FX_SetTechnique( hTech );

	// Iterative directional blur: 1 iter: 8 taps, 64 taps, 2 iter: 512 taps, 4096 taps...
	float fSampleScale = 1.0f;
	for(int i= nIterationsMul; i >= 1 ; --i)
	{
		// 1st iteration (4 taps)

		FX_PushRenderTarget(0, pTmp);

		float fSampleSize = fSampleScale;

		// Set samples position
		float s1 = fSampleSize / (float) pTex->getWidth();  // 2.0 better results on lower res images resizing        
		float t1 = fSampleSize / (float) pTex->getHeight();       
		Vec2 vBlurDir;
		vBlurDir.x = s1 * (vDir.x + 0.5f);
		vBlurDir.y = t1 * (vDir.y + 0.5f);

		// Use rotated grid
		Vec4 pParams0 = Vec4(-vBlurDir.x*4.0f, -vBlurDir.y*4.0f, -vBlurDir.x * 3.0f, -vBlurDir.y * 3.0f); 
		Vec4 pParams1 = Vec4(-vBlurDir.x *2.0f,-vBlurDir.y * 2.0f, -vBlurDir.x , -vBlurDir.y ); 
		Vec4 pParams2 = Vec4(vBlurDir.x *2.0f,vBlurDir.y * 2.0f, vBlurDir.x , vBlurDir.y ); 
		Vec4 pParams3 = Vec4(vBlurDir.x*4.0f, vBlurDir.y*4.0f, vBlurDir.x * 3.0f, vBlurDir.y * 3.0f); 


		pShader->FX_SetValue("texToTexParams0", &pParams0, sizeof(Vec4));
		pShader->FX_SetValue("texToTexParams1", &pParams1, sizeof(Vec4));
		pShader->FX_SetValue("texToTexParams2", &pParams2, sizeof(Vec4));
		pShader->FX_SetValue("texToTexParams3", &pParams3, sizeof(Vec4));
		pShader->FX_Commit();
		pTex->Apply(0, 0);

		UINT cPasses;
		pShader->FX_Begin( &cPasses, 0 );
		for( UINT p = 0; p < cPasses; ++p )
		{
			pShader->FX_BeginPass( p );
			gkPostProcessManager::DrawFullScreenQuad(pTmp);
			pShader->FX_EndPass();
		}
		pShader->FX_End();

		FX_PopRenderTarget(0);



		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// 2nd iteration (4 x 4 taps)

		FX_PushRenderTarget(0, pTex);


		fSampleScale *= 0.25f;

		fSampleSize = fSampleScale;
		s1 = fSampleSize / (float) pTex->getWidth();  // 2.0 better results on lower res images resizing        
		t1 = fSampleSize / (float) pTex->getHeight();       
		vBlurDir.x = vDir.x * s1;
		vBlurDir.y = vDir.y * t1;

		pParams0 = Vec4(-vBlurDir.x*4.0f, -vBlurDir.y*4.0f, -vBlurDir.x * 3.0f, -vBlurDir.y * 3.0f); 
		pParams1 = Vec4(-vBlurDir.x *2.0f,-vBlurDir.y * 2.0f, -vBlurDir.x , -vBlurDir.y ); 
		pParams2 = Vec4(vBlurDir.x , vBlurDir.y, vBlurDir.x *2.0f,vBlurDir.y * 2.0f); 
		pParams3 = Vec4(vBlurDir.x * 3.0f, vBlurDir.y * 3.0f, vBlurDir.x*4.0f, vBlurDir.y*4.0f); 


		pShader->FX_SetValue("texToTexParams0", &pParams0, sizeof(Vec4));
		pShader->FX_SetValue("texToTexParams1", &pParams1, sizeof(Vec4));
		pShader->FX_SetValue("texToTexParams2", &pParams2, sizeof(Vec4));
		pShader->FX_SetValue("texToTexParams3", &pParams3, sizeof(Vec4));
		pShader->FX_Commit();
		pTmp->Apply(0, 0);

		pShader->FX_Begin( &cPasses, 0 );
		for( UINT p = 0; p < cPasses; ++p )
		{
			pShader->FX_BeginPass( p );
			gkPostProcessManager::DrawFullScreenQuad(pTex);
			pShader->FX_EndPass();
		}
		pShader->FX_End();

		FX_PopRenderTarget(0);
	}
}

void gkRendererD3D9::FX_TexBlurGaussian( gkTexturePtr tgt, int nAmount, float fScale, float fDistribution, gkTexturePtr tmp, int iterate )
{
	// blur first
	float s1 = 1.f / (float)tmp->getWidth();
	float t1 = 1.f / (float)tmp->getHeight();
	Vec4 vWhite( 1.0f, 1.0f, 1.0f, 1.0f );

	// Horizontal/Vertical pass params
	const int nSamples = 16;
	int nHalfSamples = (nSamples>>1);

	Vec4 pHParams[32], pVParams[32], pWeightsPS[32];
	float pWeights[32], fWeightSum = 0;

	memset( pWeights,0,sizeof(pWeights) );

	int s;
	for(s = 0; s<nSamples; ++s)
	{
		if(fDistribution != 0.0f)
			pWeights[s] = GaussianDistribution1D(s - nHalfSamples, fDistribution);      
		else
			pWeights[s] = 0.0f;
		fWeightSum += pWeights[s];
	}

	// normalize weights
	for(s = 0; s < nSamples; ++s)
	{
		pWeights[s] /= fWeightSum;  
	}

	// set bilinear offsets
	for(s = 0; s < nHalfSamples; ++s)
	{
		float off_a = pWeights[s*2];
		float off_b = ( (s*2+1) <= nSamples-1 )? pWeights[s*2+1] : 0;   
		float a_plus_b = (off_a + off_b);
		if (a_plus_b == 0)
			a_plus_b = 1.0f;
		float offset = off_b / a_plus_b;

		pWeights[s] = off_a + off_b;
		pWeights[s] *= fScale ;
		pWeightsPS[s] = vWhite * pWeights[s];

		float fCurrOffset = (float) s*2 + offset - nHalfSamples;
		pHParams[s] = Vec4(s1 * fCurrOffset , 0, 0, 0);  
		pVParams[s] = Vec4(0, t1 * fCurrOffset , 0, 0);       
	}


	gkShaderPtr pShader = gkShaderManager::ms_PostCommon;
	GKHANDLE hTech = pShader->FX_GetTechniqueByName("GaussBlurBilinear");
	pShader->FX_SetTechnique( hTech );


	for (uint8 i=0; i < iterate; ++i)
	{
		//////////////////////////////////////////////////////////////////////////
		// Horizon first
		// tgt -> tmp
		{
			FX_PushRenderTarget(0, tmp);

			pShader->FX_SetValue("PI_psOffsets", pHParams, sizeof(Vec4) * nHalfSamples);
			pShader->FX_SetValue("psWeights", pWeightsPS, sizeof(Vec4) * nHalfSamples);
			pShader->FX_Commit();
			tgt->Apply(0, 0);

			UINT cPasses;
			pShader->FX_Begin( &cPasses, 0 );
			for( UINT p = 0; p < cPasses; ++p )
			{
				pShader->FX_BeginPass( p );
				gkPostProcessManager::DrawFullScreenQuad(tmp);
				pShader->FX_EndPass();
			}
			pShader->FX_End();

			FX_PopRenderTarget(0);
		}

		//////////////////////////////////////////////////////////////////////////
		// Vertical second
		// tmp -> tgt
		{
			FX_PushRenderTarget(0, tgt);

			pShader->FX_SetValue("PI_psOffsets", pVParams, sizeof(Vec4) * nHalfSamples);
			pShader->FX_SetValue("psWeights", pWeightsPS, sizeof(Vec4) * nHalfSamples);
			pShader->FX_Commit();
			tmp->Apply(0, 0);

			UINT cPasses;
			pShader->FX_Begin( &cPasses, 0 );
			for( UINT p = 0; p < cPasses; ++p )
			{
				pShader->FX_BeginPass( p );
				gkPostProcessManager::DrawFullScreenQuad(tgt);
				pShader->FX_EndPass();
			}
			pShader->FX_End();

			FX_PopRenderTarget(0);
		}
	}




}

void gkRendererD3D9::FX_ClearAllSampler()
{
	GetStateManager()->SetTexture(0, NULL);
	GetStateManager()->SetTexture(1, NULL);
	GetStateManager()->SetTexture(2, NULL);
	GetStateManager()->SetTexture(3, NULL);
	GetStateManager()->SetTexture(4, NULL);
	GetStateManager()->SetTexture(5, NULL);
	GetStateManager()->SetTexture(6, NULL);
	GetStateManager()->SetTexture(7, NULL);
	GetStateManager()->SetTexture(8, NULL);
	GetStateManager()->SetTexture(9, NULL);
	GetStateManager()->SetTexture(10, NULL);
	GetStateManager()->SetTexture(11, NULL);
	GetStateManager()->SetTexture(12, NULL);
	GetStateManager()->SetTexture(13, NULL);
	GetStateManager()->SetTexture(14, NULL);
	GetStateManager()->SetTexture(15, NULL);
}


void gkRendererD3D9::FX_StrechRect( gkTexturePtr src, gkTexturePtr dest, bool FilterIfNeed )
{
	// check if they are null [10/21/2011 Kaiming]
	if (src.isNull() || dest.isNull())
	{
		return;
	}

	gkTexture* hwSrc = reinterpret_cast<gkTexture*>( src.getPointer() );
	gkTexture* hwDest = reinterpret_cast<gkTexture*>( dest.getPointer() );

	IDirect3DSurface9* pSourceSurf;
	IDirect3DSurface9* pTargetSurf;

	hwSrc->get2DTexture()->GetSurfaceLevel(0, &pSourceSurf);
	hwDest->get2DTexture()->GetSurfaceLevel(0, &pTargetSurf);

	if (FilterIfNeed || (hwSrc->getWidth() == hwDest->getWidth()) && (hwSrc->getHeight() == hwDest->getHeight()))
		m_pd3d9Device->StretchRect(pSourceSurf, NULL, pTargetSurf, NULL, D3DTEXF_POINT );
	else
		m_pd3d9Device->StretchRect(pSourceSurf, NULL, pTargetSurf, NULL, D3DTEXF_LINEAR );

	SAFE_RELEASE(pSourceSurf);
	SAFE_RELEASE(pTargetSurf);
}

void gkRendererD3D9::FX_DrawDynTextures(gkTexturePtr tex, Vec4& region)
{
	LPDIRECT3DSURFACE9 pSrc;
	LPDIRECT3DSURFACE9 pDst;

	gkTexture* hwTex = reinterpret_cast<gkTexture*>(tex.getPointer());

	hwTex->get2DTexture()->GetSurfaceLevel(0, &pSrc);
	m_pd3d9Device->GetRenderTarget(0, &pDst);

	float screenWidth = GetScreenWidth();
	float screenHeight = GetScreenHeight();

	RECT targetRect;
	targetRect.left = screenWidth * region.x;
	targetRect.top = screenHeight * region.y;
	targetRect.right = screenWidth * region.z;
	targetRect.bottom = screenHeight * region.w;

	m_pd3d9Device->StretchRect(pSrc, NULL, pDst, &targetRect, D3DTEXF_LINEAR);

	SAFE_RELEASE( pSrc );
	SAFE_RELEASE( pDst );

	// add debug text to renderer
	getAuxRenderer()->AuxRenderText( tex->getName().c_str(), region.x * (float)GetScreenWidth(), region.y * (float)GetScreenHeight(), m_pDefaultFont, ColorB(114,243,0,200));
	getAuxRenderer()->AuxRenderText( tex->getAttr(_T("format")).c_str(), region.x * (float)GetScreenWidth(), region.y * (float)GetScreenHeight() + 10, m_pDefaultFont, ColorB(180,180,180,200));

	TCHAR buffer[50];
	_stprintf( buffer, _T("%d x %d"), tex->getWidth(), tex->getHeight() );
	getAuxRenderer()->AuxRenderText( buffer, region.x * (float)GetScreenWidth(), region.y * (float)GetScreenHeight() + 20, m_pDefaultFont, ColorB(180,180,180,200));
}

void gkRendererD3D9::FX_DrawFloatTextures(gkTexturePtr tex, Vec4& region, float scale, float base)
{
	// selete tech
	gkShaderPtr pShader = gkShaderManager::ms_PostCommon;
	GKHANDLE hTech = pShader->FX_GetTechniqueByName("ShowDepthBuffer");
	pShader->FX_SetTechnique( hTech );

	// params set
	tex->Apply(0,0);

	pShader->FX_SetFloat4( "g_modifier", Vec4(scale, base, 0, 0));

	pShader->FX_Commit();

	UINT cPasses;
	pShader->FX_Begin( &cPasses, 0 );
	for( UINT p = 0; p < cPasses; ++p )
	{
		pShader->FX_BeginPass( p );

		gkPostProcessManager::DrawFullScreenQuad( GetScreenWidth(), GetScreenHeight(), region );

		pShader->FX_EndPass();
	}
	pShader->FX_End();

	// add debug text to renderer
	getAuxRenderer()->AuxRenderText( tex->getName().c_str(), region.x * (float)GetScreenWidth(), region.y * (float)GetScreenHeight(), m_pDefaultFont, ColorB(114,243,0,200));
	getAuxRenderer()->AuxRenderText( tex->getAttr(_T("format")).c_str(), region.x * (float)GetScreenWidth(), region.y * (float)GetScreenHeight() + 10, m_pDefaultFont, ColorB(180,180,180,200));
	TCHAR buffer[50];
	_stprintf( buffer, _T("%d x %d"), tex->getWidth(), tex->getHeight() );
	getAuxRenderer()->AuxRenderText( buffer, region.x * (float)GetScreenWidth(), region.y * (float)GetScreenHeight() + 20, m_pDefaultFont, ColorB(180,180,180,200));
}

void gkRendererD3D9::FX_SetRenderTarget( uint8 channel, gkTexturePtr src, bool bNeedDS, bool bClearTarget )
{
	gkTexture* hwSrc = reinterpret_cast<gkTexture*>( src.getPointer() );


	GK_ASSERT(channel < 4);
	GK_ASSERT( hwSrc->get2DTexture() );
	LPDIRECT3DSURFACE9 pSurf;
	hwSrc->get2DTexture()->GetSurfaceLevel(0, &pSurf);

	m_pd3d9Device->SetRenderTarget(channel, pSurf);
	if (bClearTarget)
		m_pd3d9Device->Clear( channel, NULL, D3DCLEAR_TARGET,
		0x00000000, 1.0f, 0L );

	if (bNeedDS)
	{
		// check if need change ds [4/30/2013 Kaiming]
		IDirect3DSurface9* currentDS;
		m_pd3d9Device->GetDepthStencilSurface( &currentDS );

		D3DSURFACE_DESC currentDesc;
		currentDS->GetDesc( &currentDesc );

		if ( currentDesc.Width != src->getWidth() || currentDesc.Height != src->getHeight() )
		{
			gkRenderTargetSize size;
			size.width = src->getWidth();
			size.height = src->getHeight();

			// need change
			gkDSCaches::iterator it = m_dsCaches.find( size );
			if ( it != m_dsCaches.end())
			{
				m_pd3d9Device->SetDepthStencilSurface( it->second );
			}
			else
			{
				IDirect3DSurface9* surface;

				// create
				m_pd3d9Device->CreateDepthStencilSurface(size.width,
					size.height,
					D3DFMT_D24S8,
					D3DMULTISAMPLE_NONE,
					0,
					TRUE,
					&(surface),
					NULL );

				m_pd3d9Device->SetDepthStencilSurface( surface );

				m_dsCaches.insert( gkDSCaches::value_type( size, surface ) );
			}
		}

		SAFE_RELEASE( currentDS );
	}

	SAFE_RELEASE( pSurf );
}

void gkRendererD3D9::FX_RestoreRenderTarget( uint8 channel, bool bNeedDS )
{
	GK_ASSERT(channel < 4);

	uint8 sizeofstack = m_RTStack[channel].size();
	if(sizeofstack > 0)
	{
		// restore the saved RT
		FX_SetRenderTarget( channel, m_RTStack[channel][sizeofstack - 1].m_pTex, bNeedDS, false);
	}
	else
	{
		// restore a NULL, if 0 channel, restore the backbuffer
		if (channel == 0)
		{
			m_pd3d9Device->SetRenderTarget(channel, m_pBackBufferSurface);
		}
		else
		{
			m_pd3d9Device->SetRenderTarget(channel, NULL);
		}
	}
}

LPDIRECT3DSURFACE9 g_prevSurf;

void gkRendererD3D9::FX_PushHwDepthTarget(gkTexturePtr src, bool bClearTarget)
{
	gkRenderTargetSize size;
	size.width = src->getWidth();
	size.height = src->getHeight();
	
	m_pd3d9Device->GetDepthStencilSurface( &g_prevSurf );

	gkTexture* hwTex = reinterpret_cast<gkTexture*>(src.getPointer());

	LPDIRECT3DSURFACE9 pSurf;
	hwTex->get2DTexture()->GetSurfaceLevel(0, &pSurf);

	m_pd3d9Device->SetDepthStencilSurface( pSurf );

	pSurf->Release();


}

void gkRendererD3D9::FX_PopHwDepthTarget()
{

	m_pd3d9Device->SetDepthStencilSurface( g_prevSurf );
	if (g_prevSurf)
	{
		g_prevSurf->Release();
	}
}

void gkRendererD3D9::FX_PushRenderTarget( uint8 channel, gkTexturePtr src, bool bNeedDS, bool bClearTarget )
{
	GK_ASSERT(channel < 4);
	m_RTStack[channel].push_back(gkDynTex(src, bNeedDS));

	FX_SetRenderTarget(channel, src, bNeedDS, bClearTarget);
}

void gkRendererD3D9::FX_PopRenderTarget( uint8 channel )
{
	GK_ASSERT(channel < 4);
	GK_ASSERT(m_RTStack[channel].size() > 0);
	
	gkDynTex tex = m_RTStack[channel].back();

	m_RTStack[channel].pop_back();

	FX_RestoreRenderTarget(channel, tex.m_needDS);
}

void gkRendererD3D9::FX_RestoreBackBuffer()
{
	m_pd3d9Device->SetRenderTarget(0, m_pBackBufferSurface);
}

void gkRendererD3D9::FX_StrechToBackBuffer(gkTexturePtr tex, ERenderStereoType m_currStereoType)
{
	IDirect3DSurface9* pTargetSurf;

	IDirect3DSurface9* pSourceSurf;
	gkTexture* hwTex = reinterpret_cast<gkTexture*>(tex.getPointer());
	hwTex->get2DTexture()->GetSurfaceLevel(0, &pSourceSurf);

	pTargetSurf = m_pBackBufferSurface;

	IDirect3DSurface9* pStereoSurf;
	gkTexture* hwStereoOut = reinterpret_cast<gkTexture*>(gkTextureManager::ms_BackBuffersStereoOut.getPointer());
	hwStereoOut->get2DTexture()->GetSurfaceLevel(0, &pStereoSurf);

	if ( m_currStereoType != eRS_Mono)
	{
		pTargetSurf = pStereoSurf;
	}

	D3DSURFACE_DESC desc;
	pTargetSurf->GetDesc( &desc );
	RECT rc;
	rc.left = 0;
	rc.top = 0;
	rc.right = desc.Width;
	rc.bottom = desc.Height;

// 	float aspect_w_h = (float)tex->getWidth() / (float)tex->getHeight();
// 
// 	float newHeight = desc.Width / aspect_w_h;
// 
// 	if (newHeight < desc.Height)
// 	{
// 		int height_new = newHeight + 0.5f;
// 
// 		rc.top = (desc.Height - height_new) / 2;
// 		rc.bottom = desc.Height - (desc.Height - height_new) / 2;
// 	}

	switch( m_currStereoType )
	{
	case eRS_Mono:

		break;
	case eRS_LeftEye:
		rc.right = desc.Width / 2;
		break;
	case eRS_RightEye:
		rc.left = desc.Width / 2;
		break;
	}

	if (tex->getWidth() != (rc.right - rc.left) || tex->getHeight() != (rc.bottom - rc.top))
	{
		m_pd3d9Device->StretchRect(pSourceSurf, NULL, pTargetSurf, &rc, D3DTEXF_LINEAR );
	}
	else
	{
		m_pd3d9Device->StretchRect(pSourceSurf, NULL, pTargetSurf, &rc, D3DTEXF_POINT );
	}
	

	SAFE_RELEASE(pSourceSurf);
	SAFE_RELEASE(pStereoSurf);
}


void gkRendererD3D9::FX_DrawScreenImage( gkTexturePtr tex, Vec4& region )
{
	gkShaderPtr pShader = gkShaderManager::ms_PostCommon;
	GKHANDLE hTech = pShader->FX_GetTechniqueByName("SimpleCopyBlended");
	pShader->FX_SetTechnique( hTech );

	tex->Apply(0, 0);

	RS_SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	RS_SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	RS_SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	UINT cPasses;
	pShader->FX_Begin( &cPasses, 0 );
	for( UINT p = 0; p < cPasses; ++p )
	{
		pShader->FX_BeginPass( p );
		gkPostProcessManager::DrawScreenQuad(region);
		pShader->FX_EndPass();
	}
	pShader->FX_End();

	RS_SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
}


void gkRendererD3D9::FX_DrawFullScreenQuad(gkTexturePtr& tex)
{
	gkPostProcessManager::DrawFullScreenQuad(tex);
}








