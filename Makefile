# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/06/14 18:51:28 by vvaucoul          #+#    #+#              #
#    Updated: 2022/08/29 20:21:37 by vvaucoul         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

#*******************************************************************************
#*                                     VAR                                     *
#*******************************************************************************

NAME			=	kfs
ISO				=	$(NAME).iso
LIBKFS			=	lkfs
LIBKFS_A		=	libkfs/libkfs.a
CC				=	clang
LD				=	ld
INLCUDES_PATH	=	-I./kernel/includes/
CFLAGS			=	-Wall -Wextra -Wfatal-errors \
					-fno-builtin -fno-exceptions -fno-stack-protector \
					-nostdlib -nodefaultlibs \
					-std=gnu99 -ffreestanding -O2 
LDFLAGS			= 	-g3 -m32
LD_FLAGS		=	-m elf_i386

ASM				=	nasm
ASMFLAGS		=	-f elf32
BOOT			=	boot/boot
KDSRCS			=	srcs/kernel
HEADERS_DIR		=	kernel/includes/
BIN				=	kernel.bin
BIN_DIR			=	bin
LINKER			=	linker.ld

XORRISO			=	xorriso-1.4.6
MK_INCLUDE_DIR	=	mk-files

DEPENDS			=	$(KOBJS:.o=.d)

#*******************************************************************************
#*                                  INLCUDES                                   *
#*******************************************************************************

include $(MK_INCLUDE_DIR)/Colors.mk
include $(MK_INCLUDE_DIR)/Sources-Boot.mk
include $(MK_INCLUDE_DIR)/Sources.mk
include $(MK_INCLUDE_DIR)/Sources-ASM.mk
include $(MK_INCLUDE_DIR)/Headers.mk

#*******************************************************************************
#*                                    KSRCS                                    *
#*******************************************************************************

%.o: %.c
	@printf "$(_LWHITE)    $(_DIM)- Compiling: $(_END)$(_DIM)--------$(_END)$(_LCYAN) %s $(_END)$(_LGREEN)[$(_LWHITE)✓$(_LGREEN)]$(_END)\n" $< 
	@$(CC) $(LDFLAGS) $(CFLAGS) $(INLCUDES_PATH) -MD -c $< -o ${<:.c=.o}

%.o: %.s
	@printf "$(_LWHITE)    $(_DIM)- Compiling: $(_END)$(_DIM)--------$(_END)$(_LPURPLE) %s $(_END)$(_LGREEN)[$(_LWHITE)✓$(_LGREEN)]$(_END)\n" $< 
	@$(ASM) $(ASMFLAGS) $< -o ${<:.s=.o}

#*******************************************************************************
#*                                    RULES                                    *
#*******************************************************************************

all: $(NAME)

$(NAME): ascii $(XORRISO) $(LIBKFS) $(BOOT) $(KDSRCS) $(HEADERS) $(BIN_DIR)/$(BIN) $(ISO) helper
	@true

$(LIBKFS):
	@make -s -C libkfs
	@printf "$(_LWHITE)- LIBKFS$(_END)$(_END)$(_DIM)-----------------$(_END) $(_LGREEN)[$(_LWHITE)✓$(_LGREEN)]$(_END)\n"

$(BOOT): $(KBOOT_OBJS)
	@printf "$(_LWHITE)- ASM BOOT $(_END)$(_DIM)--------------$(_END) $(_LGREEN)[$(_LWHITE)✓$(_LGREEN)]$(_END)\n"

$(KDSRCS): $(KOBJS) $(KOBJS_ASM)
	@printf "$(_LWHITE)- KERNEL SRCS $(_END)$(_DIM)-----------$(_END) $(_LGREEN)[$(_LWHITE)✓$(_LGREEN)]$(_END)\n"

$(XORRISO):
	@printf "$(_LYELLOW)- COMPILING XORRISO $(_END)$(_DIM)-----$(_END) $(_LYELLOW)[$(_LWHITE)⚠️ $(_LYELLOW)]$(_END) $(_LYELLOW)>$(_END)$(_DIM)$(shell pwd)/$(XORRISO)/$(_END)$(_LYELLOW)<$(_END)\n"
	@tar xf $(XORRISO).tar.gz
	@cd $(XORRISO) && ./configure --prefix=/usr > /dev/null 2>&1 && make -j$(nproc) > /dev/null 2>&1
	@printf "$(_LWHITE)- $(XORRISO) $(_END)$(_DIM)---------$(_END) $(_LGREEN)[$(_LWHITE)✓$(_LGREEN)]$(_END)\n"

check:
	@grub-file --is-x86-multiboot $(BIN_DIR)/$(BIN) && printf "$(_LWHITE)- $(BIN) $(_END)$(_DIM)------------$(_END) $(_LGREEN)[$(_LWHITE)✓$(_LGREEN)]$(_END)" || printf "$(_LWHITE)- $(BIN) $(_END)$(_DIM)------------$(_END) $(_LRED)[$(_LWHITE)✗$(_LRED)]$(_END)"
	printf "$(_END)$(_DIM) -> ISO CHECKER $(_END)\n"

clean:
	@make -s -C libkfs clean
	@rm -rf $(NAME).iso $(KBOOT_OBJS) isodir $(BIN_DIR)/$(BIN) $(KOBJS) $(KOBJS_ASM) $(BIN) $(DEPENDS)
	@printf "$(_LWHITE)- CLEAN $(_END)$(_DIM)-----------------$(_END) $(_LGREEN)[$(_LWHITE)✓$(_LGREEN)]$(_END)\n"

fclean: clean docker-clear
	@make -s -C libkfs fclean
	@rm -rf $(XORRISO) $(BIN_DIR)
	@printf "$(_LWHITE)- FCLEAN $(_END)$(_DIM)----------------$(_END) $(_LGREEN)[$(_LWHITE)✓$(_LGREEN)]$(_END)\n"

re: clean
	@rm -rf $(BIN_DIR)
	@make -s -C libkfs re > /dev/null 2>&1
	@make -s -C . all

ascii:
	@printf "$(_LRED)\r██╗  ██╗███████╗███████╗$(_LWHITE)     $(_LRED)██████╗ \n$(_END)"
	@printf "$(_LRED)\r██║ ██╔╝██╔════╝██╔════╝$(_LWHITE)     $(_LRED)╚════██╗\n$(_END)"
	@printf "$(_LRED)\r█████╔╝ █████╗  ███████╗$(_LWHITE)█████╗$(_LRED)█████╔╝\n$(_END)"
	@printf "$(_LRED)\r██╔═██╗ ██╔══╝  ╚════██║$(_LWHITE)╚════╝$(_LRED)╚═══██╗\n$(_END)"
	@printf "$(_LRED)\r██║  ██╗██║     ███████║$(_LWHITE)     $(_LRED)██████╔╝\n$(_END)"
	@printf "$(_LRED)\r╚═╝  ╚═╝╚═╝     ╚══════╝$(_LWHITE)     $(_LRED)╚═════╝ \n$(_END)"

helper:
	@printf "\n$(_LWHITE)- Now you use: '$(_LYELLOW)make run$(_END)$(_LWHITE)' or '$(_LYELLOW)make run-iso$(_END)$(_LWHITE)' to start the kernel !$(_END)\n"

include $(MK_INCLUDE_DIR)/QEMU-Runner.mk
include $(MK_INCLUDE_DIR)/Docker.mk
include $(MK_INCLUDE_DIR)/Kernel-Maker.mk

-include $(DEPENDS)

.PHONY: all clean fclean re debug ascii helper