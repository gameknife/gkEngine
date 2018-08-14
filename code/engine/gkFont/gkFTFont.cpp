#include "gkFTFont.h"
#include "ISystem.h"
#include "IResFile.h"
#include "IResourceManager.h"
#include "IRenderSequence.h"
#include "IRenderer.h"
#include "gkRenderOperation.h"
#include "UnicodeConverter.h"

#define table_texture_width 1024
#define table_texture_width_f 1024.f
#define table_texture_width_str "1024"

gkFTFont::gkFTFont(void)
{
	m_face = NULL;
	m_face2 = NULL;
}

gkFTFont::~gkFTFont(void)
{
}

void gkFTFont::init( const gkFTFontKey& key, int height, int weight )
{
	gkAutoLock<gkCritcalSectionLock> lock(eLGID_global, 0xff);
	int error = 0;

	m_key = key;

	IResFile* pFile = gEnv->pFileSystem->loadResFile( m_key.m_filename.c_str() );

	if (pFile)
	{
		
		// load font file
		error = FT_New_Memory_Face( getFontMain()->getFTLib(), (FT_Byte*)(pFile->DataPtr()), pFile->Size(), 0, &m_face );
		error = FT_Select_Charmap(m_face,FT_ENCODING_UNICODE);

		// build pixel info
		if (1)
		{
			//float lean=0.35f;
			FT_Matrix matrix;
			matrix.xx=0x10000L;

			// italic
// 			if (m_bItalic)
// 			{
// 				matrix.xy=(FT_Fixed)(lean*0x10000L);
// 			} else

			{
				matrix.xy=0;
			}

			matrix.yx = 0;
			matrix.yy = 0x10000L;
			FT_Set_Transform(m_face,&matrix,0);
		}
		error = FT_Set_Pixel_Sizes(m_face,0,height);
	}
	else
	{
		gkLogError(_T("font file not exist"));
	}

	if (m_key.m_filename2 != _T("") && m_key.m_filename2 != m_key.m_filename)
	{
		IResFile* secondFile = gEnv->pFileSystem->loadResFile( m_key.m_filename2.c_str() );
		if (secondFile)
		{

			// load font file
			error = FT_New_Memory_Face( getFontMain()->getFTLib(), (FT_Byte*)(secondFile->DataPtr()), secondFile->Size(), 0, &m_face2 );

			if (error)
			{
				gkLogError(_T("parse font file failed") );
			}

			error = FT_Select_Charmap(m_face2,FT_ENCODING_UNICODE);

			if (error)
			{
				gkLogError(_T("parse font file failed") );
			}

			// build pixel info
			if (1)
			{
				//float lean=0.35f;
				FT_Matrix matrix;
				matrix.xx=0x10000L;

				// italic
				// 			if (m_bItalic)
				// 			{
				// 				matrix.xy=(FT_Fixed)(lean*0x10000L);
				// 			} else

				{
					matrix.xy=0;
				}

				matrix.yx = 0;
				matrix.yy = 0x10000L;
				FT_Set_Transform(m_face2,&matrix,0);
			}
			error = FT_Set_Pixel_Sizes(m_face2,0,height);
		}
	}

	gkNameValuePairList createlist;
	TCHAR buffer[MAX_PATH];

	// set params
	_stprintf(buffer, _T("Font_%s_%d_%d"), m_key.m_filename.c_str(), height, weight);
	createlist[_T("file")] = _T("engine/assets/meshs/font.mtl");

// 	m_material = gEnv->pSystem->getMaterialMngPtr()->create( buffer, _T("font"), &createlist );
// 	m_material->load();

	m_textRenderIndex = -1;

	m_height = height;
	m_weight = weight * 64;
	addTexture();

	m_pixelOffset = 0.5f;

	if (gEnv->pRenderer->GetRendererAPI() != ERdAPI_D3D9)
	{
		m_pixelOffset = 0.0f;
	}

	m_spaceWidth = m_height / 2;
	m_useKernering = FT_HAS_KERNING( m_face );
	m_ascend = 0;
	m_ascend = m_face->size->metrics.ascender / 72;
	m_lineGap = (m_face->size->metrics.height) / 64;
}

void gkFTFont::addTexture()
{
	m_textRenderIndex++;
	m_textRenderBufferi.zero();
	m_textRenderBufferi.z = m_height;

	gkNameValuePairList createlist;

	// set params
	createlist[_T("d3dpool")]		=	_T("D3DX_DEFAULT");
	createlist[_T("usage")]		=	_T("RAW");
	createlist[_T("size")]			=	_T(table_texture_width_str);
	createlist[_T("format")]		=	_T("A8");

	TCHAR strName[MAX_PATH];
#if defined(__x86_64__) || defined(__arm64__)
	_stprintf( strName, _T("FONTMAP_%u_%d"), (uint64)this, m_textRenderIndex );
#else
    _stprintf( strName, _T("FONTMAP_%u_%d"), (uint32)this, m_textRenderIndex );
#endif
	gkTexturePtr tex = gEnv->pSystem->getTextureMngPtr()->create( strName, _T("font"), &createlist );

	gkFontRenderable renderable;
	renderable.m_texture = tex;

	TCHAR buffer[MAX_PATH];


	// set params
	_stprintf(buffer, _T("Font_%s_%d_%d_%d"), m_key.m_filename.c_str(), m_height, m_weight, m_textRenderIndex);
	createlist[_T("file")] = _T("engine/assets/meshs/font.mtl");

	gkMaterialPtr mat = gEnv->pSystem->getMaterialMngPtr()->create( buffer, _T("font"), &createlist );
	mat->load();
	mat->setTexture( tex, 0 );
	renderable.m_material = mat;

	createlist[_T("type")] =	_T("P3T2U4");
	gkMeshPtr mesh = gEnv->pSystem->getMeshMngPtr()->create( buffer, _T("font"), &createlist );
	mesh->load();
	renderable.m_mesh = mesh;

	renderable.resizeHwBuffer();

	m_renderables.push_back(renderable);

	m_current_renderable = &(m_renderables.back());
}


gkFTCharInfo& gkFTFont::getCharacterInfo( wchar_t character )
{
	gkFTCharMap::iterator it = m_charMap.find(character);
	if (it != m_charMap.end())
	{
		return it->second;
	}
	else
	{
		// need create

		FT_Face tmpFace = NULL;
		if ( character >= 0x30 && character <= 0x7a && m_face2 )
		{
			tmpFace = m_face2;
		}
		else
		{
			tmpFace = m_face;
		}

		int error=0;
		FT_UInt glyph_index = FT_Get_Char_Index(tmpFace, character);
		error = FT_Load_Glyph(tmpFace, glyph_index, FT_LOAD_DEFAULT);  

		// BOLD
		if (m_weight)
		{                                                                     
 			if ( tmpFace->glyph->format == FT_GLYPH_FORMAT_OUTLINE )             
 				FT_Outline_Embolden( &tmpFace->glyph->outline, m_weight );       
		}

		if (tmpFace->glyph->format != FT_GLYPH_FORMAT_BITMAP) 
		{
			error = FT_Render_Glyph(tmpFace->glyph, FT_RENDER_MODE_NORMAL);
		}
		//////////////////////////////////////////////////////////////////////////
		int32 width = tmpFace->glyph->bitmap.width;
		int32 height = tmpFace->glyph->bitmap.rows;
		//////////////////////////////////////////////////////////////////////////

		// 计算和分配空间
		if (m_textRenderBufferi.x+width > table_texture_width)
		{
			m_textRenderBufferi.x=0;
			m_textRenderBufferi.y+=( m_textRenderBufferi.z + 1 );
		} 
		else
		{
			m_textRenderBufferi.z=max(m_textRenderBufferi.z,height);
		}

		// 这里的判断,要加上height,不然可能写越界
		if (m_textRenderBufferi.y + height>table_texture_width)
		{
			// 当前纹理已经用完，开下一张纹理
			addTexture();
		}

		//////////////////////////////////////////////////////////////////////////
		// map到字体纹理
		int posx = (m_textRenderBufferi.x);
		int posy = (m_textRenderBufferi.y);

		gkLockOperation lockOpt;
		lockOpt.m_flag = gkLockOperation::eLF_Keep;
		lockOpt.m_level = 0;
		lockOpt.m_pos = Vec2i(posx, posy);
		lockOpt.m_size = Vec2i(width, height);

		if (width <= 0 || height <= 0)
		{
			gkLogError(_T("Font Glyph to Zero!"));
		}

		// copy to raw texture
 		m_renderables[m_textRenderIndex].m_texture->touch();
		if( m_renderables[m_textRenderIndex].m_texture->lock( lockOpt ) )
		{
			for (int i=0;i<height;i++)
			{
				BYTE* src = tmpFace->glyph->bitmap.buffer+tmpFace->glyph->bitmap.pitch*i;
				BYTE* dst = (uint8*)(lockOpt.m_outData) +  lockOpt.m_outPitch*i;

				// overwrite
				memcpy(dst,src,sizeof(BYTE)*width);

				// ADD ALPHA IF SIZE LESS THAN 12
				if (m_height < 12)
				{
					for (int k=0; k < width; ++k)
					{
						dst[k] = min(255, max(0, dst[k] - 25) * 2);
					}
				}
			}

			m_renderables[m_textRenderIndex].m_texture->unlock();
		}
		
		gkFTCharInfo info;

		info.texcoordInfo = Vec4( (m_textRenderBufferi.x + m_pixelOffset) / table_texture_width_f, (m_textRenderBufferi.y + m_pixelOffset )/ table_texture_width_f, width / table_texture_width_f, height / table_texture_width_f );
		info.tex_index = m_textRenderIndex;
//		old method
  		info.width = width;
  		info.height = height;

 	
// 		info.width = tmpFace->glyph->metrics.width/64;
// 		info.height = tmpFace->glyph->metrics.height/64;
		

 		info.bearingX = tmpFace->glyph->metrics.horiBearingX/64;
 		info.bearingY = tmpFace->glyph->metrics.horiBearingY/64;
 		info.advanceInfo = tmpFace->glyph->metrics.horiAdvance/64;
		info.advanceInfo += (width - tmpFace->glyph->metrics.width/64) / 2;

		m_charMap.insert( gkFTCharMap::value_type( character, info) );

		m_textRenderBufferi.x+= (width+1);

		return getCharacterInfo( character );
	}
}

void gkFTFont::drawString( const TCHAR* strings, const Vec2& pos, const ColorB& color, uint32 alignment, uint32 depth_order )
{
	// get char info 1 by 1
	wchar_t* wstr = NULL;

#ifdef UNICODE
	wstr = strings;
#else
	uint32 len = strlen(strings);
	wstr = new wchar_t[len + 1];
    
#ifdef OS_WIN32
    //MultiByteToWideChar( CP_ACP, NULL, strings, NULL, wstr, len + 1 );
	CUnicodeConverter::UTF8Str_To_UTF16Str((BYTE*)strings, (WORD*)wstr);
	//CUnicodeConverter::UTF8Str_To_UTF32Str((BYTE*)strings, (DWORD*)wstr);
#else
	CUnicodeConverter::UTF8Str_To_UTF32Str( (BYTE*)strings, (DWORD*)wstr );
#endif
    
#endif
	
	Vec2i rect = measureString(wstr);

	// 这里的顶点坐标，必须是整数！
	// 使用上基线距离(ascend)来调整第一行的渲染位置，使得是从字符顶部开始渲染
	Vec3i tmpPos = Vec3i(pos.x, pos.y + m_ascend, 0);

	if (alignment & eFA_Left)
	{
		
	}
	else if(alignment & eFA_HCenter)
	{
		tmpPos.x -= rect.x * .5f;
	}
	else
	{
		tmpPos.x -= rect.x;
	}

	if (alignment & eFA_Top)
	{
		
	}
	else if (alignment & eFA_VCenter)
	{
		tmpPos.y -= rect.y * .5f;
	}
	else if (alignment & eFA_Bottom)
	{
		tmpPos.y -= rect.y;
	}

	Vec3i tmpPos2;
	tmpPos2.zero();

	FT_UInt glyphPrev = 0;

	for ( uint32 i=0; i < wcslen(wstr); ++i )
	{
		wchar_t thisChar = wstr[i];

		int advance = StepByCharacter(thisChar, tmpPos2, glyphPrev);

		if (advance == 0)
		{
			continue;
		}

		gkFTCharInfo& info = getCharacterInfo( thisChar );

		m_renderables[info.tex_index].addCharacter( info, tmpPos2 + tmpPos, color);

		tmpPos2.x += advance;
	}

#ifndef UNICODE
	delete[] wstr;
#endif
}

void gkFTFont::flush()
{
	gkFontRenderables::iterator it = m_renderables.begin();
	for (; it != m_renderables.end(); ++it)
	{
		it->flush();
	}
}

bool gkFTFont::equal( gkFTFontKey& other )
{
	return (m_key == other);
}

Vec2i gkFTFont::measureString( const char* strings )
{
	int len = strlen(strings);
	wchar_t* wstr = new wchar_t[len + 1];

#ifdef OS_WIN32
	MultiByteToWideChar( CP_ACP, NULL, strings, NULL, wstr, len + 1 );
#else
	CUnicodeConverter::UTF8Str_To_UTF32Str( (BYTE*)strings, (DWORD*)wstr );
#endif

	Vec2i ret = measureString( wstr );

	delete[] wstr;

	return ret;
}

Vec2i gkFTFont::measureString( const wchar_t* wstr )
{
	Vec3i tmpPos;
	tmpPos.zero();

	Vec2i textRect;
	textRect.zero();

	// 先把第一行的高度加上
	tmpPos.y += m_height;

	// 遍历文字
	FT_UInt glyphPrev = 0;
	for ( uint32 i=0; i < wcslen(wstr); ++i )
	{
		wchar_t thisChar = wstr[i];
		
		int advance = StepByCharacter(thisChar, tmpPos, glyphPrev);

		if (advance == 0)
		{
			continue;
		}

		tmpPos.x += advance;

		textRect.x = max(tmpPos.x, textRect.x);
		textRect.y = tmpPos.y;
	}

	return textRect;
}

int gkFTFont::StepByCharacter( wchar_t thisChar, Vec3i &tmpPos, FT_UInt& glyphPrev )
{
	// 开始决定步进值
	FT_Face tmpFace = NULL;
	if ( thisChar >= 0x30 && thisChar <= 0x7a && m_face2 )
	{
		tmpFace = m_face2;
	}
	else
	{
		tmpFace = m_face;
	}

	// 换行符, 空格, tab等的处理
	if ( thisChar == L'\n')
	{
		tmpPos.y += m_lineGap;
		tmpPos.x = 0;
		return 0;
	}
	if ( thisChar == L'\0')
	{
		return 0;
	}
	if ( thisChar == L' ')
	{
		tmpPos.x += m_spaceWidth;
		return 0;
	}
	if ( thisChar == L'\t')
	{
		tmpPos.x = m_spaceWidth * 3;
		return 0;
	}

	gkFTCharInfo& info = getCharacterInfo( thisChar );

	FT_UInt glyph_index = FT_Get_Char_Index(tmpFace, thisChar);
	if ( glyphPrev != 0 )
	{
		FT_Vector delta;
		FT_Get_Kerning( tmpFace, glyphPrev, glyph_index, FT_KERNING_DEFAULT, &delta );
		tmpPos.x += delta.x >> 6;
		//tmpPos.x += m_weight / 64 * 2;
	}
	glyphPrev = glyph_index;
	
	

	return info.advanceInfo;
}

void gkFTFont::destroy()
{
	
	if( m_face2 )
	{
		FT_Done_Face(m_face2);
	}

	FT_Done_Face( m_face );
}
