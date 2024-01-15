# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Rules.mk                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/11/17 01:12:56 by vvaucoul          #+#    #+#              #
#    Updated: 2024/01/13 12:32:30 by vvaucoul         ###   ########.fr        #
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

LIBKFS_DIR				=	Hephaistos
LIBKFS					=	$(LIBKFS_DIR)/Hephaistos.a

DEPENDENCIES_DIR		=	dependencies

# ! ||--------------------------------------------------------------------------------||
# ! ||                                   FILESYSTEM                                   ||
# ! ||--------------------------------------------------------------------------------||

INITRD_DIR				= 	isodir/boot
INITRD					=	initrd.img

# DISK_PATH				=	isodir/boot
# DISK_NAME				=	floppy.img
# DISK_SIZE				=	256M

# VFS_CLEAN_SCRIPT		=	./utils/vfs/clean.sh

HDD_PATH				=	isodir/boot
HDD_FILENAME			=	hdd.img
HDD_SIZE				=	256M