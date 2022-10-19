# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    compil.sh                                          :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/10/19 14:51:45 by vvaucoul          #+#    #+#              #
#    Updated: 2022/10/19 15:06:55 by vvaucoul         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

#!/bin/sh

nasm -f elf32 srcs/boot.s -o boot.o
gcc -g3 -m32 -Wall -Wextra -Wfatal-errors -fno-builtin -fno-exceptions -fno-stack-protector -nostdlib -nodefaultlibs -std=gnu99 -ffreestanding -O2 srcs/kernel.c -c -o kernel.o

ld -m elf_i386 -T srcs/linker.ld -o kernel.bin boot.o kernel.o

mkdir -p isodir/boot/grub
cp kernel.bin isodir/boot/kernel.bin
cp srcs/grub.cfg isodir/boot/grub/grub.cfg
grub-mkrescue -o kernel.iso isodir --xorriso=/mnt/nfs/homes/vvaucoul/Documents/github_vintz/KronOS/xorriso-1.4.6/xorriso/xorriso