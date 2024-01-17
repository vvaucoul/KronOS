# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    create_disk.sh                                     :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/01/16 17:59:42 by vvaucoul          #+#    #+#              #
#    Updated: 2024/01/16 18:10:33 by vvaucoul         ###   ########.fr        #
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

if command -v qemu-img >/dev/null 2>&1; then
    echo -n ""
else
    printf "$WHITE    - qemu-img not found, please install it.$RESET\n"
    exit 1
fi

hdd_path="../../isodir/boot"
hdd_filename=$(grep -oP 'disk_filename="\K[^"]+' ../../mk-files/.config)
hdd_size=$(grep -oP 'disk_size="\K[^"]+' ../../mk-files/.config)
hdd_format=$(grep -oP 'disk_format="\K[^"]+' ../../mk-files/.config)

printf "$WHITE    - Creating disk...\n"
printf "$WHITE    \t- Disk path: $GREEN%s\n" $hdd_path
printf "$WHITE    \t- Disk filename: $GREEN%s\n" $hdd_filename
printf "$WHITE    \t- Disk size: $GREEN%s\n" $hdd_size
printf "$WHITE    \t- Disk format: $GREEN%s\n" $hdd_format

qemu-img create -f $hdd_format $hdd_path/$hdd_filename $hdd_size >.hdd_output.log

printf "$WHITE    - Disk created. $RESET$DIM(log: .hdd_output.log)\n$RESET"
