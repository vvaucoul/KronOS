# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    create_image.sh                                    :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/07/20 10:23:28 by vvaucoul          #+#    #+#              #
#    Updated: 2023/10/25 12:15:01 by vvaucoul         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

#!/bin/bash

# Paths
KERNEL_BIN_PATH="../../isodir/boot/kernel.bin"
GRUB_CFG_PATH="../../isodir/boot/grub/grub.cfg"
ISO_DIR="../../isodir"

# Check if mkfs.ext2 is installed
if ! [ -x "$(command -v mkfs.ext2)" ]; then
    echo "mkfs.ext2 is not installed, please install it"
    exit 1
fi

# Check if grub-mkrescue is installed
if ! [ -x "$(command -v grub-mkrescue)" ]; then
    echo "grub-mkrescue is not installed, please install it"
    exit 1
fi

# Create floppy.img -> EXT2
dd if=/dev/zero of=floppy.img bs=1M count=1
mkfs.ext2 floppy.img

# Mount floppy.img to copy necessary files
mkdir -p tmp_mount
sudo mount -o loop floppy.img tmp_mount

# Copy kernel and grub config to the floppy image
sudo mkdir -p tmp_mount/boot/grub
sudo cp $KERNEL_BIN_PATH tmp_mount/boot/kernel.bin
sudo cp $GRUB_CFG_PATH tmp_mount/boot/grub/grub.cfg

# Unmount floppy image
sudo umount tmp_mount
rm -rf tmp_mount

# Create bootable ISO
grub-mkrescue -o boot.iso $ISO_DIR

# Copy floppy.img and boot.iso to the desired location
cp floppy.img ../../isodir/boot/floppy.img
cp boot.iso ../../isodir/boot/boot.iso