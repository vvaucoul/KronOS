# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/06/14 18:51:28 by vvaucoul          #+#    #+#              #
#    Updated: 2023/02/11 23:01:29 by vvaucoul         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

#*******************************************************************************
#*                                  INLCUDES                                   *
#*******************************************************************************

MK_INCLUDE_DIR	=	mk-files

include $(MK_INCLUDE_DIR)/utils/Colors.mk
include $(MK_INCLUDE_DIR)/dependencies/ShellRules-Dependencies.mk
include $(MK_INCLUDE_DIR)/rules/Rules.mk

include $(MK_INCLUDE_DIR)/includes/Headers.mk

include $(MK_INCLUDE_DIR)/sources/Sources-Boot.mk
include $(MK_INCLUDE_DIR)/sources/Sources.mk
include $(MK_INCLUDE_DIR)/sources/Sources-Workflows.mk
include $(MK_INCLUDE_DIR)/sources/Sources-ASM.mk

#*******************************************************************************
#*                                     VAR                                     *
#*******************************************************************************

ifeq ($(CHECK_CLANG_INSTALL), false)
	CLANG_INSTALLED	=	false
else
	CLANG_INSTALLED	=	true
endif

NAME				=	kfs
ISO					=	$(NAME).iso

LD					=	ld
INLCUDES_PATH		=	-I./kernel/includes/ \
						-I./$(LIBKFS_DIR)/$(LIBKFS_DIR)/
CFLAGS				=	-Wall -Wextra -Wfatal-errors \
						-fno-builtin -fno-exceptions -fno-stack-protector \
						-nostdlib -nodefaultlibs \
						-std=c99 -ffreestanding -O2 
LDFLAGS				= 	-g3 -m32
LD_FLAGS			=	-m elf_i386

OBJS_DIR			=	objs
DEPENDS_DIR			=	depends

ASM					=	nasm
ASMFLAGS			=	-f elf32

KDSRCS				=	srcs/kernel
HEADERS_DIR			=	kernel/includes/
BIN					=	kernel.bin
BIN_DIR				=	bin

XORRISO				=	xorriso-1.4.6
XORRISO_INSTALLED	=	$(CHECK_XORRISO_INSTALL)

CCACHE_INSTALLED	=	$(CHECK_CCACHE_INSTALL)
CCACHE_DIR			=	ccache

ifeq ($(CCACHE_INSTALLED), false)
	CCACHE			=	./$(DEPENDENCIES_DIR)/$(CCACHE_DIR)/ccache
else
	CCACHE 			=	ccache
endif

# Todo: Check clang version / do not use gcc -> error
ifeq ($(CLANG_INSTALLED), false)
	CC				=	$(CCACHE) gcc
else
	CC				=	$(CCACHE) clang
endif

DEPENDS				=	$(KOBJS:.o=.d)
WDEPENDS			=	$(WOBJS:.o=.d)
DEPENDS_ASM			=	$(KOBJS_ASM:.o=.d)

#*******************************************************************************
#*                                    KSRCS                                    *
#*******************************************************************************

%.o: %.c
	@printf "$(_LWHITE) $(_DIM)- Compiling: $(_END)$(_DIM)--------$(_END)$(_LCYAN) %s $(_END)$(_LGREEN)[$(_LWHITE)✓$(_LGREEN)]$(_END)\n" $< 
	@$(CC) $(LDFLAGS) $(CFLAGS) $(INLCUDES_PATH) -MD -c $< -o ${<:.c=.o}

%.o: %.s
	@printf "$(_LWHITE) $(_DIM)- Compiling: $(_END)$(_DIM)--------$(_END)$(_LPURPLE) %s $(_END)$(_LGREEN)[$(_LWHITE)✓$(_LGREEN)]$(_END)\n" $< 
	@$(ASM) $(ASMFLAGS) $< -o ${<:.s=.o}

#*******************************************************************************
#*                                    RULES                                    *
#*******************************************************************************

all: $(NAME)

$(NAME): ascii $(XORRISO) $(CCACHE) lkfs $(BOOT) $(KDSRCS) $(HEADERS) $(BIN_DIR)/$(BIN) $(ISO) helper
	@true

lkfs-install:
	@sh $(SCRIPTS_DIR)/installHephaistos.sh
	
lkfs: lkfs-install
	@make -s -C $(LIBKFS_DIR) CLANG_INSTALLED=$(CLANG_INSTALLED) CCACHE_INSTALLED=$(CCACHE_INSTALLED)
	@printf "$(_LWHITE)- LIBKFS$(_END)$(_END)$(_DIM)-----------------$(_END) $(_LGREEN)[$(_LWHITE)✓$(_LGREEN)]$(_END)\n"

.PHONY: lkfs

$(BOOT): $(KBOOT_OBJS)
	@printf "$(_LWHITE)- ASM BOOT $(_END)$(_DIM)--------------$(_END) $(_LGREEN)[$(_LWHITE)✓$(_LGREEN)]$(_END)\n"

$(KDSRCS): $(KOBJS) $(KOBJS_ASM) $(WOBJS)
	@printf "$(_LWHITE)- KERNEL SRCS $(_END)$(_DIM)-----------$(_END) $(_LGREEN)[$(_LWHITE)✓$(_LGREEN)]$(_END)\n"

$(XORRISO):
	@sh $(SCRIPTS_DIR)/installXorriso.sh

$(CCACHE):
	@sh $(SCRIPTS_DIR)/installCcache.sh

check:
	@grub-file --is-x86-multiboot $(BIN_DIR)/$(BIN) && printf "$(_LWHITE)- $(BIN) $(_END)$(_DIM)------------$(_END) $(_LGREEN)[$(_LWHITE)✓$(_LGREEN)]$(_END)" || printf "$(_LWHITE)- $(BIN) $(_END)$(_DIM)------------$(_END) $(_LRED)[$(_LWHITE)✗$(_LRED)]$(_END)"
	printf "$(_END)$(_DIM) -> ISO CHECKER $(_END)\n"

clean:
	@make -s -C $(LIBKFS_DIR) clean
	@make -s -C . clean-disk
	@rm -rf $(NAME).iso $(KBOOT_OBJS) isodir $(BIN_DIR)/$(BIN) $(KOBJS) $(KOBJS_ASM) $(WOBJS) $(BIN) $(DEPENDS) $(WDEPENDS) $(DEPENDS_ASM)
	@printf "$(_LWHITE)- CLEAN $(_END)$(_DIM)-----------------$(_END) $(_LGREEN)[$(_LWHITE)✓$(_LGREEN)]$(_END)\n"

fclean: clean docker-clear
	@make -s -C $(LIBKFS_DIR) fclean
	@rm -rf $(DEPENDENCIES_DIR)/$(XORRISO) $(BIN_DIR) $(DEPENDENCIES_DIR)/$(CCACHE_DIR)
	@printf "$(_LWHITE)- FCLEAN $(_END)$(_DIM)----------------$(_END) $(_LGREEN)[$(_LWHITE)✓$(_LGREEN)]$(_END)\n"

re: clean
	@rm -rf $(BIN_DIR)
	@make -s -C $(LIBKFS_DIR) re #> /dev/null 2>&1
	@make -s -C . all

ascii:
	@printf "$(_LRED)\r██╗  ██╗███████╗███████╗$(_LWHITE)      $(_LRED)███████╗\n$(_END)"
	@printf "$(_LRED)\r██║ ██╔╝██╔════╝██╔════╝$(_LWHITE)      $(_LRED)██╔════╝\n$(_END)"
	@printf "$(_LRED)\r█████╔╝ █████╗  ███████╗$(_LWHITE)█████╗$(_LRED)███████╗\n$(_END)"
	@printf "$(_LRED)\r██╔═██╗ ██╔══╝  ╚════██║$(_LWHITE)╚════╝$(_LRED)╚════██║\n$(_END)"
	@printf "$(_LRED)\r██║  ██╗██║     ███████║$(_LWHITE)      $(_LRED)███████║\n$(_END)"
	@printf "$(_LRED)\r╚═╝  ╚═╝╚═╝     ╚══════╝$(_LWHITE)      $(_LRED)╚══════╝\n$(_END)"

helper:
	@printf "\n$(_LWHITE)- Now you use: '$(_LYELLOW)make run$(_END)$(_LWHITE)' or '$(_LYELLOW)make run-iso$(_END)$(_LWHITE)' to start the kernel !$(_END)\n"

include $(MK_INCLUDE_DIR)/kernel-starter/QEMU-Runner.mk
include $(MK_INCLUDE_DIR)/docker/Docker.mk
include $(MK_INCLUDE_DIR)/kernel-maker/Kernel-Maker.mk
include $(MK_INCLUDE_DIR)/dependencies/Dependencies.mk
include $(MK_INCLUDE_DIR)/tiny-kernels/tiny-kernels.mk

-include $(DEPENDS)
-include $(WDEPENDS)
-include $(DEPENDS_ASM)

.PHONY: all clean fclean re debug ascii helper