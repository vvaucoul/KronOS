# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    ext2_format.sh                                     :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/01/19 10:40:47 by vvaucoul          #+#    #+#              #
#    Updated: 2024/01/19 10:52:38 by vvaucoul         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

#!/bin/bash

# Colors
RESET="\033[0m"
BOLD="\033[1m"
DIM="\033[2m"
RED="\033[1;31m"
GREEN="\033[1;32m"
YELLOW="\033[1;33m"
WHITE="\033[1;37m"

if command -v mkfs.ext2 >/dev/null 2>&1; then
    echo -n ""
else
    printf "$WHITE    - mkfs.ext2 not found, please install it.$RESET\n"
    exit 1
fi

hdd_path="../../isodir/boot"
hdd_filename=$(grep -oP 'disk_filename="\K[^"]+' ../../mk-files/.config)

printf "$WHITE    - Formating disk... $RESET\n"
printf "$WHITE    \t- Disk path: $GREEN%s $RESET\n" $hdd_path
printf "$WHITE    \t- Disk filename: $GREEN%s $RESET\n" $hdd_filename

sudo mkfs.ext2 $hdd_path/$hdd_filename
tmp_mount="/mnt/tmp"
sudo mkdir -p $tmp_mount
sudo mount $hdd_path/$hdd_filename $tmp_mount

sudo mkdir -p $tmp_mount/mnt
sudo mkdir -p $tmp_mount/dev
sudo mkdir -p $tmp_mount/bin
sudo mkdir -p $tmp_mount/proc
sudo mkdir -p $tmp_mount/etc
sudo mkdir -p $tmp_mount/home
sudo mkdir -p $tmp_mount/opt
sudo mkdir -p $tmp_mount/tmp
sudo mkdir -p $tmp_mount/usr
sudo mkdir -p $tmp_mount/var
sudo mkdir -p $tmp_mount/boot
sudo mkdir -p $tmp_mount/lib
sudo mkdir -p $tmp_mount/media
sudo mkdir -p $tmp_mount/mnt
sudo mkdir -p $tmp_mount/run
sudo mkdir -p $tmp_mount/sbin
sudo mkdir -p $tmp_mount/srv
sudo mkdir -p $tmp_mount/sys

ls -l $tmp_mount

sudo umount $tmp_mount
sudo rm -rf $tmp_mount

printf "$WHITE    - Disk formated. $RESET\n"