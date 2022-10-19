# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Dockerfile                                         :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/10/19 15:11:48 by vvaucoul          #+#    #+#              #
#    Updated: 2022/10/19 15:12:45 by vvaucoul         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

FROM debian:latest
MAINTAINER Vincent Vaucouleur <vvaucoul@student.42.fr>

RUN apt-get update
RUN apt-get install build-essential xorriso nasm clang grub -y
RUN apt-get install grub-pc-bin -y

WORKDIR	/kfs