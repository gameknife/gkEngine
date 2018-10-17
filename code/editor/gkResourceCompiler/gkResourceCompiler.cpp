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
#include <iostream>
#include "icompiler.h"

std::vector<ICompiler*> g_compilers;

#include "compilerlist.h"

 

#ifndef _STATIC_LIB
#include "gkPlatform_impl.h"
#endif

char g_inputPath[MAX_PATH];
char g_outputPath[MAX_PATH];
int g_compressLevel = 5;
int g_platform = 0;
int g_suppressLevel = 1;
char g_cfg[MAX_PATH];

void showHelp()
{
	printf("Usage: gkPak -i [in path] -o [out path] -l [compress level]\n -p [platform enum] -cfg [custom para]\n");
	printf("需要*.pak作为输入路径或者输入路径，若为输入路径，则为解包。若为输出路径，则为打包。\n");
	printf("-l 后跟压缩级别0-10，0为不压缩，压缩率顺序增高，耗时顺序增多。默认：5\n");
	printf("-p 平台代码： 0: PC  1: ANDORID/IOS。\n");
	printf("-cfg 资源特殊编译选项, 图片转码时，为格式，例如OGL4444 PVRTC ATC等");
	printf("-y 输出级别： 0: 安静模式  1: 正常\n");
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
	else if ( !stricmp(args, "-cfg") )
	{
		strcpy(g_cfg, args1);
	}
	else if (!stricmp(args, "-y"))
	{
		g_suppressLevel = atoi(args1);
	}	
}

#ifdef OS_APPLE
//
//#import <Foundation/Foundation.h>
//#import <CoreFoundation/CoreFoundation.h>
//
//// implement of macPathUtil
//std::string macBundlePath()
//{
//    char path[PATH_MAX];
//    CFBundleRef mainBundle = CFBundleGetMainBundle();
//    assert(mainBundle);
//    
//    CFURLRef mainBundleURL = CFBundleCopyBundleURL(mainBundle);
//    assert(mainBundleURL);
//    
//    CFStringRef cfStringRef = CFURLCopyFileSystemPath( mainBundleURL, kCFURLPOSIXPathStyle);
//    assert(cfStringRef);
//    
//    CFStringGetCString(cfStringRef, path, PATH_MAX, kCFStringEncodingASCII);
//    
//    CFRelease(mainBundleURL);
//    CFRelease(cfStringRef);
//    
//    char* lastpath = strrchr(path, '/');
//    if (lastpath) {
//        *lastpath = 0;
//    }
//    lastpath = strrchr(path, '/');
//    if (lastpath) {
//        *lastpath = 0;
//    }
//    lastpath = strrchr(path, '/');
//    if (lastpath) {
//        *lastpath = 0;
//    }
//    
//    return std::string(path);
//}
//
//std::string iOSDocumentsDirectory()
//{
//    return macBundlePath();
//}
//
#endif

int main(int numArgs, const char *args[])
{	
	// 解析命令行执行 [3/8/2013 Kaiming]
	if (numArgs <= 1)
	{
		// 若没有执行命令，输出帮助信息
		showHelp();
		system("pause");
		return 0;
	}

	for ( int i=0; i < numArgs - 1; ++i )
	{
		ProcessArgs( args[i], args[i+1] );
	}

	if(g_suppressLevel)
	{
		printf("GameKnife Resource Compiler. \n");
		printf("Created by yi kaiming. 2013.03.25 \n");
		printf("-----------------------------------------\n");

		for (uint32 i = 0; i < g_compilers.size(); ++i)
		{
			printf("	");
			printf(g_compilers[i]->getName());
			printf(" integraded. \n");
		}
		printf("-----------------------------------------\n");

		std::cout << g_inputPath << " -> " << g_outputPath << " use: cl " << g_compressLevel << "| pf " << g_platform << std::endl;
	}

	const char* in_ext = strrchr( g_inputPath, '.' );
	const char* out_ext = strrchr( g_outputPath, '.');

	for (uint32 i=0; i < g_compilers.size(); ++i)
	{
		ICompiler* compiler = g_compilers[i];
		if (compiler->supportInput(in_ext) && compiler->supportOutput(out_ext))
		{
			compiler->encode(g_inputPath, g_cfg);
			compiler->writeFile(g_outputPath, g_cfg);
			break;
		}
	}	
}