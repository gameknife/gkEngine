#include "gkVideo.h"
#include "ITexture.h"

#include "vlc/vlc.h"


ITexture* gkVideo::ms_fastPlayTexture = NULL;

gkVideo::gkVideo(void)
{
}

gkVideo::~gkVideo(void)
{
}

void gkVideo::checkFastPlayResource()
{
	if (!ms_fastPlayTexture)
	{
		if (gEnv->pRenderer)
		{
// 			gkNameValuePairList createlist;
// 
// 			// set params
// 			createlist[_T("d3dpool")]		=	_T("D3DX_DEFAULT");
// 			createlist[_T("usage")]		=	_T("RAW");
// 			createlist[_T("size")]			=	_T(table_texture_width_str);
// 			createlist[_T("format")]		=	_T("A8");
// 
// 			gEnv->pSystem->getTextureMngPtr()->create( strName, _T("video"), &createlist );
		}
	}
}

void gkVideo::fastPlay( const TCHAR* path, const Vec4& screen_pos )
{
	
}
