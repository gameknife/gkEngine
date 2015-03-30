//////////////////////////////////////////////////////////////////////////
/*
Copyright (c) 2011-2015 Kaiming Yi
	
	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:
	
	The above copyright notice and this permission notice shall be included in
	all copies or substantial portions of the Software.
	
	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
	THE SOFTWARE.
	
*/
//////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////
//
// yikaiming (C) 2013
// gkENGINE Source File 
//
// Name:   	gkRendererGL330.h
// Desc:	
// 	
// 
// Author:  YiKaiming
// Date:	2013/7/18
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _gkRendererGL320_h_
#define _gkRendererGL320_h_

#include "Prerequisites.h"
#include "gkShaderParamDataSource.h"
#include "gkRenderOperation.h"

class gkAuxRendererGLES2;
struct gkShaderGroup;
struct gkRenderableList;
class gkShaderGLES2;
struct IDeviceRenderContext;
struct gkDynTex
{
	gkTexturePtr m_pTex;
	bool m_needDS;
	gkDynTex(gkTexturePtr pTex, bool needDS) { m_pTex = pTex; m_needDS = needDS; }
	~gkDynTex() {}
};

struct gkFBO
{
	ITexture* m_colorRT;
	ITexture* m_depthRT;
	GLuint m_hwFBO;

	bool m_need_Stencil;

	uint32 width;
	uint32 height;

	gkFBO(gkTexturePtr color, gkTexturePtr depth, bool need_Stencil)
	{
		m_colorRT = color.getPointer();
		m_depthRT = depth.getPointer();
		m_need_Stencil = need_Stencil;
	}

	bool operator== (const gkFBO& other)
	{
		if (other.m_colorRT != m_colorRT || other.m_depthRT != m_depthRT || m_need_Stencil != other.m_need_Stencil)
		{
			return false;
		}
		return true;
	}
};

typedef std::vector<gkFBO*> gkFBOStack;

struct gkFBOMap
{
	typedef std::vector<gkFBO> gkFBOs;
	gkFBOs m_fbos;

	gkFBO* find(gkFBO& fbo)
	{
		for (uint32 i = 0; i < m_fbos.size(); ++i) {
			if (m_fbos[i] == fbo)
			{
				return &(m_fbos[i]);
			}
		}

		return NULL;
	}

	void push_back(gkFBO& fbo)
	{
		m_fbos.push_back(fbo);
	}
};


typedef std::vector<gkDynTex> gkRenderTargetStack;
typedef std::map<gkVertexBuffer*, GLuint> gkVAOmapping;

class gkRendererGL : public IRenderer
{
public:
	gkRendererGL(void);
	~gkRendererGL(void);

	virtual IResourceManager*	getResourceManager(BYTE type);
	virtual HWND Init(ISystemInitInfo& sii);
	virtual void Destroy();
	virtual void _render(const gkRenderOperation& op, bool isShadowPass = false);

	void apply_vertex_layout(const gkRenderOperation &op);

	// GPU粒子更新ROUTINE接口 [4/12/2013 YiKaiming]
	virtual IParticleProxy* createGPUParticleProxy() { return NULL; }
	virtual void destroyGPUParticelProxy(IParticleProxy*) {}
	// RT的驱动入口
	bool RT_StartRender();
	bool RT_EndRender();
	virtual IRenderSequence* RT_SwapRenderSequence();
	virtual IRenderSequence* RT_GetRenderSequence();

	bool RP_RenderScene(){ return true; }

	uint32 GetScreenHeight(bool forceOrigin = false);
	uint32 GetScreenWidth(bool forceOrigin = false);

	virtual HWND GetWindowHwnd() { return m_hWnd; }
	virtual void SetCurrContent(HWND hWnd, uint32 posx, uint32 posy, uint32 width, uint32 height, bool fullscreen = false);
	IAuxRenderer* getAuxRenderer(){ return reinterpret_cast<IAuxRenderer*>(m_pAuxRenderer); }

	virtual void RC_SetMainCamera(const CCamera* cam);
	virtual void RC_SetShadowCascadeCamera(const CCamera* cam, uint8 index);
	virtual void RC_SetSunDir(const Vec3& lightdir);

	virtual IResourceManager* getTextureMngPtr() { return m_pTextureManager; }
	virtual IResourceManager* getMaterialMngPtr() { return m_pMaterialManager; }
	virtual IResourceManager* getMeshMngPtr() { return m_pMeshManager; }
	virtual IResourceManager* getShaderMngPtr() { return m_pShaderManager; }

	virtual void RT_CleanRenderSequence();

	virtual void RT_SkipOneFrame(int framecount);

	virtual void FX_ColorGradingTo(gkTexturePtr& pCch, float fAmount);

	void RP_ProcessZpassObjects(const gkRenderableList* objs, IShader* pShader, bool skinned /*= 0*/);
	// for evert renderlayer
	void RP_ProcessRenderLayer(uint8 layerID, bool skinned = false, DWORD systemMacro = 0);
	// for every shader group call
	void RP_ProcessShaderGroup(const gkShaderGroup* pShaderGroup, bool skinned);
	// for every material group call
	void RP_ProcessRenderList(std::list<gkRenderable*>& renderableList, gkShaderGLES2* pShader, int shadowcascade = -1);

	static gkShaderParamDataSource& getShaderContent() { return m_ShaderParamDataSource; }
#ifdef RENDERAPI_GL330
	virtual ERendererAPI GetRendererAPI() { return ERdAPI_OPENGL; }
#else
	virtual ERendererAPI GetRendererAPI() { return ERdAPI_OPENGLES; }
#endif
	virtual Ray GetRayFromScreen(uint32 nX, uint32 nY);
	virtual Vec3 ProjectScreenPos(const Vec3& worldpos);

	virtual Vec2i GetWindowOffset();

	virtual void SetRenderState(uint32 key, uint32 value);

	virtual void FX_DrawScreenImage(gkTexturePtr tex, Vec4& region) {}

	void FX_PushRenderTarget(uint8 channel, gkTexturePtr src, gkTexturePtr depth, bool needStencil = false);
	void FX_PopRenderTarget(uint8 channel);
	void FX_DiscardRenderTarget();

	void FX_DrawScreenQuad();
	void FX_DrawScreenQuad(Vec4 region);

	void FX_TexBlurGaussian(gkTexturePtr tgt, int nAmount, float fScale, float fDistribution, gkTexturePtr tmp, int iterate = 1);

	IDeviceRenderContext* getDeviceContex();

	virtual Vec3 ScreenPosToViewportPos(Vec3 screenPos);
private:
	uint32 m_screenWidth;
	uint32 m_screenHeight;

	static IResourceManager*			m_pTextureManager;
	static IResourceManager*			m_pMeshManager;
	static IResourceManager*			m_pMaterialManager;
	static IResourceManager*			m_pShaderManager;

	static gkAuxRendererGLES2*			m_pAuxRenderer;
	static gkShaderParamDataSource		m_ShaderParamDataSource;

	class gkRenderSequence*			m_pUpdatingRenderSequence;
	gkRenderSequence*					m_pRenderingRenderSequence;

	uint32 m_uScreenWidth;
	uint32 m_uScreenHeight;

	gkVAOmapping m_vaoMap;

	IDeviceRenderContext*				m_pDeviceContext;

	GLint m_backbufferFBO;
	GLuint m_rtFBO;
	GLuint m_drtFBO;
    
	GLuint m_stencil;

	std::vector<IFtFont*> m_fonts;

    gkFBOMap m_fboMap;
    gkFBOStack m_fboStack;

	HWND m_hWnd;

	int m_bSkipThisFrame;

	GLuint tmpVAO;
	GLuint tmpVBO;
};

gkRendererGL* getRenderer();

#endif

