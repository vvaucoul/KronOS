# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    create_hda.sh                                      :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/01/16 17:59:42 by vvaucoul          #+#    #+#              #
#    Updated: 2024/02/13 14:23:01 by vvaucoul         ###   ########.fr        #
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

hda_path="../../isodir/boot"
hda_filename=$(grep -oP 'hda_filename="\K[^"]+' ../../mk-files/.config)
hda_size=$(grep -oP 'hda_size="\K[^"]+' ../../mk-files/.config)
hda_format=$(grep -oP 'hda_format="\K[^"]+' ../../mk-files/.config)

printf "$WHITE    - Creating disk...\n"
printf "$WHITE    \t- Disk path: $GREEN%s\n" $hda_path
printf "$WHITE    \t- Disk filename: $GREEN%s\n" $hda_filename
printf "$WHITE    \t- Disk size: $GREEN%s\n" $hda_size
printf "$WHITE    \t- Disk format: $GREEN%s\n" $hda_format

qemu-img create -f $hda_format $hda_path/$hda_filename $hda_size >.hda_output.log

printf "$WHITE    - Disk created. $RESET$DIM(log: .hda_output.log)\n$RESET"
