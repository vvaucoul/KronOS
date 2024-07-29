# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Sources-Boot.mk                                    :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/06/27 17:36:21 by vvaucoul          #+#    #+#              #
#    Updated: 2024/07/29 15:21:05 by vvaucoul         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

#*******************************************************************************
#*                                    KBOOT                                    *
#*******************************************************************************

BOOT_DIR_ERROR 	=	$(KERNEL_BOOT_DIR)/errors
MULTIBOOT_DIR	=	$(KERNEL_BOOT_DIR)/multiboot

KBOOT_SRCS		=	$(BOOT).s \
					$(KERNEL_BOOT_DIR)/stack.s \
					$(BOOT_DIR_ERROR)/boot_error.s

ifeq ($(CHECK_HIGHER_HALF_KERNEL), false)
	KBOOT_SRCS	+=	$(MULTIBOOT_DIR)/lhk_multiboot.s
else
	KBOOT_SRCS	+=	$(MULTIBOOT_DIR)/hhk_multiboot.s
endif

KBOOT_OBJS		=	$(KBOOT_SRCS:.s=.o)

#
# SETUP BOOT FILE
#

ifeq ($(CHECK_HIGHER_HALF_KERNEL), false)
	BOOT			=	$(KERNEL_BOOT_DIR)/lowerHalfKernel
else
	BOOT			=	$(KERNEL_BOOT_DIR)/boot
endif

#
# SETUP LINKER FILE
#

ifeq ($(CHECK_HIGHER_HALF_KERNEL), false)
	LINKER			=	$(LINKER_DIR)/linker.ld
else
	LINKER			=	$(LINKER_DIR)/HigherHalfLinker.ld
endif
