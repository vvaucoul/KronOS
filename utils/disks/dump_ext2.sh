# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    dump_ext2.sh                                       :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/01/19 18:10:07 by vvaucoul          #+#    #+#              #
#    Updated: 2024/02/13 14:25:52 by vvaucoul         ###   ########.fr        #
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

if command -v dumpe2fs >/dev/null 2>&1; then
    echo -n ""
else
    printf "$WHITE    - dumpe2fs not found, please install it.$RESET\n"
    exit 1
fi

hda_path="../../isodir/boot"
hda_filename=$(grep -oP 'hda_filename="\K[^"]+' ../../mk-files/.config)

printf "$WHITE    - Dumping disk... $RESET\n"

sudo mkdir -p /mnt/tmp
sudo mount $hda_path/$hda_filename /mnt/tmp

printf "$WHITE    - Superblock: $GREEN%s $RESET\n"
sudo dumpe2fs $hda_path/$hda_filename

# superblock_offset=$(sudo dumpe2fs $hdd_path/$hdd_filename | grep -oP 'Block group: \K[^"]+')
# printf "$WHITE    - Superblock offset: $GREEN%s $RESET\n" $superblock_offset

sudo umount /mnt/tmp