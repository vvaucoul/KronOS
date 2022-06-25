# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/06/21 21:08:53 by vvaucoul          #+#    #+#              #
#    Updated: 2022/06/25 12:26:49 by vvaucoul         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = libkfs.a
CC = gcc
ASM = nasm
CFLAGS = -Wall -Wextra \
				-fno-builtin -fno-exceptions -fno-stack-protector \
				-nostdlib -nodefaultlibs \
				-std=gnu99 -ffreestanding -O2 $(INCLUDE)
LDFLAGS		= 	-g3 -m32
ASM_FLAGS	=	-f elf32
INCLUDE = -I./include
MK_INCLUDE_DIR	=	mk-files

SRCS =	$(shell find srcs -name '*.c') \
		$(shell find libs -name '*.c')
OBJS = $(SRCS:.c=.o)

SRCS_ASM	=	$(shell find srcs -name '*.s')
OBJS_ASM	=	$(SRCS_ASM:.s=.o)

#*******************************************************************************
#*                                  INLCUDES                                   *
#*******************************************************************************

include ../$(MK_INCLUDE_DIR)/Colors.mk

%.o: %.c
	@printf "$(_LWHITE)    $(_DIM)- Compiling: $(_END)$(_DIM)--------$(_END)$(_LCYAN) %s $(_END)$(_LGREEN)[$(_LWHITE)✓$(_LGREEN)]$(_END)\n" $< 
	@$(CC) $(LDFLAGS) $(CFLAGS) -c $< -o ${<:.c=.o}

%.o: %.s
	@printf "$(_LWHITE)    $(_DIM)- Compiling: $(_END)$(_DIM)--------$(_END)$(_LPURPLE) %s $(_END)$(_LGREEN)[$(_LWHITE)✓$(_LGREEN)]$(_END)\n" $<
	@$(ASM) $(ASM_FLAGS) $< -o ${<:.s=.o}

all: $(NAME)

$(NAME): $(OBJS) $(OBJS_ASM)
	@ar -rcs $(NAME) $(OBJS) $(OBJS_ASM)

clean:
	@rm -f $(OBJS) $(OBJS_ASM)

fclean: clean
	@rm -f $(NAME)

re: fclean all

.PHONY: clean fclean all re