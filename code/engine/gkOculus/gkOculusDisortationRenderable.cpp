#include "gkOculusDisortationRenderable.h"
#include "gkOculus.h"

gkOculusDisortationRenderable::gkOculusDisortationRenderable(gkOculus* oculus)
{
	m_aabb.Reset();
	m_eye_index = 0;
	m_oculusDevice = oculus;
}

gkOculusDisortationRenderable::~gkOculusDisortationRenderable(void)
{
}

void gkOculusDisortationRenderable::getWorldTransforms(Matrix44*) const
{
	//throw std::logic_error("The method or operation is not implemented.");
}

IMaterial* gkOculusDisortationRenderable::getMaterial()
{
	return m_material.getPointer();
}

void gkOculusDisortationRenderable::setMaterialName(const gkStdString& matName)
{
	
}

const gkStdString& gkOculusDisortationRenderable::getMaterialName() const
{
	return _T("OculusDisortationMaterial");
}

AABB& gkOculusDisortationRenderable::getAABB()
{
	return m_aabb;
}

void gkOculusDisortationRenderable::getRenderOperation(gkRenderOperation& op)
{
	m_mesh->getRenderOperation( op );
}

gkMeshPtr& gkOculusDisortationRenderable::getMesh()
{
	throw std::logic_error("The method or operation is not implemented.");
}

void gkOculusDisortationRenderable::RP_Prepare()
{
	m_mesh->UpdateHwBuffer();

}

void gkOculusDisortationRenderable::RT_Prepare()
{
	
}

void gkOculusDisortationRenderable::RP_ShaderSet()
{
	m_material->getShader()->FX_SetFloat2( "EyeToSourceUVScale", Vec2(UVScaleOffset[m_eye_index][0].x, UVScaleOffset[m_eye_index][0].y));
	m_material->getShader()->FX_SetFloat2( "EyeToSourceUVOffset",  Vec2(UVScaleOffset[m_eye_index][1].x, UVScaleOffset[m_eye_index][1].y));
	ovrMatrix4f timeWarpMatrices[2];
	ovrHmd_GetEyeTimewarpMatrices(HMD, (ovrEyeType)m_eye_index, m_oculusDevice->eyeRenderPose[m_eye_index], timeWarpMatrices);
	m_material->getShader()->FX_SetValue( "EyeRotationStart", &(timeWarpMatrices[0]), sizeof(ovrMatrix4f)  );
	m_material->getShader()->FX_SetValue( "EyeRotationEnd", &(timeWarpMatrices[1]), sizeof(ovrMatrix4f) );
}
