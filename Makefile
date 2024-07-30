# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/06/14 18:51:28 by vvaucoul          #+#    #+#              #
#    Updated: 2024/07/31 01:26:35 by vvaucoul         ###   ########.fr        #
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
						-I./userspace/includes/ \
						-I./$(LIBKFS_DIR)/$(LIBKFS_DIR)/include
CFLAGS				=	-Wall -Wextra -Wfatal-errors -Wimplicit-function-declaration -Wincompatible-pointer-types \
						-fno-builtin -fno-exceptions -fno-stack-protector \
						-nostdlib -nodefaultlibs -nostdinc \
						-std=c2x -ffreestanding -O2 #-Werror
CXXFLAGS			=	-Wall -Wextra -Wfatal-errors -Wimplicit-function-declaration -Wincompatible-pointer-types \
						-fno-builtin -fno-exceptions -fno-stack-protector \
						-fno-rtti -nostdlib -nodefaultlibs -nostdinc \
						-std=c++17 -ffreestanding -O2 #-Werror
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

DEPENDS				=	$(KOBJS:.o=.d)
DEPENDSXX			=	$(KOBJSXX:.o=.d)
DEPENDS_USER		=	$(UOBJS:.o=.d)
WDEPENDS			=	$(WOBJS:.o=.d)
DEPENDS_ASM			=	$(KOBJS_ASM:.o=.d)

#*******************************************************************************
#*                                    KSRCS                                    *
#*******************************************************************************

%.o: %.c
	@if echo $< | grep -q "workflow"; then \
		COLOR="$(_LRED)"; \
	elif echo $< | grep -q "multitasking"; then \
        COLOR="$(_LGREEN)"; \
	elif echo $< | grep -q "fs"; then \
		COLOR="$(_LYELLOW)"; \
	elif echo $< | grep -q "shell"; then \
		COLOR="$(_LPURPLE)"; \
	elif echo $< | grep -q "drivers"; then \
		COLOR="$(_LCYAN)"; \
	elif echo $< | grep -q "memory"; then \
		COLOR="$(_LYELLOW)"; \
	elif echo $< | grep -q "userspace"; then \
		COLOR="$(_LWHITE)"; \
	else \
		COLOR="$(_LCYAN)"; \
	fi; \
	printf "$(_LWHITE) $(_DIM)- Compiling: $(_END)$(_DIM)--------$(_END)$$COLOR %s $(_END)$(_LGREEN)[$(_LWHITE)✓$(_LGREEN)]$(_END)\n" $< 
	@$(CC) $(LDFLAGS) $(CFLAGS) $(INLCUDES_PATH) -MD -c $< -o ${<:.c=.o}

%.o: %.s
	@printf "$(_LWHITE) $(_DIM)- Compiling: $(_END)$(_DIM)--------$(_END)$(_LPURPLE) %s $(_END)$(_LGREEN)[$(_LWHITE)✓$(_LGREEN)]$(_END)\n" $< 
	@$(ASM) $(ASMFLAGS) $< -o ${<:.s=.o}

%.o: %.cpp
	@printf "$(_LWHITE) $(_DIM)- Compiling: $(_END)$(_DIM)--------$(_END)$(_LGREEN) %s $(_END)$(_LGREEN)[$(_LWHITE)✓$(_LGREEN)]$(_END)\n" $< 
	@$(CXX) $(LDFLAGS) $(CXXFLAGS) $(INLCUDES_PATH) -MD -c $< -o ${<:.cpp=.o}

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

$(KDSRCS): $(KOBJS) $(KOBJSXX) $(KOBJS_ASM) $(WOBJS) $(UOBJS)
	@printf "$(_LWHITE)- KERNEL SRCS $(_END)$(_DIM)-----------$(_END) $(_LGREEN)[$(_LWHITE)✓$(_LGREEN)]$(_END)\n"

$(XORRISO):
	@sh $(SCRIPTS_DIR)/installXorriso.sh

$(CCACHE):
	@sh $(SCRIPTS_DIR)/installCcache.sh

check:
	@grub-file --is-x86-multiboot $(BIN_DIR)/$(BIN) && printf "$(_LWHITE)- $(BIN) $(_END)$(_DIM)------------$(_END) $(_LGREEN)[$(_LWHITE)✓$(_LGREEN)]$(_END)" || printf "$(_LWHITE)- $(BIN) $(_END)$(_DIM)------------$(_END) $(_LRED)[$(_LWHITE)✗$(_LRED)]$(_END)"
	printf "$(_END)$(_DIM) -> ISO CHECKER $(_END)\n"

clean-ccache:
	@printf "$(_LWHITE)- CLEAN CCACHE $(_END)$(_DIM)----------$(_END) $(_LGREEN)[$(_LWHITE)✓$(_LGREEN)]$(_END)\n"
	@$(CCACHE) -c -C > /dev/null 2>&1

clean:
	@make -s -C $(LIBKFS_DIR) clean
	@rm -rf $(NAME).iso $(KBOOT_OBJS) isodir $(BIN_DIR)/$(BIN) $(KOBJS) $(KOBJSXX) $(KOBJS_ASM) $(WOBJS) $(UOBJS) $(BIN) $(DEPENDS) $(WDEPENDS) $(DEPENDS_ASM) $(DEPENDSXX) $(DEPENDS_USER) 
	@printf "$(_LWHITE)- CLEAN $(_END)$(_DIM)-----------------$(_END) $(_LGREEN)[$(_LWHITE)✓$(_LGREEN)]$(_END)\n"

fclean: clean docker-clear clean-ccache
	@make -s -C $(LIBKFS_DIR) fclean
	@rm -rf $(DEPENDENCIES_DIR)/$(XORRISO) $(BIN_DIR) $(DEPENDENCIES_DIR)/$(CCACHE_DIR) 
	@printf "$(_LWHITE)- FCLEAN $(_END)$(_DIM)----------------$(_END) $(_LGREEN)[$(_LWHITE)✓$(_LGREEN)]$(_END)\n"

fre: clean
	@rm -rf $(BIN_DIR)
	@make -s -C $(LIBKFS_DIR) re #> /dev/null 2>&1
	@make -s -C . all

re: fclean
	@rm -rf $(BIN_DIR)
	@make -s -C $(LIBKFS_DIR) re #> /dev/null 2>&1
	@make -s -C . all

ascii:
	@printf "$(_LRED)\r██╗  ██╗███████╗███████╗$(_LWHITE)      $(_LRED) ██████╗ \n$(_END)"
	@printf "$(_LRED)\r██║ ██╔╝██╔════╝██╔════╝$(_LWHITE)      $(_LRED)██╔════╝ \n$(_END)"
	@printf "$(_LRED)\r█████╔╝ █████╗  ███████╗$(_LWHITE)█████╗$(_LRED)███████╗ \n$(_END)"
	@printf "$(_LRED)\r██╔═██╗ ██╔══╝  ╚════██║$(_LWHITE)╚════╝$(_LRED)██╔═══██╗\n$(_END)"
	@printf "$(_LRED)\r██║  ██╗██║     ███████║$(_LWHITE)      $(_LRED)╚██████╔╝\n$(_END)"
	@printf "$(_LRED)\r╚═╝  ╚═╝╚═╝     ╚══════╝$(_LWHITE)      $(_LRED) ╚═════╝ \n$(_END)"

helper:
	@printf "\n$(_LWHITE)- Now you use: '$(_LYELLOW)make run$(_END)$(_LWHITE)' or '$(_LYELLOW)make run-iso$(_END)$(_LWHITE)' to start the kernel !$(_END)\n"

clean-vfs:
	# @printf "$(_LWHITE)    $(_DIM)- Cleaning: $(_END)$(_DIM)---------$(_END)$(_LYELLOW) %s $(_END)$(_LGREEN)[$(_LWHITE)✓$(_LGREEN)]$(_END)\n" "VFS"
	@sh $(VFS_CLEAN_SCRIPT)

.PHONY: clean-vfs

include $(MK_INCLUDE_DIR)/kernel-starter/QEMU-Runner.mk
include $(MK_INCLUDE_DIR)/docker/Docker.mk
include $(MK_INCLUDE_DIR)/kernel-maker/Kernel-Maker.mk
include $(MK_INCLUDE_DIR)/dependencies/Dependencies.mk

-include $(DEPENDS)
-include $(WDEPENDS)
-include $(DEPENDS_ASM)
-include $(DEPENDSXX)

.PHONY: all clean fclean fre re debug ascii helper check clean-ccache