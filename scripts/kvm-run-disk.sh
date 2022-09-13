# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    kvm-run-disk.sh                                    :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/09/12 19:42:30 by vvaucoul          #+#    #+#              #
#    Updated: 2022/09/12 19:43:48 by vvaucoul         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

#!/bin/sh

DISKNAME=$1
DISKPATH=$2
FLAGS=$3

if [ -z $DISKNAME ] || [ -z $DISKPATH ]; then
    echo "Usage: $0 <diskname> <diskpath>"
    exit 1
fi

if [ ! -f $DISKPATH/$DISKNAME ]; then
    echo "Disk $DISKNAME doesn't exists"
    exit 1
fi

qemu-system-i386 $FLAGS -hda $DISKPATH/$DISKNAME