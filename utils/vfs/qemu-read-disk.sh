# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    qemu-read-disk.sh                                  :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/01/10 20:36:05 by vvaucoul          #+#    #+#              #
#    Updated: 2024/01/10 20:38:25 by vvaucoul         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

if [ $# -ne 1 ]; then
    echo "Usage: $0 <disk.img>"
    exit 1
fi

disk=$1

sudo modprobe nbd
sudo modprobe nbd nbds_max=128

sudo qemu-nbd --connect=/dev/nbd0 $disk
sudo mount /dev/nbd0 /mnt
sudo ls -la /mnt
sudo umount /mnt
sudo qemu-nbd --disconnect /dev/nbd0