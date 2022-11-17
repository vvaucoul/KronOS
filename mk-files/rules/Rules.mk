# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Rules.mk                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/11/17 01:12:56 by vvaucoul          #+#    #+#              #
#    Updated: 2022/11/17 01:15:33 by vvaucoul         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

KERNEL_DIR				=	kernel

KERNEL_X86_DIR			=	$(KERNEL_DIR)/arch/x86

KERNEL_BOOT_DIR			=	$(KERNEL_X86_DIR)/boot
KERNEL_CONFIG_DIR		=	$(KERNEL_X86_DIR)/config
KERNEL_LINKERS_DIR		=	$(KERNEL_X86_DIR)/linkers

KERNEL_SRCS_DIR			=	$(KERNEL_DIR)/srcs
KERNEL_INCLUDES_DIR		=	$(KERNEL_DIR)/includes
KERNEL_WORKFLOWS_DIR	=	$(KERNEL_DIR)/workflows

LIBKFS_DIR				=	libkfs