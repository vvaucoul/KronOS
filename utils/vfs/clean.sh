# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    clean.sh                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/10/25 11:56:01 by vvaucoul          #+#    #+#              #
#    Updated: 2023/10/25 12:01:44 by vvaucoul         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

#!/bin/bash

dir="utils/vfs"

rm -f "$dir/boot.iso" "$dir/floppy.img" "$dir/initrd.img"
rm -f "$dir/ext2/make_initrd" "$dir/ext2/initrd.img"
