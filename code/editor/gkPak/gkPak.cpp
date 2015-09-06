//////////////////////////////////////////////////////////////////////////
//
// Name:   	gkPak.cpp
// Desc:	
// 	
// 
// Author:  Kaiming
// Date:	2013/3/4	
// 
//////////////////////////////////////////////////////////////////////////

#include "gkPlatform.h"
#include "IResFile.h"
#include <iostream>

#include "crc32.h"
#include "lzma/LzmaLib.h"

#include <mach-o/dyld.h>
#include <limits.h>

#import <Foundation/Foundation.h>

std::string macBundlePath(void)
{
    std::string ret = "";
    char buf [PATH_MAX];
    uint32_t bufsize = PATH_MAX;
    if(!_NSGetExecutablePath(buf, &bufsize))
    {
        ret = buf;
        ret = GetParent(ret);
        ret = GetParent(ret);
    }
    
    return ret;
}


bool mac_gothrough(const TCHAR* root_path, fnGoThrough func )
{
    //fileList便是包含有该文件夹下所有文件的文件名及文件夹名的数组
    NSString* docsDir = [[NSString alloc] initWithBytes:root_path length:strlen(root_path) encoding:NSUTF8StringEncoding];
    NSFileManager *localFileManager = [NSFileManager defaultManager];
    NSDirectoryEnumerator *dirEnum = [localFileManager enumeratorAtPath:docsDir];
    NSString *file = nil;
    NSData *fileContents = [NSData data];
    while ((file = [dirEnum nextObject]))
    {
        NSString *fileNamePath = [docsDir stringByAppendingPathComponent:file];
        fileContents = [NSData dataWithContentsOfFile:fileNamePath]; // This will store file contents in form of bytes
        
        BOOL isdir = NO;
        [localFileManager fileExistsAtPath:fileNamePath isDirectory:&isdir];
        
        if(!isdir)
        {
            func( [fileNamePath UTF8String] );
        }
        
    }
    
    return true;
}



struct gPakStatus
{
	FILE* pakFile;
	uint32 fileAddress;

	gPakStatus()
	{
		pakFile = NULL;
		fileAddress = 0;
	}
};
gPakStatus g_status;

Crc32Gen crc32gen;

gkPak_Header g_header;
typedef std::map<uint32, gkPak_FileRecord*> FileRecords;
FileRecords g_fileRecords;

std::vector<gkPak_FileRecord> g_fileRecording;

char g_inputPath[MAX_PATH];
char g_outputPath[MAX_PATH];
int g_compressLevel = 5;
int g_platform = 0;

char* g_filenameBuffer;

bool FilterCompressFile( const TCHAR* file )
{
	const char* ext = strrchr( file, '.' );
	if (ext && g_platform == 0)
	{
		if ( !stricmp(ext, ".gmf") ||
			!stricmp(ext, ".dds") ||
			!stricmp(ext, ".raw") ||
			!stricmp(ext, ".gks") ||
			!stricmp(ext, ".fx") ||
			!stricmp(ext, ".h") ||
			!stricmp(ext, ".hkx") ||
			!stricmp(ext, ".hkt") ||
			!stricmp(ext, ".chr") ||
			!stricmp(ext, ".mtl") ||
			!stricmp(ext, ".tod") ||
			!stricmp(ext, ".ttf") ||
			!stricmp(ext, ".cfg")			
			)
		{
			return true;
		}
	}
	else if (ext && g_platform == 1)
	{
		if ( !stricmp(ext, ".gmf") ||
			!stricmp(ext, ".pvr") ||
			!stricmp(ext, ".raw") ||
			!stricmp(ext, ".gks") ||
			!stricmp(ext, ".gfx") ||
			!stricmp(ext, ".ffx") ||
			!stricmp(ext, ".vfx") ||
			!stricmp(ext, ".h") ||
			!stricmp(ext, ".hkx") ||
			!stricmp(ext, ".hkt") ||
			!stricmp(ext, ".chr") ||
			!stricmp(ext, ".mtl") ||
			!stricmp(ext, ".tod") ||
			!stricmp(ext, ".ttf") ||
			!stricmp(ext, ".cfg")			
			)
		{
			return true;
		}
	}


	return false;
}

bool FilterEngineFile( const TCHAR* file)
{
	const char* ext = strrchr( file, '.' );
	if (ext && g_platform == 0)
	{
		if ( !stricmp(ext, ".gmf") ||
			!stricmp(ext, ".dds") ||
			!stricmp(ext, ".raw") ||
			!stricmp(ext, ".mtl") ||
			!stricmp(ext, ".gks") ||
			!stricmp(ext, ".fx") ||
			!stricmp(ext, ".h") ||
			!stricmp(ext, ".hkx") ||
			!stricmp(ext, ".hkt") ||
			!stricmp(ext, ".chr") ||
			!stricmp(ext, ".o") ||
			!stricmp(ext, ".tod") ||
			!stricmp(ext, ".ttf") ||
			!stricmp(ext, ".gfx") ||
			!stricmp(ext, ".cfg")			
			)
		{
			return true;
		}
	}
	else if (ext && g_platform == 1)
	{
		if ( !stricmp(ext, ".gmf") ||
			!stricmp(ext, ".pvr") ||
			!stricmp(ext, ".raw") ||
			!stricmp(ext, ".mtl") ||
			!stricmp(ext, ".gks") ||
			!stricmp(ext, ".gfx") ||
			!stricmp(ext, ".ffx") ||
			!stricmp(ext, ".vfx") ||
			!stricmp(ext, ".h") ||
			!stricmp(ext, ".hkx") ||
			!stricmp(ext, ".hkt") ||
			!stricmp(ext, ".chr") ||
			//!stricmp(ext, ".o") ||
			!stricmp(ext, ".tod") ||
			!stricmp(ext, ".ttf") ||
			!stricmp(ext, ".cfg")			
			)
		{
			return true;
		}
	}
	else if (ext && g_platform == 2)
	{
		if (!stricmp(ext, ".gmf") ||
			!stricmp(ext, ".atc") ||
			!stricmp(ext, ".raw") ||
			!stricmp(ext, ".mtl") ||
			!stricmp(ext, ".gks") ||
			!stricmp(ext, ".gfx") ||
			!stricmp(ext, ".ffx") ||
			!stricmp(ext, ".vfx") ||
			!stricmp(ext, ".h") ||
			!stricmp(ext, ".hkx") ||
			!stricmp(ext, ".hkt") ||
			!stricmp(ext, ".chr") ||
			//!stricmp(ext, ".o") ||
			!stricmp(ext, ".tod") ||
			!stricmp(ext, ".ttf") ||
			!stricmp(ext, ".cfg")
			)
		{
			return true;
		}
	}

	return false;
}

void PrintFile( const TCHAR* file)
{
	if (!FilterEngineFile(file))
	{
		return;
	}

	gkStdString relpath = gkGetExecRelativePath( file );
	printf("%s\n", relpath.c_str());
}

void IndexdFile( const TCHAR* file)
{
	if (!FilterEngineFile(file))
	{
		return;
	}

	gkStdString relpath = gkGetExecRelativePath( file );
	gkStdString absPath = gkGetExecRootDir();
	absPath += relpath;

	gkNormalizePath( relpath );
	
	uint32 crc32 = crc32gen.GetCRC32Lowercase( relpath.c_str() );
	printf("0x%x | %s\n", crc32, relpath.c_str());

	FileRecords::iterator it = g_fileRecords.find( crc32 );
	if ( it != g_fileRecords.end() )
	{
		printf("error!!!!!!!!\n\n\n");
	}
	else
	{
		

		FILE* tmpFile = fopen( absPath.c_str(), "rb" );

		if (tmpFile)
		{
			fseek(tmpFile, 0, SEEK_END);
			uint32 size = ftell(tmpFile);
			fseek(tmpFile, 0, SEEK_SET);

			gkPak_FileRecord* record = new gkPak_FileRecord;
			record->crc32 = crc32;
			record->size = size;
			record->start = g_status.fileAddress;
			record->compressHeaderSize = 0;

			g_fileRecords[crc32] = record;
			
			g_header.m_count++;
			g_status.fileAddress += size;

			strcat( g_filenameBuffer, relpath.c_str() );
			strcat( g_filenameBuffer, "\n" );

			fclose(tmpFile);
		}



	}
}

void WriteFile( const TCHAR* file)
{
	if (!FilterEngineFile(file))
	{
		return;
	}

	FILE* tmpFile = NULL;
	

	gkStdString relpath = gkGetExecRelativePath( file );	
	gkStdString absPath = gkGetExecRootDir();
	absPath += relpath;
    
    gkNormalizePath( relpath );

	tmpFile = fopen( absPath.c_str(), "rb" );

	if (tmpFile)
	{
		fseek(tmpFile, 0, SEEK_END);
		uint32 size = ftell(tmpFile);
		fseek(tmpFile, 0, SEEK_SET);

		// read the data
		char* pTmp = new char[size];
		size_t BytesRead = fread(pTmp, 1, size, tmpFile);

		if (BytesRead == size)
		{
			// record the file record
			// get crc32
			uint32 crc32 = crc32gen.GetCRC32Lowercase( relpath.c_str() );
			FileRecords::iterator it = g_fileRecords.find( crc32 );
			if ( it != g_fileRecords.end() )
			{
				// 如果改类型需要压缩，并且大于最小尺寸，才启用压缩
				bool needCompress = FilterCompressFile( relpath.c_str() );

				if ( size > GKPAK_UNCOMPRESS_HEADER_SIZE && needCompress)
				{
					uint8* pTmpCompressed = new uint8[size];
					SizeT compressSize = size;

					uint8 header[LZMA_PROPS_SIZE + 8];
					size_t headerSize = LZMA_PROPS_SIZE;

					// 前1KB不压缩，一般文件头就这么大把？
					SRes res = LzmaCompress( pTmpCompressed, &compressSize, (Byte*)pTmp + GKPAK_UNCOMPRESS_HEADER_SIZE, size - GKPAK_UNCOMPRESS_HEADER_SIZE, header, &headerSize, g_compressLevel, 1<<24, 3, 0, 2, 32, 4 );
					if (res == SZ_OK)
					{
						std::cout<< relpath <<" | LZMA ok! headersize: "<< headerSize <<" ratio:"<<compressSize * 100 / size<<"%"<<std::endl;

						it->second->compressHeaderSize = it->second->size;
						it->second->size = compressSize + headerSize + GKPAK_UNCOMPRESS_HEADER_SIZE;
						it->second->start = g_status.fileAddress;

						g_fileRecording.push_back( *(it->second));
						
						// 先写不压缩的文件头
						fwrite( pTmp, 1, GKPAK_UNCOMPRESS_HEADER_SIZE, g_status.pakFile );

						// 写压缩header和压缩数据
						fwrite( header, 1, headerSize, g_status.pakFile );
						fwrite( pTmpCompressed, 1, compressSize, g_status.pakFile );

						g_status.fileAddress += it->second->size;
					}
					else
					{
						switch( res )
						{
						case SZ_ERROR_MEM:
							std::cout<<"compress LZMA SZ_ERROR_MEM failed!"<<std::endl;
							break;
						case SZ_ERROR_PARAM:
							std::cout<<"compress LZMA SZ_ERROR_PARAM failed!"<<std::endl;
							break;
						case SZ_ERROR_OUTPUT_EOF:
							std::cout<<"compress LZMA SZ_ERROR_OUTPUT_EOF failed!"<<std::endl;
							break;
						}

						it->second->compressHeaderSize = 0;
						it->second->size = size;
						it->second->start = g_status.fileAddress;

						g_fileRecording.push_back( *(it->second));

						fwrite( pTmp, 1, size, g_status.pakFile );
						g_status.fileAddress += it->second->size;
					}					
					delete[] pTmpCompressed;
				}
				else
				{
					std::cout<< relpath <<"UNCOMPRESS ok! size: "<< size <<std::endl;

					// write to pak
					it->second->start = g_status.fileAddress;

					g_fileRecording.push_back( *(it->second));

					fwrite( pTmp, 1, size, g_status.pakFile );
					g_status.fileAddress += it->second->size;
				}
			}
		}

		delete[] pTmp;

		fclose(tmpFile);
	}

}

void showHelp()
{
	printf("Usage: gkPak -i [in path] -o [out path] -l [compress level]\n -p [platform enum] \n");
	printf("需要*.pak作为输入路径或者输入路径，若为输入路径，则为解包。若为输出路径，则为打包。\n");
	printf("-l 后跟压缩级别0-10，0为不压缩，压缩率顺序增高，耗时顺序增多。默认：5\n");
	printf("-p 平台代码： 0: PC  1: ANDORID/IOS。\n");
	printf("-----------------------------------------\n");
}

void ProcessArgs(const char* args, const char* args1)
{
	if ( !stricmp(args, "-i") )
	{
		strcpy(g_inputPath, args1);
	}
	else if ( !stricmp(args, "-o") )
	{
		strcpy(g_outputPath, args1);
	}
	else if ( !stricmp(args, "-l") )
	{
		g_compressLevel = atoi( args1 );
	}
	else if ( !stricmp(args, "-p") )
	{
		g_platform = atoi( args1 );
	}
}

int main(int numArgs, const char *args[])
{	
	printf("GameKnife Pak System. \n");
	printf("Created by yi kaiming. 2013.03.08 \n");
	printf("-----------------------------------------\n");
	
	g_status.pakFile = NULL;
	g_status.fileAddress = 0;

	// 解析命令行执行 [3/8/2013 Kaiming]
	if (numArgs <= 1)
	{
		// 若没有执行命令，输出帮助信息
		showHelp();
		return 0;
	}

	for ( uint32 i=0; i < numArgs - 1; ++i )
	{
		ProcessArgs( args[i], args[i+1] );
	}

	std::cout<< g_inputPath << " -> " << g_outputPath << " use: cl " << g_compressLevel << "| pf " << g_platform << std::endl;

	bool compressing = true;
	if ( strstr( g_inputPath, ".gpk" ) )
	{
		compressing = false;
	}

	if ( compressing )
	{
		printf("Paking...\n");
		crc32gen.Init();
		printf("crc32 generator init.\n");

		g_filenameBuffer = new char[1024*1024*1];
		strcpy(g_filenameBuffer, "");

		printf("processing folder...\n");

		gkStdString inPath = gkGetExecRootDir();
		inPath += g_inputPath;
		gkGoThroughFolder( inPath.c_str(), IndexdFile );

		printf( "indexed %d files total.", g_header.m_count );

		gkStdString outfile = gkGetExecRootDir();
		outfile += g_outputPath;
        
        
        
		g_status.pakFile = fopen( outfile.c_str(), "wb" );
		if ( !g_status.pakFile)
		{
			printf("FATAL ERROR! 创建文件失败\n");
			return -1;
		}
		g_header.m_data_offset = sizeof(gkPak_Header) + g_header.m_count * sizeof(gkPak_FileRecord) + strlen(g_filenameBuffer);

//		不必直接write，直接fseek到直接buffer
		printf("paking file...\n");

		fseek( g_status.pakFile, g_header.m_data_offset, SEEK_SET );


		// set zero
		g_status.fileAddress = 0;
		gkGoThroughFolder( inPath.c_str(), WriteFile );

//		最终，写入
		fseek( g_status.pakFile, 0, SEEK_SET );
		fwrite( &g_header, 1, sizeof(gkPak_Header), g_status.pakFile );
		fwrite( g_fileRecording.data(), 1, g_header.m_count * sizeof(gkPak_FileRecord), g_status.pakFile );
		fwrite( g_filenameBuffer, 1, strlen(g_filenameBuffer), g_status.pakFile );

		fclose( g_status.pakFile );

		delete[] g_filenameBuffer;

		printf( "writed %d files total.\n", g_header.m_count );

		printf("end.\n");
	}
	else
	{
		printf("UnPaking...\n");

		gkStdString outfile = gkGetExecRootDir();
		outfile += g_inputPath;
		g_status.pakFile = fopen( outfile.c_str(), "rb" );

		if (g_status.pakFile)
		{
			gkPak_Header pakHeader;
			fread( &pakHeader, 1, sizeof(gkPak_Header), g_status.pakFile );

			gkPak_FileRecord* pakFileRecords = new gkPak_FileRecord[pakHeader.m_count];
			fread( pakFileRecords, 1, pakHeader.m_count * sizeof(gkPak_FileRecord), g_status.pakFile );

			uint32 filenameStart = sizeof(gkPak_Header) + pakHeader.m_count * sizeof(gkPak_FileRecord);
			char* filenameBuffer = new char[ pakHeader.m_data_offset - filenameStart + 1 ];
			fread( filenameBuffer, 1, pakHeader.m_data_offset - filenameStart, g_status.pakFile );
			filenameBuffer[ pakHeader.m_data_offset - filenameStart + 1 ]  = 0;

			
			printf( "[record] id         | start byte | size \n" );
			printf("-----------------------------------------\n");
			for (uint32 i=0; i < pakHeader.m_count; ++i)
			{
				printf( "[record] 0x%8x | %10d | %.2fkb\n", pakFileRecords[i].crc32, pakFileRecords[i].start, pakFileRecords[i].size / 1000.f );
			}
			printf("-----------------------------------------\n");
			printf( "find %d files total.\n", pakHeader.m_count );

			//printf( "%s", filenameBuffer );



			fclose( g_status.pakFile );
		}
		else
		{
			printf( "read pak file failed.\n" );
		}

	}
}