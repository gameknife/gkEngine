#include "shaderCompiler.h"
#include "gk_Color.h"
#include "IXmlUtil.h"
#include <iomanip>


shaderCompiler::shaderCompiler(void)
{
	g_compilers.push_back( this );
}


shaderCompiler::~shaderCompiler(void)
{
}

gkStdString shaderCompiler::getMacroProfile(uint32 macromask)
{
	gkStdString ret = _T("");

	std::map<uint32, ShaderMacro>::iterator it = m_shaderMacros.begin();
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

bool shaderCompiler::encode(const char* filename, const char* cfg)
{

	gkStdString m_gfxName = gkGetPureFilename( filename );

	// compiler for start to end
	IRapidXmlParser parser;
	parser.initializeReading( filename );

	//printf( parser.buf );
	CRapidXmlParseNode* rootNode = parser.getRootXmlNode();

	// read vfx & ffx file
	CRapidXmlParseNode* d3d9shaderNode = rootNode->getChildNode(_T("D3D9Shader"));

	if(!d3d9shaderNode)
	{
		return false;
	}
	CRapidXmlParseNode* effectNode = d3d9shaderNode->getChildNode(_T("Effect"));
	CRapidXmlParseNode* marcoNode = rootNode->getChildNode(_T("Marco"));
	CRapidXmlParseNode* layerNode = rootNode->getChildNode(_T("RenderLayer"));

	// find compiled binary
	if (effectNode)
	{
		TCHAR filenamepure[MAX_PATH];

		strcpy( filenamepure,  effectNode->GetAttribute(_T("FileName")) );

		int large_mask = 0;

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

				large_mask |= maskflag;
			}
		}
			uint32 systemMacro[2];
			systemMacro[0] = 0;
			systemMacro[1] = eSSM_Skinned;

			for (int k=0; k < 2; ++k )
			{
				
				for (int i=0; i < large_mask + 1; ++i)
				{
					uint32 final_mask = (systemMacro[k] & 0xff000000) | i;
					gkStdString marcoProfile = getMacroProfile( final_mask );

					// should merge with mask, use string stream
					gkStdStringstream ss;

					ss << _T("engine/shaders/d3d9/shadercache/") << gkStdString(m_gfxName) <<  _T("@");
					ss <<_T("0x")<<std::setw(8)<<std::setfill(_T('0'))<<std::hex<<std::right<<final_mask;
					ss << _T(".o");

					gkStdString sourcePath = gkGetExecRootDir() + filenamepure;
					gkStdString binaryPath = gkGetExecRootDir() + ss.str();

					// 只要决定了编译，就先删除cache
					DeleteFile( binaryPath.c_str() );

					TCHAR buffer[1024];
					_stprintf(buffer, _T("%sbin32\\fxc.exe /Zpc /T fx_2_0 %s /Fo %s %s"), gkGetExecRootDir().c_str(), marcoProfile.c_str(), binaryPath.c_str(), sourcePath.c_str());


					char bufferA[1024];
					strcpy( bufferA, buffer );
					system(bufferA);
				}

			}
	}
	//system("pause");

	return true;
}

bool shaderCompiler::writeFile(const char* filename, const char* cfg)
{
	// do nothing
	return true;
}

bool shaderCompiler::supportInput(const char* ext)
{
	if (!stricmp(ext, ".gfx"))
	{
		return true;
	}
	return false;
}
bool shaderCompiler::supportOutput(const char* ext)
{
	if (!stricmp(ext, ".o"))
	{
		return true;
	}
	return false;
}
const char* shaderCompiler::getName()
{
	return "shaderCompiler";
}
