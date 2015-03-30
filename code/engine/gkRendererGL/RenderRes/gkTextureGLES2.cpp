#include "StableHeader.h"
#include "gkTextureGLES2.h"
#include "gkIterator.h"
#include "gkFilePath.h"

#include "IResFile.h"
#include "ITask.h"

#include "../gkRendererGL330.h"

#include "DDsTextureLoader.h"

#ifdef RENDERAPI_GLES2
#include "HwTextureLoader.h"
#include "glExtension.h"
#endif



#define RESIZE_POWER 0

struct gkTexture2DGLES2StreamingTask : public ITask
{
 	TCHAR m_filepath[MAX_PATH];
	GLuint* m_hw_texptr;

	//PVR_Loader loader;
	IResFile* pFile;
    
	HWT_Loader* loader;
	IResFile* pTextureFile;

	gkTextureGLES2* pTexturePtr;
	bool skip;

	virtual void Execute() 
	{
		skip = false;

		#ifdef RENDERAPI_GLES2
		loader = new PVR_Loader();
		#endif

		#ifdef RENDERAPI_GL330
		loader = new DDS_Loader();
		#endif
		pTextureFile = gEnv->pFileSystem->loadResFile( m_filepath );
		if (!pTextureFile)
		{
			skip = true;
			return;
		}

 		EHwTexError loaderror;
		loaderror = loader->Load_Data(pTextureFile->DataPtr(), pTextureFile->Size() );

		if(loaderror != PVR_SUCCESS)
		{
			gkLogWarning(_T("loading %s failed."), m_filepath);
			//*pErrorStr = "ERROR: Failed to load texture.";
			gEnv->pFileSystem->closeResFile( pTextureFile );
			skip = true;
			return;
		}
		
		
	}

	virtual void Complete_MT()
	{
     
		if(!skip)
		{
			pTexturePtr->setCube( loader->Load_IsCube( pTextureFile->DataPtr() ) );

			loader->Load_Bind( m_hw_texptr, 0 );
			pTexturePtr->m_uWidth = loader->getWidth();
			pTexturePtr->m_uHeight = loader->getHeight();

			gEnv->pFileSystem->closeResFile( pTextureFile );

		}
		delete loader;


//         if(pTexturePtr)
//         {
//             pTexturePtr->m_uWidth = iWidth;
//             pTexturePtr->m_uHeight = iHeight;
// 
// 			if (iArraySize > 1)
// 			{
// 				pTexturePtr->setCube(true);
// 				//return;
// 			}
//         }
// 
// 		gEnv->pFileSystem->closeResFile( pFile );
	}
};

gkTextureGLES2::gkTextureGLES2( IResourceManager* creator, const gkStdString& name, gkResourceHandle handle, const gkStdString& group, gkNameValuePairList* params )
					:ITexture(creator,name,handle,group),
					m_bIsManaged(false),
					m_bIsDefaultPool(false),
					m_uWidth(1),
					m_uHeight(1),
					m_wstrFormat(_T("A16B16G16R16F")),
					m_uHwTexture2D(0),
					m_uHwTextureCube(0),
					m_bCubemap(false),
					m_rawDirty(false)
{
	// save the loading params
	if (params)
	{
		loadingParams = *params;
	}

}

gkTextureGLES2::~gkTextureGLES2( void )
{
	if(isLoaded())
		unload();
}

bool gkTextureGLES2::loadImpl( void )
{
	HRESULT hr = S_OK;

	m_sizable = false;
	m_rawData = NULL;
	m_dynamic = false;
	// if defaultpool, just create
	gkNameValuePairList::iterator it = loadingParams.find(_T("d3dpool"));
	if (it != loadingParams.end())
	{
		// this may not a managed texture
		if (it->second == _T("D3DX_DEFAULT"))
		{
			// this is a defaultpool texture, maybe a RTT
			m_bIsDefaultPool = true;
			
			// get texture params
			gkNameValuePairList::iterator itusage = loadingParams.find(_T("usage"));
			if (itusage == loadingParams.end())
			{ gkLogError(_T("gkTexture[ %s ] LoadingFailed."), m_wstrFileName.c_str()); return false; }

			if ( itusage->second == _T("RENDERTARGET") )
			{
				m_bIsDefaultPool = true;

				m_dynamic = true;
				gkNameValuePairList::iterator itsize = loadingParams.find(_T("size"));
				if (itsize == loadingParams.end())
				{ gkLogError(_T("gkTexture[ %s ] LoadingFailed."), m_wstrFileName.c_str()); return false; }

				double rtResize = 1.0;
				gkCVar* var = gEnv->pCVManager->getCVar( _T("sys_pixelscale") );
				if (var)
				{
					rtResize = var->getFloat();
				}

				m_sizable = true;

				if ( itsize->second == _T("full") )
				{
					m_uWidth = gEnv->pRenderer->GetScreenWidth() * rtResize; 
					m_uHeight = gEnv->pRenderer->GetScreenHeight() * rtResize;
				}
				else if ( itsize->second == _T("half") )
				{
					m_uWidth = gEnv->pRenderer->GetScreenWidth() / 2 * rtResize;
					m_uHeight = gEnv->pRenderer->GetScreenHeight() / 2 * rtResize;
				}
				else if ( itsize->second == _T("quad") )
				{
					m_uWidth = gEnv->pRenderer->GetScreenWidth() / 4 * rtResize;
					m_uHeight = gEnv->pRenderer->GetScreenHeight() / 4 * rtResize;
				}
				else if ( itsize->second == _T("quadquad") )
				{
					m_uWidth = gEnv->pRenderer->GetScreenWidth() / 8 * rtResize;
					m_uHeight = gEnv->pRenderer->GetScreenHeight() / 8 * rtResize;
				}
				else if ( itsize->second == _T("quadquadquad") )
				{
					m_uWidth = gEnv->pRenderer->GetScreenWidth() / 16 * rtResize;
					m_uHeight = gEnv->pRenderer->GetScreenHeight() / 16 * rtResize;
				}
				else
				{
					m_sizable = false;
					gkStdStringstream ss(itsize->second);
					ss >> m_uWidth;
					m_uHeight = m_uWidth;

					// get texture params
					gkNameValuePairList::iterator itsizey = loadingParams.find(_T("sizey"));
					if (itsizey != loadingParams.end())
					{
						gkStdStringstream ss(itsizey->second);
						ss >> m_uHeight;
					}
				}

				gkNameValuePairList::iterator itformat = loadingParams.find(_T("format"));
				if (itformat == loadingParams.end())
				{ gkLogError(_T("gkTexture[ %s ] LoadingFailed."), m_wstrFileName.c_str()); return false; }

				m_wstrFormat = itformat->second;

				GLint internalFormat = GL_RGBA8;
				GLint format = GL_RGBA;
				GLuint type = GL_UNSIGNED_BYTE;

#ifdef RENDERAPI_GL330
				if (m_wstrFormat == _T("A16B16G16R16F"))
				{
					internalFormat = GL_RGBA16F;
					format = GL_RGBA;
					type = GL_HALF_FLOAT;
				}
				else if (m_wstrFormat == _T("G16R16F"))
				{
					internalFormat = GL_RG16F;
					format = GL_RG;
					type = GL_HALF_FLOAT;
				}
				else if (m_wstrFormat == _T("R32F"))
				{
                    internalFormat = GL_R32F;
					format = GL_RED;
					type = GL_FLOAT;
				}
				else if (m_wstrFormat == _T("R16F"))
				{
                    internalFormat = GL_R16F;
					format = GL_RED;
					type = GL_HALF_FLOAT;
				}
				else if (m_wstrFormat == _T("D32F"))
				{
                    internalFormat = GL_DEPTH_COMPONENT32;
					format = GL_DEPTH_COMPONENT;
					type = GL_UNSIGNED_INT;
				}
#else
				if (m_wstrFormat == _T("A16B16G16R16F"))
				{
					internalFormat = GL_RGBA;
					format = GL_RGBA;
					if ( gkGLExtension::IsGLExtensionSupported("GL_OES_texture_half_float") )
					{
						type = GL_HALF_FLOAT_OES;
					}
				}
				else if (m_wstrFormat == _T("G16R16F"))
				{
					internalFormat = GL_RGBA;
					format = GL_RGBA;
				}
				else if (m_wstrFormat == _T("R32F"))
				{
					internalFormat = GL_DEPTH_COMPONENT;
					format = GL_DEPTH_COMPONENT;
					type = GL_UNSIGNED_INT;
				}
				else if (m_wstrFormat == _T("R16F"))
				{
					internalFormat = GL_DEPTH_COMPONENT;
					format = GL_DEPTH_COMPONENT;
					type = GL_UNSIGNED_INT;
				}
				else if (m_wstrFormat == _T("D32F"))
				{
					internalFormat = GL_DEPTH_COMPONENT;
					format = GL_DEPTH_COMPONENT;
					type = GL_UNSIGNED_INT;
				}
#endif
				glGenTextures(1, &m_uHwTexture2D);
				glBindTexture(GL_TEXTURE_2D, m_uHwTexture2D);

				glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_uWidth, m_uHeight, 0, format, type, 0);
                GLenum err = glGetError();
                if (err) {
                    gkLogError(_T("create dynamic texture failed."));
                }
			}
			else if ( itusage->second == _T("RAW") )
			{
				gkNameValuePairList::iterator itsize = loadingParams.find(_T("size"));
				if (itsize == loadingParams.end())
				{ gkLogError(_T("gkTexture[ %s ] LoadingFailed."), m_wstrFileName.c_str()); return false; }

				{
					gkStdStringstream ss(itsize->second);
					ss >> m_uWidth;
					m_uHeight = m_uWidth;

					// get texture params
					gkNameValuePairList::iterator itsizey = loadingParams.find(_T("sizey"));
					if (itsizey != loadingParams.end())
					{
						gkStdStringstream ss(itsizey->second);
						ss >> m_uHeight;
					}
				}

				m_rawData = new uint8[m_uHeight * m_uWidth * sizeof(uint8)];

				// finally, created it
				glGenTextures(1, &m_uHwTexture2D);
				glBindTexture(GL_TEXTURE_2D, m_uHwTexture2D);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, m_uWidth, m_uHeight, 0, GL_RED, GL_UNSIGNED_BYTE, 0);
                
                GLenum err = glGetError();
                if (err) {
                    gkLogError(_T("create raw texture failed."));
                }
			}

			m_rt = true;
			return true;
		}
	}
	m_rt = false;

	// load a managed pool texture
	// 先在目录内查找名字 [9/18/2010 Kaiming-Desktop]
	TCHAR wszPath[MAX_PATH] = _T("");

	_tcscpy(wszPath, m_wstrFileName.c_str());

#ifdef RENDERAPI_GL330
	TCHAR wszExtendedName[MAX_PATH] = _T(".dds");
#else
#ifdef OS_ANDROID
	TCHAR wszExtendedName[MAX_PATH] = _T(".atc");
#else
	TCHAR wszExtendedName[MAX_PATH] = _T(".pvr");
#endif
#endif
	TCHAR* strLastSlash = NULL;
	strLastSlash = _tcsrchr( wszPath, _T( '.' ) );
	if( strLastSlash )
	{
		if ( !_tcsicmp( strLastSlash, _T(".raw") ) )
		{
			// 是，读取raw
			IResFile* pFile = gEnv->pFileSystem->loadResFile( wszPath, false );

			if (!pFile)
			{
				gkLogError(_T("gkTexture::Loading Texture [%s] Failed."), wszPath);
				return false;
			}

			m_uWidth = 0;
			m_uHeight = 0;

			uint32 filesize = pFile->Size();
			for (uint32 i=0; i < 2048; ++i)
			{
				if (i * i == filesize)
				{
					m_uWidth = i;
					m_uHeight = i;
					break;
				}
			}

			if (m_uHeight != 0)
			{
				// read data to raw
				m_rawData = new uint8[filesize];
				memcpy(m_rawData, pFile->DataPtr(), filesize);

				// final
			}

			gEnv->pFileSystem->closeResFile(pFile);

			m_bIsDefaultPool = false;
			return true;
		}
	}

	if( strLastSlash )
	{
		_tcscpy_s( strLastSlash, MAX_PATH, wszExtendedName );
	}

	
	// go to task
	gkTexture2DGLES2StreamingTask* task = new gkTexture2DGLES2StreamingTask;
	_tcscpy(task->m_filepath, wszPath);
	task->m_hw_texptr = &m_uHwTexture2D;
    task->pTexturePtr = this;

	gEnv->pCommonTaskDispatcher->PushTask( task,m_syncLoad ? -1 : 0 );

	//gEnv->pCommonTaskDispatcher->Wait();


	// if go here, we are a managed resource [10/20/2011 Kaiming]
	m_bIsDefaultPool = false;

	gkLogMessage(_T("texture %s loaded."), m_wstrFileName.c_str());
	return true;
}

bool gkTextureGLES2::unloadImpl(void)
{
	glBindTexture(GL_TEXTURE_2D, 0);
	if(m_uHwTexture2D)
	{
		glDeleteTextures(1, &m_uHwTexture2D );
		m_uHwTexture2D = 0;
	}

	return true;
}

void gkTextureGLES2::onReset()
{
	if( m_bIsDefaultPool )
	{
		load();
	}
}

void gkTextureGLES2::onLost()
{
	if( m_bIsDefaultPool )
	{
		unload();
	}
}

void gkTextureGLES2::Apply( uint32 channel, uint8 filter )
{
	touch();

	RawDataFlush();

	glActiveTexture(GL_TEXTURE0 + channel);
	//glUniform1i(userData->samplerLoc, 0);

	GLenum texUnit = GL_TEXTURE_2D;

	if (m_bCubemap)
	{
		texUnit = GL_TEXTURE_CUBE_MAP;
	}

	GLenum error = glGetError();


	glBindTexture(texUnit, m_uHwTexture2D);
	error = glGetError();
	if (error)
	{
		gkLogWarning(_T("bind tex failed.") );
	}
	if (m_rt)
	{
		if (filter)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		}
		else
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		}

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	}
	else
	{
		if (m_rawData)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		}
		else
		{
			if (filter == 2)
			{
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			}
			else
			{
				//glTexParameteri(texUnit, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				//glTexParameteri(texUnit, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
			}
		}


	}
}

float gkTextureGLES2::Tex2DRAW( const Vec2& texcoord, int filter /*= 1*/ )
{


	if (filter)
	{
		float u = texcoord.x - floor(texcoord.x);
		float v = texcoord.y - floor(texcoord.y);

		// 取得纹理空间的地址
		u *= (m_uWidth - 1);
		v *= (m_uHeight - 1);

		// 取整
		int x = (int)( u );
		int y = (int)( v );

		// 计算子像素偏移
		float du = u - x;
		float dv = v - y;

		// 取得地址位置，确保在纹理采样范围内
		int left = x % m_uWidth;
		int right = (x + 1) % m_uWidth;
		int up = y % m_uHeight;
		int down = (y + 1) % m_uHeight;

		GK_ASSERT( left >= 0 && left < m_uWidth );
		GK_ASSERT( right >= 0 && right < m_uWidth );
		GK_ASSERT( up >= 0 && up < m_uHeight );
		GK_ASSERT( down >= 0 && down < m_uHeight );

		// 采样4个颜色
		float lt = Tex2DRAW( Vec2i(left,up) );
		float t = Tex2DRAW( Vec2i(right,up) );
		float l = Tex2DRAW( Vec2i(left,down) );
		float rb =  Tex2DRAW( Vec2i(right,down) );

		float final = ((1.f-du)*(1.f-dv)) * lt + t * (du*(1.f - dv)) + l * ((1.f-du)*dv) + rb * (du*dv);

		return final;
	}
	else
	{
		return 0;
	}
}


float gkTextureGLES2::Tex2DRAW( const Vec2i& texcoord )
{
	return m_rawData[ texcoord.y * m_uWidth + texcoord.x ] / 255.f;
}

void gkTextureGLES2::changeAttr( const gkStdString& key, const gkStdString& value )
{
	// find in loadparam
	gkNameValuePairList::iterator it = loadingParams.find( key );
	if (it != loadingParams.end())
	{
		if ( it->second != value)
		{
			it->second = value;
			reload();
		}

	}
}

const gkStdString& gkTextureGLES2::getAttr( const gkStdString& key ) const
{
	// find in loadparam
	gkNameValuePairList::const_iterator it = loadingParams.find( key );
	if (it != loadingParams.end())
	{
		return it->second;
	}
	else
	{
		static gkStdString notfind(_T("notfind"));
		return notfind;
	}
}

bool gkTextureGLES2::lock( gkLockOperation& lockopt )
{
	if (!m_rawData && m_uHwTexture2D)
	{

	}
	else if (m_rawData)
	{
		// may raw data
		lockopt.m_outData = m_rawData + lockopt.m_pos.y * m_uWidth + lockopt.m_pos.x;
		lockopt.m_outPitch = m_uWidth;

		m_rawDirty = true;

		return true;
	}

	return false;
}

void gkTextureGLES2::unlock()
{

}

void gkTextureGLES2::RawDataFlush()
{
	if( m_uHwTexture2D && m_rawData && m_rawDirty )
	{
		glBindTexture( GL_TEXTURE_2D, m_uHwTexture2D );
		glTexImage2D( GL_TEXTURE_2D, 0, GL_R8, m_uWidth, m_uHeight, 0, GL_RED, GL_UNSIGNED_BYTE, m_rawData  );

		GLenum error = glGetError();
		if (error)
		{
			gkLogWarning(_T("flush raw texture error."));
		}
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );

		m_rawDirty = false;
	}
}
bool gkTextureGLES2::sizable()
{
	return m_sizable;
}