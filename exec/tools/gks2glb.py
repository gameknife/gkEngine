#!/usr/bin/env python3
"""Export a gkENGINE .gks scene to a self-contained binary glTF (.glb).

The exporter intentionally does not initialize the renderer.  A gks file is
XML, the referenced meshes are the engine's OBJ files, and the engine's mtl
files are XML material descriptions, so all three are handled here.  Images
are converted from TGA to PNG and embedded in the resulting GLB.

The gkENGINE coordinate system is Z-up.  glTF is Y-up, so positions, normals,
and node transforms are converted with a -90 degree rotation around X.  The
engine render camera uses local +Y as its view axis, which maps to glTF's local
-Z camera axis through the same world conversion.
"""

from __future__ import annotations

import argparse
import json
import math
import struct
import sys
import zlib
import xml.etree.ElementTree as ET

from collections import OrderedDict
from dataclasses import dataclass, field
from pathlib import Path
from typing import Dict, Iterable, List, Optional, Sequence, Tuple


Vec2 = Tuple[float, float]
Vec3 = Tuple[float, float, float]
Quat = Tuple[float, float, float, float]


class ExportError(RuntimeError):
    pass


def warn(message: str) -> None:
    print("gks2glb: warning: " + message, file=sys.stderr)


def parse_floats(value: Optional[str], count: int, default: Sequence[float]) -> Tuple[float, ...]:
    if not value:
        return tuple(default[:count])
    try:
        values = [float(item) for item in value.replace(",", " ").split()]
    except ValueError:
        return tuple(default[:count])
    if len(values) < count:
        return tuple(default[:count])
    return tuple(values[:count])


def clamp(value: float, low: float, high: float) -> float:
    return max(low, min(high, value))


def gk_to_gltf(value: Vec3) -> Vec3:
    """Convert a Z-up gkENGINE vector to a Y-up glTF vector."""
    return value[0], value[2], -value[1]


def normalize(value: Vec3) -> Vec3:
    length = math.sqrt(value[0] * value[0] + value[1] * value[1] + value[2] * value[2])
    if length <= 1.0e-12:
        return 0.0, 0.0, 1.0
    return value[0] / length, value[1] / length, value[2] / length


def cross(lhs: Vec3, rhs: Vec3) -> Vec3:
    return (
        lhs[1] * rhs[2] - lhs[2] * rhs[1],
        lhs[2] * rhs[0] - lhs[0] * rhs[2],
        lhs[0] * rhs[1] - lhs[1] * rhs[0],
    )


def subtract(lhs: Vec3, rhs: Vec3) -> Vec3:
    return lhs[0] - rhs[0], lhs[1] - rhs[1], lhs[2] - rhs[2]


def quat_multiply(lhs: Quat, rhs: Quat) -> Quat:
    lx, ly, lz, lw = lhs
    rx, ry, rz, rw = rhs
    return (
        lw * rx + lx * rw + ly * rz - lz * ry,
        lw * ry - lx * rz + ly * rw + lz * rx,
        lw * rz + lx * ry - ly * rx + lz * rw,
        lw * rw - lx * rx - ly * ry - lz * rz,
    )


def normalize_quat(value: Quat) -> Quat:
    length = math.sqrt(sum(component * component for component in value))
    if length <= 1.0e-12:
        return 0.0, 0.0, 0.0, 1.0
    return tuple(component / length for component in value)  # type: ignore[return-value]


def gk_quat_to_gltf(value: Quat) -> Quat:
    # C = Rx(-90deg), matching p_gltf = (x, z, -y).
    half = math.sqrt(0.5)
    basis = (-half, 0.0, 0.0, half)
    inverse = (half, 0.0, 0.0, half)
    return normalize_quat(quat_multiply(quat_multiply(basis, value), inverse))


def gk_camera_quat_to_gltf(value: Quat) -> Quat:
    """Convert a gkENGINE camera orientation to the glTF camera convention."""
    # C maps engine +Y (the render camera's view axis) to glTF -Z, and engine
    # +Z (up) to glTF +Y, so no additional local rotation is needed.
    return gk_quat_to_gltf(value)


def resolve_media_root(gks_path: Path, explicit_root: Optional[Path]) -> Path:
    if explicit_root:
        root = explicit_root.resolve()
        if not root.is_dir():
            raise ExportError("media root does not exist: {}".format(root))
        return root

    for parent in [gks_path.parent] + list(gks_path.parents):
        if (parent / "objects").is_dir() and ((parent / "level").is_dir() or (parent.parent / "engine").is_dir()):
            return parent.resolve()
    raise ExportError(
        "could not find media root above {}; pass --media-root explicitly".format(gks_path)
    )


def resolve_asset(media_root: Path, value: Optional[str]) -> Optional[Path]:
    if not value:
        return None
    normalized = value.strip().replace("\\", "/")
    if not normalized:
        return None

    # gkENGINE paths use a leading slash for a path relative to exec/media.
    if len(normalized) >= 2 and normalized[1] == ":":
        candidate = Path(normalized)
    else:
        candidate = media_root / normalized.lstrip("/")
    return candidate.resolve()


@dataclass
class MaterialDef:
    name: str
    base_color: Tuple[float, float, float, float] = (0.8, 0.8, 0.8, 1.0)
    specular: Tuple[float, float, float] = (0.0, 0.0, 0.0)
    gloss: float = 25.0
    opacity: float = 1.0
    base_texture: Optional[Path] = None
    normal_texture: Optional[Path] = None
    normal_scale: float = 1.0
    uv_params: Optional[Tuple[float, float, float, float]] = None
    effect: str = ""
    mask: int = 0


def parse_material_file(path: Optional[Path], media_root: Path) -> Dict[str, MaterialDef]:
    if not path:
        return {}
    if not path.is_file():
        warn("material file not found: {}".format(path))
        return {}

    try:
        root = ET.parse(str(path)).getroot()
    except (OSError, ET.ParseError) as error:
        warn("cannot read material file {}: {}".format(path, error))
        return {}

    materials: Dict[str, MaterialDef] = {}
    for element in root.findall("./SubMaterial"):
        name = element.attrib.get("Name", "").strip()
        if not name:
            continue
        effect = element.find("./Effect")
        diffuse_param = element.find("./Effect/Param[@name='g_MatDiffuse']")
        specular_param = element.find("./Effect/Param[@name='g_MatSpecular']")
        gloss_param = element.find("./Effect/Param[@name='g_Gloss']")
        opacity_param = element.find("./Effect/Param[@name='g_Opacity']")

        diffuse = parse_floats(
            diffuse_param.attrib.get("value") if diffuse_param is not None else None,
            4,
            (0.8, 0.8, 0.8, 1.0),
        )
        specular = parse_floats(
            specular_param.attrib.get("value") if specular_param is not None else None,
            3,
            (0.0, 0.0, 0.0),
        )
        gloss = parse_floats(
            gloss_param.attrib.get("value") if gloss_param is not None else None,
            1,
            (25.0,),
        )[0]
        opacity = parse_floats(
            opacity_param.attrib.get("value") if opacity_param is not None else None,
            1,
            (diffuse[3],),
        )[0]

        textures: Dict[int, Path] = {}
        for texture_param in element.findall("./TexChannel/Param"):
            try:
                channel = int(texture_param.attrib.get("chn", "-1"))
            except ValueError:
                continue
            texture = resolve_asset(media_root, texture_param.attrib.get("value"))
            if texture:
                textures[channel] = texture

        uv_params = None
        uv_param = element.find("./Static/Param[@name='uvparams']")
        if uv_param is not None:
            uv_params = parse_floats(uv_param.attrib.get("value"), 4, (1.0, 1.0, 0.0, 0.0))

        try:
            mask = int(effect.attrib.get("Mask", "0")) if effect is not None else 0
        except ValueError:
            mask = 0

        materials[name] = MaterialDef(
            name=name,
            base_color=tuple(clamp(component, 0.0, 1.0) for component in diffuse),  # type: ignore[assignment]
            specular=tuple(clamp(component, 0.0, 1.0) for component in specular),  # type: ignore[assignment]
            gloss=max(0.0, gloss),
            opacity=clamp(opacity, 0.0, 1.0),
            base_texture=textures.get(0),
            normal_texture=textures.get(1),
            uv_params=uv_params,
            effect=effect.attrib.get("FileName", "") if effect is not None else "",
            mask=mask,
        )
    return materials


def parse_multi_root_xml_file(path: Path, kind: str) -> ET.Element:
    """Parse gkENGINE XML, including its legacy two-root layout."""
    try:
        return ET.parse(str(path)).getroot()
    except ET.ParseError:
        try:
            text = path.read_text(encoding="utf-8", errors="replace")
            # gkENGINE scenes and camera files commonly put two roots next to
            # each other.  Wrap them only for the standard XML parser.
            return ET.fromstring("<gkENGINEDocument>" + text + "</gkENGINEDocument>")
        except (OSError, ET.ParseError) as error:
            raise ExportError("cannot read {} file {}: {}".format(kind, path, error))


def parse_gks_file(path: Path) -> ET.Element:
    return parse_multi_root_xml_file(path, "scene")


@dataclass(frozen=True)
class FaceCorner:
    position: int
    texcoord: int
    normal: int


@dataclass
class Triangle:
    corners: Tuple[FaceCorner, FaceCorner, FaceCorner]
    face_normal: Vec3


@dataclass
class ObjData:
    positions: List[Vec3] = field(default_factory=list)
    texcoords: List[Vec2] = field(default_factory=list)
    normals: List[Vec3] = field(default_factory=list)
    groups: "OrderedDict[str, List[Triangle]]" = field(default_factory=OrderedDict)


def parse_obj_index(value: str, length: int) -> int:
    if not value:
        return -1
    try:
        index = int(value)
    except ValueError:
        return -1
    if index > 0:
        index -= 1
    else:
        index = length + index
    return index if 0 <= index < length else -1


def parse_obj(path: Path) -> ObjData:
    if not path.is_file():
        raise ExportError("mesh file not found: {}".format(path))
    data = ObjData()
    current_material = "default"
    data.groups[current_material] = []

    try:
        lines = path.read_text(encoding="utf-8", errors="replace").splitlines()
    except OSError as error:
        raise ExportError("cannot read mesh file {}: {}".format(path, error))

    for line_number, raw_line in enumerate(lines, 1):
        line = raw_line.split("#", 1)[0].strip()
        if not line:
            continue
        parts = line.split()
        command = parts[0]

        try:
            if command == "v" and len(parts) >= 4:
                data.positions.append((float(parts[1]), float(parts[2]), float(parts[3])))
            elif command == "vt" and len(parts) >= 3:
                # Match CObjMeshLoader::LoadGeometryFromOBJ, which flips V.
                data.texcoords.append((float(parts[1]), 1.0 - float(parts[2])))
            elif command == "vn" and len(parts) >= 4:
                data.normals.append(normalize((float(parts[1]), float(parts[2]), float(parts[3]))))
            elif command == "usemtl":
                current_material = parts[1] if len(parts) > 1 else "default"
                data.groups.setdefault(current_material, [])
            elif command == "f" and len(parts) >= 4:
                corners = []
                for token in parts[1:]:
                    values = token.split("/")
                    corners.append(
                        FaceCorner(
                            parse_obj_index(values[0] if len(values) > 0 else "", len(data.positions)),
                            parse_obj_index(values[1] if len(values) > 1 else "", len(data.texcoords)),
                            parse_obj_index(values[2] if len(values) > 2 else "", len(data.normals)),
                        )
                    )
                for index in range(1, len(corners) - 1):
                    triangle_corners = (corners[0], corners[index], corners[index + 1])
                    if any(corner.position < 0 for corner in triangle_corners):
                        warn("{}:{} contains a face with an invalid position index".format(path, line_number))
                        continue
                    p0 = data.positions[triangle_corners[0].position]
                    p1 = data.positions[triangle_corners[1].position]
                    p2 = data.positions[triangle_corners[2].position]
                    face_normal = normalize(cross(subtract(p1, p0), subtract(p2, p0)))
                    data.groups[current_material].append(Triangle(triangle_corners, face_normal))
        except (IndexError, ValueError) as error:
            raise ExportError("invalid OBJ at {}:{}: {}".format(path, line_number, error))

    if not data.positions or not any(data.groups.values()):
        raise ExportError("mesh has no triangles: {}".format(path))
    return data


def is_default_texture(path: Optional[Path]) -> bool:
    if not path:
        return True
    normalized = path.as_posix().lower()
    return "/engine/assets/textures/default/" in normalized


def png_chunk(chunk_type: bytes, payload: bytes) -> bytes:
    return (
        struct.pack(">I", len(payload))
        + chunk_type
        + payload
        + struct.pack(">I", zlib.crc32(chunk_type + payload) & 0xFFFFFFFF)
    )


def rgba_to_png(width: int, height: int, pixels: bytes) -> bytes:
    scanlines = bytearray()
    stride = width * 4
    for row in range(height):
        scanlines.append(0)
        scanlines.extend(pixels[row * stride:(row + 1) * stride])
    header = struct.pack(">IIBBBBB", width, height, 8, 6, 0, 0, 0)
    return (
        b"\x89PNG\r\n\x1a\n"
        + png_chunk(b"IHDR", header)
        + png_chunk(b"IDAT", zlib.compress(bytes(scanlines), 9))
        + png_chunk(b"IEND", b"")
    )


def read_tga(path: Path) -> Tuple[int, int, bytes]:
    try:
        data = path.read_bytes()
    except OSError as error:
        raise ExportError("cannot read image {}: {}".format(path, error))
    if len(data) < 18:
        raise ExportError("invalid TGA header: {}".format(path))

    id_length, color_map_type, image_type = data[0], data[1], data[2]
    width, height = struct.unpack_from("<HH", data, 12)
    bits_per_pixel, descriptor = data[16], data[17]
    if color_map_type != 0:
        raise ExportError("color-mapped TGA is not supported: {}".format(path))
    if image_type not in (2, 3, 10, 11):
        raise ExportError("unsupported TGA image type {}: {}".format(image_type, path))
    if bits_per_pixel not in (8, 24, 32):
        raise ExportError("unsupported TGA pixel depth {}: {}".format(bits_per_pixel, path))

    pixel_size = bits_per_pixel // 8
    cursor = 18 + id_length
    pixel_count = width * height
    raw_pixels = bytearray()

    def read_pixel() -> bytes:
        nonlocal cursor
        if cursor + pixel_size > len(data):
            raise ExportError("truncated TGA image: {}".format(path))
        pixel = data[cursor:cursor + pixel_size]
        cursor += pixel_size
        if bits_per_pixel == 8:
            return pixel + pixel + pixel + b"\xff"
        if bits_per_pixel == 24:
            return bytes((pixel[2], pixel[1], pixel[0], 255))
        return bytes((pixel[2], pixel[1], pixel[0], pixel[3]))

    if image_type in (2, 3):
        for _ in range(pixel_count):
            raw_pixels.extend(read_pixel())
    else:
        while len(raw_pixels) < pixel_count * 4:
            if cursor >= len(data):
                raise ExportError("truncated RLE TGA image: {}".format(path))
            packet = data[cursor]
            cursor += 1
            count = (packet & 0x7F) + 1
            if packet & 0x80:
                pixel = read_pixel()
                raw_pixels.extend(pixel * count)
            else:
                for _ in range(count):
                    raw_pixels.extend(read_pixel())

    # TGA origin bit 5 is top-left when set.  Bit 4 indicates right-to-left.
    rows = [raw_pixels[row * width * 4:(row + 1) * width * 4] for row in range(height)]
    if not (descriptor & 0x20):
        rows.reverse()
    if descriptor & 0x10:
        rows = [b"".join(row[index:index + 4] for index in range(len(row) - 4, -1, -4)) for row in rows]
    return width, height, b"".join(rows)


class GlbBuilder:
    def __init__(self) -> None:
        self.binary = bytearray()
        self.buffer_views: List[dict] = []
        self.accessors: List[dict] = []
        self.meshes: List[dict] = []
        self.materials: List[dict] = []
        self.textures: List[dict] = []
        self.images: List[dict] = []
        self.nodes: List[dict] = []
        self.cameras: List[dict] = []
        self.lights: List[dict] = []
        self.image_cache: Dict[str, int] = {}
        self.texture_cache: Dict[int, int] = {}
        self.material_cache: Dict[Tuple[str, str], int] = {}

    def _align_binary(self) -> None:
        while len(self.binary) % 4:
            self.binary.append(0)

    def add_binary(self, value: bytes) -> Tuple[int, int]:
        self._align_binary()
        offset = len(self.binary)
        self.binary.extend(value)
        return offset, len(value)

    def add_buffer_view(self, value: bytes, target: Optional[int] = None) -> int:
        offset, length = self.add_binary(value)
        view = {"buffer": 0, "byteOffset": offset, "byteLength": length}
        if target is not None:
            view["target"] = target
        self.buffer_views.append(view)
        return len(self.buffer_views) - 1

    def add_accessor(
        self,
        buffer_view: int,
        count: int,
        component_type: int,
        accessor_type: str,
        minimum: Optional[Sequence[float]] = None,
        maximum: Optional[Sequence[float]] = None,
    ) -> int:
        accessor = {
            "bufferView": buffer_view,
            "componentType": component_type,
            "count": count,
            "type": accessor_type,
        }
        if minimum is not None:
            accessor["min"] = list(minimum)
        if maximum is not None:
            accessor["max"] = list(maximum)
        self.accessors.append(accessor)
        return len(self.accessors) - 1

    def add_image(self, path: Path) -> Optional[int]:
        key = str(path).lower()
        if key in self.image_cache:
            return self.image_cache[key]
        if not path.is_file():
            warn("texture file not found: {}".format(path))
            return None
        if path.suffix.lower() != ".tga":
            warn("texture is not TGA and will be omitted: {}".format(path))
            return None
        try:
            width, height, pixels = read_tga(path)
            png = rgba_to_png(width, height, pixels)
        except ExportError as error:
            warn(str(error))
            return None
        view = self.add_buffer_view(png)
        self.images.append({
            "name": path.stem,
            "bufferView": view,
            "mimeType": "image/png",
        })
        image_index = len(self.images) - 1
        self.image_cache[key] = image_index
        return image_index

    def add_texture(self, path: Optional[Path]) -> Optional[int]:
        image = self.add_image(path) if path else None
        if image is None:
            return None
        if image in self.texture_cache:
            return self.texture_cache[image]
        self.textures.append({"source": image})
        texture_index = len(self.textures) - 1
        self.texture_cache[image] = texture_index
        return texture_index

    def add_material(self, material: MaterialDef, material_file: Optional[Path]) -> int:
        cache_key = (str(material_file).lower() if material_file else "", material.name)
        if cache_key in self.material_cache:
            return self.material_cache[cache_key]

        diffuse = list(material.base_color)
        diffuse[3] = clamp(diffuse[3] * material.opacity, 0.0, 1.0)
        roughness = clamp(1.0 - math.sqrt(material.gloss / 256.0), 0.05, 1.0)
        specular_luminance = (
            material.specular[0] * 0.2126
            + material.specular[1] * 0.7152
            + material.specular[2] * 0.0722
        )
        material_json = {
            "name": material.name,
            "doubleSided": True,
            "pbrMetallicRoughness": {
                "baseColorFactor": diffuse,
                "metallicFactor": clamp(specular_luminance * 0.25, 0.0, 1.0),
                "roughnessFactor": roughness,
            },
            "extras": {
                "gkEffect": material.effect,
                "gkMask": material.mask,
            },
        }
        if material.uv_params is not None:
            material_json["extras"]["gkUvParams"] = list(material.uv_params)

        base_texture = self.add_texture(None if is_default_texture(material.base_texture) else material.base_texture)
        if base_texture is not None:
            material_json["pbrMetallicRoughness"]["baseColorTexture"] = {"index": base_texture}
        normal_texture = self.add_texture(None if is_default_texture(material.normal_texture) else material.normal_texture)
        if normal_texture is not None:
            material_json["normalTexture"] = {"index": normal_texture, "scale": material.normal_scale}
        if diffuse[3] < 0.999:
            material_json["alphaMode"] = "BLEND"
        self.materials.append(material_json)
        index = len(self.materials) - 1
        self.material_cache[cache_key] = index
        return index

    def add_geometry(self, obj_path: Path, mtl_path: Optional[Path], material_defs: Dict[str, MaterialDef]) -> int:
        obj = parse_obj(obj_path)
        primitives = []
        for material_name, triangles in obj.groups.items():
            if not triangles:
                continue
            vertices: List[float] = []
            normals: List[float] = []
            texcoords: List[float] = []
            indices: List[int] = []
            vertex_cache: Dict[Tuple[int, int, int, Vec3], int] = {}
            positions_for_bounds: List[Vec3] = []

            for triangle in triangles:
                for corner in triangle.corners:
                    normal = (
                        obj.normals[corner.normal]
                        if 0 <= corner.normal < len(obj.normals)
                        else triangle.face_normal
                    )
                    cache_key = (corner.position, corner.texcoord, corner.normal, normal)
                    index = vertex_cache.get(cache_key)
                    if index is None:
                        position = gk_to_gltf(obj.positions[corner.position])
                        normal_gltf = normalize(gk_to_gltf(normal))
                        uv = obj.texcoords[corner.texcoord] if 0 <= corner.texcoord < len(obj.texcoords) else (0.0, 0.0)
                        index = len(vertices) // 3
                        vertex_cache[cache_key] = index
                        vertices.extend(position)
                        normals.extend(normal_gltf)
                        texcoords.extend(uv)
                        positions_for_bounds.append(position)
                    indices.append(index)

            if not positions_for_bounds:
                continue
            position_data = struct.pack("<{}f".format(len(vertices)), *vertices)
            normal_data = struct.pack("<{}f".format(len(normals)), *normals)
            texcoord_data = struct.pack("<{}f".format(len(texcoords)), *texcoords)
            index_type = 5123 if len(vertices) // 3 <= 65535 else 5125
            if index_type == 5123:
                index_data = struct.pack("<{}H".format(len(indices)), *indices)
            else:
                index_data = struct.pack("<{}I".format(len(indices)), *indices)

            pos_view = self.add_buffer_view(position_data, 34962)
            normal_view = self.add_buffer_view(normal_data, 34962)
            uv_view = self.add_buffer_view(texcoord_data, 34962)
            index_view = self.add_buffer_view(index_data, 34963)
            minimum = [min(position[component] for position in positions_for_bounds) for component in range(3)]
            maximum = [max(position[component] for position in positions_for_bounds) for component in range(3)]
            pos_accessor = self.add_accessor(pos_view, len(vertices) // 3, 5126, "VEC3", minimum, maximum)
            normal_accessor = self.add_accessor(normal_view, len(normals) // 3, 5126, "VEC3")
            uv_accessor = self.add_accessor(uv_view, len(texcoords) // 2, 5126, "VEC2")
            index_accessor = self.add_accessor(index_view, len(indices), index_type, "SCALAR")

            material = material_defs.get(material_name)
            if material is None:
                material = MaterialDef(material_name)
            material_index = self.add_material(material, mtl_path)
            primitives.append({
                "attributes": {
                    "POSITION": pos_accessor,
                    "NORMAL": normal_accessor,
                    "TEXCOORD_0": uv_accessor,
                },
                "indices": index_accessor,
                "material": material_index,
                "mode": 4,
            })

        self.meshes.append({"name": obj_path.stem, "primitives": primitives})
        return len(self.meshes) - 1

    def add_light(self, name: str, color: Sequence[float], radius: float) -> int:
        light = {
            "name": name,
            "type": "point",
            "color": [clamp(float(value), 0.0, 1.0) for value in color[:3]],
            "intensity": 1.0,
        }
        if radius > 0.0:
            light["range"] = radius
        self.lights.append(light)
        return len(self.lights) - 1

    def add_camera(
        self,
        name: str,
        aspect_ratio: float = 1.7778,
        yfov_degrees: float = 68.0,
        near_clip: float = 0.25,
        far_clip: float = 4000.0,
    ) -> int:
        camera = {
            "name": name,
            "type": "perspective",
            "perspective": {
                "aspectRatio": aspect_ratio,
                "yfov": math.radians(yfov_degrees),
                "znear": near_clip,
                "zfar": far_clip,
            },
            "extras": {
                "gkFovYDegrees": yfov_degrees,
                "gkNearClip": near_clip,
                "gkFarClip": far_clip,
            },
        }
        self.cameras.append(camera)
        return len(self.cameras) - 1

    def write(self, path: Path, root_nodes: List[int], source: Path) -> None:
        document = {
            "asset": {
                "version": "2.0",
                "generator": "gkENGINE gks2glb",
                "extras": {"source": str(source)},
            },
            "scene": 0,
            "scenes": [{"name": source.stem, "nodes": root_nodes}],
            "nodes": self.nodes,
            "meshes": self.meshes,
            "materials": self.materials,
            "accessors": self.accessors,
            "bufferViews": self.buffer_views,
            "buffers": [{"byteLength": len(self.binary)}],
        }
        if self.textures:
            document["textures"] = self.textures
        if self.images:
            document["images"] = self.images
        if self.cameras:
            document["cameras"] = self.cameras
        if self.lights:
            document["extensionsUsed"] = ["KHR_lights_punctual"]
            document["extensions"] = {"KHR_lights_punctual": {"lights": self.lights}}

        json_data = json.dumps(document, ensure_ascii=False, separators=(",", ":")).encode("utf-8")
        json_data += b" " * ((4 - len(json_data) % 4) % 4)
        binary_data = bytes(self.binary)
        binary_data += b"\x00" * ((4 - len(binary_data) % 4) % 4)
        total_length = 12 + 8 + len(json_data) + 8 + len(binary_data)
        blob = bytearray()
        blob.extend(struct.pack("<III", 0x46546C67, 2, total_length))
        blob.extend(struct.pack("<II", len(json_data), 0x4E4F534A))
        blob.extend(json_data)
        blob.extend(struct.pack("<II", len(binary_data), 0x004E4942))
        blob.extend(binary_data)
        try:
            path.parent.mkdir(parents=True, exist_ok=True)
            path.write_bytes(blob)
        except OSError as error:
            raise ExportError("cannot write GLB {}: {}".format(path, error))


def parse_transform(element: ET.Element) -> Tuple[Vec3, Quat, Vec3]:
    transform = element.find("./NodeTM")
    translation = parse_floats(
        transform.findtext("./Translation") if transform is not None else None,
        3,
        (0.0, 0.0, 0.0),
    )
    rotation = parse_floats(
        transform.findtext("./Rotation") if transform is not None else None,
        4,
        (0.0, 0.0, 0.0, 1.0),
    )
    scale = parse_floats(
        transform.findtext("./Scale") if transform is not None else None,
        3,
        (1.0, 1.0, 1.0),
    )
    return translation, rotation, scale  # type: ignore[return-value]


def append_camera_records(builder: GlbBuilder, root_nodes: List[int], camera_path: Path) -> int:
    if not camera_path.is_file():
        raise ExportError("camera file not found: {}".format(camera_path))
    camera_root = parse_multi_root_xml_file(camera_path, "camera")
    cameras = camera_root if camera_root.tag == "Cameras" else camera_root.find("./Cameras")
    if cameras is None:
        raise ExportError("camera file has no Cameras node: {}".format(camera_path))

    count = 0
    for ordinal, element in enumerate(cameras.findall("./CameraRecord")):
        try:
            camera_index = int(element.attrib.get("Index", str(ordinal)))
        except ValueError:
            camera_index = ordinal
        position = parse_floats(element.attrib.get("Pos"), 3, (0.0, 0.0, 0.0))
        rotation = parse_floats(element.attrib.get("Rot"), 4, (0.0, 0.0, 0.0, 1.0))
        name = "{}_camera_{:02d}_index_{}".format(camera_path.stem, ordinal, camera_index)
        camera = builder.add_camera(name)
        node = {
            "name": name,
            "camera": camera,
            "translation": list(gk_to_gltf(position)),
            "rotation": list(gk_camera_quat_to_gltf(rotation)),
            "scale": [1.0, 1.0, 1.0],
            "extras": {
                "gkCameraIndex": camera_index,
                "gkCameraRecord": ordinal,
                "gkCameraFile": str(camera_path),
            },
        }
        builder.nodes.append(node)
        root_nodes.append(len(builder.nodes) - 1)
        count += 1
    return count


def export_scene(
    gks_path: Path,
    output_path: Path,
    media_root: Optional[Path],
    camera_file: Optional[Path] = None,
) -> Tuple[int, int, int, int]:
    if not gks_path.is_file():
        raise ExportError("scene file not found: {}".format(gks_path))
    root = parse_gks_file(gks_path)

    media = resolve_media_root(gks_path, media_root)
    scene_objects = root.find("./SceneObjects")
    if scene_objects is None:
        raise ExportError("scene has no SceneObjects node: {}".format(gks_path))

    builder = GlbBuilder()
    mesh_cache: Dict[Tuple[str, str], int] = {}
    material_cache: Dict[str, Dict[str, MaterialDef]] = {}
    root_nodes: List[int] = []
    mesh_object_count = 0
    light_count = 0

    def make_node(element: ET.Element) -> int:
        nonlocal mesh_object_count, light_count
        name = element.attrib.get("Name", "gkObject")
        superclass = element.attrib.get("SuperClass", "Unknown")
        superclass_lower = superclass.lower()
        translation, rotation, scale = parse_transform(element)
        node = {
            "name": name,
            "translation": list(gk_to_gltf(translation)),
            "rotation": list(gk_quat_to_gltf(rotation)),
            "scale": list(scale),
            "extras": {
                "gkSuperClass": superclass,
            },
        }

        if superclass_lower == "mesh":
            mesh_value = element.attrib.get("MeshName")
            mesh_path = resolve_asset(media, mesh_value)
            if not mesh_path:
                raise ExportError("mesh object {} has no MeshName".format(name))
            if mesh_path.suffix.lower() != ".obj":
                raise ExportError(
                    "mesh {} uses {}, but gks2glb currently supports OBJ meshes only".format(
                        name, mesh_path.suffix or "a mesh format"
                    )
                )
            material_path = resolve_asset(media, element.attrib.get("MaterialName"))
            cache_key = (
                str(mesh_path).lower(),
                str(material_path).lower() if material_path else "",
            )
            if cache_key not in mesh_cache:
                if material_path and str(material_path).lower() not in material_cache:
                    material_cache[str(material_path).lower()] = parse_material_file(material_path, media)
                material_defs = material_cache.get(str(material_path).lower(), {}) if material_path else {}
                mesh_cache[cache_key] = builder.add_geometry(mesh_path, material_path, material_defs)
            node["mesh"] = mesh_cache[cache_key]
            node["extras"]["gkMeshName"] = mesh_value
            if element.attrib.get("MaterialName"):
                node["extras"]["gkMaterialName"] = element.attrib["MaterialName"]
            mesh_object_count += 1
        elif superclass_lower == "light":
            color = parse_floats(element.attrib.get("Color"), 3, (1.0, 1.0, 1.0))
            radius = parse_floats(element.attrib.get("Radius"), 1, (0.0,))[0]
            light_index = builder.add_light(name, color, radius)
            node["extensions"] = {"KHR_lights_punctual": {"light": light_index}}
            node["extras"]["gkRadius"] = radius
            light_count += 1
        else:
            for attribute in ("MeshName", "MaterialName", "Color", "Radius"):
                if attribute in element.attrib:
                    node["extras"]["gk" + attribute] = element.attrib[attribute]

        node_index = len(builder.nodes)
        builder.nodes.append(node)
        children = []
        for child in element.findall("./gkObject"):
            children.append(make_node(child))
        if children:
            node["children"] = children
        return node_index

    for element in scene_objects.findall("./gkObject"):
        root_nodes.append(make_node(element))

    camera_count = 0
    inferred_camera_file = gks_path.with_suffix(".cam")
    if camera_file is None:
        if inferred_camera_file.is_file():
            camera_count = append_camera_records(builder, root_nodes, inferred_camera_file)
    else:
        camera_count = append_camera_records(builder, root_nodes, camera_file)

    builder.write(output_path, root_nodes, gks_path)
    return len(root_nodes), mesh_object_count, light_count, camera_count


def build_argument_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        description="Export a gkENGINE .gks scene to a self-contained binary glTF (.glb)."
    )
    parser.add_argument("scene", type=Path, help="input .gks scene")
    parser.add_argument("output", type=Path, help="output .glb file")
    parser.add_argument(
        "--media-root",
        type=Path,
        default=None,
        help="exec/media directory; inferred from the scene path when omitted",
    )
    parser.add_argument(
        "--camera-file",
        type=Path,
        default=None,
        help="camera .cam file; defaults to a same-name file next to the .gks scene",
    )
    return parser


def main(argv: Optional[Sequence[str]] = None) -> int:
    parser = build_argument_parser()
    args = parser.parse_args(argv)
    try:
        roots, meshes, lights, cameras = export_scene(
            args.scene.resolve(),
            args.output.resolve(),
            args.media_root.resolve() if args.media_root else None,
            args.camera_file.resolve() if args.camera_file else None,
        )
    except ExportError as error:
        print("gks2glb: error: {}".format(error), file=sys.stderr)
        return 1
    print(
        "gks2glb: wrote {} ({} root nodes, {} mesh objects, {} lights, {} cameras)".format(
            args.output.resolve(), roots, meshes, lights, cameras
        )
    )
    return 0


if __name__ == "__main__":
    sys.exit(main())
