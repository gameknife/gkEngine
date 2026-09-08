#!/bin/bash

set -euo pipefail

GKENGINE_ROOT="$(cd "$(dirname "$0")" && pwd)"
BUILD_DIR="${GKENGINE_BUILD_DIR:-$GKENGINE_ROOT/build-osx}"
APP_PATH="$BUILD_DIR/gkLauncher.app"
EXECUTABLE="$APP_PATH/Contents/MacOS/gkLauncher"

if [[ -z "${VULKAN_SDK:-}" ]]; then
	for SDK_ENV in \
		"$HOME/VulkanSDK/1.3.290.0/setup-env.sh" \
		"$HOME/VulkanSDK/1.3.280.1/setup-env.sh"; do
		if [[ -f "$SDK_ENV" ]]; then
			# shellcheck disable=SC1090
			source "$SDK_ENV"
			break
		fi
	done
fi

if [[ -z "${VULKAN_SDK:-}" || ! -x "$VULKAN_SDK/bin/glslc" ]]; then
	echo "[ERROR] Vulkan SDK not found. Run auto_cmake.sh --osx or set VULKAN_SDK." >&2
	exit 1
fi

if [[ ! -f "$BUILD_DIR/CMakeCache.txt" ]]; then
	echo "[ERROR] $BUILD_DIR is not configured. Run ./auto_cmake.sh --osx first." >&2
	exit 1
fi

# Ninja is a single-configuration generator. Keep its output equivalent to
# the Windows RelWithDebInfo build even if the build directory was configured
# before auto_cmake.sh started selecting that configuration.
if ! grep -q '^CMAKE_BUILD_TYPE:STRING=RelWithDebInfo$' "$BUILD_DIR/CMakeCache.txt"; then
	echo "Reconfiguring macOS build as RelWithDebInfo..."
	cmake -S "$GKENGINE_ROOT" -B "$BUILD_DIR" -G Ninja \
		-DCMAKE_BUILD_TYPE=RelWithDebInfo \
		-DGK_RENDERER_BACKEND=VULKAN
fi

cmake --build "$BUILD_DIR" \
	--config RelWithDebInfo \
	--target gkLauncher gkVulkanSmoke \
	--parallel

if [[ ! -x "$EXECUTABLE" ]]; then
	echo "[ERROR] gkLauncher was not produced at $EXECUTABLE." >&2
	exit 1
fi

# Stop an earlier copy launched by this script, matching auto_buildrun.bat.
OLD_PIDS="$(pgrep -f "$EXECUTABLE" || true)"
if [[ -n "$OLD_PIDS" ]]; then
	while read -r PID; do
		[[ -n "$PID" ]] && kill "$PID" 2>/dev/null || true
	done <<< "$OLD_PIDS"
fi

LOG_FILE="$BUILD_DIR/gkLauncher.log"
echo "Starting gkLauncher with conf_room..."
echo "Log: $LOG_FILE"
(
	cd "$GKENGINE_ROOT"
	nohup "$EXECUTABLE" >"$LOG_FILE" 2>&1 </dev/null &
	echo "PID: $!"
)
