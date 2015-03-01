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

struct gkFBO
{
    ITexture* m_colorRT;
    ITexture* m_depthRT;
    GLuint m_hwFBO;
    
    uint32 width;
    uint32 height;
    
    gkFBO(gkTexturePtr color, gkTexturePtr depth)
    {
        m_colorRT = color.getPointer();
        m_depthRT = depth.getPointer();
    }
    
    bool operator== ( const gkFBO& other)
    {
        if( other.m_colorRT != m_colorRT || other.m_depthRT != m_depthRT)
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
            for (uint32 i=0; i < m_fbos.size(); ++i) {
                if( m_fbos[i] == fbo )
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
    
typedef std::map<gkVertexBuffer*, GLuint> gkVAOmapping;

class gkRendererGL330 : public IRenderer
{
public:
	gkRendererGL330(void);
	~gkRendererGL330(void);

	virtual IResourceManager* getResourceManager( BYTE type );

	virtual HWND Init( struct ISystemInitInfo& sii );

	virtual void Destroy();

	virtual IParticleProxy* createGPUParticleProxy();

	virtual void destroyGPUParticelProxy( IParticleProxy* );

	virtual IRenderSequence* RT_SwapRenderSequence();

	virtual IRenderSequence* RT_GetRenderSequence();

	virtual void RT_CleanRenderSequence();

	virtual bool RT_StartRender();

	virtual bool RT_EndRender();

	virtual void RT_SkipOneFrame( int framecount = 1 );

	virtual void FX_ColorGradingTo( gkTexturePtr& pCch, float fAmount );

	virtual void RC_SetSunDir( const Vec3& lightdir );

	virtual uint32 GetScreenHeight(bool forceOrigin = false);

	virtual uint32 GetScreenWidth(bool forceOrigin = false);

	virtual HWND GetWindowHwnd();

	virtual Vec2i GetWindowOffset();

	virtual void SetCurrContent( HWND hWnd, uint32 posx, uint32 posy, uint32 width, uint32 height, bool fullscreen = false );

	virtual IAuxRenderer* getAuxRenderer();

	virtual ERendererAPI GetRendererAPI() {return ERdAPI_OPENGL;}

	virtual Ray GetRayFromScreen( uint32 nX, uint32 nY );

	virtual Vec3 ProjectScreenPos( const Vec3& worldpos );

	static gkShaderParamDataSource& getShaderContent() {return m_ShaderParamDataSource;}

	void FX_PushRenderTarget(uint8 channel, gkTexturePtr src, gkTexturePtr depth, bool bClearTarget = false);
	void FX_PopRenderTarget(uint8 channel);

	void FX_DrawScreenQuad();
	void FX_DrawScreenQuad(Vec4 region);

	void FX_TexBlurGaussian(gkTexturePtr tgt, int nAmount, float fScale, float fDistribution, gkTexturePtr tmp, int iterate = 1);


private:
	void _render(const gkRenderOperation& op, bool isShadowPass = false);
	// for evert renderlayer
	void RP_ProcessRenderLayer( uint8 layerID );
	// for every shader group call
	void RP_ProcessShaderGroup( const gkShaderGroup* pShaderGroup, BYTE sortType);
	// for every material group call
	void RP_ProcessRenderList(  const gkRenderableList* obj, gkShaderGLES2* pShader, int shadowcascade = -1 );

	virtual void RC_SetMainCamera(const CCamera* cam);
	virtual void RC_SetShadowCascadeCamera(const CCamera* cam, uint8 index);

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

	gkVAOmapping m_vaoMap;

	IDeviceRenderContext*				m_deviceContext;

	GLint m_backbufferFBO;
	GLuint m_rtFBO;
	GLuint m_drtFBO;
    
    gkFBOMap m_fboMap;
    gkFBOStack m_fboStack;

	HWND m_hWnd;

	int m_bSkipThisFrame;
};

gkRendererGL330* getRenderer();

#endif

