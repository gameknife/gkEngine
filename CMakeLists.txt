cmake_minimum_required(VERSION 3.6)
project (gkENGINE)

if (IOS)
    MESSAGE ("This is build for ios, use static lib")
	set(CoreLinkType STATIC)
else()
    MESSAGE ("This is build for ios, use dynamic lib")
	set(CoreLinkType SHARED)
endif()

set(CMAKE_MACOSX_RPATH 0)

# The version number.
set (gkENGINE_VERSION_MAJOR 1)
set (gkENGINE_VERSION_MINOR 0)

# add the binary tree to the search path for include files
# so that we will find TutorialConfig.h
include_directories ("${PROJECT_BINARY_DIR}")

# add warning mute
#add_compile_options(/w44996)
#add_compile_options(/w44819)
#add_compile_options(/w44215)
#add_compile_options(/w44251)
#add_compile_options(/w44267)
#add_compile_options(/w44005)
#add_compile_options(/w44311)
#add_compile_options(/w44302)
#add_compile_options(/w44067)

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

# add gkFont
FILE(GLOB sources ./code/engine/gkFont/*.cpp ./code/engine/gkFont/*/*.cpp)
add_library(gkFont ${CoreLinkType} ${sources})
target_include_directories(gkFont PRIVATE
    ./code/engine/gkFont/
    ./code/engine/gkFont/include/
    ./code/ThirdParty/freetype/include/
)
target_link_libraries(gkFont 
    ${PROJECT_SOURCE_DIR}/code/ThirdParty/freetype/lib/freetype_x64release.lib
)

FILE(GLOB sources ./code/engine/gkGameFramework/*.cpp ./code/engine/gkGameFramework/*/*.cpp)
add_library(gkGameFramework ${CoreLinkType} ${sources})
target_include_directories(gkGameFramework PRIVATE
    ./code/engine/gkGameFramework/
  ./code/engine/gkGameFramework/include/
)

FILE(GLOB sources ./code/engine/gkGameObjectSystem/*.cpp ./code/engine/gkGameObjectSystem/*/*.cpp)
add_library(gkGameObjectSystem ${CoreLinkType} ${sources})
target_include_directories(gkGameObjectSystem PRIVATE
    ./code/engine/gkGameObjectSystem/
  ./code/engine/gkGameObjectSystem/include/
)

FILE(GLOB sources ./code/engine/gkInput/*.cpp ./code/engine/gkInput/source/*.cpp)
if (IOS)
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
target_link_libraries(gkInput 
    ${PROJECT_SOURCE_DIR}/code/ThirdParty/DXSDK/lib/x64/dinput8.lib
    ${PROJECT_SOURCE_DIR}/code/ThirdParty/DXSDK/lib/x64/dxguid
)

FILE(GLOB sources ./code/engine/gkCommon/*.cpp ./code/engine/gkSystem/*.cpp ./code/engine/gkSystem/*/*.cpp ./code/engine/gkSystem/*/*.c)
add_library(gkSystem ${CoreLinkType} ${sources})
target_include_directories(gkSystem PRIVATE
  ./code/engine/gkSystem/
  ./code/engine/gkSystem/include/
)
target_link_libraries(gkSystem 
    winmm.lib
)

# platfrom specific
if (IOS)

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
    ${PROJECT_SOURCE_DIR}/code/ThirdParty/DXSDK/lib/x64/d3d9
    ${PROJECT_SOURCE_DIR}/code/ThirdParty/DXSDK/lib/x64/dxerr
    ${PROJECT_SOURCE_DIR}/code/ThirdParty/DXSDK/lib/x64/dxguid
    ${PROJECT_SOURCE_DIR}/code/ThirdParty/DXSDK/lib/x64/d3dx9
    ${PROJECT_SOURCE_DIR}/code/ThirdParty/DXSDK/lib/x64/d3dx10
)

FILE(GLOB sources ./code/engine/gkLauncherUniverse/*.cpp ./code/engine/gkLauncherUniverse/*/*.cpp)
add_executable(gkLauncher ${sources})
target_include_directories(gkLauncher PRIVATE
    ./code/engine/gkLauncherUniverse/
  ./code/engine/gkLauncherUniverse/include/
)

# set all targets output dir
set_target_properties(gkCore gkLauncher gkSystem gkRendererD3D9 gkInput gkGameObjectSystem gkGameFramework gkFont
    PROPERTIES
    #ARCHIVE_OUTPUT_DIRECTORY "/home/xy/cmake_practice/lib/"
    LIBRARY_OUTPUT_DIRECTORY_RELEASE "${PROJECT_SOURCE_DIR}/exec/bin64"
    LIBRARY_OUTPUT_DIRECTORY_DEBUG "${PROJECT_SOURCE_DIR}/exec/bin64"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${PROJECT_SOURCE_DIR}/exec/bin64"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${PROJECT_SOURCE_DIR}/exec/bin64"
)

endif()


