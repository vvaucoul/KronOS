# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/06/21 21:08:53 by vvaucoul          #+#    #+#              #
#    Updated: 2022/08/29 21:05:45 by vvaucoul         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = libkfs.a
CC = clang
ASM = nasm
INLCUDES_PATH = -I./includes/ \
				-I./libs/
CFLAGS = -Wall -Wextra -Werror -Wfatal-errors \
				-fno-builtin -fno-exceptions -fno-stack-protector \
				-nostdlib -nodefaultlibs \
				-std=gnu99 -ffreestanding -O2
LDFLAGS		= 	-g3 -m32
ASM_FLAGS	=	-f elf32
MK_INCLUDE_DIR	=	mk-files

SRCS =	$(shell find srcs -name '*.c') \
		$(shell find libs -name '*.c')
OBJS = $(SRCS:.c=.o)

DEPENDS		=	$(OBJS:.o=.d)

SRCS_ASM	=	$(shell find srcs -name '*.s') \
				$(shell find libs -name '*.s')
OBJS_ASM	=	$(SRCS_ASM:.s=.o)

#*******************************************************************************
#*                                  INLCUDES                                   *
#*******************************************************************************

include ../$(MK_INCLUDE_DIR)/Colors.mk

%.o: %.c
	@printf "$(_LWHITE)    $(_DIM)- Compiling: $(_END)$(_DIM)--------$(_END)$(_LCYAN) %s $(_END)$(_LGREEN)[$(_LWHITE)✓$(_LGREEN)]$(_END)\n" $< 
	@$(CC) $(LDFLAGS) $(CFLAGS) $(INLCUDES_PATH) -MD -c $< -o ${<:.c=.o}

%.o: %.s
	@printf "$(_LWHITE)    $(_DIM)- Compiling: $(_END)$(_DIM)--------$(_END)$(_LPURPLE) %s $(_END)$(_LGREEN)[$(_LWHITE)✓$(_LGREEN)]$(_END)\n" $<
	@$(ASM) $(ASM_FLAGS) $< -o ${<:.s=.o}

all: $(NAME)

$(NAME): $(OBJS) $(OBJS_ASM)
	@ar -rcs $(NAME) $(OBJS) $(OBJS_ASM)

clean:
	@rm -f $(OBJS) $(OBJS_ASM) $(DEPENDS)

fclean: clean
	@rm -f $(NAME)

re: fclean all

-include $(DEPENDS)

.PHONY: clean fclean all re