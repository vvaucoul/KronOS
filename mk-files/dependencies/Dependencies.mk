# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Dependencies.mk                                    :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/08/29 20:39:54 by vvaucoul          #+#    #+#              #
#    Updated: 2023/06/01 16:52:37 by vvaucoul         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

#******************************************************************************#
#*                            INSTALL DEPENDENCIES                            *#
#******************************************************************************#

DEPENDENCIES_DIR	=	dependencies

install-dependencies:
	@sudo apt-get update && sudo apt-get upgrade -y
	@sudo apt-get install grub-common -y
	@sudo apt-get install nasm -y
	@sudo apt-get install mtools -y
	@sudo apt-get install xorriso -y
	@sudo apt-get install ccache -y
	@sudo apt-get install clang -y
	@sudo apt install qemu-kvm libvirt-daemon-system libvirt-clients bridge-utils virt-manager -y
	@sudo usermod -aG libvirt $USER
	@sudo apt-get update && sudo apt-get upgrade -y