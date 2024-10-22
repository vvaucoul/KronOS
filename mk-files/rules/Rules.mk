# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Rules.mk                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/11/17 01:12:56 by vvaucoul          #+#    #+#              #
#    Updated: 2024/10/20 16:51:50 by vvaucoul         ###   ########.fr        #
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
# ! ||                                     DISKS                                      ||
# ! ||--------------------------------------------------------------------------------||

INITRD_DIR				= 	isodir/boot
INITRD					=	initrd.img

HDA_PATH				=	isodir/boot
HDA_FILENAME			=	$(shell grep -oP 'hda_filename="\K[^"]+' mk-files/.config)
HDA_SIZE				=	$(shell grep -oP 'hda_size="\K[^"]+' mk-files/.config)

HDB_PATH				=	isodir/boot
HDB_FILENAME			=	$(shell grep -oP 'hdb_filename="\K[^"]+' mk-files/.config)
HDB_SIZE				=	$(shell grep -oP 'hdb_size="\K[^"]+' mk-files/.config)

# ! ||--------------------------------------------------------------------------------||
# ! ||                                 COMPILER_RULES                                 ||
# ! ||--------------------------------------------------------------------------------||

# Define default compiler values
CC_DEFAULT				=	i386-elf-gcc
# CXX_DEFAULT			=	i386-elf-gcc

# Try to extract the compiler from mk-files/.config if it exists
CONFIG_FILE			=	mk-files/.config
ifneq ("$(wildcard $(CONFIG_FILE))","")
	COMPILER		=	$(shell grep -oP 'COMPILER="\K[^"]+' $(CONFIG_FILE))
else
	COMPILER		=	$(CC_DEFAULT)
endif

# Set compiler variables based on the COMPILER value
ifeq ($(COMPILER), clang)
	CC					=	clang-15
# CXX					=	clang++
else
	CC					=	$(CC_DEFAULT)
# CXX					=	$(CXX_DEFAULT)
endif