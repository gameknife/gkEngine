#include "txtCompiler.h"
#include "strnormalize.h"

bool isutf8(uint8 *str, int size)
{
	int i = 0;

	while(i < size)
	{
		int step = 0;
		if((str[i] & 0x80) == 0x00)
		{
			step = 1;
		}
		else if((str[i] & 0xe0) == 0xc0)
		{
			if(i + 1 >= size) return false;
			if((str[i + 1] & 0xc0) != 0x80) return false;

			step = 2;
		}
		else if((str[i] & 0xf0) == 0xe0)
		{
			if(i + 2 >= size) return false;
			if((str[i + 1] & 0xc0) != 0x80) return false;
			if((str[i + 2] & 0xc0) != 0x80) return false;

			step = 3;
		}
		else
		{
			return false;
		}

		i += step;
	}

	if(i == size) return true;

	return false;
}

bool utf8Compiler::encode(const char* filename, const char* cfg)
{
	utf8Str = NULL;

	std::string m_filename(filename);

	if ( m_filename.empty() )
	{
		return false;
	}

	// open file
	FILE* tmpFile = fopen( m_filename.c_str(), "rb" );

	uint8* strMeshData = NULL;

	if (tmpFile)
	{
		fseek(tmpFile, 0, SEEK_END);
		uint32 size = ftell(tmpFile);
		fseek(tmpFile, 0, SEEK_SET);

		strMeshData = new uint8[size];
		fread( strMeshData, 1, size, tmpFile );

		fclose(tmpFile);

		//////////////////////////////////////////////////////////////////////////
		// check BOM utf-8

		if ( strMeshData[0] == 0xef && strMeshData[1] == 0xbb && strMeshData[2] == 0xbf )
		{
			printf("already utf8, skiped.");
			return false;
		}

		//////////////////////////////////////////////////////////////////////////
		// check utf-8 NOBOM
		if( isutf8( strMeshData, size ) )
		{
			// just add BOM, not transcode
			printf("already utf8 NOBOM, add BOM.");
			utf8len = size;
			utf8Str = new char[utf8len];

			memcpy( utf8Str, strMeshData, utf8len );

		}
		else
		{
			//////////////////////////////////////////////////////////////////////////
			// we just convert gbk to utf8, other codepage cannot
			utf8len = size + size;
			utf8Str = new char[utf8len];

			gbk_to_utf8((char*)strMeshData, size, &utf8Str, &utf8len);
		}


		delete[] strMeshData;
	}

	

	

	return true;
}

bool utf8Compiler::writeFile( const char* filename, const char* cfg )
{
	if (utf8Str)
	{
		// 打开文件写入
		FILE* file = fopen( filename, "wb" );
		if ( !file)
		{
			printf("FATAL ERROR! 创建文件失败\n");
			return -1;
		}

		fseek( file, 0, SEEK_SET );
		// write BOM
		uint8 BOM[3];
		BOM[0] = 0xef;
		BOM[1] = 0xbb;
		BOM[2] = 0xbf;
		fwrite( BOM, 1, 3, file );

		// write MEM
		fwrite( utf8Str, 1, utf8len, file );

		fclose( file );

		delete[] utf8Str;
	}

	return true;
}

bool utf8Compiler::supportInput( const char* ext )
{
	if (!stricmp(ext, ".txt"))
	{
		return true;
	}
	if (!stricmp(ext, ".ini"))
	{
		return true;
	}
	if (!stricmp(ext, ".cpp"))
	{
		return true;
	}
	if (!stricmp(ext, ".h"))
	{
		return true;
	}
	if (!stricmp(ext, ".c"))
	{
		return true;
	}
	return false;
}

bool utf8Compiler::supportOutput( const char* ext )
{
	if (!stricmp(ext, ".txt"))
	{
		return true;
	}
	if (!stricmp(ext, ".ini"))
	{
		return true;
	}
	if (!stricmp(ext, ".cpp"))
	{
		return true;
	}
	if (!stricmp(ext, ".h"))
	{
		return true;
	}
	if (!stricmp(ext, ".c"))
	{
		return true;
	}
	return false;
}

const char* utf8Compiler::getName()
{
	return "utf8Compiler";
}

//////////////////////////////////////////////////////////////////////////

bool copyrightCompiler::encode(const char* filename, const char* cfg)
{
	utf8Str = NULL;

	std::string m_filename(filename);

	if ( m_filename.empty() )
	{
		return false;
	}

	// open file
	FILE* tmpFile = fopen( m_filename.c_str(), "rb" );

	uint8* strMeshData = NULL;

	if (tmpFile)
	{
		fseek(tmpFile, 0, SEEK_END);
		uint32 size = ftell(tmpFile);
		fseek(tmpFile, 0, SEEK_SET);

		strMeshData = new uint8[size];
		fread( strMeshData, 1, size, tmpFile );

		fclose(tmpFile);

		//////////////////////////////////////////////////////////////////////////
		// check BOM utf-8

		if ( strMeshData[0] == 0xef && strMeshData[1] == 0xbb && strMeshData[2] == 0xbf )
		{
			utf8len = size - 3;
			utf8Str = new char[utf8len];
			memcpy( utf8Str, strMeshData + 3, utf8len );
		}
		else
		{
			utf8len = size;
			utf8Str = new char[utf8len];
		}

		delete[] strMeshData;
	}
	return true;
}

const char* copyrightstr = "//////////////////////////////////////////////////////////////////////////\r\n"
"/*\r\n"
	"Copyright (c) 2011-2015 Kaiming Yi\r\n\
	\r\n\
	Permission is hereby granted, free of charge, to any person obtaining a copy\r\n\
	of this software and associated documentation files (the \"Software\"), to deal\r\n\
	in the Software without restriction, including without limitation the rights\r\n\
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell\r\n\
	copies of the Software, and to permit persons to whom the Software is\r\n\
	furnished to do so, subject to the following conditions:\r\n\
	\r\n\
	The above copyright notice and this permission notice shall be included in\r\n\
	all copies or substantial portions of the Software.\r\n\
	\r\n\
	THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR\r\n\
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,\r\n\
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE\r\n\
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER\r\n\
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,\r\n\
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN\r\n\
	THE SOFTWARE.\r\n\
	\r\n"
"*/\r\n"
"//////////////////////////////////////////////////////////////////////////\r\n\r\n\r\n\r\n";

bool copyrightCompiler::writeFile( const char* filename, const char* cfg )
{
	if (utf8Str)
	{
		// 打开文件写入
		FILE* file = fopen( filename, "wb" );
		if ( !file)
		{
			printf("FATAL ERROR! 创建文件失败\n");
			return -1;
		}

		fseek( file, 0, SEEK_SET );
		// write BOM
		uint8 BOM[3];
		BOM[0] = 0xef;
		BOM[1] = 0xbb;
		BOM[2] = 0xbf;
		fwrite( BOM, 1, 3, file );

		// write MEM
		fwrite( copyrightstr, 1, strlen(copyrightstr), file );

		fwrite( utf8Str, 1, utf8len, file );

		fclose( file );

		delete[] utf8Str;
	}

	return true;
}

bool copyrightCompiler::supportInput( const char* ext )
{
	if (!stricmp(ext, ".txt"))
	{
		return true;
	}
	if (!stricmp(ext, ".ini"))
	{
		return true;
	}
	if (!stricmp(ext, ".cpp"))
	{
		return true;
	}
	if (!stricmp(ext, ".h"))
	{
		return true;
	}
	if (!stricmp(ext, ".c"))
	{
		return true;
	}
	return false;
}

bool copyrightCompiler::supportOutput( const char* ext )
{
	if (!stricmp(ext, ".txt"))
	{
		return true;
	}
	if (!stricmp(ext, ".ini"))
	{
		return true;
	}
	if (!stricmp(ext, ".cpp"))
	{
		return true;
	}
	if (!stricmp(ext, ".h"))
	{
		return true;
	}
	if (!stricmp(ext, ".c"))
	{
		return true;
	}
	return false;
}

const char* copyrightCompiler::getName()
{
	return "copyrightCompiler";
}

//copyrightCompiler g_copyrightCompiler;