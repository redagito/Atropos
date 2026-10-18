#!/usr/bin/env python3
"""Generates the glyph atlas and metrics used by the UI font.

Renders printable ASCII from DejaVuSansMono.ttf into a 32 bit uncompressed TGA
atlas plus a plain text metrics file. The atlas is committed so neither the
build nor the runtime needs Pillow or the source font.

Usage: python tools/make_font_atlas.py [size]
"""
import os
import struct
import sys

from PIL import Image, ImageDraw, ImageFont

FONT = "App/data/font/DejaVuSansMono.ttf"
ATLAS = "App/data/font/DejaVuSansMono.tga"
METRICS = "App/data/font/DejaVuSansMono.fnt"
FIRST, LAST = 32, 126
PAD = 4


def write_tga(path, width, height, rgba):
    """Writes an uncompressed 32 bit BGRA TGA with a top-left origin."""
    header = struct.pack(
        "<BBBHHBHHHHBB",
        0,              # id length
        0,              # colour map type
        2,              # uncompressed true colour
        0, 0, 0,        # colour map spec
        0, 0,           # x/y origin
        width, height,
        32,             # bits per pixel
        0x28,           # top-left origin, 8 alpha bits
    )
    body = bytearray()
    for i in range(0, len(rgba), 4):
        r, g, b, a = rgba[i], rgba[i + 1], rgba[i + 2], rgba[i + 3]
        body += bytes((b, g, r, a))
    with open(path, "wb") as handle:
        handle.write(header)
        handle.write(body)


def main():
    size = int(sys.argv[1]) if len(sys.argv) > 1 else 32
    font = ImageFont.truetype(FONT, size)
    ascent, descent = font.getmetrics()
    line_height = ascent + descent

    cell = size * 3
    glyphs = []
    for code in range(FIRST, LAST + 1):
        char = chr(code)
        canvas = Image.new("L", (cell, cell), 0)
        ImageDraw.Draw(canvas).text((PAD, PAD), char, font=font, fill=255)
        bbox = canvas.getbbox()
        advance = font.getlength(char)
        if bbox is None:
            glyphs.append((code, None, 0, 0, 0, 0, advance))
            continue
        ink = canvas.crop(bbox)
        glyphs.append((code, ink, bbox[0] - PAD, bbox[1] - PAD, ink.width, ink.height, advance))

    # Row pack the ink boxes into a power of two atlas.
    max_w = max((g[4] for g in glyphs if g[1] is not None), default=1)
    columns = max(1, 512 // max_w)
    rows = (len(glyphs) + columns - 1) // columns
    atlas_w = 512
    atlas_h = 1
    needed = rows * (size + PAD * 2)
    while atlas_h < needed:
        atlas_h *= 2

    atlas = Image.new("RGBA", (atlas_w, atlas_h), (255, 255, 255, 0))
    entries = []
    for index, (code, ink, off_x, off_y, w, h, advance) in enumerate(glyphs):
        col, row = index % columns, index // columns
        ax, ay = col * max_w, row * (size + PAD * 2)
        if ink is not None:
            white = Image.new("RGBA", ink.size, (255, 255, 255, 255))
            white.putalpha(ink)
            atlas.paste(white, (ax, ay))
        entries.append((code, ax, ay, w, h, off_x, off_y, advance))

    write_tga(ATLAS, atlas_w, atlas_h, atlas.tobytes())

    with open(METRICS, "w", encoding="ascii", newline="\n") as handle:
        handle.write("# atlas width height size line_height\n")
        handle.write(f"{atlas_w} {atlas_h} {size} {line_height}\n")
        for code, ax, ay, w, h, off_x, off_y, advance in entries:
            handle.write(f"{code} {ax} {ay} {w} {h} {off_x} {off_y} {advance:.2f}\n")

    print(f"atlas   {atlas_w}x{atlas_h}  glyphs={len(entries)}  line_height={line_height}")
    print(f"advance={glyphs[ord('0')][6]:.2f}  '0' ink={glyphs[ord('0')][4]}x{glyphs[ord('0')][5]}")
    print(f"wrote {ATLAS}")
    print(f"wrote {METRICS}")


if __name__ == "__main__":
    main()