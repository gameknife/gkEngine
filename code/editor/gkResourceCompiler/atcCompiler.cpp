#include "atcCompiler.h"
#include "Image.h"

#include "../../ThirdParty/atctexlib/TextureConverter.h"

#ifdef _M_X64
#	ifdef _DEBUG
#	pragma comment(lib,"..\\..\\ThirdParty\\atctexlib\\lib\\x64\\vs2010\\TextureConverterD.lib")
#	else
#	pragma comment(lib,"..\\..\\ThirdParty\\atctexlib\\lib\\x64\\vs2010\\TextureConverter.lib")
#	endif
#else
#	ifdef _DEBUG
#	pragma comment(lib,"..\\..\\ThirdParty\\atctexlib\\lib\\win32\\vs2010\\TextureConverterD.lib")
#	else
#	pragma comment(lib,"..\\..\\ThirdParty\\atctexlib\\lib\\win32\\vs2010\\TextureConverter.lib")
#	endif
#endif

const uint32 PVRTEX3_IDENT			= 0x03525650;	// 'P''V''R'3

//A 64 bit pixel format ID & this will give you the high bits of a pixel format to check for a compressed format.
static const uint64 PVRTEX_PFHIGHMASK=0xffffffff00000000ull;

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

#define PVRT_MIN(a,b)            (((a) < (b)) ? (a) : (b))
#define PVRT_MAX(a,b)            (((a) > (b)) ? (a) : (b))

#define ATC_SIGNATURE        0xCCC40002
#define GL_ATC_RGB_AMD       0x8C92
#define GL_ATC_RGBA_AMD      0x8C93
#define ATC_RGB              0x00000001
#define ATC_RGBA             0x00000002
#define ATC_TILED            0X00000004

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

// atc lib help function
TQonvertImage* CreateEmptyTexture()
{
	TQonvertImage* pTexture;

	pTexture = (TQonvertImage*)malloc(sizeof(TQonvertImage));
	memset(pTexture, 0, sizeof(TQonvertImage));
	pTexture->pFormatFlags = (TFormatFlags*)malloc(sizeof(TFormatFlags));
	memset(pTexture->pFormatFlags, 0, sizeof(TFormatFlags));

	return pTexture;
}
void FreeTexture(TQonvertImage* pTexture)
{
	free(pTexture->pFormatFlags);
	//free(pTexture->pData);
	free(pTexture);
}

pvrCompiler::pvrCompiler(void)
{
	img= new Image();
	imgtexture = new _tTexImage();
	g_compilers.push_back( this );
}


pvrCompiler::~pvrCompiler(void)
{
	img->release(imgtexture);
	delete img;
	delete imgtexture;
}

bool pvrCompiler::encode(const char* filename, const char* cfg)
{
	bool result = true;

	if( !stricmp( cfg, "atc" ) )
	{
		// not pvr
		result = img->loadTGA( imgtexture, filename );
		extern_tool = false;
	}
	else
	{
        
#ifdef OS_WIN32
		extern_tool = true;
		// call pvr coder
		TCHAR szExePath[MAX_PATH];
		GetModuleFileName( NULL, szExePath, MAX_PATH );

		char* split = strrchr(szExePath, '\\');
		if( split )
		{
			strcpy( split, "\\pvrtextool.exe" );

			TCHAR cmdline[MAX_PATH];
			sprintf( cmdline, "%s -m -f%s -yflip0 -silent -i %s", szExePath, cfg, filename );

			system( cmdline );

		}
#else
        
#endif

		//%GKENGINE_HOME%\tools\pvrtextool -m -fOGL8888 -yflip0 -silent -i %%~fA
	}
	
	
	return result;
}

bool pvrCompiler::writeFile(const char* filename, const char* cfg)
{
	if( extern_tool )
	{
		return true;
	}

	// read from img, write atc
	struct ATC_HEADER 
	{
		uint32	signature;
		uint32	width;
		uint32	height;
		uint32	flags;
		uint32	dataOffset;  // From start of header/file
		uint32  mipmaplevel;
	} header;

	memset(&header, 0, sizeof(header));
	header.signature = ATC_SIGNATURE;

	TQonvertImage*  pInputTexture        = CreateEmptyTexture();

	// mipmap saving go
	header.width = imgtexture->width;
	header.height = imgtexture->height;
	header.dataOffset = sizeof(ATC_HEADER);
	if( imgtexture->bpp > 24 )
	{
		header.flags |= ATC_RGBA;
	}
	else
	{
		header.flags |= ATC_RGB;
	}
	
	FILE* fp = fopen( filename, "wb" );
	if (fp)
	{
		// try to use mipmap
		void* rawdata = NULL;
		rawdata = imgtexture->imageData;
		header.mipmaplevel = 1;//image->gen_mipmaps( rawdata );
		BYTE* startBytes = (BYTE*)rawdata;

		// write header first
		//fwrite( &header, sizeof(ATC_HEADER), 1, fp );

		// write pvr headerinstead [2/13/2015 gameKnife]

		PVRTextureHeaderV3 headerNew;
		headerNew.u32Width = header.width;
		headerNew.u32Height = header.height;
		headerNew.u64PixelFormat = ePVRTPF_ATC_RGBA;
		headerNew.u32MIPMapCount = 1;

		fwrite( &headerNew, sizeof(PVRTextureHeaderV3), 1, fp );

		uint32 u32MIPWidth = header.width;
		uint32 u32MIPHeight = header.height;
		for (uint32 i = 0; i < header.mipmaplevel; ++i)
		{
			pInputTexture->nWidth       = u32MIPWidth;
			pInputTexture->nHeight      = u32MIPHeight;
			if( imgtexture->bpp > 24 )
			{
				pInputTexture->nFormat = Q_FORMAT_BGRA_8888;
			}
			else
			{
				pInputTexture->nFormat = Q_FORMAT_RGB_888;
			}

			pInputTexture->nDataSize    = u32MIPWidth * u32MIPHeight * imgtexture->bpp / 8;
			pInputTexture->pData		= startBytes;
			pInputTexture->pFormatFlags->nFlipY = 1;

			TQonvertImage*  pOutputTexture       = CreateEmptyTexture();
			pOutputTexture->nWidth = u32MIPWidth;
			pOutputTexture->nHeight = u32MIPHeight;
			pOutputTexture->nFormat = Q_FORMAT_ATITC_RGBA;

			// convert
			pOutputTexture->nDataSize = getATCMipSize( u32MIPWidth, u32MIPHeight, ATC_RGBA, 0 );
			pOutputTexture->pData = (uint8*)malloc(pOutputTexture->nDataSize);
			startBytes += pInputTexture->nDataSize;



			uint32 nRet = Qonvert( pInputTexture, pOutputTexture );
			if (nRet != Q_SUCCESS || pOutputTexture->nDataSize == 0)
			{
				gkLogError( _T("encode atc file [%s] failed."), filename );
			}

			fwrite( pOutputTexture->pData, pOutputTexture->nDataSize, 1, fp );

			free( pOutputTexture->pData );
			FreeTexture(  pOutputTexture ); 

			u32MIPWidth=PVRT_MAX(4,u32MIPWidth>>1);
			u32MIPHeight=PVRT_MAX(4,u32MIPHeight>>1);
		}


		//free(rawdata);

		fclose(fp);
	}

	return true;
}

bool pvrCompiler::supportInput(const char* ext)
{
	//throw std::logic_error("The method or operation is not implemented.");
	if (!stricmp(ext, ".tga"))
	{
		return true;
	}
	return false;
}

bool pvrCompiler::supportOutput(const char* ext)
{
	//throw std::logic_error("The method or operation is not implemented.");
	if (!stricmp(ext, ".atc"))
	{
		return true;
	}
	if (!stricmp(ext, ".pvr"))
	{
		return true;
	}
	return false;
}

const char* pvrCompiler::getName()
{
	return "pvrCompiler";
}

