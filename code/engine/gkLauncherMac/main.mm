//
//  main.m
//  gkLauncherMac
//
//  Created by gameKnife on 13-7-14.
//  Copyright (c) 2013年 kkstudio. All rights reserved.
//

#include "gkPlatform.h"
#include "IGameFramework.h"
#include "IRenderer.h"
#include "gkPlatform_impl.h"

#import <Foundation/Foundation.h>
#import <CoreFoundation/CoreFoundation.h>
#import <AppKit/NSApplication.h>
#import <AppKit/NSWindow.h>

IGameFramework* gkLoadStaticModule_gkGameFramework();
void gkFreeStaticModule_gkGameFramework();

IGameFramework* g_pGame;

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

// implement of macPathUtil
std::string macBundlePath()
{
    NSFileManager* files = [NSFileManager defaultManager];
    bool (^isExecRoot)(NSString*) = ^bool(NSString* candidate) {
        if (!candidate)
            return false;
        BOOL isDirectory = NO;
        NSString* enginePath = [candidate stringByAppendingPathComponent:@"engine"];
        NSString* toolsPath = [candidate stringByAppendingPathComponent:@"tools"];
        return [files fileExistsAtPath:enginePath isDirectory:&isDirectory] &&
            isDirectory && [files fileExistsAtPath:toolsPath isDirectory:&isDirectory] &&
            isDirectory;
    };

    NSMutableArray* candidates = [NSMutableArray array];
    const char* environmentRoot = getenv("GK_ENGINE_EXEC_ROOT");
    if (environmentRoot && environmentRoot[0])
        [candidates addObject:[NSString stringWithUTF8String:environmentRoot]];
#ifdef GK_ENGINE_EXEC_ROOT
    [candidates addObject:@GK_ENGINE_EXEC_ROOT];
#endif

    NSString* searchPath = [[NSBundle mainBundle] bundlePath];
    for (int depth = 0; depth < 8 && searchPath.length; ++depth) {
        [candidates addObject:[searchPath stringByAppendingPathComponent:@"exec"]];
        NSString* parent = [searchPath stringByDeletingLastPathComponent];
        if ([parent isEqualToString:searchPath])
            break;
        searchPath = parent;
    }
    NSString* workingDirectory = [files currentDirectoryPath];
    [candidates addObject:workingDirectory];
    [candidates addObject:[workingDirectory stringByAppendingPathComponent:@"exec"]];

    for (NSString* candidate in candidates) {
        NSString* standardized = [[candidate stringByExpandingTildeInPath]
            stringByStandardizingPath];
        if (isExecRoot(standardized))
            return std::string([standardized fileSystemRepresentation]);
    }

    fprintf(stderr,
        "gkLauncher: unable to locate exec (set GK_ENGINE_EXEC_ROOT)\n");
    return std::string();
}

std::string iOSDocumentsDirectory()
{
    return macBundlePath();
    
    
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *documentsDirectory = [paths objectAtIndex:0];
    
    return std::string([documentsDirectory cStringUsingEncoding:NSASCIIStringEncoding]);
}

std::string macCachePath()
{
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
    NSString *cachesDirectory = [paths objectAtIndex:0];
    
    return [[cachesDirectory stringByAppendingString:@"/"] cStringUsingEncoding:NSASCIIStringEncoding];
}

void enum_all_files_in_folder( const TCHAR* root_path,std::vector<gkStdString>& result,bool inc_sub_folders/*=false*/ )
{
    
}

int main(int argc, char *argv[])
{
    g_pGame = gkLoadStaticModule_gkGameFramework();
    
    ISystemInitInfo sii;
    sii.fWidth = 1280;
    sii.fHeight = 720;
    const std::string execRoot = macBundlePath();
    if (execRoot.empty()) {
        gkFreeStaticModule_gkGameFramework();
        return EXIT_FAILURE;
    }
    gkStdString rootPath = execRoot + "/";
    sii.rootDir = rootPath.c_str();
    
    if (!g_pGame->Init(sii)) {
        gkFreeStaticModule_gkGameFramework();
        return EXIT_FAILURE;
    }
    g_pGame->PostInit(0, sii);
    
    g_pGame->InitGame(NULL);
    
    while (1) {
        if( !g_pGame->Update() )
        {
            break;
        }
    }
    
    g_pGame->Destroy();
    
    return 0;
}
