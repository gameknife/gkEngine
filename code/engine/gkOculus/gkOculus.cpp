#include "gkOculus.h"
#include "gkOculusDisortationRenderable.h"
#include "IRenderer.h"
#include "IRenderSequence.h"
#include "IResourceManager.h"
#include "I3DEngine.h"
#include "ICamera.h"

#ifdef _DEBUG
#pragma comment(lib,"..\\..\\thirdparty\\libovr\\Lib\\Win32\\VS2010\\libovrd.lib")
#else
#pragma comment(lib,"..\\..\\thirdparty\\libovr\\Lib\\Win32\\VS2010\\libovr.lib")
#endif

#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"winmm.lib")
ovrHmd HMD = 0;

void gkOculus::Init()
{
	ovr_Initialize();

	if (!HMD)
	{
		HMD = ovrHmd_Create(0);
		if (!HMD)
		{
			//MessageBoxA(NULL, "Oculus Rift not detected.", "", MB_OK);
			//return(1);
			return;
		}
		if (HMD->ProductName[0] == '\0')
		{
			//MessageBoxA(NULL, "Rift detected, display not enabled.", "", MB_OK);
		
			return;
		}

		InitHMD();
	}
}

void gkOculus::OnFrameBegin()
{
	if(HMD)
	{
		ovrHmd_BeginFrameTiming(HMD, 0);
	}
	else
	{
		HMD = ovrHmd_Create(0, 0.01f);
		if (!HMD)
		{
			//MessageBoxA(NULL, "Oculus Rift not detected.", "", MB_OK);
			//return(1);
			return;
		}
		if (HMD->ProductName[0] == '\0')
		{
			//MessageBoxA(NULL, "Rift detected, display not enabled.", "", MB_OK);

			return;
		}

		InitHMD();
	}
	
}

void gkOculus::OnFrameEnd()
{
	if(HMD)
	{
		ovrHmd_EndFrameTiming(HMD);
	}

	
}

void gkOculus::Destory()
{
	HMD = 0;

	ovrHmd_Destroy(HMD);
	HMD = 0;
	// No OVR functions involving memory are allowed after this.
	ovr_Shutdown(); 
}

void gkOculus::QueryHeadStatus(IStereoHeadStatus& out)
{

}

void gkOculus::Flush()
{
	if(HMD)
	{
		// OVERRIDE MAIN CAMERA

		ICamera* cam = gEnv->p3DEngine->getMainCamera();

		static ovrTrackingState HmdState;

		ovrVector3f hmdToEyeViewOffset[2] = { EyeRenderDesc[0].HmdToEyeViewOffset, EyeRenderDesc[1].HmdToEyeViewOffset };
		ovrHmd_GetEyePoses(HMD, 0, hmdToEyeViewOffset, eyeRenderPose, &HmdState);

		Quat camOrientation;
		camOrientation.v.x = eyeRenderPose->Orientation.x;
		camOrientation.v.y = -eyeRenderPose->Orientation.z;
		camOrientation.v.z = eyeRenderPose->Orientation.y;
		camOrientation.w = eyeRenderPose->Orientation.w;

		cam->setAdditionalOrientation( camOrientation );

		Quat dRot = cam->getDerivedOrientation();

		Vec3 eyeOffsetL = Vec3(eyeRenderPose[0].Position.x, -eyeRenderPose[0].Position.z, eyeRenderPose[0].Position.y);
		Vec3 eyeOffsetR = Vec3(eyeRenderPose[1].Position.x, -eyeRenderPose[1].Position.z, eyeRenderPose[1].Position.y);

		cam->setStereoOffset( dRot * eyeOffsetL, dRot * eyeOffsetR );
		
		Matrix44 leftMat,rightMat;
		
		OVR::Matrix4f projLeft;
		OVR::Matrix4f projRight;

		projLeft = ovrMatrix4f_Projection(EyeRenderDesc[0].Fov, 0.01f, 10000.0f, true);
		projRight = ovrMatrix4f_Projection(EyeRenderDesc[1].Fov, 0.01f, 10000.0f, true);

		leftMat = *((Matrix44*)(&projLeft));
		rightMat = *((Matrix44*)(&projRight));

		leftMat.Transpose();
		rightMat.Transpose();

		cam->setStereoProjMatrix( leftMat, rightMat );

		IRenderSequence* rs = gEnv->pRenderer->RT_GetRenderSequence();
		if (rs)
		{
			rs->addToRenderSequence( m_disortation_renderable_eyes[0], RENDER_LAYER_STEREO_DEVICE );
			rs->addToRenderSequence( m_disortation_renderable_eyes[1], RENDER_LAYER_STEREO_DEVICE );
		}
	}

}

void gkOculus::InitHMD()
{
	ovrHmd_SetEnabledCaps(HMD, ovrHmdCap_LowPersistence);

	// Start the sensor which informs of the Rift's pose and motion
	ovrHmd_ConfigureTracking(HMD,   ovrTrackingCap_Orientation |
		ovrTrackingCap_MagYawCorrection |
		ovrTrackingCap_Position, 0);

	OVR::Sizei recommenedTex0Size = ovrHmd_GetFovTextureSize(HMD, ovrEye_Left,  HMD->DefaultEyeFov[0], 1.0f);
	OVR::Sizei recommenedTex1Size = ovrHmd_GetFovTextureSize(HMD, ovrEye_Right, HMD->DefaultEyeFov[1], 1.0f);

	gEnv->pRenderer->SetOverrideSize( recommenedTex0Size.w, recommenedTex0Size.h, true );

	gkTexturePtr tex = gEnv->pSystem->getTextureMngPtr()->getByName(_T("RT_BACKBUFFER_STEREOOUT"));

	for (int eyenum=0; eyenum < 2; ++eyenum)
	{
		m_disortation_renderable_eyes[eyenum] = new gkOculusDisortationRenderable(this);
		m_disortation_renderable_eyes[eyenum]->m_eye_index = eyenum;
		m_disortation_renderable_eyes[eyenum]->HMD = HMD;

		gkNameValuePairList createlist;

		createlist[_T("file")] = _T("engine/assets/meshs/oculus_disort.mtl");

		TCHAR buffer[255];
		_stprintf( buffer, _T("$OculusDisortation_%d"), eyenum );

		gkMaterialPtr mat = gEnv->pSystem->getMaterialMngPtr()->create( buffer, _T("stereo"), &createlist );
		mat->load();
		mat->setTexture( tex , 0);


		m_disortation_renderable_eyes[eyenum]->m_material = mat;

		_stprintf( buffer, _T("$OculusDisortationMesh_%d"), eyenum );

		createlist[_T("type")] =	_T("Pt2T2T2T2T2");
		gkMeshPtr mesh = gEnv->pSystem->getMeshMngPtr()->create( buffer, _T("stereo"), &createlist );
		mesh->load();
		m_disortation_renderable_eyes[eyenum]->m_mesh = mesh;

		ovrFovPort eyeFov[2] = { HMD->DefaultEyeFov[0], HMD->DefaultEyeFov[1] } ;

		float fovy = atan( eyeFov[0].UpTan ) * 2;
		gEnv->p3DEngine->getMainCamera()->setFOVy( fovy );

		ovrDistortionMesh meshData;
		ovrHmd_CreateDistortionMesh(HMD, (ovrEyeType)eyenum, eyeFov[eyenum],
			ovrDistortionCap_Chromatic | ovrDistortionCap_TimeWarp, &meshData);

		mesh->getVertexBuffer()->resizeDiscard( meshData.VertexCount );
		mesh->getIndexBuffer()->resizeDiscard( meshData.IndexCount );

		memcpy( mesh->getVertexBuffer()->data, meshData.pVertexData, meshData.VertexCount * sizeof(ovrDistortionVertex) );
		memcpy( mesh->getIndexBuffer()->data, meshData.pIndexData, meshData.IndexCount * sizeof(unsigned short) );

		ovrHmd_DestroyDistortionMesh( &meshData );

		//Create eye render description for use later
		EyeRenderDesc[eyenum] = ovrHmd_GetRenderDesc(HMD, (ovrEyeType)eyenum,  eyeFov[eyenum]);

		//Do scale and offset
		OVR::Sizei RenderTargetSize;
		RenderTargetSize.w = recommenedTex0Size.w;
		RenderTargetSize.h = recommenedTex1Size.h;

		ovrRecti EyeRenderViewport[2];
		EyeRenderViewport[0].Pos  = OVR::Vector2i(0,0);
		EyeRenderViewport[0].Size = OVR::Sizei(RenderTargetSize.w / 2, RenderTargetSize.h);
		EyeRenderViewport[1].Pos  = OVR::Vector2i((RenderTargetSize.w + 1) / 2, 0);
		EyeRenderViewport[1].Size = EyeRenderViewport[0].Size;

		ovrHmd_GetRenderScaleAndOffset(eyeFov[eyenum],RenderTargetSize, EyeRenderViewport[eyenum], m_disortation_renderable_eyes[eyenum]->UVScaleOffset[eyenum]);
	}
}

bool gkOculus::Avaliable()
{
	return HMD;
}

