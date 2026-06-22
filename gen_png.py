import struct
import zlib

width, height = 16, 16
bit_depth = 8
color_type = 2
data = b'\x00' + b'\x00\x00\x00' * width
raw_data = data * height
compressed = zlib.compress(raw_data)

IHDR = struct.pack('>IIBBBBB', width, height, bit_depth, color_type, 0, 0, 0)
IDAT = compressed

def chunk(type_b, data_b):
    return struct.pack('>I', len(data_b)) + type_b + data_b + struct.pack('>I', zlib.crc32(type_b + data_b) & 0xffffffff)

png = b'\x89PNG\r\n\x1a\n' + chunk(b'IHDR', IHDR) + chunk(b'IDAT', IDAT) + chunk(b'IEND', b'')

with open('black16.png', 'wb') as f:
    f.write(png)
