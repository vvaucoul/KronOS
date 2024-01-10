# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    check_image_integrity.sh                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/01/09 23:33:26 by vvaucoul          #+#    #+#              #
#    Updated: 2024/01/09 23:55:12 by vvaucoul         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

if [ -n "$1" ]; then
    floppy_path="$1"
else
    floppy_path="../../isodir/boot/floppy.img"
fi

sudo mkdir -p /mnt/floppy
sudo mount -o loop "$floppy_path" /mnt/floppy

echo "Checking floppy.img..."
sudo ls -lR /mnt/floppy

sudo umount /mnt/floppy
sudo rm -rf /mnt/floppy