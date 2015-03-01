#include "RendererD3D9StableHeader.h"
#include "gkMaterial.h"
#include "gkShaderManager.h"
#include "gkTextureManager.h"
#include "gkIterator.h"
#include "gkFilePath.h"
#include "IXmlUtil.h"

#include "gkMemoryLeakDetecter.h"
#include "gkMaterialManager.h"
#include "gkShader.h"
#include "gkTexture.h"

gkMaterial::gkMaterial( IResourceManager* creator, const gkStdString& name, gkResourceHandle handle, const gkStdString& group /*= _T("none")*/, gkNameValuePairList* params )
:IMaterial(creator,name,handle,group)
{
	m_yLoadType = GKMATERIAL_LOADTYPE_FILE;
	m_bDoubleSide = false;
	m_fGlossness = 8.0f;
	m_nOpacity = NULL;
	m_pLoadingParamBlock = NULL;
	m_bLoadFromFile = true;
	m_vUVTill = Vec2(1,1);
	m_vUVOffset = Vec2(0,0);
	m_fUVSpeed = 0;
	m_uSubMtlCount = 0;
	m_alphaRef = 0.f;
	m_castShadow = true;
	m_bSSRL = false;
	m_shaderFileSet = false;

	m_marcoMask = -1;

	if (params)
	{
		loadingParams = *params;
	}
}


void gkMaterial::releaseParameterBlock()
{
	SAFE_DELETE( m_pLoadingParamBlock );
}

gkMaterial::~gkMaterial( void )
{
	releaseParameterBlock();
	if(isLoaded())
		unload();
}

bool gkMaterial::loadImpl( void )
{
	float t_mat_load_start = gEnv->pTimer->GetAsyncCurTime();

	bool ret = false;

	//////////////////////////////////////////////////////////////////////////
	// 手动加载
	gkNameValuePairList::iterator it = loadingParams.find(_T("file"));
	if (it != loadingParams.end())
	{
		m_wstrIndividualFileName = it->second;
		m_yLoadType = GKMATERIAL_LOADTYPE_INDIVIDUALFILE;
	}

	//////////////////////////////////////////////////////////////////////////
	// 自动加载
	switch(m_yLoadType)
	{
	case GKMATERIAL_LOADTYPE_FILE:
		m_wstrIndividualFileName = m_wstrFileName;
		{
			const TCHAR* path = m_wstrFileName.c_str();

			// if not has the mat file, go with it [12/4/2011 Kaiming]
			if (_tcscmp( &(path[_tcslen(path) - 4]), _T(".mtl") ))
			{
				m_wstrIndividualFileName += _T(".mtl");
			}
		}
	case GKMATERIAL_LOADTYPE_INDIVIDUALFILE:
		{
			if (m_bLoadFromFile)
			{
				m_bLoadFromFile = false;

				IRapidXmlParser parser;
				parser.initializeReading(m_wstrIndividualFileName.c_str());
				CRapidXmlParseNode* rootXmlNode = parser.getRootXmlNode();

				if (!rootXmlNode)
				{
					return false;
				}

				// check
				if (_tcsicmp(rootXmlNode->GetName(), _T("Material")))
				{
					GK_ASSERT(0);
					return false;
				}

				// if main
				if (rootXmlNode->getChildNode(_T("Effect")))
				{
					ret = loadMaterialFormRapidXmlNode(rootXmlNode);
				}

				// submaterial loading

				// 这里也有可能从文件重新读取，这时已经有submtl了 [5/30/2013 gameKnife]
				CRapidXmlParseNode* subMaterial = rootXmlNode->getChildNode(_T("SubMaterial"));
				for (; subMaterial; subMaterial = subMaterial->getNextSiblingNode(_T("SubMaterial")))
				{
					gkStdString submtlname = m_wstrFileName;
					submtlname.append( _T("|") );
					submtlname.append( subMaterial->GetAttribute(_T("Name")) );

					gkMaterial* pSub = getSubMtl(submtlname.c_str());
					ret = pSub->loadMaterialFormRapidXmlNode( subMaterial );
				}

				parser.finishReading();
			}
			else
			{
				// old routine to load material form memory [12/31/2014 gameKnife]
				ret = loadFromParamBlock();

				// if sub
				SubMaterialList::iterator it = m_vecSubMtlList.begin();
				for (; it != m_vecSubMtlList.end(); ++it)
				{
					ret = ((gkMaterial*)((*it).getPointer()))->loadFromParamBlock();
				}
			}
		}
		break;

	case GKMATERIAL_LOADTYPE_SCENE:
		{
			ret = loadFromParamBlock();
		}
		break;
	}

	t_mat_load_start = gEnv->pTimer->GetAsyncCurTime() - t_mat_load_start;
	gkLogMessage( _T("Material Load use %.2fms."), t_mat_load_start * 1000 );

	return ret;
}

gkMaterial* gkMaterial::getSubMtl( const TCHAR* name )
{
	for (uint32 i=0; i < m_vecSubMtlList.size(); ++i)
	{
		if ( !_tcsicmp( name, m_vecSubMtlList[i]->getName().c_str() ))
		{
			return (gkMaterial*)(m_vecSubMtlList[i].getPointer());
		}
	}

	gkMaterial* pSub = new gkMaterial(NULL, name, 0 );
	m_uSubMtlCount++;
	m_vecSubMtlList.push_back(gkMaterialPtr(pSub));

	return pSub;
}

bool gkMaterial::unloadImpl(void)
{
	m_vecTextureArray.clear();
	return true;
}

gkShaderPtr& gkMaterial::getShader()
{
	return m_pShader;
}

void gkMaterial::BasicApply(IShader* pShader)
{
	Vec4 bindUVParam = Vec4(m_vUVTill.x, m_vUVTill.y, m_vUVOffset.x, m_vUVOffset.y);
	pShader->FX_SetValue("g_UVParam", &bindUVParam, sizeof(Vec4));
	pShader->FX_SetValue("g_AlphaRef", &m_alphaRef, sizeof(float));
	
}

void gkMaterial::ApplyParameterBlock( bool texture, IShader* shader )
{
	IShader* commitShader = shader ? shader : m_pShader.getPointer();

	BasicApply( commitShader );
	ApplyParams( commitShader );

	if (texture)
	{
		// 手动设置纹理通道 [1/2/2013 Kaiming]
		// set texture by hand
		for (uint32 i=0; i < m_vecTextureArray.size(); ++i)
		{
			if ( !m_vecTextureArray[i].isNull() )
			{
				m_vecTextureArray[i]->Apply(i, 0);
			}		
		}
	}


	// 附加的设置
}
//-----------------------------------------------------------------------
bool gkMaterial::loadFromParamBlock()
{
	m_alphaRef = 0.f;

	if (!m_pLoadingParamBlock)
	{
		gkLogWarning(_T("MatSystem:: Material: [%s] loaded failed. not find paramblock."), m_wstrFileName.c_str());
		return false;
	}

	HRESULT hr = S_OK;

	assert( !m_pShader.isNull() );
	if (m_pShader.isNull())
	{
		return false;
	}

	// 不再从effect中构建参数 [2/17/2014 gameKnife]

	gkMaterialParams::ParameterList::iterator it = m_pLoadingParamBlock->m_vecParams.begin();
	for ( it ; it != m_pLoadingParamBlock->m_vecParams.end(); ++it )
	{
		GKSHADERPARAM* param = *it;
		if ( param->Type == GKSHADERT_FLOAT)
		{
			if (!_stricmp(param->pParamName, "g_SpecularPow"))
			{
				m_fGlossness = *((float*)(param->pValue));
			}
			else if (!_stricmp(param->pParamName, "g_AlphaRef"))
			{
				m_alphaRef = *((float*)(param->pValue));
			}
			else if (!_stricmp(param->pParamName, "g_Opacity"))
			{
				m_nOpacity = (float*)(param->pValue);
			}
		}
	}

	return true;
}

gkTexturePtr& gkMaterial::getTexture( EMaterialSlot index )
{
	static gkTexturePtr ptr;
	if (m_vecTextureArray.size() < (uint32)index + 1)
		return ptr;
	return m_vecTextureArray[index];
}
//-----------------------------------------------------------------------
void gkMaterial::setTexture( gkTexturePtr& pTex, byte index /*= 0*/ )
{
	m_vecTextureArray[index] = pTex;
}

float gkMaterial::getGlossness()
{
	return m_fGlossness;
}

int gkMaterial::getOpacity()
{
	if (m_nOpacity)
	{
		return *m_nOpacity * 100;
	}
	return 100;
}


bool gkMaterial::saveMaterialToRapidXmlNode( CRapidXmlAuthorNode* rootNode )
{
	if (m_pLoadingParamBlock)
	{
		rootNode->AddAttribute(_T("Name"), m_pLoadingParamBlock->m_wstrMatName.c_str());

		CRapidXmlAuthorNode* fxNode = rootNode->createChildNode(_T("Effect"));
		fxNode->AddAttribute(_T("FileName"), m_shaderFilename.c_str() );
		fxNode->AddAttribute(_T("ParamCount"), (int)(m_pLoadingParamBlock->m_vecParams.size()) );
		fxNode->AddAttribute(_T("Mask"), (int)(m_marcoMask == -1 ? 0 : m_marcoMask) );
		fxNode->AddAttribute(_T("CastShadow"), m_castShadow);

		for (uint32 i=0; i < m_pLoadingParamBlock->m_vecParams.size(); ++i)
		{
			

			switch (m_pLoadingParamBlock->m_vecParams[i]->Type) {
			case GKSHADERT_FLOAT:
			case GKSHADERT_FLOAT2:
			case GKSHADERT_FLOAT3:
			case GKSHADERT_FLOAT4:
				{
					CRapidXmlAuthorNode* paramNode = fxNode->createChildNode(_T("Param"));

					paramNode->AddAttribute(_T("name"), m_pLoadingParamBlock->m_vecParams[i]->pParamName);

					switch (m_pLoadingParamBlock->m_vecParams[i]->Type)
					{
					case GKSHADERT_FLOAT:
						paramNode->AddAttribute(_T("type"), _T("float"));
						break;
					case GKSHADERT_FLOAT2:
						paramNode->AddAttribute(_T("type"), _T("float2"));
						break;
					case GKSHADERT_FLOAT3:
						paramNode->AddAttribute(_T("type"), _T("float3"));
						break;
					case GKSHADERT_FLOAT4:
						paramNode->AddAttribute(_T("type"), _T("float4"));
						break;
					}

					float params[4];
					uint8 numfloat = m_pLoadingParamBlock->m_vecParams[i]->NumBytes / sizeof(float);
					memcpy_s(params, m_pLoadingParamBlock->m_vecParams[i]->NumBytes, m_pLoadingParamBlock->m_vecParams[i]->pValue, m_pLoadingParamBlock->m_vecParams[i]->NumBytes);
					gkStdStringstream wss;
					for(uint8 i=0; i < numfloat; ++i)
					{
						wss << params[i] << _T(" ");
					}
					paramNode->AddAttribute(_T("value"), wss.str().c_str());
				}



				break;
				// 固定纹理通道存储 [3/2/2014 gameKnife]
// 			case GKSHADERT_STRING:
// 				{
// 					paramNode->AddAttribute(_T("name"), m_pLoadingParamBlock->m_vecParams[i]->pParamName);
// 					paramNode->AddAttribute(_T("type"), _T("texture"));
// 
// 					// find the full-relative path of texture to write [12/30/2011 Kaiming]
// 					gkStdString relPath = gkGetGameRelativePath((TCHAR*)(m_pLoadingParamBlock->m_vecParams[i]->pValue), true);
// 
// 					paramNode->AddAttribute(_T("value"), relPath.c_str());
// 				}
// 
// 				break;
			case GKSHADERT_DWORD:
				{
					CRapidXmlAuthorNode* paramNode = fxNode->createChildNode(_T("Param"));

					paramNode->AddAttribute(_T("name"), m_pLoadingParamBlock->m_vecParams[i]->pParamName);
					paramNode->AddAttribute(_T("type"), _T("int"));

					int* intvalue = (int*)(m_pLoadingParamBlock->m_vecParams[i]->pValue);

					paramNode->AddAttribute(_T("value"), *intvalue);

					
				}

				break;

			default:
				{
					gkLogWarning(_T("gkMaterial::SaveToFile find invalid params. skipped."));
				}
				break;
			}
		}

		CRapidXmlAuthorNode* texchnNode = rootNode->createChildNode(_T("TexChannel"));

		TextureArray::iterator it = m_vecTextureArray.begin();
		for (int chn = 0; it != m_vecTextureArray.end(); ++it, ++chn)
		{
			CRapidXmlAuthorNode* paramNode = texchnNode->createChildNode(_T("Param"));

			// it 不一定存在 [10/7/2014 gameKnife]
			if ( !(*it).isNull()  )
			{
				gkStdString name = (*it)->getName();

				gkStdString relPath = gkGetGameRelativePath(name.c_str(), true);
				// ENGINE PATH PRIORITY [12/31/2014 gameKnife]
				if ( gkIsEnginePath(name.c_str()))
				{
					relPath = gkGetExecRelativePath(  name.c_str(), true );
				}

				

				if ( relPath == _T("bad") )
				{
					// 替换dds再找一次
					
				}

				paramNode->AddAttribute(_T("chn"), chn);
				paramNode->AddAttribute(_T("value"), relPath.c_str());
			}
		}

		// finally, link static params [11/19/2011 Kaiming]

		CRapidXmlAuthorNode* staticNode = rootNode->createChildNode(_T("Static"));
		staticNode->AddAttribute(_T("ParamCount"), 1);

		// uv params
		CRapidXmlAuthorNode* uvParamNode = staticNode->createChildNode(_T("Param"));
		uvParamNode->AddAttribute(_T("name"), _T("uvparams"));
		// define type external, to load from external way [11/19/2011 Kaiming]
		uvParamNode->AddAttribute(_T("type"), _T("external"));

		gkStdStringstream wss;
		wss << m_vUVTill.x << _T(" ") << m_vUVTill.y << _T(" ") << m_vUVOffset.x << _T(" ") << m_vUVOffset.y;

		uvParamNode->AddAttribute(_T("value"), wss.str().c_str());

// 		// uv params
// 		CRapidXmlAuthorNode* ssrlParamNode = staticNode->createChildNode(_T("Param"));
// 		ssrlParamNode->AddAttribute(_T("name"))

		return true;
	}
	return false;
}

void gkMaterial::saveAsMtlFile( const TCHAR* filename )
{
	IRapidXmlAuthor materialAuthor;
	materialAuthor.initializeSceneWrite();

	CRapidXmlAuthorNode* rootNode = materialAuthor.createRapidXmlNode(_T("Material"));
	
	// judge if multi-material
	if (!m_vecSubMtlList.empty())
	{
		for(uint32 i=0; i<m_vecSubMtlList.size(); ++i)
		{
			gkMaterial* pMtl = reinterpret_cast<gkMaterial*>( m_vecSubMtlList[i].getPointer() );
			CRapidXmlAuthorNode* pSubMtlNode = rootNode->createChildNode(_T("SubMaterial"));
			pMtl->saveMaterialToRapidXmlNode( pSubMtlNode );
		}		
	}
	else
	{
		saveMaterialToRapidXmlNode(rootNode);
	}

	materialAuthor.writeToFile(filename);	
}

void gkMaterial::saveToMtlFile()
{
	TCHAR absPath[MAX_PATH];
	gkFindFileRelativeGame(absPath, MAX_PATH, m_wstrIndividualFileName.c_str());
	saveAsMtlFile(absPath);
}
//////////////////////////////////////////////////////////////////////////
IMaterial* gkMaterial::getSubMaterial( uint8 index )
{
	if (index < m_vecSubMtlList.size())
	{
		return m_vecSubMtlList[index].getPointer();
	}
	return gkMaterialManager::ms_DefaultMaterial.getPointer();	
}





//////////////////////////////////////////////////////////////////////////
bool gkMaterial::loadMaterialFormRapidXmlNode( CRapidXmlParseNode* rootXmlNode )
{
	CRapidXmlParseNode* fxXmlNode = rootXmlNode->getChildNode();

	if ( !fxXmlNode || _tcsicmp(fxXmlNode->GetName(), _T("Effect")))
	{
		GK_ASSERT(0);
		return false;
	}

	CRapidXmlParseNode* staticXmlNode = rootXmlNode->getChildNode(_T("Static") );

	if (staticXmlNode)
	{
		if (!_tcsicmp(staticXmlNode->GetName(), _T("Static")))
		{
			CRapidXmlParseNode* uvXmlNode = staticXmlNode->getChildNode();
			if (uvXmlNode->GetAttribute(_T("value")) == 0)
			{
				return false;
			}
			gkStdStringstream stream(uvXmlNode->GetAttribute(_T("value")));
			stream>> m_vUVTill.x >> m_vUVTill.y >> m_vUVOffset.x >> m_vUVOffset.y;
		}
	}

	// now we begin
	// 这里也不直接删除，而是在后面查询是否有重名项 [5/30/2013 gameKnife]
	//releaseParameterBlock();

	// 读取shader
	gkStdString shaderFilename = fxXmlNode->GetAttribute(_T("FileName"));

	fxXmlNode->GetAttribute(_T("CastShadow"), m_castShadow);
	
	// 合并mask
	if (m_marcoMask == -1)
	{
		int mask = 0;
		fxXmlNode->GetAttribute(_T("Mask"), mask);
		m_marcoMask = mask;
	}

	if (!m_shaderFileSet)
	{
		m_shaderFilename = shaderFilename;
	}
	
	// remove extension
	m_shaderFilename = gkGetPureFilename(m_shaderFilename.c_str());

	gkStdStringstream ss;
	ss << m_shaderFilename << _T("@") << m_marcoMask;

	m_pShader = getRenderer()->getShaderMngPtr()->load( ss.str(), _T("") );
	// 从shader拷贝marcos
	// 这里直接拷贝会覆盖之前的设置和指针，应该拷贝数据
	if (m_shaderMacros.empty())
	{
		m_shaderMacros = m_pShader->getShaderMarcos();
	}

	// 遍历一下
	m_bSSRL = false;
	ShaderMacros::iterator it = m_shaderMacros.begin();
	for (; it != m_shaderMacros.end(); ++it)
	{
		if( it->second.m_active )
		{
			if ( !_tcscmp(it->second.m_name.c_str(), _T("SSRL")) )
			{
				m_bSSRL = true;
			}
		}
	}
	

	gkMaterialParams* shaderParam = m_pShader->getShaderDefaultParams();

	// 如果没有，创建
	bool firstLoad = true;

	if (!m_pLoadingParamBlock)
	{
		m_pLoadingParamBlock = new gkMaterialParams( *shaderParam );

		if (m_pLoadingParamBlock->m_wstrMatName.empty())
		{
			m_pLoadingParamBlock->m_wstrMatName = rootXmlNode->GetAttribute(_T("Name"));
		}

		// parameter accessing
		CRapidXmlParseNode *pParamChild = fxXmlNode->getChildNode();

		// 遍历params
		for ( pParamChild ; pParamChild != 0; pParamChild = pParamChild->getNextSiblingNode()) 
		{
			//GKSHADERPARAM sParam;
			gkStdString typeName;

			// 先找属性
			// 赋值
			//sParam.pParamName = new char[MAX_PATH];
			//ZeroMemory(sParam.pParamName, sizeof(char) * MAX_PATH);
			char paramName[MAX_PATH];

			strcpy(paramName, "");

#ifdef UNICODE
			WideCharToMultiByte(CP_ACP, 0,  pParamChild->GetAttribute(_T("name")), -1, paramName, MAX_PATH, NULL, NULL);
#else
			_tcscpy_s(paramName, MAX_PATH, pParamChild->GetAttribute(_T("name")) );
#endif

			GKSHADERPARAM* param = m_pLoadingParamBlock->getParam( paramName );
			if (!param)
			{
				continue;
			}

			typeName = pParamChild->GetAttribute(_T("type"));
			gkStdStringstream stream(pParamChild->GetAttribute(_T("value")));
			// 根据属性提供的类型，写入
			if(typeName == _T("float"))
			{
				float tmpValue;
				stream>>tmpValue;
				param->setValue( tmpValue );
			}
			else if(typeName == _T("float2"))
			{
				Vec2 tmpValue;
				stream>>tmpValue.x>>tmpValue.y;
				param->setValue(tmpValue);
			}
			else if(typeName == _T("float3"))
			{
				Vec3 tmpValue;
				stream>>tmpValue.x>>tmpValue.y>>tmpValue.z;
				param->setValue(tmpValue);
			}
			else if(typeName == _T("float4"))
			{
				Vec4 tmpValue;
				stream>>tmpValue.x>>tmpValue.y>>tmpValue.z>>tmpValue.w;
				param->setValue(tmpValue);
			}
			else if(typeName == _T("int"))
			{
				int tmpValue;
				stream>>tmpValue;
				param->setValue(tmpValue);
			}
			else if(typeName == _T("texture") || typeName == _T("string"))
			{
				//TCHAR tmpValue[MAX_PATH];
				param->setValue(pParamChild->GetAttribute(_T("value")));
			}
			else
			{
				// if not define params, skip [11/19/2011 Kaiming]
				continue;
			}

		}
	}
	else
	{
		// 否则检查比对，不同的清理或添加
		m_pLoadingParamBlock->ripFromOther( *shaderParam );
		firstLoad = false;
	}

	// load textures

	// this texture array should be reallocate
	LoadTexturesFromXmlNode(rootXmlNode);


	// load it
	return loadFromParamBlock();
}

void gkMaterial::ApplyParams(IShader* shader)
{
	gkMaterialParams::ParameterList::iterator it = m_pLoadingParamBlock->m_vecParams.begin();
	for ( it ; it != m_pLoadingParamBlock->m_vecParams.end(); ++it )
	{
		GKSHADERPARAM* param = *it;

		switch(param->Type)
		{
		case GKSHADERT_FLOAT:
		case GKSHADERT_FLOAT2:
		case GKSHADERT_FLOAT3:
		case GKSHADERT_FLOAT4:
		case GKSHADERT_DWORD:
			shader->FX_SetValue( param->pParamName, param->pValue, param->NumBytes );
		}

		
	}
}

void gkMaterial::rebuildMarco()
{
	m_bLoadFromFile = true;

	m_marcoMask = 0;

	ShaderMacros& marcos = getShaderMarcos();
	ShaderMacros::iterator it = marcos.begin();
	for ( ; it != marcos.end(); ++it )
	{
		if( it->second.m_active )
		{
			m_marcoMask |= it->first;
		}
	}
}

void gkMaterial::changeShader(const TCHAR* shadername)
{
	m_shaderFilename = shadername;
	m_shaderFileSet = true;
}

void gkMaterial::LoadTexturesFromXmlNode(CRapidXmlParseNode* rootXmlNode)
{
	m_vecTextureArray.assign(8, gkTexturePtr(NULL));

	// assign basic texture
	m_vecTextureArray[eMS_Diffuse] = gkTextureManager::ms_DefaultDiffuse;
	m_vecTextureArray[eMS_Normal] = gkTextureManager::ms_DefaultNormal;
	m_vecTextureArray[eMS_Specular] = gkTextureManager::ms_DefaultDiffuse;
	m_vecTextureArray[eMS_Detail] = gkTextureManager::ms_DefaultNormal;
	m_vecTextureArray[eMS_Custom1] = gkTextureManager::ms_DefaultDiffuse;
	m_vecTextureArray[eMS_Custom2] = gkTextureManager::ms_DefaultDiffuse;
	m_vecTextureArray[eMS_EnvMap] = gkTextureManager::ms_DefaultDiffuse;
	//m_vecTextureArray[eMS_CubeMap] = gkTextureManager::ms_DefaultDiffuse;


	CRapidXmlParseNode *pParamTexChannel = rootXmlNode->getChildNode( _T("TexChannel") );

	if (pParamTexChannel)
	{
		CRapidXmlParseNode* texchn = pParamTexChannel->getChildNode();

		// 遍历params
		for ( texchn ; texchn != 0; texchn = texchn->getNextSiblingNode()) 
		{
			int slot = 0;
			texchn->GetAttribute(_T("chn"), slot);

			// 字符串的情况：1.贴图，2.脚本(暂时不支持)
			const TCHAR* texturename = texchn->GetAttribute(_T("value"));

			if (texturename)
			{
				gkTexturePtr pTexture =getRenderer()->getTextureMngPtr()->load(texturename, _T("external"));

				if (!pTexture.isNull())
				{
					m_vecTextureArray[slot] = pTexture;
				}
			}

		}
	}
	else
	{
		//////////////////////////////////////////////////////////////////////////
		// 老版本纹理读取
		CRapidXmlParseNode* fxXmlNode = rootXmlNode->getChildNode();
		CRapidXmlParseNode *pParamChild = fxXmlNode->getChildNode();

		// 遍历params
		for ( pParamChild ; pParamChild != 0; pParamChild = pParamChild->getNextSiblingNode()) 
		{
			gkStdString typeName = pParamChild->GetAttribute(_T("type"));
			gkStdString elementName = pParamChild->GetAttribute(_T("name"));
			if ( typeName == _T("texture") || typeName == _T("string") )
			{
				EMaterialSlot slot = eMS_Diffuse;
				if (elementName == _T("texDiffuse"))
				{
					slot = eMS_Diffuse;
				}
				else if (elementName == _T("texNormal"))
				{
					slot = eMS_Normal;
				}
				else if (elementName == _T("texSpec"))
				{
					slot = eMS_Specular;
				}
				else if (elementName == _T("texDetail"))
				{
					slot = eMS_Detail;
				}
				else if (elementName == _T("texCustom1"))
				{
					slot = eMS_Custom1;
				}
				else if (elementName == _T("texCustom2"))
				{
					slot = eMS_Custom2;
				}
				else if (elementName == _T("IBLenvmap"))
				{
					slot = eMS_EnvMap;
				}
				else if (elementName == _T("IBLcubemap"))
				{
					slot = eMS_CubeMap;
				}

				if ( slot < eMS_Invalid )
				{

					// 字符串的情况：1.贴图，2.脚本(暂时不支持)
					const TCHAR* texturename = pParamChild->GetAttribute(_T("value"));
					gkTexturePtr pTexture =getRenderer()->getTextureMngPtr()->load(texturename, _T("external"));

					if (!pTexture.isNull())
					{
						m_vecTextureArray[slot] = pTexture;
					}
				}
				else
				{
					GK_ASSERT(0);
				}
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
}




