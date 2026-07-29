#pragma once

#include "IResourceManager.h"
#include "ITexture.h"
#include "IMesh.h"
#include "IMaterial.h"
#include "IShader.h"
#include "gkRenderOperation.h"

#include <map>
#include <set>
#include <string>

struct CRapidXmlParseNode;

class gkVkTexture : public ITexture
{
public:
	gkVkTexture(IResourceManager* creator, const gkStdString& name,
		gkResourceHandle handle, const gkStdString& group, gkNameValuePairList* params);
	virtual ~gkVkTexture();

	virtual void Apply(uint32 channel, uint8 filter);
	virtual uint32 getHeight();
	virtual uint32 getWidth();
	virtual uint32 getMipLevel();
	virtual void onReset();
	virtual void onLost();
	virtual float Tex2DRAW(const Vec2& texcoord, int filter = 1);
	virtual uint8* RawData();
	virtual void changeAttr(const gkStdString& key, const gkStdString& value);
	virtual const gkStdString& getAttr(const gkStdString& key) const;
	virtual bool lock(gkLockOperation& lockopt);
	virtual void unlock();
	virtual bool dynamic();
	virtual bool sizable();
	virtual ETexutureFormat format();
	virtual uint64 revision() const { return m_revision; }

protected:
	virtual bool loadImpl();
	virtual bool unloadImpl();

private:
	void resolveDimensions();

	gkNameValuePairList m_params;
	uint32 m_width;
	uint32 m_height;
	uint32 m_mips;
	ETexutureFormat m_format;
	bool m_dynamic;
	bool m_sizable;
	uint64 m_revision;
	std::vector<uint8> m_rawData;
};

class gkVkMesh : public IMesh
{
public:
	gkVkMesh(IResourceManager* creator, const gkStdString& name,
		gkResourceHandle handle, const gkStdString& group, gkNameValuePairList* params);
	virtual ~gkVkMesh();

	virtual void onReset();
	virtual void onLost();
	virtual BoneBaseTransforms& GetBoneBaseTransforms();
	virtual AABB& GetAABB();
	virtual void getRenderOperation(gkRenderOperation& op, uint32 subset = 0);
	virtual bool RaycastMesh(Ray& ray, std::vector<float>& dists);
	virtual bool RaycastMesh_WorldSpace(Ray& ray, Matrix34& world, std::vector<float>& dists);
	virtual uint32 getSubsetCount();
	virtual gkVertexBuffer* getVertexBuffer();
	virtual gkIndexBuffer* getIndexBuffer();
	virtual void UpdateHwBuffer();
	virtual void ReleaseSysBuffer();
	virtual void modifyCustomPrimitiveCount(uint32 count);

protected:
	virtual bool loadImpl();
	virtual bool unloadImpl();

private:
	bool loadEngineMesh();

	gkNameValuePairList m_params;
	gkVertexBuffer* m_vertexBuffer;
	gkIndexBuffer* m_indexBuffer;
	gkMeshSubsets m_subsets;
	BoneBaseTransforms m_bones;
	AABB m_aabb;
	uint32 m_customPrimitiveCount;
	bool m_ready;
};

class gkVkShader : public IShader
{
public:
	gkVkShader(IResourceManager* creator, const gkStdString& name,
		gkResourceHandle handle, const gkStdString& group);

	virtual uint32 getDefaultRenderLayer();
	virtual void onReset();
	virtual void onLost();
	virtual void FX_SetTechniqueByName(LPCSTR name);
	virtual void FX_SetTechnique(GKHANDLE technique);
	virtual GKHANDLE FX_GetTechniqueByName(LPCSTR name);
	virtual GKHANDLE FX_GetTechnique(EShaderInternalTechnique technique);
	virtual void FX_SetValue(GKHANDLE parameter, const void* data, uint32 bytes);
	virtual void FX_SetFloat(GKHANDLE parameter, float data);
	virtual void FX_SetFloat3(GKHANDLE parameter, const Vec3& data);
	virtual void FX_SetFloat4(GKHANDLE parameter, const Vec4& data);
	virtual void FX_SetMatrixArray(GKHANDLE parameter, D3DXMATRIX* data, uint32 size);
	virtual void FX_SetMatrix(GKHANDLE parameter, const Matrix44& data);
	virtual void FX_Begin(uint32* passes, DWORD flag);
	virtual void FX_BeginPass(uint32 pass);
	virtual void FX_Commit();
	virtual void FX_EndPass();
	virtual void FX_End();
	virtual gkMaterialParams* getShaderDefaultParams();
	virtual ShaderMacros& getShaderMarcos();
	virtual uint32 getShaderMarcoMask();
	virtual void switchSystemMacro(uint32 systemMacro = 0);

protected:
	virtual bool loadImpl();
	virtual bool unloadImpl();

private:
	gkMaterialParams m_defaultParams;
	ShaderMacros m_macros;
	uint32 m_systemMacro;
	uint32 m_materialMacro;
	GKHANDLE m_technique;
	std::set<std::string> m_internedNames;
	std::map<std::string, std::vector<uint8> > m_parameterValues;
};

class gkVkMaterial : public IMaterial
{
public:
	gkVkMaterial(IResourceManager* creator, const gkStdString& name,
		gkResourceHandle handle, const gkStdString& group, gkNameValuePairList* params);
	virtual ~gkVkMaterial();

	virtual gkShaderPtr& getShader();
	virtual gkTexturePtr& getTexture(EMaterialSlot index);
	virtual void setTexture(gkTexturePtr& texture, BYTE index = 0);
	virtual void ApplyParameterBlock(bool texture = true, IShader* shader = NULL);
	virtual void BasicApply(IShader* shader);
	virtual Vec2& getUVTill();
	virtual bool isDoubleSide();
	virtual float getGlossness();
	virtual int getOpacity();
	virtual void setUVTill(Vec2 till);
	virtual void setUVMovement(float speed);
	virtual bool& getSSRL();
	virtual bool& getCastShadow();
	virtual gkMaterialParams* getLoadingParameterBlock();
	virtual void saveAsMtlFile(const TCHAR* filename);
	virtual void saveToMtlFile();
	virtual uint8 getSubMtlCount();
	virtual IMaterial* getSubMaterial(uint8 index);
	virtual const gkStdString& getShaderName();
	virtual ShaderMacros& getShaderMarcos();
	virtual uint32 getShaderMarcoMask();
	const ColorF& diffuseColor() const { return m_diffuseColor; }

protected:
	virtual bool loadImpl();
	virtual bool unloadImpl();

private:
	bool loadMaterialNode(CRapidXmlParseNode* node);

	gkNameValuePairList m_params;
	gkShaderPtr m_shader;
	gkTexturePtr m_textures[eMS_Invalid];
	gkMaterialParams m_materialParams;
	Vec2 m_uvTiling;
	float m_uvSpeed;
	float m_glossness;
	int m_opacity;
	bool m_doubleSided;
	bool m_ssrl;
	bool m_castShadow;
	gkStdString m_shaderName;
	ShaderMacros m_macros;
	uint32 m_macroMask;
	ColorF m_diffuseColor;
	std::vector<gkMaterialPtr> m_subMaterials;
};

class gkVkTextureManager : public IResourceManager
{
protected:
	virtual IResource* createImpl(const gkStdString& name, gkResourceHandle handle,
		const gkStdString& group, gkNameValuePairList* params = NULL);
};

class gkVkMeshManager : public IResourceManager
{
protected:
	virtual IResource* createImpl(const gkStdString& name, gkResourceHandle handle,
		const gkStdString& group, gkNameValuePairList* params = NULL);
};

class gkVkShaderManager : public IResourceManager
{
protected:
	virtual IResource* createImpl(const gkStdString& name, gkResourceHandle handle,
		const gkStdString& group, gkNameValuePairList* params = NULL);
};

class gkVkMaterialManager : public IResourceManager
{
protected:
	virtual IResource* createImpl(const gkStdString& name, gkResourceHandle handle,
		const gkStdString& group, gkNameValuePairList* params = NULL);
};
