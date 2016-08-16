#include "gkFont.h"
#include "gkFTFont.h"
#include "gkFTFaceManager.h"


#ifdef _M_X64

#ifdef _DEBUG  
#pragma comment(lib,"..\\..\\thirdparty\\freetype\\lib\\freetype_x64debug.lib")
#else
#pragma comment(lib,"..\\..\\thirdparty\\freetype\\lib\\freetype_x64release.lib")
#endif

#else

#ifdef _DEBUG  

#if _MSC_VER == 1900
#pragma comment(lib,"..\\..\\thirdparty\\freetype\\lib\\freetype_win32debug_vc14.lib")
#else
#pragma comment(lib,"..\\..\\thirdparty\\freetype\\lib\\freetype_win32debug.lib")
#endif
#else
#if _MSC_VER == 1900
#pragma comment(lib,"..\\..\\thirdparty\\freetype\\lib\\freetype_win32release_vc14.lib")
#else
#pragma comment(lib,"..\\..\\thirdparty\\freetype\\lib\\freetype_win32release.lib")
#endif
#endif

#endif

gkFont::gkFont(void)
{
}

gkFont::~gkFont(void)
{
}

void gkFont::Init()
{
	FT_Error error = FT_Init_FreeType ( &glibrary ); 
	if (error)
	{
		gkLogError( _T("freetype init failed.") );
	}

	m_faceManager = new gkFTFaceManager();
}

void gkFont::Destroy()
{
	delete m_faceManager;

	// free all font
	gkFTFonts::iterator it = m_fonts.begin();
	for (; it != m_fonts.end(); ++it)
	{
		(*it)->destroy();
		delete (*it);
	}

	FT_Done_FreeType(glibrary);
}

IFtFont* gkFont::CreateFont( const TCHAR* filename, int height, int weight, const TCHAR* secondname )
{
	gkFTFonts::iterator it = m_fonts.begin();

	gkFTFontKey key;
	key.m_filename = filename;
	if (secondname)
	{
		key.m_filename2 = secondname;
	}
	
	key.m_height = height;
	key.m_weight = weight;

	for (; it != m_fonts.end(); ++it)
	{
		if ( (*it)->equal( key ) )
		{
			return (*it);
		}
	}

	gkFTFont* ret = NULL;

	ret = new gkFTFont();
	ret->init(key, height, weight);

	m_fonts.push_back( ret );

	//m_fonts.insert( gkFTFonts::value_type( filename, ret ) );

	return ret;
}

void gkFont::DrawString( const TCHAR* strings, const IFtFont* font, const Vec2& screenpos, const ColorB& color, uint32 alignment )
{
	if (font)
	{
		((gkFTFont*)font)->drawString( strings, screenpos, color, alignment );
	}
}

void gkFont::Flush()
{
	gkFTFonts::iterator it = m_fonts.begin();
	for (; it != m_fonts.end(); ++it)
	{
		(*it)->flush();
	}
}

void gkFont::DestroyFont( IFtFont* target )
{
	gkFTFonts::iterator it = m_fonts.begin();
	for (; it != m_fonts.end(); ++it)
	{
		if ( *it == target )
		{
			(*it)->destroy();
			delete (*it);
			m_fonts.erase(it);
			break;
		}
	}
}
