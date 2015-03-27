#include "StableHeader.h"
#include "gkMaterial.h"
#include "gkShaderManager.h"
#include "gkTextureManager.h"
#include "gkIterator.h"
#include "gkFilePath.h"
#include "IXmlUtil.h"

#include "gkMemoryLeakDetecter.h"
#include "gkMaterialManager.h"

gkMaterial::gkMaterial( IResourceManager* creator, const gkStdString& name, gkResourceHandle handle, const gkStdString& group /*= _T("none")*/, gkNameValuePairList* params )
:IMaterial(creator,name,handle,group)
{
	//m_hTechnique = NULL;
	//m_hParameterBlock = NULL;
	m_yLoadType = GKMATERIAL_LOADTYPE_FILE;
	//m_hMainDiffuse =NULL;
	m_bDoubleSide = false;
	m_fGlossness = 8.0f;
	m_nOpacity = 100;
	m_pLoadingParamBlock = NULL;
	m_bLoadFromFile = true;
	m_vUVTill = Vec2(1,1);
	m_vUVOffset = Vec2(0,0);
	m_fUVSpeed = 0;
	m_uSubMtlCount = 0;
	m_bSSRL = false;
	m_castShadow = true;

	m_vecTextureArray.clear();
	m_vecTextureArray.assign(8, gkTexturePtr(NULL));

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
	m_vecTextureArray.clear();
	releaseParameterBlock();
	if(isLoaded())
		unload();
}

bool gkMaterial::loadImpl( void )
{

	//////////////////////////////////////////////////////////////////////////
	// 手动加载
	gkNameValuePairList::iterator it = loadingParams.find(_T("file"));
	if (it != loadingParams.end())
	{
		m_wstrIndividualFileName = it->second;
		m_yLoadType = GKMATERIAL_LOADTYPE_INDIVIDUALFILE;
	}

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
			bool succesed = false;
			if (m_bLoadFromFile)
			{
				// if loaded, just get params from params block
				m_bLoadFromFile = false;
				gkLogMessage(_T("load material %s"), m_wstrIndividualFileName.c_str());
				IRapidXmlParser parser;
				parser.initializeReading(m_wstrIndividualFileName.c_str());
				CRapidXmlParseNode* rootXmlNode = parser.getRootXmlNode();

				if (!rootXmlNode)
				{
					return false;
				}

				gkLogMessage(_T("parsing material %s"), m_wstrIndividualFileName.c_str());
				// check
				if (_tcsicmp(rootXmlNode->GetName(), _T("Material")))
				{
					GK_ASSERT(0);
					return false;
				}

				// if main
				if (rootXmlNode->getChildNode(_T("Effect")))
				{
					gkLogMessage(_T("parsing main material %s"), m_wstrIndividualFileName.c_str());
					succesed = loadMaterialFormRapidXmlNode(rootXmlNode);
				}

				// submaterial loading
				CRapidXmlParseNode* subMaterial = rootXmlNode->getChildNode(_T("SubMaterial"));
				if (subMaterial)
				{
					for (; subMaterial; subMaterial = subMaterial->getNextSiblingNode(_T("SubMaterial")))
					{
						m_uSubMtlCount++;
						gkStdString submtlname = m_wstrFileName;
						submtlname.append( _T("|") );
						submtlname.append( subMaterial->GetAttribute(_T("Name")) );
						gkMaterial* pSub = new gkMaterial(NULL, submtlname.c_str(), 0 );
						succesed = pSub->loadMaterialFormRapidXmlNode( subMaterial );

						m_vecSubMtlList.push_back(gkMaterialPtr(pSub));
					}
				}
				gkLogMessage(_T("parsing material %s"), m_wstrIndividualFileName.c_str());

				parser.finishReading();
				gkLogMessage(_T("parsed material %s"), m_wstrIndividualFileName.c_str());
			}
			else
			{
				//succesed = loadFromParamBlock();

				// if sub
				SubMaterialList::iterator it = m_vecSubMtlList.begin();
				for (; it != m_vecSubMtlList.end(); ++it)
				{
					//succesed = ((gkMaterial*)((*it).getPointer()))->loadFromParamBlock();
				}
			}

			return succesed;
		}
		break;

	case GKMATERIAL_LOADTYPE_SCENE:
		{
			//return loadFromParamBlock();
		}
		break;
	}

	return true;
}

bool gkMaterial::unloadImpl(void)
{
	m_vecTextureArray.clear();
	m_vecSubMtlList.clear();
	//m_bLoadFromFile = true;
	//releaseParameterBlock();
	return true;
}

gkShaderPtr& gkMaterial::getShader()
{
//	assert(!m_pShader.isNull());
	return m_pShader;
}

void gkMaterial::BasicApply(IShader* pShader)
{
	Vec4 bindUVParam = Vec4(m_vUVTill.x, m_vUVTill.y, m_vUVOffset.x, m_vUVOffset.y);
	pShader->FX_SetValue("g_UVParam", &bindUVParam, sizeof(Vec4));
}

void gkMaterial::ApplyParameterBlock( bool texture, IShader* shader)
{
	BasicApply(m_pShader.getPointer());

	//int32 nIndex = 0;
	for (int i=0; i<8; ++i)
	{
		if (!(m_vecTextureArray[i].isNull()))
		{
			m_vecTextureArray[i]->Apply( i, 0 );
		}		
	}

	gkMaterialParams::ParameterList::iterator it = m_pLoadingParamBlock->m_vecParams.begin();
	for ( ; it != m_pLoadingParamBlock->m_vecParams.end(); ++it )
	{
 		if( (*it)->Type != GKSHADERT_STRING )
 		{
			m_pShader->FX_SetValue( (*it)->pParamName, (*it)->pValue, (*it)->NumBytes );
		}
	}

}

gkTexturePtr& gkMaterial::getTexture( EMaterialSlot index )
{
    static gkTexturePtr nullPtr(0);
	if (m_vecTextureArray.size() < (uint32)index + 1)
		return nullPtr;
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
	return m_nOpacity;
}

void gkMaterial::saveAsMtlFile( const TCHAR* filename )
{

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

	if (_tcsicmp(fxXmlNode->GetName(), _T("Effect")))
	{
		GK_ASSERT(0);
		return false;
	}

	CRapidXmlParseNode* staticXmlNode = rootXmlNode->getChildNode(_T("Static"));

	if (staticXmlNode)
	{
        CRapidXmlParseNode* uvXmlNode = staticXmlNode->getChildNode();
        if (uvXmlNode->GetAttribute(_T("value")) == 0)
        {
            return false;
        }
        gkStdStringstream stream(uvXmlNode->GetAttribute(_T("value")));
        stream>> m_vUVTill.x >> m_vUVTill.y >> m_vUVOffset.x >> m_vUVOffset.y;
	}

	// 构建一个临时的ParamBlock
	// FIXME [11/2/2011 Kaiming]
	gkMaterialParams* psParamBlock = new gkMaterialParams;
	gkMaterialParams& sParamBlock = *psParamBlock;

	// 赋值
	sParamBlock.m_wstrMatName = rootXmlNode->GetAttribute(_T("Name"));

	// 赋值
	m_shaderFilename = fxXmlNode->GetAttribute(_T("FileName"));

	fxXmlNode->GetAttribute(_T("CastShadow"), m_castShadow);

	// macro

	// 合并mask
	if (m_marcoMask == -1)
	{
		int mask = 0;
		fxXmlNode->GetAttribute(_T("Mask"), mask);
		m_marcoMask = mask;
	}

	// remove extension
	m_shaderFilename = gkGetPureFilename(m_shaderFilename.c_str());

	gkStdStringstream ss;
	ss << m_shaderFilename << _T("@") << m_marcoMask;

	// 在这里建立MARCOS，加载shader
	m_pShader = gEnv->pSystem->getShaderMngPtr()->load(ss.str(), _T(""));

	gkLogMessage(_T("Parsing MaterialParams..."));

	// parameter accessing
	CRapidXmlParseNode *pParamChild = fxXmlNode->getChildNode();

	// 遍历params
	for ( ; pParamChild != 0; pParamChild = pParamChild->getNextSiblingNode()) 
	{
		//GKSHADERPARAM sParam;
		gkStdString typeName;
		GKSHADERPARAM* param = NULL;
		// 先找属性
		// 赋值
		//sParam.pParamName = new char[MAX_PATH];
		//ZeroMemory(sParam.pParamName, sizeof(char) * MAX_PATH);
		char paramName[MAX_PATH];
#ifdef UNICODE
		WideCharToMultiByte(CP_ACP, 0,  pParamChild->GetAttribute(_T("name")), -1, paramName, MAX_PATH, NULL, NULL);
#else
		_tcscpy_s(paramName, MAX_PATH, pParamChild->GetAttribute(_T("name")) );
#endif

		typeName = pParamChild->GetAttribute(_T("type"));
		gkStdStringstream stream(pParamChild->GetAttribute(_T("value")));
		// 根据属性提供的类型，写入
		if(typeName == _T("float"))
		{
			float tmpValue;
			stream>>tmpValue;
			param = new GKSHADERPARAM( paramName, tmpValue );
		}
		else if(typeName == _T("float2"))
		{
			Vec2 tmpValue;
			stream>>tmpValue.x>>tmpValue.y;
			param = new GKSHADERPARAM( paramName, tmpValue );
		}
		else if(typeName == _T("float3"))
		{
			Vec3 tmpValue;
			stream>>tmpValue.x>>tmpValue.y>>tmpValue.z;
			param = new GKSHADERPARAM( paramName, tmpValue );
		}
		else if(typeName == _T("float4"))
		{
			Vec4 tmpValue;
			stream>>tmpValue.x>>tmpValue.y>>tmpValue.z>>tmpValue.w;
			param = new GKSHADERPARAM( paramName, tmpValue );
		}
		else if(typeName == _T("int"))
		{
			int tmpValue;
			stream>>tmpValue;
			param = new GKSHADERPARAM( paramName, tmpValue );
		}
		else if(typeName == _T("texture") || typeName == _T("string"))
		{
			TCHAR tmpValue[MAX_PATH];
			stream>>tmpValue;
			param = new GKSHADERPARAM( paramName, tmpValue );
		}
		else
		{
			// if not define params, skip [11/19/2011 Kaiming]
			continue;
		}


		// 写完了，压入
		// 赋值指针方式，应该可以，看看会不会泄露吧
		if (param)
		{
			sParamBlock.m_vecParams.push_back(param);
		}
		

		//SAFE_DELETE_ARRAY( sParam.pParamName );
	}

	gkLogMessage(_T("Loading MaterialParams..."));
	releaseParameterBlock();
	m_pLoadingParamBlock = &sParamBlock;
    
    // yikaiming glow hack
    if (!getLoadingParameterBlock()->getParam("g_Glow")) {
        //GKSHADERPARAM* para = getLoadingParameterBlock()->getParam("g_Glow");
        GKSHADERPARAM* param = new GKSHADERPARAM("g_Glow", 1.0f );
        m_pLoadingParamBlock->m_vecParams.push_back(param);
    }

    
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
    
    CRapidXmlParseNode* texChannelNode = rootXmlNode->getChildNode(_T("TexChannel"));
    
	if (texChannelNode)
	{
		CRapidXmlParseNode* texchn = texChannelNode->getChildNode();
        
		// 遍历params
		for ( ; texchn != 0; texchn = texchn->getNextSiblingNode())
		{
			int slot = 0;
			texchn->GetAttribute(_T("chn"), slot);
            
			// 字符串的情况：1.贴图，2.脚本(暂时不支持)
			const TCHAR* texturename = texchn->GetAttribute(_T("value"));
            
			if (texturename)
			{
				gkTexturePtr pTexture = gEnv->pSystem->getTextureMngPtr()->load(texturename, _T("external"));
                
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
		for ( ; pParamChild != 0; pParamChild = pParamChild->getNextSiblingNode())
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
					gkTexturePtr pTexture = gEnv->pSystem->getTextureMngPtr()->load(texturename, _T("external"));
                    
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
    
    return true;
	
}




