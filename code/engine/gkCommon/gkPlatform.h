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



#pragma once

//////////////////////////////////////////////////////////////////////////
//
// Name:   	gkPlatform.h
// Desc:	define the win32 platfrom and other basic things
// 描述:	
// 
// Author:  Kaiming-Desktop	 
// Date:	9/9/2010 	
// 
//////////////////////////////////////////////////////////////////////////

/// platform


//////////////////////////////////////////////////////////////////////////
// define win32 platform
#ifndef WINVER                  // Specifies that the minimum required platform is Windows Vista.
#define WINVER 0x0600           // Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT            // Specifies that the minimum required platform is Windows Vista.
#define _WIN32_WINNT 0x0600     // Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINDOWS          // Specifies that the minimum required platform is Windows 98.
#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE                       // Specifies that the minimum required platform is Internet Explorer 7.0.
#define _WIN32_IE 0x0700        // Change this to the appropriate value to target other versions of IE.
#endif
//////////////////////////////////////////////////////////////////////////



#ifndef INCLUDED_GKPLATFORM_H
#define INCLUDED_GKPLATFORM_H

//////////////////////////////////////////////////////////////////////////
// disable type conversion warnings
#pragma warning(disable:4244)
#pragma warning(disable:4305)


//////////////////////////////////////////////////////////////////////////
// Compiler Judge
// Determine compiler: after this, one of the following symbols will
// be defined: COMPILER_GCC, COMPILER_MSVC.

#if defined(__GNUG__)
# define COMPILER_GCC
#elif defined(_MSC_VER)
# define COMPILER_MSVC
// i don't care about your debug symbol issues...
# pragma warning(disable:4786)
#else
# error "Could not determine compiler"
#endif


//////////////////////////////////////////////////////////////////////////
// OS Judge
// Determine OS: after this, one of the following symbols will be
// defined: OS_LINUX, OS_WIN32, OS_NETBSD.


#if defined( __SYMBIAN32__ ) 
#   define OS_SYMBIAN
#elif defined( __WIN32__ ) || defined( _WIN32 )
#   define OS_WIN32
#elif defined( __APPLE_CC__)
#   if __ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__ >= 40000 || __IPHONE_OS_VERSION_MIN_REQUIRED >= 40000
#       define OS_IOS
#   else
#       define OS_IOS
#       define OS_APPLE
#   endif
#elif defined(__ANDROID__)
#	define OS_ANDROID
#else
#	error "Could not determine OS"
#endif

#if defined (OS_ANDROID) || defined( OS_IOS ) || defined( OS_APPLE )
#	define OS_LINUX
#endif

#define BIT(x) (1<<(x))

//default stack size for threads, currently only used on PS3
#define SIMPLE_THREAD_STACK_SIZE_KB (32)

// WIN32 SPECIFIC
#if defined( OS_WIN32 )
#	include "gkWin32Specific.h"
#endif

#if defined( OS_LINUX )
#	include "gkLinuxSpecific.h"
#endif


//////////////////////////////////////////////////////////////////////////
// shared lib access define

#if defined(COMPILER_GCC)
#define DLL_EXPORT __attribute__ ((visibility("default")))
#define DLL_IMPORT __attribute__ ((visibility("default")))

#define DLL_OPEN(a) dlopen(a, RTLD_LAZY | RTLD_GLOBAL)
#define DLL_GETSYM(a,b) dlsym(a,b)
#define DLL_FREE(a) dlclose(a)

#define DLL_PREFIX _T("/data/data/com.kkstudio.gklauncher/lib/lib")
#define DLL_SUFFIX _T(".so")

#else
#define DLL_EXPORT __declspec(dllexport)
#define DLL_IMPORT __declspec(dllimport)

#define DLL_OPEN(a) LoadLibraryEx(a, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
#define DLL_GETSYM(a,b) GetProcAddress(a,b)
#define DLL_FREE(a) FreeLibrary(a)

#define DLL_PREFIX _T("")
#define DLL_SUFFIX _T(".dll")

#endif



//////////////////////////////////////////////////////////////////////////
// Module Init

struct SSystemGlobalEnvironment; 
extern SSystemGlobalEnvironment* gEnv;

// log declare
extern void gkLogMessage(const TCHAR *, ...);
extern void gkLogInput(const TCHAR *, ...);
extern void gkLogWarning(const TCHAR *, ...); 
extern void gkLogError(const TCHAR *, ...);

// start & end func
typedef void (*MODULE_START)(SSystemGlobalEnvironment*);
typedef void (*MODULE_END)(void);

// module accessing func
void gkLoadModule(HINSTANCE&, const TCHAR*);
void gkOpenModule(HINSTANCE&, const TCHAR*);
void gkFreeModule(HINSTANCE&);

inline void gkFreeModule( HINSTANCE& hHandle)
{
	if (hHandle)
	{
		MODULE_END pFunc = (MODULE_END)DLL_GETSYM(hHandle, "gkModuleUnload");
		pFunc();
		DLL_FREE(hHandle);
	}
}

inline void gkLoadModule( HINSTANCE& hHandle, const TCHAR* moduleName)
{
	// load input dll [7/20/2011 Kaiming-Desktop]
	// cat .dll or .so
	gkLogMessage( _T("loading lib[ %s ]..."), moduleName );
	TCHAR finalModuleName[MAX_PATH] = DLL_PREFIX;
	_tcscat( finalModuleName, moduleName );
	_tcscat( finalModuleName, DLL_SUFFIX);

	hHandle = DLL_OPEN(finalModuleName);
	if (hHandle)
	{
		gkLogMessage( _T("lib[ %s ] entry calling..."), moduleName );
		MODULE_START pFunc = (MODULE_START)DLL_GETSYM(hHandle, "gkModuleInitialize");
		if(pFunc)
		{
			pFunc(gEnv);
		}
				
	}
	else
	{
		gkLogError( _T("loading lib[ %s ] failed."), moduleName );
#ifdef OS_ANDROID
		gkLogError( _T("dlerr: %s"), dlerror());
#endif
	}
}

inline void gkOpenModule( HINSTANCE& hHandle, const TCHAR* moduleName)
{
	// load input dll [7/20/2011 Kaiming-Desktop]
	// cat .dll or .so
	gkLogMessage( _T("loading lib[ %s ]..."), moduleName );
	TCHAR finalModuleName[MAX_PATH] = DLL_PREFIX;
	_tcscat( finalModuleName, moduleName );
	_tcscat( finalModuleName, DLL_SUFFIX);

	hHandle = DLL_OPEN(finalModuleName);
	if (hHandle)
	{
		gkLogMessage( _T("loading lib[ %s ] successed."), moduleName );
	}
	else
	{
		gkLogMessage( _T("loading lib[ %s ] failed."), moduleName );
	}
}

//////////////////////////////////////////////////////////////////////////
// static library define

#ifdef _STATIC_LIB
	#define LOAD_MODULE_GLOBAL( a, b )		gkLoadStaticModule_##b(gEnv);a = (HINSTANCE)1;
	#define UNLOAD_MODULE_GLOBAL( a, b )	gkFreeStaticModule_##b();a = (HINSTANCE)0;	
#else
	#define LOAD_MODULE_GLOBAL( a, b )		gkLoadModule( a, _T(#b) )
	#define UNLOAD_MODULE_GLOBAL( a, b )	gkFreeModule( a )
#endif

//////////////////////////////////////////////////////////////////////////
// ASSERT
#include "gkAssert.h"

//////////////////////////////////////////////////////////////////////////
// MATH LIB
#include "math.h"


//#ifdef OS_IOS
//#	include "gk_Math.h"
//#else
#	include "MathLib/gk_Math.h"
//#endif

//////////////////////////////////////////////////////////////////////////
// STD INCLUDE
#include <memory>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <deque>
#include <stack>
#include <algorithm>
#include <functional>
#include <string>
#include <queue>

//////////////////////////////////////////////////////////////////////////
// STD PLATFORM DEFINE
#if defined( OS_WIN32 )
#	include <hash_map>
#	define gkHashMap stdext::hash_map
#else
#	define gkHashMap std::map
#endif

//////////////////////////////////////////////////////////////////////////
// string binding
#if defined( UNICODE ) || defined( _UNICODE )
#	define gkStdString std::wstring
#	define gkStdStringstream std::wstringstream
#	define gkStdOStringstream std::wostringstream
#	define gkStdIFstream wifstream
#else
#	define gkStdString std::string
#	define gkStdStringstream std::stringstream
#	define gkStdOStringstream std::ostringstream
#	define gkStdIFstream ifstream
#endif

extern gkStdString GKNULLSTR;

#ifndef UNIQUE_IFACE
#define UNIQUE_IFACE
#endif


#define ANDROID_APP_NAME "com.kkstudio.gklauncher"
#define ANDROID_APP_ROMROOT "/data/data/com.kkstudio.gklauncher/"
#define ANDROID_APP_SDCARDROOT "/storage/emulated/legacy/gkENGINE/"

// gkCommonStructs
#include "gkCommonStructs.h"

// gkPathtool
//#if !defined( _AFXDLL ) && !defined( _3DSMAX )
#	include "gkFilePath.h"
//#endif


#if defined( OS_ANDROID )
#	include "gkAndroidSpecific.h"
#endif

#if defined( OS_IOS )
#	include "gkIOSSpecific.h"
#endif

#ifdef OS_WIN32
typedef uint32 gk_thread_id;
#else
typedef pthread_t gk_thread_id;
#endif


#endif // !INCLUDED_GKPLATFORM_H


