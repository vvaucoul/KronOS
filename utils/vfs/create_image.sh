# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    create_image.sh                                    :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/07/20 10:23:28 by vvaucoul          #+#    #+#              #
#    Updated: 2023/10/25 11:21:09 by vvaucoul         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

#!/bin/bash

# Create floppy.img

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

# Create initrd.img

dd if=/dev/zero of=initrd.img bs=1M count=1
mkfs.ext2 initrd.img

mkdir tmp_mount
sudo mount -o loop initrd.img tmp_mount

sudo mount floppy.img /mnt

sudo mkdir -p /mnt/boot/grub
sudo cp ../../isodir/boot/kernel.bin /mnt/boot/kernel.bin
sudo cp initrd.img /mnt/initrd
sudo cp ../../isodir/boot/grub/grub.cfg /mnt/boot/grub/grub.cfg

sudo umount tmp_mount
sudo umount /mnt
sudo rm -rf tmp_mount

grub-mkrescue -o boot.iso /mnt

cp floppy.img ../../floppy.img
cp initrd.img ../../initrd.img
cp boot.iso ../../boot.iso