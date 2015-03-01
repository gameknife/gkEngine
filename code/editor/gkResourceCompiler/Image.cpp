/*
		说说异或运算^和他的一个常用作用。
		异或的运算方法是一个二进制运算：
		1^1=0
		0^0=0
		1^0=1
		0^1=1

		两者相等为0,不等为1.

		这样我们发现交换两个整数的值时可以不用第三个参数。
		如a=11,b=9.以下是二进制
		a=a^b=1011^1001=0010;
		b=b^a=1001^0010=1011;
		a=a^b=0010^1011=1001;
		这样一来a=9,b=13了。

		举一个运用， 按一个按钮交换两个mc的位置可以这样。

		mybt.onPress=function()
		{
			mc1._x=mc1._x^mc2._x;
			mc2._x=mc2._x^mc1._x;
			mc1._x=mc1._x^mc2._x;

			mc1._y=mc1._y^mc2._y; 
			mc2._y=mc2._y^mc1._y;
			mc1._y=mc1._y^mc2._y;
		}

		这样就可以不通过监时变量来传递了。

		最后要声明：只能用于整数。

		vertex[0].position = vec2( -width/2, height/2 );
		vertex[1].position = vec2( width/2, height/2 );
		vertex[2].position = vec2( width/2, -height/2 );
		vertex[3].position = vec2( -width/2, -height/2 );
*/
//#include "StdAfx.h"
//#include "global.h"
#include "Image.h"

Image::Image(void)
{
}

Image::~Image(void)
{
}

bool Image::loadTGA( TexImage* texture, LPCSTR filename )
{
	if ( filename == NULL )
		return FALSE;
		
	Header uTGAcompare = { 0,0,2,0,0,0,0,0};		//2为非压缩RGB格式		3  -  未压缩的，黑白图像
	Header cTGAcompare = { 0,0,10,0,0,0,0,0};		//10为压缩RGB格式

	TGAHeader header;
	FILE* file = fopen( filename, "rb" );
	if ( !file ){
		gkLogWarning("Openf file %s failed!\n", filename );
		return FALSE;
	}
	
	if ( fread( &header, 1, sizeof(TGAHeader), file ) != sizeof( TGAHeader ) ){		//读取TGA整个头结构体
		if ( file )
			fclose( file );
		gkLogWarning("Read data failed\n");
		return FALSE;
	}
	
	texture->width = header.width;
	texture->height = header.height;
	texture->bpp = header.bpp;

	if ( header.bpp == 32 )
		texture->imageType = GL_RGBA;
	else if ( header.bpp = 24 )
		texture->imageType = GL_RGB;
	else{
		gkLogWarning("Image type error!\n");
		return FALSE;
	}
		

	if ( memcmp( &uTGAcompare, &header.head, sizeof(header.head) )== 0 ){		//未压缩TGA
		texture->bCompressed = FALSE;
		if ( !loadUncompressedTGA( texture, file ) ){
			gkLogWarning("Load uncompressed TGA failed!\n");
			return FALSE;
		}
	}else if ( memcmp( &cTGAcompare, &header.head ,sizeof(header.head) ) == 0 ){	//压缩TGA
		texture->bCompressed = TRUE;
		if ( !loadCompressedTGA( texture, file ) ){
			gkLogWarning("Load compressed TGA failed!\n");
			return FALSE;
		}
	}else{
		gkLogWarning("Error TGA type!\n");
		return FALSE;
	}

	return TRUE;
}

bool Image::loadUncompressedTGA( TexImage* texture, FILE* file )
{
	//ASSERT( file != NULL && texture!=NULL );

	uint32 bytePerPixel = texture->bpp/8;
	uint32 imgSize = texture->width*texture->height*bytePerPixel;				//图像总字节数
	texture->imageData = new uint8[ imgSize ];

	if ( fread( texture->imageData, 1, imgSize, file ) != imgSize  )
	{
		gkLogWarning("Read texture imagedata failed!\n");
		return FALSE;
	}

	//TGA采用了逆OpenGL的格式,要将BGR转换成为RGB
	// Go through all of the pixels and swap the B and R values since TGA
	// files are stored as BGR instead of RGB (or use GL_BGR_EXT verses GL_RGB)
	for( int i = 0; i < (int)imgSize; i+=bytePerPixel ){
		/*GLushort temp = texture->imageData[i];
		texture->imageData[i] = texture->imageData[i+2];
		texture->imageData[i+2] = temp;*/
		texture->imageData[i] ^= texture->imageData[i+2] ^= texture->imageData[i] ^= texture->imageData[ i+2 ];		//位操作提高速度,更换B,R分量
	}

	::fclose( file );
	return TRUE;
}

bool Image::loadCompressedTGA( TexImage* texture, FILE* file )
{
	//ASSERT( file != NULL && texture!=NULL );
	
	uint32 bytePerPixel = texture->bpp/8;
	uint32 imgSize = texture->width*texture->height*bytePerPixel;
	texture->imageData = new uint8[ imgSize ];

	uint32 pixelcount = texture->width * texture->height;
	uint32 currentPixel = 0;		//当前正在读取的像素
	uint32 currentByte = 0;			//当前正在向图像中写入的像素
	uint8 *colorbuffer = (uint8 *)malloc( bytePerPixel );	// 一个像素的存储空间s

	do
	{
		uint8 chunkHeader  = 0;		//存储ID块值的变量
		if ( !fread( &chunkHeader,1, sizeof( uint8 ), file ) ){
			return FALSE;
		}
		if ( chunkHeader < 128 )			//RAW块
		{
			chunkHeader++;				// 变量值加1以获取RAW像素的总数

			for( int i = 0; i < chunkHeader; i++ ){
				if ( fread( colorbuffer, 1,sizeof( bytePerPixel ), file ) != sizeof( bytePerPixel ) ){
					gkLogWarning("Read pixel failed!\n");
					return FALSE;
				}
				texture->imageData[currentByte] = colorbuffer[ 2 ];
				texture->imageData[currentByte+1] = colorbuffer[1];
				texture->imageData[currentByte+2] = colorbuffer[0];
				if ( bytePerPixel == 4 )
					texture->imageData[ currentByte+3] = colorbuffer[3];

				currentPixel++;
				currentByte += bytePerPixel;
			}
		}
		 //下一段处理描述RLE段的“块”头。首先我们将chunkheader减去127来得到获取下一个颜色重复的次数。 
		else		
		{
			chunkHeader -= 127;			//减去127获得ID bit 的rid	开始循环拷贝我们多次读到内存中的像素，这由RLE头中的值规定。

			if ( fread( colorbuffer,1, sizeof( bytePerPixel ), file ) != sizeof( bytePerPixel ) ){
				gkLogWarning("Read pixel failed!\n");
				return FALSE;
			}
			
			for( int i = 0; i < chunkHeader; i++ ){
				texture->imageData[ currentByte ] = colorbuffer[ 2 ];				// 拷贝“R”字节
				texture->imageData[ currentByte +1 ] = colorbuffer[ 1 ];			// 拷贝“G”字节
				texture->imageData[ currentByte + 2 ] = colorbuffer[ 0 ];			// 拷贝“B”字节
				if ( bytePerPixel == 4 )
					texture->imageData[ currentByte+3 ] = colorbuffer[ 3 ];			// 拷贝“A”字节

				currentPixel++;
				currentByte += bytePerPixel;
			}
		}
	}while( currentPixel < pixelcount );

	free( colorbuffer );
	::fclose( file );
	return TRUE;
}

bool Image::release( TexImage* texture )
{
	if ( !texture )
		return FALSE;
	
	if ( texture->imageData )
		delete[] texture->imageData;
	//glDeleteTextures( 1, &texture->texID );
	return TRUE;
}
