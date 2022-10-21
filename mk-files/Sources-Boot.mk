# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Sources-Boot.mk                                    :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/06/27 17:36:21 by vvaucoul          #+#    #+#              #
#    Updated: 2022/10/20 18:41:06 by vvaucoul         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

#*******************************************************************************
#*                                    KBOOT                                    *
#*******************************************************************************

BOOT_DIR		=	boot
MULTIBOOT_DIR	=	boot/multiboot

KBOOT_SRCS		=	$(BOOT).s \
					boot/boot_error.s

ifeq ($(CHECK_HIGHER_HALF_KERNEL), false)
	KBOOT_SRCS	+=	$(MULTIBOOT_DIR)/lhk_multiboot.s
else
	KBOOT_SRCS	+=	$(MULTIBOOT_DIR)/hhk_multiboot.s
endif

KBOOT_OBJS		=	$(KBOOT_SRCS:.s=.o)