#include "RendererD3D9StableHeader.h"
#include "IRenderer.h"
#include "RenderThread.h"
#include "ISystemProfiler.h"
#include "gkRendererD3D9.h"

CRenderThread::CRenderThread()
{
	m_renderingFlag = new CManualResetEvent(false);
	m_waitFlag = new CManualResetEvent(false);
}

CRenderThread::~CRenderThread()
{

}

int CRenderThread::Run()
{
	while(true)
	{
		m_waitFlag->Wait();
		m_waitFlag->Reset();

		gEnv->pProfiler->setStartRendering();

		getRenderer()->_startFrame();
		getRenderer()->RP_RenderScene();
		getRenderer()->_endFrame();

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

