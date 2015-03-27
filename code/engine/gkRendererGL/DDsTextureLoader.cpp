#include "StableHeader.h"
#include "DDsTextureLoader.h"

#include "RenderRes/DDS.h"

#define RESIZE_POWER 0

EHwTexError DDS_Loader::Load_Data(const void* pointer, uint32 size)
{



	const uint8* start = (const uint8*)pointer;

	// DDS files always start with the same magic number ("DDS ")
	DWORD dwMagicNumber = *( DWORD* )( start );
	if( dwMagicNumber != DDS_MAGIC )
	{
		return PVR_FAIL;
	}

	const DDS_HEADER* pHeader = reinterpret_cast<const DDS_HEADER*>( start + sizeof( DWORD ) );

	// Verify header to validate DDS file
	if( pHeader->dwSize != sizeof(DDS_HEADER)
		|| pHeader->ddspf.dwSize != sizeof(DDS_PIXELFORMAT) )
	{
		return PVR_FAIL ;
	}

	// Check for DX10 extension
	bool bDXT10Header = false;
	if ( (pHeader->ddspf.dwFlags & DDS_FOURCC)
		&& (MAKEFOURCC( 'D', 'X', '1', '0' ) == pHeader->ddspf.dwFourCC) )
	{
		return PVR_FAIL;
	}

	// setup the pointers in the process request
	int32 offset = sizeof( DWORD ) + sizeof( DDS_HEADER );

	const uint8* pBitData = start + offset;
	uint32 BitSize = size - offset;

	iWidth = pHeader->dwWidth;
	iHeight = pHeader->dwHeight;
	iMipCount = pHeader->dwMipMapCount;


	iArraySize = 1;

	if (pHeader->dwCubemapFlags != 0
		|| (pHeader->dwHeaderFlags & DDS_HEADER_FLAGS_VOLUME) )
	{
		// For now only support 2D textures, not cubemaps or volumes
		// return;

		// load cubemap dds
		// load just as 6 arrayable dds
		iArraySize = 6;
	}

	fmt = GetGLInternalFormat( pHeader->ddspf );

	uint32 RowBytes, NumRows;
	pSrcBits = pBitData;

	if (RESIZE_POWER > 0)
	{
		GetSurfaceInfo( iWidth, iHeight, fmt, NULL, &RowBytes, &NumRows );
		for( uint32 h = 0; h < NumRows; h++ )
		{
			pSrcBits += RowBytes;
		}
	}

	iWidth = iWidth >> RESIZE_POWER;
	iHeight = iHeight >> RESIZE_POWER;
	iMipCount = iMipCount - RESIZE_POWER;


	if( 0 >= iMipCount )
		iMipCount = 1;

	return PVR_SUCCESS;
}

EHwTexError DDS_Loader::Load_Bind(GLuint *const texName, const unsigned int nLoadFromLevel)
{
	GLenum error = glGetError();
	if (error)
	{
		//gkLogWarning(_T("gen texture failed."));
	}

	glGenTextures(1, texName);
	error = glGetError();
	if (error)
	{
		gkLogWarning(_T("gen texture failed."));
	}
	GLint eTarget = GL_TEXTURE_2D;
	if (iArraySize > 1)
	{
		eTarget = GL_TEXTURE_CUBE_MAP;
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	error = glGetError();
	if (error)
	{
		gkLogWarning(_T("unbind texture failed."));
	}

	glBindTexture(eTarget, *texName);

	error = glGetError();
	if (error)
	{
		gkLogWarning(_T("bind texture failed."));
	}

	// 		glTexParameteri(eTarget, GL_TEXTURE_BASE_LEVEL, 0);
	// 		glTexParameteri(eTarget, GL_TEXTURE_MAX_LEVEL, iMipCount);

	if (iArraySize > 1)
	{
		//glEnable(GL_TEXTURE_CUBE_MAP);
		eTarget = GL_TEXTURE_CUBE_MAP_POSITIVE_X;
	}

	uint32 RowBytes, NumRows;

	uint32 rtWidth = iWidth;
	uint32 rtHeight = iHeight;

	// 		static uint8* red256bitmap = NULL;
	// 		if (red256bitmap == NULL)
	// 		{
	// 			red256bitmap = new uint8[256 * 256 * 4 * 2];
	// 			memset(red256bitmap, 0xff, 256 * 256 * 4 * 2);
	// 		}

	for (int a = 0; a < iArraySize; ++a)
	{

		rtWidth = iWidth;
		rtHeight = iHeight;

		for (int i = 0; i < iMipCount; i++)
		{
			GetSurfaceInfo(rtWidth, rtHeight, fmt, NULL, &RowBytes, &NumRows);


			{
				//BYTE* pDestBits = ( BYTE* )LockedRect.pBits;
				if (IsCompressedTex(fmt))
				{
					glCompressedTexImage2D(
						eTarget + a,
						i,
						fmt,
						rtWidth,
						rtHeight,
						0,
						RowBytes * NumRows,
						pSrcBits);

					GLenum error = glGetError();
					if (error)
					{
						gkLogWarning(_T("bind compress texture error."));
					}
				}
				else
				{

					//uint8* white = new uint8[iWidth * iHeight * 4];
					//memset(white, 0xff, iWidth * iHeight * 4);

					glTexImage2D(
						eTarget + a,
						i,
						fmt,
						rtWidth,
						rtHeight,
						0,
						GL_BGRA,
						GL_UNSIGNED_BYTE,
						pSrcBits
						);


					//delete white;

					GLenum error = glGetError();
					if (error)
					{
						gkLogWarning(_T("bind texture error."));
					}
				}

				//glPixelStorei(GL_UNPACK_ALIGNMENT, 4);


				pSrcBits += (RowBytes * NumRows);

			}

			GLenum error = glGetError();
			if (error)
			{
				gkLogWarning(_T("bind texture error."));
			}

			rtWidth = rtWidth >> 1;
			rtHeight = rtHeight >> 1;
			if (rtWidth == 0)
				rtWidth = 1;
			if (rtHeight == 0)
				rtHeight = 1;
		}

	}

	if (eTarget != GL_TEXTURE_2D)
	{
		eTarget = GL_TEXTURE_CUBE_MAP;
	}


	glTexParameteri(eTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (eTarget != GL_TEXTURE_2D)
	{
		glTexParameteri(eTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
	else
	{
		glTexParameteri(eTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	}


	glTexParameteri(eTarget, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(eTarget, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(eTarget, GL_TEXTURE_WRAP_R, GL_REPEAT);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 128, 128, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(eTarget, 0);


	return PVR_SUCCESS;
}

bool DDS_Loader::Load_IsCube(const void* pointer)
{
	return (iArraySize > 1);
	return false;
}