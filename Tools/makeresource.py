#! /usr/bin/env python3

"""
    This script adds resource header and converts byte ordering for binary data.

    Input can be obtained from binary file, by parsing WAV files or C sources containing LUTs.

    C format parsing tested with Magus LED RGB tables, power and log LUTs.
"""

import argparse
import struct
import sys
import wave

parser = argparse.ArgumentParser(
    description='Convert binary data to OpenWare resource')
parser.add_argument(
    '-i', '--input', required=True, help='Input file')
parser.add_argument(
    '-o', '--output', required=True, help='Output file')
parser.add_argument(
    '-p', '--parse', help='Parse LUT from C header file', action='store_true')
parser.add_argument(
    '-w', '--wav', help='Convert WAV file to wavetable', action='store_true')
parser.add_argument(
    '-f', '--format', help='WAV file sample wavetable (i.e. <H for 16bit unsigned little-endian)', default='<H')
parser.add_argument(
    '-d', '--destination_format',
    help='Normally this will be retrieved from parsed header. If that fails, use the same format as python\'s "struct" modlue (i.e. <I, <f)')
parser.add_argument('name', help='Resource name')

args = parser.parse_args()

in_file = open(args.input, 'rb')
out_file = open(args.output, 'wb')

data = in_file.read()

formats = {
    'float': '<f',
    'uint32_t': '<I',
    'int32_t': '<i',
    'uint16_t': '<H',
    'int16_t': '<h',
    'uint8_t': '<B',
    'int8_t': '<b',
    'char': '<c',
    'size_t': '<N',
}

if args.parse:
    values = data.split(b'{', 1)[1].split(b'}', 1)[0].split(b',')

    destination_format = args.destination_format
    if not args.destination_format:
        last_line = data.split(b'{', 1)[0].splitlines()[-1]
        for t in formats:            
            if f' {t} '.encode('ascii') in last_line:
                print(f'Found format: {t} -> {formats[t]}')
                destination_format = formats[t]
                break

    if destination_format is None:
        raise ValueError(
            'Please specify destination format value with -d parameter. '
            'See https://docs.python.org/3/library/struct.html#format-characters')


    if 'f' in destination_format or 'F' in destination_format:
        values = [float(value.strip()) for value in values if value.strip()]
        print('Parsing data as floats')
    else:
        # Using eval() instead of int() allows us to parse prefixes like 0x or 0b
        values = [eval(value.strip()) for value in values if value.strip()]
        print('Parsing data as integers')

    data = b''.join(struct.pack(destination_format, value) for value in values)

elif args.wav:
    w = wave.open(args.input)
    n_frames = w.getnframes()
    data = w.readframes(w.getnframes())
    input_format = args.format[:-1] + str(n_frames) + args.format[-1]    
    print(f'Parsing file as {input_format}')
    values = struct.unpack(input_format, data)
    destination_format = args.destination_format or args.format
    data = b''.join(struct.pack(destination_format, value) for value in values)

# Magick
out_file.write(0xdadadeed.to_bytes(4, 'little'))

# Data size
size = len(data)
if size % 4:
    # Align to 4 bytes
    size += 4 - size % 4
print(f'Data size is {size} bytes')
out_file.write(size.to_bytes(4, 'little'))    

# Resource name
name = list(struct.pack('24s', args.name.encode('ascii')))
name[-1] = 0 # Force terminate name with zero
name = bytes(name)
for i in range(0, 24, 4):
    out_file.write(int.from_bytes(name[i : i + 4], sys.byteorder).to_bytes(4, 'little'))

# Data
for i in range(0, size, 4):
    out_file.write(int.from_bytes(data[i : i + 4], sys.byteorder).to_bytes(4, 'little'))
