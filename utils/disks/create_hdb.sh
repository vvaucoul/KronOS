# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    create_hdb.sh                                      :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/01/16 17:59:42 by vvaucoul          #+#    #+#              #
#    Updated: 2024/02/13 14:30:54 by vvaucoul         ###   ########.fr        #
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

hdb_path="../../isodir/boot"
hdb_filename=$(grep -oP 'hdb_filename="\K[^"]+' ../../mk-files/.config)
hdb_size=$(grep -oP 'hdb_size="\K[^"]+' ../../mk-files/.config)
hdb_format=$(grep -oP 'hdb_format="\K[^"]+' ../../mk-files/.config)

printf "$WHITE    - Creating disk...\n"
printf "$WHITE    \t- Disk path: $GREEN%s\n" $hdb_path
printf "$WHITE    \t- Disk filename: $GREEN%s\n" $hdb_filename
printf "$WHITE    \t- Disk size: $GREEN%s\n" $hdb_size
printf "$WHITE    \t- Disk format: $GREEN%s\n" $hdb_format

qemu-img create -f $hdb_format $hdb_path/$hdb_filename $hdb_size >.hdb_output.log

printf "$WHITE    - Disk created. $RESET$DIM(log: .hdb_output.log)\n$RESET"
