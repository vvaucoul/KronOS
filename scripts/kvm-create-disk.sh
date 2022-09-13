# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    kvm-create-disk.sh                                 :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/09/12 19:38:09 by vvaucoul          #+#    #+#              #
#    Updated: 2022/09/12 20:12:35 by vvaucoul         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

#!/bin/sh

DISKNAME=$1
DISKSIZE=$2
DISKPATH=$3

if [ -z $DISKNAME ] || [ -z $DISKSIZE ] || [ -z $DISKPATH ]; then
    echo "Usage: $0 <diskname> <disksize> <diskpath>"
    exit 1
fi

if [ -f $DISKPATH/$DISKNAME ]; then
    echo "Disk $DISKNAME already exists"
    exit 1
fi

qemu-img create -f raw $DISKPATH/$DISKNAME $DISKSIZE > /dev/null 2>&1