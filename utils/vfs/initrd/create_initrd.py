# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    create_initrd.py                                   :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/01/13 13:56:03 by vvaucoul          #+#    #+#              #
#    Updated: 2024/01/13 14:19:21 by vvaucoul         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

import struct
import os

# Definition of the initrd header structure
InitrdHeader = struct.Struct('I')  # uint32_t nfiles
InitrdFileHeader = struct.Struct('B64sII')  # uint8_t magic; char name[64]; uint32_t size; uint32_t offset

# Directory containing the files to include in the initrd
initrd_directory = 'initrd_files'

# Initrd magic
magic = 0xBF

# Create a list to store the file headers
file_headers = []
files_data = b''

# Iterate through the files in the initrd directory
for filename in os.listdir(initrd_directory):
    filepath = os.path.join(initrd_directory, filename)
    if os.path.isfile(filepath):
        with open(filepath, 'rb') as f:
            file_data = f.read()
            files_data += file_data
            # Add a magic byte in front of the file name
            file_headers.append((magic, filename.encode('ascii'), len(file_data), len(files_data) - len(file_data)))

# Create the initrd header
initrd_header = InitrdHeader.pack(len(file_headers))

# Create the initrd file headers
headers_data = b''.join(InitrdFileHeader.pack(*header) for header in file_headers)

# Write the complete initrd to a file
with open('initrd.img', 'wb') as f:
    f.write(initrd_header)
    f.write(headers_data)
    f.write(files_data)