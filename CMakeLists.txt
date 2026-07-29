cmake_minimum_required(VERSION 3.10)
project (gkENGINE)
include(CTest)

set(GK_RENDERER_BACKEND "AUTO" CACHE STRING "Renderer backend: AUTO;D3D9;GL330;GLES2;VULKAN")
set_property(CACHE GK_RENDERER_BACKEND PROPERTY STRINGS AUTO D3D9 GL330 GLES2 VULKAN)
option(GK_VULKAN_VALIDATION "Enable Vulkan validation layers in development builds" ON)
option(GK_VULKAN_BUNDLED_MOLTENVK "Use a bundled MoltenVK runtime on Apple" OFF)

if (APPLE)
    message(STATUS "This is build for ios, use static lib")
	set(CoreLinkType STATIC)
else()
    message(STATUS "This is build for windows, use dynamic lib")
    set(CoreLinkType SHARED)

    if (WIN32)
        add_compile_definitions(DIRECTINPUT_VERSION=0x0800)
    endif()

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
    add_compile_options(/w44566)
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
set(GK_FREETYPE_ROOT ${PROJECT_SOURCE_DIR}/code/ThirdParty/freetype)
add_library(gkFreetype STATIC
    ${GK_FREETYPE_ROOT}/src/autofit/autofit.c
    ${GK_FREETYPE_ROOT}/src/bdf/bdf.c
    ${GK_FREETYPE_ROOT}/src/cff/cff.c
    ${GK_FREETYPE_ROOT}/src/base/ftbase.c
    ${GK_FREETYPE_ROOT}/src/base/ftbitmap.c
    ${GK_FREETYPE_ROOT}/src/cache/ftcache.c
    ${GK_FREETYPE_ROOT}/builds/win32/ftdebug.c
    ${GK_FREETYPE_ROOT}/src/base/ftfstype.c
    ${GK_FREETYPE_ROOT}/src/base/ftgasp.c
    ${GK_FREETYPE_ROOT}/src/base/ftglyph.c
    ${GK_FREETYPE_ROOT}/src/gzip/ftgzip.c
    ${GK_FREETYPE_ROOT}/src/base/ftinit.c
    ${GK_FREETYPE_ROOT}/src/lzw/ftlzw.c
    ${GK_FREETYPE_ROOT}/src/base/ftstroke.c
    ${GK_FREETYPE_ROOT}/src/base/ftsystem.c
    ${GK_FREETYPE_ROOT}/src/smooth/smooth.c
    ${GK_FREETYPE_ROOT}/src/base/ftbbox.c
    ${GK_FREETYPE_ROOT}/src/base/ftgxval.c
    ${GK_FREETYPE_ROOT}/src/base/ftlcdfil.c
    ${GK_FREETYPE_ROOT}/src/base/ftmm.c
    ${GK_FREETYPE_ROOT}/src/base/ftotval.c
    ${GK_FREETYPE_ROOT}/src/base/ftpatent.c
    ${GK_FREETYPE_ROOT}/src/base/ftpfr.c
    ${GK_FREETYPE_ROOT}/src/base/ftsynth.c
    ${GK_FREETYPE_ROOT}/src/base/fttype1.c
    ${GK_FREETYPE_ROOT}/src/base/ftwinfnt.c
    ${GK_FREETYPE_ROOT}/src/base/ftxf86.c
    ${GK_FREETYPE_ROOT}/src/pcf/pcf.c
    ${GK_FREETYPE_ROOT}/src/pfr/pfr.c
    ${GK_FREETYPE_ROOT}/src/psaux/psaux.c
    ${GK_FREETYPE_ROOT}/src/pshinter/pshinter.c
    ${GK_FREETYPE_ROOT}/src/psnames/psmodule.c
    ${GK_FREETYPE_ROOT}/src/raster/raster.c
    ${GK_FREETYPE_ROOT}/src/sfnt/sfnt.c
    ${GK_FREETYPE_ROOT}/src/truetype/truetype.c
    ${GK_FREETYPE_ROOT}/src/type1/type1.c
    ${GK_FREETYPE_ROOT}/src/cid/type1cid.c
    ${GK_FREETYPE_ROOT}/src/type42/type42.c
    ${GK_FREETYPE_ROOT}/src/winfonts/winfnt.c
)
target_include_directories(gkFreetype PUBLIC ${GK_FREETYPE_ROOT}/include)
target_compile_definitions(gkFreetype PRIVATE
    FT2_BUILD_LIBRARY _CRT_SECURE_NO_WARNINGS)
target_link_libraries(gkFont gkFreetype)
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
FILE(GLOB TESTCASE_SOURCES ./code/engine/gkTestApps/TestCases/*.cpp)
add_library(Testcases ${CoreLinkType} ${TESTCASE_SOURCES})
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

if (WIN32)
    add_custom_command(TARGET gkRendererD3D9 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${PROJECT_SOURCE_DIR}/code/ThirdParty/DXSDK/bin/x64/D3DX9_43.dll"
            "$<TARGET_FILE_DIR:gkRendererD3D9>/D3DX9_43.dll"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${PROJECT_SOURCE_DIR}/code/ThirdParty/DXSDK/bin/x64/D3DCompiler_43.dll"
            "$<TARGET_FILE_DIR:gkRendererD3D9>/D3DCompiler_43.dll"
    )
endif()

set(GK_WINDOWS_RENDERER_TARGETS gkRendererD3D9)
if (WIN32)
    if (GK_RENDERER_BACKEND STREQUAL "VULKAN")
        find_package(Vulkan REQUIRED)
    else()
        find_package(Vulkan QUIET)
    endif()

    if (Vulkan_FOUND)
        set(GK_VULKAN_SHADER_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/vulkan-shaders")
        set(GK_VULKAN_ACCEPTANCE_VERT "${GK_VULKAN_SHADER_OUTPUT_DIR}/gk_vk_acceptance.vert.spv")
        set(GK_VULKAN_ACCEPTANCE_FRAG "${GK_VULKAN_SHADER_OUTPUT_DIR}/gk_vk_acceptance.frag.spv")
        set(GK_VULKAN_BASIC_VERT "${GK_VULKAN_SHADER_OUTPUT_DIR}/gk_vk_basic.vert.spv")
        set(GK_VULKAN_BASIC_FRAG "${GK_VULKAN_SHADER_OUTPUT_DIR}/gk_vk_basic.frag.spv")
        set(GK_VULKAN_LIT_VERT "${GK_VULKAN_SHADER_OUTPUT_DIR}/gk_vk_lit.vert.spv")
        set(GK_VULKAN_LIT_FRAG "${GK_VULKAN_SHADER_OUTPUT_DIR}/gk_vk_lit.frag.spv")
        find_program(GK_SPIRV_VAL_EXECUTABLE spirv-val
            HINTS "$ENV{VULKAN_SDK}/Bin"
        )
        find_program(GK_SPIRV_CROSS_EXECUTABLE spirv-cross
            HINTS "$ENV{VULKAN_SDK}/Bin"
        )
        find_package(Python3 COMPONENTS Interpreter REQUIRED)
        if (GK_SPIRV_VAL_EXECUTABLE)
            set(GK_SPIRV_VAL_ARGUMENT --spirv-val "${GK_SPIRV_VAL_EXECUTABLE}")
        endif()
        if (GK_SPIRV_CROSS_EXECUTABLE)
            set(GK_SPIRV_CROSS_ARGUMENT --spirv-cross "${GK_SPIRV_CROSS_EXECUTABLE}")
        endif()
        add_custom_command(
            OUTPUT "${GK_VULKAN_ACCEPTANCE_VERT}" "${GK_VULKAN_ACCEPTANCE_FRAG}"
                "${GK_VULKAN_BASIC_VERT}" "${GK_VULKAN_BASIC_FRAG}"
                "${GK_VULKAN_LIT_VERT}" "${GK_VULKAN_LIT_FRAG}"
            COMMAND ${CMAKE_COMMAND} -E make_directory "${GK_VULKAN_SHADER_OUTPUT_DIR}"
            COMMAND "${Python3_EXECUTABLE}"
                "${PROJECT_SOURCE_DIR}/exec/tools/compile_vulkan_shaders.py"
                --source "${PROJECT_SOURCE_DIR}/exec/engine/shaders/vulkan/source"
                --output "${GK_VULKAN_SHADER_OUTPUT_DIR}"
                --glslc "${Vulkan_GLSLC_EXECUTABLE}"
                --target-env vulkan1.2
                --gfx-root "${PROJECT_SOURCE_DIR}/exec/engine/shaders/template"
                --required-gfx ksbase
                --required-gfx kssimple
                --required-gfx ks_skyhdr
                --required-gfx ksbaseskinned
                --required-gfx terrian
                --required-gfx vegetation
                --required-gfx vegetation_autoexpand
                --required-gfx grass
                ${GK_SPIRV_VAL_ARGUMENT}
                ${GK_SPIRV_CROSS_ARGUMENT}
            DEPENDS
                "${PROJECT_SOURCE_DIR}/exec/tools/compile_vulkan_shaders.py"
                "${PROJECT_SOURCE_DIR}/exec/engine/shaders/vulkan/source/debug/acceptance.vert"
                "${PROJECT_SOURCE_DIR}/exec/engine/shaders/vulkan/source/debug/acceptance.frag"
                "${PROJECT_SOURCE_DIR}/exec/engine/shaders/vulkan/source/scene/basic.vert"
                "${PROJECT_SOURCE_DIR}/exec/engine/shaders/vulkan/source/scene/basic.frag"
                "${PROJECT_SOURCE_DIR}/exec/engine/shaders/vulkan/source/scene/lit.vert"
                "${PROJECT_SOURCE_DIR}/exec/engine/shaders/vulkan/source/scene/lit.frag"
                "${PROJECT_SOURCE_DIR}/exec/engine/shaders/template/built_in/ksbase.gfx"
                "${PROJECT_SOURCE_DIR}/exec/engine/shaders/template/built_in/kssimple.gfx"
                "${PROJECT_SOURCE_DIR}/exec/engine/shaders/template/built_in/ks_skyhdr.gfx"
                "${PROJECT_SOURCE_DIR}/exec/engine/shaders/template/built_in/ksbaseskinned.gfx"
                "${PROJECT_SOURCE_DIR}/exec/engine/shaders/template/built_in/terrian.gfx"
                "${PROJECT_SOURCE_DIR}/exec/engine/shaders/template/built_in/vegetation.gfx"
                "${PROJECT_SOURCE_DIR}/exec/engine/shaders/template/built_in/vegetation_autoexpand.gfx"
                "${PROJECT_SOURCE_DIR}/exec/engine/shaders/template/built_in/grass.gfx"
            VERBATIM
        )
        add_custom_target(gkVulkanShaders
            DEPENDS "${GK_VULKAN_ACCEPTANCE_VERT}" "${GK_VULKAN_ACCEPTANCE_FRAG}"
                "${GK_VULKAN_BASIC_VERT}" "${GK_VULKAN_BASIC_FRAG}"
                "${GK_VULKAN_LIT_VERT}" "${GK_VULKAN_LIT_FRAG}"
        )
        add_custom_target(gkVulkanRuntimeShaders
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${GK_VULKAN_ACCEPTANCE_VERT}" "${PROJECT_SOURCE_DIR}/exec/bin64/gk_vk_acceptance.vert.spv"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${GK_VULKAN_ACCEPTANCE_FRAG}" "${PROJECT_SOURCE_DIR}/exec/bin64/gk_vk_acceptance.frag.spv"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${GK_VULKAN_BASIC_VERT}" "${PROJECT_SOURCE_DIR}/exec/bin64/gk_vk_basic.vert.spv"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${GK_VULKAN_BASIC_FRAG}" "${PROJECT_SOURCE_DIR}/exec/bin64/gk_vk_basic.frag.spv"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${GK_VULKAN_LIT_VERT}" "${PROJECT_SOURCE_DIR}/exec/bin64/gk_vk_lit.vert.spv"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${GK_VULKAN_LIT_FRAG}" "${PROJECT_SOURCE_DIR}/exec/bin64/gk_vk_lit.frag.spv"
            DEPENDS gkVulkanShaders
        )

        FILE(GLOB_RECURSE VULKAN_RENDERER_SOURCES CONFIGURE_DEPENDS
            ./code/engine/gkRendererVulkan/*.cpp
        )
        list(FILTER VULKAN_RENDERER_SOURCES EXCLUDE REGEX "/tests/")
        add_library(gkRendererVulkan ${CoreLinkType} ${VULKAN_RENDERER_SOURCES})
        target_include_directories(gkRendererVulkan PRIVATE
            ./code/engine/gkRendererVulkan/include/
            ./code/engine/gkRendererVulkan/Vulkan/
            ./code/engine/gkRendererVulkan/RenderRes/
            ./code/engine/gkRendererVulkan/Platform/Win32/
        )
        target_compile_definitions(gkRendererVulkan PRIVATE
            RENDERER_VULKAN
            VK_USE_PLATFORM_WIN32_KHR
            $<$<BOOL:${GK_VULKAN_VALIDATION}>:GK_VULKAN_VALIDATION=1>
        )
        target_link_libraries(gkRendererVulkan PRIVATE Vulkan::Vulkan)
        add_dependencies(gkRendererVulkan gkVulkanRuntimeShaders)
        add_custom_command(TARGET gkRendererVulkan POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${GK_VULKAN_ACCEPTANCE_VERT}" "$<TARGET_FILE_DIR:gkRendererVulkan>/gk_vk_acceptance.vert.spv"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${GK_VULKAN_ACCEPTANCE_FRAG}" "$<TARGET_FILE_DIR:gkRendererVulkan>/gk_vk_acceptance.frag.spv"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${GK_VULKAN_BASIC_VERT}" "$<TARGET_FILE_DIR:gkRendererVulkan>/gk_vk_basic.vert.spv"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${GK_VULKAN_BASIC_FRAG}" "$<TARGET_FILE_DIR:gkRendererVulkan>/gk_vk_basic.frag.spv"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${GK_VULKAN_LIT_VERT}" "$<TARGET_FILE_DIR:gkRendererVulkan>/gk_vk_lit.vert.spv"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${GK_VULKAN_LIT_FRAG}" "$<TARGET_FILE_DIR:gkRendererVulkan>/gk_vk_lit.frag.spv"
        )
        list(APPEND GK_WINDOWS_RENDERER_TARGETS gkRendererVulkan)

        if (BUILD_TESTING)
            add_executable(gkVulkanSmoke
                ./code/engine/gkRendererVulkan/tests/gkVulkanSmoke.cpp
                ./code/engine/gkRendererVulkan/Vulkan/gkVkDeviceContext.cpp
            )
            target_include_directories(gkVulkanSmoke PRIVATE
                ./code/engine/gkRendererVulkan/Vulkan/
            )
            target_compile_definitions(gkVulkanSmoke PRIVATE
                RENDERER_VULKAN
                VK_USE_PLATFORM_WIN32_KHR
                $<$<BOOL:${GK_VULKAN_VALIDATION}>:GK_VULKAN_VALIDATION=1>
            )
            target_link_libraries(gkVulkanSmoke PRIVATE Vulkan::Vulkan)
            set_target_properties(gkVulkanSmoke PROPERTIES
                RUNTIME_OUTPUT_DIRECTORY_RELEASE "${PROJECT_SOURCE_DIR}/exec/bin64"
                RUNTIME_OUTPUT_DIRECTORY_DEBUG "${PROJECT_SOURCE_DIR}/exec/bin64"
                RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${PROJECT_SOURCE_DIR}/exec/bin64"
                RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${PROJECT_SOURCE_DIR}/exec/bin64"
            )
            add_dependencies(gkVulkanSmoke gkVulkanRuntimeShaders)
            add_custom_command(TARGET gkVulkanSmoke POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${GK_VULKAN_ACCEPTANCE_VERT}" "$<TARGET_FILE_DIR:gkVulkanSmoke>/gk_vk_acceptance.vert.spv"
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${GK_VULKAN_ACCEPTANCE_FRAG}" "$<TARGET_FILE_DIR:gkVulkanSmoke>/gk_vk_acceptance.frag.spv"
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${GK_VULKAN_BASIC_VERT}" "$<TARGET_FILE_DIR:gkVulkanSmoke>/gk_vk_basic.vert.spv"
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${GK_VULKAN_BASIC_FRAG}" "$<TARGET_FILE_DIR:gkVulkanSmoke>/gk_vk_basic.frag.spv"
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${GK_VULKAN_LIT_VERT}" "$<TARGET_FILE_DIR:gkVulkanSmoke>/gk_vk_lit.vert.spv"
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${GK_VULKAN_LIT_FRAG}" "$<TARGET_FILE_DIR:gkVulkanSmoke>/gk_vk_lit.frag.spv"
            )
            add_test(NAME gkVulkanSmoke COMMAND gkVulkanSmoke)
            set_tests_properties(gkVulkanSmoke PROPERTIES TIMEOUT 30)
        endif()
    elseif (GK_RENDERER_BACKEND STREQUAL "AUTO")
        message(STATUS "Vulkan SDK not found; gkRendererVulkan will not be built")
    endif()
endif()

FILE(GLOB sources ./code/engine/gkLauncherUniverse/*.cpp ./code/engine/gkLauncherUniverse/*/*.cpp)
add_executable(gkLauncher ${sources})
target_include_directories(gkLauncher PRIVATE
    ./code/engine/gkLauncherUniverse/
  ./code/engine/gkLauncherUniverse/include/
)

# set all targets output dir
set_target_properties(gkCore gkLauncher gkSystem ${GK_WINDOWS_RENDERER_TARGETS} gkInput gkGameObjectSystem gkGameFramework gkFont Testcases
    PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY_RELEASE "${PROJECT_SOURCE_DIR}/exec/bin64"
    LIBRARY_OUTPUT_DIRECTORY_DEBUG "${PROJECT_SOURCE_DIR}/exec/bin64"
    LIBRARY_OUTPUT_DIRECTORY_MINSIZEREL "${PROJECT_SOURCE_DIR}/exec/bin64"
    LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO "${PROJECT_SOURCE_DIR}/exec/bin64"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${PROJECT_SOURCE_DIR}/exec/bin64"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${PROJECT_SOURCE_DIR}/exec/bin64"
    RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${PROJECT_SOURCE_DIR}/exec/bin64"
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${PROJECT_SOURCE_DIR}/exec/bin64"
)

endif()


