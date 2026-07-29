#include "gkRendererVulkan.h"
#include "gkVkDeviceContext.h"

#include "IAuxRenderer.h"
#include "IRenderSequence.h"
#include "IResourceManager.h"
#include "ICamera.h"
#include "gkRenderable.h"
#include "gk_Camera.h"
#include "gkVkResources.h"

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
			float totalWeight = 0.0f;
			for (uint32 influence = 0; influence < 4; ++influence)
			{
				if (indices[influence] >= matrixCount || weights[influence] == 0)
					continue;
				const float weight = weights[influence] / 255.0f;
				skinned += transformSkinPointRowVector(
					matrices[indices[influence]], input->position) * weight;
				totalWeight += weight;
			}
			output->position = totalWeight > 0.0f ? skinned / totalWeight : input->position;
		}
		destination.m_needRebind = true;
	}
}

class gkVkRenderSequence : public IRenderSequence
{
public:
	gkVkRenderSequence()
		: m_camera(NULL)
		, m_fillMode(eRFMode_General)
	{
	}

	virtual void addToRenderSequence(gkRenderable* renderable, BYTE)
	{
		if (!renderable || m_fillMode != eRFMode_General)
			return;
		IMaterial* material = renderable->getMaterial();
		if (material)
			material->touch();
		m_renderables.push_back(renderable);
	}
	virtual void addToRenderSequence(gkRenderable* renderable)
	{
		addToRenderSequence(renderable, 0);
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
		m_renderables.clear();
		m_camera = NULL;
		m_fillMode = eRFMode_General;
	}

	const std::vector<gkRenderable*>& renderables() const { return m_renderables; }
	ICamera* camera() const { return m_camera; }

private:
	std::vector<gkRenderable*> m_renderables;
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

	virtual void AuxRender3DLine(const Vec3&, const Vec3&, ColorF&, bool) {}
	virtual void AuxRender3DGird(const Vec3&, int, float, ColorF&, bool) {}
	virtual void AuxRender3DBoxFrameRotated(const Vec3&, const Quat&, float, ColorF&, bool) {}
	virtual void AuxRender3DBoxFrameRotated(const Vec3&, const Quat&, const Vec3&, ColorF&, bool) {}
	virtual void AuxRender3DBoxFrame(const Vec3&, float, ColorF&, bool) {}
	virtual void AuxRender3DBoxFrame(const Vec3&, Vec3&, ColorF&, bool) {}
	virtual void AuxRender3DCircle(const Vec3&, const Vec3&, float, uint32, ColorF&, bool) {}
	virtual void AuxRenderAABB(const AABB&, ColorF&, bool) {}
	virtual void AuxRenderGizmo(const Matrix44&, float, uint8, bool, uint8) {}
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
	}
	virtual void _cleanBuffer()
	{
		m_updatingQuads.clear();
		m_renderingQuads.clear();
	}

	void prepare(gkVkDeviceContext* context)
	{
		for (size_t i = 0; i < m_renderingQuads.size(); ++i)
			context->prepareTexture(m_renderingQuads[i].texture);
	}

	void render(gkVkDeviceContext* context, uint32 width, uint32 height)
	{
		if (!width || !height)
			return;
		const Matrix44 identity(IDENTITY);
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

	const std::vector<gkRenderable*>& renderables = m_renderingSequence->renderables();
	m_context->prepareTexture(NULL);
	for (size_t i = 0; i < renderables.size(); ++i)
	{
		gkVkMaterial* material = renderables[i] ?
			dynamic_cast<gkVkMaterial*>(renderables[i]->getMaterial()) : NULL;
		if (material)
			m_context->prepareTexture(
				material->getTexture(eMS_Diffuse).getPointer());
	}
	m_auxRenderer->prepare(m_context);
	for (uint32 renderPass = 0; renderPass < 2; ++renderPass)
	{
		for (size_t i = 0; i < renderables.size(); ++i)
		{
			gkRenderable* renderable = renderables[i];
			if (!renderable)
				continue;
			gkVkMaterial* material =
				dynamic_cast<gkVkMaterial*>(renderable->getMaterial());
			const bool fontRenderable =
				renderable->getMaterialName() == _T("FontMaterial");
			const bool transparent = fontRenderable ||
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
			ITexture* diffuseTexture = NULL;
			Vec2 uvTiling(1.0f, 1.0f);
			if (material)
			{
				color = material->diffuseColor();
				color.a *= material->getOpacity() / 100.0f;
				diffuseTexture = material->getTexture(eMS_Diffuse).getPointer();
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
			m_context->drawRenderOperation(operation, worldViewProjection,
				color, diffuseTexture, uvTiling, transparent,
				material && (material->getShaderMarcoMask() & (1u << 7)) != 0,
				localSunDirection, fontRenderable ||
					(material && material->isDoubleSide()),
				fontRenderable);
			operation.vertexData = originalVertexData;
			delete cpuSkinnedVertexData;
		}
	}
	m_auxRenderer->render(m_context, m_context->width(), m_context->height());
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
