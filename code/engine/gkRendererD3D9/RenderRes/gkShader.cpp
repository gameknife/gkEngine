#include "RendererD3D9StableHeader.h"
#include "IRenderer.h"
#include "gkShader.h"
#include "gkIterator.h"
#include "gkFilePath.h"
#include "gkRendererD3D9.h"
#include "gkTexture.h"
#include <iomanip>

#include "IXmlUtil.h"
#include "gkMemoryLeakDetecter.h"

gkShader::gkShader( IResourceManager* creator, const gkStdString& name, gkResourceHandle handle, const gkStdString& group /*= _T("none")*/ )
:IShader(creator,name,handle,group),
m_pEffect(NULL)
{
	m_uDefaultRenderLayer = -1; 
	m_forceCompile = false;

	memset(m_staticTechnique, 0, sizeof(m_staticTechnique));

	// creation time, add to file change monitor
	gEnv->pFileChangeMonitor->AddListener(this);
}

gkShader::~gkShader( void )
{
	if(isLoaded())
		unload();

	std::map<uint32, LPD3DXEFFECT>::iterator it = m_macro_effects.begin();
	for( ; it != m_macro_effects.end(); ++it)
	{
		SAFE_RELEASE( it->second );
	}
	m_macro_effects.clear();

	gEnv->pFileChangeMonitor->RemoveListener(this);
}

bool gkShader::loadImpl( void )
{
	// 现在是最简单的调用内部的loadImpl [9/17/2010 Kaiming-Laptop]
	IDirect3DDevice9* pDevice = getRenderer()->getDevice();
	ID3DXEffectPool** ppPool = getRenderer()->getEffectPoolPointer();
	return loadImpl(pDevice, ppPool);
}

gkStdString gkShader::getMacroProfile(uint32 macromask)
{
	gkStdString ret = _T("");

	ShaderMacros::iterator it = m_shaderMacros.begin();
	for ( ; it != m_shaderMacros.end(); ++it)
	{
		if ( macromask & it->first)
		{
			ret+= _T(" /D ");
			ret+= it->second.m_name;
		}
	}

	return ret;
}

bool gkShader::loadFromGfxShader( CRapidXmlParseNode* rootNode, uint32 mask, IDirect3DDevice9* d3d9Device, ID3DXEffectPool** ppPool )
{
	HRESULT hr;

	// test

	// read vfx & ffx file
	CRapidXmlParseNode* effectNode = rootNode->getChildNode(_T("D3D9Shader"))->getChildNode(_T("Effect"));
	CRapidXmlParseNode* marcoNode = rootNode->getChildNode(_T("Marco"));
	CRapidXmlParseNode* layerNode = rootNode->getChildNode(_T("RenderLayer"));

	// find compiled binary
	if (effectNode)
	{
		TCHAR filenamepure[MAX_PATH];

		TCHAR* strLastSlash = NULL;
		strLastSlash = _tcsrchr( effectNode->GetAttribute(_T("FileName")), _T( '/' ) );
		if (strLastSlash)
		{
			_tcscpy( filenamepure, strLastSlash + 1 );
		}

		strLastSlash = _tcsrchr( filenamepure, _T( '.' ) );
		if (strLastSlash)
		{
			*strLastSlash = 0;
		}

		m_gfxName = filenamepure;

		// find and parse marco
		if (marcoNode)
		{
			m_shaderMacros.clear();

			// add system macro
			m_shaderMacros[eSSM_Skinned].m_name = _T("SKIN");
			m_shaderMacros[eSSM_Skinned].m_value = _T("");
			m_shaderMacros[eSSM_Skinned].m_desc = _T("Skinning");
			m_shaderMacros[eSSM_Skinned].m_active = false;
			m_shaderMacros[eSSM_Skinned].m_hidden = true;

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
				

				if ( mask & maskflag)
				{
					m_shaderMacros[maskflag].m_active = true;
				}
			}
		}

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

		// compile skinned shader
		switchSystemMacro( eSSM_Skinned );

		// compile the default
		switchSystemMacro( 0 );
	}
	else
	{
		return false;
	}

	return true;
}

bool gkShader::loadImpl( IDirect3DDevice9* d3d9Device, ID3DXEffectPool** ppPool )
{
	HRESULT hr = S_OK;

	// 先在目录内查找名字 [9/18/2010 Kaiming-Desktop]
	TCHAR pureName[MAX_PATH];
	_tcscpy(pureName, m_wstrFileName.c_str());

	TCHAR* strLastSlash = NULL;
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

	// chop it's path

	strLastSlash = _tcsrchr( pureName, _T( '.' ) );
	if( strLastSlash )
	{
		*strLastSlash = 0;
	}

	m_gfxName = pureName;

	// load gfx
	IRapidXmlParser parser;

	// gfx可能存放在三个地方
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

				gkLogWarning( _T("Shader [%s] use old routine, this should be deprecate soon, please clean."), pureName);
			}
		}
	}

	if (!legacy_routine)
	{
		parser.initializeReading( gfxPath.c_str() );
	}

	if (!legacy_routine && parser.getRootXmlNode() && parser.getRootXmlNode()->getChildNode(_T("D3D9Shader")) )
	{
		bool success = loadFromGfxShader( parser.getRootXmlNode(), mask, d3d9Device, ppPool );
	}
	else
	{
		switchSystemMacro(0);
	}
	parser.finishReading();

	if (hr != S_OK)
	{
		gkLogError( _T("gkShader::Shader[%s] compile failed."), m_wstrFileName.c_str() );
		return false;
	}

	D3DXHANDLE script = m_pEffect->GetParameterBySemantic( NULL, "STANDARDSGLOBAL");
	if (script)
	{
		D3DXHANDLE layer = m_pEffect->GetAnnotationByName(script, "ScriptLayer");
		if (layer)
		{
			LPCSTR szParamName;
			m_pEffect->GetString(layer, &szParamName);

			if (!(stricmp(szParamName, "RENDERLAYER_WATER")))
			{
				m_uDefaultRenderLayer = RENDER_LAYER_WATER;
			}
			else if (!(stricmp(szParamName, "RENDERLAYER_SKY")))
			{
				m_uDefaultRenderLayer = RENDER_LAYER_SKIES_EARLY;
			}
		}
	}

	memset( m_staticTechnique, 0, sizeof(m_staticTechnique) );

	m_staticTechnique[eSIT_General] = FX_GetTechniqueByName("RenderScene");
	m_staticTechnique[eSIT_Zpass_DL] = FX_GetTechniqueByName("ZPass");
	m_staticTechnique[eSIT_Zpass_DS] = FX_GetTechniqueByName("ZPass_DS");
	m_staticTechnique[eSIT_ShadowPass] = FX_GetTechniqueByName("ShadowPass");
	m_staticTechnique[eSIT_ReflGenPass] = FX_GetTechniqueByName("ReflGenPass");

	if ( getRenderer()->GetStateManager() )
	{
		m_pEffect->SetStateManager( getRenderer()->GetStateManager() );
	}

	// 绑定默认参数 [4/15/2013 Kaiming]
	m_params.cleanParams();

	D3DXEFFECT_DESC EffectDesc;
	// Gather the parameters
	hr = m_pEffect->GetDesc( &EffectDesc );

	for( UINT i=0; i < EffectDesc.Parameters; i++ )
	{
		D3DXHANDLE hParameter = m_pEffect->GetParameter( NULL, i );
		if( NULL == hParameter )
		{
			continue;
		}
		
		D3DXPARAMETER_DESC desc;
		m_pEffect->GetParameterDesc( hParameter, &desc );

		if (desc.Annotations > 0)
		{
			//m_pEffect->GetAnnotation(  )
			GKSHADERPARAM* param = NULL;
			switch( desc.Type)
			{
			case D3DXPT_STRING:
			case D3DXPT_TEXTURE:
				{
					param = new GKSHADERPARAM( desc.Name, _T("/engine/assets/textures/default/flat.tga") );
				}
				break;
			case D3DXPT_FLOAT:
				{
					char* databuf = new char[desc.Bytes];
					m_pEffect->GetValue( hParameter, databuf , desc.Bytes );

					switch( desc.Bytes )
					{
					case sizeof(float):
						{
							float data = *((float*)databuf);
							param = new GKSHADERPARAM( desc.Name, data);

							// 现在只给float型的添加最大最小值 [4/16/2013 YiKaiming]
							D3DXHANDLE handle = m_pEffect->GetAnnotationByName( hParameter, "UIMin" );
							if ( handle )
							{
								float min;
								m_pEffect->GetFloat( handle, &min );
								param->minValue = min;
							}
							handle = m_pEffect->GetAnnotationByName( hParameter, "UIMax" );
							if ( handle )
							{
								float max;
								m_pEffect->GetFloat( handle, &max );
								param->maxValue = max;
							}
						}
						break;
					case sizeof(Vec2):
						{
							Vec2 data = *((Vec2*)databuf);
							param = new GKSHADERPARAM( desc.Name, data);
						}
						break;
					case sizeof(Vec3):
						{
							Vec3 data = *((Vec3*)databuf);
							param = new GKSHADERPARAM( desc.Name, data);
						}
						break;
					case sizeof(Vec4):
						{
							Vec4 data = *((Vec4*)databuf);
							param = new GKSHADERPARAM( desc.Name, data);
						}
						break;
					}

					delete[] databuf;
				}
				break;
			case D3DXPT_BOOL:
			case D3DXPT_INT:
				{

				}
				break;
			}


			if (param)
			{
				m_params.m_vecParams.push_back( param );
			}
		}

	}


	if (FAILED(hr))
	{
		gkLogWarning(_T("ShaderSystem:: Shader [%s] loaded failed. Creating Effect."), m_wstrFileName.c_str());
		return false;
	}

	return true;
}

bool gkShader::unloadImpl(void)
{
	m_pEffect = 0;

	std::map<uint32, LPD3DXEFFECT>::iterator it = m_macro_effects.begin();
	for( ; it != m_macro_effects.end(); ++it)
	{
		uint32 refcount = it->second->Release();
		if (refcount > 0)
		{
			gkLogError( _T("Shader [%s] REFCOUNT not ZERO!"), m_wstrFileName.c_str() );
		}
	}
	m_macro_effects.clear();

	m_params.cleanParams();

	return true;
}

LPD3DXEFFECT gkShader::getEffect()
{
	assert(m_pEffect);
	if (m_pEffect)
	{
		return m_pEffect;
	}
	else
	{
		return NULL;
	}
}
//-----------------------------------------------------------------------
void gkShader::onReset()
{
	touch();
	m_pEffect->OnResetDevice();	
}
//-----------------------------------------------------------------------
void gkShader::onLost()
{
	m_pEffect->OnLostDevice();
}
//-----------------------------------------------------------------------
void gkShader::FX_SetTechniqueByName( LPCSTR name )
{
	FX_SetTechnique(m_pEffect->GetTechniqueByName( name ));

}
//-----------------------------------------------------------------------
void gkShader::FX_SetTechnique( GKHANDLE hTech )
{
	if (hTech)
	{
		m_pEffect->SetTechnique(hTech);
	}
	else
	{
		m_pEffect->FindNextValidTechnique(NULL, &hTech);
		if (hTech)
		{
			m_pEffect->SetTechnique(hTech);
		}
		else
		{
			GK_ASSERT(0);
		}
	}

}
//-----------------------------------------------------------------------
GKHANDLE gkShader::FX_GetTechniqueByName( LPCSTR name )
{
	return m_pEffect->GetTechniqueByName( name );
}
//////////////////////////////////////////////////////////////////////////
void gkShader::FX_SetBool( GKHANDLE hParam, bool value )
{
	m_pEffect->SetBool(hParam, value);
}
//-----------------------------------------------------------------------
void gkShader::FX_SetValue( GKHANDLE hParam, const void* data, UINT bytes )
{
	m_pEffect->SetValue(hParam, data, bytes);
}

void gkShader::FX_SetMatrix( GKHANDLE hParam, const Matrix44& data )
{
	m_pEffect->SetValue(hParam, &data, sizeof(Matrix44));
}

//-----------------------------------------------------------------------
void gkShader::FX_SetFloat( GKHANDLE hParam, float data)
{
	FX_SetValue(hParam, &data, sizeof(float));
}

void gkShader::FX_SetFloat2(GKHANDLE hParam, const Vec2& data)
{
	FX_SetValue(hParam, &data, sizeof(Vec2));
}
//-----------------------------------------------------------------------
void gkShader::FX_SetFloat3( GKHANDLE hParam, const Vec3& data )
{
	FX_SetValue(hParam, &data, sizeof(Vec3));
}
//-----------------------------------------------------------------------
void gkShader::FX_SetFloat4( GKHANDLE hParam, const Vec4& data )
{
	FX_SetValue(hParam, &data, sizeof(Vec4));
}
//////////////////////////////////////////////////////////////////////////
void gkShader::FX_SetColor4( GKHANDLE hParam, const ColorF& data )
{
	FX_SetValue(hParam, &data, sizeof(ColorF));
}

//-----------------------------------------------------------------------
void gkShader::FX_Begin( UINT* pPasses, DWORD flag )
{
	m_pEffect->Begin(pPasses, D3DXFX_DONOTSAVESTATE);
}
//-----------------------------------------------------------------------
void gkShader::FX_BeginPass( UINT uPass )
{
	m_pEffect->BeginPass(uPass);
}
//-----------------------------------------------------------------------
void gkShader::FX_Commit()
{
	m_pEffect->CommitChanges();
}
//-----------------------------------------------------------------------
void gkShader::FX_End()
{
	m_pEffect->End();
}
//-----------------------------------------------------------------------
void gkShader::FX_EndPass()
{
	m_pEffect->EndPass();
}
//-----------------------------------------------------------------------
void gkShader::FX_SetTexture( GKHANDLE hParam, gkTexturePtr& pTexture )
{
	if (!pTexture.isNull())
	{
		gkTexture* hwTex = reinterpret_cast<gkTexture*>( pTexture.getPointer() );
		m_pEffect->SetTexture(hParam, hwTex->getTexture());
	}
}

//////////////////////////////////////////////////////////////////////////
uint32 gkShader::getDefaultRenderLayer()
{
	return m_uDefaultRenderLayer;
}
//////////////////////////////////////////////////////////////////////////
void gkShader::FX_SetMatrixArray( GKHANDLE hParam, D3DXMATRIX* data, uint32 size )
{
	m_pEffect->SetMatrixArray(hParam, data, size);
}

void gkShader::OnFileChange( const TCHAR* filename )
{
	// 文件更改，检查是否需要重新读取
	if ( !_tcsicmp(filename, m_internalName.c_str()))
	{
		gkLogMessage(_T("Shader Change, Recompile."));
		m_forceCompile = true;
		reload();
		m_forceCompile = false;

		TCHAR buffer[MAX_PATH];
		_stprintf(buffer, _T("Shader[%s] Compiled."), m_wstrShortName.c_str());
		gEnv->pInGUI->gkPopupMsg( buffer );
	}
}

bool gkShader::CompileShader( gkStdString &binaryPath, gkStdString &sourcePath, IResFile* &pFile, const TCHAR* profile )
{
	gkStdString absbinaryPath = gkGetExecRootDir() + binaryPath;

	// 只要决定了编译，就先删除cache
	DeleteFile( absbinaryPath.c_str() );

	// not exist compile
	gkLogWarning( _T("gkShader::Shader[%s] binary not exist. "), m_wstrFileName.c_str() );

	TCHAR buffer[1024];
	if(profile)
	{
		_stprintf(buffer, _T("%sbin32\\fxc.exe /Zpc /T fx_2_0 %s /Fo %s %s"), gkGetExecRootDir().c_str(), profile, absbinaryPath.c_str(), sourcePath.c_str());
	}
	else
	{
		_stprintf(buffer, _T("%sbin32\\fxc.exe /Zpc /T fx_2_0 /Fo %s %s"), gkGetExecRootDir().c_str(), absbinaryPath.c_str(), sourcePath.c_str());
	}
	

	char bufferA[1024];

#ifdef UNICODE
	WideCharToMultiByte(CP_ACP, NULL, buffer, -1, bufferA, 1024, NULL, NULL );
#else
	_tcscpy( bufferA, buffer );
#endif
	system(bufferA);

	pFile = gEnv->pFileSystem->loadResFile( binaryPath.c_str(), true );
	if (!pFile)
	{
		// 编译错误，使用默认的报错shader
		gkLogError( _T("gkShader::Shader[%s] compile failed. Use Default."), m_wstrFileName.c_str() );
		pFile = gEnv->pFileSystem->loadResFile( _T("Engine/Shaders/d3d9/ksCompileFailed.sto"), true );

		if (!pFile)
		{
			// 编译一下默认shader
			gkStdString absbinaryPath = gkGetExecRootDir() + _T("Engine/Shaders/d3d9/ksCompileFailed.sto");
			gkStdString sourcePath = gkGetExecRootDir() + _T("Engine/Shaders/d3d9/hwcode/kscompilefailed.fx"); 
			TCHAR buffer[1024];
			_stprintf(buffer, _T("%sbin32\\fxc.exe /Zpc /T fx_2_0 /Fo %s %s"), gkGetExecRootDir().c_str(), absbinaryPath.c_str(), sourcePath.c_str());
			char bufferA[1024];

#ifdef UNICODE
			WideCharToMultiByte(CP_ACP, NULL, buffer, -1, bufferA, 1024, NULL, NULL );
#else
			_tcscpy( bufferA, buffer );
#endif
			system(bufferA);


			pFile = gEnv->pFileSystem->loadResFile( _T("Engine/Shaders/d3d9/ksCompileFailed.sto"), true );
			if (!pFile)
			{
				// 如果报错shader还有错误，只崩溃了
				gkLogError( _T("gkShader::Shader[%s] compile failed."), m_wstrFileName.c_str() );
				return false;
			}

		}
	}

	return true;
}

GKHANDLE gkShader::FX_GetTechnique( EShaderInternalTechnique tech )
{
	return m_staticTechnique[tech];
}

void gkShader::switchSystemMacro(uint32 systemMarcro /*= 0*/)
{
	std::map<uint32, LPD3DXEFFECT>::iterator it = m_macro_effects.find(systemMarcro);
	if (it != m_macro_effects.end())
	{
		m_pEffect = it->second;
		return;
	}

	uint32 final_mask = (systemMarcro & 0xff000000) | m_macroMask;

	// 创建对应sysmacro的内置shader
	gkStdString marcoProfile = getMacroProfile( final_mask );

	IResFile* pFile;

	// should merge with mask, use string stream
	gkStdStringstream ss;

	ss << _T("engine/shaders/d3d9/shadercache/") << gkStdString(m_gfxName) <<  _T("@");
	ss<<_T("0x")<<std::setw(8)<<std::setfill(_T('0'))<<std::hex<<std::right<<final_mask;
	ss << _T(".o");

	m_internalName = gkStdString(_T("/engine/shaders/d3d9/hwcode/")) + m_gfxName + gkStdString(_T(".fx"));

	gkStdString sourcePath = gkGetExecRootDir() + _T("engine/shaders/d3d9/hwcode/") + m_gfxName + _T(".fx");
	gkStdString binaryPath = ss.str();

	if (m_forceCompile)
	{
		bool success = CompileShader(binaryPath, sourcePath, pFile, marcoProfile.c_str());
		if (!success)
		{
			return;
		}
	}
	else
	{
		pFile = gEnv->pFileSystem->loadResFile( binaryPath.c_str(), true );
		if (!pFile)
		{
			bool success = CompileShader(binaryPath, sourcePath, pFile, marcoProfile.c_str());
			if (!success)
			{
				return;
			}
		}
	}

	IDirect3DDevice9* pDevice = getRenderer()->getDevice();
	ID3DXEffectPool** ppPool = getRenderer()->getEffectPoolPointer();

	HRESULT hr = D3DXCreateEffectEx(pDevice, pFile->DataPtr(), pFile->Size(), NULL, NULL, NULL, 
		D3DXFX_NOT_CLONEABLE, *ppPool, &m_pEffect, NULL );

	gEnv->pFileSystem->closeResFile(pFile);


	m_macro_effects.insert( std::map<uint32,LPD3DXEFFECT>::value_type( systemMarcro, m_pEffect )  );
}
