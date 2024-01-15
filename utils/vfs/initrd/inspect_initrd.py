# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    inspect_initrd.py                                  :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/01/13 13:57:31 by vvaucoul          #+#    #+#              #
#    Updated: 2024/01/13 14:17:31 by vvaucoul         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

import struct
import sys

# Definition of the initrd header structure
InitrdHeader = struct.Struct('I')  # uint32_t nfiles
InitrdFileHeader = struct.Struct('B64sII')  # uint8_t magic; char name[64]; uint32_t size; uint32_t offset

# Open the initrd file
initrd_path = 'initrd.img'
try:
    with open(initrd_path, 'rb') as f:
        initrd_data = f.read()
except FileNotFoundError:
    print(f"Error: File '{initrd_path}' not found.")
    sys.exit(1)

# Read the initrd header
header_size = InitrdHeader.size
header_data = initrd_data[:header_size]
nfiles, = InitrdHeader.unpack(header_data)

print("Inspecting the initrd image")
print(f"Number of files: {nfiles}")
print("\n")

# Display file headers
print("File table in the initrd:")
print("| Magic | File Name                           | Size (bytes)    | Offset   |")
print("|-------|-------------------------------------|-----------------|----------|")

offset = header_size
for _ in range(nfiles):
    file_header_data = initrd_data[offset:offset + InitrdFileHeader.size]
    magic, filename, size, file_offset = InitrdFileHeader.unpack(file_header_data)
    filename = filename.split(b'\0', 1)[0].decode('ascii')  # Clean up the file name
    print(f"| {magic:<5} | {filename:<35} | {size:>15} | {file_offset:>8} |")
    offset += InitrdFileHeader.size

print("|-------|-------------------------------------|-----------------|----------|")
