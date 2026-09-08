#!/bin/bash
CMAKE=1

# Check arguments
while test $# -gt 0
do
	case "$1" in
		--ios) CMAKE_ARGS="-DCMAKE_TOOLCHAIN_FILE=cmake/Toolchains/ios.cmake ${CMAKE_ARGS}"
			GENERATOR="Xcode"
			TARGET_OS=iOS
			;;
		--osx) CMAKE_ARGS="-DCMAKE_BUILD_TYPE=RelWithDebInfo -DGK_RENDERER_BACKEND=VULKAN ${CMAKE_ARGS}"
			GENERATOR="Ninja"
			TARGET_OS=OSX
			;;
		--win64) CMAKE_ARGS="${CMAKE_ARGS}"
			GENERATOR="Visual Studio 15 2017 Win64"
			TARGET_OS=WIN64
			;;
		*) MAKE_OPT="$1 ${MAKE_OPT}"
			;;
	esac
	shift
done

echo "Building for $TARGET_OS"

if [ ! -z "$TARGET_OS" ]; then
	# echo "Building for $TARGET_OS"
	BUILD_DIR="$(tr [A-Z] [a-z] <<< build-"$TARGET_OS")"
else
	# echo "Building for native host."
	BUILD_DIR="build"
fi

# Strict errors. Any non-zero return exits this script
set -e

if [ "$TARGET_OS" = "OSX" ]; then
	# LunarG's macOS SDK is installed under $HOME/VulkanSDK when the
	# installer is run without system-wide installation. Load it here so
	# auto_cmake.sh works from a fresh shell as well.
	if [ -z "${VULKAN_SDK:-}" ]; then
		for SDK_ENV in \
			"$HOME/VulkanSDK/1.3.290.0/setup-env.sh" \
			"$HOME/VulkanSDK/1.3.280.1/setup-env.sh"; do
			if [ -f "$SDK_ENV" ]; then
				# shellcheck disable=SC1090
				source "$SDK_ENV"
				break
			fi
		done
	fi
	if [ -z "${VULKAN_SDK:-}" ] || [ ! -x "$VULKAN_SDK/bin/glslc" ]; then
		echo "Vulkan SDK not found. Set VULKAN_SDK to a macOS SDK directory." >&2
		exit 1
	fi
	"$(cd "$(dirname "$0")" && pwd)/deploy_macos_assets.sh"
fi

mkdir -p ${BUILD_DIR}
pushd ${BUILD_DIR}
cmake -G "$GENERATOR" $CMAKE_ARGS ..

#make -j4 $MAKE_OPT
popd
