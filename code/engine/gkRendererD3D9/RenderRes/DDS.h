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



//--------------------------------------------------------------------------------------
// dds.h
//
// This header defines constants and structures that are useful when parsing 
// DDS files.  DDS files were originally designed to use several structures
// and constants that are native to DirectDraw and are defined in ddraw.h,
// such as DDSURFACEDESC2 and DDSCAPS2.  This file defines similar 
// (compatible) constants and structures so that one can use DDS files 
// without needing to include ddraw.h.
//--------------------------------------------------------------------------------------

#ifndef _DDS_H_
#define _DDS_H_

#include <d3d9.h>

#pragma pack(push,1)

#define DDS_MAGIC 0x20534444 // "DDS "

struct DDS_PIXELFORMAT
{
    DWORD dwSize;
    DWORD dwFlags;
    DWORD dwFourCC;
    DWORD dwRGBBitCount;
    DWORD dwRBitMask;
    DWORD dwGBitMask;
    DWORD dwBBitMask;
    DWORD dwABitMask;
};

#define DDS_FOURCC      0x00000004  // DDPF_FOURCC
#define DDS_RGB         0x00000040  // DDPF_RGB
#define DDS_RGBA        0x00000041  // DDPF_RGB | DDPF_ALPHAPIXELS
#define DDS_LUMINANCE   0x00020000  // DDPF_LUMINANCE
#define DDS_ALPHA       0x00000002  // DDPF_ALPHA

const DDS_PIXELFORMAT DDSPF_DXT1 =
    { sizeof(DDS_PIXELFORMAT), DDS_FOURCC, MAKEFOURCC('D','X','T','1'), 0, 0, 0, 0, 0 };

const DDS_PIXELFORMAT DDSPF_DXT2 =
    { sizeof(DDS_PIXELFORMAT), DDS_FOURCC, MAKEFOURCC('D','X','T','2'), 0, 0, 0, 0, 0 };

const DDS_PIXELFORMAT DDSPF_DXT3 =
    { sizeof(DDS_PIXELFORMAT), DDS_FOURCC, MAKEFOURCC('D','X','T','3'), 0, 0, 0, 0, 0 };

const DDS_PIXELFORMAT DDSPF_DXT4 =
    { sizeof(DDS_PIXELFORMAT), DDS_FOURCC, MAKEFOURCC('D','X','T','4'), 0, 0, 0, 0, 0 };

const DDS_PIXELFORMAT DDSPF_DXT5 =
    { sizeof(DDS_PIXELFORMAT), DDS_FOURCC, MAKEFOURCC('D','X','T','5'), 0, 0, 0, 0, 0 };

const DDS_PIXELFORMAT DDSPF_A8R8G8B8 =
    { sizeof(DDS_PIXELFORMAT), DDS_RGBA, 0, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000 };

const DDS_PIXELFORMAT DDSPF_A1R5G5B5 =
    { sizeof(DDS_PIXELFORMAT), DDS_RGBA, 0, 16, 0x00007c00, 0x000003e0, 0x0000001f, 0x00008000 };

const DDS_PIXELFORMAT DDSPF_A4R4G4B4 =
    { sizeof(DDS_PIXELFORMAT), DDS_RGBA, 0, 16, 0x00000f00, 0x000000f0, 0x0000000f, 0x0000f000 };

const DDS_PIXELFORMAT DDSPF_R8G8B8 =
    { sizeof(DDS_PIXELFORMAT), DDS_RGB, 0, 24, 0x00ff0000, 0x0000ff00, 0x000000ff, 0x00000000 };

const DDS_PIXELFORMAT DDSPF_R5G6B5 =
    { sizeof(DDS_PIXELFORMAT), DDS_RGB, 0, 16, 0x0000f800, 0x000007e0, 0x0000001f, 0x00000000 };

// This indicates the DDS_HEADER_DXT10 extension is present (the format is in dxgiFormat)
const DDS_PIXELFORMAT DDSPF_DX10 =
    { sizeof(DDS_PIXELFORMAT), DDS_FOURCC, MAKEFOURCC('D','X','1','0'), 0, 0, 0, 0, 0 };

#define D3DFMT_3DC ((D3DFORMAT)(MAKEFOURCC('A', 'T', 'I', '2')))

#define DDS_HEADER_FLAGS_TEXTURE        0x00001007  // DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT 
#define DDS_HEADER_FLAGS_MIPMAP         0x00020000  // DDSD_MIPMAPCOUNT
#define DDS_HEADER_FLAGS_VOLUME         0x00800000  // DDSD_DEPTH
#define DDS_HEADER_FLAGS_PITCH          0x00000008  // DDSD_PITCH
#define DDS_HEADER_FLAGS_LINEARSIZE     0x00080000  // DDSD_LINEARSIZE

#define DDS_SURFACE_FLAGS_TEXTURE 0x00001000 // DDSCAPS_TEXTURE
#define DDS_SURFACE_FLAGS_MIPMAP  0x00400008 // DDSCAPS_COMPLEX | DDSCAPS_MIPMAP
#define DDS_SURFACE_FLAGS_CUBEMAP 0x00000008 // DDSCAPS_COMPLEX

#define DDS_CUBEMAP_POSITIVEX 0x00000600 // DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_POSITIVEX
#define DDS_CUBEMAP_NEGATIVEX 0x00000a00 // DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_NEGATIVEX
#define DDS_CUBEMAP_POSITIVEY 0x00001200 // DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_POSITIVEY
#define DDS_CUBEMAP_NEGATIVEY 0x00002200 // DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_NEGATIVEY
#define DDS_CUBEMAP_POSITIVEZ 0x00004200 // DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_POSITIVEZ
#define DDS_CUBEMAP_NEGATIVEZ 0x00008200 // DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_NEGATIVEZ

#define DDS_CUBEMAP_ALLFACES ( DDS_CUBEMAP_POSITIVEX | DDS_CUBEMAP_NEGATIVEX |\
                               DDS_CUBEMAP_POSITIVEY | DDS_CUBEMAP_NEGATIVEY |\
                               DDS_CUBEMAP_POSITIVEZ | DDS_CUBEMAP_NEGATIVEZ )

#define DDS_FLAGS_VOLUME 0x00200000 // DDSCAPS2_VOLUME

typedef struct
{
    DWORD dwSize;
    DWORD dwHeaderFlags;
    DWORD dwHeight;
    DWORD dwWidth;
    DWORD dwPitchOrLinearSize;
    DWORD dwDepth; // only if DDS_HEADER_FLAGS_VOLUME is set in dwHeaderFlags
    DWORD dwMipMapCount;
    DWORD dwReserved1[11];
    DDS_PIXELFORMAT ddspf;
    DWORD dwSurfaceFlags;
    DWORD dwCubemapFlags;
    DWORD dwReserved2[3];
} DDS_HEADER;

//--------------------------------------------------------------------------------------
// Return the BPP for a particular format
//--------------------------------------------------------------------------------------
static inline UINT BitsPerPixel( D3DFORMAT fmt )
{
	UINT fmtU = ( UINT )fmt;
	switch( fmtU )
	{
	case D3DFMT_A32B32G32R32F:
		return 128;

	case D3DFMT_A16B16G16R16:
	case D3DFMT_Q16W16V16U16:
	case D3DFMT_A16B16G16R16F:
	case D3DFMT_G32R32F:
		return 64;

	case D3DFMT_A8R8G8B8:
	case D3DFMT_X8R8G8B8:
	case D3DFMT_A2B10G10R10:
	case D3DFMT_A8B8G8R8:
	case D3DFMT_X8B8G8R8:
	case D3DFMT_G16R16:
	case D3DFMT_A2R10G10B10:
	case D3DFMT_Q8W8V8U8:
	case D3DFMT_V16U16:
	case D3DFMT_X8L8V8U8:
	case D3DFMT_A2W10V10U10:
	case D3DFMT_D32:
	case D3DFMT_D24S8:
	case D3DFMT_D24X8:
	case D3DFMT_D24X4S4:
	case D3DFMT_D32F_LOCKABLE:
	case D3DFMT_D24FS8:
	case D3DFMT_INDEX32:
	case D3DFMT_G16R16F:
	case D3DFMT_R32F:
		return 32;

	case D3DFMT_R8G8B8:
		return 24;

	case D3DFMT_A4R4G4B4:
	case D3DFMT_X4R4G4B4:
	case D3DFMT_R5G6B5:
	case D3DFMT_L16:
	case D3DFMT_A8L8:
	case D3DFMT_X1R5G5B5:
	case D3DFMT_A1R5G5B5:
	case D3DFMT_A8R3G3B2:
	case D3DFMT_V8U8:
	case D3DFMT_CxV8U8:
	case D3DFMT_L6V5U5:
	case D3DFMT_G8R8_G8B8:
	case D3DFMT_R8G8_B8G8:
	case D3DFMT_D16_LOCKABLE:
	case D3DFMT_D15S1:
	case D3DFMT_D16:
	case D3DFMT_INDEX16:
	case D3DFMT_R16F:
	case D3DFMT_YUY2:
		return 16;

	case D3DFMT_R3G3B2:
	case D3DFMT_A8:
	case D3DFMT_A8P8:
	case D3DFMT_P8:
	case D3DFMT_L8:
	case D3DFMT_A4L4:
		return 8;

	case D3DFMT_DXT1:
		return 4;

	case D3DFMT_DXT2:
	case D3DFMT_DXT3:
	case D3DFMT_DXT4:
	case D3DFMT_DXT5:
	case D3DFMT_3DC:
		return  8;

		// From DX docs, reference/d3d/enums/d3dformat.asp
		// (note how it says that D3DFMT_R8G8_B8G8 is "A 16-bit packed RGB format analogous to UYVY (U0Y0, V0Y1, U2Y2, and so on)")
	case D3DFMT_UYVY:
		return 16;

		// http://msdn.microsoft.com/library/default.asp?url=/library/en-us/directshow/htm/directxvideoaccelerationdxvavideosubtypes.asp
	case MAKEFOURCC( 'A', 'I', '4', '4' ):
	case MAKEFOURCC( 'I', 'A', '4', '4' ):
		return 8;

	case MAKEFOURCC( 'Y', 'V', '1', '2' ):
		return 12;

#if !defined(D3D_DISABLE_9EX)
	case D3DFMT_D32_LOCKABLE:
		return 32;

	case D3DFMT_S8_LOCKABLE:
		return 8;

	case D3DFMT_A1:
		return 1;
#endif // !D3D_DISABLE_9EX

	default:
		assert( FALSE ); // unhandled format
		return 0;
	}
}

//--------------------------------------------------------------------------------------
// Get surface information for a particular format
//--------------------------------------------------------------------------------------
static inline void GetSurfaceInfo( UINT width, UINT height, D3DFORMAT fmt, UINT* pNumBytes, UINT* pRowBytes, UINT* pNumRows )
{
	UINT numBytes = 0;
	UINT rowBytes = 0;
	UINT numRows = 0;

	// From the DXSDK docs:
	//
	//     When computing DXTn compressed sizes for non-square textures, the 
	//     following formula should be used at each mipmap level:
	//
	//         max(1, width ?4) x max(1, height ?4) x 8(DXT1) or 16(DXT2-5)
	//
	//     The pitch for DXTn formats is different from what was returned in 
	//     Microsoft DirectX 7.0. It now refers the pitch of a row of blocks. 
	//     For example, if you have a width of 16, then you will have a pitch 
	//     of four blocks (4*8 for DXT1, 4*16 for DXT2-5.)"

	if( fmt == D3DFMT_DXT1 || fmt == D3DFMT_DXT2 || fmt == D3DFMT_DXT3 || fmt == D3DFMT_DXT4 || fmt == D3DFMT_DXT5 )
	{
		int numBlocksWide = 0;
		if( width > 0 )
			numBlocksWide = max( 1, width / 4 );
		int numBlocksHigh = 0;
		if( height > 0 )
			numBlocksHigh = max( 1, height / 4 );
		int numBytesPerBlock = ( fmt == D3DFMT_DXT1 ? 8 : 16 );
		rowBytes = numBlocksWide * numBytesPerBlock;
		numRows = numBlocksHigh;
	}
	else
	{
		UINT bpp = BitsPerPixel( fmt );
		rowBytes = ( width * bpp + 7 ) / 8; // round up to nearest byte
		numRows = height;
	}
	numBytes = rowBytes * numRows;
	if( pNumBytes != NULL )
		*pNumBytes = numBytes;
	if( pRowBytes != NULL )
		*pRowBytes = rowBytes;
	if( pNumRows != NULL )
		*pNumRows = numRows;
}

//--------------------------------------------------------------------------------------
#define ISBITMASK( r,g,b,a ) ( ddpf.dwRBitMask == r && ddpf.dwGBitMask == g && ddpf.dwBBitMask == b && ddpf.dwABitMask == a )



//--------------------------------------------------------------------------------------
static inline D3DFORMAT GetD3D9Format( const DDS_PIXELFORMAT& ddpf )
{
	if( ddpf.dwFlags & DDS_RGB )
	{
		switch (ddpf.dwRGBBitCount)
		{
		case 32:
			if( ISBITMASK(0x00ff0000,0x0000ff00,0x000000ff,0xff000000) )
				return D3DFMT_A8R8G8B8;
			if( ISBITMASK(0x00ff0000,0x0000ff00,0x000000ff,0x00000000) )
				return D3DFMT_X8R8G8B8;
			if( ISBITMASK(0x000000ff,0x0000ff00,0x00ff0000,0xff000000) )
				return D3DFMT_A8B8G8R8;
			if( ISBITMASK(0x000000ff,0x0000ff00,0x00ff0000,0x00000000) )
				return D3DFMT_X8B8G8R8;

			// Note that many common DDS reader/writers swap the
			// the RED/BLUE masks for 10:10:10:2 formats. We assumme
			// below that the 'correct' header mask is being used
			if( ISBITMASK(0x3ff00000,0x000ffc00,0x000003ff,0xc0000000) )
				return D3DFMT_A2R10G10B10;
			if( ISBITMASK(0x000003ff,0x000ffc00,0x3ff00000,0xc0000000) )
				return D3DFMT_A2B10G10R10;

			if( ISBITMASK(0x0000ffff,0xffff0000,0x00000000,0x00000000) )
				return D3DFMT_G16R16;
			if( ISBITMASK(0xffffffff,0x00000000,0x00000000,0x00000000) )
				return D3DFMT_R32F; // D3DX writes this out as a FourCC of 114
			break;

		case 24:
			if( ISBITMASK(0x00ff0000,0x0000ff00,0x000000ff,0x00000000) )
				return D3DFMT_R8G8B8;
			break;

		case 16:
			if( ISBITMASK(0x0000f800,0x000007e0,0x0000001f,0x00000000) )
				return D3DFMT_R5G6B5;
			if( ISBITMASK(0x00007c00,0x000003e0,0x0000001f,0x00008000) )
				return D3DFMT_A1R5G5B5;
			if( ISBITMASK(0x00007c00,0x000003e0,0x0000001f,0x00000000) )
				return D3DFMT_X1R5G5B5;
			if( ISBITMASK(0x00000f00,0x000000f0,0x0000000f,0x0000f000) )
				return D3DFMT_A4R4G4B4;
			if( ISBITMASK(0x00000f00,0x000000f0,0x0000000f,0x00000000) )
				return D3DFMT_X4R4G4B4;
			if( ISBITMASK(0x000000e0,0x0000001c,0x00000003,0x0000ff00) )
				return D3DFMT_A8R3G3B2;
			break;
		}
	}
	else if( ddpf.dwFlags & DDS_LUMINANCE )
	{
		if( 8 == ddpf.dwRGBBitCount )
		{
			if( ISBITMASK(0x0000000f,0x00000000,0x00000000,0x000000f0) )
				return D3DFMT_A4L4;
			if( ISBITMASK(0x000000ff,0x00000000,0x00000000,0x00000000) )
				return D3DFMT_L8;
		}

		if( 16 == ddpf.dwRGBBitCount )
		{
			if( ISBITMASK(0x0000ffff,0x00000000,0x00000000,0x00000000) )
				return D3DFMT_L16;
			if( ISBITMASK(0x000000ff,0x00000000,0x00000000,0x0000ff00) )
				return D3DFMT_A8L8;
		}
	}
	else if( ddpf.dwFlags & DDS_ALPHA )
	{
		if( 8 == ddpf.dwRGBBitCount )
		{
			return D3DFMT_A8;
		}
	}
	else if( ddpf.dwFlags & DDS_FOURCC )
	{
		if( MAKEFOURCC( 'D', 'X', 'T', '1' ) == ddpf.dwFourCC )
			return D3DFMT_DXT1;
		if( MAKEFOURCC( 'D', 'X', 'T', '2' ) == ddpf.dwFourCC )
			return D3DFMT_DXT2;
		if( MAKEFOURCC( 'D', 'X', 'T', '3' ) == ddpf.dwFourCC )
			return D3DFMT_DXT3;
		if( MAKEFOURCC( 'D', 'X', 'T', '4' ) == ddpf.dwFourCC )
			return D3DFMT_DXT4;
		if( MAKEFOURCC( 'D', 'X', 'T', '5' ) == ddpf.dwFourCC )
			return D3DFMT_DXT5;

		if( MAKEFOURCC( 'R', 'G', 'B', 'G' ) == ddpf.dwFourCC )
			return D3DFMT_R8G8_B8G8;
		if( MAKEFOURCC( 'G', 'R', 'G', 'B' ) == ddpf.dwFourCC )
			return D3DFMT_G8R8_G8B8;

		if( MAKEFOURCC( 'U', 'Y', 'V', 'Y' ) == ddpf.dwFourCC )
			return D3DFMT_UYVY;
		if( MAKEFOURCC( 'Y', 'U', 'Y', '2' ) == ddpf.dwFourCC )
			return D3DFMT_YUY2;

		if( MAKEFOURCC( 'A', 'T', 'I', '2' ) == ddpf.dwFourCC )
			return D3DFMT_3DC;
		// Check for D3DFORMAT enums being set here
		switch( ddpf.dwFourCC )
		{
		case D3DFMT_A16B16G16R16:
		case D3DFMT_Q16W16V16U16:
		case D3DFMT_R16F:
		case D3DFMT_G16R16F:
		case D3DFMT_A16B16G16R16F:
		case D3DFMT_R32F:
		case D3DFMT_G32R32F:
		case D3DFMT_A32B32G32R32F:
		case D3DFMT_CxV8U8:
			return (D3DFORMAT)ddpf.dwFourCC;
		}
	}

	return D3DFMT_UNKNOWN;
}

#pragma pack(pop)

#endif // _DDS_H
