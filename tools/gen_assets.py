#!/usr/bin/env python3
"""Deterministic asset generator for test task 01.

Creates:
  assets/checker.png  — 512x512 RGBA8 checkerboard texture (8x8 cells, colored
                        border and corner markers that show the UV orientation);
  assets/cube.obj     — unit cube (24 vertices, 12 triangles) with positions,
                        UVs and normals.

Python standard library only (zlib, struct). Usage:
    python3 -I tools/gen_assets.py            # writes to ../assets
    python3 -I tools/gen_assets.py --out DIR
"""
import argparse
import os
import struct
import zlib


def write_png_rgba(path, width, height, pixels):
    """pixels: bytes of length width*height*4 (RGBA8, rows top to bottom)."""
    def chunk(tag, data):
        body = tag + data
        return struct.pack(">I", len(data)) + body + struct.pack(">I", zlib.crc32(body) & 0xFFFFFFFF)

    stride = width * 4
    raw = bytearray()
    for y in range(height):
        raw.append(0)  # filter type 0 (None)
        raw += pixels[y * stride:(y + 1) * stride]
    ihdr = struct.pack(">IIBBBBB", width, height, 8, 6, 0, 0, 0)  # 8 bit, RGBA
    with open(path, "wb") as f:
        f.write(b"\x89PNG\r\n\x1a\n")
        f.write(chunk(b"IHDR", ihdr))
        f.write(chunk(b"IDAT", zlib.compress(bytes(raw), 9)))
        f.write(chunk(b"IEND", b""))


def make_checker(size=512, cells=8):
    light = (235, 235, 235, 255)
    dark = (40, 44, 52, 255)
    # Corner markers: they make a flipped V coordinate easy to spot.
    corner = {
        (0, 0): (220, 50, 50, 255),      # top-left (u=0, v=0 with v pointing down): red
        (1, 0): (50, 170, 60, 255),      # top-right: green
        (0, 1): (50, 90, 220, 255),      # bottom-left: blue
        (1, 1): (240, 200, 40, 255),     # bottom-right: yellow
    }
    cell = size // cells
    marker = cell // 2
    border = 4
    px = bytearray(size * size * 4)
    for y in range(size):
        for x in range(size):
            c = light if ((x // cell) + (y // cell)) % 2 == 0 else dark
            if x < border or y < border or x >= size - border or y >= size - border:
                c = (255, 140, 0, 255)
            for (cx, cy), col in corner.items():
                mx0 = border + 4 if cx == 0 else size - border - 4 - marker
                my0 = border + 4 if cy == 0 else size - border - 4 - marker
                if mx0 <= x < mx0 + marker and my0 <= y < my0 + marker:
                    c = col
            i = (y * size + x) * 4
            px[i:i + 4] = bytes(c)
    return size, size, bytes(px)


CUBE_OBJ_HEADER = """# Unit cube for BHS test task 01 (Quake RTX on Apple Metal)
# 24 vertices (4 per face, so every face has its own UVs and normal), 12 triangles.
# Coordinates: right-handed, Y up, cube spans [-0.5, 0.5]^3.
# Winding: counter-clockwise when looking at the face from outside.
# UV: (0,0) = bottom-left of the image as stored in OBJ convention (v up).
#     Metal/Vulkan sample textures with v pointing DOWN -> flip v (v' = 1 - v)
#     either when loading or in the shader. The corner markers in checker.png
#     make a wrong flip obvious.
# Faces use 1-based indices: f v/vt/vn
"""


def make_cube_obj():
    faces = [
        # normal,        u axis,        v axis
        ((0, 0, 1), (1, 0, 0), (0, 1, 0)),     # +Z (front)
        ((0, 0, -1), (-1, 0, 0), (0, 1, 0)),   # -Z (back)
        ((1, 0, 0), (0, 0, -1), (0, 1, 0)),    # +X (right)
        ((-1, 0, 0), (0, 0, 1), (0, 1, 0)),    # -X (left)
        ((0, 1, 0), (1, 0, 0), (0, 0, -1)),    # +Y (top)
        ((0, -1, 0), (1, 0, 0), (0, 0, 1)),    # -Y (bottom)
    ]
    names = ["front", "back", "right", "left", "top", "bottom"]
    v_lines, vt_lines, vn_lines, f_lines = [], [], [], []
    uvs = [(0, 0), (1, 0), (1, 1), (0, 1)]
    for fi, ((n, u, v), name) in enumerate(zip(faces, names)):
        vn_lines.append("vn %d %d %d" % n)
        for (s, t) in uvs:
            p = [0.5 * n[k] + (s - 0.5) * u[k] + (t - 0.5) * v[k] for k in range(3)]
            v_lines.append("v %.4f %.4f %.4f" % tuple(p))
        base = fi * 4 + 1
        nidx = fi + 1
        f_lines.append("# %s" % name)
        for tri in ((0, 1, 2), (0, 2, 3)):
            f_lines.append("f " + " ".join("%d/%d/%d" % (base + i, i + 1, nidx) for i in tri))
    for (s, t) in uvs:
        vt_lines.append("vt %.4f %.4f" % (s, t))
    return CUBE_OBJ_HEADER + "o cube\n" + "\n".join(v_lines + vt_lines + vn_lines + f_lines) + "\n"


def main():
    here = os.path.dirname(os.path.abspath(__file__))
    ap = argparse.ArgumentParser()
    ap.add_argument("--out", default=os.path.join(here, "..", "assets"))
    args = ap.parse_args()
    os.makedirs(args.out, exist_ok=True)
    w, h, px = make_checker()
    write_png_rgba(os.path.join(args.out, "checker.png"), w, h, px)
    with open(os.path.join(args.out, "cube.obj"), "w", newline="\n") as f:
        f.write(make_cube_obj())
    print("written:", os.path.normpath(args.out))


if __name__ == "__main__":
    main()
