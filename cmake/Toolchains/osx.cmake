# Options:
#
# IOS_PLATFORM = OS (default)
#   This needs to be OS as the simulator cannot use the required GLES2.0 environment.
#   OS - the default, used to build for iPhone and iPad physical devices, which have an arm arch.
#
# CMAKE_IOS_SDK_ROOT = automatic(default) or /path/to/platform/Developer/SDKs/SDK folder
#   By default this location is automatcially chosen based on the CMAKE_IOS_DEVELOPER_ROOT value.
#   In this case it will always be the most up-to-date SDK found in the CMAKE_IOS_DEVELOPER_ROOT path.
#   If set manually, this will force the use of a specific SDK version

add_definitions(
  -D_STATIC_LIB
  -U__STRICT_ANSI__
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
set(CMAKE_CXX_FLAGS "-x objective-c++")
set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")

# Standard settings
set(OSX ON)
set(APPLE ON)

set(CMAKE_SYSTEM_NAME Darwin)
set(CMAKE_SYSTEM_VERSION 1)
#set(CMAKE_SYSTEM_PROCESSOR armv7)
set(CMAKE_CROSSCOMPILING ON)
set(CMAKE_MACOSX_BUNDLE YES)
#set(CMAKE_XCODE_ATTRIBUTE_CODE_SIGNING_REQUIRED "NO")
#set(CMAKE_XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET ${IPHONEOS_DEPLOYMENT_TARGET})


