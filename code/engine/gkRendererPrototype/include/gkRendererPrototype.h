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
// Name:   	gkD3D9RenderSystem.h
// Desc:	抽象出的渲染系统，以方便配合各个部件的工作
// 描述:	
// 
// Author:  Kaiming-Desktop	 
// Date:	9/16/2010 	
// 
//////////////////////////////////////////////////////////////////////////
#pragma once

#include "Prerequisites.h"
#include "IRenderer.h"
#include "gkShaderParamDataSource.h"
#include "ITexture.h"
#include "IShader.h"
#include "RenderThread.h"

struct D3DSURFACE_DESC;
class gkAuxRenderer;


struct gkRenderContent
{
	HWND m_hwnd;
	POINT m_offset;
	POINT m_size;
};

struct gkDynTex
{
	gkTexturePtr m_pTex;
	
	gkDynTex(gkTexturePtr pTex) {m_pTex = pTex;}
	~gkDynTex() {}
};

typedef std::vector<gkDynTex> gkRenderTargetStack;

class gkRendererPrototype : public IRenderer
{
public:
	gkRendererPrototype(void);
	virtual ~gkRendererPrototype(void);

	virtual IDirect3DDevice9*	getDevice() {return NULL;}
	virtual ID3DXEffectPool**	getEffectPoolPointer() {return NULL;}

	virtual IResourceManager*	getResourceManager(BYTE type) {return NULL;}

	virtual HWND Init(ISystemInitInfo& sii) {return 0;}
	virtual void Destroy() {}

	virtual void FX_RenderText( const TCHAR* text, int posx, int posy, ColorB& color = ColorB(255, 255, 255, 180) ){}

	virtual bool CheckDevice(){return true;}
	virtual void _startFrame(){}
	virtual void _endFrame(){}

	virtual void _beginScene(){}
	virtual void _endScene(){}

	virtual void _clearBuffer(bool clearDepth, DWORD color = 0x00000000){}
	virtual void _render(const gkRenderOperation& op, bool isShadowPass = false){}

	// 窗口方法
	void createDefaultRenderWindow(){}
	void setDefaultRenderWindowCamera(ICamera* cam){}

	// RT方法
	IRenderTarget* createRenderTarget(const gkStdString& name){return NULL;}
	const IRenderTarget* getRenderTarget(const gkStdString& name) const{return NULL;}
	void destroyAllRenderTarget(){}

	// RTT方法
	IRenderTarget* createRenderTexture( const gkStdString& name, BYTE rttType, UINT customW = 128, UINT customH = 128 ){return NULL;}
	IRenderTarget* getRenderTexture(const gkStdString& name) const{return NULL;}

	// RT的驱动入口
	bool MT_PrepareRenderSequence(){return true;}
	bool RT_StartRender(){return true;}
	bool RT_EndRender(){return true;}
	virtual IRenderSequence* RT_SwapRenderSequence() {return NULL;}

	bool RP_RenderScene(){return true;}

	UINT GetScreenHeight(){return 0;}
	UINT GetScreenWidth(){return 0;}
	bool CheckDeviceLost(){return true;}
	
	virtual void RS_EnableAlphaTest(bool enable, DWORD ref = 0x00000060){}
	virtual void RS_SetRenderState(FX_D3DRENDERSTATETYPE rs, DWORD value){}
	bool SetWindow(int width, int height, bool fullscreen = false, HWND hWnd = 0){return true;}

	virtual void SetCurrContent(HWND hWnd, LONG posx, LONG posy, LONG width, LONG height){}

	IAuxRenderer* getAuxRenderer(){return reinterpret_cast<IAuxRenderer*>(m_pAuxRenderer);}
	
	virtual void backupOpaque(){}
	
	virtual void RC_SetTodKey(const STimeOfDayKey& key){}
	virtual void RC_SetMainCamera(const CCamera* cam){}
	virtual void RC_SetShadowCascadeCamera(const CCamera* cam, uint8 index){}
	virtual void RC_SetSunDir(const Vec3& lightdir){}

	virtual void setLightAmbient(const ColorF& ambient){}
	virtual void setLightDiffuse(const ColorF& diffuse){}

	// pipeline control

	// for every shadow shader group call
	virtual void renderShadowZPassObjects( const gkRenderableList* objs, uint8 cascade, BYTE sortType = 0 ){}

	// for every frame, call once, shared the visibility test with main cam
	virtual void RP_GenReflectExcute( const gkRenderableList* objs, IShader* pShader, BYTE sortType /*= 0*/){}

	virtual void RP_ZprepassPrepare(){}
	// for every frame, call once, shared the visibility test with main cam
	virtual void renderZPassObjects( const gkRenderableList* objs, gkShaderPtr pShader, BYTE sortType /*= 0*/){}
	virtual void RP_ZprepassEnd(){}

	virtual void RP_DeferredLightExcute(const gkRenderLightList& LightList){}

	virtual void RP_GeneralPrepare(){}

	// for every shader group call
	virtual void renderObjects( const gkRenderableList* objs, IShader* pShader, BYTE sortType /*= 0*/){}

	virtual void RP_GeneralEnd(){}

	virtual void RP_DeferredSnow(){}
	virtual void RP_FogProcess(){}
	virtual void RP_HDRProcess(){}
	virtual void RP_FinalOutput(){}
	virtual void RP_FXSpecil(){}
	virtual void RP_SetHDRParams(gkShaderPtr pShader){}

	virtual void SetDebugDynTex(int type) {}
	
	virtual IResourceManager* getTextureMngPtr() {return m_pTextureManager;}
	virtual IResourceManager* getMaterialMngPtr() {return m_pMaterialManager;}
	virtual IResourceManager* getMeshMngPtr() {return m_pMeshManager;}
	virtual IResourceManager* getShaderMngPtr() {return m_pShaderManager;}

	virtual void RT_CleanRenderSequence();

	virtual void RT_SkipOneFrame();

	virtual void FX_ColorGradingTo( gkTexturePtr& pCch, float fAmount );

	virtual void RP_StretchRefraction();


private:
	// resource management
	static IResourceManager*			m_pTextureManager;
	static IResourceManager*			m_pMeshManager;
	static IResourceManager*			m_pMaterialManager;
	static IResourceManager*			m_pShaderManager;

	// aux renderer
	static gkAuxRenderer*				m_pAuxRenderer;
};

gkRendererPrototype* getRenderer();
