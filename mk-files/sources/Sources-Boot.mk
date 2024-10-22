# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Sources-Boot.mk                                    :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/06/27 17:36:21 by vvaucoul          #+#    #+#              #
#    Updated: 2024/10/20 15:23:58 by vvaucoul         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

#*******************************************************************************
#*                                    KBOOT                                    *
#*******************************************************************************

BOOT_DIR_ERROR 	=	$(KERNEL_BOOT_DIR)/errors
MULTIBOOT_DIR	=	$(KERNEL_BOOT_DIR)/multiboot

KBOOT_SRCS		=	$(BOOT).s 

KBOOT_OBJS		=	$(KBOOT_SRCS:.s=.o)

#
# SETUP BOOT FILE
#

BOOT			=	$(KERNEL_BOOT_DIR)/boot

#
# SETUP LINKER FILE
#

LINKER			=	$(LINKER_DIR)/linker.ld
