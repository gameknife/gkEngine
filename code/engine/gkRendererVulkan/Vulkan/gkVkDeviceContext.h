#pragma once

#include "gkPlatform.h"
#include "gkRenderOperation.h"
#include "gk_Color.h"
#include "ITexture.h"

struct ISystemInitInfo;

class gkVkDeviceContext
{
public:
	gkVkDeviceContext();
	~gkVkDeviceContext();

	HWND initialize(const ISystemInitInfo& initInfo);
	void shutdown();

	bool beginFrame();
	bool endFrame();
	bool prepareTexture(ITexture* texture);
	bool drawRenderOperation(const gkRenderOperation& operation,
		const Matrix44& worldViewProjection, const ColorF& color,
		ITexture* texture, const Vec2& uvTiling, bool transparent = false,
		bool alphaTest = false,
		const Vec3& lightDirection = Vec3(-0.35f, -0.45f, 0.82f),
		bool doubleSided = false, bool alphaOnlyTexture = false,
		bool overlay = false);
	void resize(uint32 width, uint32 height);
	void setFullscreen(bool fullscreen);

	HWND window() const;
	uint32 width() const;
	uint32 height() const;
	bool minimized() const;
	float gpuFrameTimeMs() const;

private:
	struct Impl;
	Impl* m_impl;
};
