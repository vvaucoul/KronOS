# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    clear.sh                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/07/03 16:29:33 by vvaucoul          #+#    #+#              #
#    Updated: 2022/07/03 17:34:27 by vvaucoul         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

#!/bin/sh
docker stop vvaucoul-kfs
docker rm vvaucoul-kfs
rm -rf kernel.bin kfs.iso build