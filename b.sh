#!/bin/bash
CMAKE=1

# Check arguments
while test $# -gt 0
do
	case "$1" in
		--ios) CMAKE_ARGS="-G Xcode -DCMAKE_TOOLCHAIN_FILE=cmake/Toolchains/ios.cmake ${CMAKE_ARGS}"
			TARGET_OS=iOS
			;;
		--osx) CMAKE_ARGS="-G Xcode -DCMAKE_TOOLCHAIN_FILE=cmake/Toolchains/osx.cmake ${CMAKE_ARGS}"
			TARGET_OS=OSX
			;;
		*) MAKE_OPT="$1 ${MAKE_OPT}"
			;;
	esac
	shift
done

echo "Building for $TARGET_OS"

if [ ! -z "$TARGET_OS" ]; then
	echo "Building for $TARGET_OS"
	BUILD_DIR="$(tr [A-Z] [a-z] <<< build-"$TARGET_OS")"
else
	echo "Building for native host."
	BUILD_DIR="build"
fi

# Strict errors. Any non-zero return exits this script
set -e

mkdir -p ${BUILD_DIR}
pushd ${BUILD_DIR}

cmake $CMAKE_ARGS ..

#make -j4 $MAKE_OPT
popd
