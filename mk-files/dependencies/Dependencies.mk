# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Dependencies.mk                                    :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/08/29 20:39:54 by vvaucoul          #+#    #+#              #
#    Updated: 2024/07/31 14:15:20 by vvaucoul         ###   ########.fr        #
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
	@sudo apt-get install clang clang-15 gcc-12 -y
	@sudo apt-get install build-essential gdb -y
	@sudo apt install qemu-kvm libvirt-daemon-system libvirt-clients bridge-utils virt-manager -y
	@sudo usermod -aG libvirt $(USER)
	@sudo apt-get update && sudo apt-get upgrade -y