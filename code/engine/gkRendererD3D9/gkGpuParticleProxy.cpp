#include "RendererD3D9StableHeader.h"
#include "gkGpuParticleProxy.h"
#include "IResourceManager.h"
#include "RenderRes/gkShaderManager.h"
#include "gkPostProcessManager.h"

void gkGpuParticleProxy::Update()
{
	float fElapsedTime = gEnv->pTimer->GetFrameTime();
	float fTime = gEnv->pTimer->GetCurrTime();

	gkShaderPtr pShader = gkShaderManager::ms_ParticleUpdate;

	// EXPIRE
 	m_content.PushVelRT();
 	m_content.PushPosRT(1);
 
 	pShader->FX_SetTechniqueByName( "Expire" );
 
 	UINT cPasses;
 	pShader->FX_Begin( &cPasses, 0 );
 	for( UINT p = 0; p < cPasses; ++p )
 	{
 		pShader->FX_BeginPass( p );
 
 		m_content.g_prevVELRT->Apply(0, 0);
 		m_content.g_prevPOSRT->Apply(1, 0);
 
 		gkPostProcessManager::DrawFullScreenQuad( m_content.g_currPOSRT );
 
 		pShader->FX_EndPass();
 	}
 	pShader->FX_End();
 
 	m_content.PopVelRT();
 	m_content.PopPosRT(1);
 	m_content.Swap();

	// EMIT
	// hack emit
 	m_emitTimer += fElapsedTime;
 	if (m_emitTimer > 0.1)
 	{
 		m_emitTimer = 0;
 
 		m_content.PushVelRT();
 		m_content.PushPosRT(1);
 
 		pShader->FX_SetTechniqueByName( "Emit" );
 		int emitCount = 1;
 		//pShader->FX_SetValue( "g_emitCount", &emitCount, sizeof(int) );
 
 		float initSpeed = 5.0f;
 		float ran0 = rand() % 10000 / 10000.f - 0.5f;
 		float ran1 = rand() % 10000 / 10000.f * 0.5 + 0.5;// - 0.5f;
 		float ran2 = rand() % 10000 / 10000.f - 0.5f;
 		pShader->FX_SetFloat4( "g_emitParam", Vec4(ran0,ran1 * initSpeed,ran2,1));
 		pShader->FX_SetFloat4( "g_emitPos", Vec4(0,0,0,1));
 
		m_emitted++;
		m_emitted = m_emitted % m_maxCount;
		int x = m_emitted % (64 );
		int y = m_emitted / (64 );

		//g_pEffect9->SetVectorArray("g_emitIndex", &(D3DXVECTOR4( (float)x / (float)(SQRT_PARTICLE_COUNT - 1), (float)y / (float)(SQRT_PARTICLE_COUNT - 1),0,1)), 1);
		pShader->FX_SetFloat4("g_emitIndex", Vec4( x, y, 0,1));


 		UINT cPasses;
 		pShader->FX_Begin( &cPasses, 0 );
 		for( UINT p = 0; p < cPasses; ++p )
 		{
 			pShader->FX_BeginPass( p );
 
 			m_content.g_prevVELRT->Apply(0, 0);
 			m_content.g_prevPOSRT->Apply(1, 0);
 
 			gkPostProcessManager::DrawFullScreenQuad( m_content.g_currPOSRT );
 
 			pShader->FX_EndPass();
 		}
 		pShader->FX_End();
 
 		m_content.PopVelRT();
 		m_content.PopPosRT(1);
 		m_content.Swap();
 	}

	// UPDATE
	{
		// ACCELERATE

		m_content.PushVelRT();

		pShader->FX_SetTechniqueByName( "Eular_Attraction" );

		Vec4 time(fElapsedTime, fElapsedTime, fElapsedTime, fElapsedTime);
		Vec4 pos[3];
		pos[0] = Vec4(sinf(fTime + 0.5) * 1,cosf(fTime + 0.5) * 0.8,cosf(fTime + 0.5) * 1,0);
		pos[1] = Vec4(sinf(fTime) + 0.5, cosf(fTime), cosf(fTime + 0.5), 0);
		pos[2] = Vec4(sinf(fTime - 0.5) * 0.5,cosf(fTime - 0.5) * 0.2,cosf(fTime + 0.5) * 0.5,0);

		pShader->FX_SetValue( "g_timeVar", &time, sizeof(Vec4) );
		//pShader->SetValue( "g_staticForce", &force, sizeof(Vec4) );


		pShader->FX_SetValue( "g_attract0", &(pos[0]), sizeof(Vec4) );
		pShader->FX_SetValue( "g_attract1", &(pos[1]), sizeof(Vec4) );
		pShader->FX_SetValue( "g_attract2", &(pos[2]), sizeof(Vec4) );

		UINT cPasses;
		pShader->FX_Begin( &cPasses, 0 );
		for( UINT p = 0; p < cPasses; ++p )
		{
			pShader->FX_BeginPass( p );
			
			m_content.g_prevVELRT->Apply(0, 0);
			m_content.g_prevPOSRT->Apply(1, 0);

			gkPostProcessManager::DrawFullScreenQuad( m_content.g_currPOSRT );

			pShader->FX_EndPass();
		}
		pShader->FX_End();

		m_content.PopVelRT();
		//m_content.SwapVelocity();


		m_content.PushPosRT();

		pShader->FX_SetTechniqueByName( "Eular_Update" );

		pShader->FX_SetValue( "g_timeVar", &time, sizeof(Vec4) );

		pShader->FX_Begin( &cPasses, 0 );
		for( UINT p = 0; p < cPasses; ++p )
		{
			pShader->FX_BeginPass( p );

			m_content.g_currVELRT->Apply(0, 0);
			m_content.g_prevPOSRT->Apply(1, 0);

			gkPostProcessManager::DrawFullScreenQuad( m_content.g_currPOSRT );

			pShader->FX_EndPass();
		}
		pShader->FX_End();

		m_content.PopPosRT();
	}


	m_content.Swap();

}

void gkGpuParticleProxy::createFromXmlNode( CRapidXmlParseNode* node )
{
	TCHAR buffer[MAX_PATH];	
	m_emitted = 0;

	gkNameValuePairList createlist;

	// set params
	createlist[_T("d3dpool")] =	_T("D3DX_DEFAULT");
	createlist[_T("usage")] =		_T("RENDERTARGET");
	createlist[_T("size")] =		_T("64");
	createlist[_T("format")] =		_T("A32B32G32R32F");
	createlist[_T("initcolor")] =		_T("WHITE");

	_stprintf( buffer, _T("rt_gpuptc_%x_POSRT0"), (uint32)this);
	m_updatePOSRT0 = gEnv->pSystem->getTextureMngPtr()->create( buffer, _T("dyntex"), &createlist );
	//m_updatePOSRT0->load();

	_stprintf( buffer, _T("rt_gpuptc_%x_POSRT1"), (uint32)this);
	m_updatePOSRT1 = gEnv->pSystem->getTextureMngPtr()->create( buffer, _T("dyntex"), &createlist );
	//m_updatePOSRT0->load();

	_stprintf( buffer, _T("rt_gpuptc_%x_VELRT0"), (uint32)this);
	m_updateVELRT0 = gEnv->pSystem->getTextureMngPtr()->create( buffer, _T("dyntex"), &createlist );
	//m_updatePOSRT0->load();

	_stprintf( buffer, _T("rt_gpuptc_%x_VELRT1"), (uint32)this);
	m_updateVELRT1 = gEnv->pSystem->getTextureMngPtr()->create( buffer, _T("dyntex"), &createlist );
	//m_updatePOSRT0->load();


	m_content.Bind( m_updatePOSRT0, m_updatePOSRT1, m_updateVELRT0, m_updateVELRT1 );

	m_emitTimer = 0;
	m_maxCount = 1000;
}

void gkGpuParticleProxy::ApplyGpuData()
{
	m_content.g_prevPOSRT->Apply( D3DVERTEXTEXTURESAMPLER0, 0 );
}

