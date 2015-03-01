#include "gkStableHeader.h"
#if 0
#include "gkVtxLayerManager.h"
#include "gkCamera.h"
#include "GameKnifeUI.h"
#include "gkMovieMovable.h"
#include "vtxAtlasPacker.h"
//#include "gkRenderTexture.h"
#include "gkSceneManager.h"
#include "gk3DEngine.h"
//#include "gkD3D9RenderSystem.h"
#include "gkSceneNode.h"
#include "IRenderSequence.h"
#include "IRenderTarget.h"

#include "gkFilePath.h"


#ifndef WIN64

// vtxGUI库
#include "vtxFile.h"
#include "vtxFileManager.h"
#include "vtxMovie.h"
#include "vtxMovieDebugger.h"
#include "vtxRoot.h"
#include "vtxScriptCallbackListener.h"
#include "vtxScriptEngine.h"
#include "gkVtxMovieListener.h"


//-----------------------------------------------------------------------
gkVtxLayerManager::gkVtxLayerManager():m_pVtxRoot(NULL)
,m_pVtxCamera(NULL)
,m_bEnable(false)
// 垂直和水平倾斜角度
,m_fVerticalLeanAmount(0)
,m_fHorizonLeanAmount(0)

,m_fVerticalDamping(0)
,m_fHorizonDamping(0)

// 目标垂直和水平倾斜角度
,m_fDstVerticalLeanAmount(0)
,m_fDstHorizonLeanAmount(0)

// 屏幕震动计时器
,m_fWekerTimer(0)
{

}
//-----------------------------------------------------------------------
gkVtxLayerManager::~gkVtxLayerManager()
{
	SAFE_DELETE( m_pVtxRoot );
}
//-----------------------------------------------------------------------
void gkVtxLayerManager::initializeVtxSystem()
{
	// vtx启动
	//	/*
	// start vektrix
	m_pVtxRoot = new vtx::Root();

	// vektrix plugins
	VTX_LOAD_PLUGIN(vektrix_AS3Plugin);
	VTX_LOAD_PLUGIN(vektrix_CairoPlugin);
	VTX_LOAD_PLUGIN(vektrix_FreeImgPlugin);
	VTX_LOAD_PLUGIN(vektrix_SwfPlugin);

	// gkVtxUI Module [7/23/2011 Kaiming-Laptop]
	VTX_LOAD_PLUGIN(gkVtxUI);

	// add vtx file container
	HRESULT hr = S_OK;
	// 先在目录内查找名字 [9/18/2010 Kaiming-Desktop]
	WCHAR wszPath[MAX_PATH] = _T("");

	hr = gkFindFileRelativeGame(wszPath, MAX_PATH, _T("swf_path.txt"));
	assert( hr == S_OK );

	WCHAR* strLastSlash = NULL;
	strLastSlash = wcsrchr( wszPath, TEXT( '\\' ) );

	if( strLastSlash )
		*strLastSlash = 0;

	CHAR szPath[MAX_PATH] = "";
	int i = WideCharToMultiByte(CP_ACP, 0, wszPath, -1, szPath, MAX_PATH, NULL, NULL);

	//vtx::FileManager::getSingletonPtr()->addFileContainer("");
	vtx::FileManager::getSingletonPtr()->addFileContainer(szPath);


	// 提请一个VTX渲染层
	IRenderTarget* pVtxOverlay = gEnv->pRenderer->createRenderTexture(_T("vtxOverlay"), IRenderTarget::GK_RTT_TYPE_CM_CUSTOM);
	// 提请一个camera
	m_pVtxCamera = getSceneMngPtr()->createCamera(_T("vtxCamera"));
	m_pVtxCamera->setNearClipDistance(100);
	m_pVtxCamera->setFarClipDistance(2000);
	m_pVtxCamera->setHideMask(GK_CAMERA_VIEWMASK_OVERLAY);
	pVtxOverlay->setCamera(m_pVtxCamera);

	m_bEnable = true;
}

//-----------------------------------------------------------------------
gkSceneNode* gkVtxLayerManager::createMovie( const std::string& name, const std::string& filename, const std::string& factoryname, gkVtxMovieListener* pMovieListener )
{
	if(!m_pVtxRoot)
		return NULL;
	vtx::gkMovableMovie* pVtxMovie = (vtx::gkMovableMovie*)m_pVtxRoot->createMovie(name, filename, factoryname, pMovieListener);

	pVtxMovie->setRenderLayer(RENDER_LAYER_HUDUI);
	BYTE yHideMask = 0;
	yHideMask |= GK_CAMERA_VIEWMASK_OVERLAY;
	pVtxMovie->setHideMask(yHideMask);
	//pVtxMovie->setVisible(false);

	getSceneMngPtr()->getRootSceneNode()->createChildSceneNode()->attachObject(pVtxMovie);

	std::pair<gkVtxMovieMap::iterator, bool> resultHandle = m_mapVtxMovies.insert(gkVtxMovieMap::value_type(name.c_str(), pVtxMovie));
	if (!resultHandle.second)
	{
		OutputDebugString(_T("++++gkVtxLayerManager.cpp: a gkVtxMovie has already exist!\n"));
		return(resultHandle.first->second->getParentSceneNode());
	}

	pVtxMovie->play();

	return pVtxMovie->getParentSceneNode();
}
//-----------------------------------------------------------------------
void gkVtxLayerManager::OnFrameMove( float fElapsedTime,  POINT ptMousePos, bool isMouseLeft )
{
	if (m_pVtxRoot)
	{
		m_pVtxRoot->update(fElapsedTime);
		gkVtxMovieMap::iterator itMovie = m_mapVtxMovies.begin();
		for (; itMovie != m_mapVtxMovies.end(); ++itMovie)
		{
			float width = 0;
			float height = 0;
			vtx::gkMovableMovie* pMovie = itMovie->second;
			if (pMovie->getFile()/* && !pMovie->getVisible()*/)
			{
				width = (float)pMovie->getFile()->getHeader().width;
				height = (float)pMovie->getFile()->getHeader().height;

				pMovie->setMouseAbs(ptMousePos.x, ptMousePos.y);

				static bool bMouseLeftPrev = isMouseLeft;
				if (bMouseLeftPrev != isMouseLeft)
				{
					if (isMouseLeft)
					{
						pMovie->mouseDown();
					}
					else
					{
						pMovie->mouseUp();
					}
					bMouseLeftPrev = isMouseLeft;
				}



				static bool firstTime = true;
				if(firstTime)
				{
					// zoom to height
					pMovie->getParentSceneNode()->setPosition(-width/2.0f, height/2.0f,0);

					m_pVtxCamera->setPosition(0,0,-(float)height*0.5f/tanf(m_pVtxCamera->getFOVy()*0.5f));

					m_pVtxCamera->setNearClipDistance((float)height*0.5f/tanf(m_pVtxCamera->getFOVy()*0.5f) - 500);
					m_pVtxCamera->setFarClipDistance((float)height*0.5f/tanf(m_pVtxCamera->getFOVy()*0.5f) + 500);

					firstTime = false;
				}

			}

			if (m_fWekerTimer > 0)
			{
				float leanH = 0.1f * m_fWekerAmount * sinf(m_fWekerTimer * m_fWekerFerq);
				float leanV = 0.1f * m_fWekerAmount * cosf(m_fWekerTimer * m_fWekerFerq);

				m_fDstHorizonLeanAmount = leanH;
				m_fDstVerticalLeanAmount = leanV;

				m_fHorizonDamping = 1.0f;
				m_fVerticalDamping = 1.0f;
				
				m_fWekerTimer -= fElapsedTime;
				// <0 结束操作

				if (m_fWekerTimer < 0)
				{
					m_fDstHorizonLeanAmount = 0;
					m_fDstVerticalLeanAmount = 0;
					m_fHorizonDamping = 1.0f;
					m_fVerticalDamping = 1.0f;
				}

			}
			

			bool bNeedUpdate = false;
			// lean manage
			if (m_fDstHorizonLeanAmount != m_fHorizonLeanAmount)
			{
				bNeedUpdate = true;
				m_fHorizonLeanAmount += (m_fDstHorizonLeanAmount - m_fHorizonLeanAmount) * m_fHorizonDamping;
				if (fabsf(m_fDstHorizonLeanAmount - m_fHorizonLeanAmount) < 0.01f)
				{
					m_fHorizonLeanAmount = m_fDstHorizonLeanAmount;
				}
			}

			if (m_fDstVerticalLeanAmount != m_fVerticalLeanAmount)
			{
				bNeedUpdate = true;
				m_fVerticalLeanAmount += (m_fDstVerticalLeanAmount - m_fVerticalLeanAmount) * m_fVerticalDamping;
				if (fabsf(m_fDstVerticalLeanAmount - m_fVerticalLeanAmount) < 0.01f)
				{
					m_fVerticalLeanAmount = m_fDstVerticalLeanAmount;
				}
			}


			if (bNeedUpdate)
			{
				pMovie->getParentSceneNode()->setOrientation(1,0,0,0);
				//pMovie->getParentSceneNode()->setPosition(0,0,0);
				pMovie->getParentSceneNode()->rotate(Vec3(0,1,0), m_fHorizonLeanAmount, gkTransformSpace::TS_PARENT );
				float trans = sinf(m_fHorizonLeanAmount) * width * 0.5;
				pMovie->getParentSceneNode()->rotate(Vec3(1,0,0), m_fVerticalLeanAmount, gkTransformSpace::TS_PARENT );
				trans += sinf(m_fVerticalLeanAmount) * height * 0.5;
				pMovie->getParentSceneNode()->setPosition(-width/2.0f, height/2.0f,trans);
				//pMovie->getParentSceneNode()->translate(0,0,-trans, gkNode::TS_PARENT);
			}
		}

	}
}
//-----------------------------------------------------------------------
void gkVtxLayerManager::setVerticalLean( float fAmount, float fDamping /*= 0.1f*/ )
{
	m_fDstVerticalLeanAmount = fAmount;
	m_fVerticalDamping = fDamping;
}
//-----------------------------------------------------------------------
void gkVtxLayerManager::setHorizonLean( float fAmount, float fDamping /*= 0.1f*/ )
{
	m_fDstHorizonLeanAmount = fAmount;
	m_fHorizonDamping = fDamping;
}
//-----------------------------------------------------------------------
void gkVtxLayerManager::setScreenWeker( float fDuration, float fFreq /*= 1.0f*/, float fAmount /*= 1.0f*/ )
{
	m_fWekerTimer = fDuration;
	m_fWekerFerq = fFreq;
	m_fWekerAmount = fAmount;
}
//-----------------------------------------------------------------------
void gkVtxLayerManager::destroyMovie( const std::string& name )
{
	if(m_pVtxRoot)
	{
		m_pVtxRoot->destroyMovie(name);
	}
	gkVtxMovieMap::iterator it = m_mapVtxMovies.find(name);
	if (it != m_mapVtxMovies.end())
	{
		m_mapVtxMovies.erase(it);
	}
}
//-----------------------------------------------------------------------
gkSceneNode* gkVtxLayerManager::getMovie( const std::string& name )
{
	gkVtxMovieMap::iterator it = m_mapVtxMovies.find(name);
	if (it != m_mapVtxMovies.end())
	{
		return it->second->getParentSceneNode();
	}
	else
	{
		return NULL;
	}
}

#endif
#endif