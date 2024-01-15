# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    create_initrd.sh                                   :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/01/13 12:24:31 by vvaucoul          #+#    #+#              #
#    Updated: 2024/01/13 14:09:48 by vvaucoul         ###   ########.fr        #
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

initd="initrd.img"

if [ -f "create_initrd.py" ] && [ -f "inspect_initrd.py" ]; then

    if [ -f "initrd.img" ]; then
        printf "    $RED- initrd.img already exist, deleting it...$RESET\n"
        rm initrd.img
    fi

    printf "$WHITE    - Creating initrd.img...$RESET\n"
    python3 create_initrd.py

    printf "$WHITE    - Inspecting initrd.img...$RESET\n\n"
    python3 inspect_initrd.py


    mkdir ../../../isodir/boot/ -p
    cp initrd.img ../../../isodir/boot/initrd.img
    printf "\n$WHITE    - Copied initrd.img to ../../../isodir/boot/initrd.img$RESET\n"
    
    printf "$WHITE    - Done.$RESET\n"

    exit 0
else
    printf "$WHITE One or both files are missing.$RESET\n"
    exit 1
fi
