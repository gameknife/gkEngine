//////////////////////////////////////////////////////////////////////////
/*
Copyright (c) 2011-2015 Kaiming Yi
	
	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:
	
	The above copyright notice and this permission notice shall be included in
	all copies or substantial portions of the Software.
	
	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
	THE SOFTWARE.
	
*/
//////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////
//
// VR Platform (C) 2013
// OpenVRP Kernel Source File 
//
// Name:   	pvrtex_def.h
// Desc:	
// 	
// 
// Author:  YiKaiming
// Date:	2013/5/31
// 
//////////////////////////////////////////////////////////////////////////

#ifndef _pvrtex_def_h_
#define _pvrtex_def_h_



#define PVRT_MIN(a,b)            (((a) < (b)) ? (a) : (b))
#define PVRT_MAX(a,b)            (((a) > (b)) ? (a) : (b))
#define PVRT_CLAMP(x, l, h)      (PVRT_MIN((h), PVRT_MAX((x), (l))))

/*****************************************************************************
* Texture related constants and enumerations. 
*****************************************************************************/
// V3 Header Identifiers.
const uint32 PVRTEX3_IDENT			= 0x03525650;	// 'P''V''R'3
const uint32 PVRTEX3_IDENT_REV		= 0x50565203;   
// If endianness is backwards then PVR3 will read as 3RVP, hence why it is written as an int.

//Current version texture identifiers
const uint32 PVRTEX_CURR_IDENT		= PVRTEX3_IDENT;
const uint32 PVRTEX_CURR_IDENT_REV	= PVRTEX3_IDENT_REV;

// PVR Header file flags.										Condition if true. If false, opposite is true unless specified.
const uint32 PVRTEX3_FILE_COMPRESSED	= (1<<0);		//	Texture has been file compressed using PVRTexLib (currently unused)
const uint32 PVRTEX3_PREMULTIPLIED		= (1<<1);		//	Texture has been premultiplied by alpha value.	

// Mip Map level specifier constants. Other levels are specified by 1,2...n
const int32 PVRTEX_TOPMIPLEVEL			= 0;
const int32 PVRTEX_ALLMIPLEVELS			= -1; //This is a special number used simply to return a total of all MIP levels when dealing with data sizes.

//values for each meta data type that we know about. Texture arrays hinge on each surface being identical in all but content, including meta data. 
//If the meta data varies even slightly then a new texture should be used. It is possible to write your own extension to get around this however.
enum EPVRTMetaData
{
	ePVRTMetaDataTextureAtlasCoords=0,
	ePVRTMetaDataBumpData,
	ePVRTMetaDataCubeMapOrder,
	ePVRTMetaDataTextureOrientation,
	ePVRTMetaDataBorderData,
	ePVRTMetaDataNumMetaDataTypes
};

enum EPVRTAxis
{
	ePVRTAxisX = 0,
	ePVRTAxisY = 1,
	ePVRTAxisZ = 2
};

enum EPVRTOrientation
{
	ePVRTOrientLeft	= 1<<ePVRTAxisX,
	ePVRTOrientRight= 0,
	ePVRTOrientUp	= 1<<ePVRTAxisY,
	ePVRTOrientDown	= 0,
	ePVRTOrientOut	= 1<<ePVRTAxisZ,
	ePVRTOrientIn	= 0
};

enum EPVRTColourSpace
{
	ePVRTCSpacelRGB,
	ePVRTCSpacesRGB
};

//Compressed pixel formats
enum EPVRTPixelFormat
{
	ePVRTPF_PVRTCI_2bpp_RGB,
	ePVRTPF_PVRTCI_2bpp_RGBA,
	ePVRTPF_PVRTCI_4bpp_RGB,
	ePVRTPF_PVRTCI_4bpp_RGBA,
	ePVRTPF_PVRTCII_2bpp,
	ePVRTPF_PVRTCII_4bpp,
	ePVRTPF_ETC1,
	ePVRTPF_DXT1,
	ePVRTPF_DXT2,
	ePVRTPF_DXT3,
	ePVRTPF_DXT4,
	ePVRTPF_DXT5,

	//These formats are identical to some DXT formats.
	ePVRTPF_BC1 = ePVRTPF_DXT1,
	ePVRTPF_BC2 = ePVRTPF_DXT3,
	ePVRTPF_BC3 = ePVRTPF_DXT5,

	//These are currently unsupported:
	ePVRTPF_BC4,
	ePVRTPF_BC5,
	ePVRTPF_BC6,
	ePVRTPF_BC7,
	ePVRTPF_UYVY,
	ePVRTPF_YUY2,
	ePVRTPF_BW1bpp,
	ePVRTPF_SharedExponentR9G9B9E5,
	ePVRTPF_RGBG8888,
	ePVRTPF_GRGB8888,
	ePVRTPF_ETC2_RGB,
	ePVRTPF_ETC2_RGBA,
	ePVRTPF_ETC2_RGB_A1,
	ePVRTPF_EAC_R11_Unsigned,
	ePVRTPF_EAC_R11_Signed,
	ePVRTPF_EAC_RG11_Unsigned,
	ePVRTPF_EAC_RG11_Signed,

	// these are atc fileformat
	//  [2/13/2015 gameKnife]
	ePVRTPF_ATC_RGB,
	ePVRTPF_ATC_RGBA,


	ePVRTPF_NumCompressedPFs
};

//Variable Type Names
enum EPVRTVariableType
{
	ePVRTVarTypeUnsignedByteNorm,
	ePVRTVarTypeSignedByteNorm,
	ePVRTVarTypeUnsignedByte,
	ePVRTVarTypeSignedByte,
	ePVRTVarTypeUnsignedShortNorm,
	ePVRTVarTypeSignedShortNorm,
	ePVRTVarTypeUnsignedShort,
	ePVRTVarTypeSignedShort,
	ePVRTVarTypeUnsignedIntegerNorm,
	ePVRTVarTypeSignedIntegerNorm,
	ePVRTVarTypeUnsignedInteger,
	ePVRTVarTypeSignedInteger,
	ePVRTVarTypeFloat,
	ePVRTVarTypeNumVarTypes
};

//A 64 bit pixel format ID & this will give you the high bits of a pixel format to check for a compressed format.
static const uint64 PVRTEX_PFHIGHMASK=0xffffffff00000000ull;

#pragma pack(push,4)
struct PVRTextureHeaderV3{
	uint32	u32Version;			//Version of the file header, used to identify it.
	uint32	u32Flags;			//Various format flags.
	uint64	u64PixelFormat;		//The pixel format, 8cc value storing the 4 channel identifiers and their respective sizes.
	uint32	u32ColourSpace;		//The Colour Space of the texture, currently either linear RGB or sRGB.
	uint32	u32ChannelType;		//Variable type that the channel is stored in. Supports signed/unsigned int/short/byte or float for now.
	uint32	u32Height;			//Height of the texture.
	uint32	u32Width;			//Width of the texture.
	uint32	u32Depth;			//Depth of the texture. (Z-slices)
	uint32	u32NumSurfaces;		//Number of members in a Texture Array.
	uint32	u32NumFaces;		//Number of faces in a Cube Map. Maybe be a value other than 6.
	uint32	u32MIPMapCount;		//Number of MIP Maps in the texture - NB: Includes top level.
	uint32	u32MetaDataSize;	//Size of the accompanying meta data.

	//Constructor for the header - used to make sure that the header is initialised usefully. The initial pixel format is an invalid one and must be set.
	PVRTextureHeaderV3() : 
	u32Version(PVRTEX3_IDENT),u32Flags(0),
		u64PixelFormat(ePVRTPF_NumCompressedPFs),
		u32ColourSpace(0),u32ChannelType(0),
		u32Height(1),u32Width(1),u32Depth(1),
		u32NumSurfaces(1),u32NumFaces(1),
		u32MIPMapCount(1),u32MetaDataSize(0)
	{}
};
#pragma pack(pop)
#define PVRTEX3_HEADERSIZE 52


/****************************************************************************
** Internal Functions
****************************************************************************/
//Preprocessor definitions to generate a pixelID for use when consts are needed. For example - switch statements. These should be evaluated by the compiler rather than at run time - assuming that arguments are all constant.

//Generate a 4 channel PixelID.
#define PVRTGENPIXELID4(C1Name, C2Name, C3Name, C4Name, C1Bits, C2Bits, C3Bits, C4Bits) ( ( (uint64)C1Name) + ( (uint64)C2Name<<8) + ( (uint64)C3Name<<16) + ( (uint64)C4Name<<24) + ( (uint64)C1Bits<<32) + ( (uint64)C2Bits<<40) + ( (uint64)C3Bits<<48) + ( (uint64)C4Bits<<56) )

//Generate a 1 channel PixelID.
#define PVRTGENPIXELID3(C1Name, C2Name, C3Name, C1Bits, C2Bits, C3Bits)( PVRTGENPIXELID4(C1Name, C2Name, C3Name, 0, C1Bits, C2Bits, C3Bits, 0) )

//Generate a 2 channel PixelID.
#define PVRTGENPIXELID2(C1Name, C2Name, C1Bits, C2Bits) ( PVRTGENPIXELID4(C1Name, C2Name, 0, 0, C1Bits, C2Bits, 0, 0) )

//Generate a 3 channel PixelID.
#define PVRTGENPIXELID1(C1Name, C1Bits) ( PVRTGENPIXELID4(C1Name, 0, 0, 0, C1Bits, 0, 0, 0))


/*!***********************************************************************
 @Function		PVRTGetBitsPerPixel
 @Input			u64PixelFormat			A PVR Pixel Format ID.
 @Return		const PVRTuint32	Number of bits per pixel.
 @Description	Returns the number of bits per pixel in a PVR Pixel Format 
				identifier.
*************************************************************************/
inline uint32 PVRTGetBitsPerPixel(uint64 u64PixelFormat)
{
	if((u64PixelFormat&PVRTEX_PFHIGHMASK)!=0)
	{
		uint8* PixelFormatChar=(uint8*)&u64PixelFormat;
		return PixelFormatChar[4]+PixelFormatChar[5]+PixelFormatChar[6]+PixelFormatChar[7];
	}
	else
	{
		switch (u64PixelFormat)
		{
		case ePVRTPF_BW1bpp:
			return 1;
		case ePVRTPF_PVRTCI_2bpp_RGB:
		case ePVRTPF_PVRTCI_2bpp_RGBA:
		case ePVRTPF_PVRTCII_2bpp:
			return 2;
		case ePVRTPF_PVRTCI_4bpp_RGB:
		case ePVRTPF_PVRTCI_4bpp_RGBA:
		case ePVRTPF_PVRTCII_4bpp:
		case ePVRTPF_ETC1:
		case ePVRTPF_EAC_R11_Unsigned:
		case ePVRTPF_EAC_R11_Signed:
		case ePVRTPF_ETC2_RGB:	
		case ePVRTPF_ETC2_RGB_A1:
		case ePVRTPF_DXT1:
		case ePVRTPF_BC4:
		case ePVRTPF_ATC_RGB:
			return 4;
		case ePVRTPF_DXT2:
		case ePVRTPF_DXT3:
		case ePVRTPF_DXT4:
		case ePVRTPF_DXT5:
		case ePVRTPF_BC5:
		case ePVRTPF_EAC_RG11_Unsigned:
		case ePVRTPF_EAC_RG11_Signed:
		case ePVRTPF_ETC2_RGBA:
		case ePVRTPF_ATC_RGBA:
			return 8;
		case ePVRTPF_YUY2:
		case ePVRTPF_UYVY:
			return 16;
		default:
			return 0;
		}
	}
}

/*!***********************************************************************
 @Function		PVRTGetFormatMinDims
 @Input			u64PixelFormat	A PVR Pixel Format ID.
 @Modified		minX			Returns the minimum width.
 @Modified		minY			Returns the minimum height.
 @Modified		minZ			Returns the minimum depth.
 @Description	Gets the minimum dimensions (x,y,z) for a given pixel format.
*************************************************************************/
inline void PVRTGetFormatMinDims(uint64 u64PixelFormat, uint32 &minX, uint32 &minY, uint32 &minZ)
{
	switch(u64PixelFormat)
	{
	case ePVRTPF_DXT1:
	case ePVRTPF_DXT2:
	case ePVRTPF_DXT3:
	case ePVRTPF_DXT4:
	case ePVRTPF_DXT5:
	case ePVRTPF_BC4:
	case ePVRTPF_BC5:
	case ePVRTPF_ETC1:
	case ePVRTPF_ATC_RGB:
	case ePVRTPF_ATC_RGBA:
		minX = 4;
		minY = 4;
		minZ = 1;
		break;
	case ePVRTPF_PVRTCI_4bpp_RGB:
	case ePVRTPF_PVRTCI_4bpp_RGBA:
		minX = 8;
		minY = 8;
		minZ = 1;
		break;
	case ePVRTPF_PVRTCI_2bpp_RGB:
	case ePVRTPF_PVRTCI_2bpp_RGBA:
		minX = 16;
		minY = 8;
		minZ = 1;
		break;
	case ePVRTPF_PVRTCII_4bpp:
		minX = 4;
		minY = 4;
		minZ = 1;
		break;
	case ePVRTPF_PVRTCII_2bpp:
		minX = 8;
		minY = 4;
		minZ = 1;
		break;
	case ePVRTPF_UYVY:
	case ePVRTPF_YUY2:
	case ePVRTPF_RGBG8888:
	case ePVRTPF_GRGB8888:
		minX = 2;
		minY = 1;
		minZ = 1;
		break;
	case ePVRTPF_BW1bpp:
		minX = 8;
		minY = 1;
		minZ = 1;
		break;
	default: //Non-compressed formats all return 1.
		minX = 1;
		minY = 1;
		minZ = 1;
		break;
	}
}

/*****************************************************************************
** Functions
****************************************************************************/


/*!***********************************************************************
	@Function:		PVRTGetOGLESTextureFormat
	@Input:			sTextureHeader
	@Modified:		internalformat
	@Modified:		format
	@Modified:		type
	@Description:	Gets the OpenGLES equivalent values of internal format, 
					format and type for this texture header. This will return 
					any supported OpenGLES texture values, it is up to the user 
					to decide if these are valid for their current platform.
*************************************************************************/
inline const void PVRTGetOGLES2TextureFormat(const PVRTextureHeaderV3& sTextureHeader, uint32& internalformat, uint32& format, uint32& type)
{
	uint64 PixelFormat = sTextureHeader.u64PixelFormat;
	EPVRTVariableType ChannelType = (EPVRTVariableType)sTextureHeader.u32ChannelType;
	
	//Initialisation. Any invalid formats will return 0 always.
	format = 0;
	type = 0;
	internalformat=0;

	//Get the last 32 bits of the pixel format.
	uint64 PixelFormatPartHigh = PixelFormat&PVRTEX_PFHIGHMASK;

	//Check for a compressed format (The first 8 bytes will be 0, so the whole thing will be equal to the last 32 bits).
	if (PixelFormatPartHigh==0)
	{
		//Format and type == 0 for compressed textures.
		switch (PixelFormat)
		{
		case ePVRTPF_PVRTCI_2bpp_RGB:
			{
				internalformat=GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG;
				return;
			}
		case ePVRTPF_PVRTCI_2bpp_RGBA:
			{
				internalformat=GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG;
				return;
			}
		case ePVRTPF_PVRTCI_4bpp_RGB:
			{
				internalformat=GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG;
				return;
			}
		case ePVRTPF_PVRTCI_4bpp_RGBA:
			{
				internalformat=GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG;
				return;
			}
        case ePVRTPF_PVRTCII_2bpp:
            {
  				internalformat=GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG;
				return;
            }
        case ePVRTPF_PVRTCII_4bpp:
            {
  				internalformat=GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG;
				return;
            }
#ifndef OS_IOS
		case ePVRTPF_ETC1:
			{
				internalformat=GL_ETC1_RGB8_OES;
				return;
			}
		case ePVRTPF_ATC_RGB:
			{
				internalformat=GL_ATC_RGB_AMD;
				return;
			}
		case ePVRTPF_ATC_RGBA:
			{
				internalformat=GL_ATC_RGBA_EXPLICIT_ALPHA_AMD;
				return;
			}
#endif
		default:
			return;
		}
	}
	else
	{
		switch (ChannelType)
		{
		case ePVRTVarTypeFloat:
			{
				switch (PixelFormat)
				{
					//HALF_FLOAT_OES
				case PVRTGENPIXELID4('r','g','b','a',16,16,16,16):
					{
						type=GL_HALF_FLOAT_OES;
						format = GL_RGBA;
						internalformat=GL_RGBA;
						return;
					}
				case PVRTGENPIXELID3('r','g','b',16,16,16):
					{
						type=GL_HALF_FLOAT_OES;
						format = GL_RGB;
						internalformat=GL_RGB;
						return;
					}
				case PVRTGENPIXELID2('l','a',16,16):
					{
						type=GL_HALF_FLOAT_OES;
						format = GL_LUMINANCE_ALPHA;
						internalformat=GL_LUMINANCE_ALPHA;
						return;
					}
				case PVRTGENPIXELID1('l',16):
					{
						type=GL_HALF_FLOAT_OES;
						format = GL_LUMINANCE;
						internalformat=GL_LUMINANCE;
						return;
					}
				case PVRTGENPIXELID1('a',16):
					{
						type=GL_HALF_FLOAT_OES;
						format = GL_ALPHA;
						internalformat=GL_ALPHA;
						return;
					}
					//FLOAT (OES)
				case PVRTGENPIXELID4('r','g','b','a',32,32,32,32):
					{
						type=GL_FLOAT;
						format = GL_RGBA;
						internalformat=GL_RGBA;
						return;
					}
				case PVRTGENPIXELID3('r','g','b',32,32,32):
					{
						type=GL_FLOAT;
						format = GL_RGB;
						internalformat=GL_RGB;
						return;
					}
				case PVRTGENPIXELID2('l','a',32,32):
					{
						type=GL_FLOAT;
						format = GL_LUMINANCE_ALPHA;
						internalformat=GL_LUMINANCE_ALPHA;
						return;
					}
				case PVRTGENPIXELID1('l',32):
					{
						type=GL_FLOAT;
						format = GL_LUMINANCE;
						internalformat=GL_LUMINANCE;
						return;
					}
				case PVRTGENPIXELID1('a',32):
					{
						type=GL_FLOAT;
						format = GL_ALPHA;
						internalformat=GL_ALPHA;
						return;
					}
				}
				break;
			}
		case ePVRTVarTypeUnsignedByteNorm:
			{
				type = GL_UNSIGNED_BYTE;
				switch (PixelFormat)
				{
				case PVRTGENPIXELID4('r','g','b','a',8,8,8,8):
					{
						format = internalformat = GL_RGBA;
						return;
					}
				case PVRTGENPIXELID3('r','g','b',8,8,8):
					{
						format = internalformat = GL_RGB;
						return;
					}
				case PVRTGENPIXELID2('l','a',8,8):
					{
						format = internalformat = GL_LUMINANCE_ALPHA;
						return;
					}
				case PVRTGENPIXELID1('l',8):
					{
						format = internalformat = GL_LUMINANCE;
						return;
					}
				case PVRTGENPIXELID1('a',8):
					{
						format = internalformat = GL_ALPHA;
						return;
					}
				case PVRTGENPIXELID4('b','g','r','a',8,8,8,8):
					{
						format = internalformat = GL_BGRA_EXT;
						return;
					}
				}
				break;
			}
		case ePVRTVarTypeUnsignedShortNorm:
			{
				switch (PixelFormat)
				{
				case PVRTGENPIXELID4('r','g','b','a',4,4,4,4):
					{
						type = GL_UNSIGNED_SHORT_4_4_4_4;
						format = internalformat = GL_RGBA;
						return;
					}
				case PVRTGENPIXELID4('r','g','b','a',5,5,5,1):
					{
						type = GL_UNSIGNED_SHORT_5_5_5_1;
						format = internalformat = GL_RGBA;
						return;
					}
				case PVRTGENPIXELID3('r','g','b',5,6,5):
					{
						type = GL_UNSIGNED_SHORT_5_6_5;
						format = internalformat = GL_RGB;
						return;
					}
				}
				break;
			}
		default:
			return;
		}
	}
}

inline uint32 PVRTGetTextureDataSize(PVRTextureHeaderV3 sTextureHeader, int32 iMipLevel, bool bAllSurfaces, bool bAllFaces)
{
	if ( sTextureHeader.u64PixelFormat == ePVRTPF_ATC_RGB || sTextureHeader.u64PixelFormat == ePVRTPF_ATC_RGBA )
	{
		uint32 nTotalBlocks, nBytesPerBlock, nHasAlpha;
		nTotalBlocks = ((sTextureHeader.u32Width + 3) >> 2) * ((sTextureHeader.u32Height + 3) >> 2);
		nHasAlpha = (sTextureHeader.u64PixelFormat == ePVRTPF_ATC_RGBA);
		nBytesPerBlock = (nHasAlpha) ? 16 : 8;
		uint32 size   = nTotalBlocks * nBytesPerBlock;
		size = size >> (iMipLevel * 2);

		return size;
	}

	//The smallest divisible sizes for a pixel format
	uint32 uiSmallestWidth=1;
	uint32 uiSmallestHeight=1;
	uint32 uiSmallestDepth=1;

	uint64 PixelFormatPartHigh = sTextureHeader.u64PixelFormat&PVRTEX_PFHIGHMASK;

	//If the pixel format is compressed, get the pixel format's minimum dimensions.
	if (PixelFormatPartHigh==0)
	{
		PVRTGetFormatMinDims((EPVRTPixelFormat)sTextureHeader.u64PixelFormat, uiSmallestWidth, uiSmallestHeight, uiSmallestDepth);
	}

	uint32 uiDataSize = 0;
	if (iMipLevel==-1)
	{
		for (uint8 uiCurrentMIP = 0; uiCurrentMIP<sTextureHeader.u32MIPMapCount; ++uiCurrentMIP)
		{
			//Get the dimensions of the current MIP Map level.
			uint32 uiWidth = PVRT_MAX(1,sTextureHeader.u32Width>>uiCurrentMIP);
			uint32 uiHeight = PVRT_MAX(1,sTextureHeader.u32Height>>uiCurrentMIP);
			uint32 uiDepth = PVRT_MAX(1,sTextureHeader.u32Depth>>uiCurrentMIP);

			//If pixel format is compressed, the dimensions need to be padded.
			if (PixelFormatPartHigh==0)
			{
				uiWidth=uiWidth+( (-1*uiWidth)%uiSmallestWidth);
				uiHeight=uiHeight+( (-1*uiHeight)%uiSmallestHeight);
				uiDepth=uiDepth+( (-1*uiDepth)%uiSmallestDepth);
			}

			//Add the current MIP Map's data size to the total.
			uiDataSize+=PVRTGetBitsPerPixel(sTextureHeader.u64PixelFormat)*uiWidth*uiHeight*uiDepth;
		}
	}
	else
	{
		//Get the dimensions of the specified MIP Map level.
		uint32 uiWidth = PVRT_MAX(1,sTextureHeader.u32Width>>iMipLevel);
		uint32 uiHeight = PVRT_MAX(1,sTextureHeader.u32Height>>iMipLevel);
		uint32 uiDepth = PVRT_MAX(1,sTextureHeader.u32Depth>>iMipLevel);

		//If pixel format is compressed, the dimensions need to be padded.
		if (PixelFormatPartHigh==0)
		{
			uiWidth=uiWidth+( (-1*uiWidth)%uiSmallestWidth);
			uiHeight=uiHeight+( (-1*uiHeight)%uiSmallestHeight);
			uiDepth=uiDepth+( (-1*uiDepth)%uiSmallestDepth);
		}

		//Work out the specified MIP Map's data size
		uiDataSize=PVRTGetBitsPerPixel(sTextureHeader.u64PixelFormat)*uiWidth*uiHeight*uiDepth;
	}

	//The number of faces/surfaces to register the size of.
	uint32 numfaces = ((bAllFaces)?(sTextureHeader.u32NumFaces):(1));
	uint32 numsurfs = ((bAllSurfaces)?(sTextureHeader.u32NumSurfaces):(1));

	//Multiply the data size by number of faces and surfaces specified, and return.
	return (uiDataSize/8)*numsurfs*numfaces;
}


#endif