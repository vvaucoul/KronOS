# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    flags.mk                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/10/19 19:17:24 by vvaucoul          #+#    #+#              #
#    Updated: 2022/11/04 15:28:12 by vvaucoul         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

INCLUDES			=	-I srcs

CFLAGS				=	-Wall -Wextra -Wfatal-errors \
						-fno-builtin -fno-exceptions -fno-stack-protector \
						-nostdlib -nodefaultlibs \
						-std=gnu99 -ffreestanding -O2 $(INCLUDES)

LDFLAGS				= 	-g3 -m32
LD_FLAGS			=	-m elf_i386

ASM					=	nasm
ASMFLAGS			=	-f elf32

BOOT_DIR			=	boot

LD					=	ld

CC					=	clang