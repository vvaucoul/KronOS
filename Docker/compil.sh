# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    compil.sh                                          :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/07/03 16:32:59 by vvaucoul          #+#    #+#              #
#    Updated: 2022/10/12 15:00:20 by vvaucoul         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

make -C ../ fclean
sh Docker/clear.sh 2>&1

mkdir -pv build

cp -r ../Makefile build/Makefile
cp -r ../config build/config
cp -r ../linkers build/linkers
cp -r ../boot build/boot
cp -r ../kernel build/kernel
cp -r ../libkfs build/libkfs
cp -r ../mk-files build/mk-files
cp -r ../xorriso-1.4.6.tar.gz build/xorriso-1.4.6.tar.gz
cp -r ../scripts build/scripts

docker stop vvaucoul-kfs
docker rm vvaucoul-kfs
docker build --platform linux/amd64 -t vvaucoul-kfs .

docker run -d --name vvaucoul-kfs --rm -i -t vvaucoul-kfs
docker cp build/ vvaucoul-kfs:/kfs

docker exec -t vvaucoul-kfs make -C ./build

docker cp vvaucoul-kfs:/kfs/build/isodir/boot/kernel.bin .
docker cp vvaucoul-kfs:/kfs/build/kfs.iso .
