#!/bin/bash

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
cd "$SCRIPT_DIR"

git submodule update --init --depth 1 mediapack/base mediapack/confroom

ARCHIVER="$SCRIPT_DIR/exec/tools/7zr"
if [[ ! -x "$ARCHIVER" ]]; then
	if command -v 7zz >/dev/null 2>&1; then
		ARCHIVER="$(command -v 7zz)"
	elif command -v 7z >/dev/null 2>&1; then
		ARCHIVER="$(command -v 7z)"
	else
		echo "Unable to find 7zr, 7zz, or 7z for media extraction." >&2
		exit 1
	fi
fi

mkdir -p "$SCRIPT_DIR/exec/media/config" "$SCRIPT_DIR/exec/paks"
cp "$SCRIPT_DIR/exec/tools/default_cfg/startup.cfg" \
	"$SCRIPT_DIR/exec/media/config/startup.cfg"

extract_pack()
{
	local archive="$1"
	if [[ ! -f "$archive" ]]; then
		echo "Missing media archive: $archive" >&2
		exit 1
	fi
	"$ARCHIVER" x -y "-o$SCRIPT_DIR/exec/media" "$archive"
}

extract_pack "$SCRIPT_DIR/mediapack/base/basic.7z"
extract_pack "$SCRIPT_DIR/mediapack/base/textures.7z"
extract_pack "$SCRIPT_DIR/mediapack/confroom/conf_room.7z"

echo "macOS assets deployed to $SCRIPT_DIR/exec/media"
