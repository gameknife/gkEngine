#include "StableHeader.h"
#include "HwTextureLoader.h"
#include "glExtension.h"

uint32 getATCMipSize( uint32 width, uint32 height, uint32 flags, uint32 mip_level )
{
	uint32 nTotalBlocks, nBytesPerBlock, nHasAlpha;
	nTotalBlocks = ((width + 3) >> 2) * ((height + 3) >> 2);
	nHasAlpha = flags & ATC_RGBA;
	nBytesPerBlock = (nHasAlpha) ? 16 : 8;
	uint32 size   = nTotalBlocks * nBytesPerBlock;
	size = size >> (mip_level * 2);

	return size;
}

bool PVR_Loader::Load_IsCube(const void* pointer)
{
	//Just in case header and pointer for decompression.
	PVRTextureHeaderV3 sTextureHeaderDecomp;
	sTextureHeader=*(PVRTextureHeaderV3*)pointer;

	return (sTextureHeader.u32NumFaces == 6);
}

EHwTexError PVR_Loader::Load_Data( const void* pointer )
{
	//Compression bools
	bIsCompressedFormat=false;
	eTextureFormat = 0;
	eTextureInternalFormat = 0;
	eTextureType = 0;

	//Texture setup
	pTextureData=NULL;

	//Just in case header and pointer for decompression.
	PVRTextureHeaderV3 sTextureHeaderDecomp;
	void* pDecompressedData=NULL;

	//Check if it's an old header format
	if((*(uint32*)pointer)!=PVRTEX3_IDENT)
	{
		gkLogMessage(_T("PVR Legacy input, not support"));
		return PVR_FAIL;
	}
	else
	{
		sTextureHeader=*(PVRTextureHeaderV3*)pointer;
		pTextureData = (uint8*)pointer+PVRTEX3_HEADERSIZE+sTextureHeader.u32MetaDataSize;
	}

	//Get the OGLES format values.
	PVRTGetOGLES2TextureFormat(sTextureHeader,eTextureInternalFormat,eTextureFormat,eTextureType);

	//Check supported texture formats.
	bool bIsPVRTCSupported = gkGLExtension::IsGLExtensionSupported("GL_IMG_texture_compression_pvrtc");
#ifndef TARGET_OS_IPHONE
	bool bIsBGRA8888Supported  = gkGLExtension::IsGLExtensionSupported("GL_IMG_texture_format_BGRA8888");
#else
	bool bIsBGRA8888Supported  = gkGLExtension::IsGLExtensionSupported("GL_APPLE_texture_format_BGRA8888");
#endif
	bool bIsFloat16Supported = gkGLExtension::IsGLExtensionSupported("GL_OES_texture_half_float");
	bool bIsFloat32Supported = gkGLExtension::IsGLExtensionSupported("GL_OES_texture_float");
#ifndef TARGET_OS_IPHONE
	bool bIsETCSupported = gkGLExtension::IsGLExtensionSupported("GL_OES_compressed_ETC1_RGB8_texture");
#endif
	bool bIsATCSupported = gkGLExtension::IsGLExtensionSupported("GL_AMD_compressed_ATC_texture");
	//Check for compressed formats
	if (eTextureFormat==0 && eTextureType==0 && eTextureInternalFormat!=0)
	{
		if (eTextureInternalFormat>=GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG && eTextureInternalFormat<=GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG)
		{
			//Check for PVRTCI support.
			if(bIsPVRTCSupported)
			{
				bIsCompressedFormat = true;
			}
			else
			{
				gkLogWarning(_T("PVRTTextureLoadFromPointer error: PVRTC not supported.\n"));
				return PVR_FAIL;
			}
		}
#ifndef TARGET_OS_IPHONE //TODO
		else if (eTextureFormat==GL_ETC1_RGB8_OES)
		{
			if(bIsETCSupported)
			{
				bIsCompressedFormat = true;
			}
			else
			{
				gkLogWarning(_T("PVRTTextureLoadFromPointer error: ETC not supported.\n"));
				return PVR_FAIL;
			}
		}

		else if (eTextureInternalFormat==GL_ATC_RGB_AMD || eTextureInternalFormat==GL_ATC_RGBA_EXPLICIT_ALPHA_AMD || eTextureInternalFormat==GL_ATC_RGBA_INTERPOLATED_ALPHA_AMD )
		{
			if(bIsATCSupported)
			{
				bIsCompressedFormat = true;
			}
			else
			{
				gkLogWarning(_T("PVRTTextureLoadFromPointer error: ATC not supported.\n"));
				return PVR_FAIL;
			}
		}
#endif
	}

	//Check for BGRA support.	
	if(eTextureFormat==GL_BGRA)
	{
#ifdef TARGET_OS_IPHONE
		eTextureInternalFormat = GL_RGBA;
#endif
		if(!bIsBGRA8888Supported)
		{
#ifdef TARGET_OS_IPHONE
			gkLogWarning(_T("PVRTTextureLoadFromPointer failed: Unable to load GL_BGRA texture as extension GL_APPLE_texture_format_BGRA8888 is unsupported.\n"));
#else
			gkLogWarning(_T("PVRTTextureLoadFromPointer failed: Unable to load GL_BGRA texture as extension GL_IMG_texture_format_BGRA8888 is unsupported.\n"));
#endif
			return PVR_FAIL;
		}
	}

	return PVR_SUCCESS;
}

EHwTexError PVR_Loader::Load_Bind( GLuint *const texName, const unsigned int nLoadFromLevel )
{
		//PVR files are never row aligned.
	glPixelStorei(GL_UNPACK_ALIGNMENT,1);
    if(glGetError())
    {
        gkLogWarning(_T("HwTexLoader failed: glPixelStorei() failed.\n"));
    }
	//Generate a texture
	glGenTextures(1, texName);
    if(glGetError())
    {
        gkLogWarning(_T("HwTexLoader failed: glGenTextures() failed.\n"));
    }
	//Initialise a texture target.
	GLint eTarget=GL_TEXTURE_2D;
	
// 	if(sTextureHeader.u32NumFaces>1)
// 	{
// 		eTarget=GL_TEXTURE_CUBE_MAP;
// 	}
    
    if(sTextureHeader.u32NumFaces>1)
	{
		eTarget=GL_TEXTURE_CUBE_MAP;
	}

	//Bind the 2D texture
	glActiveTexture( GL_TEXTURE0 );
    if(glGetError())
    {
        gkLogWarning(_T("HwTexLoader failed: glActiveTexture() failed.\n"));
    }
	glBindTexture(GL_TEXTURE_2D, 0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    if(glGetError())
    {
        gkLogWarning(_T("HwTexLoader failed: glBindTexture(0) failed.\n"));
    }
    glBindTexture(eTarget, *texName);

	if(glGetError())
	{
		gkLogWarning(_T("PVRTTextureLoadFromPointer failed: glBindTexture() failed.\n"));
		//return PVR_FAIL;
	}

	//Initialise the current MIP size.
	uint32 uiCurrentMIPSize=0;

	//Loop through the faces
	//Check if this is a cube map.
	if(sTextureHeader.u32NumFaces>1)
	{
		eTarget=GL_TEXTURE_CUBE_MAP_POSITIVE_X;
	}

	//Initialise the width/height
	uint32 u32MIPWidth = sTextureHeader.u32Width>>nLoadFromLevel;
	uint32 u32MIPHeight = sTextureHeader.u32Height>>nLoadFromLevel;

	//Temporary data to save on if statements within the load loops.
	uint8* pTempData=NULL;
	PVRTextureHeaderV3 *psTempHeader=NULL;
	{
		pTempData=pTextureData;
		psTempHeader=&sTextureHeader;
	}


#if 1
	//Loop through all MIP levels.
	{
		for (uint32 uiMIPLevel=nLoadFromLevel; uiMIPLevel<psTempHeader->u32MIPMapCount; ++uiMIPLevel)
		{
			//Get the current MIP size.
			uiCurrentMIPSize=PVRTGetTextureDataSize(*psTempHeader,uiMIPLevel,false,false);

			GLint eTextureTarget=eTarget;
			for (uint32 uiFace=0; uiFace<psTempHeader->u32NumFaces; ++uiFace)
			{
				//Upload the texture
				if (bIsCompressedFormat)
				{
					gkLogMessage(_T("hwTex %d x %d x %d, %.2fkb.\n"), u32MIPWidth, u32MIPHeight, psTempHeader->u32MIPMapCount, uiCurrentMIPSize / 1024.f);

					glCompressedTexImage2D(eTextureTarget,uiMIPLevel-nLoadFromLevel,eTextureInternalFormat,u32MIPWidth, u32MIPHeight, 0, uiCurrentMIPSize, pTempData);
				}
				else
				{
					glTexImage2D(eTextureTarget,uiMIPLevel-nLoadFromLevel,eTextureInternalFormat, u32MIPWidth, u32MIPHeight, 0, eTextureFormat, eTextureType, pTempData);
				}

				if(glGetError())
				{
					gkLogWarning(_T("Data Flush To Tex Failed.\n"));
					
					//return PVR_FAIL;
				}

				pTempData+=uiCurrentMIPSize;
				eTextureTarget++;
			}

			//Reduce the MIP Size.
			u32MIPWidth=PVRT_MAX(1,u32MIPWidth>>1);
			u32MIPHeight=PVRT_MAX(1,u32MIPHeight>>1);

		}
	}

#else

	GLint eTextureTarget=eTarget;

	//Loop through all the faces.
	for (uint32 uiFace=0; uiFace<psTempHeader->u32NumFaces; ++uiFace)
	{
		//Loop through all the mip levels.
		for (uint32 uiMIPLevel=nLoadFromLevel; uiMIPLevel<psTempHeader->u32MIPMapCount; ++uiMIPLevel)
		{
			//Get the current MIP size.
			uiCurrentMIPSize=PVRTGetTextureDataSize(*psTempHeader,uiMIPLevel,false,false);

			//Upload the texture
			if (bIsCompressedFormat)
			{
				glCompressedTexImage2D(eTextureTarget,uiMIPLevel-nLoadFromLevel,eTextureInternalFormat,u32MIPWidth, u32MIPHeight, 0, uiCurrentMIPSize, pTempData);
			}
			else
			{
				glTexImage2D(eTextureTarget,uiMIPLevel-nLoadFromLevel,eTextureInternalFormat, u32MIPWidth, u32MIPHeight, 0, eTextureFormat, eTextureType, pTempData);
			}

			if(glGetError())
			{
				gkLogWarning(_T("Data Flush To Tex Failed.\n"));
				//return PVR_FAIL;
			}

			pTempData+=uiCurrentMIPSize;

			//Reduce the MIP Size.
			u32MIPWidth=PVRT_MAX(1,u32MIPWidth>>1);
			u32MIPHeight=PVRT_MAX(1,u32MIPHeight>>1);
		}

		//Increase the texture target.
		eTextureTarget++;

		//Reset the current MIP dimensions.
		u32MIPWidth=psTempHeader->u32Width>>nLoadFromLevel;
		u32MIPHeight=psTempHeader->u32Height>>nLoadFromLevel;

		//Error check
// 		if(glGetError())
// 		{
// 			free(pDecompressedData);
// 			gkLogWarning(_T("PVRTTextureLoadFromPointer failed: glTexImage2D() failed.\n"));
// 			return PVR_FAIL;
// 		}
	}

#endif

	if (eTarget!=GL_TEXTURE_2D)
	{
		eTarget=GL_TEXTURE_CUBE_MAP;
	}

	//Error check
	if(glGetError())
	{
		gkLogWarning(_T("PVRTTextureLoadFromPointer failed: glTexImage2D() failed.\n"));
		return PVR_FAIL;
	}
	
	//Set Minification and Magnification filters according to whether MIP maps are present.
	if(eTextureType==GL_FLOAT || eTextureType==GL_HALF_FLOAT_OES)
	{
		if(sTextureHeader.u32MIPMapCount==1)
		{	// Texture filter modes are limited to these for float textures
			glTexParameteri(eTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(eTarget, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		}
		else
		{
			glTexParameteri(eTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
			glTexParameteri(eTarget, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		}
	}
	else
	{
		if(sTextureHeader.u32MIPMapCount==1)
		{
			glTexParameteri(eTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(eTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		else
		{
			glTexParameteri(eTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
			glTexParameteri(eTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
	}

	if(	(sTextureHeader.u32Width & (sTextureHeader.u32Width - 1)) | (sTextureHeader.u32Height & (sTextureHeader.u32Height - 1)))
	{
		/*
			NPOT textures requires the wrap mode to be set explicitly to
			GL_CLAMP_TO_EDGE or the texture will be inconsistent.
		*/
		glTexParameteri(eTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(eTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
	else
	{
		glTexParameteri(eTarget, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(eTarget, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

	//Error check
	if(glGetError())
	{
		gkLogWarning(_T("PVRTTextureLoadFromPointer failed: glTexParameter() failed.\n"));
		return PVR_FAIL;
	}

	return PVR_SUCCESS;
}
