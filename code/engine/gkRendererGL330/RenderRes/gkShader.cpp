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
	HRESULT hr = S_OK;
	// 先在目录内查找名字 [9/18/2010 Kaiming-Desktop]
	TCHAR wszPath[MAX_PATH] = _T("engine/shaders/");
	TCHAR wszPrevPath[MAX_PATH] = _T("");

	_tcscpy(wszPrevPath, m_wstrFileName.c_str());

	TCHAR pureName[MAX_PATH];
	_tcscpy(pureName, m_wstrFileName.c_str());
	// chop it's path
	TCHAR* strLastSlash = NULL;
	strLastSlash = _tcsrchr( pureName, _T( '.' ) );
	if( strLastSlash )
	{
		*strLastSlash = 0;
	}

	// chop it's path
	strLastSlash = NULL;
	TCHAR* strLastBackSlash = NULL;
	strLastBackSlash = _tcsrchr( wszPrevPath, _T( '/' ) );
	strLastSlash = _tcsrchr( wszPrevPath, _T( '\\' ) );
	strLastSlash = max( strLastSlash, strLastBackSlash );

	if (strLastSlash)
	{
		_tcscpy(wszPrevPath, strLastSlash);
	}

	_tcscat(wszPath, wszPrevPath);


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
    
	strLastSlash = _tcsrchr( wszPath, _T( '.' ) );
	if( strLastSlash )
	{
		_tcscpy( strLastSlash, _T(".gfx") );
	}
	m_internalName = gkStdString(_T("engine/shaders/gles2/")) + gkStdString(pureName) + gkStdString(_T(".ffx"));

	// load gfx
	IRapidXmlParser parser;
	parser.initializeReading( gfxPath.c_str() );

	if (!parser.getRootXmlNode())
	{
		parser.finishReading();
		return false;
	}

	bool ret = false;

	if (parser.getRootXmlNode()->getChildNode(_T("GLES2Shader")) )
	{
		ret = loadFromGfxShader( parser.getRootXmlNode()->getChildNode(_T("GLES2Shader")) );
	}
	else
	{
		ret = loadFromGfxShader( parser.getRootXmlNode() );
	}
	

	parser.finishReading();

	if ( !_tcsicmp( m_wstrFileName.c_str(), _T("ks_SkyHdr.fx") ) )
	{
		m_uDefaultRenderLayer = RENDER_LAYER_SKIES_EARLY;
	}

	return ret;
}


bool gkShaderGLES2::loadFromGfxShader( CRapidXmlParseNode* rootNode )
{
	// read vfx & ffx file
	CRapidXmlParseNode* vsnode = rootNode->getChildNode(_T("VSShader"));
	CRapidXmlParseNode* psnode = rootNode->getChildNode(_T("PSShader"));
	CRapidXmlParseNode* statenode = rootNode->getChildNode(_T("StateStage"));

	if (vsnode && psnode)
	{
		IResFile* pVertexShaderSource = gEnv->pFileSystem->loadResFile(vsnode->GetAttribute(_T("FileName")), true);
		if (!pVertexShaderSource)
		{
			return false;
		}

		IResFile* pPixelShaderSource = gEnv->pFileSystem->loadResFile(psnode->GetAttribute(_T("FileName")), true);
		if (!pPixelShaderSource)
		{
			return false;
		}

		std::string vsSource( (char*)(pVertexShaderSource->DataPtr()),  pVertexShaderSource->Size() );
		std::string psSource( (char*)(pPixelShaderSource->DataPtr()),  pPixelShaderSource->Size() );

		PreProcess(vsSource);
		PreProcess(psSource);
		
		bool bRes = true;
		GLint Linked;

		/* Create and compile the shader object */
		bRes = CompileShader(vsSource.c_str(), m_VertexShader, GL_VERTEX_SHADER);
		bRes = ( bRes && CompileShader(psSource.c_str(), m_FragmentShader, GL_FRAGMENT_SHADER) );

		gEnv->pFileSystem->closeResFile(pVertexShaderSource);
		gEnv->pFileSystem->closeResFile(pPixelShaderSource);

		if (!bRes)
		{
			return false;
		}



		m_Program = glCreateProgram();
		glAttachShader(m_Program, m_VertexShader);
		glAttachShader(m_Program, m_FragmentShader);

		//////////////////////////////////////////////////////////////////////////
		// that is general
		glBindAttribLocation(m_Program, GK_SHADER_VERTEX_ARRAY,		"inPosition");
		glBindAttribLocation(m_Program, GK_SHADER_TEXCOORD_ARRAY,	"inTexcoord");
		glBindAttribLocation(m_Program, GK_SHADER_TANGENT_ARRAY,     "inTangent");
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
			int location = glGetUniformLocation(m_Program, hParam);
			const Vec2& tmp = *((const Vec2*)data);
			glUniform2f( location, tmp.x, tmp.y );
		}
		break;
	case 1 * sizeof(float):
		{
			int location = glGetUniformLocation(m_Program, hParam);
			glUniform1f( location, *((float*)data) );
		}
		break;
	default:
		vec = true;
		break;
	}

	if ( vec && (bytes % sizeof(Vec4) == 0) && (bytes / sizeof(Vec4) > 1))
	{
		int location = glGetUniformLocation(m_Program, hParam);
		glUniform4fv( location, bytes / sizeof(Vec4), (const float*)data);
	}
}

void gkShaderGLES2::FX_SetMatrix( GKHANDLE hParam, const Matrix44& data )
{
	// bind MVP MATRIX
	int location = glGetUniformLocation(m_Program, hParam);
	//Matrix44 mvpMat = gkRendererGLES2::getShaderContent().getWorldViewProjMatrix();
	glUniformMatrix4fv( location, 1, GL_FALSE, &(data.m00) );
}

//-----------------------------------------------------------------------
void gkShaderGLES2::FX_SetFloat( GKHANDLE hParam, float data)
{
	FX_SetValue(hParam, &data, sizeof(float));
}

void gkShaderGLES2::FX_SetFloat3( GKHANDLE hParam, const Vec3& data )
{
	int location = glGetUniformLocation(m_Program, hParam);
	glUniform3f( location, data.x, data.y, data.z );
}

//-----------------------------------------------------------------------
void gkShaderGLES2::FX_SetFloat4( GKHANDLE hParam, const Vec4& data )
{
	int location = glGetUniformLocation(m_Program, hParam);
	glUniform4f( location, data.x, data.y, data.z, data.w );
}
//////////////////////////////////////////////////////////////////////////
void gkShaderGLES2::FX_SetColor4( GKHANDLE hParam, const ColorF& data )
{
	FX_SetValue(hParam, &data, sizeof(ColorF));
}

//-----------------------------------------------------------------------
void gkShaderGLES2::FX_Begin( uint32* pPasses, DWORD flag )
{
	// use program
	glUseProgram(m_Program);



	if (flag == 1)
	{
		glUniform1i( glGetUniformLocation(m_Program, "texDiffuse"), 0 );
		glUniform1i( glGetUniformLocation(m_Program, "texNormal"), 1 );
		glUniform1i( glGetUniformLocation(m_Program, "texSpecular"), 2 );
		glUniform1i( glGetUniformLocation(m_Program, "texDetail"), 3 );
        glUniform1i( glGetUniformLocation(m_Program, "texCustom1"), 4 );
        glUniform1i( glGetUniformLocation(m_Program, "texCustom2"), 5 );
        glUniform1i( glGetUniformLocation(m_Program, "texEnvmap"), 6 );
        glUniform1i( glGetUniformLocation(m_Program, "texCubemap"), 7 );
	}
	else
	{
        // apply shader state
        ApplyState();
        
		// bind texture sampler
		int location = glGetUniformLocation(m_Program, "texDiffuse");
		if(location > -1) glUniform1i( location  ,0);

		location = glGetUniformLocation(m_Program, "texNormal");
		if(location > -1) glUniform1i( location  ,1);

		location = glGetUniformLocation(m_Program, "texSpecular");
		if(location > -1) glUniform1i( location  ,2);

		location = glGetUniformLocation(m_Program, "texDetail");
		if(location > -1) glUniform1i( location  ,3);

		location = glGetUniformLocation(m_Program, "texCustom1");
		if(location > -1) glUniform1i( location  ,4);

		location = glGetUniformLocation(m_Program, "texCustom2");
		if(location > -1) glUniform1i( location  ,5);

		location = glGetUniformLocation(m_Program, "texEnvmap");
		if(location > -1) glUniform1i( location  ,6);

		location = glGetUniformLocation(m_Program, "texCubemap");
		if(location > -1) glUniform1i( location  ,7);
	}
	//location = glGetUniformLocation(m_Program, "tex4");
	//glUniform1i( location  ,4);

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
	int location = glGetUniformLocation(m_Program, hParam);
	//Matrix44 mvpMat = gkRendererGLES2::getShaderContent().getWorldViewProjMatrix();
	glUniformMatrix4fv( location, size, GL_FALSE, (float*)(&(data)) );
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
	// include insert
    
    
//        {
//            size_t includepos = vsSource.find("precision highp float;");
//            while ( std::string::npos != includepos )
//            {
//                vsSource.replace(includepos, strlen("precision highp float;"), "", 0);
//                includepos = vsSource.find("precision highp float;");
//            }
//        }

	{
		size_t includepos = vsSource.find("#version");
		if (std::string::npos != includepos) {
			includepos = vsSource.find("\n");
		}

		if (std::string::npos == includepos) {
			includepos = 0;
		}

		vsSource.insert(includepos, "\n #define GL330 \n");
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

//	{
//		size_t includepos = vsSource.find("highp");
//		while ( std::string::npos != includepos )
//		{
//			vsSource.replace(includepos, 5, "", 0);
//			includepos = vsSource.find("highp");
//		}
//	}
//
//	{
//		size_t includepos = vsSource.find("mediump");
//		while ( std::string::npos != includepos )
//		{
//			vsSource.replace(includepos, 7, "", 0);
//			includepos = vsSource.find("mediump");
//		}
//	}
//
//	{
//		size_t includepos = vsSource.find("lowp");
//		while ( std::string::npos != includepos )
//		{
//			vsSource.replace(includepos, 4, "", 0);
//			includepos = vsSource.find("lowp");
//		}
//	}
    

    
    //vsSource.insert(0, "#version 100\n");

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
	if ( !_tcsicmp(filename, m_internalName.c_str()))
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
		gkLogWarning(_T("Compile VS Shader %s failed. %s"), m_wstrFileName.c_str(), warning_info);

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
