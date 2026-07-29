#include "gkRendererVulkan.h"
#include "gkVkDeviceContext.h"

#include "IAuxRenderer.h"
#include "IRenderSequence.h"
#include "IResourceManager.h"
#include "ICamera.h"
#include "gkRenderable.h"
#include "gk_Camera.h"
#include "gkVkResources.h"

#include <algorithm>
#include <vector>

namespace
{
	Vec3 transformSkinPointRowVector(const Matrix44A& matrix, const Vec3& point)
	{
		return Vec3(
			point.x * matrix.m00 + point.y * matrix.m10 + point.z * matrix.m20 + matrix.m30,
			point.x * matrix.m01 + point.y * matrix.m11 + point.z * matrix.m21 + matrix.m31,
			point.x * matrix.m02 + point.y * matrix.m12 + point.z * matrix.m22 + matrix.m32);
	}

	Vec3 transformSkinDirectionRowVector(const Matrix44A& matrix, const Vec3& direction)
	{
		return Vec3(
			direction.x * matrix.m00 + direction.y * matrix.m10 + direction.z * matrix.m20,
			direction.x * matrix.m01 + direction.y * matrix.m11 + direction.z * matrix.m21,
			direction.x * matrix.m02 + direction.y * matrix.m12 + direction.z * matrix.m22);
	}

	void skinVertexBuffer(gkVertexBuffer& source, gkVertexBuffer& destination,
		const Matrix44A* matrices, uint32 matrixCount)
	{
		memcpy(destination.data, source.data, source.getSize());
		for (uint32 vertexIndex = 0; vertexIndex < source.elementCount; ++vertexIndex)
		{
			const GKVL_P3F2F4F3F4U4* input =
				reinterpret_cast<const GKVL_P3F2F4F3F4U4*>(
					source.data + static_cast<size_t>(vertexIndex) * source.elementSize);
			GKVL_P3F2F4F3F4U4* output =
				reinterpret_cast<GKVL_P3F2F4F3F4U4*>(
					destination.data + static_cast<size_t>(vertexIndex) * destination.elementSize);
			const uint8 indices[4] = {
				input->blendIndice.x, input->blendIndice.y,
				input->blendIndice.z, input->blendIndice.w
			};
			const uint8 weights[4] = {
				input->blendWeight.x, input->blendWeight.y,
				input->blendWeight.z, input->blendWeight.w
			};
			Vec3 skinned(ZERO);
			Vec3 skinnedTangent(ZERO);
			Vec3 skinnedBinormal(ZERO);
			float totalWeight = 0.0f;
			for (uint32 influence = 0; influence < 4; ++influence)
			{
				if (indices[influence] >= matrixCount || weights[influence] == 0)
					continue;
				const float weight = weights[influence] / 255.0f;
				skinned += transformSkinPointRowVector(
					matrices[indices[influence]], input->position) * weight;
				skinnedTangent += transformSkinDirectionRowVector(
					matrices[indices[influence]],
					Vec3(input->tangent_ti.x, input->tangent_ti.y,
						input->tangent_ti.z)) * weight;
				skinnedBinormal += transformSkinDirectionRowVector(
					matrices[indices[influence]], input->binormal) * weight;
				totalWeight += weight;
			}
			output->position = totalWeight > 0.0f ? skinned / totalWeight : input->position;
			if (totalWeight > 0.0f)
			{
				skinnedTangent.NormalizeSafe(
					Vec3(input->tangent_ti.x, input->tangent_ti.y, input->tangent_ti.z));
				skinnedBinormal.NormalizeSafe(input->binormal);
				output->tangent_ti = Vec4(skinnedTangent, input->tangent_ti.w);
				output->binormal = skinnedBinormal;
			}
		}
		destination.m_needRebind = true;
	}
}

class gkVkRenderSequence : public IRenderSequence
{
public:
	struct Entry
	{
		gkRenderable* renderable;
		BYTE layer;
	};

	gkVkRenderSequence()
		: m_camera(NULL)
		, m_fillMode(eRFMode_General)
	{
	}

	virtual void addToRenderSequence(gkRenderable* renderable, BYTE layer)
	{
		if (!renderable || m_fillMode != eRFMode_General)
			return;
		IMaterial* material = renderable->getMaterial();
		if (material)
			material->touch();
		Entry entry = { renderable, layer };
		m_entries.push_back(entry);
	}
	virtual void addToRenderSequence(gkRenderable* renderable)
	{
		if (!renderable)
			return;
		BYTE layer = RENDER_LAYER_OPAQUE;
		IMaterial* material = renderable->getMaterial();
		if (material)
		{
			IShader* shader = material->getShader().getPointer();
			const uint32 shaderLayer = shader ? shader->getDefaultRenderLayer() :
				static_cast<uint32>(-1);
			if (shaderLayer != static_cast<uint32>(-1))
				layer = static_cast<BYTE>(shaderLayer);
			else if (material->getOpacity() < 100)
				layer = RENDER_LAYER_TRANSPARENT;
		}
		addToRenderSequence(renderable, layer);
	}
	virtual void addRenderLight(gkRenderLight&) {}
	virtual void setCamera(ICamera* camera, BYTE mode)
	{
		if (mode == eRFMode_General)
			m_camera = camera;
	}
	virtual void setRenderableFillMode(BYTE mode) { m_fillMode = mode; }
	virtual void markFinished() {}
	virtual void clear()
	{
		m_entries.clear();
		m_camera = NULL;
		m_fillMode = eRFMode_General;
	}

	const std::vector<Entry>& entries() const { return m_entries; }
	ICamera* camera() const { return m_camera; }

private:
	std::vector<Entry> m_entries;
	ICamera* m_camera;
	BYTE m_fillMode;
};

class gkVkNullAuxRenderer : public IAuxRenderer
{
public:
	struct ScreenQuad
	{
		Vec2 points[4];
		ColorB color;
		ITexture* texture;
	};

	struct WorldLine
	{
		Vec3 from;
		Vec3 to;
		ColorF color;
		bool ignoreZ;
	};

	virtual void AuxRender3DLine(const Vec3& from, const Vec3& to,
		ColorF& color, bool ignoreZ)
	{
		if ((to - from).GetLengthSquared() < 0.000001f)
			return;
		WorldLine line = { from, to, color, ignoreZ };
		m_updatingLines.push_back(line);
	}
	virtual void AuxRender3DGird(const Vec3& center, int row, float gap,
		ColorF& color, bool ignoreZ)
	{
		const float extent = row * gap;
		for (int index = -row; index <= row; ++index)
		{
			const float offset = index * gap;
			AuxRender3DLine(center + Vec3(-extent, offset, 0),
				center + Vec3(extent, offset, 0), color, ignoreZ);
			AuxRender3DLine(center + Vec3(offset, -extent, 0),
				center + Vec3(offset, extent, 0), color, ignoreZ);
		}
	}
	virtual void AuxRender3DBoxFrameRotated(const Vec3& center, const Quat& rotation,
		float length, ColorF& color, bool ignoreZ)
	{
		AuxRender3DBoxFrameRotated(center, rotation,
			Vec3(length, length, length), color, ignoreZ);
	}
	virtual void AuxRender3DBoxFrameRotated(const Vec3& center, const Quat& rotation,
		const Vec3& size, ColorF& color, bool ignoreZ)
	{
		const Vec3 half = size * 0.5f;
		Vec3 corners[8] = {
			Vec3(-half.x, -half.y, -half.z), Vec3(half.x, -half.y, -half.z),
			Vec3(half.x, half.y, -half.z), Vec3(-half.x, half.y, -half.z),
			Vec3(-half.x, -half.y, half.z), Vec3(half.x, -half.y, half.z),
			Vec3(half.x, half.y, half.z), Vec3(-half.x, half.y, half.z)
		};
		for (uint32 index = 0; index < 8; ++index)
			corners[index] = rotation * corners[index] + center;
		appendBoxCorners(corners, color, ignoreZ);
	}
	virtual void AuxRender3DBoxFrame(const Vec3& center, float radius,
		ColorF& color, bool ignoreZ)
	{
		Vec3 half(radius, radius, radius);
		AuxRender3DBoxFrame(center, half, color, ignoreZ);
	}
	virtual void AuxRender3DBoxFrame(const Vec3& center, Vec3& size,
		ColorF& color, bool ignoreZ)
	{
		const Vec3 half = size * 0.5f;
		appendBox(center - half, center + half, color, ignoreZ);
	}
	virtual void AuxRender3DCircle(const Vec3& center, const Vec3& normal,
		float radius, uint32 side, ColorF& color, bool ignoreZ)
	{
		if (side < 3 || radius <= 0.0f)
			return;
		Vec3 axis = fabs(normal.z) < 0.9f ? normal.Cross(Vec3(0, 0, 1)) :
			normal.Cross(Vec3(0, 1, 0));
		axis.NormalizeSafe(Vec3(1, 0, 0));
		Vec3 tangent = normal.Cross(axis);
		tangent.NormalizeSafe(Vec3(0, 1, 0));
		Vec3 previous = center + axis * radius;
		for (uint32 index = 1; index <= side; ++index)
		{
			const float angle = gf_PI2 * index / static_cast<float>(side);
			const Vec3 current = center +
				(axis * cosf(angle) + tangent * sinf(angle)) * radius;
			AuxRender3DLine(previous, current, color, ignoreZ);
			previous = current;
		}
	}
	virtual void AuxRenderAABB(const AABB& aabb, ColorF& color, bool ignoreZ)
	{
		appendBox(aabb.min, aabb.max, color, ignoreZ);
	}
	virtual void AuxRenderGizmo(const Matrix44& matrix, float size, uint8,
		bool ignoreZ, uint8)
	{
		const Vec3 center(matrix.m30, matrix.m31, matrix.m32);
		ColorF red(1, 0, 0, 1);
		ColorF green(0, 1, 0, 1);
		ColorF blue(0, 0.5f, 1, 1);
		AuxRender3DLine(center,
			center + Vec3(matrix.m00, matrix.m01, matrix.m02) * size, red, ignoreZ);
		AuxRender3DLine(center,
			center + Vec3(matrix.m10, matrix.m11, matrix.m12) * size, green, ignoreZ);
		AuxRender3DLine(center,
			center + Vec3(matrix.m20, matrix.m21, matrix.m22) * size, blue, ignoreZ);
	}
	virtual void AuxRenderSkeleton(const Vec3&, const Vec3&, ColorF&, float, bool) {}
	virtual void AuxRender3DBoxSolid(const Vec3&, float, ColorF&, bool) {}
	virtual void AuxRender3DBoxSolid(const Vec3&, Vec3&, ColorF&, bool) {}
	virtual void AuxRenderPyramid(const Vec3&, const Vec3&, const float, ColorF&, bool) {}
	virtual void AuxRenderMeshFrame(IGameObjectRenderLayer*, ColorF&, bool) {}
	virtual void AuxRenderText(const TCHAR* text, int posx, int posy,
		const IFtFont* font, const ColorB& color, uint32 alignment, uint32 style)
	{
		if (!gEnv || !gEnv->pFont || !text || !font)
			return;
		if (style)
			gEnv->pFont->DrawString(text, font, Vec2(posx + 1, posy + 1),
				ColorB(0, 0, 0, 127), alignment);
		gEnv->pFont->DrawString(text, font, Vec2(posx, posy), color, alignment);
	}
	virtual void AuxRenderScreenBox(const Vec2& pos, const Vec2& wh,
		const ColorB& color)
	{
		appendScreenBox(pos, wh, color, NULL);
	}
	virtual void AuxRenderScreenBox(const Vec2& pos, const Vec2& wh,
		const ColorB& color, ITexture* texture)
	{
		appendScreenBox(pos, wh, color, texture);
	}
	virtual void AuxRenderScreenLine(const Vec2& from, const Vec2& to,
		const ColorB& color)
	{
		Vec2 direction = to - from;
		const float length = direction.GetLength();
		if (length <= 0.0001f)
			return;
		const Vec2 normal(-direction.y / length * 0.75f,
			direction.x / length * 0.75f);
		ScreenQuad quad;
		quad.points[0] = from - normal;
		quad.points[1] = to - normal;
		quad.points[2] = to + normal;
		quad.points[3] = from + normal;
		quad.color = color;
		quad.texture = NULL;
		m_updatingQuads.push_back(quad);
	}
	virtual void _swapBufferForRendering()
	{
		m_renderingQuads.swap(m_updatingQuads);
		m_updatingQuads.clear();
		m_renderingLines.swap(m_updatingLines);
		m_updatingLines.clear();
	}
	virtual void _cleanBuffer()
	{
		m_updatingQuads.clear();
		m_renderingQuads.clear();
		m_updatingLines.clear();
		m_renderingLines.clear();
	}

	void prepare(gkVkDeviceContext* context)
	{
		for (size_t i = 0; i < m_renderingQuads.size(); ++i)
			context->prepareTexture(m_renderingQuads[i].texture);
	}

	void render(gkVkDeviceContext* context, uint32 width, uint32 height,
		const Matrix44& viewProjection, const Vec3& cameraPosition,
		const Vec3& cameraDirection)
	{
		if (!width || !height)
			return;
		const Matrix44 identity(IDENTITY);
		for (size_t lineIndex = 0; lineIndex < m_renderingLines.size(); ++lineIndex)
		{
			const WorldLine& line = m_renderingLines[lineIndex];
			const Vec3 direction = line.to - line.from;
			Vec3 normal = direction.Cross(cameraDirection);
			if (normal.GetLengthSquared() < 0.000001f)
				normal = direction.Cross(Vec3(0, 0, 1));
			normal.NormalizeSafe(Vec3(1, 0, 0));
			const float distance = ((line.from + line.to) * 0.5f -
				cameraPosition).GetLength();
			normal *= (std::max)(0.005f, (std::min)(0.25f, distance * 0.0015f));
			gkVertexBuffer vertices(sizeof(GKVL_P3T2U4), 4,
				eVI_P3T2U4, eBF_Discard);
			vertices.m_needRebind = true;
			const Vec3 positions[4] = {
				line.from - normal, line.to - normal,
				line.to + normal, line.from + normal
			};
			for (uint32 vertexIndex = 0; vertexIndex < 4; ++vertexIndex)
			{
				GKVL_P3T2U4* vertex = reinterpret_cast<GKVL_P3T2U4*>(
					vertices.data + vertexIndex * vertices.elementSize);
				vertex->position = positions[vertexIndex];
				vertex->texcoord = Vec2(0, 0);
				vertex->color = 0xffffffff;
			}
			gkIndexBuffer indices(6, true);
			indices.m_needRebind = true;
			indices.push_back(0); indices.push_back(1); indices.push_back(2);
			indices.push_back(0); indices.push_back(2); indices.push_back(3);
			gkRenderOperation operation;
			operation.vertexData = &vertices;
			operation.vertexCount = 4;
			operation.vertexStart = 0;
			operation.vertexSize = sizeof(GKVL_P3T2U4);
			operation.useIndexes = true;
			operation.indexData = &indices;
			operation.indexCount = 6;
			operation.indexStart = 0;
			context->drawRenderOperation(operation, viewProjection, line.color, NULL,
				Vec2(1, 1), line.color.a < 1.0f, false, Vec3(0, 0, 1),
				true, false, line.ignoreZ);
		}
		for (size_t quadIndex = 0; quadIndex < m_renderingQuads.size(); ++quadIndex)
		{
			const ScreenQuad& quad = m_renderingQuads[quadIndex];
			gkVertexBuffer vertices(sizeof(GKVL_P3T2U4), 4,
				eVI_P3T2U4, eBF_Discard);
			vertices.m_needRebind = true;
			for (uint32 vertexIndex = 0; vertexIndex < 4; ++vertexIndex)
			{
				GKVL_P3T2U4* vertex = reinterpret_cast<GKVL_P3T2U4*>(
					vertices.data + vertexIndex * vertices.elementSize);
				vertex->position = Vec3(
					quad.points[vertexIndex].x / static_cast<float>(width) * 2.0f - 1.0f,
					1.0f - quad.points[vertexIndex].y / static_cast<float>(height) * 2.0f,
					0.0f);
				vertex->texcoord = Vec2(
					vertexIndex == 1 || vertexIndex == 2 ? 1.0f : 0.0f,
					vertexIndex >= 2 ? 1.0f : 0.0f);
				vertex->color = 0xffffffff;
			}
			gkIndexBuffer indices(6, true);
			indices.m_needRebind = true;
			indices.push_back(0);
			indices.push_back(1);
			indices.push_back(2);
			indices.push_back(0);
			indices.push_back(2);
			indices.push_back(3);
			gkRenderOperation operation;
			operation.vertexData = &vertices;
			operation.vertexCount = 4;
			operation.vertexStart = 0;
			operation.vertexSize = sizeof(GKVL_P3T2U4);
			operation.useIndexes = true;
			operation.indexData = &indices;
			operation.indexCount = 6;
			operation.indexStart = 0;
			const ColorF color(
				quad.color.r / 255.0f, quad.color.g / 255.0f,
				quad.color.b / 255.0f, quad.color.a / 255.0f);
			context->drawRenderOperation(operation, identity, color, quad.texture,
				Vec2(1.0f, 1.0f), quad.color.a < 255, false,
				Vec3(0, 0, 1), true, false, true);
		}
	}

private:
	void appendBox(const Vec3& minimum, const Vec3& maximum,
		ColorF& color, bool ignoreZ)
	{
		const Vec3 corners[8] = {
			Vec3(minimum.x, minimum.y, minimum.z),
			Vec3(maximum.x, minimum.y, minimum.z),
			Vec3(maximum.x, maximum.y, minimum.z),
			Vec3(minimum.x, maximum.y, minimum.z),
			Vec3(minimum.x, minimum.y, maximum.z),
			Vec3(maximum.x, minimum.y, maximum.z),
			Vec3(maximum.x, maximum.y, maximum.z),
			Vec3(minimum.x, maximum.y, maximum.z)
		};
		appendBoxCorners(corners, color, ignoreZ);
	}

	void appendBoxCorners(const Vec3 (&corners)[8], ColorF& color, bool ignoreZ)
	{
		const uint8 edges[24] = {
			0,1, 1,2, 2,3, 3,0, 4,5, 5,6, 6,7, 7,4,
			0,4, 1,5, 2,6, 3,7
		};
		for (uint32 edge = 0; edge < 12; ++edge)
			AuxRender3DLine(corners[edges[edge * 2]],
				corners[edges[edge * 2 + 1]], color, ignoreZ);
	}

	void appendScreenBox(const Vec2& pos, const Vec2& wh,
		const ColorB& color, ITexture* texture)
	{
		ScreenQuad quad;
		quad.points[0] = pos;
		quad.points[1] = Vec2(pos.x + wh.x, pos.y);
		quad.points[2] = pos + wh;
		quad.points[3] = Vec2(pos.x, pos.y + wh.y);
		quad.color = color;
		quad.texture = texture;
		m_updatingQuads.push_back(quad);
	}

	std::vector<ScreenQuad> m_updatingQuads;
	std::vector<ScreenQuad> m_renderingQuads;
	std::vector<WorldLine> m_updatingLines;
	std::vector<WorldLine> m_renderingLines;
};

gkRendererVulkan::gkRendererVulkan()
	: m_context(new gkVkDeviceContext())
	, m_auxRenderer(new gkVkNullAuxRenderer())
	, m_updatingSequence(new gkVkRenderSequence())
	, m_renderingSequence(new gkVkRenderSequence())
	, m_textureManager(new gkVkTextureManager())
	, m_meshManager(new gkVkMeshManager())
	, m_materialManager(new gkVkMaterialManager())
	, m_shaderManager(new gkVkShaderManager())
	, m_skipFrames(0)
	, m_fullscreen(false)
	, m_sunDirection(Vec3(-0.35f, -0.45f, 0.82f).GetNormalized())
{
}

gkRendererVulkan::~gkRendererVulkan()
{
	Destroy();
	delete m_renderingSequence;
	delete m_updatingSequence;
	delete m_shaderManager;
	delete m_materialManager;
	delete m_meshManager;
	delete m_textureManager;
	delete m_auxRenderer;
	delete m_context;
}

IResourceManager* gkRendererVulkan::getResourceManager(BYTE type)
{
	switch (type)
	{
	case GK_RESOURCE_MANAGER_TYPE_TEXTURE:
		return m_textureManager;
	case GK_RESOURCE_MANAGER_TYPE_MATERIAL:
		return m_materialManager;
	case GK_RESOURCE_MANAGER_TYPE_MESH:
		return m_meshManager;
	case GK_RESOURCE_MANAGER_TYPE_SHADER:
		return m_shaderManager;
	default:
		return NULL;
	}
}

HWND gkRendererVulkan::Init(ISystemInitInfo& sii)
{
	return m_context->initialize(sii);
}

void gkRendererVulkan::Destroy()
{
	m_materialManager->removeAll();
	m_textureManager->removeAll();
	m_meshManager->removeAll();
	m_shaderManager->removeAll();
	m_context->shutdown();
}

IParticleProxy* gkRendererVulkan::createGPUParticleProxy()
{
	return NULL;
}

void gkRendererVulkan::destroyGPUParticelProxy(IParticleProxy*)
{
}

IRenderSequence* gkRendererVulkan::RT_SwapRenderSequence()
{
	gkVkRenderSequence* temp = m_renderingSequence;
	m_renderingSequence = m_updatingSequence;
	m_updatingSequence = temp;
	m_updatingSequence->clear();
	m_auxRenderer->_swapBufferForRendering();
	return m_updatingSequence;
}

IRenderSequence* gkRendererVulkan::RT_GetRenderSequence()
{
	return m_updatingSequence;
}

void gkRendererVulkan::RT_CleanRenderSequence()
{
	m_updatingSequence->clear();
	m_renderingSequence->clear();
	m_auxRenderer->_cleanBuffer();
}

bool gkRendererVulkan::RT_StartRender()
{
	if (m_skipFrames > 0)
		--m_skipFrames;
	if (!m_context->beginFrame())
		return false;

	ICamera* camera = m_renderingSequence->camera();
	if (!camera)
		return true;
	// Force the engine camera's derived CCamera to synchronize. D3D9 consumes
	// this matrix directly: column 1 is forward (+Y), column 2 is up (+Z).
	camera->getDerivedPosition();
	CCamera* renderCamera = camera->getCCam();
	if (!renderCamera)
		return true;
	const Matrix34& cameraMatrix = renderCamera->GetMatrix();
	const Vec3 cameraPosition = renderCamera->GetPosition();
	const Vec3 cameraDirection = cameraMatrix.GetColumn1();
	const Vec3 cameraUp = cameraMatrix.GetColumn2();
	Matrix44A view;
	Matrix44A projection;
	mathMatrixLookAt(&view, cameraPosition,
		cameraPosition + cameraDirection, cameraUp);
	mathMatrixPerspectiveFov(&projection, renderCamera->GetFov(),
		renderCamera->GetProjRatio(), renderCamera->GetNearPlane(),
		renderCamera->GetFarPlane());
	const Matrix44 viewProjection = view * projection;

	std::vector<gkVkRenderSequence::Entry> entries = m_renderingSequence->entries();
	std::stable_sort(entries.begin(), entries.end(),
		[&cameraPosition](const gkVkRenderSequence::Entry& left,
			const gkVkRenderSequence::Entry& right)
		{
			if (left.layer != right.layer)
				return left.layer < right.layer;
			if (left.layer < RENDER_LAYER_TRANSPARENT)
				return false;
			Matrix44 leftWorld(IDENTITY);
			Matrix44 rightWorld(IDENTITY);
			left.renderable->getWorldTransforms(&leftWorld);
			right.renderable->getWorldTransforms(&rightWorld);
			const Vec3 leftPosition(leftWorld.m30, leftWorld.m31, leftWorld.m32);
			const Vec3 rightPosition(rightWorld.m30, rightWorld.m31, rightWorld.m32);
			return (leftPosition - cameraPosition).GetLengthSquared() >
				(rightPosition - cameraPosition).GetLengthSquared();
		});
	m_context->prepareTexture(NULL);
	for (size_t i = 0; i < entries.size(); ++i)
	{
		gkRenderable* renderable = entries[i].renderable;
		gkVkMaterial* material = renderable ?
			dynamic_cast<gkVkMaterial*>(renderable->getMaterial()) : NULL;
		if (material)
		{
			ITexture* textures[16] = {};
			for (uint32 stage = 0; stage < eMS_Invalid; ++stage)
				textures[stage] = material->getTexture(
					static_cast<EMaterialSlot>(stage)).getPointer();
			m_context->prepareTextures(textures, 16);
		}
	}
	m_auxRenderer->prepare(m_context);
	for (uint32 renderPass = 0; renderPass < 2; ++renderPass)
	{
		for (size_t i = 0; i < entries.size(); ++i)
		{
			gkRenderable* renderable = entries[i].renderable;
			if (!renderable)
				continue;
			gkVkMaterial* material =
				dynamic_cast<gkVkMaterial*>(renderable->getMaterial());
			const bool fontRenderable =
				renderable->getMaterialName() == _T("FontMaterial");
			const bool transparent = fontRenderable ||
				entries[i].layer >= RENDER_LAYER_TRANSPARENT ||
				(material && material->getOpacity() < 100);
			if (transparent != (renderPass == 1))
				continue;
			renderable->RT_Prepare();
			renderable->RP_Prepare();
			gkRenderOperation operation;
			renderable->getRenderOperation(operation);
			if (operation.operationType == gkRenderOperation::OT_SKIP)
				continue;
			Matrix44A* skinMatrices = NULL;
			uint32 skinMatrixCount = 0;
			gkVertexBuffer* originalVertexData = operation.vertexData;
			gkVertexBuffer* cpuSkinnedVertexData = NULL;
			if (operation.vertexData &&
				operation.vertexData->vertexType == eVI_P4F4F4F4U4 &&
				renderable->getSkinnedMatrix(&skinMatrices, skinMatrixCount) &&
				skinMatrices && skinMatrixCount)
			{
				cpuSkinnedVertexData = new gkVertexBuffer(
					operation.vertexData->elementSize, operation.vertexData->elementCount,
					operation.vertexData->vertexType, eBF_Discard);
				skinVertexBuffer(*operation.vertexData, *cpuSkinnedVertexData,
					skinMatrices, skinMatrixCount);
				operation.vertexData = cpuSkinnedVertexData;
			}
			Matrix44 world;
			renderable->getWorldTransforms(&world);
			Matrix44 worldViewProjection = (world * view) * projection;
			if (fontRenderable)
				worldViewProjection.SetIdentity();
			ColorF color(0.8f, 0.75f, 0.65f, 1.0f);
			ITexture* materialTextures[16] = {};
			Vec2 uvTiling(1.0f, 1.0f);
			if (material)
			{
				color = material->diffuseColor();
				color.a *= material->getOpacity() / 100.0f;
				for (uint32 stage = 0; stage < eMS_Invalid; ++stage)
					materialTextures[stage] = material->getTexture(
						static_cast<EMaterialSlot>(stage)).getPointer();
				uvTiling = material->getUVTill();
			}
			if (fontRenderable)
				color = ColorF(1.0f, 1.0f, 1.0f, 1.0f);
			Vec3 localSunDirection(
				world.m00 * m_sunDirection.x + world.m01 * m_sunDirection.y +
					world.m02 * m_sunDirection.z,
				world.m10 * m_sunDirection.x + world.m11 * m_sunDirection.y +
					world.m12 * m_sunDirection.z,
				world.m20 * m_sunDirection.x + world.m21 * m_sunDirection.y +
					world.m22 * m_sunDirection.z);
			localSunDirection.NormalizeSafe(m_sunDirection);
			m_context->drawRenderOperationTextures(operation, worldViewProjection,
				color, materialTextures, 16, uvTiling, transparent,
				material && (material->getShaderMarcoMask() & (1u << 7)) != 0,
				localSunDirection, fontRenderable ||
					(material && material->isDoubleSide()),
				fontRenderable);
			operation.vertexData = originalVertexData;
			delete cpuSkinnedVertexData;
		}
	}
	m_auxRenderer->render(m_context, m_context->width(), m_context->height(),
		viewProjection, cameraPosition, cameraDirection);
	return true;
}

bool gkRendererVulkan::RT_EndRender()
{
	return m_context->endFrame();
}

void gkRendererVulkan::RT_SkipOneFrame(int framecount)
{
	m_skipFrames = framecount;
	RT_CleanRenderSequence();
}

void gkRendererVulkan::FX_ColorGradingTo(gkTexturePtr&, float)
{
}

void gkRendererVulkan::RC_SetSunDir(const Vec3& lightdir)
{
	if (!lightdir.IsZero())
		m_sunDirection = lightdir.GetNormalized();
}

uint32 gkRendererVulkan::GetScreenHeight(bool)
{
	return m_context->height();
}

uint32 gkRendererVulkan::GetScreenWidth(bool)
{
	return m_context->width();
}

HWND gkRendererVulkan::GetWindowHwnd()
{
	return m_context->window();
}

Vec2i gkRendererVulkan::GetWindowOffset()
{
	return Vec2i(0, 0);
}

void gkRendererVulkan::SetCurrContent(HWND hWnd, uint32, uint32,
	uint32 width, uint32 height, bool fullscreen)
{
	if (hWnd && hWnd != m_context->window())
		return;
	m_fullscreen = fullscreen;
	m_context->setFullscreen(fullscreen);
	if (!fullscreen)
		m_context->resize(width, height);
}

IAuxRenderer* gkRendererVulkan::getAuxRenderer()
{
	return m_auxRenderer;
}

ERendererAPI gkRendererVulkan::GetRendererAPI()
{
	return ERdAPI_VULKAN;
}

Ray gkRendererVulkan::GetRayFromScreen(uint32 nX, uint32 nY)
{
	ICamera* camera = m_updatingSequence->camera();
	if (!camera)
		camera = m_renderingSequence->camera();
	if (!camera || GetScreenWidth() == 0 || GetScreenHeight() == 0)
		return Ray(Vec3(ZERO), Vec3(0, 1, 0));

	camera->getDerivedPosition();
	CCamera* renderCamera = camera->getCCam();
	if (!renderCamera)
		return Ray(Vec3(ZERO), Vec3(0, 1, 0));
	const Matrix34& cameraMatrix = renderCamera->GetMatrix();
	const Vec3 forward = cameraMatrix.GetColumn1();
	const Vec3 right = cameraMatrix.GetColumn0();
	const Vec3 up = cameraMatrix.GetColumn2();
	const float width = static_cast<float>(GetScreenWidth());
	const float height = static_cast<float>(GetScreenHeight());
	const float screenX = (2.0f * static_cast<float>(nX) / width) - 1.0f;
	const float screenY = 1.0f - (2.0f * static_cast<float>(nY) / height);
	const float halfHeight = tanf(renderCamera->GetFov() * 0.5f);
	Vec3 direction = forward +
		right * (screenX * halfHeight * renderCamera->GetProjRatio()) +
		up * (screenY * halfHeight);
	direction.NormalizeSafe(forward);
	return Ray(renderCamera->GetPosition(), direction);
}

Vec3 gkRendererVulkan::ProjectScreenPos(const Vec3& worldpos)
{
	ICamera* camera = m_updatingSequence->camera();
	if (!camera)
		camera = m_renderingSequence->camera();
	Vec3 projected(ZERO);
	if (camera && camera->getCCam())
	{
		camera->getDerivedPosition();
		camera->getCCam()->Project(worldpos, projected, Vec2i(0, 0),
			Vec2i(GetScreenWidth(), GetScreenHeight()));
	}
	return projected;
}

Vec3 gkRendererVulkan::ScreenPosToViewportPos(Vec3 screenPos)
{
	const float width = static_cast<float>(GetScreenWidth());
	const float height = static_cast<float>(GetScreenHeight());
	if (width > 0.0f)
		screenPos.x = screenPos.x / width * 2.0f - 1.0f;
	if (height > 0.0f)
		screenPos.y = 1.0f - screenPos.y / height * 2.0f;
	return screenPos;
}

bool gkRendererVulkan::FullScreenMode()
{
	return m_fullscreen;
}
