cmake_minimum_required(VERSION 3.6)
project (gkENGINE)

if (APPLE)
    MESSAGE ("This is build for ios, use static lib")
	set(CoreLinkType STATIC)
else()
    MESSAGE ("This is build for windows, use dynamic lib")
	set(CoreLinkType SHARED)

    # add warning mute
    add_compile_options(/w44996)
    add_compile_options(/w44819)
    add_compile_options(/w44215)
    add_compile_options(/w44251)
    add_compile_options(/w44267)
    add_compile_options(/w44005)
    add_compile_options(/w44311)
    add_compile_options(/w44302)
    add_compile_options(/w44067)
    
endif()

set(CMAKE_MACOSX_RPATH 0)

# The version number.
set (gkENGINE_VERSION_MAJOR 1)
set (gkENGINE_VERSION_MINOR 0)

# add the binary tree to the search path for include files
# so that we will find TutorialConfig.h
include_directories ("${PROJECT_BINARY_DIR}")


# add the global inlude dir
include_directories ("${PROJECT_SOURCE_DIR}/code/engine/gkCommon")
include_directories ("${PROJECT_SOURCE_DIR}/code/engine/gkCommon/MathLib")
include_directories ("${PROJECT_SOURCE_DIR}/code/engine/gkCommon/serializer")
include_directories ("${PROJECT_SOURCE_DIR}/code/thirdparty")
include_directories ("${PROJECT_SOURCE_DIR}/code/thirdparty/rapidxml")

# add gkCore
FILE(GLOB sources ./code/engine/gkCore/*.cpp ./code/engine/gkCore/*/*.cpp)
add_library(gkCore ${CoreLinkType} ${sources})
target_include_directories(gkCore PRIVATE
  ./code/engine/gkCore/
  ./code/engine/gkCore/include/
)
set (EXTRA_LIBS ${EXTRA_LIBS} gkCore)

# add gkFont
FILE(GLOB sources ./code/engine/gkFont/*.cpp ./code/engine/gkFont/*/*.cpp)
add_library(gkFont ${CoreLinkType} ${sources})
target_include_directories(gkFont PRIVATE
    ./code/engine/gkFont/
    ./code/engine/gkFont/include/
    ./code/ThirdParty/freetype/include/
)
if (IOS)
target_link_libraries(gkFont 
    ${PROJECT_SOURCE_DIR}/code/ThirdParty/freetype/lib/ios/Release-iphoneos/libfreetype2.a
)
elseif(OSX)
target_link_libraries(gkFont 
    ${PROJECT_SOURCE_DIR}/code/ThirdParty/freetype/lib/ios/Release-OSX/libfreetype2.a
)
else()
target_link_libraries(gkFont 
    ${PROJECT_SOURCE_DIR}/code/ThirdParty/freetype/lib/freetype_x64release.lib
)
endif()
set (EXTRA_LIBS ${EXTRA_LIBS} gkFont)

FILE(GLOB sources ./code/engine/gkGameFramework/*.cpp ./code/engine/gkGameFramework/*/*.cpp)
add_library(gkGameFramework ${CoreLinkType} ${sources})
target_include_directories(gkGameFramework PRIVATE
    ./code/engine/gkGameFramework/
  ./code/engine/gkGameFramework/include/
)
set (EXTRA_LIBS ${EXTRA_LIBS} gkGameFramework)

FILE(GLOB sources ./code/engine/gkGameObjectSystem/*.cpp ./code/engine/gkGameObjectSystem/*/*.cpp)
add_library(gkGameObjectSystem ${CoreLinkType} ${sources})
target_include_directories(gkGameObjectSystem PRIVATE
    ./code/engine/gkGameObjectSystem/
  ./code/engine/gkGameObjectSystem/include/
)
set (EXTRA_LIBS ${EXTRA_LIBS} gkGameObjectSystem)

# gkInput
FILE(GLOB sources ./code/engine/gkInput/*.cpp ./code/engine/gkInput/source/*.cpp)
if (APPLE)
    add_library(gkInput ${CoreLinkType} 
    ./code/engine/gkInput/source/dllmain.cpp
    ./code/engine/gkInput/source/gkInputManager.cpp
    ./code/engine/gkInput/source/gkIOSInput.cpp
    ./code/engine/gkInput/source/InputStableHeader.cpp
    )
else()
    add_library(gkInput ${CoreLinkType} ${sources})
endif()

target_include_directories(gkInput PRIVATE
    ./code/engine/gkInput/
    ./code/engine/gkInput/include/
    ./code/ThirdParty/DXSDK/include/
)

if (APPLE)

else()
target_link_libraries(gkInput 
    ${PROJECT_SOURCE_DIR}/code/ThirdParty/DXSDK/lib/x64/dinput8.lib
    ${PROJECT_SOURCE_DIR}/code/ThirdParty/DXSDK/lib/x64/dxguid.lib
)
endif()

set (EXTRA_LIBS ${EXTRA_LIBS} gkInput)

# gkSystem
if (APPLE)
FILE(GLOB sources ./code/engine/gkCommon/*.cpp ./code/engine/gkSystem/*.cpp ./code/engine/gkSystem/*/*.cpp ./code/engine/gkSystem/*/*.mm ./code/engine/gkSystem/*/*.c)
else()
FILE(GLOB sources ./code/engine/gkCommon/*.cpp ./code/engine/gkSystem/*.cpp ./code/engine/gkSystem/*/*.cpp ./code/engine/gkSystem/*/*.c)
endif()

add_library(gkSystem ${CoreLinkType} ${sources})
target_include_directories(gkSystem PRIVATE
  ./code/engine/gkSystem/
  ./code/engine/gkSystem/include/
)

if (APPLE)

else()
target_link_libraries(gkSystem 
    winmm.lib
)
endif()

set (EXTRA_LIBS ${EXTRA_LIBS} gkSystem)


# Testcases
add_library(Testcases ${CoreLinkType} ./code/engine/gkTestApps/TestCases/TestCaseFramework.cpp)
target_include_directories(Testcases PRIVATE
    ./code/engine/gkTestApps/TestCases/
)

set (EXTRA_LIBS ${EXTRA_LIBS} Testcases)

# platfrom specific
if (APPLE)




if(IOS)

    FILE(GLOB sources 
        ./code/engine/gkRendererGL/*.cpp 
        ./code/engine/gkRendererGL/RenderRes/*.cpp 
        ./code/engine/gkRendererGL/Platform/IOS/*.mm
    )
    list(FILTER sources EXCLUDE REGEX ".*DDsTextureLoader.cpp$")

    add_library(gkRendererGLES2 ${CoreLinkType} ${sources})
    target_include_directories(gkRendererGLES2 PRIVATE
        ./code/engine/gkRendererGL/
        ./code/engine/gkRendererGLES2/
        ./code/engine/gkRendererGLES2/include/
    )
    target_compile_definitions(gkRendererGLES2 PRIVATE -DBUILD_OGLES2 -DRENDERER_GLES -DRENDERAPI_GLES2)
    set (EXTRA_LIBS ${EXTRA_LIBS} gkRendererGLES2)




    FILE(GLOB sources ./code/engine/gkLauncherUniverse/*.m ./code/engine/gkLauncherUniverse/*.mm)
    add_executable(gkLauncher ${sources})
    target_include_directories(gkLauncher PRIVATE
        ./code/engine/gkLauncherUniverse/
        ./code/engine/gkLauncherUniverse/include/
    )
    target_link_libraries(gkLauncher "-framework Foundation -framework UIKit -framework OpenGLES -framework CoreGraphics -framework QuartzCore")
    target_link_libraries (gkLauncher ${EXTRA_LIBS} )

    set(DEPLOYMENT_TARGET 8.0)


else()
#OSX
FILE(GLOB sources 
./code/engine/gkRendererGL/*.cpp 
./code/engine/gkRendererGL/RenderRes/*.cpp 
./code/engine/gkRendererGL/Platform/OSX/*.cpp
./code/engine/gkRendererGL/Platform/OSX/*.mm
)
list(FILTER sources EXCLUDE REGEX ".*HwTextureLoader.cpp$")
list(FILTER sources EXCLUDE REGEX ".*glExtension.cpp$")

add_library(gkRendererGL330 ${CoreLinkType} ${sources})
target_include_directories(gkRendererGL330 PRIVATE
    ./code/engine/gkRendererGL/
    ./code/engine/gkRendererGL330/
    ./code/engine/gkRendererGL330/include/
    ./code/thirdparty/oglsdk/include/
)
target_compile_definitions(gkRendererGL330 PRIVATE -DRENDERAPI_GL330)
set (EXTRA_LIBS ${EXTRA_LIBS} gkRendererGL330)

target_link_libraries(gkRendererGL330 
    ${PROJECT_SOURCE_DIR}/code/ThirdParty/oglsdk/lib/libglfw3.a
)



FILE(GLOB sources ./code/engine/gkLauncherMac/*.m ./code/engine/gkLauncherMac/*.mm)
add_executable(gkLauncher ${sources})
target_include_directories(gkLauncher PRIVATE
    ./code/engine/gkLauncherMac/
    ./code/engine/gkLauncherMac/include/
)
target_link_libraries(gkLauncher "-framework AppKit -framework OpenGL -framework CoreGraphics -framework IOKit -framework CoreVideo")
target_link_libraries (gkLauncher ${EXTRA_LIBS} )

#set(DEPLOYMENT_TARGET 8.0)

endif()


        



#file(GLOB IOSAssets ios/assets/*.png)
#list(REMOVE_ITEM IOSAssets ${CMAKE_CURRENT_SOURCE_DIR}/ios/assets/Default-568h@2x.png)
#list(REMOVE_ITEM IOSAssets ${CMAKE_CURRENT_SOURCE_DIR}/ios/assets/Default-568h@3x.png)
#file(INSTALL ${IOSAssets} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/assets)
#file(GLOB IOSAssets ios/assets/Default-568h@*.png)
#file(INSTALL ${IOSAssets} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

#set(RSRC_XIB_FILES assets/Icon@2x.png)
#set(RSRC_XIB_FILES "Launch Screen.storyboard")
#set_source_files_properties(${RSRC_XIB_FILES}
#    PROPERTIES MACOSX_PACKAGE_LOCATION Resources
#)

if(CMAKE_GENERATOR STREQUAL "Xcode")
    set(APP_DIR_NAME "$(TARGET_BUILD_DIR)/$(FULL_PRODUCT_NAME)")
else()
    set(APP_DIR_NAME "$<TARGET_FILE_DIR:PPSSPP>")
endif()

add_custom_command(TARGET gkLauncher POST_BUILD
    COMMAND rm -rf "$(TARGET_BUILD_DIR)/$(PRODUCT_NAME).app/data"
    COMMAND mkdir "$(TARGET_BUILD_DIR)/$(PRODUCT_NAME).app/data"
    COMMAND cp -Rf "$(BUILD_DIR)/../exec/paks" "$(TARGET_BUILD_DIR)/$(PRODUCT_NAME).app/data/paks"
)

set(MACOSX_DEPLOYMENT_TARGET ${DEPLOYMENT_TARGET})





if(IOS)

set_target_properties(gkLauncher PROPERTIES
    MACOSX_BUNDLE_INFO_PLIST "${PROJECT_SOURCE_DIR}/code/engine/gkLauncherUniverse/Info.plist"
    #RESOURCE "ios/Launch Screen.storyboard"
    XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET ${DEPLOYMENT_TARGET}
    XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY "iPhone/iPad"
    XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC YES
    XCODE_ATTRIBUTE_ENABLE_BITCODE NO
    XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "-"
)

set_target_properties(gkLauncher
    PROPERTIES
    #ARCHIVE_OUTPUT_DIRECTORY "/home/xy/cmake_practice/lib/"
    LIBRARY_OUTPUT_DIRECTORY_RELEASE "${PROJECT_SOURCE_DIR}/exec/ios"
    LIBRARY_OUTPUT_DIRECTORY_DEBUG "${PROJECT_SOURCE_DIR}/exec/ios"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${PROJECT_SOURCE_DIR}/exec/ios"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${PROJECT_SOURCE_DIR}/exec/ios"
)

else()

set_target_properties(gkLauncher PROPERTIES
    MACOSX_BUNDLE_INFO_PLIST "${PROJECT_SOURCE_DIR}/code/engine/gkLauncherUniverse/Info.plist"
    #RESOURCE "ios/Launch Screen.storyboard"
    #XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET ${DEPLOYMENT_TARGET}
    #XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY "iPhone/iPad"
    XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC YES
    #XCODE_ATTRIBUTE_ENABLE_BITCODE NO
    #XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "-"
)

endif()



else()

FILE(GLOB sources ./code/engine/gkRendererD3D9/*.cpp ./code/engine/gkRendererD3D9/*/*.cpp)
add_library(gkRendererD3D9 ${CoreLinkType} ${sources})
target_include_directories(gkRendererD3D9 PRIVATE
    ./code/ThirdParty/DXSDK/include
    ./code/engine/gkRendererD3D9/DXUT/
    ./code/engine/gkRendererD3D9/RenderRes/
    ./code/engine/gkRendererD3D9/
    ./code/engine/gkRendererD3D9/include/
)
target_link_libraries(gkRendererD3D9 
    winmm.lib
    comctl32.lib
    ${PROJECT_SOURCE_DIR}/code/ThirdParty/DXSDK/lib/x64/d3d9.lib
    ${PROJECT_SOURCE_DIR}/code/ThirdParty/DXSDK/lib/x64/dxerr.lib
    ${PROJECT_SOURCE_DIR}/code/ThirdParty/DXSDK/lib/x64/dxguid.lib
    ${PROJECT_SOURCE_DIR}/code/ThirdParty/DXSDK/lib/x64/d3dx9.lib
    ${PROJECT_SOURCE_DIR}/code/ThirdParty/DXSDK/lib/x64/d3dx10.lib
)

FILE(GLOB sources ./code/engine/gkLauncherUniverse/*.cpp ./code/engine/gkLauncherUniverse/*/*.cpp)
add_executable(gkLauncher ${sources})
target_include_directories(gkLauncher PRIVATE
    ./code/engine/gkLauncherUniverse/
  ./code/engine/gkLauncherUniverse/include/
)

# set all targets output dir
set_target_properties(gkCore gkLauncher gkSystem gkRendererD3D9 gkInput gkGameObjectSystem gkGameFramework gkFont Testcases
    PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY_RELEASE "${PROJECT_SOURCE_DIR}/exec/bin64"
    LIBRARY_OUTPUT_DIRECTORY_DEBUG "${PROJECT_SOURCE_DIR}/exec/bin64"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${PROJECT_SOURCE_DIR}/exec/bin64"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${PROJECT_SOURCE_DIR}/exec/bin64"
)

endif()


