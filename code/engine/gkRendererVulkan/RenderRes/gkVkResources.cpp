#include "gkVkResources.h"

#include "ISystem.h"
#include "IRenderer.h"
#include "IMeshLoader.h"
#include "IResFile.h"
#include "IXmlUtil.h"
#include "gkFilePath.h"

#include <algorithm>
#include <cmath>

namespace
{
	uint32 parseDimension(const gkStdString& value, uint32 fallback)
	{
		gkStdStringstream stream(value);
		uint32 result = fallback;
		stream >> result;
		return result;
	}

	bool decodeTga(const uint8* bytes, size_t size, uint32& width, uint32& height,
		std::vector<uint8>& rgba)
	{
		if (!bytes || size < 18)
			return false;
		const uint8 idLength = bytes[0];
		const uint8 imageType = bytes[2];
		width = static_cast<uint32>(bytes[12] | (bytes[13] << 8));
		height = static_cast<uint32>(bytes[14] | (bytes[15] << 8));
		const uint8 bitsPerPixel = bytes[16];
		if (!width || !height || (bitsPerPixel != 24 && bitsPerPixel != 32) ||
			(imageType != 2 && imageType != 10))
			return false;
		const size_t pixelBytes = bitsPerPixel / 8;
		size_t cursor = 18 + idLength;
		const size_t pixelCount = static_cast<size_t>(width) * height;
		rgba.resize(pixelCount * 4);
		size_t outputPixel = 0;
		while (outputPixel < pixelCount && cursor < size)
		{
			size_t runLength = 1;
			bool repeated = false;
			if (imageType == 10)
			{
				const uint8 packet = bytes[cursor++];
				runLength = (packet & 0x7f) + 1;
				repeated = (packet & 0x80) != 0;
			}
			for (size_t run = 0; run < runLength && outputPixel < pixelCount; ++run)
			{
				if (cursor + pixelBytes > size)
					return false;
				const uint8* source = bytes + cursor;
				const size_t sourcePixel = outputPixel;
				const uint32 sourceY = static_cast<uint32>(sourcePixel / width);
				const uint32 sourceX = static_cast<uint32>(sourcePixel % width);
				const bool topOrigin = (bytes[17] & 0x20) != 0;
				const uint32 destinationY = topOrigin ? sourceY : height - 1 - sourceY;
				uint8* destination = &rgba[
					(static_cast<size_t>(destinationY) * width + sourceX) * 4];
				destination[0] = source[2];
				destination[1] = source[1];
				destination[2] = source[0];
				destination[3] = pixelBytes == 4 ? source[3] : 255;
				++outputPixel;
				if (!repeated)
					cursor += pixelBytes;
			}
			if (repeated)
				cursor += pixelBytes;
		}
		return outputPixel == pixelCount;
	}

	uint8 expandMaskedChannel(uint32 pixel, uint32 mask, uint8 fallback)
	{
		if (!mask)
			return fallback;
		uint32 shift = 0;
		while (((mask >> shift) & 1u) == 0u)
			++shift;
		const uint32 valueMask = mask >> shift;
		const uint32 value = (pixel & mask) >> shift;
		return static_cast<uint8>((value * 255u + valueMask / 2u) / valueMask);
	}

	void decode565(uint16 value, uint8 color[3])
	{
		color[0] = static_cast<uint8>(((value >> 11) & 31) * 255 / 31);
		color[1] = static_cast<uint8>(((value >> 5) & 63) * 255 / 63);
		color[2] = static_cast<uint8>((value & 31) * 255 / 31);
	}

	bool decodeDds(const uint8* bytes, size_t size, uint32& width, uint32& height,
		std::vector<uint8>& rgba)
	{
		if (!bytes || size < 128 || memcmp(bytes, "DDS ", 4) != 0 ||
			*reinterpret_cast<const uint32*>(bytes + 4) != 124)
			return false;
		width = *reinterpret_cast<const uint32*>(bytes + 16);
		height = *reinterpret_cast<const uint32*>(bytes + 12);
		if (!width || !height)
			return false;
		const uint32 fourCC = *reinterpret_cast<const uint32*>(bytes + 84);
		const uint32 rgbBits = *reinterpret_cast<const uint32*>(bytes + 88);
		const uint8* source = bytes + 128;
		const size_t pixelCount = static_cast<size_t>(width) * height;
		rgba.assign(pixelCount * 4, 255);
		if (fourCC == 0 && rgbBits == 32)
		{
			if (size < 128 + pixelCount * 4)
				return false;
			const uint32 redMask = *reinterpret_cast<const uint32*>(bytes + 92);
			const uint32 greenMask = *reinterpret_cast<const uint32*>(bytes + 96);
			const uint32 blueMask = *reinterpret_cast<const uint32*>(bytes + 100);
			const uint32 alphaMask = *reinterpret_cast<const uint32*>(bytes + 104);
			for (size_t i = 0; i < pixelCount; ++i)
			{
				const uint32 pixel = *reinterpret_cast<const uint32*>(source + i * 4);
				rgba[i * 4 + 0] = expandMaskedChannel(pixel, redMask, 255);
				rgba[i * 4 + 1] = expandMaskedChannel(pixel, greenMask, 255);
				rgba[i * 4 + 2] = expandMaskedChannel(pixel, blueMask, 255);
				rgba[i * 4 + 3] = expandMaskedChannel(pixel, alphaMask, 255);
			}
			return true;
		}
		const uint32 dxt5 = static_cast<uint32>('D') |
			(static_cast<uint32>('X') << 8) |
			(static_cast<uint32>('T') << 16) |
			(static_cast<uint32>('5') << 24);
		if (fourCC != dxt5)
			return false;
		const uint32 blocksX = (width + 3) / 4;
		const uint32 blocksY = (height + 3) / 4;
		if (size < 128 + static_cast<size_t>(blocksX) * blocksY * 16)
			return false;
		for (uint32 blockY = 0; blockY < blocksY; ++blockY)
		{
			for (uint32 blockX = 0; blockX < blocksX; ++blockX, source += 16)
			{
				uint8 alpha[8] = { source[0], source[1] };
				if (alpha[0] > alpha[1])
					for (uint32 i = 1; i < 7; ++i)
						alpha[i + 1] = static_cast<uint8>(
							((7 - i) * alpha[0] + i * alpha[1]) / 7);
				else
				{
					for (uint32 i = 1; i < 5; ++i)
						alpha[i + 1] = static_cast<uint8>(
							((5 - i) * alpha[0] + i * alpha[1]) / 5);
					alpha[6] = 0;
					alpha[7] = 255;
				}
				uint64 alphaIndices = 0;
				for (uint32 i = 0; i < 6; ++i)
					alphaIndices |= static_cast<uint64>(source[2 + i]) << (8 * i);
				uint8 colors[4][3];
				decode565(*reinterpret_cast<const uint16*>(source + 8), colors[0]);
				decode565(*reinterpret_cast<const uint16*>(source + 10), colors[1]);
				for (uint32 channel = 0; channel < 3; ++channel)
				{
					colors[2][channel] = static_cast<uint8>(
						(2 * colors[0][channel] + colors[1][channel]) / 3);
					colors[3][channel] = static_cast<uint8>(
						(colors[0][channel] + 2 * colors[1][channel]) / 3);
				}
				const uint32 colorIndices =
					*reinterpret_cast<const uint32*>(source + 12);
				for (uint32 y = 0; y < 4; ++y)
					for (uint32 x = 0; x < 4; ++x)
					{
						const uint32 destinationX = blockX * 4 + x;
						const uint32 destinationY = blockY * 4 + y;
						if (destinationX >= width || destinationY >= height)
							continue;
						const uint32 index = y * 4 + x;
						uint8* destination = &rgba[
							(static_cast<size_t>(destinationY) * width + destinationX) * 4];
						const uint32 colorIndex = (colorIndices >> (2 * index)) & 3;
						destination[0] = colors[colorIndex][0];
						destination[1] = colors[colorIndex][1];
						destination[2] = colors[colorIndex][2];
						destination[3] = alpha[(alphaIndices >> (3 * index)) & 7];
					}
			}
		}
		return true;
	}

	bool hasExtension(const gkStdString& name, const TCHAR* extension)
	{
		return name.size() >= _tcslen(extension) &&
			!_tcsicmp(name.c_str() + name.size() - _tcslen(extension), extension);
	}

	bool decodeRawR8(const uint8* bytes, size_t size, uint32& width, uint32& height,
		std::vector<uint8>& pixels)
	{
		if (!bytes || !size)
			return false;
		const uint32 side = static_cast<uint32>(sqrt(static_cast<double>(size)));
		if (!side || static_cast<size_t>(side) * side != size)
			return false;
		width = side;
		height = side;
		pixels.assign(bytes, bytes + size);
		return true;
	}

	EMaterialSlot textureSlotFromParameter(const TCHAR* name)
	{
		if (!name)
			return eMS_Invalid;
		if (!_tcsicmp(name, _T("texDiffuse")) || !_tcsicmp(name, _T("g_DiffuseMap")))
			return eMS_Diffuse;
		if (!_tcsicmp(name, _T("texNormal")) || !_tcsicmp(name, _T("g_NormalMap")))
			return eMS_Normal;
		if (!_tcsicmp(name, _T("texSpecular")) || !_tcsicmp(name, _T("g_SpecularMap")))
			return eMS_Specular;
		if (!_tcsicmp(name, _T("texDetail")) || !_tcsicmp(name, _T("g_DetailMap")))
			return eMS_Detail;
		return eMS_Invalid;
	}
}

gkVkTexture::gkVkTexture(IResourceManager* creator, const gkStdString& name,
	gkResourceHandle handle, const gkStdString& group, gkNameValuePairList* params)
	: ITexture(creator, name, handle, group)
	, m_width(1)
	, m_height(1)
	, m_mips(1)
	, m_format(eTF_RGBA8)
	, m_dynamic(false)
	, m_sizable(false)
	, m_revision(0)
{
	if (params)
		m_params = *params;
}

gkVkTexture::~gkVkTexture()
{
	if (isLoaded())
		unload();
}

void gkVkTexture::resolveDimensions()
{
	gkNameValuePairList::const_iterator size = m_params.find(_T("size"));
	if (size != m_params.end())
	{
		if (size->second == _T("full"))
		{
			m_width = gEnv->pRenderer->GetScreenWidth();
			m_height = gEnv->pRenderer->GetScreenHeight();
			m_sizable = true;
		}
		else if (size->second == _T("half"))
		{
			m_width = (std::max)(1u, gEnv->pRenderer->GetScreenWidth() / 2);
			m_height = (std::max)(1u, gEnv->pRenderer->GetScreenHeight() / 2);
			m_sizable = true;
		}
		else
		{
			m_width = parseDimension(size->second, 1);
			m_height = m_width;
		}
	}
	gkNameValuePairList::const_iterator sizeY = m_params.find(_T("sizey"));
	if (sizeY != m_params.end())
		m_height = parseDimension(sizeY->second, m_height);
	gkNameValuePairList::const_iterator formatValue = m_params.find(_T("format"));
	if (formatValue != m_params.end())
	{
		m_format = getTextureFormat(formatValue->second);
		if (m_format == eTF_UnKnown && formatValue->second == _T("A8"))
			m_format = eTF_A8;
	}
	gkNameValuePairList::const_iterator usage = m_params.find(_T("usage"));
	m_dynamic = usage != m_params.end();
}

bool gkVkTexture::loadImpl()
{
	resolveDimensions();
	const bool rawTexture = hasExtension(m_wstrFileName, _T(".raw"));
	if (m_params.empty() && gEnv && gEnv->pFileSystem)
	{
		IResFile* file = gEnv->pFileSystem->loadResFile(m_wstrFileName.c_str(), true);
		if (file)
		{
			const uint8* bytes = static_cast<const uint8*>(file->DataPtr());
			const bool decoded = rawTexture ?
				decodeRawR8(bytes, file->Size(), m_width, m_height, m_rawData) :
				(decodeTga(bytes, file->Size(), m_width, m_height, m_rawData) ||
					decodeDds(bytes, file->Size(), m_width, m_height, m_rawData));
			gEnv->pFileSystem->closeResFile(file);
			if (decoded)
			{
				m_format = rawTexture ? eTF_R8 : eTF_RGBA8;
				m_nSize = static_cast<uint32>(m_rawData.size());
				return true;
			}
		}
	}
	// Legacy terrain test packs are optional. A missing RAW map must be a flat,
	// empty field; treating it as the generic white texture puts the terrain
	// 80 metres above the testcase camera and creates maximum vegetation.
	if (rawTexture)
	{
		m_width = 512;
		m_height = 512;
		m_format = eTF_R8;
		m_rawData.assign(static_cast<size_t>(m_width) * m_height, 0);
		m_nSize = static_cast<uint32>(m_rawData.size());
		return true;
	}
	const uint32 bytesPerPixel = (m_format == eTF_A8 || m_format == eTF_R8) ? 1 : 4;
	m_rawData.assign(static_cast<size_t>(m_width) * m_height * bytesPerPixel, 0xff);
	m_nSize = static_cast<uint32>(m_rawData.size());
	return true;
}

bool gkVkTexture::unloadImpl()
{
	m_rawData.clear();
	return true;
}

void gkVkTexture::Apply(uint32, uint8) {}
uint32 gkVkTexture::getHeight() { return m_height; }
uint32 gkVkTexture::getWidth() { return m_width; }
uint32 gkVkTexture::getMipLevel() { return m_mips; }
void gkVkTexture::onReset() {}
void gkVkTexture::onLost() {}

float gkVkTexture::Tex2DRAW(const Vec2& texcoord, int)
{
	if (m_rawData.empty())
		return 0.0f;
	const uint32 x = (std::min)(m_width - 1, static_cast<uint32>(fabs(texcoord.x) * m_width));
	const uint32 y = (std::min)(m_height - 1, static_cast<uint32>(fabs(texcoord.y) * m_height));
	const uint32 bytesPerPixel = (m_format == eTF_A8 || m_format == eTF_R8) ? 1 : 4;
	return m_rawData[(static_cast<size_t>(y) * m_width + x) * bytesPerPixel] / 255.0f;
}

uint8* gkVkTexture::RawData()
{
	return m_rawData.empty() ? NULL : &m_rawData[0];
}

void gkVkTexture::changeAttr(const gkStdString& key, const gkStdString& value)
{
	m_params[key] = value;
	if (key == _T("size") || key == _T("sizey"))
	{
		resolveDimensions();
		const uint32 bytesPerPixel = (m_format == eTF_A8 || m_format == eTF_R8) ? 1 : 4;
		m_rawData.assign(static_cast<size_t>(m_width) * m_height * bytesPerPixel, 0);
	}
}

const gkStdString& gkVkTexture::getAttr(const gkStdString& key) const
{
	static const gkStdString empty;
	gkNameValuePairList::const_iterator value = m_params.find(key);
	return value == m_params.end() ? empty : value->second;
}

bool gkVkTexture::lock(gkLockOperation& operation)
{
	if (m_rawData.empty())
		return false;
	const uint32 bytesPerPixel = (m_format == eTF_A8 || m_format == eTF_R8) ? 1 : 4;
	const uint32 x = (std::max)(0, operation.m_pos.x);
	const uint32 y = (std::max)(0, operation.m_pos.y);
	if (x >= m_width || y >= m_height)
		return false;
	operation.m_outPitch = static_cast<int>(m_width * bytesPerPixel);
	operation.m_outData = &m_rawData[(static_cast<size_t>(y) * m_width + x) * bytesPerPixel];
	return true;
}

void gkVkTexture::unlock() { ++m_revision; }
bool gkVkTexture::dynamic() { return m_dynamic; }
bool gkVkTexture::sizable() { return m_sizable; }
ETexutureFormat gkVkTexture::format() { return m_format; }

gkVkMesh::gkVkMesh(IResourceManager* creator, const gkStdString& name,
	gkResourceHandle handle, const gkStdString& group, gkNameValuePairList* params)
	: IMesh(creator, name, handle, group)
	, m_vertexBuffer(NULL)
	, m_indexBuffer(NULL)
	, m_customPrimitiveCount(UINT32_MAX)
	, m_ready(false)
{
	if (params)
		m_params = *params;
	m_aabb.Reset();
}

gkVkMesh::~gkVkMesh()
{
	if (isLoaded())
		unload();
}

bool gkVkMesh::loadImpl()
{
	gkNameValuePairList::const_iterator type = m_params.find(_T("type"));
	if (type != m_params.end())
	{
		EVertexInputType vertexType = eVI_P3T2U4;
		uint32 elementSize = sizeof(GKVL_P3T2U4);
		if (type->second == _T("PARTICLE_VB"))
		{
			vertexType = eVI_T2T2;
			elementSize = sizeof(GKVL_GpuParticle);
		}
		else if (type->second == _T("TERRIAN_BLOCK") || type->second == _T("GRASS_BLOCK"))
		{
			vertexType = eVI_P4F4F4;
			elementSize = sizeof(GKVL_P3F2F4F3);
		}
		m_vertexBuffer = new gkVertexBuffer(elementSize, 0, vertexType, eBF_Discard);
		m_indexBuffer = new gkIndexBuffer(0);
		m_ready = true;
		return true;
	}
	return loadEngineMesh();
}

bool gkVkMesh::loadEngineMesh()
{
	IMeshLoader* loader = gEnv->pSystem->getOBJMeshLoader();
	if (!loader || !loader->LoadGeometry(m_wstrFileName.c_str()))
	{
		if (loader)
		{
			loader->FinishLoading();
			gEnv->pSystem->ReturnMeshLoader(loader);
		}
		return false;
	}
	gkVertexBuffer* sourceVB = loader->getVB();
	gkIndexBuffer* sourceIB = loader->getIB();
	m_vertexBuffer = new gkVertexBuffer(sourceVB->elementSize, sourceVB->elementCount,
		sourceVB->vertexType, eBF_Discard);
	memcpy(m_vertexBuffer->data, sourceVB->data, sourceVB->getSize());
	m_indexBuffer = new gkIndexBuffer(sourceIB->count, sourceIB->wordbit);
	memcpy(m_indexBuffer->data, sourceIB->data, sourceIB->getSize());
	m_indexBuffer->currAddress = sourceIB->count;
	for (uint32 i = 0; i < loader->getSubsetCount(); ++i)
	{
		gkMeshSubset subset;
		loader->getSubset(i, subset.indexStart, subset.indexCount, subset.mtlname);
		m_subsets.push_back(subset);
	}
	loader->FinishLoading();
	gEnv->pSystem->ReturnMeshLoader(loader);
	UpdateHwBuffer();
	return true;
}

bool gkVkMesh::unloadImpl()
{
	delete m_vertexBuffer;
	delete m_indexBuffer;
	m_vertexBuffer = NULL;
	m_indexBuffer = NULL;
	m_subsets.clear();
	m_ready = false;
	return true;
}

void gkVkMesh::onReset() {}
void gkVkMesh::onLost() {}
IMesh::BoneBaseTransforms& gkVkMesh::GetBoneBaseTransforms() { return m_bones; }
AABB& gkVkMesh::GetAABB() { return m_aabb; }

void gkVkMesh::getRenderOperation(gkRenderOperation& operation, uint32 subset)
{
	if (!m_ready || !m_vertexBuffer || !m_indexBuffer)
	{
		operation.operationType = gkRenderOperation::OT_SKIP;
		return;
	}
	operation.operationType = gkRenderOperation::OT_TRIANGLE_LIST;
	operation.vertexData = m_vertexBuffer;
	operation.vertexCount = m_vertexBuffer->elementCount;
	operation.vertexStart = 0;
	operation.vertexSize = m_vertexBuffer->elementSize;
	operation.useIndexes = true;
	operation.indexData = m_indexBuffer;
	operation.indexStart = 0;
	operation.indexCount = m_indexBuffer->count;
	if (subset < m_subsets.size())
	{
		operation.indexStart = m_subsets[subset].indexStart;
		operation.indexCount = m_subsets[subset].indexCount;
	}
	if (m_customPrimitiveCount != UINT32_MAX)
		operation.indexCount = (std::min)(operation.indexCount, m_customPrimitiveCount * 3);
}

bool gkVkMesh::RaycastMesh(Ray&, std::vector<float>&) { return false; }
bool gkVkMesh::RaycastMesh_WorldSpace(Ray&, Matrix34&, std::vector<float>&) { return false; }
uint32 gkVkMesh::getSubsetCount() { return m_subsets.empty() ? 1 : static_cast<uint32>(m_subsets.size()); }
gkVertexBuffer* gkVkMesh::getVertexBuffer() { return m_vertexBuffer; }
gkIndexBuffer* gkVkMesh::getIndexBuffer() { return m_indexBuffer; }

void gkVkMesh::UpdateHwBuffer()
{
	if (m_vertexBuffer && m_vertexBuffer->data && m_vertexBuffer->elementCount)
		gkMeshUtilComuputeBoundingBox(m_vertexBuffer->data, m_vertexBuffer->elementSize,
			NULL, m_vertexBuffer->elementCount, m_aabb);
	m_ready = m_vertexBuffer && m_indexBuffer;
}

void gkVkMesh::ReleaseSysBuffer() {}
void gkVkMesh::modifyCustomPrimitiveCount(uint32 count) { m_customPrimitiveCount = count; }

gkVkShader::gkVkShader(IResourceManager* creator, const gkStdString& name,
	gkResourceHandle handle, const gkStdString& group)
	: IShader(creator, name, handle, group)
	, m_systemMacro(0)
	, m_materialMacro(0)
	, m_defaultRenderLayer(static_cast<uint32>(-1))
	, m_technique("General")
{
}

bool gkVkShader::loadImpl()
{
	gkStdString shaderName = m_wstrFileName;
	const size_t macroSeparator = shaderName.find(_T('@'));
	if (macroSeparator != gkStdString::npos)
	{
		gkStdStringstream mask(shaderName.substr(macroSeparator + 1));
		mask >> m_materialMacro;
		shaderName = shaderName.substr(0, macroSeparator);
	}
	gkStdString metadataPath = _T("engine/shaders/template/built_in/");
	metadataPath += shaderName;
	metadataPath += _T(".gfx");
	IRapidXmlParser parser;
	parser.initializeReading(metadataPath.c_str());
	CRapidXmlParseNode* root = parser.getRootXmlNode();
	if (!root)
	{
		parser.finishReading();
		gkLogError(_T("Vulkan shader metadata/variant missing: %s"),
			m_wstrFileName.c_str());
		return false;
	}
	if (root)
	{
		CRapidXmlParseNode* renderLayer = root->getChildNode(_T("RenderLayer"));
		if (renderLayer)
		{
			const TCHAR* layer = renderLayer->GetAttribute(_T("layer"));
			if (layer && !_tcsicmp(layer, _T("WATER")))
				m_defaultRenderLayer = RENDER_LAYER_WATER;
			else if (layer && !_tcsicmp(layer, _T("RENDER_LAYER_SKIES_EARLY")))
				m_defaultRenderLayer = RENDER_LAYER_SKIES_EARLY;
		}
		CRapidXmlParseNode* macros = root->getChildNode(_T("Marco"));
		if (macros)
		{
			for (CRapidXmlParseNode* attribute = macros->getChildNode(_T("Attribute"));
				attribute; attribute = attribute->getNextSiblingNode(_T("Attribute")))
			{
				int bit = 0;
				attribute->GetAttribute(_T("mask"), bit);
				if (!bit)
					continue;
				ShaderMacro macro;
				const TCHAR* name = attribute->GetAttribute(_T("name"));
				const TCHAR* value = attribute->GetAttribute(_T("value"));
				const TCHAR* description = attribute->GetAttribute(_T("desc"));
				if (name)
					macro.m_name = name;
				if (value)
					macro.m_value = value;
				if (description)
					macro.m_desc = description;
				macro.m_active = (m_materialMacro & static_cast<uint32>(bit)) != 0;
				m_macros[static_cast<uint32>(bit)] = macro;
			}
		}
		parser.finishReading();
	}
	m_technique = FX_GetTechniqueByName("General");
	return true;
}

bool gkVkShader::unloadImpl()
{
	m_macros.clear();
	m_parameterValues.clear();
	m_internedNames.clear();
	m_technique = NULL;
	return true;
}
uint32 gkVkShader::getDefaultRenderLayer() { return m_defaultRenderLayer; }
void gkVkShader::onReset() {}
void gkVkShader::onLost() {}
void gkVkShader::FX_SetTechniqueByName(LPCSTR name)
{
	m_technique = FX_GetTechniqueByName(name);
}
void gkVkShader::FX_SetTechnique(GKHANDLE technique) { m_technique = technique; }
GKHANDLE gkVkShader::FX_GetTechniqueByName(LPCSTR name)
{
	if (!name)
		return NULL;
	return m_internedNames.insert(name).first->c_str();
}

GKHANDLE gkVkShader::FX_GetTechnique(EShaderInternalTechnique technique)
{
	static const char* techniques[eSIT_Count] =
		{ "General", "ZpassDL", "Shadow", "ZpassDS", "Reflection", "FastCube" };
	return FX_GetTechniqueByName(
		technique < eSIT_Count ? techniques[technique] : techniques[0]);
}

void gkVkShader::FX_SetValue(GKHANDLE parameter, const void* data, uint32 bytes)
{
	if (!parameter || !data || !bytes)
		return;
	std::vector<uint8>& destination = m_parameterValues[parameter];
	destination.resize(bytes);
	memcpy(&destination[0], data, bytes);
}
void gkVkShader::FX_SetFloat(GKHANDLE parameter, float data)
{
	FX_SetValue(parameter, &data, sizeof(data));
}
void gkVkShader::FX_SetFloat3(GKHANDLE parameter, const Vec3& data)
{
	FX_SetValue(parameter, &data, sizeof(data));
}
void gkVkShader::FX_SetFloat4(GKHANDLE parameter, const Vec4& data)
{
	FX_SetValue(parameter, &data, sizeof(data));
}
void gkVkShader::FX_SetMatrixArray(GKHANDLE parameter, D3DXMATRIX* data, uint32 size)
{
	FX_SetValue(parameter, data, static_cast<uint32>(sizeof(Matrix44) * size));
}
void gkVkShader::FX_SetMatrix(GKHANDLE parameter, const Matrix44& data)
{
	FX_SetValue(parameter, &data, sizeof(data));
}
void gkVkShader::FX_Begin(uint32* passes, DWORD) { if (passes) *passes = 1; }
void gkVkShader::FX_BeginPass(uint32) {}
void gkVkShader::FX_Commit() {}
void gkVkShader::FX_EndPass() {}
void gkVkShader::FX_End() {}
gkMaterialParams* gkVkShader::getShaderDefaultParams() { return &m_defaultParams; }
ShaderMacros& gkVkShader::getShaderMarcos() { return m_macros; }
uint32 gkVkShader::getShaderMarcoMask() { return m_materialMacro | m_systemMacro; }
void gkVkShader::switchSystemMacro(uint32 macro) { m_systemMacro = macro; }

gkVkMaterial::gkVkMaterial(IResourceManager* creator, const gkStdString& name,
	gkResourceHandle handle, const gkStdString& group, gkNameValuePairList* params)
	: IMaterial(creator, name, handle, group)
	, m_uvTiling(1, 1)
	, m_uvSpeed(0)
	, m_glossness(8)
	, m_opacity(100)
	, m_doubleSided(false)
	, m_ssrl(false)
	, m_castShadow(true)
	, m_shaderName(_T("kssimple"))
	, m_macroMask(0)
	, m_diffuseColor(0.8f, 0.8f, 0.8f, 1.0f)
{
	if (params)
		m_params = *params;
}

gkVkMaterial::~gkVkMaterial()
{
	if (isLoaded())
		unload();
}

bool gkVkMaterial::loadImpl()
{
	if (gEnv && gEnv->pSystem && gEnv->pSystem->getShaderMngPtr())
		m_shader = gEnv->pSystem->getShaderMngPtr()->load(_T("kssimple"), _T("vulkan"));

	gkStdString filename = m_wstrFileName;
	if (filename.size() < 4 || _tcsicmp(filename.c_str() + filename.size() - 4, _T(".mtl")))
		filename += _T(".mtl");
	IRapidXmlParser parser;
	parser.initializeReading(filename.c_str());
	CRapidXmlParseNode* root = parser.getRootXmlNode();
	if (!root)
		return true;
	bool loaded = false;
	if (root->getChildNode(_T("Effect")))
		loaded = loadMaterialNode(root);
	for (CRapidXmlParseNode* sub = root->getChildNode(_T("SubMaterial")); sub;
		sub = sub->getNextSiblingNode(_T("SubMaterial")))
	{
		gkStdString subName = m_wstrFileName + _T("|");
		const TCHAR* nodeName = sub->GetAttribute(_T("Name"));
		if (nodeName)
			subName += nodeName;
		gkVkMaterial* subMaterial = new gkVkMaterial(NULL, subName, 0, m_wstrGroupName, NULL);
		subMaterial->m_shader = m_shader;
		if (subMaterial->loadMaterialNode(sub))
		{
			m_subMaterials.push_back(gkMaterialPtr(subMaterial));
			loaded = true;
		}
		else
			delete subMaterial;
	}
	parser.finishReading();
	return loaded || !m_shader.isNull();
}

bool gkVkMaterial::loadMaterialNode(CRapidXmlParseNode* node)
{
	if (!node)
		return false;
	CRapidXmlParseNode* effect = node->getChildNode(_T("Effect"));
	if (effect)
	{
		const TCHAR* shader = effect->GetAttribute(_T("FileName"));
		if (shader)
			m_shaderName = gkGetPureFilename(shader);
		effect->GetAttribute(_T("CastShadow"), m_castShadow);
		int mask = 0;
		effect->GetAttribute(_T("Mask"), mask);
		m_macroMask = static_cast<uint32>(mask);
		const bool vegetationShader =
			!_tcsicmp(m_shaderName.c_str(), _T("grass")) ||
			!_tcsicmp(m_shaderName.c_str(), _T("vegetation")) ||
			!_tcsicmp(m_shaderName.c_str(), _T("vegetation_autoexpand"));
		if (vegetationShader || !_tcsicmp(m_shaderName.c_str(), _T("ks_skyhdr")))
			m_doubleSided = true;
		if (vegetationShader)
		{
			m_macroMask |= 128u;
		}
		if (gEnv && gEnv->pSystem && gEnv->pSystem->getShaderMngPtr())
		{
			gkStdStringstream variantName;
			variantName << m_shaderName << _T("@") << m_macroMask;
			m_shader = gEnv->pSystem->getShaderMngPtr()->load(
				variantName.str(), _T("vulkan"));
		}
		for (CRapidXmlParseNode* parameter = effect->getChildNode(_T("Param")); parameter;
			parameter = parameter->getNextSiblingNode(_T("Param")))
		{
			const TCHAR* name = parameter->GetAttribute(_T("name"));
			const TCHAR* value = parameter->GetAttribute(_T("value"));
			if (!name || !value)
				continue;
			if (!_tcsicmp(name, _T("g_MatDiffuse")))
			{
				gkStdStringstream values(value);
				values >> m_diffuseColor.r >> m_diffuseColor.g >>
					m_diffuseColor.b >> m_diffuseColor.a;
			}
			else if (!_tcsicmp(name, _T("g_Gloss")))
			{
				gkStdStringstream values(value);
				values >> m_glossness;
			}
			else if (!_tcsicmp(name, _T("g_Opacity")))
			{
				float opacity = 1.0f;
				gkStdStringstream values(value);
				values >> opacity;
				m_opacity = static_cast<int>(opacity * 100.0f);
			}
			const EMaterialSlot textureSlot = textureSlotFromParameter(name);
			if (textureSlot != eMS_Invalid && gEnv && gEnv->pSystem &&
				gEnv->pSystem->getTextureMngPtr())
			{
				m_textures[textureSlot] =
					gEnv->pSystem->getTextureMngPtr()->loadSync(
						value, _T("vulkan-material"));
			}
		}
	}

	CRapidXmlParseNode* textureChannels = node->getChildNode(_T("TexChannel"));
	if (textureChannels && gEnv && gEnv->pSystem && gEnv->pSystem->getTextureMngPtr())
	{
		for (CRapidXmlParseNode* parameter = textureChannels->getChildNode(_T("Param")); parameter;
			parameter = parameter->getNextSiblingNode(_T("Param")))
		{
			int channel = 0;
			parameter->GetAttribute(_T("chn"), channel);
			const TCHAR* value = parameter->GetAttribute(_T("value"));
			if (value && channel >= 0 && channel < eMS_Invalid)
				m_textures[channel] = gEnv->pSystem->getTextureMngPtr()->loadSync(
					value, _T("vulkan-material"));
		}
	}
	CRapidXmlParseNode* staticParameters = node->getChildNode(_T("Static"));
	if (staticParameters)
	{
		CRapidXmlParseNode* uv = staticParameters->getChildNode(_T("Param"));
		if (uv)
		{
			const TCHAR* value = uv->GetAttribute(_T("value"));
			if (value)
			{
				float offsetX = 0.0f;
				float offsetY = 0.0f;
				gkStdStringstream values(value);
				values >> m_uvTiling.x >> m_uvTiling.y >> offsetX >> offsetY;
			}
		}
	}
	return effect != NULL;
}

bool gkVkMaterial::unloadImpl()
{
	m_shader.setNull();
	m_subMaterials.clear();
	for (uint32 i = 0; i < eMS_Invalid; ++i)
		m_textures[i].setNull();
	return true;
}

gkShaderPtr& gkVkMaterial::getShader() { return m_shader; }
gkTexturePtr& gkVkMaterial::getTexture(EMaterialSlot index)
{
	static gkTexturePtr nullTexture;
	return index < eMS_Invalid ? m_textures[index] : nullTexture;
}
void gkVkMaterial::setTexture(gkTexturePtr& texture, BYTE index)
{
	if (index < eMS_Invalid)
		m_textures[index] = texture;
}
void gkVkMaterial::ApplyParameterBlock(bool texture, IShader* shader)
{
	IShader* target = shader ? shader : m_shader.getPointer();
	if (!target)
		return;
	BasicApply(target);
	if (texture)
		for (uint32 i = 0; i < eMS_Invalid; ++i)
			if (!m_textures[i].isNull())
				m_textures[i]->Apply(i, 0);
}
void gkVkMaterial::BasicApply(IShader* shader)
{
	if (shader)
	{
		Vec4 uv(m_uvTiling.x, m_uvTiling.y, 0, 0);
		shader->FX_SetValue("g_UVParam", &uv, sizeof(uv));
	}
}
Vec2& gkVkMaterial::getUVTill() { return m_uvTiling; }
bool gkVkMaterial::isDoubleSide() { return m_doubleSided; }
float gkVkMaterial::getGlossness() { return m_glossness; }
int gkVkMaterial::getOpacity() { return m_opacity; }
void gkVkMaterial::setUVTill(Vec2 till) { m_uvTiling = till; }
void gkVkMaterial::setUVMovement(float speed) { m_uvSpeed = speed; }
bool& gkVkMaterial::getSSRL() { return m_ssrl; }
bool& gkVkMaterial::getCastShadow() { return m_castShadow; }
gkMaterialParams* gkVkMaterial::getLoadingParameterBlock() { return &m_materialParams; }
void gkVkMaterial::saveAsMtlFile(const TCHAR*) {}
void gkVkMaterial::saveToMtlFile() {}
uint8 gkVkMaterial::getSubMtlCount() { return static_cast<uint8>(m_subMaterials.size()); }
IMaterial* gkVkMaterial::getSubMaterial(uint8 index)
{
	return index < m_subMaterials.size() ? m_subMaterials[index].getPointer() : this;
}
const gkStdString& gkVkMaterial::getShaderName() { return m_shaderName; }
ShaderMacros& gkVkMaterial::getShaderMarcos() { return m_macros; }
uint32 gkVkMaterial::getShaderMarcoMask() { return m_macroMask; }

IResource* gkVkTextureManager::createImpl(const gkStdString& name, gkResourceHandle handle,
	const gkStdString& group, gkNameValuePairList* params)
{
	return new gkVkTexture(this, name, handle, group, params);
}

IResource* gkVkMeshManager::createImpl(const gkStdString& name, gkResourceHandle handle,
	const gkStdString& group, gkNameValuePairList* params)
{
	return new gkVkMesh(this, name, handle, group, params);
}

IResource* gkVkShaderManager::createImpl(const gkStdString& name, gkResourceHandle handle,
	const gkStdString& group, gkNameValuePairList*)
{
	return new gkVkShader(this, name, handle, group);
}

IResource* gkVkMaterialManager::createImpl(const gkStdString& name, gkResourceHandle handle,
	const gkStdString& group, gkNameValuePairList* params)
{
	return new gkVkMaterial(this, name, handle, group, params);
}
