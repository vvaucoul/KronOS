# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    compil-docker.sh                                   :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/10/19 15:11:54 by vvaucoul          #+#    #+#              #
#    Updated: 2022/10/19 15:19:29 by vvaucoul         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

mkdir -pv build

cp -r srcs build/srcs
cp compil.sh build/compil.sh

# docker stop vvaucoul-kfs
# docker rm vvaucoul-kfs
docker build --platform linux/amd64 -t vvaucoul-kfs .

docker run -d --name vvaucoul-kfs --rm -i -t vvaucoul-kfs
docker cp build/ vvaucoul-kfs:/kfs

docker exec -t vvaucoul-kfs sh compil.sh

docker cp vvaucoul-kfs:/kfs/build/isodir/boot/kernel.bin .
docker cp vvaucoul-kfs:/kfs/build/kfs.iso .
