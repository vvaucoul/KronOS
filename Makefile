# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/06/14 18:51:28 by vvaucoul          #+#    #+#              #
#    Updated: 2022/09/13 12:20:27 by vvaucoul         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

#*******************************************************************************
#*                                  INLCUDES                                   *
#*******************************************************************************

MK_INCLUDE_DIR	=	mk-files

include $(MK_INCLUDE_DIR)/Colors.mk
include $(MK_INCLUDE_DIR)/Sources-Boot.mk
include $(MK_INCLUDE_DIR)/Sources.mk
include $(MK_INCLUDE_DIR)/Sources-ASM.mk
include $(MK_INCLUDE_DIR)/Headers.mk
include $(MK_INCLUDE_DIR)/ShellRules-Dependencies.mk

#*******************************************************************************
#*                                     VAR                                     *
#*******************************************************************************

NAME				=	kfs
ISO					=	$(NAME).iso
LIBKFS				=	lkfs
LIBKFS_A			=	libkfs/libkfs.a
CC					=	clang
LD					=	ld
INLCUDES_PATH		=	-I./kernel/includes/ \
						-I./libkfs/includes/
CFLAGS				=	-Wall -Wextra -Wfatal-errors \
						-fno-builtin -fno-exceptions -fno-stack-protector \
						-nostdlib -nodefaultlibs \
						-std=gnu99 -ffreestanding -O2
LDFLAGS				= 	-g3 -m32
LD_FLAGS			=	-m elf_i386

OBJS_DIR			=	objs
DEPENDS_DIR			=	depends

ASM					=	nasm
ASMFLAGS			=	-f elf32

BOOT_DIR			=	boot

ifeq ($(CHECK_HIGHER_HALF_KERNEL), false)
	BOOT			=	$(BOOT_DIR)/lowerHalfKernel
else
	BOOT			=	$(BOOT_DIR)/boot
endif

KDSRCS				=	srcs/kernel
HEADERS_DIR			=	kernel/includes/
BIN					=	kernel.bin
BIN_DIR				=	bin

ifeq ($(CHECK_HIGHER_HALF_KERNEL), false)
	LINKER			=	$(LINKER_DIR)/linker.ld
else
	LINKER			=	$(LINKER_DIR)/HigherHalfLinker.ld
endif

XORRISO				=	xorriso-1.4.6
XORRISO_INSTALLED	=	$(CHECK_XORRISO_INSTALL)

DEPENDS				=	$(KOBJS:.o=.d)
DEPENDS_ASM			=	$(KOBJS_ASM:.o=.d)

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
	@sh $(SCRIPTS_DIR)/installXorriso.sh

check:
	@grub-file --is-x86-multiboot $(BIN_DIR)/$(BIN) && printf "$(_LWHITE)- $(BIN) $(_END)$(_DIM)------------$(_END) $(_LGREEN)[$(_LWHITE)✓$(_LGREEN)]$(_END)" || printf "$(_LWHITE)- $(BIN) $(_END)$(_DIM)------------$(_END) $(_LRED)[$(_LWHITE)✗$(_LRED)]$(_END)"
	printf "$(_END)$(_DIM) -> ISO CHECKER $(_END)\n"

clean:
	@make -s -C libkfs clean
	@make -s -C . clean-disk
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
include $(MK_INCLUDE_DIR)/Dependencies.mk

-include $(DEPENDS)
-include $(DEPENDS_ASM)

.PHONY: all clean fclean re debug ascii helper