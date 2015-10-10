#include "StableHeader.h"
#include "IRenderer.h"
#include "gkShader.h"
#include "gkIterator.h"
#include "gkFilePath.h"
#include "IResFile.h"

#include "IXmlUtil.h"

#include "gkMemoryLeakDetecter.h"




gkShaderGLES2::gkShaderGLES2( IResourceManager* creator, const gkStdString& name, gkResourceHandle handle, const gkStdString& group /*= _T("none")*/ )
	:IShader(creator,name,handle,group)
{
	m_uDefaultRenderLayer = -1;

	// creation time, add to file change monitor
#ifdef OS_WIN32
	gEnv->pFileChangeMonitor->AddListener(this);
#endif
    
    m_currUniformGroup = &m_defaultUniformGroup;
}

gkShaderGLES2::~gkShaderGLES2( void )
{
	if(isLoaded())
		unload();
#ifdef OS_WIN32
	gEnv->pFileChangeMonitor->RemoveListener(this);
#endif
}

bool gkShaderGLES2::loadImpl( void )
{
	gkLogMessage( _T("start load shader[%s]"), m_wstrFileName.c_str());

	HRESULT hr = S_OK;

	// 先在目录内查找名字 [9/18/2010 Kaiming-Desktop]
	TCHAR pureName[MAX_PATH];
	_tcscpy(pureName, m_wstrFileName.c_str());

	TCHAR* strLastSlash = NULL;
	// 摘除扩展名
	strLastSlash = _tcsrchr( pureName, _T( '.' ) );
	if( strLastSlash )
	{
		*strLastSlash = 0;
	}

	// 取得MASK并摘除
	strLastSlash = _tcsrchr( pureName, _T( '@' ) );
	uint32 mask = 0;
	if (strLastSlash)
	{
		TCHAR strmask[MAX_PATH];
		_tcscpy(strmask, strLastSlash + 1);
		*strLastSlash = 0;

		gkStdStringstream ss(strmask);
		ss >> mask;
	}
	m_macroMask = mask;

	bool legacy_routine = false;

	gkStdString gfxPath = _T("engine/shaders/template/built_in/") + gkStdString(pureName) + _T(".gfx");

	if( gEnv->pFileSystem->checkFileExist( gfxPath.c_str() ) == eFS_notExsit )
	{
		gfxPath = _T("engine/shaders/template/hidden/") + gkStdString(pureName) + _T(".gfx");

		if( gEnv->pFileSystem->checkFileExist( gfxPath.c_str() )  == eFS_notExsit )
		{
			gfxPath = _T("engine/shaders/template/extern/") + gkStdString(pureName) + _T(".gfx");

			if( gEnv->pFileSystem->checkFileExist( gfxPath.c_str() )  == eFS_notExsit )
			{
				legacy_routine = true;
			}
		}
	}

	m_internalName = gkStdString(_T("engine/shaders/gles2/")) + gkStdString(pureName) + gkStdString(_T(".ffx"));

	gkLogMessage( _T("start process shadefiler[%s]"), gfxPath.c_str());

	// load gfx
	IRapidXmlParser parser;
	parser.initializeReading( gfxPath.c_str() );

	if (!parser.getRootXmlNode())
	{
		parser.finishReading();
		return false;
	}

	bool ret = false;

	ret = loadFromGfxShader( parser.getRootXmlNode() );

	parser.finishReading();

	return ret;
}


bool gkShaderGLES2::loadFromGfxShader( CRapidXmlParseNode* rootNode )
{
	CRapidXmlParseNode* marcoNode = rootNode->getChildNode(_T("Marco"));
	CRapidXmlParseNode* layerNode = rootNode->getChildNode(_T("RenderLayer"));

	if (layerNode)
	{
		if ( !_tcsicmp( layerNode->GetAttribute(_T("layer")), _T("WATER") ) )
		{
			m_uDefaultRenderLayer = RENDER_LAYER_WATER;
		}

		if ( !_tcsicmp( layerNode->GetAttribute(_T("layer")), _T("RENDER_LAYER_SKIES_EARLY") ) )
		{
			m_uDefaultRenderLayer = RENDER_LAYER_SKIES_EARLY;
		}
	}

	if (marcoNode)
	{
		m_shaderMacros.clear();

		// add system macro
		m_shaderMacros[eSSM_Skinned].m_name = _T("SKIN");
		m_shaderMacros[eSSM_Skinned].m_value = _T("");
		m_shaderMacros[eSSM_Skinned].m_desc = _T("Skinning");
		m_shaderMacros[eSSM_Skinned].m_active = false;
		m_shaderMacros[eSSM_Skinned].m_hidden = true;

		m_shaderMacros[eSSM_Zpass].m_name = _T("ZPASS");
		m_shaderMacros[eSSM_Zpass].m_value = _T("");
		m_shaderMacros[eSSM_Zpass].m_desc = _T("Zpass");
		m_shaderMacros[eSSM_Zpass].m_active = false;
		m_shaderMacros[eSSM_Zpass].m_hidden = true;

		CRapidXmlParseNode* marco_sub = marcoNode->getChildNode();
		for (; marco_sub; marco_sub = marco_sub->getNextSiblingNode())
		{
			// every marco
			TCHAR* name = marco_sub->GetAttribute( _T("name") );
			TCHAR* value = marco_sub->GetAttribute( _T("value") );
			TCHAR* desc = marco_sub->GetAttribute(_T("desc") );

			int maskflag = 0;
			marco_sub->GetAttribute( _T("mask"), maskflag );

			if (maskflag != 0)
			{
				m_shaderMacros[maskflag].m_name = name;
				m_shaderMacros[maskflag].m_value = value;
				m_shaderMacros[maskflag].m_desc = desc;
				m_shaderMacros[maskflag].m_active = false;
			}


			if ( m_macroMask & maskflag)
			{
				m_shaderMacros[maskflag].m_active = true;
			}
		}
	}

	CRapidXmlParseNode* glesNode = NULL;
	if (rootNode->getChildNode(_T("GLES2Shader")) )
	{
		glesNode = rootNode->getChildNode(_T("GLES2Shader"));
	}
	else
	{
		glesNode = rootNode;
	}

	// read vfx & ffx file
	CRapidXmlParseNode* vsnode = glesNode->getChildNode(_T("VSShader"));
	CRapidXmlParseNode* psnode = glesNode->getChildNode(_T("PSShader"));
	CRapidXmlParseNode* statenode = glesNode->getChildNode(_T("StateStage"));

	if (vsnode && psnode)
	{
		m_vs_file_name = vsnode->GetAttribute(_T("FileName"));
		m_ps_file_name = psnode->GetAttribute(_T("FileName"));

		// compile shader
		//switchSystemMacro( eSSM_Skinned );

		switchSystemMacro( 0 );
	}
	else
	{
		return false;
	}

	if (statenode)
	{
		int count = 0;
		statenode->GetAttribute(_T("Count"), count);

		CRapidXmlParseNode* states = statenode->getChildNode();
		for ( ;states; states = states->getNextSiblingNode() )
		{
			// cullmode
			if(!_tcsicmp( states->GetAttribute(_T("name")), _T("cullmode")))
			{

				if(!_tcsicmp( states->GetAttribute(_T("type")), _T("none")))
				{
					m_thisState.isCullFaceEnabled = false;
				}
				else if(!_tcsicmp( states->GetAttribute(_T("type")), _T("ccw")))
				{
					m_thisState.isCullFaceEnabled = true;
					m_thisState.eFrontFace = GL_CCW;
					m_thisState.eCullFaceMode = GL_BACK;
				}
				else if(!_tcsicmp( states->GetAttribute(_T("type")), _T("cw")))
				{
					m_thisState.isCullFaceEnabled = true;
					m_thisState.eFrontFace = GL_CW;
					m_thisState.eCullFaceMode = GL_BACK;
				}

				m_thisState.enable = true;
			}

			// BLEND
			if(!_tcsicmp( states->GetAttribute(_T("name")), _T("alphablend")))
			{

				if(!_tcsicmp( states->GetAttribute(_T("type")), _T("true")))
				{
					m_thisState.isBlendEnabled = true;
				}
				else if(!_tcsicmp( states->GetAttribute(_T("type")), _T("false")))
				{
					m_thisState.isBlendEnabled = false;
				}

				m_thisState.enable = true;
			}
		}
	}


	return true;
}



bool gkShaderGLES2::unloadImpl(void)
{
	std::map<uint32, MacroGroup>::iterator it = m_macroPrograms.begin();
	for (;it != m_macroPrograms.end();++it)
	{
		GLuint program = it->second.first;
		glDeleteProgram(program); 
	}
	m_macroPrograms.clear();
	return true;
}

//-----------------------------------------------------------------------
void gkShaderGLES2::onReset()
{
	touch();
}
//-----------------------------------------------------------------------
void gkShaderGLES2::onLost()
{

}
//-----------------------------------------------------------------------
void gkShaderGLES2::FX_SetTechniqueByName( LPCSTR name )
{

}
//-----------------------------------------------------------------------
void gkShaderGLES2::FX_SetTechnique( GKHANDLE hTech )
{

}
//-----------------------------------------------------------------------
GKHANDLE gkShaderGLES2::FX_GetTechniqueByName( LPCSTR name )
{
	return 0;
}
//////////////////////////////////////////////////////////////////////////
void gkShaderGLES2::FX_SetBool( GKHANDLE hParam, bool value )
{

}
//-----------------------------------------------------------------------
void gkShaderGLES2::FX_SetValue( GKHANDLE hParam, const void* data, uint32 bytes )
{
	bool vec = false;
	switch ( bytes)
	{
	case 4 * sizeof(float):
		{
			const Vec4& tmp = *((const Vec4*)data);
			FX_SetFloat4( hParam, tmp);
		}
		break;
	case 2 * sizeof(float):
		{
			int location = get_cache_handle_by_name(hParam);
			const Vec2& tmp = *((const Vec2*)data);
			if(location != -1) glUniform2f( location, tmp.x, tmp.y );
		}
		break;
	case 1 * sizeof(float):
		{
			int location = get_cache_handle_by_name(hParam);
			if(location != -1) glUniform1f( location, *((float*)data) );
		}
		break;
	default:
		vec = true;
		break;
	}

	if ( vec && (bytes % sizeof(Vec4) == 0) && (bytes / sizeof(Vec4) > 1))
	{
		int location = get_cache_handle_by_name(hParam);
		if(location != -1) glUniform4fv( location, bytes / sizeof(Vec4), (const float*)data);
	}
}

int gkShaderGLES2::get_cache_handle_by_name(GKHANDLE hParam)
{
    if (m_currUniformGroup == nullptr) {
        return 0;
    }
    if(m_Program == 0)
    {
        return 0;
    }
    gkStdString name(hParam);
    auto it = m_currUniformGroup->find(name);
    if( it != m_currUniformGroup->end() )
    {
        return it->second;
    }
    else
    {
        int location = glGetUniformLocation(m_Program,hParam);
        m_currUniformGroup->insert( std::map<std::string, int>::value_type( name, location) );
        return location;
    }
}

void gkShaderGLES2::FX_SetMatrix( GKHANDLE hParam, const Matrix44& data )
{
	// bind MVP MATRIX
	int location = get_cache_handle_by_name(hParam);
    if(location != -1) glUniformMatrix4fv( location, 1, GL_FALSE, &(data.m00) );
}

//-----------------------------------------------------------------------
void gkShaderGLES2::FX_SetFloat( GKHANDLE hParam, float data)
{
	FX_SetValue(hParam, &data, sizeof(float));
}

void gkShaderGLES2::FX_SetFloat3( GKHANDLE hParam, const Vec3& data )
{
	int location = get_cache_handle_by_name(hParam);
	if(location != -1) glUniform3f( location, data.x, data.y, data.z );
}

//-----------------------------------------------------------------------
void gkShaderGLES2::FX_SetFloat4( GKHANDLE hParam, const Vec4& data )
{
	int location = get_cache_handle_by_name(hParam);
	if(location != -1) glUniform4f( location, data.x, data.y, data.z, data.w );
}
//////////////////////////////////////////////////////////////////////////
void gkShaderGLES2::FX_SetColor4( GKHANDLE hParam, const ColorF& data )
{
	FX_SetValue(hParam, &data, sizeof(ColorF));
}

#define BIND_TEX_UNIT(X,Y) {int location = get_cache_handle_by_name(#X);\
if(location != -1) glUniform1i( location, Y );}

//-----------------------------------------------------------------------
void gkShaderGLES2::FX_Begin( uint32* pPasses, DWORD flag )
{
	// use program
	glUseProgram(m_Program);



	if (flag == 1)
	{
        //int location = get_cache_handle_by_name("texDiffuse");
        //if(location != -1) glUniform1i( location, 0 );
        BIND_TEX_UNIT(texDiffuse,0)
        BIND_TEX_UNIT(texNormal,1)
        BIND_TEX_UNIT(texSpecular,2)
        BIND_TEX_UNIT(texDetail,3)
        BIND_TEX_UNIT(texCustom1,4)
        BIND_TEX_UNIT(texCustom2,5)
        BIND_TEX_UNIT(texEnvmap,6)
        BIND_TEX_UNIT(texCubemap,7)
	}
	else
	{
        // apply shader state
        ApplyState();
        
		// bind texture sampler
        BIND_TEX_UNIT(texDiffuse,0)
        BIND_TEX_UNIT(texNormal,1)
        BIND_TEX_UNIT(texSpecular,2)
        BIND_TEX_UNIT(texDetail,3)
        BIND_TEX_UNIT(texCustom1,4)
        BIND_TEX_UNIT(texCustom2,5)
        BIND_TEX_UNIT(texEnvmap,6)
        BIND_TEX_UNIT(texCubemap,7)
	}

}
//-----------------------------------------------------------------------
void gkShaderGLES2::FX_BeginPass( uint32 uPass )
{
	
}
//-----------------------------------------------------------------------
void gkShaderGLES2::FX_Commit()
{

}
//-----------------------------------------------------------------------
void gkShaderGLES2::FX_End()
{
	glUseProgram(0);
	RevertState();
}
//-----------------------------------------------------------------------
void gkShaderGLES2::FX_EndPass()
{

}
//-----------------------------------------------------------------------
void gkShaderGLES2::FX_SetTexture( GKHANDLE hParam, gkTexturePtr& pTexture )
{

}

//////////////////////////////////////////////////////////////////////////
uint32 gkShaderGLES2::getDefaultRenderLayer()
{
	return m_uDefaultRenderLayer;
}
//////////////////////////////////////////////////////////////////////////
void gkShaderGLES2::FX_SetMatrixArray( GKHANDLE hParam, D3DXMATRIX* data, uint32 size )
{
	int location = get_cache_handle_by_name(hParam);
	if(location != -1) glUniformMatrix4fv( location, size, GL_FALSE, (float*)(data) );
}

bool gkShaderGLES2::ApplyState()
{
	if (m_thisState.enable)
	{
		StateRestoreInternal(m_prevState);
		StateApplyInternal(m_thisState, m_prevState);
	}
	return true;
}

bool gkShaderGLES2::RevertState()
{
	if (m_thisState.enable)
	{
		StateApplyInternal(m_prevState, m_thisState);
	}
	return true;
}

bool gkShaderGLES2::StateApplyInternal( ShaderState& newState, ShaderState& oldState )
{
	if (newState.isCullFaceEnabled != oldState.isCullFaceEnabled)
	{
		if( newState.isCullFaceEnabled )
			glEnable( GL_CULL_FACE );
		else
			glDisable( GL_CULL_FACE );
	}

	if (newState.eFrontFace != oldState.eFrontFace)
	{
		//glFrontFace( newState.eFrontFace );
	}

	return true;
}

bool gkShaderGLES2::StateRestoreInternal( ShaderState& state )
{
	/* Get previous render states */
	// add future
	state.isCullFaceEnabled = glIsEnabled(GL_CULL_FACE);
	state.isBlendEnabled = glIsEnabled(GL_BLEND);
	state.isDepthTestEnabled = glIsEnabled(GL_DEPTH_TEST);
	//glGetIntegerv(GL_FRONT_FACE, &state.eFrontFace); 
	glGetIntegerv(GL_CULL_FACE_MODE, &state.eCullFaceMode); 
	glGetIntegerv(GL_ARRAY_BUFFER_BINDING,&state.nArrayBufferBinding);

	return true;
}

void gkShaderGLES2::PreProcess( std::string &vsSource )
{
#ifdef RENDERAPI_GL330
	vsSource.insert(0, "#version 100 \n");

    {
        size_t includepos = vsSource.find("#version");
        if (std::string::npos != includepos) {
            includepos = vsSource.find("\n");
        }
        
        if (std::string::npos == includepos) {
            includepos = 0;
        }
        
        vsSource.insert(includepos, "\n #define GL330 \n");

    // insert macros
    ShaderMacros::iterator it = m_shaderMacros.begin();
    for ( ; it != m_shaderMacros.end(); ++it)
    {
        if ( it->second.m_active )
        {
            gkStdString macroline = _T("\n#define ");
            macroline += it->second.m_name;
            macroline += _T("\n");
#ifdef UNICODE
            char macrolineA[MAX_PATH];
            WideCharToMultiByte( CP_ACP, 0, macroline.c_str(), -1, macrolineA, MAX_PATH, 0, 0 );
            vsSource.insert(includepos, macrolineA);
#else
            vsSource.insert(includepos, macroline);
#endif
        }
    }

    }
    
    size_t includepos = vsSource.find("#include");
    while ( std::string::npos != includepos )
    {
        // find one
        size_t firstOne = vsSource.find('"', includepos);
        size_t SecondOne = vsSource.find('"', firstOne + 1);
        
        // get the filename
        std::string filename("engine/shaders/gles2/");
        filename += vsSource.substr( firstOne + 1, SecondOne - firstOne - 1 );
        
#ifdef _UNICODE
        TCHAR* textbuf = new TCHAR[filename.size() + 1];
        MultiByteToWideChar( CP_ACP, 0, filename.c_str(), -1, textbuf, filename.size() + 1 );
        textbuf[filename.size()] = 0;
        IResFile* includeFile = gEnv->pFileSystem->loadResFile(textbuf, false);
#else
        IResFile* includeFile = gEnv->pFileSystem->loadResFile(filename.c_str(), false);
#endif
        
        if ( includeFile )
        {
            vsSource.replace(includepos, SecondOne - includepos + 1, (char*)(includeFile->DataPtr()), includeFile->Size() );
        }
        
        includepos = vsSource.find("#include");
    }
#else
	{
		// replace #version for android device
		size_t includepos = vsSource.find("#version");
		if (std::string::npos != includepos) {
			size_t start = includepos;
			includepos = vsSource.find("\n");
			vsSource.replace( start, includepos - start + 1, " ", 1 );
		}
	}

	// insert macros
	ShaderMacros::iterator it = m_shaderMacros.begin();
	for ( ; it != m_shaderMacros.end(); ++it)
	{
		if ( it->second.m_active )
		{
			gkStdString macroline = _T("#define ");
			macroline += it->second.m_name;
			macroline += _T("\n");
#ifdef UNICODE
			char macrolineA[MAX_PATH];
			WideCharToMultiByte( CP_ACP, 0, macroline.c_str(), -1, macrolineA, MAX_PATH, 0, 0 );

			vsSource.insert(0, macrolineA);
#else
			vsSource.insert(0, macroline);
#endif

		}
	}

	vsSource.insert(0, "#define GLES2 \n");
#ifdef WIN32
	vsSource.insert(0, "#define textureCubeLod textureCubeLod\n");
#elif defined(OS_ANDROID)
	vsSource.insert(0, "#define textureCubeLod textureCube\n");
#else
	vsSource.insert(0, "#extension GL_EXT_shader_texture_lod : enable\n");
	vsSource.insert(0, "#define textureCubeLod textureCubeLodEXT\n");
#endif

	// include insert
	size_t includepos = vsSource.find("#include");
	while ( std::string::npos != includepos )
	{
		// find one
		size_t firstOne = vsSource.find('"', includepos);
		size_t SecondOne = vsSource.find('"', firstOne + 1);

		// get the filename
		std::string filename("engine/shaders/gles2/");
		filename += vsSource.substr( firstOne + 1, SecondOne - firstOne - 1 );

#ifdef _UNICODE
		TCHAR* textbuf = new TCHAR[filename.size() + 1];
		MultiByteToWideChar( CP_ACP, 0, filename.c_str(), -1, textbuf, filename.size() + 1 );
		textbuf[filename.size()] = 0;
		IResFile* includeFile = gEnv->pFileSystem->loadResFile(textbuf, false);
#else
		IResFile* includeFile = gEnv->pFileSystem->loadResFile(filename.c_str(), false);
#endif		

		if ( includeFile )
		{
			vsSource.replace(includepos, SecondOne - includepos + 1, (char*)(includeFile->DataPtr()), includeFile->Size() );
		}

		includepos = vsSource.find("#include");
	}
#endif
}


GKHANDLE gkShaderGLES2::FX_GetTechnique( EShaderInternalTechnique tech )
{
	return NULL;
}

bool gkShaderGLES2::FX_TechniqueSkinned( EShaderInternalTechnique tech )
{
	return false;
}

void gkShaderGLES2::OnFileChange( const TCHAR* filename )
{
	// 文件更改，检查是否需要重新读取
	gkStdString changeFile = filename;
	gkStdString myFile = m_internalName;
	gkNormalizePath(changeFile);
	gkNormalizePath(myFile);

	if ( !_tcsicmp(changeFile.c_str(), myFile.c_str()))
	{
		gkLogMessage(_T("Shader Change, Recompile."));
		reload();

		TCHAR buffer[MAX_PATH];
		_stprintf(buffer, _T("Shader[%s] Compiled."), m_wstrShortName.c_str());
		gEnv->pInGUI->gkPopupMsg( buffer );
	}
}

bool gkShaderGLES2::CompileShader( const char* source, GLuint& shader, GLenum type  )
{
	shader = glCreateShader(type);
	glShaderSource(shader, 1, &source, NULL);
	glCompileShader(shader);

	/* Test if compilation succeeded */
	GLint ShaderCompiled;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &ShaderCompiled);
	if (!ShaderCompiled)
	{
		int i32InfoLogLength, i32CharsWritten;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &i32InfoLogLength);
		char* pszInfoLog = new char[i32InfoLogLength];
		glGetShaderInfoLog(shader, i32InfoLogLength, &i32CharsWritten, pszInfoLog);

#ifdef UNICODE
		TCHAR* warning_info = new TCHAR[ i32InfoLogLength + 1 ];
		MultiByteToWideChar( CP_ACP, 0, pszInfoLog, -1, warning_info, i32InfoLogLength + 1 );
#else
		const char* warning_info = pszInfoLog;
#endif
		//gkLogWarning(_T("shader file %s"), source);
		gkLogWarning(_T("Compile Shader %s failed. %s"), m_wstrFileName.c_str(), warning_info);

#ifdef UNICODE
		delete[] warning_info;
#endif

		delete [] pszInfoLog;
		glDeleteShader(shader);
		return false;
	}
	else
	{
		// 找一下有warning也输出
		int i32InfoLogLength, i32CharsWritten;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &i32InfoLogLength);
		if(i32InfoLogLength > 1)
		{
			char* pszInfoLog = new char[i32InfoLogLength];
			glGetShaderInfoLog(shader, i32InfoLogLength, &i32CharsWritten, pszInfoLog);

#ifdef UNICODE
			TCHAR* warning_info = new TCHAR[ i32InfoLogLength + 1 ];
			MultiByteToWideChar( CP_ACP, 0, pszInfoLog, -1, warning_info, i32InfoLogLength + 1 );
#else
			const char* warning_info = pszInfoLog;
#endif
			gkLogWarning(_T("Compile VS Shader %s warning. %s"), m_wstrFileName.c_str(), warning_info);

#ifdef UNICODE
			delete[] warning_info;
#endif

			delete [] pszInfoLog;
		}

		return true;
	}
}

void gkShaderGLES2::switchSystemMacro(uint32 systemMarcro /*= 0*/)
{
	std::map<uint32, MacroGroup>::iterator it = m_macroPrograms.find(systemMarcro);
	if (it != m_macroPrograms.end())
	{
        m_rtMacroMask = systemMarcro;
		m_Program = it->second.first;
        m_currUniformGroup = &(it->second.second);
		return;
	}

	uint32 final_mask = (systemMarcro & 0xff000000) | m_macroMask;

    m_rtMacroMask = systemMarcro;
	buildShader( final_mask );

    m_macroPrograms.insert( std::map<uint32, MacroGroup>::value_type( systemMarcro, MacroGroup(m_Program, UniformMap() ) ) );
}

bool gkShaderGLES2::buildShader(uint32 marco_mask)
{
	// reprocess the marco list

	ShaderMacros::iterator it = m_shaderMacros.begin();
	for ( ; it != m_shaderMacros.end(); ++it)
	{
		it->second.m_active = false;

		if (it->first & marco_mask)
		{
			it->second.m_active = true;
		}
	}

	// build shader
	IResFile* pVertexShaderSource = gEnv->pFileSystem->loadResFile( m_vs_file_name.c_str(), true);
	if (!pVertexShaderSource)
	{
		return false;
	}

	IResFile* pPixelShaderSource = gEnv->pFileSystem->loadResFile( m_ps_file_name.c_str(), true);
	if (!pPixelShaderSource)
	{
		return false;
	}

	std::string vsSource( (char*)(pVertexShaderSource->DataPtr()),  pVertexShaderSource->Size() );
	std::string psSource( (char*)(pPixelShaderSource->DataPtr()),  pPixelShaderSource->Size() );




	PreProcess(vsSource);
	PreProcess(psSource);


	//gkLogMessage( _T("shader vs[%s]"), vsSource.c_str());
	//gkLogMessage( _T("shader ps[%s]"), psSource.c_str());
	bool bRes = true;
	GLint Linked;

	/* Create and compile the shader object */
	bRes = CompileShader(vsSource.c_str(), m_VertexShader, GL_VERTEX_SHADER);
	bRes = ( bRes && CompileShader(psSource.c_str(), m_FragmentShader, GL_FRAGMENT_SHADER) );

	gEnv->pFileSystem->closeResFile(pVertexShaderSource);
	gEnv->pFileSystem->closeResFile(pPixelShaderSource);

	if (!bRes)
	{
		gkLogMessage(_T("Compile Shader %s failed."), m_wstrFileName.c_str());
		return false;
	}

	m_Program = glCreateProgram();
	glAttachShader(m_Program, m_VertexShader);
	glAttachShader(m_Program, m_FragmentShader);

	//////////////////////////////////////////////////////////////////////////
	// that is general
	glBindAttribLocation(m_Program, GK_SHADER_VERTEX_ARRAY,		"inPosition");
	glBindAttribLocation(m_Program, GK_SHADER_TEXCOORD_ARRAY,	"inTexcoord");
	glBindAttribLocation(m_Program, GK_SHADER_TANGENT_ARRAY,    "inTangent");
	glBindAttribLocation(m_Program, GK_SHADER_BINORMAL_ARRAY,	"inBinormal");
	glBindAttribLocation(m_Program, GK_SHADER_BLENDWIGHT_ARRAY,	"inBlendWight");
	glBindAttribLocation(m_Program, GK_SHADER_BLENDINDEX_ARRAY,	"inBlendIndex");

	//////////////////////////////////////////////////////////////////////////
	// this is post process
	glBindAttribLocation(m_Program, GK_SHADER_DWORDCOLOR_ARRAY, "inColor");

	glLinkProgram(m_Program);
	glGetProgramiv(m_Program, GL_LINK_STATUS, &Linked);

	if (!Linked)
		bRes = false;

	if(!bRes)
	{
		gkLogMessage(_T("Compile Shader %s failed."), m_wstrFileName.c_str());
		return false;
	}
	else
	{
		gkLogMessage(_T("Compile Shader %s success."), m_wstrFileName.c_str());
	}
}
