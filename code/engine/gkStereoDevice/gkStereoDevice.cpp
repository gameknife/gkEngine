#include "gkStereoDevice.h"
#include "gkStereoDeviceRenderable.h"
#include "IRenderer.h"
#include "IRenderSequence.h"
#include "IResourceManager.h"
#include "I3DEngine.h"
#include "ICamera.h"

void gkStereoDevice::Init()
{
	gkTexturePtr tex = gEnv->pSystem->getTextureMngPtr()->getByName(_T("RT_BACKBUFFER_STEREOOUT"));

	m_stereo_renderer = new gkStereoDeivceRenderable();

	gkNameValuePairList createlist;

	createlist[_T("file")] = _T("engine/assets/meshs/stereo_general.mtl");

	TCHAR buffer[255];
	_stprintf( buffer, _T("$StereoGeneral") );

	gkMaterialPtr mat = gEnv->pSystem->getMaterialMngPtr()->create( buffer, _T("stereo"), &createlist );
	mat->load();
	mat->setTexture( tex , 0);


	m_stereo_renderer->m_material = mat;

	_stprintf( buffer, _T("$StereoGeneralMesh") );

	createlist[_T("type")] =	_T("Pt2T2T2T2T2");
	gkMeshPtr mesh = gEnv->pSystem->getMeshMngPtr()->create( buffer, _T("stereo"), &createlist );
	mesh->load();
	m_stereo_renderer->m_mesh = mesh;

	mesh->getVertexBuffer()->resizeDiscard( 4 );
	mesh->getIndexBuffer()->resizeDiscard( 6 );

	GKVL_Pt2T2T2T2T2* vt_start = (GKVL_Pt2T2T2T2T2*)( mesh->getVertexBuffer()->data );

	vt_start[0].position = Vec2(-1,1);
	vt_start[0].timewarp_vig = Vec2(0,0);

	vt_start[1].position = Vec2(-1,-1);
	vt_start[1].timewarp_vig = Vec2(0,1);

	vt_start[2].position = Vec2(1,1);
	vt_start[2].timewarp_vig = Vec2(1,0);

	vt_start[3].position = Vec2(1,-1);
	vt_start[3].timewarp_vig = Vec2(1,1);

	mesh->getIndexBuffer()->Clear();

	mesh->getIndexBuffer()->push_back( 0 );
	mesh->getIndexBuffer()->push_back( 1 );
	mesh->getIndexBuffer()->push_back( 2 );

	mesh->getIndexBuffer()->push_back( 2 );
	mesh->getIndexBuffer()->push_back( 1 );
	mesh->getIndexBuffer()->push_back( 3 );

	//gEnv->pRenderer->SetOverrideSize( 960, 1080, true );

}

void gkStereoDevice::OnFrameBegin()
{

}

void gkStereoDevice::OnFrameEnd()
{

}

void gkStereoDevice::Destory()
{

}

void gkStereoDevice::QueryHeadStatus(IStereoHeadStatus& out)
{

}

void gkStereoDevice::Flush()
{
	ICamera* cam = gEnv->p3DEngine->getMainCamera();

	Quat dRot = cam->getDerivedOrientation();

	Vec3 eyeOffsetL = Vec3(-0.07,0,0);
	Vec3 eyeOffsetR = Vec3(0.07,0,0);

	cam->setStereoOffset( dRot * eyeOffsetL, dRot * eyeOffsetR );

	IRenderSequence* rs = gEnv->pRenderer->RT_GetRenderSequence();
	if (rs)
	{
		rs->addToRenderSequence( m_stereo_renderer, RENDER_LAYER_STEREO_DEVICE );
	}
}

bool gkStereoDevice::Avaliable()
{
	return true;
}
