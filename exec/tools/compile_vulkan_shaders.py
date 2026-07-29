#!/usr/bin/env python3
"""Incrementally compile gkENGINE Vulkan GLSL sources and write a stable manifest."""

from __future__ import annotations

import argparse
import hashlib
import json
import pathlib
import subprocess
import sys


STAGES = {".vert", ".frag", ".comp", ".geom", ".tesc", ".tese"}


def digest(path: pathlib.Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def run(command: list[str]) -> None:
    result = subprocess.run(command, text=True, capture_output=True)
    if result.returncode:
        sys.stderr.write(result.stdout)
        sys.stderr.write(result.stderr)
        raise SystemExit(result.returncode)


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--source", type=pathlib.Path, required=True)
    parser.add_argument("--output", type=pathlib.Path, required=True)
    parser.add_argument("--glslc", required=True)
    parser.add_argument("--spirv-val")
    parser.add_argument("--target-env", default="vulkan1.2")
    args = parser.parse_args()

    source_root = args.source.resolve()
    output_root = args.output.resolve()
    output_root.mkdir(parents=True, exist_ok=True)
    manifest_path = output_root / "manifest.json"
    previous = {}
    if manifest_path.exists():
        try:
            previous = json.loads(manifest_path.read_text(encoding="utf-8")).get("shaders", {})
        except (OSError, ValueError):
            previous = {}

    shaders: dict[str, dict[str, str]] = {}
    for source in sorted(path for path in source_root.rglob("*") if path.suffix in STAGES):
        relative = source.relative_to(source_root).as_posix()
        source_hash = digest(source)
        output = output_root / (relative.replace("/", "_") + ".spv")
        cached = previous.get(relative, {})
        if cached.get("sha256") != source_hash or not output.exists():
            run([
                args.glslc,
                f"--target-env={args.target_env}",
                "-o",
                str(output),
                str(source),
            ])
        if args.spirv_val:
            run([args.spirv_val, "--target-env", args.target_env, str(output)])
        shaders[relative] = {
            "sha256": source_hash,
            "output": output.name,
            "stage": source.suffix[1:],
        }

    manifest = {
        "schema": 1,
        "target_env": args.target_env,
        "shaders": shaders,
    }
    manifest_path.write_text(
        json.dumps(manifest, indent=2, sort_keys=True) + "\n", encoding="utf-8"
    )
    print(f"compiled/validated {len(shaders)} Vulkan shaders")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
