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

void gkRendererD3D9::FX_TexBlurGaussian(gkTexturePtr tgt, int nAmount, float fScale, float fDistribution, gkTexturePtr tmp, int iterate, bool blur_mipmapchain)
{
	gkShaderPtr pShader = gkShaderManager::ms_PostCommon;
	GKHANDLE hTech = 0;

	if (!blur_mipmapchain)
	{
		hTech = pShader->FX_GetTechniqueByName("GaussBlurBilinear");
	}
	else
	{
		hTech = pShader->FX_GetTechniqueByName("GaussBlurBilinearLOD");
	}

	pShader->FX_SetTechnique( hTech );

	uint8 source_blur_mipmapchain = 0;
	uint8 target_blur_mipmapchain = 0;

	if (blur_mipmapchain)
	{
		for (int i = 0; i < tgt->getMipLevel() - 1; ++i)
		{
			GaussionBlurWithMipLevel(tmp, fDistribution, fScale, iterate, source_blur_mipmapchain++, pShader, tgt, ++target_blur_mipmapchain);
		}
		
	}
	else
	{
		GaussionBlurWithMipLevel(tmp, fDistribution, fScale, iterate, source_blur_mipmapchain, pShader, tgt, target_blur_mipmapchain);
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


void gkRendererD3D9::FX_StrechRect(gkTexturePtr src, gkTexturePtr dest, uint8 src_level, uint8 dest_level, bool FilterIfNeed, uint8 cubeindex)
{
	// check if they are null [10/21/2011 Kaiming]
	if (src.isNull() || dest.isNull())
	{
		return;
	}

	gkTexture* hwSrc = reinterpret_cast<gkTexture*>( src.getPointer() );
	gkTexture* hwDest = reinterpret_cast<gkTexture*>( dest.getPointer() );

	IDirect3DSurface9* pSourceSurf = 0;
	IDirect3DSurface9* pTargetSurf = 0;


	// copy 2d
	if (hwSrc->get2DTexture())
	{
		hwSrc->get2DTexture()->GetSurfaceLevel(src_level, &pSourceSurf);
	}
	else if (hwSrc->getCubeTexture())
	{
		hwSrc->getCubeTexture()->GetCubeMapSurface((D3DCUBEMAP_FACES)cubeindex, src_level, &pSourceSurf);
	}

	if (hwDest->get2DTexture())
	{
		hwDest->get2DTexture()->GetSurfaceLevel(dest_level, &pTargetSurf);
	}
	else if(hwDest->getCubeTexture())
	{
		hwDest->getCubeTexture()->GetCubeMapSurface((D3DCUBEMAP_FACES)cubeindex, dest_level, &pTargetSurf);
	}
		
	D3DSURFACE_DESC descSrc;
	D3DSURFACE_DESC descTgt;
	pSourceSurf->GetDesc(&descSrc);
	pTargetSurf->GetDesc(&descTgt);

	if (FilterIfNeed || (descSrc.Width == descTgt.Width) && (descSrc.Height == descTgt.Height))
		m_pd3d9Device->StretchRect(pSourceSurf, NULL, pTargetSurf, NULL, D3DTEXF_POINT);
	else
		m_pd3d9Device->StretchRect(pSourceSurf, NULL, pTargetSurf, NULL, D3DTEXF_LINEAR);

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

void gkRendererD3D9::FX_SetRenderTarget( uint8 channel, gkTexturePtr src, uint8 level, uint8 index, bool bNeedDS, bool bClearTarget )
{
	gkTexture* hwSrc = reinterpret_cast<gkTexture*>( src.getPointer() );


	GK_ASSERT(channel < 4);
	//GK_ASSERT( hwSrc->get2DTexture() );
	LPDIRECT3DSURFACE9 pSurf;

	if (hwSrc->get2DTexture())
	{
		hwSrc->get2DTexture()->GetSurfaceLevel(level, &pSurf);
	}
	else if (hwSrc->getCubeTexture())
	{
		hwSrc->getCubeTexture()->GetCubeMapSurface((D3DCUBEMAP_FACES)index, level, &pSurf);
	}
	

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

void gkRendererD3D9::FX_RestoreRenderTarget( uint8 channel )
{
	GK_ASSERT(channel < 4);

	uint8 sizeofstack = m_RTStack[channel].size();
	if(sizeofstack > 0)
	{
		// restore the saved RT
		FX_SetRenderTarget(channel, m_RTStack[channel][sizeofstack - 1].m_pTex, m_RTStack[channel][sizeofstack - 1].m_level, m_RTStack[channel][sizeofstack - 1].m_index, m_RTStack[channel][sizeofstack - 1].m_needDS, false);
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

void gkRendererD3D9::FX_PushRenderTarget( uint8 channel, gkTexturePtr src, uint8 level, uint8 index, bool bNeedDS, bool bClearTarget )
{
	GK_ASSERT(channel < 4);
	m_RTStack[channel].push_back(gkDynTex(src, level, index, bNeedDS));

	FX_SetRenderTarget(channel, src, level, index, bNeedDS, bClearTarget);
}

void gkRendererD3D9::FX_PopRenderTarget( uint8 channel )
{
	GK_ASSERT(channel < 4);
	GK_ASSERT(m_RTStack[channel].size() > 0);
	
	gkDynTex tex = m_RTStack[channel].back();

	m_RTStack[channel].pop_back();

	FX_RestoreRenderTarget(channel);
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


void gkRendererD3D9::GaussionBlurWithMipLevel(gkTexturePtr tmp, float fDistribution, float fScale, int iterate, uint8 source_blur_mipmapchain, gkShaderPtr pShader, gkTexturePtr tgt, uint8 target_blur_mipmapchain)
{
	// blur first


	for (uint8 i = 0; i < iterate; ++i)
	{
		//////////////////////////////////////////////////////////////////////////
		// Horizon first
		// tgt -> tmp
		{
			float s1 = 1.f / (float)(tmp->getWidth() >> source_blur_mipmapchain);
			float t1 = 1.f / (float)(tmp->getHeight() >> source_blur_mipmapchain);
			GaussionBlurH(fDistribution, fScale, s1, t1, tmp, source_blur_mipmapchain, pShader, tgt);

		}

		//////////////////////////////////////////////////////////////////////////
		// Vertical second
		// tmp -> tgt
		{
			float s1 = 1.f / (float)(tgt->getWidth() >> source_blur_mipmapchain);
			float t1 = 1.f / (float)(tgt->getHeight() >> source_blur_mipmapchain);
			GaussionBlurV(fDistribution, fScale, s1, t1, tgt, target_blur_mipmapchain, pShader, source_blur_mipmapchain, tmp);
		}
	}
}


void gkRendererD3D9::GaussionBlurH(float fDistribution, float fScale, float s1, float t1, gkTexturePtr tmp, uint8 source_blur_mipmapchain, gkShaderPtr pShader, gkTexturePtr tgt)
{
	Vec4 vWhite(1.0f, 1.0f, 1.0f, 1.0f);

	// Horizontal/Vertical pass params
	const int nSamples = 16;
	int nHalfSamples = (nSamples >> 1);

	Vec4 pHParams[32], pVParams[32], pWeightsPS[32];
	float pWeights[32], fWeightSum = 0;

	memset(pWeights, 0, sizeof(pWeights));

	int s;
	for (s = 0; s < nSamples; ++s)
	{
		if (fDistribution != 0.0f)
			pWeights[s] = GaussianDistribution1D(s - nHalfSamples, fDistribution);
		else
			pWeights[s] = 0.0f;
		fWeightSum += pWeights[s];
	}

	// normalize weights
	for (s = 0; s < nSamples; ++s)
	{
		pWeights[s] /= fWeightSum;
	}

	// set bilinear offsets
	for (s = 0; s < nHalfSamples; ++s)
	{
		float off_a = pWeights[s * 2];
		float off_b = ((s * 2 + 1) <= nSamples - 1) ? pWeights[s * 2 + 1] : 0;
		float a_plus_b = (off_a + off_b);
		if (a_plus_b == 0)
			a_plus_b = 1.0f;
		float offset = off_b / a_plus_b;

		pWeights[s] = off_a + off_b;
		pWeights[s] *= fScale;
		pWeightsPS[s] = vWhite * pWeights[s];

		float fCurrOffset = (float)s * 2 + offset - nHalfSamples;
		pHParams[s] = Vec4(s1 * fCurrOffset, 0, 0, 0);
		pVParams[s] = Vec4(0, t1 * fCurrOffset, 0, 0);
	}


	FX_PushRenderTarget(0, tmp, source_blur_mipmapchain);

	pShader->FX_SetValue("PI_psOffsets", pHParams, sizeof(Vec4) * nHalfSamples);
	pShader->FX_SetValue("psWeights", pWeightsPS, sizeof(Vec4) * nHalfSamples);

	pShader->FX_SetFloat("source_lod", (float)source_blur_mipmapchain);

	pShader->FX_Commit();
	tgt->Apply(0, 0);

	UINT cPasses;
	pShader->FX_Begin(&cPasses, 0);
	for (UINT p = 0; p < cPasses; ++p)
	{
		pShader->FX_BeginPass(p);
		gkPostProcessManager::DrawFullScreenQuad(tmp, Vec4(0, 0, 1, 1), Vec2(1, 1), source_blur_mipmapchain);
		pShader->FX_EndPass();
	}
	pShader->FX_End();

	FX_PopRenderTarget(0);
}


void gkRendererD3D9::GaussionBlurV(float fDistribution, float fScale, float s1, float t1, gkTexturePtr tgt, uint8 target_blur_mipmapchain, gkShaderPtr pShader, uint8 source_blur_mipmapchain, gkTexturePtr tmp)
{
	Vec4 vWhite(1.0f, 1.0f, 1.0f, 1.0f);

	// Horizontal/Vertical pass params
	const int nSamples = 16;
	int nHalfSamples = (nSamples >> 1);

	Vec4 pHParams[32], pVParams[32], pWeightsPS[32];
	float pWeights[32], fWeightSum = 0;

	memset(pWeights, 0, sizeof(pWeights));

	int s;
	for (s = 0; s < nSamples; ++s)
	{
		if (fDistribution != 0.0f)
			pWeights[s] = GaussianDistribution1D(s - nHalfSamples, fDistribution);
		else
			pWeights[s] = 0.0f;
		fWeightSum += pWeights[s];
	}

	// normalize weights
	for (s = 0; s < nSamples; ++s)
	{
		pWeights[s] /= fWeightSum;
	}

	// set bilinear offsets
	for (s = 0; s < nHalfSamples; ++s)
	{
		float off_a = pWeights[s * 2];
		float off_b = ((s * 2 + 1) <= nSamples - 1) ? pWeights[s * 2 + 1] : 0;
		float a_plus_b = (off_a + off_b);
		if (a_plus_b == 0)
			a_plus_b = 1.0f;
		float offset = off_b / a_plus_b;

		pWeights[s] = off_a + off_b;
		pWeights[s] *= fScale;
		pWeightsPS[s] = vWhite * pWeights[s];

		float fCurrOffset = (float)s * 2 + offset - nHalfSamples;
		pHParams[s] = Vec4(s1 * fCurrOffset, 0, 0, 0);
		pVParams[s] = Vec4(0, t1 * fCurrOffset, 0, 0);
	}

	FX_PushRenderTarget(0, tgt, target_blur_mipmapchain);

	pShader->FX_SetValue("PI_psOffsets", pVParams, sizeof(Vec4) * nHalfSamples);
	pShader->FX_SetValue("psWeights", pWeightsPS, sizeof(Vec4) * nHalfSamples);

	pShader->FX_SetFloat("source_lod", (float)source_blur_mipmapchain);

	pShader->FX_Commit();
	tmp->Apply(0, 0);

	UINT cPasses;
	pShader->FX_Begin(&cPasses, 0);
	for (UINT p = 0; p < cPasses; ++p)
	{
		pShader->FX_BeginPass(p);
		gkPostProcessManager::DrawFullScreenQuad(tgt, Vec4(0, 0, 1, 1), Vec2(1, 1), target_blur_mipmapchain);
		pShader->FX_EndPass();
	}
	pShader->FX_End();

	FX_PopRenderTarget(0);
}

void gkRendererD3D9::FX_BlurCubeMap(gkTexturePtr cubetgt, int nAmount, float fScale, float fDistribution, gkTexturePtr tmp, int iterate)
{
	for (int cube = 0; cube < 6; ++cube)
	{
		for (int level = 1; level < cubetgt->getMipLevel(); ++level)
		{
			//for (int i = 0; i < 2; ++i)
			{
				Vec4 vWhite(1.0f, 1.0f, 1.0f, 1.0f);

				gkShaderPtr pShader = gkShaderManager::ms_PostCommon;
				GKHANDLE hTech = 0;
				hTech = pShader->FX_GetTechniqueByName("GaussBlurBilinearCubeLOD");
				pShader->FX_SetTechnique(hTech);

				float s1 = 1.f / (float)(cubetgt->getWidth() >> level);
				float t1 = 1.f / (float)(cubetgt->getHeight() >> level);

				//////////////////////////////////////////////////////////////////////////
				// Horizontal/Vertical pass params
				const int nSamples = 16;
				int nHalfSamples = (nSamples >> 1);

				Vec4 pHParams[32], pVParams[32], pWeightsPS[32];
				float pWeights[32], fWeightSum = 0;

				memset(pWeights, 0, sizeof(pWeights));

				int s;
				for (s = 0; s < nSamples; ++s)
				{
					if (fDistribution != 0.0f)
						pWeights[s] = GaussianDistribution1D(s - nHalfSamples, fDistribution);
					else
						pWeights[s] = 0.0f;
					fWeightSum += pWeights[s];
				}

				// normalize weights
				for (s = 0; s < nSamples; ++s)
				{
					pWeights[s] /= fWeightSum;
				}

				// set bilinear offsets
				for (s = 0; s < nHalfSamples; ++s)
				{
					float off_a = pWeights[s * 2];
					float off_b = ((s * 2 + 1) <= nSamples - 1) ? pWeights[s * 2 + 1] : 0;
					float a_plus_b = (off_a + off_b);
					if (a_plus_b == 0)
						a_plus_b = 1.0f;
					float offset = off_b / a_plus_b;

					pWeights[s] = off_a + off_b;
					pWeights[s] *= fScale;
					pWeightsPS[s] = vWhite * pWeights[s];

					float fCurrOffset = (float)s * 2 + offset - nHalfSamples;
					pHParams[s] = Vec4( fCurrOffset, 0, 0, 0);
					pVParams[s] = Vec4(0, fCurrOffset, 0, 0);
				}

				//////////////////////////////////////////////////////////////////////////
				// change VParam to cube normal


				// cubemap space
// 				y  x
// 				| /
// 			 z__|/

				Vec3 rotateNormalH;
				Vec3 rotateNormalV;
				switch (cube)
				{
					case 0: // POS X
					case 1: // NEG X
						rotateNormalH = Vec3(0, 0, 1);
						rotateNormalV = Vec3(0, 1, 0);
						break;
					case 2: // POS Y
					case 3: // NEG Y
						rotateNormalH = Vec3(0, 0, 1);
						rotateNormalV = Vec3(1, 0, 0);
						break;
					case 4: // POS Z
					case 5: // NEG Z
						rotateNormalH = Vec3(1, 0, 0);
						rotateNormalV = Vec3(0, 1, 0);
						break;
				}
				
				// 90 degree == tex width
				// 0.5PI == tex width
				// 1 pixel == 0.5PI / tex width

				for (s = 0; s < nHalfSamples; ++s)
				{
					pHParams[s] = Vec4(rotateNormalH, pHParams[s].x * 0.5 * g_PI * s1 );
					pVParams[s] = Vec4(rotateNormalV, pVParams[s].y * 0.5 * g_PI * t1 );
				}


				//////////////////////////////////////////////////////////////////////////
				// down sample now
				gkRendererD3D9::FX_StrechRect(cubetgt, cubetgt, level - 1, level, false, cube);


				gkRendererD3D9::FX_PushRenderTarget(0, tmp, level, cube);
				{
					pShader->FX_SetValue("PI_psOffsets", pHParams, sizeof(Vec4) * nHalfSamples);
					pShader->FX_SetValue("psWeights", pWeightsPS, sizeof(Vec4) * nHalfSamples);
					pShader->FX_SetFloat("source_lod", (float)(level));

					pShader->FX_Commit();
					cubetgt->Apply(0, 0);

					UINT cPasses;
					pShader->FX_Begin(&cPasses, 0);
					for (UINT p = 0; p < cPasses; ++p)
					{
						pShader->FX_BeginPass(p);
						gkPostProcessManager::DrawFullScreenQuadCubeSpec(tmp->getWidth(), tmp->getHeight(), level, cube);
						pShader->FX_EndPass();
					}
					pShader->FX_End();
				}


				gkRendererD3D9::FX_PopRenderTarget(0);

				gkRendererD3D9::FX_PushRenderTarget(0, cubetgt, level, cube);
				{
					pShader->FX_SetValue("PI_psOffsets", pVParams, sizeof(Vec4) * nHalfSamples);
					pShader->FX_SetValue("psWeights", pWeightsPS, sizeof(Vec4) * nHalfSamples);
					pShader->FX_SetFloat("source_lod", (float)(level));

					pShader->FX_Commit();
					tmp->Apply(0, 0);


					UINT cPasses;
					pShader->FX_Begin(&cPasses, 0);
					for (UINT p = 0; p < cPasses; ++p)
					{
						pShader->FX_BeginPass(p);
						gkPostProcessManager::DrawFullScreenQuadCubeSpec(tmp->getWidth(), tmp->getHeight(), level, cube);
						pShader->FX_EndPass();
					}
					pShader->FX_End();
				}


				gkRendererD3D9::FX_PopRenderTarget(0);
			}
		}
	}
}