# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/11/04 15:22:55 by vvaucoul          #+#    #+#              #
#    Updated: 2022/11/04 15:26:06 by vvaucoul         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

include ../flags.mk
include ../../mk-files/Colors.mk

NAME 		=	memory

SRCS		= $(shell find srcs -name "*.c")
ASM_SRCS	= $(shell find srcs -name "*.s")

LINKER		= srcs/linker.ld
GRUB		= srcs/grub.cfg

OBJS		= $(SRCS:.c=.o)
OBJS_ASM	= $(ASM_SRCS:.s=.o)

%.o: %.c
	@$(CC) $(LDFLAGS) $(CFLAGS) -c $< -o ${<:.c=.o}

%.o: %.s
	@$(ASM) $(ASMFLAGS) $< -o ${<:.s=.o}

all: $(NAME)

$(NAME): $(OBJS) $(OBJS_ASM)
	@$(LD) $(LD_FLAGS) -T $(LINKER) -o $(NAME).bin $(OBJS) $(OBJS_ASM)
	@mkdir -p isodir/boot/grub
	@cp $(NAME).bin isodir/boot/$(NAME).bin
	@cp $(GRUB) isodir/boot/grub/grub.cfg
	@grub-mkrescue -o $(NAME).iso isodir > /dev/null 2>&1

clean:
	@rm -rf $(OBJS) $(OBJS_ASM)
	@printf "$(_LWHITE)- CLEAN $(_END)$(_DIM)-----------------$(_END) $(_LGREEN)[$(_LWHITE)✓$(_LGREEN)]$(_END)\n"

fclean: clean
	@rm -rf $(NAME).bin $(NAME).iso isodir
	@printf "$(_LWHITE)- FCLEAN $(_END)$(_DIM)----------------$(_END) $(_LGREEN)[$(_LWHITE)✓$(_LGREEN)]$(_END)\n"