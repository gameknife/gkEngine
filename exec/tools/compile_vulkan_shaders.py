#!/usr/bin/env python3
"""Incrementally compile gkENGINE Vulkan GLSL sources and write a stable manifest."""

from __future__ import annotations

import argparse
import hashlib
import json
import pathlib
import re
import subprocess
import sys
import xml.etree.ElementTree as ET


STAGES = {".vert", ".frag", ".comp", ".geom", ".tesc", ".tese"}
INCLUDE = re.compile(r'^\s*#\s*include\s*[<"]([^">]+)[">]', re.MULTILINE)


def digest(path: pathlib.Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def run(command: list[str]) -> str:
    result = subprocess.run(command, text=True, capture_output=True)
    if result.returncode:
        sys.stderr.write(result.stdout)
        sys.stderr.write(result.stderr)
        raise SystemExit(result.returncode)
    return result.stdout


def dependencies(source: pathlib.Path, roots: list[pathlib.Path]) -> list[pathlib.Path]:
    result: list[pathlib.Path] = []
    visiting: set[pathlib.Path] = set()

    def visit(path: pathlib.Path) -> None:
        path = path.resolve()
        if path in visiting:
            return
        visiting.add(path)
        result.append(path)
        text = path.read_text(encoding="utf-8")
        for name in INCLUDE.findall(text):
            candidates = [path.parent / name, *(root / name for root in roots)]
            included = next((candidate.resolve() for candidate in candidates if candidate.is_file()), None)
            if included is None:
                raise SystemExit(f"{path}: unresolved shader include {name!r}")
            visit(included)

    visit(source)
    return result


def dependency_digest(paths: list[pathlib.Path], root: pathlib.Path) -> str:
    value = hashlib.sha256()
    for path in sorted(paths):
        try:
            name = path.relative_to(root).as_posix()
        except ValueError:
            name = path.as_posix()
        value.update(name.encode("utf-8"))
        value.update(b"\0")
        value.update(path.read_bytes())
        value.update(b"\0")
    return value.hexdigest()


def validate_reflection(shader: str, reflection: dict) -> None:
    bindings: set[tuple[int, int]] = set()
    resource_groups = (
        "ubos", "ssbos", "textures", "separate_images", "separate_samplers",
        "images", "subpass_inputs", "acceleration_structures",
    )
    for group in resource_groups:
        for resource in reflection.get(group, []):
            if "binding" not in resource:
                continue
            key = (resource.get("set", 0), resource["binding"])
            if key in bindings:
                raise SystemExit(f"{shader}: duplicate descriptor set/binding {key}")
            bindings.add(key)
    locations: set[int] = set()
    for resource in reflection.get("inputs", []):
        if "location" in resource and resource["location"] in locations:
            raise SystemExit(f"{shader}: duplicate input location {resource['location']}")
        if "location" in resource:
            locations.add(resource["location"])


def validate_stage_pairs(reflections: dict[str, dict]) -> None:
    for relative, vertex in reflections.items():
        if not relative.endswith(".vert"):
            continue
        fragment_name = relative[:-5] + ".frag"
        fragment = reflections.get(fragment_name)
        if fragment is None:
            continue
        outputs = {
            value["location"]: value["type"]
            for value in vertex.get("outputs", []) if "location" in value
        }
        for value in fragment.get("inputs", []):
            if "location" not in value:
                continue
            actual = outputs.get(value["location"])
            if actual != value["type"]:
                raise SystemExit(
                    f"{relative} -> {fragment_name}: location {value['location']} "
                    f"type mismatch ({actual!r} != {value['type']!r})"
                )


def validate_gfx(
    gfx_root: pathlib.Path, required: set[str]
) -> dict[str, str]:
    hashes: dict[str, str] = {}
    found: set[str] = set()
    exec_root = gfx_root.resolve().parents[2]
    for path in sorted(gfx_root.rglob("*.gfx")):
        try:
            root = ET.parse(path).getroot()
        except ET.ParseError as error:
            raise SystemExit(f"{path}: invalid GFX XML: {error}") from error
        name = path.stem.lower()
        hashes[path.relative_to(gfx_root).as_posix()] = digest(path)
        vulkan = root.find("VulkanShader")
        if vulkan is None:
            continue
        found.add(name)
        for stage in ("VSShader", "PSShader"):
            node = vulkan.find(stage)
            filename = node.get("FileName") if node is not None else None
            if not filename:
                raise SystemExit(f"{path}: VulkanShader is missing {stage}/FileName")
            source = exec_root / filename
            if not source.is_file():
                raise SystemExit(f"{path}: Vulkan shader source does not exist: {source}")
    missing = required - found
    if missing:
        raise SystemExit(
            "required GFX files have no VulkanShader mapping: "
            + ", ".join(sorted(missing))
        )
    return hashes


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--source", type=pathlib.Path, required=True)
    parser.add_argument("--output", type=pathlib.Path, required=True)
    parser.add_argument("--glslc", required=True)
    parser.add_argument("--spirv-val")
    parser.add_argument("--spirv-cross")
    parser.add_argument("-I", "--include", action="append", type=pathlib.Path, default=[])
    parser.add_argument("--gfx-root", type=pathlib.Path)
    parser.add_argument("--required-gfx", action="append", default=[])
    parser.add_argument("--target-env", default="vulkan1.2")
    args = parser.parse_args()

    source_root = args.source.resolve()
    include_roots = [source_root, *(path.resolve() for path in args.include)]
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
    reflections: dict[str, dict] = {}
    output_owners: dict[str, str] = {}
    for source in sorted(path for path in source_root.rglob("*") if path.suffix in STAGES):
        relative = source.relative_to(source_root).as_posix()
        shader_dependencies = dependencies(source, include_roots)
        source_hash = dependency_digest(shader_dependencies, source_root)
        output = output_root / ("gk_vk_" + source.name + ".spv")
        owner = output_owners.setdefault(output.name, relative)
        if owner != relative:
            raise SystemExit(
                f"shader output collision: {owner} and {relative} both map to {output.name}"
            )
        cached = previous.get(relative, {})
        if cached.get("sha256") != source_hash or not output.exists():
            command = [
                args.glslc,
                f"--target-env={args.target_env}",
                "-o",
                str(output),
                str(source),
            ]
            for include_root in include_roots:
                command.extend(["-I", str(include_root)])
            run(command)
        if args.spirv_val:
            run([args.spirv_val, "--target-env", args.target_env, str(output)])
        reflection_hash = ""
        if args.spirv_cross:
            reflection_text = run([args.spirv_cross, str(output), "--reflect"])
            reflection = json.loads(reflection_text)
            validate_reflection(relative, reflection)
            reflections[relative] = reflection
            reflection_hash = hashlib.sha256(
                json.dumps(reflection, sort_keys=True).encode("utf-8")
            ).hexdigest()
        shaders[relative] = {
            "sha256": source_hash,
            "output": output.name,
            "stage": source.suffix[1:],
            "dependencies": [
                path.relative_to(source_root).as_posix()
                if path.is_relative_to(source_root) else path.as_posix()
                for path in shader_dependencies
            ],
            "reflection_sha256": reflection_hash,
        }

    validate_stage_pairs(reflections)
    gfx_hashes = {}
    if args.gfx_root:
        gfx_hashes = validate_gfx(
            args.gfx_root.resolve(),
            {name.lower() for name in args.required_gfx},
        )
    manifest = {
        "schema": 2,
        "target_env": args.target_env,
        "shaders": shaders,
        "gfx": gfx_hashes,
    }
    manifest_path.write_text(
        json.dumps(manifest, indent=2, sort_keys=True) + "\n", encoding="utf-8"
    )
    print(f"compiled/validated {len(shaders)} Vulkan shaders")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
