#include "gkStereoDeviceRenderable.h"
//#include "gkOculus.h"

gkStereoDeivceRenderable::gkStereoDeivceRenderable(void)
{
	m_aabb.Reset();
//	m_eye_index = 0;
}

gkStereoDeivceRenderable::~gkStereoDeivceRenderable(void)
{
}

void gkStereoDeivceRenderable::getWorldTransforms(Matrix44*) const
{
	//throw std::logic_error("The method or operation is not implemented.");
}

IMaterial* gkStereoDeivceRenderable::getMaterial()
{
	return m_material.getPointer();
}

void gkStereoDeivceRenderable::setMaterialName(const gkStdString& matName)
{
	
}

const gkStdString& gkStereoDeivceRenderable::getMaterialName() const
{
	return _T("StereoDeviceMaterial");
}

AABB& gkStereoDeivceRenderable::getAABB()
{
	return m_aabb;
}

void gkStereoDeivceRenderable::getRenderOperation(gkRenderOperation& op)
{
	m_mesh->getRenderOperation( op );
}

gkMeshPtr& gkStereoDeivceRenderable::getMesh()
{
	throw std::logic_error("The method or operation is not implemented.");
}

void gkStereoDeivceRenderable::RP_Prepare()
{
	m_mesh->UpdateHwBuffer();

}

void gkStereoDeivceRenderable::RT_Prepare()
{
	
}

void gkStereoDeivceRenderable::RP_ShaderSet()
{
// 	m_material->getShader()->FX_SetFloat2( "EyeToSourceUVScale", Vec2(UVScaleOffset[m_eye_index][0].x, UVScaleOffset[m_eye_index][0].y));
// 	m_material->getShader()->FX_SetFloat2( "EyeToSourceUVOffset",  Vec2(UVScaleOffset[m_eye_index][1].x, UVScaleOffset[m_eye_index][1].y));
// 	ovrMatrix4f timeWarpMatrices[2];
// 	ovrHmd_GetEyeTimewarpMatrices(HMD, (ovrEyeType)m_eye_index, m_oculusDevice->eyeRenderPose[m_eye_index], timeWarpMatrices);
// 	m_material->getShader()->FX_SetValue( "EyeRotationStart", &(timeWarpMatrices[0]), sizeof(ovrMatrix4f)  );
// 	m_material->getShader()->FX_SetValue( "EyeRotationEnd", &(timeWarpMatrices[1]), sizeof(ovrMatrix4f) );
}
