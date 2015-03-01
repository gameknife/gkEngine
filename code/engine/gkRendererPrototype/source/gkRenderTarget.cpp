#include "StableHeader.h"
#include "gkRenderTarget.h"
#include "ICamera.h"

gkRenderTarget::gkRenderTarget():	
	m_wstrName(_T("DefaultRT")),
	m_pCamera(NULL),
// 	m_pD3DDevice9(NULL),
// 	m_ppD3DSurf9_RT0(NULL),
// 	m_ppD3DSurf9_RT1(NULL),
// 	m_ppD3DSurf9_RT2(NULL),
// 	m_ppD3DSurf9_RT3(NULL),
	m_bActive(true),
	m_bAutoUpdate(true),
	m_fUpdateInterval(0.5f),
	m_bIsCreated(false)
{

}
//-----------------------------------------------------------------------
gkRenderTarget::gkRenderTarget( gkStdString name):
	m_wstrName(name),
	m_pCamera(NULL),
// 	m_pD3DDevice9(NULL),
// 	m_ppD3DSurf9_RT0(NULL),
// 	m_ppD3DSurf9_RT1(NULL),
// 	m_ppD3DSurf9_RT2(NULL),
// 	m_ppD3DSurf9_RT3(NULL),
	m_bActive(true),
	m_bAutoUpdate(true),
	m_fUpdateInterval(0.5f),
	m_bIsCreated(false)
{

}
gkRenderTarget::~gkRenderTarget()
{

}

//-----------------------------------------------------------------------
void gkRenderTarget::setCamera( ICamera* cam )
{
	m_pCamera = cam;
}

//-----------------------------------------------------------------------
void gkRenderTarget::_beginUpdate()
{
	//TODO: RTT和RTW通用的一些统计信息
}

//-----------------------------------------------------------------------
void gkRenderTarget::_endUpdate()
{
	//TODO: RTT和RTW通用的一些统计信息
}

//-----------------------------------------------------------------------
void gkRenderTarget::_update()
{
	//TODO: RTT和RTW通用的一些统计信息
	updateImpl();
}

//-----------------------------------------------------------------------
void gkRenderTarget::updateImpl()
{
	_beginUpdate();
	if (m_pCamera)
	{
		// do not set the RT
// 		if(m_ppD3DSurf9_RT0 && *m_ppD3DSurf9_RT0)
// 		{
// 			// 设置该RT的SURFACE作为渲染输出
// 			m_pD3DDevice9->SetRenderTarget(0, (*m_ppD3DSurf9_RT0));
// 			// 是否需要设置normal和position
// 			if (m_ppD3DSurf9_RT1 && *m_ppD3DSurf9_RT1)
// 			{
// 				m_pD3DDevice9->SetRenderTarget(1, (*m_ppD3DSurf9_RT1));
// 			}
// 			if (m_ppD3DSurf9_RT2 && *m_ppD3DSurf9_RT2)
// 			{
// 				m_pD3DDevice9->SetRenderTarget(2, (*m_ppD3DSurf9_RT2));
// 			}
// 			if (m_ppD3DSurf9_RT3 && *m_ppD3DSurf9_RT3)
// 			{
// 				m_pD3DDevice9->SetRenderTarget(3, (*m_ppD3DSurf9_RT3));
// 			}

			m_pCamera->_renderScene();

			// release

//		}
	}
	_endUpdate();
}
