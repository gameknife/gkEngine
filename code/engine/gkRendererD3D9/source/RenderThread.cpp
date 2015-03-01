#include "RendererD3D9StableHeader.h"
#include "IRenderer.h"
#include "RenderThread.h"
#include "ISystemProfiler.h"
#include "gkRendererD3D9.h"
#include "IStereoDevice.h"

CRenderThread::CRenderThread()
{
	m_renderingFlag = new CManualResetEvent(false);
	m_waitFlag = new CManualResetEvent(false);
}

CRenderThread::~CRenderThread()
{
	SAFE_DELETE(m_renderingFlag);
	SAFE_DELETE(m_waitFlag);
}

int CRenderThread::Run()
{
	while(true)
	{
		m_waitFlag->Wait();
		m_waitFlag->Reset();

		gEnv->pProfiler->setStartRendering();

		if( g_pRendererCVars->r_stereo && gEnv->pStereoDevice && gEnv->pStereoDevice->Avaliable() )
		{
			getRenderer()->_startFrame(eRS_LeftEye);
			getRenderer()->RP_RenderScene(eRS_LeftEye);
			getRenderer()->_endFrame(eRS_LeftEye);

			getRenderer()->_startFrame(eRS_RightEye);
			getRenderer()->RP_RenderScene(eRS_RightEye);
			getRenderer()->_endFrame(eRS_RightEye);
		}
		else
		{
			getRenderer()->_startFrame(eRS_Mono);
			getRenderer()->RP_RenderScene(eRS_Mono);
			getRenderer()->_endFrame(eRS_Mono);
		}

		gEnv->pProfiler->setEndRendering();

		m_renderingFlag->Set();
	}

	return 1;
}

int CRenderThread::WaitForRenderFinish()
{
	gEnv->pProfiler->setStartWaiting();
	m_renderingFlag->Wait();
	m_renderingFlag->Reset();
	gEnv->pProfiler->setEndWaiting();
	return 1;
}

int CRenderThread::SetReadyToRender()
{
	m_waitFlag->Set();
	return 1;
}

