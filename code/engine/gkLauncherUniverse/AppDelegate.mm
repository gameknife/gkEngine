//
//  AppDelegate.m
//  gkLauncherIOS
//
//  Created by gameKnife on 13-5-3.
//  Copyright (c) 2013年 gameKnife. All rights reserved.
//

#import "AppDelegate.h"
#import <QuartzCore/CADisplayLink.h>

#include "gkPlatform.h"
#include "IGameFramework.h"
#include "IRenderer.h"
#include "gkPlatform_impl.h"

IGameFramework* gkLoadStaticModule_gkGameFramework();
void gkFreeStaticModule_gkGameFramework();

IGameFramework* g_pGame;

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

// implement of macPathUtil
std::string macBundlePath()
{
    char path[PATH_MAX];
    CFBundleRef mainBundle = CFBundleGetMainBundle();
    assert(mainBundle);
    
    CFURLRef mainBundleURL = CFBundleCopyBundleURL(mainBundle);
    assert(mainBundleURL);
    
    CFStringRef cfStringRef = CFURLCopyFileSystemPath( mainBundleURL, kCFURLPOSIXPathStyle);
    assert(cfStringRef);
    
    CFStringGetCString(cfStringRef, path, PATH_MAX, kCFStringEncodingASCII);
    
    CFRelease(mainBundleURL);
    CFRelease(cfStringRef);
    
    return std::string(path);
}

std::string iOSDocumentsDirectory()
{
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
    NSFileManager *fileManager = [NSFileManager defaultManager];
    NSError *error = nil;
    NSArray *fileList = [[NSArray alloc] init];
    //fileList便是包含有该文件夹下所有文件的文件名及文件夹名的数组
    NSString* string = [[[NSString alloc] initWithBytes:root_path length:strlen(root_path) encoding:NSUTF8StringEncoding] autorelease];
    
    fileList = [fileManager contentsOfDirectoryAtPath:string error:&error];
    //NSLog(@"路径==%@,fileList%@",string,fileList);
    
    for (NSString *str in fileList) {
        result.push_back( [str UTF8String] );
    }
    
    
    
    return;
}

@implementation AppDelegate


//CONSTANTS:

const int kFPS = 60.0;
#define kAccelerometerFrequency		30.0 //Hz
#define kFilteringFactor			0.1

// MACROS
#define DEGREES_TO_RADIANS(__ANGLE__) ((__ANGLE__) / 180.0 * M_PI)

- (void) _renderGLScene
{
    g_pGame->Update();
}

- (void) applicationDidFinishLaunching:(UIApplication*)application
{
    g_pGame = gkLoadStaticModule_gkGameFramework();
    
    ISystemInitInfo sii;
    
    g_pGame->Init(sii);
    g_pGame->PostInit(0, sii);
    
    g_pGame->InitGame(NULL);
	
	//Configure and start accelerometer
	[[UIAccelerometer sharedAccelerometer] setUpdateInterval:(1.0 / kAccelerometerFrequency)];
	[[UIAccelerometer sharedAccelerometer] setDelegate:self];
    
	[UIApplication sharedApplication].idleTimerDisabled = YES;
	
	//Render the initial frame
	[self _renderGLScene];
	
	//Create our rendering timer
	// In later versions of the iPhone SDK there is a dedicated function for this
	// This version is used here for compatiblity
    
    _renderTimer = [CADisplayLink displayLinkWithTarget:self selector:@selector(_renderGLScene)];
    
    _renderTimer.frameInterval = 1;
    
    [_renderTimer addToRunLoop: [NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
}

- (void) dealloc {
	// In ordinary circumstances this isn't called
	[super dealloc];
}

// throws up a warning dialog
- (void) doExitFromFunction:(NSString*)reason
{
	// Exiting an iPhone application programmatically is not possible from public APIs (at time of writing)
	// So this just puts up a message and lets the user press the button and quit
	printf("%s\n",[reason UTF8String]);
	UIAlertView *myExitWindow = [[UIAlertView alloc]
								 initWithFrame: [[UIScreen mainScreen] bounds]];
	[myExitWindow setTitle:reason];
	[myExitWindow show];
}


- (void) accelerometer:(UIAccelerometer*)accelerometer didAccelerate:(UIAcceleration*)acceleration {

}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    return (interfaceOrientation == UIInterfaceOrientationLandscapeRight);
}

@end
