# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Dependencies.mk                                    :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/08/29 20:39:54 by vvaucoul          #+#    #+#              #
#    Updated: 2022/11/17 12:02:49 by vvaucoul         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

#******************************************************************************#
#*                            INSTALL DEPENDENCIES                            *#
#******************************************************************************#

DEPENDENCIES_DIR	=	dependencies

install-dependencies:
	@sudo apt-get install grub-common
	@sudo apt-get install nasm
	@sudo apt-get install mtools
	@sudo apt-get install xorriso
	@sudo apt-get install ccache
	@sudo apt-get install clang
	@sudo apt-get update && sudo apt-get upgrade
	
