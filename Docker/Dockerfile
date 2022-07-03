# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Dockerfile                                         :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/07/03 16:04:26 by vvaucoul          #+#    #+#              #
#    Updated: 2022/07/03 16:31:58 by vvaucoul         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

FROM debian:latest
MAINTAINER Vincent Vaucouleur <vvaucoul@student.42.fr>

RUN apt-get update
RUN apt-get install build-essential xorriso nasm clang grub -y
RUN apt-get install grub-pc-bin -y

WORKDIR	/kfs