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

#ifndef gkShader_h_
#define gkShader_h_

#include "Prerequisites.h"

#include "IShader.h"

// Index to bind the attributes to vertex shaders
const int GK_SHADER_VERTEX_ARRAY = 0;
const int GK_SHADER_TEXCOORD_ARRAY = 1;
const int GK_SHADER_TANGENT_ARRAY = 2;
const int GK_SHADER_BINORMAL_ARRAY = 3;
const int GK_SHADER_BLENDWIGHT_ARRAY = 4;
const int GK_SHADER_BLENDINDEX_ARRAY = 5;

const int GK_SHADER_DWORDCOLOR_ARRAY = 1;

struct CRapidXmlParseNode;

class gkShaderGLES2 : public IShader
	,public IFileChangeMonitorListener
{
protected:
	GLuint				m_VertexShader; 
	GLuint				m_FragmentShader;
	GLuint				m_Program;
	gkStdString			m_internalName;
	std::map<uint32, GLuint> m_macroPrograms;

public:
	//gkTexture(void);
	gkShaderGLES2(IResourceManager* creator, const gkStdString& name, gkResourceHandle handle,
		const gkStdString& group = _T("none"));
	virtual ~gkShaderGLES2(void);

	virtual uint32 getDefaultRenderLayer();

	virtual void onReset();
	virtual void onLost();

public:
	virtual bool loadImpl(void);			// 继承实现
	virtual bool unloadImpl(void);

	virtual void FX_SetTechniqueByName(LPCSTR name);
	virtual void FX_SetTechnique(GKHANDLE hTech);
	virtual GKHANDLE FX_GetTechniqueByName(LPCSTR name);

	virtual void FX_SetBool( GKHANDLE hParam, bool value);
	virtual void FX_SetValue( GKHANDLE hParam, const void* data, uint32 bytes);
	virtual void FX_SetFloat( GKHANDLE hParam, float data);
	virtual void FX_SetFloat3( GKHANDLE hParam, const Vec3& data);
	virtual void FX_SetFloat4( GKHANDLE hParam, const Vec4& data);
	virtual void FX_SetColor4( GKHANDLE hParam, const ColorF& data);
	virtual void FX_SetMatrix(GKHANDLE hParam, const Matrix44& data);
	virtual void FX_SetMatrixArray(GKHANDLE hParam, D3DXMATRIX* data, uint32 size);
	virtual void FX_Begin(uint32* pPasses, DWORD flag);
	virtual void FX_BeginPass(uint32 uPass);
	virtual void FX_Commit();
	virtual void FX_End();
	virtual void FX_EndPass();

	virtual void FX_SetTexture( GKHANDLE hParam, gkTexturePtr& pTexture);

	virtual gkMaterialParams* getShaderDefaultParams() {return NULL;}

	virtual ShaderMacros& getShaderMarcos() {return m_shaderMacros;}
	virtual uint32 getShaderMarcoMask() {return m_macroMask;}
    virtual uint32 getRealTimeShaderMarco() {return m_rtMacroMask;}
private:

	bool loadFromGfxShader( CRapidXmlParseNode* rootNode);

	bool buildShader( uint32 marco_mask );
	bool CompileShader( const char* source, GLuint& shader, GLenum type );

	void PreProcess( std::string &vsSource );

	uint32		m_uDefaultRenderLayer;
	struct ShaderState
	{
		GLboolean		isCullFaceEnabled;
		GLboolean		isBlendEnabled;
		GLboolean		isDepthTestEnabled;
		GLint		nArrayBufferBinding;
		GLint		eFrontFace;
		GLint		eCullFaceMode;

		bool		enable;

		ShaderState()
		{
			isCullFaceEnabled = true;
			isBlendEnabled = false;
			isDepthTestEnabled = true;

			eFrontFace = GL_CCW;
			eCullFaceMode = GL_BACK;
			nArrayBufferBinding = 0;

			enable = false;
		}
	};

	bool ApplyState();
	bool RevertState();

	bool StateRestoreInternal(ShaderState& state);
	bool StateApplyInternal(ShaderState& newState, ShaderState& oldState);

	virtual GKHANDLE FX_GetTechnique( EShaderInternalTechnique tech );

	virtual bool FX_TechniqueSkinned( EShaderInternalTechnique tech );

	virtual void OnFileChange( const TCHAR* filename );

	virtual void switchSystemMacro(uint32 systemMarcro = 0);

	ShaderState m_prevState;
	ShaderState m_thisState;

	ShaderMacros m_shaderMacros;
	uint32 m_macroMask;
    uint32 m_rtMacroMask;
    
	gkStdString				m_vs_file_name;
	gkStdString				m_ps_file_name;
};

#endif // gkShader_h_78475478_d43a_4cd8_9350_e5472ab43ee3
