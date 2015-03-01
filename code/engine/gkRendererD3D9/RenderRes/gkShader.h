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
// Name:   	gkShader.h
// Desc:	todo...
// 描述:	
// 
// Author:  Kaiming-Laptop
// Date:	2010/9/10
// 
//////////////////////////////////////////////////////////////////////////

#ifndef gkShader_h_78475478_d43a_4cd8_9350_e5472ab43ee3
#define gkShader_h_78475478_d43a_4cd8_9350_e5472ab43ee3

#include "RendererD3D9Prerequisites.h"

#include "IShader.h"
#include "IMaterial.h"

class gkShader : public IShader,
	public IFileChangeMonitorListener
{
protected:
	LPD3DXEFFECT			m_pEffect;			// 装载effect的指针 [9/19/2010 Kaiming-Laptop]
	gkStdString				m_internalName;
	gkStdString				m_gfxName;
	bool					m_forceCompile;		// 强制编译，用于reload

	GKHANDLE				m_staticTechnique[eSIT_Count];

public:
	//gkTexture(void);
	gkShader(IResourceManager* creator, const gkStdString& name, gkResourceHandle handle,
		const gkStdString& group = _T("none"));
	virtual ~gkShader(void);

	LPD3DXEFFECT getEffect();
	virtual uint32 getDefaultRenderLayer();
	
	virtual void onReset();
	virtual void onLost();

public:
	virtual bool loadImpl(void);			// 继承实现
	virtual bool loadImpl(IDirect3DDevice9* d3d9Device, LPD3DXEFFECTPOOL* ppPool);		//dx的构建函数

	bool CompileShader( gkStdString &binaryPath, gkStdString &sourcePath, IResFile* &pFile, const TCHAR* profile = NULL );

	virtual bool unloadImpl(void);

	virtual void FX_SetTechniqueByName(LPCSTR name);
	virtual void FX_SetTechnique(GKHANDLE hTech);
	virtual GKHANDLE FX_GetTechniqueByName(LPCSTR name);
	virtual GKHANDLE FX_GetTechnique(EShaderInternalTechnique tech);
	//virtual bool FX_TechniqueSkinned(EShaderInternalTechnique tech);

	virtual void FX_SetBool( GKHANDLE hParam, bool value);
	virtual void FX_SetValue( GKHANDLE hParam, const void* data, UINT bytes);
	virtual void FX_SetFloat( GKHANDLE hParam, float data);
	virtual void FX_SetFloat2( GKHANDLE hParam, const Vec2& data);
	//virtual void FX_SetFloatArray( GKHANDLE hParam, float data);
	virtual void FX_SetFloat3( GKHANDLE hParam, const Vec3& data);
	virtual void FX_SetFloat4( GKHANDLE hParam, const Vec4& data);
	virtual void FX_SetColor4( GKHANDLE hParam, const ColorF& data);
	virtual void FX_SetMatrix(GKHANDLE hParam, const Matrix44& data);
	virtual void FX_SetMatrixArray(GKHANDLE hParam, D3DXMATRIX* data, uint32 size);
	virtual void FX_Begin(UINT* pPasses, DWORD flag);
	virtual void FX_BeginPass(UINT uPass);
	virtual void FX_Commit();
	virtual void FX_End();
	virtual void FX_EndPass();

	virtual void FX_SetTexture( GKHANDLE hParam, gkTexturePtr& pTexture);

	virtual void OnFileChange( const TCHAR* filename );

	virtual gkMaterialParams* getShaderDefaultParams() {return &m_params;}

	bool loadFromGfxShader( CRapidXmlParseNode* rootNode, uint32 mask, IDirect3DDevice9* d3d9Device, ID3DXEffectPool** ppPool);

	virtual ShaderMacros& getShaderMarcos() {return m_shaderMacros;}
	virtual uint32 getShaderMarcoMask() {return m_macroMask;}

	virtual void switchSystemMacro(uint32 systemMarcro = 0);

	gkStdString getMacroProfile(uint32 macromask);
private:
	gkMaterialParams m_params;

	uint32		m_uDefaultRenderLayer;

	ShaderMacros m_shaderMacros;
	uint32 m_macroMask;

	std::map<uint32, LPD3DXEFFECT>	m_macro_effects;			// 装载effect的指针 [9/19/2010 Kaiming-Laptop]
};

#endif // gkShader_h_78475478_d43a_4cd8_9350_e5472ab43ee3
